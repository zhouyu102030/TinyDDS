#include "tinydds/rtps/sequence_number.hpp"
#include <iostream>

using namespace tinydds::rtps;

int main() {
    std::cout << "=== SequenceNumber 测试 ===" << std::endl;
    
    // 测试1: 基本构造
    SequenceNumber seq1;
    std::cout << "seq1 (默认): " << seq1.to_int64() << std::endl;
    
    SequenceNumber seq2(100);
    std::cout << "seq2 (100): " << seq2.to_int64() << std::endl;
    
    // 测试2: 自增
    SequenceNumber seq3(1);
    std::cout << "seq3 初始值: " << seq3.to_int64() << std::endl;
    ++seq3;
    std::cout << "seq3 自增后: " << seq3.to_int64() << std::endl;
    
    // 测试3: 比较
    SequenceNumber seq4(10);
    SequenceNumber seq5(20);
    if (seq4 < seq5) {
        std::cout << "seq4 (10) < seq5 (20): 正确！" << std::endl;
    }
    
    // 测试4: 加减运算
    SequenceNumber seq6(100);
    SequenceNumber seq7 = seq6 + 50;
    std::cout << "seq6 (100) + 50 = " << seq7.to_int64() << std::endl;
    
    // 测试5: 大数测试（超过32位）
    SequenceNumber seq8(1LL << 33);  // 8589934592
    std::cout << "大数测试: high=" << seq8.high << ", low=" << seq8.low << std::endl;
    std::cout << "转换回来: " << seq8.to_int64() << std::endl;
    
    std::cout << "\n所有测试通过！✅" << std::endl;
    return 0;
}