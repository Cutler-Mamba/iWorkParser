#include <iostream>
#include "snappy.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>  
#include "TSPArchiveMessages.pb.h"
#include "TSWPArchives.pb.h"
#include "TPArchives.pb.h"
#include <fstream>
#include "util.hpp"
using namespace std;
using namespace TSP;

// key:identifyer value: pointer to protobuf message
std::map<uint64_t, google::protobuf::Message*> valueMap;

google::protobuf::Message* getPropTypeMessage(int type)
{
    switch(type)
    {
        case 2051:
        {
            return new TSWP::TOCSettingsArchive();
            break;
        }
        case 10000:
        {
            return new TP::DocumentArchive();
            break;
        }
        case 2001:
        case 2005:
        {
            return new TSWP::StorageArchive();
        }
        default:
        {
            break;
        }
    }
    return nullptr;
}

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
                    size_t tmp_index = parse_index;
                    parse_index += msg_info.length();
                    //std::cout << "get msg type:" << msg_info.type() << std::endl;

                    google::protobuf::Message* bMessage = getPropTypeMessage(msg_info.type());
                    if (nullptr == bMessage)
                    {
                        continue;
                    }

                    if (msg_info.type() == 2001 || msg_info.type() == 2005)
                    {
                        TSWP::StorageArchive* q = (TSWP::StorageArchive*)bMessage;
                        //std::cout << "storage kind:" << q->kind() << std::endl;
                    }
                    

                    bMessage->ParseFromArray((void*)(output.c_str() + tmp_index), msg_info.length());

                    valueMap[info.identifier()] = bMessage;

                    //std::cout << "add id:" << info.identifier() << " value type:" << bMessage->GetTypeName() << std::endl;

                    //std::cout << "Content:" << bMessage->DebugString() << std::endl;                    
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

    google::protobuf::Message* bMessage = valueMap[1];
    if (nullptr == bMessage)
    {
        free(szBuffer);
        close(fd);
        return 0;
    }

    TP::DocumentArchive* newMessage = (TP::DocumentArchive*) bMessage;
    std::cout << "see id:" << newMessage->body_storage().identifier() << std::endl;

    TSWP::StorageArchive* textMessage = (TSWP::StorageArchive*)valueMap[newMessage->body_storage().identifier()];
    if (nullptr == textMessage)
    {
        free(szBuffer);
        close(fd);
        return 0;
    }

    
    if (textMessage->text_size() == 0)
    {
        free(szBuffer);
        close(fd);
        return 0;
    }

    std::string text = textMessage->text(0);

    std::wstring wtext = C::UTF82UniStr(text);
    std::cout << "get it. len:" << wtext.size() << std::endl;
    std::string nnn = C::Uni2UTF8Str(wtext);
    std::cout << nnn << std::endl;

    for(int i = 0; i < textMessage->table_para_style().entries_size(); i++)
    {
        uint32_t pos = textMessage->table_para_style().entries(i).character_index();
        uint32_t end = wtext.size();

        if(i < textMessage->table_para_style().entries_size() -1)
        {
            end = textMessage->table_para_style().entries(i+1).character_index();
        }
        std::cout << "pos:" << pos << " end:" << end << std::endl;
        std::wstring q = wtext.substr(pos, end - pos);
        
        std::cout << C::Uni2UTF8Str(q) << std::endl;

        for (int j = 0; j < textMessage->table_char_style().entries_size(); j++)
        {
            std::cout << "y" << std::endl;
            uint32_t cs = textMessage->table_char_style().entries(j).character_index();
            if (cs < pos) 
            {
				continue;
			}
			if (cs >= end) 
            {
				break;
		    }

            uint32_t ce = text.size();
            if(j < textMessage->table_char_style().entries_size() -1)
            {
                ce = textMessage->table_char_style().entries(j + 1).character_index();
            }

            if (ce > end)
            {
                ce = end;
            }  
        }
        
    }
    

    
    

  
   
    free(szBuffer);
    close(fd);
    return 0;
}