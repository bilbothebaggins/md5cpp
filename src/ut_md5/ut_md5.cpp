// ut_md5.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\md5cpp\md5cpp.h"
#include <cassert>

using namespace std;

void check_single_update_hash(const string& str, const uint8_t* expected) {
	using namespace md5cpp;
	md5cpp::md5 hasher;
	uint8_t digest_result[16];

	vector<uint8_t> input(str.begin(), str.end());
	hasher.update(input.data(), input.size());
	hasher.finalize(digest_result);

	CHECK(0 == memcmp(expected, digest_result, 16));
}

void check_split_update_hash(const string& str, const uint8_t* expected) {
	using namespace md5cpp;
	md5cpp::md5 hasher;
	uint8_t digest_result[16];

	vector<uint8_t> input(str.begin(), str.end());

	size_t len = input.size();
	size_t len2 = len / 2;
	size_t len1 = len - len2;
	assert(len1 + len2 == len);

	hasher.update(input.data(), len1);
	hasher.update(input.data()+len1, len2);
	hasher.finalize(digest_result);

	CHECK(0 == memcmp(expected, digest_result, 16));
}

void check_all_variants(const string& str, const uint8_t* expected) {
	check_single_update_hash(str, expected);
	check_split_update_hash(str, expected);
}

TEST_CASE("MD5 standard testsuite") {

	SECTION("md5:empty") {
		string str = "";
		const uint8_t expected[] = { 0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04,
			0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e };

		check_all_variants(str, expected);
	}

	SECTION("md5:a") {
		string str = "a";
		const uint8_t expected[] = { 0x0c, 0xc1, 0x75, 0xb9, 0xc0, 0xf1, 0xb6, 0xa8,
			                         0x31, 0xc3, 0x99, 0xe2, 0x69, 0x77, 0x26, 0x61 };

		check_all_variants(str, expected);
	}
}

