#pragma once

#include "..\md5cpp\md5cpp.h"

using namespace System;

namespace MD5Cli {

public ref class MD5 {
private:
	md5cpp::md5* m_nativeMD5;
protected:
	!MD5();
public:
	MD5();
	MD5(array<Byte>^ text);
	explicit MD5(md5cpp::md5* ptrToAttach);
	~MD5();

	MD5^ FullCopy(); // "Clone"
	void Init();

	void Update(array<Byte>^ buf);
	MD5^ FinalizeHash();
	// System::String^ HexDigest();
};

}
