```c++
// common_core.h

typedef struct
{
	BYTE xor_key[4];
	BYTE session_guid[sizeof(GUID)];
	DWORD enc_flags;
	DWORD length;
	DWORD type; //PacketTlvType
} PacketHeader;

typedef enum
{
	PACKET_TLV_TYPE_REQUEST        = 0,   ///< Indicates a request packet.
	PACKET_TLV_TYPE_RESPONSE       = 1,   ///< Indicates a response packet.
	PACKET_TLV_TYPE_PLAIN_REQUEST  = 10,  ///< Indicates a plain request packet.
	PACKET_TLV_TYPE_PLAIN_RESPONSE = 11,  ///< Indicates a plain response packet.
} PacketTlvType;

typedef struct
{
	DWORD length;
	DWORD type;
} TlvHeader;

// type  : upper 2 bytes data type

typedef struct
{
	TlvHeader header;
	PUCHAR    buffer;
} Tlv;


/*! @brief Packet definition. */
typedef struct _Packet
{
	PacketHeader header;

	PUCHAR    payload;
	ULONG     payloadLength;

	LIST *    decompressed_buffers;

	///! @brief Flag indicating if this packet is a local (ie. non-transmittable) packet.
	BOOL local;
	///! @brief Pointer to the associated packet (response/request)
	struct _Packet* partner;
} Packet;


// base.c
BOOL command_handle(Remote *remote, Packet *packet)

// transport?.c
static DWORD packet_receive(Remote *remote, Packet **packet)
```

packet

packetheader + tlvheader + payload