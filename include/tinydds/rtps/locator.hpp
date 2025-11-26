#pragma once

#include <cstdint>
#include <array>
#include <string>
#include <cstring>
#include <arpa/inet.h>  // for inet_pton, inet_ntop
#include <iostream>

namespace tinydds {
namespace rtps {

// ============================================================
// Locator 类型常量
// ============================================================

enum class LocatorKind : int32_t{
    LOCATOR_KIND_INVALID = -1,
    LOCATOR_KIND_RESERVED = 0,
    LOCATOR_KIND_UDPv4 = 1,
    LOCATOR_KIND_UDPv6 = 2
};

// ============================================================
// Locator: 表示网络地址（IP + Port）
// ============================================================
struct Locator{
    LocatorKind kind; // 地址类型
    uint32_t port; // 端口号
    std::array<uint8_t, 16> address; // IP 地址（IPv4 或 IPv6）

    // ========================================
    // 构造函数
    // ========================================

    // 默认构造：无效地址
    Locator() : kind(LocatorKind::LOCATOR_KIND_INVALID), port(0){
        address.fill(0);
    }

    // todo: 鉴别IPv4和IPv6地址的构造函数待实现
    // 从 IPv4 字符串和端口构造
    // 例如：Locator("192.168.1.100", 7400)
    Locator(const std::string& ipv4_str, uint32_t port_num) 
        : kind(LocatorKind::LOCATOR_KIND_UDPv4), port(port_num){
        address.fill(0);

        // IPv4 地址放在最后 4 字节
        struct in_addr addr; // 表示一个32位的IPv4地址
        // inet_pton函数将字符串形式的IP地址转换为网络字节序的二进制形式
        if (inet_pton(AF_INET, ipv4_str.c_str(), &addr) == 1) {
            // 将 4 字节 IP 地址复制到 address[12..15]
            uint8_t* ip_bytes = reinterpret_cast<uint8_t*>(&addr.s_addr); // 用于将 uint32_t 的地址转换为字节数组

            address[12] = ip_bytes[0];
            address[13] = ip_bytes[1];
            address[14] = ip_bytes[2];
            address[15] = ip_bytes[3];
        }
    }
    // ========================================
    // 工具函数
    // ========================================

    // 获取 IPv4 地址字符串
    std::string get_ipv4_string() const {
        // 仅支持 IPv4 地址
        if (kind != LocatorKind::LOCATOR_KIND_UDPv4) {
            return "";
        }
        
        // 从 address[12..15] 提取 IPv4 地址
        struct in_addr addr;
        uint8_t* ip_bytes = reinterpret_cast<uint8_t*>(&addr.s_addr);
        ip_bytes[0] = address[12];
        ip_bytes[1] = address[13];
        ip_bytes[2] = address[14];
        ip_bytes[3] = address[15];
        
        // inet_ntop 函数将网络字节序的二进制IP地址转换为字符串形式
        char buffer[INET_ADDRSTRLEN]; // 缓冲区大小 INET_ADDRSTRLEN = 16，ipv4 的 string 类型占 16 个字节
        if (inet_ntop(AF_INET, &addr, buffer, INET_ADDRSTRLEN) != nullptr) {
            return std::string(buffer); // 强制转换为 std::string 并返回
        }
        return "";
    }

    // 获取 IPv4 地址（网络字节序）
    uint32_t get_ipv4_address() const {
        return (static_cast<uint32_t>(address[12]) << 24) |
               (static_cast<uint32_t>(address[13]) << 16) |
               (static_cast<uint32_t>(address[14]) << 8) |
               static_cast<uint32_t>(address[15]);
    }

    // 判断是否是多播地址
    // IPv4 多播地址范围：224.0.0.0 ~ 239.255.255.255（11100000.00000000.000000000.000000000 - 11100000.11111111.11111111.111111111）
    bool is_multicast() const {
        // 仅支持 IPv4 地址
        if (kind != LocatorKind::LOCATOR_KIND_UDPv4) {
            return false;
        } 
        return address[12] >= 224 && address[12] <= 239;
    }

    // 判断是否有效
    bool is_valid() const {
        return kind != LocatorKind::LOCATOR_KIND_INVALID;
    }

    // 比较运算符
    bool operator==(const Locator& other) const {
        return kind == other.kind && 
               port == other.port && 
               address == other.address;
    }

    bool operator!=(const Locator& other) const {
        return !(*this == other);
    }
};

// ============================================================
// 预定义的特殊地址
// ============================================================
namespace LocatorValues {
    // DDS 默认多播地址（用于服务发现）
    // 239.255.0.1 是 RTPS 规定的默认多播地址
    inline Locator default_multicast_locator(uint32_t port = 7400) {
        return Locator("239.255.0.1", port);
    }
    
    // 本机地址
    inline Locator localhost_locator(uint32_t port = 7400) {
        return Locator("127.0.0.1", port);
    }
}

} // namespace rtps
} // namespace tinydds