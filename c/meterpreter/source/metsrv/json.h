/**
 * @brief json utilities
 * @file json.h
 */

#ifndef _JSON_H_
#define _JSON_H_

#include <stdbool.h>

#include <json-c/json.h>
// ohandle

typedef unsigned char byte_t;
typedef struct json_object hashtable;

// #include "bufferev.h"

// #include "printf_format.h"

/*
 * JSON read and dispatch methods
 */
struct json_object *json_read_file(const char *json_file);

struct json_object *json_read_buf(const void *buf, size_t buf_len);

struct json_object *json_read_bufferev(struct bufferev *bev, struct json_tokener *tok);

#if 0
typedef void (*json_read_cb)(struct json_object *obj, void *arg);


void json_read_bufferev_cb(struct bufferev *bev, struct json_tokener *tok,
		json_read_cb cb, void *arg);

void json_read_buffer_queue_cb(struct buffer_queue *queue, struct json_tokener *tok,
		json_read_cb cb, void *arg);
#endif
/*
 * General JSON object manipulation methods
 */


#if 0
int json_add_str_fmt(struct json_object *json, const char *key, const char *format, ...)
	__attribute__((format(METTLE_PRINTF_FORMAT, 3, 4)));
#endif

#if 0
int json_add_string(struct json_object *json, const char *key, const char *val);

int json_add_int32(struct json_object *json, const char *key, int32_t val);

int json_add_int64(struct json_object *json, const char *key, int64_t val);

int json_add_double(struct json_object *json, const char *key, double val);

int json_add_bool(struct json_object *json, const char *key, bool val);

int json_add_array(struct json_object *json, const char *key, struct arraylist *val);

int json_add_binary(struct json_object *json, const char *key, unsigned char *val, int len);

int json_add_object(struct json_object *json, const char *key, struct json_object *val);
// int json_add_null(struct json_object *json, const char *key);

int json_get_string(json_object *json, const char *key, const char **dst);

int json_get_str_def(json_object *json, const char *key, const char **dst, const char *def);

int json_get_int32(json_object *json, const char *key, int32_t *dst);

int json_get_int64(json_object *json, const char *key, int64_t *dst);

int json_get_double(json_object *json, const char *key, double *dst);

int json_get_bool(json_object *json, const char *key, bool *dst);

#endif

#if 0
typedef struct _DataApi
{
	/* dict */ 
	void *(*new_dict)(void);

	/* dict add */ 
	int (*dict_add_string)( void *dict, const char *key, const char *val);
	int (*dict_add_int32)( void *dict, const char *key, int32_t val);
	int (*dict_add_int64)( void *dict, const char *key, int64_t val);
	int (*dict_add_double)( void *dict, const char *key, double val);
	int (*dict_add_bool)(void  *dict, const char *key, bool val);
	int (*dict_add_binary)( void *dict, const char *key, unsigned char *val, int len);
	// add jobject
	int (*dict_add)( void *dict, const char *key, void *val);

	/* dict get*/
	int (*dict_get_string)(void *dict, const char *key, const char **dst);
	int (*dict_get_int32)(void *dict, const char *key, int32_t *dst);
	int (*dict_get_int64)(void *dict, const char *key, int64_t *dst);
	int (*dict_get_double)(void *dict, const char *key, double *dst);
	int (*dict_get_bool)(void *dict, const char *key, bool *dst);
	unsigned char *(*dict_get_binary)( void *dict, const char *key, int *len);
	// not exposed
	//struct hashtable *(*get_dict)( void *dict, const char *key);
	//struct arraylist *(*get_list)( void *dict, const char *key);

	// return as jobject
	void* (*dict_get)(void *dict, const char *key);

	/* list */
	void *(*new_list)(void);
	int (*list_length)(void *list);
/*
	int (*add)(json_object *json, const char *key, struct json_object *val);
	json_object *(*new_string)(const char *key, const char *val);
	json_object *(*new_int32)(const char *key, int32_t val);
	json_object *(*new_int64)(const char *key, int64_t val);
	json_object *(*new_double)(const char *key, double val);
	json_object *(*new_bool)(const char *key, bool val);
	json_object *(*new_binary)(const char *key, unsigned char *val, int len);
*/
	/* list add */

	int (*list_add_string)(void *list, const char *val);
	int (*list_add_int32)(void *list, int32_t val);
	int (*list_add_int64)(void *list,  int64_t val);
	int (*list_add_double)(void *list, double val);
	int (*list_add_bool)(void *list, bool val);
	int (*list_add_binary)(void *list, unsigned char *val, int len);
	// used to add dict and array
	int (*list_add)(void *list, void *val);

	/* list get */
	int (*list_get_string)(void *list, int idx, const char **dst);
	int (*list_get_int32)(void *list, int idx, int32_t *dst);
	int (*list_get_int64)(void *list, int idx, int64_t *dst);
	int (*list_get_double)(void *list, int idx, double *dst);
	int (*list_get_bool)(void *list, int idx, bool *dst);
	unsigned char *(*list_get_binary)(void *list, int idx, int *len);
	void *(*list_get)(void *list, int idx);
	//struct hashtable *(*list_get_dict)(void *list, int idx);
	//struct arraylist *(*list_get_list)(void *list, int idx);
	const char* (*to_string)(void *dict);
} DataApi;

