#pragma once

#include <algorithm>
#include <cstdint>

template<typename T>
bool in_range(const T& val, const T& min, const T& max)
{
	return std::clamp(val, min, max) == val;
}

template<typename T, typename Container>
bool in_range(const T& val, const Container& container)
{
	return in_range(val, static_cast<T>(0), static_cast<T>(container.size()));
}

template<typename T>
bool in_range(const T& val, const size_t& size)
{
	return in_range(val, static_cast<T>(0), static_cast<T>(size));
}

constexpr std::int64_t operator "" _i64(unsigned long long v) {
	return static_cast<std::int64_t>(v);
}

constexpr std::uint64_t operator "" _u64(unsigned long long v) {
	return static_cast<std::uint64_t>(v);
}

constexpr std::int32_t operator "" _i32(unsigned long long v) {
	return static_cast<std::int32_t>(v);
}

constexpr std::uint32_t operator "" _u32(unsigned long long v) {
	return static_cast<std::uint32_t>(v);
}