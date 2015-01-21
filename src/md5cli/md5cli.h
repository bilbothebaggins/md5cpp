#pragma once

using namespace System;

namespace MD5Cli {

// pimpl class. Not declared inside MD5, because MD5 is a ref class and does not allow this:
struct MD5state; 

public ref class MD5 : public System::Security::Cryptography::ICryptoTransform {
private:
	MD5state* m_nativeMD5;
protected:
	!MD5();
public:
	MD5();
	// MD5(array<Byte>^ text);
	explicit MD5(MD5state* ptrToAttach);
	~MD5();

	MD5^ FullCopy(); // "Clone"
	void Init();

	void Update(array<Byte>^ buf);
	void Update(array<Byte>^ buf, int offset, int length);

	MD5^ FinalizeHash();
	array<Byte>^ GetDigest(); // TODO - as property !
	// System::String^ HexDigest();

	// ICryptoTransform
	property bool CanReuseTransform {
		virtual bool get() {
			return false;
		}
	}

	property bool CanTransformMultipleBlocks {
		virtual bool get() {
			return true;
		}
	}

	property int InputBlockSize {
		virtual int get() {
			return 1; // As far as I can tell, this value is utterly irrelevant for the HashAlgorithms. (System.Security.Cryptography.HashAlgorithm return simply 1)
		}
	}

	property int OutputBlockSize {
		virtual int get() {
			return 1; // As far as I can tell, this value is utterly irrelevant for the HashAlgorithms. (System.Security.Cryptography.HashAlgorithm return simply 1)
		}
	}

	virtual int TransformBlock(array<Byte>^ inputBuffer, int inputOffset, int inputCount, array<Byte>^ outputBuffer, int outputOffset);

	virtual array<Byte>^ TransformFinalBlock(array<Byte>^ inputBuffer, int inputOffset, int inputCount);

};

}
