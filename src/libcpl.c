#include "libcpl.h"

#define STX 0x2
#define ETX 0x3

#define STX_LEN 1
#define ETX_LEN 1
#define DEV_ADDR_LEN 2
#define SUB_ADDR_LEN 2
#define DEV_CODE_LEN 1
#define CMD_LEN 2
#define CHKSUM_LEN 2

static unsigned long int ascii_to_hex(unsigned char const * const bytes, unsigned char length)
{
	unsigned long int hex = 0;
	
	if(length > 8) length = 8;

	for(unsigned char idx = 0; idx < length; idx++)
	{
		unsigned char value = 0;
		if(('0' <= bytes[idx]) && (bytes[idx] <= '9'))
		{
			value = bytes[idx] - '0';
		}
		else if(('A' <= bytes[idx]) && (bytes[idx] <= 'F'))
		{
			value = bytes[idx] - 'A' + 10;
		}
		hex <<= 4;
		hex |= value;
	}

	return hex;
}

static cpl_cmd_et ascii_to_cmd(unsigned char const * const bytes)
{
	unsigned int res = 0;
	if('R' == bytes[0])
	{
		res = 0;
	}
	else if('W' == bytes[0])
	{
		res = 1;
	}

	if('S' == bytes[1])
	{
	}
	else if('D' == bytes[1])
	{
		res += 2;
	}
	else if('U' == bytes[1])
	{
		res += 4;
	}
	
	return res;
}

unsigned short __attribute__((weak, alias("cpl_undefined_frame"))) cpl_read_seq_dec(unsigned char const * const frame_buf, unsigned short length, unsigned short * data);
unsigned short __attribute__((weak, alias("cpl_undefined_frame"))) cpl_write_seq_dec(unsigned char const * const frame_buf, unsigned short length, unsigned short * data);
unsigned short __attribute__((weak, alias("cpl_undefined_frame"))) cpl_read_seq_hex(unsigned char const * const frame_buf, unsigned short length, unsigned short * data);
unsigned short __attribute__((weak, alias("cpl_undefined_frame"))) cpl_write_seq_hex(unsigned char const * const frame_buf, unsigned short length, unsigned short * data);
unsigned short __attribute__((weak, alias("cpl_undefined_frame"))) cpl_read_random_hex(unsigned char const * const frame_buf, unsigned short length, unsigned short * data);
unsigned short __attribute__((weak, alias("cpl_undefined_frame"))) cpl_write_random_hex(unsigned char const * const frame_buf, unsigned short length, unsigned short * data);
static cpl_error_et cpl_command_verify(unsigned int cmd);

static unsigned short (*cpl_func[MAX_CPL_CMD])(unsigned char const * const frame_buf, unsigned short length, unsigned short * data) = 
{
    [CPL_CMD_RS] = cpl_read_seq_dec,
    [CPL_CMD_WS] = cpl_write_seq_dec,
    [CPL_CMD_RD] = cpl_read_seq_hex,
    [CPL_CMD_WD] = cpl_write_seq_hex,
    [CPL_CMD_RU] = cpl_read_random_hex,
    [CPL_CMD_WU] = cpl_write_random_hex,
};

unsigned short cpl_parser(unsigned char const * const frame_buf, const unsigned short length, cpl_frame_st * frame)
{
    cpl_error_et res = ERR_NORMAL;
    int chk_idx = 0;
    int etx_idx = -1;
    int stx_idx = -1;

    for(chk_idx = length - 1; chk_idx >= 0; chk_idx--)
    {
        if(frame_buf[chk_idx] == ETX)
        {
            etx_idx = chk_idx;
        }
        else if(frame_buf[chk_idx] == STX)
        {
            stx_idx = chk_idx;
        }

        if(stx_idx >= 0 && etx_idx >= 0)
        {
            break;
        }
    }

    if(chk_idx < 0) res = ERR_UNDEFINED_FRAME;

    if(ERR_NORMAL == res)
    {
        unsigned short curr_pos = stx_idx;

        curr_pos += STX_LEN;

        /* device address */
        frame->dev_addr = ascii_to_hex(&frame_buf[curr_pos], DEV_ADDR_LEN);
        curr_pos += DEV_ADDR_LEN;

        /* sub address */
        frame->sub_addr = ascii_to_hex(&frame_buf[curr_pos], SUB_ADDR_LEN);
        curr_pos += SUB_ADDR_LEN;

        /* device code */
        frame->dev_code = ascii_to_hex(&frame_buf[curr_pos], DEV_CODE_LEN);
        curr_pos += DEV_CODE_LEN;

        /* cpl command */
        frame->cmd = ascii_to_cmd(&frame_buf[curr_pos]);
        curr_pos += CMD_LEN;

        res = cpl_command_verify(frame->cmd);

        if(ERR_NORMAL == res)
        {
            unsigned short appl_len = etx_idx - (curr_pos + CHKSUM_LEN + ETX_LEN);
            if(appl_len < 0) res = ERR_UNDEFINED_FRAME;

            if(ERR_NORMAL == res)
            {
                if((void*)0 != cpl_func[frame->cmd])
                {
                    res = cpl_func[frame->cmd](&frame_buf[stx_idx+(STX_LEN + DEV_ADDR_LEN + SUB_ADDR_LEN + DEV_CODE_LEN)], appl_len, frame->data);
                }
            }
        }
    }

    return res;
}

static unsigned short cpl_undefined_frame(unsigned char const * const frame_buf, unsigned short length, unsigned short * data)
{
    return ERR_UNDEFINED_FRAME;
}

static cpl_error_et cpl_command_verify(unsigned int cmd)
{
	if(cmd < MAX_CPL_CMD)
		return ERR_NORMAL;
	else
		return ERR_UNDEFINED_FRAME;
}
