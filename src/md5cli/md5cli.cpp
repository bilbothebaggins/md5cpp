#include "stdafx.h"
#include "md5cli.h"

namespace MD5Cli {

MD5::MD5() : m_nativeMD5(new md5cpp::md5) {
}

MD5::MD5(array<Byte>^ text) : m_nativeMD5(new md5cpp::md5) {
}

MD5::MD5(md5cpp::md5* ptrToAttach) : m_nativeMD5(ptrToAttach) {
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
	return gcnew MD5(new md5cpp::md5(*m_nativeMD5));
}

void MD5::Init() {
	m_nativeMD5->initialize();
}

void MD5::Update(array<Byte>^ buf) {
	if (buf->Length > 0) {
		pin_ptr<Byte> p = &buf[0];
		m_nativeMD5->update(p, buf->Length);
	}
	// else: noop
}

MD5^ MD5::FinalizeHash() {
	m_nativeMD5->finalize();
	return this;
}

//System::String^ MD5::HexDigest() {
//	// can use this. No enconding problems with the base64 value
//	return gcnew System::String(m_nativeMD5->hexdigest().c_str());
//}

}
