#include <iostream>
#include "snappy.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>  
#include "TSPArchiveMessages.pb.h"
#include <fstream>
using namespace std;
using namespace TSP;

size_t decode_entire_length(void* buf)
{
    uint32_t* a = (uint32_t*)buf;
    uint32_t value = *a;
    value = value >> 8;
    return value;
}

size_t decode_varint(const char* buf, size_t& len)
{
    uint8_t* loop_ptr = (uint8_t*)buf;
    size_t varint = 0;
    size_t offset = 0;
    int base = 1;
    while(true)
    {
        offset++;
        uint8_t calc_value = *loop_ptr;
        varint += (calc_value & 0x7f) * base;
        if (calc_value & 0x80)
        {
            base *= 128;
            loop_ptr++;
        }
        else
        {
            break;
        }
    }

    len = varint;
    return offset;
}

size_t get_chunk_len(size_t chunk)
{
    int length = 0;
    while(true)
    {
        length += 1;
        if (chunk > 127)
        {
            chunk = chunk >> 7;
        }
        else
        {
            break;
        }
    }

    return length;
}

int main(void)
{
    int fd = open("/Users/mamba.serval/Downloads/ScanTest/iwork_parse/Index/Document.iwa", O_RDONLY);
    if(fd <= 0)
    {
        return -1;
    }
    
    void* szBuffer = malloc(10*1024*1024);
    int retCount = read(fd, szBuffer, 10*1024*1024);

    size_t compress_len = decode_entire_length(szBuffer);
    std::string output;
    if(!snappy::IsValidCompressedBuffer((char*)szBuffer+4, compress_len))
    {
        free(szBuffer);
        close(fd);
        return 0;
    }

    if(!snappy::Uncompress((char*)szBuffer+4, compress_len, &output))
    {
        free(szBuffer);
        close(fd);
        return 0;
    }

    ssize_t parse_index = 0;
    while(true)
    {
        size_t proto_len = 0;
        size_t offset = decode_varint(output.c_str() + parse_index, proto_len);
        int chunk_len = get_chunk_len(proto_len);
        //std::cout << "proto_len:" << proto_len << " offset:" << offset << " chunk_len:" << chunk_len << " parse_index:" << parse_index << std::endl;


        ArchiveInfo info;
        if (info.ParseFromArray((void*)(output.c_str()+offset + parse_index), proto_len))
        {
            //std::cout << "parse succ" << std::endl;
            parse_index += offset;
            parse_index += proto_len;


            if (info.message_infos_size() > 0)
            {
               for(int i = 0; i < info.message_infos_size(); i++)
               {
                    MessageInfo msg_info = info.message_infos(i);
                    std::cout << "get msg type:" << msg_info.type() << std::endl;


                    parse_index += msg_info.length();
               }
            }

            if (parse_index >= output.size())
            {
                std::cout << "parse end. index:" << parse_index << std::endl;
                break;
            }
        }
        else
        {
            std::cout << "parse fail" << std::endl;
            break;
        }
         
    }
    

  
   
    free(szBuffer);
    close(fd);
    return 0;
}