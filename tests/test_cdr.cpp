#include "tinydds/rtps/cdr.hpp"
#include <iostream>
#include <cassert>

using namespace tinydds::rtps;

int main() {
    std::cout << "=== CDR Serialization 测试 ===" << std::endl;

    // 1. 准备数据
    uint8_t  v_byte = 0xAB;
    uint16_t v_u16  = 0x1234;
    uint32_t v_u32  = 0xAABBCCDD;
    std::string v_str = "Hello DDS!";

    // 2. 序列化
    CdrSerializer serializer;
    serializer.serialize_byte(v_byte);
    serializer.serialize_uint16(v_u16); // 应该触发对齐，跳过1个字节
    serializer.serialize_uint32(v_u32);
    serializer.serialize_string(v_str);

    std::cout << "序列化完成，总字节数: " << serializer.buffer().size() << std::endl;

    // 打印缓冲区内容（十六进制）
    const auto& buf = serializer.buffer();
    for (size_t i = 0; i < buf.size(); ++i) {
        printf("%02X ", buf[i]);
        // if ((i + 1) % 16 == 0) printf("\n");
    }
    printf("\n");

    // 3. 反序列化
    CdrDeserializer deserializer(buf);
    
    uint8_t  out_byte;
    uint16_t out_u16;
    uint32_t out_u32;
    std::string out_str;

    bool ok = true;
    ok &= deserializer.deserialize_byte(out_byte);
    ok &= deserializer.deserialize_uint16(out_u16);
    ok &= deserializer.deserialize_uint32(out_u32);
    ok &= deserializer.deserialize_string(out_str);

    if (!ok) {
        std::cerr << "反序列化失败！" << std::endl;
        return 1;
    }

    // 4. 验证结果
    std::cout << "验证结果:" << std::endl;
    std::cout << "Byte: " << (int)out_byte << (out_byte == v_byte ? " ✅" : " ❌") << std::endl;
    std::cout << "U16:  " << std::hex << out_u16 << (out_u16 == v_u16 ? " ✅" : " ❌") << std::endl;
    std::cout << "U32:  " << std::hex << out_u32 << (out_u32 == v_u32 ? " ✅" : " ❌") << std::endl;
    std::cout << "Str:  " << out_str << (out_str == v_str ? " ✅" : " ❌") << std::endl;

    return 0;
}