#endif
/*
 * JSON RPC methods
 */

#if 0

struct json_rpc;

#define JSON_RPC_PARSE_ERROR -32700
#define JSON_RPC_INVALID_REQUEST -32600
#define JSON_RPC_METHOD_NOT_FOUND -32601
#define JSON_RPC_INVALID_PARAMS -32603
#define JSON_RPC_INTERNAL_ERROR -32693

struct json_method_ctx
{
	const char *method;
	json_object *params;
	json_object *id;
};

typedef json_object *(*json_method_cb)(struct json_method_ctx *ctx, void *arg);

#define JSON_RESULT_IS_ERROR (1 << 0)
struct json_result_info
{
	int flags;
	uint64_t id;
	struct json_object *response;
};
typedef void (*json_result_cb)(struct json_result_info *result, void *arg);

#define JSON_RPC_CHECK_VERSION (1 << 0)
struct json_rpc *json_rpc_new(int flags);

void json_rpc_free(struct json_rpc *jrpc);

struct json_object *json_rpc_process(struct json_rpc *jrpc,
									 struct json_object *json);

int json_rpc_register_method(struct json_rpc *jrpc,
							 const char *method_name, const char *params, json_method_cb, void *arg);

int json_rpc_register_result_cb(struct json_rpc *jrpc,
								int64_t id, json_result_cb cb, void *arg);

struct json_object *json_rpc_gen_notification(struct json_rpc *jrpc,
											  const char *method_name, struct json_object *params);

struct json_object *json_rpc_gen_result_json(struct json_rpc *jrpc,
											 struct json_object *id, json_object *result);

struct json_object *json_rpc_gen_result_str(struct json_rpc *jrpc,
											struct json_object *id, const char *result);

struct json_object *json_rpc_gen_result_int32(struct json_rpc *jrpc,
											  struct json_object *id, int32_t result);

struct json_object *json_rpc_gen_result_int64(struct json_rpc *jrpc,
											  struct json_object *id, int64_t result);

struct json_object *json_rpc_gen_error(struct json_rpc *jrpc,
									   struct json_object *id, int code, const char *message);

struct json_object *json_rpc_gen_method_call(struct json_rpc *jrpc,
											 const char *method_name, int64_t *id, struct json_object *params);

struct json_object *json_rpc_gen_notification(struct json_rpc *jrpc,
											  const char *method_name, struct json_object *params);

struct json_object *json_rpc_gen_result_json(struct json_rpc *jrpc,
											 struct json_object *id, json_object *result);

#endif
#endif
