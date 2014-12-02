#include "stdafx.h"
#include "md5cli.h"
#include "..\md5cpp\md5cpp.h"

namespace MD5Cli {

struct MD5state {
	// Contained by-value : copy of MD5state copies h, etc.
	md5cpp::md5 h;
};

MD5::MD5() : m_nativeMD5(new MD5state) {
}

MD5::MD5(array<Byte>^ text) : m_nativeMD5(new MD5state) {
}

MD5::MD5(MD5state* ptrToAttach) : m_nativeMD5(ptrToAttach) {
}

//! dtor / Dispose()
MD5::~MD5() {
	this->!MD5();
	// Note: C++/CLI automatically calls GC.SuppressFinalize(this)
}

// Finalizer
MD5::!MD5() {
	delete m_nativeMD5;
	m_nativeMD5 = nullptr;
}

MD5^ MD5::FullCopy() {
	return gcnew MD5(new MD5state(*m_nativeMD5));
}

void MD5::Init() {
	m_nativeMD5->h.initialize();
}

void MD5::Update(array<Byte>^ buf) {
	if (buf->Length > 0) {
		pin_ptr<Byte> p = &buf[0];
		m_nativeMD5->h.update(p, buf->Length);
	}
	// else: noop
}

MD5^ MD5::FinalizeHash() {
	m_nativeMD5->h.finalize();
	return this;
}

array<Byte>^ MD5::GetDigest() {
	uint8_t digest_result[16];
	m_nativeMD5->h.get_hash(digest_result);

	array<Byte>^ data = gcnew array<Byte>(16);
	IntPtr ptr((void*)digest_result);
	System::Runtime::InteropServices::Marshal::Copy(ptr, data, 0, 16);
	return data;
}

//System::String^ MD5::HexDigest() {
//	// can use this. No enconding problems with the base64 value
//	return gcnew System::String(m_nativeMD5->hexdigest().c_str());
//}

}
