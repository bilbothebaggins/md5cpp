#include "stdafx.h"
#include "..\md5cpp\md5cpp.h"
#include "..\common\hexstr2bin.h"
#include "..\common\bin2hexstr.h"
#include <cassert>

using namespace std;
using namespace System;
using namespace common;

array<Byte>^ make_clrarray(const string& str) {
	array<Byte>^ managed_array = gcnew array<Byte>(static_cast<int>(str.size()));
	IntPtr ptr((void*)str.data());
	System::Runtime::InteropServices::Marshal::Copy(ptr, managed_array, 0, static_cast<int>(str.size()));
	return managed_array;
}

void check_incremental_hashes(const string& str, const uint8_t* expected) {
	INFO("Input: 0x[" << bin2hexstr(str) << "] (\"" << str << "\")");
	INFO("Expected: 0x[" << bin2hexstr(expected, 16) << "]");

	MD5Cli::MD5^ h1 = gcnew MD5Cli::MD5();

	array<Byte>^ input = make_clrarray(str);

	const int len = input->Length;
	int len2 = len / 2;
	int len1 = len - len2;
	assert(len1 + len2 == len);

	array<Byte>^ part = gcnew array<Byte>(len2);
	array<Byte>::Copy(input, 0, part, 0, len2);
	h1->Update(part);

	MD5Cli::MD5^ h2 = h1->FullCopy();

	part = gcnew array<Byte>(len1);
	array<Byte>::Copy(input, len2, part, 0, len1);
	h1->Update(part);
	h2->Update(part);

	MD5Cli::MD5^ h3 = h2->FullCopy();

	auto r1 = h1->FinalizeHash()->GetDigest();
	auto r2 = h2->FinalizeHash()->GetDigest();
	auto r3 = h3->FinalizeHash()->GetDigest();

	REQUIRE(r1->Length == 16);
	REQUIRE(r2->Length == 16);
	REQUIRE(r3->Length == 16);

	{
		pin_ptr<Byte> p = &r1[0];
		INFO("Got:      0x[" << bin2hexstr(p, 16) << "]");
		CHECK(0 == memcmp(expected, p, 16));
	}
	{
		pin_ptr<Byte> p = &r1[0];
		INFO("Got:      0x[" << bin2hexstr(p, 16) << "]");
		CHECK(0 == memcmp(expected, p, 16));
	}
	{
		pin_ptr<Byte> p = &r1[0];
		INFO("Got:      0x[" << bin2hexstr(p, 16) << "]");
		CHECK(0 == memcmp(expected, p, 16));
	}
}

// Test of the .NET ICryptoTransform
void check_single_update_hash_cli_icrypto(const string& str, const uint8_t* expected) {
	INFO("Input: 0x[" << bin2hexstr(str) << "] (\"" << str << "\")");
	INFO("Expected: 0x[" << bin2hexstr(expected, 16) << "]");
	array<Byte>^ input = make_clrarray(str);

	MD5Cli::MD5^ h1 = gcnew MD5Cli::MD5();
	
	h1->TransformBlock(input, 0, input->Length, nullptr, 0);
	array<Byte>^ result = h1->TransformFinalBlock(input, 0, 0);

	REQUIRE(result->Length > 0);

	pin_ptr<Byte> p_result = &result[0];
	INFO("Got:      0x[" << bin2hexstr(p_result, 16) << "]");
	CHECK(0 == memcmp(expected, p_result, 16));
}

// Test of the .NET class
void check_single_update_hash_cli(const string& str, const uint8_t* expected) {
	INFO("Input: 0x[" << bin2hexstr(str) << "] (\"" << str << "\")");
	INFO("Expected: 0x[" << bin2hexstr(expected, 16) << "]");
	array<Byte>^ input = make_clrarray(str);

	MD5Cli::MD5^ h1 = gcnew MD5Cli::MD5();

	h1->Update(input);
	array<Byte>^ result = h1->FinalizeHash()->GetDigest();

	REQUIRE(result->Length > 0);

	pin_ptr<Byte> p_result = &result[0];
	INFO("Got:      0x[" << bin2hexstr(p_result, 16) << "]");
	CHECK(0 == memcmp(expected, p_result, 16));
}

// Test single update call
void check_single_update_hash(const string& str, const uint8_t* expected) {
	INFO("Input: 0x[" << bin2hexstr(str) << "] (\"" << str << "\")");
	INFO("Expected: 0x[" << bin2hexstr(expected, 16) << "]");
	md5cpp::md5 h1;
	md5cpp::md5 h2;
	md5cpp::md5 h3;

	uint8_t digest_result[16];

	vector<uint8_t> input(str.begin(), str.end());
	h1.update(input.data(), static_cast<int>(input.size()));
	h2.update(input.begin(), input.end());
	h3.update(str);

	{	
		h1.finalize(digest_result);
		INFO("Got:      0x[" << bin2hexstr(digest_result, 16) << "]");
		CHECK(0 == memcmp(expected, digest_result, 16));
	}

	{	
		h1.get_hash(digest_result);
		INFO("Got:      0x[" << bin2hexstr(digest_result, 16) << "]");
		CHECK(0 == memcmp(expected, digest_result, 16));
	}
	
	{	
		h2.finalize(digest_result);
		INFO("Got:      0x[" << bin2hexstr(digest_result, 16) << "]");
		CHECK(0 == memcmp(expected, digest_result, 16));
	}

	{	
		h2.get_hash(digest_result);
		INFO("Got:      0x[" << bin2hexstr(digest_result, 16) << "]");
		CHECK(0 == memcmp(expected, digest_result, 16));
	}
}

// test two update calls with same overall data as single call above
void check_split_update_hash(const string& str, const uint8_t* expected) {
	INFO("Input: 0x[" << bin2hexstr(str) << "] (\"" << str << "\")");
	INFO("Expected: 0x[" << bin2hexstr(expected, 16) << "]");

	md5cpp::md5 hasher;
	uint8_t digest_result[16];

	vector<uint8_t> input(str.begin(), str.end());

	int32_t len = static_cast<int32_t>(input.size());
	int32_t len2 = len / 2;
	int32_t len1 = len - len2;
	assert(len1 + len2 == len);

	hasher.update(input.data(), len1);
	hasher.update(input.data()+len1, len2);

	{
		hasher.finalize(digest_result);
		INFO("Got:      0x[" << bin2hexstr(digest_result, 16) << "]");
		CHECK(0 == memcmp(expected, digest_result, 16));
	}

	{
		hasher.get_hash(digest_result);
		INFO("Got:      0x[" << bin2hexstr(digest_result, 16) << "]");
		CHECK(0 == memcmp(expected, digest_result, 16));
	}
}

void check_all_variants(const string& str, const uint8_t* expected) {
	check_single_update_hash(str, expected);
	check_split_update_hash(str, expected);
	check_single_update_hash_cli(str, expected);
	check_single_update_hash_cli_icrypto(str, expected);
	check_incremental_hashes(str, expected);
}

void check_all_variants(const string& str, const string& expected) {
	std::vector<uint8_t> exp_buffer(expected.size() / 2, 0x00);
	common::hexstr2bin(expected.c_str(), exp_buffer.data());
	
	check_all_variants(str, exp_buffer.data());
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

	SECTION("md5:binary") {
		const char buffer[] = "\x00\x01\x02";
		string str(buffer, _countof(buffer));

		check_all_variants(str, "0f0c725e025036e905dc2ed035406463");
	}
}


