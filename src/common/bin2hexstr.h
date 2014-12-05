#pragma once

#include <cstdint>
#include <stdexcept>
#include <sstream>
#include <cassert>

namespace common {

inline std::pair<char, char> bin2hex(const uint8_t input) {
	char buf[3];
	const int check = sprintf_s(buf, "%02x", input);
	assert(check == 2);
	return std::make_pair(buf[0], buf[1]);
}


inline std::string bin2hexstr(const uint8_t* src, size_t len) {
	std::string result(2 * len, '\0');
	for (size_t i = 0; i != len; ++i) {
		auto chars = bin2hex(src[i]);
		const size_t res_idx = 2 * i;
		result[res_idx] = chars.first;
		result[res_idx + 1] = chars.second;
	}
	return result;
}

inline std::string bin2hexstr(const std::string& src) {
	const uint8_t* ptr = reinterpret_cast<const uint8_t*>(src.data());
	return bin2hexstr(ptr, src.size());
}

}