#pragma once

#include <array>
#include <cstdint>
#include <functional>

namespace tinydds{
namespace rtps{

// GuidPrefix结构体定义 用 12 个字节标识一个 DomainParticipant
struct GuidPrefix{

    // 定义一个数据存储 12 字节
    std::array<uint8_t,12> value; // uint8_t 表示无符号8位整数（0-255）

    GuidPrefix(){
        value.fill(0); // 初始化为全0
    }

    // 相等比较运算符
    bool operator==(const GuidPrefix& other) const {
        return value == other.value;
    }

    bool operator!=(const GuidPrefix& other) const {
        return !(*this == other); // 调用相等运算符
    }
};

// EntityId结构体定义 用 4 个字节标识一个实体（如 DataWriter、DataReader 等）
struct EntityId{
    // 定义一个数据存储 4 字节
    std::array<uint8_t,4> value; // uint8_t 表示无符号8位整数（0-255）

    EntityId(){
        value.fill(0); // 初始化为全0
    }

    // 带参构造：传入 4 字节的值
    EntityId(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3){
        value[0] = b0;
        value[1] = b1;
        value[2] = b2;
        value[3] = b3;
    }

    // 相等比较运算符
    bool operator==(const EntityId& other) const {
        return value == other.value;
    }

    bool operator!=(const EntityId& other) const {
        return !(*this == other); // 调用相等运算符
    }
};

// Guid结构体定义 由 GuidPrefix 和 EntityId 组成 用于唯一标识 RTPS 实体
struct GUID{
    GuidPrefix prefix;
    EntityId entityId;

    GUID() = default;

    GUID(const GuidPrefix& p, const EntityId& e) 
        : prefix(p), entityId(e) {}

    bool operator==(const GUID& other) const {
        return (prefix == other.prefix) && (entityId == other.entityId);
    }

    bool operator!=(const GUID& other) const {
        return !(*this == other); // 调用相等运算符
    }

    // TODO: 小于比较
    bool operator<(const GUID& other) const {
        // 先比较 GuidPrefix，再比较 EntityId
        if (prefix.value != other.prefix.value) {
            return prefix.value < other.prefix.value;
        }
        else{
            return entityId.value < other.entityId.value;
        }
    }
};

// 预定义 EntityId 常量
// 这些常量是在 RTPS 协议中预定义的，主要用于 内置发现协议（SPDP / SEDP），它们的用途如下：
// ENTITYID_UNKNOWN	                            (0x00,0x00,0x00,0x00)	未知或未初始化的 EntityId，通常用作默认值
// ENTITYID_PARTICIPANT	                        (0x00,0x00,0x01,0xC1)	表示 Participant 的内置标识符，用于 SPDP（Simple Participant Discovery Protocol）
// ENTITYID_SEDP_BUILTIN_TOPIC_WRITER	        (0x00,0x00,0x02,0xC2)	SEDP 内置 Topic Writer（写者），用于发布 Topic 信息
// ENTITYID_SEDP_BUILTIN_TOPIC_READER	        (0x00,0x00,0x02,0xC7)	SEDP 内置 Topic Reader（读者），用于接收 Topic 信息
// ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER	(0x00,0x00,0x03,0xC2)	SEDP 内置 Publication Writer，用于发布 DataWriter 信息
// ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER	(0x00,0x00,0x03,0xC7)	SEDP 内置 Publication Reader，用于接收 DataWriter 信息
// ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER	(0x00,0x00,0x04,0xC2)	SEDP 内置 Subscription Writer，用于发布 DataReader 信息
// ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER	(0x00,0x00,0x04,0xC7)	SEDP 内置 Subscription Reader，用于接收 DataReader 信息
// ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER	    (0x00,0x01,0x00,0xC2)	SPDP 内置 Participant Writer，用于广播 Participant 信息
// ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER	    (0x00,0x01,0x00,0xC7)	SPDP 内置 Participant Reader，用于接收 Participant 信息
namespace EntityIdValues{
    const EntityId ENTITYID_UNKNOWN(0x00, 0x00, 0x00, 0x00);
    const EntityId ENTITYID_PARTICIPANT(0x00, 0x00, 0x01, 0xC1);
    const EntityId ENTITYID_SEDP_BUILTIN_TOPIC_WRITER(0x00, 0x00, 0x02, 0xC2);
    const EntityId ENTITYID_SEDP_BUILTIN_TOPIC_READER(0x00, 0x00, 0x02, 0xC7);
    const EntityId ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER(0x00, 0x00, 0x03, 0xC2);
    const EntityId ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER(0x00, 0x00, 0x03, 0xC7);
    const EntityId ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER(0x00, 0x00, 0x04, 0xC2);
    const EntityId ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER(0x00, 0x00, 0x04, 0xC7);
    const EntityId ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER(0x00, 0x01, 0x00, 0xC2);
    const EntityId ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER(0x00, 0x01, 0x00, 0xC7);
}

} // namespace rtps
} // namespace tinydds

// 为 GUID 提供 hash 支持（用于 unordered_map）
namespace std{

// todo:学习哈希实现
// 让 GUID 可以在哈希容器里当 key 
template<>
struct hash<tinydds::rtps::GUID> {
    size_t operator()(const tinydds::rtps::GUID& guid) const noexcept {
        // 简单的 hash 算法：对所有16个字节进行异或
        size_t h = 0;
        for (int i = 0; i < 12; ++i) {
            h ^= std::hash<uint8_t>{}(guid.prefix.value[i]) << i;
        }
        for (int i = 0; i < 4; ++i) {
            h ^= std::hash<uint8_t>{}(guid.entityId.value[i]) << (i + 12);
        }
        return h;
    }
};
}
