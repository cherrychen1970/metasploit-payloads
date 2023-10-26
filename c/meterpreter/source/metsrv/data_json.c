#include "metsrv.h"
#include "json.h"
// #include "utlist.h"

#include <errno.h>
#include <inttypes.h>

static struct json_object *json_read_file(const char *filename)
{
	struct json_object *obj = NULL;
	FILE *file = fopen(filename, "r");
	struct json_tokener *tok = json_tokener_new();

	if (!file || !tok)
	{
		goto out;
	}

	char buf[4096];
	size_t buf_len;
	enum json_tokener_error rc = json_tokener_continue;

	do
	{
		buf_len = fread(buf, 1, sizeof(buf), file);
		if (buf_len > 0)
		{
			obj = json_tokener_parse_ex(tok, buf, buf_len);
			rc = json_tokener_get_error(tok);
		}
	} while (buf_len > 0 && rc == json_tokener_continue);

	if (rc != json_tokener_success)
	{
		dprintf("JSON parse error: %s", json_tokener_error_desc(rc));
	}

out:
	if (tok)
	{
		json_tokener_free(tok);
	}
	if (file)
	{
		fclose(file);
	}
	return obj;
}

static struct json_object *json_read_buf(const void *buf, size_t buf_len)
{
	struct json_tokener *tok = json_tokener_new();
	struct json_object *obj = json_tokener_parse_ex(tok, buf, buf_len);
	if (!obj)
	{
		enum json_tokener_error rc = json_tokener_get_error(tok);
		dprintf("JSON parse error: %s", json_tokener_error_desc(rc));
	}
	json_tokener_free(tok);
	return obj;
}

static const char *json_to_string(json_object *obj)
{
	// int flags = JSON_C_TO_STRING_PLAIN;
	int flags = JSON_C_TO_STRING_PRETTY_TAB;
	return json_object_to_json_string_ext(obj, flags);
}

#if 0
struct json_object *json_read_bufferev(struct bufferev *bev, struct json_tokener *tok)
{
	char buf[4096];
	size_t buf_len;
	struct json_object *obj = NULL;
	enum json_tokener_error rc = json_tokener_continue;
	do {
		buf_len = bufferev_read(bev, buf, sizeof(buf));
		if (buf_len) {
			obj = json_tokener_parse_ex(tok, buf, buf_len);
			rc = json_tokener_get_error(tok);
		}
	} while (buf_len && rc == json_tokener_continue);

	return obj;
}

void json_read_bufferev_cb(struct bufferev *bev, struct json_tokener *tok,
		json_read_cb cb, void *arg)
{
	char buf[4096];
	size_t buf_len, last_read = 0;
	struct json_object *obj = NULL;
	enum json_tokener_error rc = json_tokener_continue;
	do {
		buf_len = bufferev_read(bev, buf, sizeof(buf));
		if (buf_len) {
			last_read = buf_len;
			obj = json_tokener_parse_ex(tok, buf, buf_len);
			rc = json_tokener_get_error(tok);
			if (obj) {
				cb(obj, arg);
			}
		}
	} while (buf_len && rc == json_tokener_continue);

	if (tok->char_offset < last_read) {
		size_t offset = tok->char_offset;
		while ((obj = json_tokener_parse_ex(tok, buf + offset, last_read - offset))) {
			offset += tok->char_offset;
			cb(obj, arg);
		}
	}
}


void json_read_buffer_queue_cb(struct buffer_queue *queue, struct json_tokener *tok,
		json_read_cb cb, void *arg)
{
	void *buf;
	size_t buf_len, last_read = 0;
	struct json_object *obj = NULL;
	enum json_tokener_error rc = json_tokener_continue;
	do {
		buf_len = buffer_queue_remove_all(queue, &buf);
		if (buf_len) {
			last_read = buf_len;
			obj = json_tokener_parse_ex(tok, buf, buf_len);
			rc = json_tokener_get_error(tok);
			if (obj) {
				cb(obj, arg);
			}
			free(buf);
		}
	} while (buf_len && rc == json_tokener_continue);

	if (tok->char_offset < last_read) {
		size_t offset = tok->char_offset;
		while ((obj = json_tokener_parse_ex(tok, (const char*)(buf + offset), last_read - offset))) {
			offset += tok->char_offset;
			cb(obj, arg);
		}
	}
}
int json_add_str_fmt(struct json_object *json, int key, const char *format, ...)
{
	char *buf = NULL;
	va_list args;
	va_start(args, format);
	if (vsprintf(&buf, format, args) == -1)
	{
		buf = NULL;
	}
	va_end(args);

	int rc = json_add_string(json, key, buf);
	free(buf);
	return rc;
}
#endif

