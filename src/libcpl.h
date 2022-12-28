typedef enum {
    ERR_NORMAL = 0,

    ERR_UNDEFINED_FRAME = 99,
    ERR_PARAMETER = 10,
    ERR_DATA_LENGTH = 40,

    ERR_WARN_ADDRESS = 21,
    ERR_WARN_DATA_RANGE = 22,
    ERR_WARN_DEVICE_NOT_AVAILABLE = 23,
} cpl_error_et;

typedef enum {
    CPL_CMD_RS,
    CPL_CMD_WS,
    CPL_CMD_RD,
    CPL_CMD_WD,
    CPL_CMD_RU,
    CPL_CMD_WU,
    MAX_CPL_CMD
} cpl_cmd_et;

typedef struct {
	unsigned char dev_addr;
	unsigned char sub_addr;
	unsigned char dev_code;
	cpl_cmd_et cmd;
	unsigned short data[16];
} cpl_frame_st;

unsigned short cpl_read_seq_dec(unsigned char const * const frame_buf, unsigned short length, unsigned short * data);
unsigned short cpl_write_seq_dec(unsigned char const * const frame_buf, unsigned short length, unsigned short * data);
unsigned short cpl_read_seq_hex(unsigned char const * const frame_buf, unsigned short length, unsigned short * data);
unsigned short cpl_write_seq_hex(unsigned char const * const frame_buf, unsigned short length, unsigned short * data);
unsigned short cpl_read_random_hex(unsigned char const * const frame_buf, unsigned short length, unsigned short * data);
unsigned short cpl_write_random_hex(unsigned char const * const frame_buf, unsigned short length, unsigned short * data);


unsigned short cpl_parser(unsigned char const * const frame_buf, const unsigned short length, cpl_frame_st * frame);
