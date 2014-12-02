#pragma once

using namespace System;

namespace MD5Cli {

// pimpl class. Not declared inside MD5, because MD5 is a ref class and does not allow this:
struct MD5state; 

public ref class MD5 {
private:
	MD5state* m_nativeMD5;
protected:
	!MD5();
public:
	MD5();
	MD5(array<Byte>^ text);
	explicit MD5(MD5state* ptrToAttach);
	~MD5();

	MD5^ FullCopy(); // "Clone"
	void Init();

	void Update(array<Byte>^ buf);
	MD5^ FinalizeHash();
	array<Byte>^ GetDigest(); // TODO - as property !
	// System::String^ HexDigest();
};

}
