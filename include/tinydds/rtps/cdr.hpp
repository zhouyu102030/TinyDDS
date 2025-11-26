#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <iostream>
#include <cstring>

namespace tinydds{
namespace rtps{

// ============================================================
// CDR 序列化器
// 负责将各种数据类型写入字节流
// ============================================================
class CdrSerializer{
public:
    // 构造函数：预分配缓冲区大小
    CdrSerializer(size_t capacity = 1024){ // size_t 专门用来表示大小
        // reserve() vs resize()
        // 操作	         容量    大小	 元素状态	 用途
        // reserve(n)	≥ n	   不变 	未创建	 预分配内存
        // resize(n)	≥ n	   = n	   已创建	改变元素个数
        buffer_.reserve(capacity); // 预分配内存，避免频繁分配
    }

    // 获取底层缓冲区
    const std::vector<uint8_t>& buffer() const{
        return buffer_;
    }

    // ========================================
    // 写入基本类型
    // ========================================

    // CDR 标准的规则：对齐大小 = 数据类型的大小（地址字节数等于类型所占字节的倍数）
    // 填充补位是按照字节进行填充，原因如下：
    // 内存的最小单位是字节（byte），cpu是按字节访问内存

    // bool 类型序列化
    void serialize_bool(bool value){
        buffer_.push_back(value ? 1 : 0);
    }

    // uint8_t 类型序列化
    void serialize_byte(uint8_t value) {
        buffer_.push_back(value);
    }

    void serialize_uint16(uint16_t value) {
        align(2); // 先填充补位
        write_value(value); // 再写入值
    }

    void serialize_int16(int16_t value) {
        align(2);
        write_value(value);
    }

    void serialize_uint32(uint32_t value) {
        align(4);
        write_value(value);
    }

    void serialize_int32(int32_t value) {
        align(4);
        write_value(value);
    }

    void serialize_uint64(uint64_t value) {
        align(8);
        write_value(value);
    }

    void serialize_int64(int64_t value) {
        align(8);
        write_value(value);
    }

    void serialize_float(float value) {
        align(4);
        write_value(value);
    }

    void serialize_double(double value) {
        align(8);
        write_value(value);
    }

    // ========================================
    // 写入复杂类型
    // ========================================

    // CDR 标准规定字符串的存储格式；
    // 长度（4字节）+ 字符内容(字符串大小字节) + 空终止符（1字节）
    void serialize_string(const std::string& value){
        // 字符串格式：长度（4字节）+ 字符内容 + 空终止符（1字节）
        uint32_t length = static_cast<uint32_t>(value.size() + 1); // 包括空终止符

        // length 占 4 字节，存储的是字符串长度
        // 这样写是将 length 作为 uint32_t 写入缓存，占用 4 字节，存储字符串长度
        serialize_uint32(length); 
        for(char c : value){
            buffer_.push_back(static_cast<uint8_t>(c)); // 写入每个字符
        }
        buffer_.push_back(0); // 写入空终止符
    }

    // 写入字节数组
    void serialize_array(const std::vector<uint8_t>& value){
        // 数组格式：长度(4字节) + 内容
        serialize_uint32(static_cast<uint32_t>(value.size()));
        // 向buffer_中插入类型相同的value的begin到end范围内的元素
        buffer_.insert(buffer_.end(), value.begin(), value.end());
    }

private:
    // 选择 uint8_t 作为缓存类型有以下几个重要原因：核心原因：明确的字节语义
    // uint8_t 保证：正好 1 字节（8位）、无符号（0-255）、平台无关的大小
    std::vector<uint8_t> buffer_; // 底层缓冲区
    bool swap_bytes_ = false; // 是否需要字节序转换

    void align(size_t alignment){
        size_t current_pos = buffer_.size(); // buffer_当前的下一个写入位置
        size_t padding = (alignment - (current_pos % alignment)) % alignment; // 计算对其位置
        for(size_t i = 0; i < padding; ++i){
            buffer_.push_back(0); // 填充对齐字节
        }
    }

    // 模板函数：写入任意类型
    template<typename T>
    void write_value(T value){

        // 这里是强制类型转换，将 value 的地址解释为 uint8_t 数组，
        // 把任何类型的内存看作字节数组，这样好存入缓存中
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);

        // 是否需要打小端转换
        if(swap_bytes_){
            for(size_t i = 0; i < sizeof(T); ++i){
                buffer_.push_back(ptr[sizeof(T) - 1 - i]); // 将 value 的字节你许下写入缓存
            }
        }
        else{
            // 直接写入
            for(size_t i = 0; i < sizeof(T); ++i){
                buffer_.push_back(ptr[i]);
            }
        }
    }
};

// ============================================================
// CDR 反序列化器
// 负责从字节流读取数据
// ============================================================
class CdrDeserializer{
public:
    CdrDeserializer(const std::vector<uint8_t>& buffer) : buffer_(buffer) {}

    // ========================================
    // 读取基本类型
    // ========================================
    bool deserilalizer_bool(bool& value){
        if(pos_ >= buffer_.size()) return false; // 越界检查
        value = (buffer_[pos_++] != 0);
        return true;
    }

    bool deserialize_byte(uint8_t& value){
        if (pos_ >= buffer_.size()) return false;
        value = buffer_[pos_++]; // 先赋值，再向后移动位置
        return true;
    }

    bool deserialize_uint16(uint16_t& value){
        align(2); // 先对齐 跳过填充位
        return read_value(value); // 再读取值
    }

    bool deserialize_int16(int16_t& value) {
        align(2);
        return read_value(value);
    }

    bool deserialize_uint32(uint32_t& value) {
        align(4);
        return read_value(value);
    }

    bool deserialize_int32(int32_t& value) {
        align(4);
        return read_value(value);
    }

    bool deserialize_uint64(uint64_t& value) {
        align(8);
        return read_value(value);
    }

    bool deserialize_int64(int64_t& value) {
        align(8);
        return read_value(value);
    }

    // ========================================
    // 读取复杂类型
    // ========================================

    bool deserialize_string(std::string& value) {
        uint32_t length;
        if (!deserialize_uint32(length)) return false;
        
        if (length == 0) {
            value = "";
            return true;
        }

        if (pos_ + length > buffer_.size()) return false; // 越界检查

        // 读取字符串内容（减去最后的空终止符）
        value.assign(reinterpret_cast<const char*>(&buffer_[pos_]), length - 1); // 字符串赋值
        pos_ += length;
        return true;
    }

private:
    std::vector<uint8_t> buffer_;
    size_t pos_ = 0; // 当前读取位置
    bool swap_bytes_ = false; // 是否需要字节序转换

    void align(size_t alignment){
        size_t padding = (alignment - (pos_ % alignment)) % alignment;
        pos_ += padding; // 移动读取位置 跳过填充位
    }

    template<typename T>
    bool read_value(T& value){
        if((pos_ + sizeof(T)) > buffer_.size()) return false; // 越界检查
        if(swap_bytes_){
            uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);

            for(size_t i = 0; i < sizeof(T); ++i){
                ptr[i] = buffer_[pos_ + sizeof(T) - 1 - i];
            }
        }
        else{
            std::memcpy(&value, &buffer_[pos_], sizeof(T));
        }
        pos_ += sizeof(T); // 移动读取位置
        return true;
    }
};

} // namespace rtps
} // namespace tinydds