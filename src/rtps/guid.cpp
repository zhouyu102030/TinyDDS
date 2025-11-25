#include "tinydds/rtps/guid.hpp"
#include <random>

namespace tinydds{
namespace rtps{

// 生成随机的 GuidPrefix
// 这个函数在创建 DomainParticipaant 时被调用
// todo:学习怎样生成随机数
GuidPrefix generate_guid_prefix(){
    GuidPrefix prefix;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint8_t> dis(0, 255);
    
    for (int i = 0; i < 12; ++i) {
        prefix.value[i] = dis(gen);
    }
    
    return prefix;

}

} // namespace rtps
} // namespace tinydds
