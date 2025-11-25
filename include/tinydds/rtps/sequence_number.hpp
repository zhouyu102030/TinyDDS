#pragma once
#include <cstdint>

namespace tinydds {
namespace rtps {

// ============================================================
// SequenceNumber: 用于标识消息的顺序
// 64位整数 = high(32位) + low(32位)
// ============================================================
struct SequenceNumber{
    int32_t high; // 高32位
    uint32_t low; // 低32位

    // ========================================
    // 构造函数
    // ========================================

    // 默认构造
    SequenceNumber() : high(0), low(0) {}

    // 从一个64位整数构造
    SequenceNumber(int64_t value){
        high = static_cast<int32_t>(value >> 32); // 64位类型赋值给32位类型需要类型转换
        low = static_cast<uint32_t>(value & 0xFFFFFFFF); // 16进制中F占4位，32个F占32位
    }

    // 从 high 和 low 构造
    SequenceNumber(int32_t high, uint32_t low) : high(high), low(low) {}

    // ========================================
    // 转换为64位整数
    // ========================================
    int64_t to_int64() const{
        return (static_cast<int64_t>(high) << 32) | static_cast<int64_t>(low); // |：按位或操作，将高位和低位合并
    }

    // ========================================
    // 比较运算符
    // ========================================
    bool operator==(const SequenceNumber& other) const{
        return (high == other.high) && (low == other.low);
    }

    bool operator!=(const SequenceNumber& other) const{
        return !(*this == other);
    }

    bool operator<(const SequenceNumber& other) const{
        // 先比较高位
        if(high != other.high){
            return high < other.high;
        }
        return low < other.low; 
    }

    bool operator<=(const SequenceNumber& other) const{
        return *this < other || *this == other;
    }

    bool operator>(const SequenceNumber& other) const{
        return !(*this <= other);
    }

    // ========================================
    // 算术运算
    // ========================================

    // 自增运算符：seq++
    SequenceNumber& operator++(){
        // 低位加1
        ++low;
        // 如果低位溢出，进位到高位
        if(low == 0){
            ++high;
        }
        return *this;
    }

    // 自增运算符（后置，返回旧值）：seq++
    SequenceNumber operator++(int){
        SequenceNumber old = *this;
        ++(*this);
        return old;
    }

    // 加法运算符
    SequenceNumber operator+(int64_t value) const{
        return SequenceNumber(to_int64() + value); // 这里为什么不直接操作 high 和 low？
    }
    
    // 减法运算
    SequenceNumber operator-(int64_t value) const {
        return SequenceNumber(to_int64() - value);
    }

    // 两序号相减
    int64_t operator-(const SequenceNumber& other) const {
        return to_int64() - other.to_int64();
    }
};

// ============================================================
// 预定义常量
// ============================================================
namespace SequenceNumberValues {
    const SequenceNumber SEQUENCENUMBER_UNKNOWN(-1, 0);  // 未知序列号
    const SequenceNumber SEQUENCENUMBER_ZERO(0, 0);      // 零
}

} // namespace rtps
} // namespace tinydds
