#pragma once

#include <cstdint>
#include <stdexcept>
#include <sstream>

namespace common {

inline uint8_t hexchar2num(const char input) {
	if (input >= '0' && input <= '9') {
		return input - '0';
	} else if (input >= 'A' && input <= 'F') {
		return input - 'A' + 0x0A;
	} else if (input >= 'a' && input <= 'f') {
		return input - 'a' + 0x0A;
	} else {
		std::stringstream msg;
		msg << "The character '" << input << "' is not a valid hex char!";
		throw std::runtime_error(msg.str());
	}
}


inline size_t hexstr2bin(const char* src, uint8_t* target) {
	size_t result_len = 0;
	while (src[0] && src[1]) {
		*target = hexchar2num(src[0]) * uint8_t(16) + hexchar2num(src[1]);
		src += 2;
		++target;
		++result_len;
	}
	return result_len;
}

}