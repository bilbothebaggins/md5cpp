// ut_md5.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\md5cpp\md5cpp.h"
#include "..\common\hexstr2bin.h"
#include <cassert>

using namespace std;

void check_single_update_hash(const string& str, const uint8_t* expected) {
	using namespace md5cpp;
	md5cpp::md5 h1;
	md5cpp::md5 h2;
	md5cpp::md5 h3;

	uint8_t digest_result[16];

	vector<uint8_t> input(str.begin(), str.end());
	h1.update(input.data(), input.size());
	h2.update(input.begin(), input.end());
	h3.update(str);

	h1.finalize(digest_result);
	CHECK(0 == memcmp(expected, digest_result, 16));
	h1.get_hash(digest_result);
	CHECK(0 == memcmp(expected, digest_result, 16));

	h2.finalize(digest_result);
	CHECK(0 == memcmp(expected, digest_result, 16));
	h2.get_hash(digest_result);
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

	hasher.get_hash(digest_result);
	CHECK(0 == memcmp(expected, digest_result, 16));
}

void check_all_variants(const string& str, const uint8_t* expected) {
	check_single_update_hash(str, expected);
	check_split_update_hash(str, expected);
}

void check_all_variants(const string& str, const string& expected) {
	std::vector<uint8_t> exp_buffer(expected.size() / 2, 0x00);
	common::hexstr2bin(expected.c_str(), exp_buffer.data());
	check_single_update_hash(str, exp_buffer.data());
	check_split_update_hash(str, exp_buffer.data());
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

	SECTION("md5:...") {
		check_all_variants("abc", "900150983cd24fb0d6963f7d28e17f72");
		check_all_variants("message digest", "f96b697d7cb7938d525a2f31aaf161d0");
		check_all_variants("abcdefghijklmnopqrstuvwxyz", "c3fcd3d76192e4007dfb496cca67e13b");
		check_all_variants("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", "d174ab98d277d9f5a5611c2c9f419d9f");
		check_all_variants("12345678901234567890123456789012345678901234567890123456789012345678901234567890", "57edf4a22be3c955ac49da2e2107b67a");
	}
}