static char *to_hex_string(int type)
{
	char key[_MAX_LTOSTR_BASE16_COUNT];
	_itoa(type, key, 16);
	return _strdup(key);
}

static void* _json_object_object_get(void *obj, int key)
{
	char *strkey = to_hex_string(key);
	struct json_object *item = json_object_object_get(obj, strkey);
	free(strkey);
	return item;
}

static int _json_object_object_add(void *obj, int key, void *item)
{
	char *strkey = to_hex_string(key);
	int res = json_object_object_add(obj, strkey, item);
	free(strkey);
	return res;
}

static void *new_dict(void)
{
	return json_object_new_object();
}

static void *new_list(void)
{
	return json_object_new_array();
}

/* dict */

static int dict_add_string(void *dict, int key, const char *val)
{
	if (val == NULL)
		return -1;

	struct json_object *obj = json_object_new_string(val);
	if (!obj)
		return -1;

	_json_object_object_add(dict, key, obj);
	return 0;
}

static int dict_add_binary(void *dict, int key, unsigned char *val, int len)
{

	if (val)
	{
		struct json_object *obj = json_object_new_binary(val, len);
		if (obj)
		{
			_json_object_object_add(dict, key, obj);
			return 0;
		}
	}
	return -1;
}

static int dict_add_int32(struct json_object *json, int key, int32_t val)
{
	struct json_object *obj = json_object_new_int(val);
	if (obj)
	{
		_json_object_object_add(json, key, obj);
		return 0;
	}
	return -1;
}

static int dict_add_int64(struct json_object *json, int key, int64_t val)
{
	struct json_object *obj = json_object_new_int64(val);
	if (obj)
	{
		_json_object_object_add(json, key, obj);
		return 0;
	}
	return -1;
}

static int dict_add_double(struct json_object *json, int key, double val)
{
	struct json_object *obj = json_object_new_double(val);
	if (obj)
	{
		_json_object_object_add(json, key, obj);
		return 0;
	}
	return -1;
}

static int dict_add_bool(struct json_object *json, int key, bool val)
{
	struct json_object *obj = json_object_new_boolean(val);
	if (obj)
	{
		_json_object_object_add(json, key, obj);
		return 0;
	}
	return -1;
}

static int dict_add(void *dict, int key, void *item)
{
	int res = _json_object_object_add(dict, key, item);
	return res;
}

static int dict_get_string(void *dict, int key, const char **dst)
{
	struct json_object *obj = _json_object_object_get(dict, key);

	if (obj)
	{
		*dst = json_object_get_string(obj);
	}
	return *dst ? 0 : -1;
}

static int dict_get_int32(void *dict, int key, int32_t *dst)
{
	struct json_object *obj = _json_object_object_get(dict, key);
			dprintf("%x",*dst);
	if (obj)
	{
		*dst = json_object_get_int(obj);
		dprintf("%x",*dst);
		if (errno != EINVAL)
		{
			return 0;
		}
	}
	return -1;
}

static int dict_get_int64(void *dict, int key, int64_t *dst)
{
	struct json_object *obj = _json_object_object_get(dict, key);

	if (obj)
	{
		*dst = json_object_get_int64(obj);
		if (errno != EINVAL)
		{
			return 0;
		}
	}
	return -1;
}

static int dict_get_double(void *dict, int key, double *dst)
{
	struct json_object *obj = _json_object_object_get(dict, key);

	if (obj)
	{
		*dst = json_object_get_double(obj);
		if (errno != EINVAL)
		{
			return 0;
		}
	}
	return -1;
}

static int dict_get_bool(void *dict, int key, bool *dst)
{
	struct json_object *obj = _json_object_object_get(dict, key);
	if (obj)
	{
		*dst = json_object_get_boolean(obj);
		if (errno != EINVAL)
		{
			return 0;
		}
	}
	return -1;
}

