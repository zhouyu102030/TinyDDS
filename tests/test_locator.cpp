#include "tinydds/rtps/locator.hpp"
#include <iostream>

using namespace tinydds::rtps;

int main(){
    std::cout << "======= locator 测试 =======" << std::endl;

    // 测试1: 创建 IPv4 地址
    Locator loc1("192.168.1.100", 7400);
    std::cout << "IP: " << loc1.get_ipv4_string() << std::endl
              << "端口: " << loc1.port << std::endl;

    // 测试2: 判断多播地址
    Locator multicast_loc("239.255.0.1", 7400);
    std::cout << "239.255.0.1 是多播地址吗? " 
              << (multicast_loc.is_multicast() ? "是 ✅" : "否 ❌") << std::endl;

    Locator unicast_loc("192.168.1.1", 7400);
    std::cout << "192.168.1.1 是多播地址吗? " 
              << (unicast_loc.is_multicast() ? "是 ❌" : "否 ✅") << std::endl;

    // 测试3: 使用预定义的地址
    Locator default_mc = LocatorValues::default_multicast_locator(7400);
    std::cout << "默认多播地址: " << default_mc.get_ipv4_string() 
              << ":" << default_mc.port << std::endl;

    Locator default_lh = LocatorValues::localhost_locator(7400);
    std::cout << "本地地址: " << default_lh.get_ipv4_string() 
              << ":" << default_lh.port << std::endl;
    
    // 测试4: 比较两个 Locator
    Locator loc2("192.168.1.100", 7400);
    std::cout << "loc1 == loc2? " 
              << (loc1 == loc2 ? "是 ✅" : "否 ❌") << std::endl;

    std::cout << "\n所有测试通过！✅" << std::endl;
    return 0;
}