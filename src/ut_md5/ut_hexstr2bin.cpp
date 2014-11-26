#include "stdafx.h"
#include "..\common\hexstr2bin.h"

using namespace common;

TEST_CASE("test string conversion") {
	uint8_t buffer[1024];

	SECTION("zero") {
		size_t len = hexstr2bin("00", buffer);
		const uint8_t expected[] = { 0 };
		CHECK(0 == memcmp(expected, buffer, len));
	}

	SECTION("one") {
		size_t len = hexstr2bin("01", buffer);
		const uint8_t expected[] = { 1 };
		CHECK(0 == memcmp(expected, buffer, len));
	}

	SECTION("ten") {
		size_t len = hexstr2bin("10", buffer);
		const uint8_t expected[] = { 0x10 };
		CHECK(0 == memcmp(expected, buffer, len));
	}

	SECTION("FF") {
		size_t len = hexstr2bin("FF", buffer);
		const uint8_t expected[] = { 0xFF };
		CHECK(0 == memcmp(expected, buffer, len));
	}

	SECTION("ABCD") {
		size_t len = hexstr2bin("ABCD", buffer);
		const uint8_t expected[] = { 0xAB, 0xCD };
		CHECK(0 == memcmp(expected, buffer, len));
	}
}