static int dict_get_binary(void *dict, int key, const char **dst, int *len)
{
	struct json_object *obj = _json_object_object_get(dict, key);
	if (obj)
	{
		*dst = json_object_get_binary(obj, len);
		if (errno != EINVAL)
		{
			return 0;
		}
	}
	return -1;
}

static void *dict_get(void *dict, int key)
{
	return _json_object_object_get(dict, key);
}

/* list */

static void *list_new(void)
{
	return json_object_new_array();
}

static int list_length(void *list)
{
	return json_object_array_length(list);
}

static int list_add_string(void *list, const char *val)
{
	struct json_object *obj = json_object_new_string(val);
	if (obj == NULL)
		return -1;

	return json_object_array_add(list, obj);
}

static int list_add_int32(void *list, int32_t val)
{
	struct json_object *obj = json_object_new_int(val);
	if (obj == NULL)
		return -1;

	return json_object_array_add(list, obj);
}

static int list_add_int64(void *list, int64_t val)
{
	struct json_object *obj = json_object_new_int64(val);
	if (obj == NULL)
		return -1;

	return json_object_array_add(list, obj);
}

static int list_add_double(void *list, double val)
{
	struct json_object *obj = json_object_new_double(val);
	if (obj == NULL)
		return -1;

	return json_object_array_add(list, obj);
}

static int list_add_bool(void *list, bool val)
{
	struct json_object *obj = json_object_new_boolean(val);
	if (obj == NULL)
		return -1;

	return json_object_array_add(list, obj);
}

static int list_add_binary(void *list, byte_t val, int len)
{
	struct json_object *obj = json_object_new_binary(val, len);
	if (obj == NULL)
		return -1;

	return json_object_array_add(list, obj);
}

static int list_add(void *list, void *obj)
{
	return json_object_array_add(list, obj);
}

/* get */
static int list_get_string(void *list, int idx, const char **dst)
{
	struct json_object *obj = json_object_array_get_idx(list, idx);
	if (obj == NULL || !json_object_is_type(obj, json_type_string))
		return -1;

	*dst = json_object_get_string(obj);
	return 0;
}

static int list_get_int32(void *list, int idx, int32_t *dst)
{
	struct json_object *obj = json_object_array_get_idx(list, idx);
	if (obj == NULL || !json_object_is_type(obj, json_type_int))
		return -1;

	*dst = json_object_get_int(obj);
	return 0;
}

static int list_get_int64(void *list, int idx, int64_t *dst)
{
	struct json_object *obj = json_object_array_get_idx(list, idx);
	if (obj == NULL || !json_object_is_type(obj, json_type_int))
		return -1;

	*dst = json_object_get_int64(obj);
	return 0;
}

static int list_get_double(void *list, int idx, double *dst)
{
	struct json_object *obj = json_object_array_get_idx(list, idx);
	if (obj == NULL || !json_object_is_type(obj, json_type_double))
		return -1;

	*dst = json_object_get_double(obj);
	return 0;
}

static int list_get_bool(void *list, int idx, bool *dst)
{
	struct json_object *obj = json_object_array_get_idx(list, idx);
	if (obj == NULL || !json_object_is_type(obj, json_type_boolean))
		return -1;

	*dst = json_object_get_boolean(obj);
	return 0;
}

static byte_t *list_get_binary(void *list, int idx, int *len)
{
	struct json_object *obj = json_object_array_get_idx(list, idx);
	return json_object_get_binary(obj, len);
}

static void *list_get(void *list, int idx)
{
	return json_object_array_get_idx(list, idx);
}

DataApi data_api = {
	new_dict,
	dict_add_string,
	dict_add_int32,
	dict_add_int64,
	dict_add_double,
	dict_add_bool,
	dict_add_binary,
	dict_add,

	dict_get_string,
	dict_get_int32,
	dict_get_int64,
	dict_get_double,
	dict_get_bool,
	dict_get_binary,
	dict_get,

	new_list,
	list_length,
	list_add_string,
	list_add_int32,
	list_add_int64,
	list_add_double,
	list_add_bool,
	list_add_binary,
	list_add,

	list_get_string,
	list_get_int32,
	list_get_int64,
	list_get_double,
	list_get_bool,
	list_get_binary,
	list_get,

	// to_string
	json_to_string,
	json_read_buf,
	json_to_string
	};
