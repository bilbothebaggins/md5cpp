/* **************************************************************************
	This is free and unencumbered software released into the public domain.

	Anyone is free to copy, modify, publish, use, compile, sell, or
	distribute this software, either in source code form or as a compiled
	binary, for any purpose, commercial or non-commercial, and by any
	means.

	In jurisdictions that recognize copyright laws, the author or authors
	of this software dedicate any and all copyright interest in the
	software to the public domain. We make this dedication for the benefit
	of the public at large and to the detriment of our heirs and
	successors. We intend this dedication to be an overt act of
	relinquishment in perpetuity of all present and future rights to this
	software under copyright law.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
	OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
	ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.

	For more information, please refer to <http://unlicense.org/>
 ************************************************************************** */

#pragma once

#include <cstdint>
#include <memory>

#ifdef MD5CPP_EXPORTS
#define MD5CPP_API __declspec(dllexport)
#else
#define MD5CPP_API __declspec(dllimport)
#endif

namespace md5cpp {

class MD5CPP_API md5 {
private:
	uint32_t m_buf[4];
	uint32_t m_bits[2];
	uint8_t  m_in[64]; // TODO - basis BUFFER auf 32bit

private:
	// TODO direkt auf 32bit type anwenden
	void byteReverse(uint8_t* buf, unsigned int longs) {
		do {
			uint32_t t = (uint32_t)((unsigned)buf[3] << 8 | buf[2]) << 16 | ((unsigned)buf[1] << 8 | buf[0]);
			*(uint32_t *)buf = t;
			buf += 4;
		} while (--longs);
	}
public:
	md5() {
		initialize();
	}

	/*
	* Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious
	* initialization constants.
	*/
	void initialize() {
		m_buf[0] = 0x67452301;
		m_buf[1] = 0xefcdab89;
		m_buf[2] = 0x98badcfe;
		m_buf[3] = 0x10325476;

		m_bits[0] = 0;
		m_bits[1] = 0;
	}

	/*
	* Update context to reflect the concatenation of another buffer full
	* of bytes.
	*/
	void update(const uint8_t* buf, size_t len) {
		/* Update bitcount */

		uint32_t t = m_bits[0];
		if ((m_bits[0] = t + ((uint32_t)len << 3)) < t) {
			m_bits[1]++; 	/* Carry from low to high */
		}
		m_bits[1] += len >> 29;

		t = (t >> 3) & 0x3f;	/* Bytes already in shsInfo->data */

		/* Handle any leading odd-sized chunks */

		if (t) {
			unsigned char *p = (unsigned char *)m_in + t;

			t = 64 - t;
			if (len < t) {
				memcpy(p, buf, len);
				return;
			}
			memcpy(p, buf, t);
			byteReverse(m_in, 16);
			transform();
			buf += t;
			len -= t;
		}
		/* Process data in 64-byte chunks */

		while (len >= 64) {
			memcpy(m_in, buf, 64);
			byteReverse(m_in, 16);
			transform();
			buf += 64;
			len -= 64;
		}

		/* Handle any remaining bytes of data. */

		memcpy(m_in, buf, len);
	}

	void finalize(uint8_t* out_digest) {
		/* Compute number of bytes mod 64 */
		uint32_t count = (m_bits[0] >> 3) & 0x3F;

		/* Set the first char of padding to 0x80.  This is safe since there is
		always at least one byte free */
		uint8_t* p = m_in + count;
		*p++ = 0x80;

		/* Bytes of padding needed to make 64 bytes */
		count = 64 - 1 - count;

		/* Pad out to 56 mod 64 */
		if (count < 8) {
			/* Two lots of padding:  Pad the first block to 64 bytes */
			memset(p, 0, count);
			byteReverse(m_in, 16);
			transform();

			/* Now fill the next block with 56 bytes */
			memset(m_in, 0, 56);
		} else {
			/* Pad block to 56 bytes */
			memset(p, 0, count - 8);
		}
		byteReverse(m_in, 14);

		/* Append length in bits and transform */
		((uint32_t *)m_in)[14] = m_bits[0];
		((uint32_t *)m_in)[15] = m_bits[1];

		transform();
		byteReverse((uint8_t*)m_buf, 4);
		memcpy(out_digest, m_buf, 16);
		
		// No memset() of context! Will be optimzed out by compiler anyway if we 
		// don't jump through several hoops:
		// memset(ctx, 0, sizeof(ctx));        /* In case it's sensitive */
	}
	
	// TODO inline funcktionen -

	private:
	/* The four core functions - F1 is optimized somewhat */

	/* #define F1(x, y, z) (x & y | ~x & z) */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

	/* This is the central step in the MD5 algorithm. */
#define MD5STEP(f, w, x, y, z, data, s) \
	(w += f(x, y, z) + data, w = w << s | w >> (32 - s), w += x)

	void transform(/*uint32_t buf[4], uint32_t in[16]*/) {
		uint32_t a = m_buf[0];
		uint32_t b = m_buf[1];
		uint32_t c = m_buf[2];
		uint32_t d = m_buf[3];

		uint32_t* in32 = reinterpret_cast<uint32_t*>(m_in);

		MD5STEP(F1, a, b, c, d, in32[0] + 0xd76aa478, 7);
		MD5STEP(F1, d, a, b, c, in32[1] + 0xe8c7b756, 12);
		MD5STEP(F1, c, d, a, b, in32[2] + 0x242070db, 17);
		MD5STEP(F1, b, c, d, a, in32[3] + 0xc1bdceee, 22);
		MD5STEP(F1, a, b, c, d, in32[4] + 0xf57c0faf, 7);
		MD5STEP(F1, d, a, b, c, in32[5] + 0x4787c62a, 12);
		MD5STEP(F1, c, d, a, b, in32[6] + 0xa8304613, 17);
		MD5STEP(F1, b, c, d, a, in32[7] + 0xfd469501, 22);
		MD5STEP(F1, a, b, c, d, in32[8] + 0x698098d8, 7);
		MD5STEP(F1, d, a, b, c, in32[9] + 0x8b44f7af, 12);
		MD5STEP(F1, c, d, a, b, in32[10] + 0xffff5bb1, 17);
		MD5STEP(F1, b, c, d, a, in32[11] + 0x895cd7be, 22);
		MD5STEP(F1, a, b, c, d, in32[12] + 0x6b901122, 7);
		MD5STEP(F1, d, a, b, c, in32[13] + 0xfd987193, 12);
		MD5STEP(F1, c, d, a, b, in32[14] + 0xa679438e, 17);
		MD5STEP(F1, b, c, d, a, in32[15] + 0x49b40821, 22);

		MD5STEP(F2, a, b, c, d, in32[1] + 0xf61e2562, 5);
		MD5STEP(F2, d, a, b, c, in32[6] + 0xc040b340, 9);
		MD5STEP(F2, c, d, a, b, in32[11] + 0x265e5a51, 14);
		MD5STEP(F2, b, c, d, a, in32[0] + 0xe9b6c7aa, 20);
		MD5STEP(F2, a, b, c, d, in32[5] + 0xd62f105d, 5);
		MD5STEP(F2, d, a, b, c, in32[10] + 0x02441453, 9);
		MD5STEP(F2, c, d, a, b, in32[15] + 0xd8a1e681, 14);
		MD5STEP(F2, b, c, d, a, in32[4] + 0xe7d3fbc8, 20);
		MD5STEP(F2, a, b, c, d, in32[9] + 0x21e1cde6, 5);
		MD5STEP(F2, d, a, b, c, in32[14] + 0xc33707d6, 9);
		MD5STEP(F2, c, d, a, b, in32[3] + 0xf4d50d87, 14);
		MD5STEP(F2, b, c, d, a, in32[8] + 0x455a14ed, 20);
		MD5STEP(F2, a, b, c, d, in32[13] + 0xa9e3e905, 5);
		MD5STEP(F2, d, a, b, c, in32[2] + 0xfcefa3f8, 9);
		MD5STEP(F2, c, d, a, b, in32[7] + 0x676f02d9, 14);
		MD5STEP(F2, b, c, d, a, in32[12] + 0x8d2a4c8a, 20);

		MD5STEP(F3, a, b, c, d, in32[5] + 0xfffa3942, 4);
		MD5STEP(F3, d, a, b, c, in32[8] + 0x8771f681, 11);
		MD5STEP(F3, c, d, a, b, in32[11] + 0x6d9d6122, 16);
		MD5STEP(F3, b, c, d, a, in32[14] + 0xfde5380c, 23);
		MD5STEP(F3, a, b, c, d, in32[1] + 0xa4beea44, 4);
		MD5STEP(F3, d, a, b, c, in32[4] + 0x4bdecfa9, 11);
		MD5STEP(F3, c, d, a, b, in32[7] + 0xf6bb4b60, 16);
		MD5STEP(F3, b, c, d, a, in32[10] + 0xbebfbc70, 23);
		MD5STEP(F3, a, b, c, d, in32[13] + 0x289b7ec6, 4);
		MD5STEP(F3, d, a, b, c, in32[0] + 0xeaa127fa, 11);
		MD5STEP(F3, c, d, a, b, in32[3] + 0xd4ef3085, 16);
		MD5STEP(F3, b, c, d, a, in32[6] + 0x04881d05, 23);
		MD5STEP(F3, a, b, c, d, in32[9] + 0xd9d4d039, 4);
		MD5STEP(F3, d, a, b, c, in32[12] + 0xe6db99e5, 11);
		MD5STEP(F3, c, d, a, b, in32[15] + 0x1fa27cf8, 16);
		MD5STEP(F3, b, c, d, a, in32[2] + 0xc4ac5665, 23);

		MD5STEP(F4, a, b, c, d, in32[0] + 0xf4292244, 6);
		MD5STEP(F4, d, a, b, c, in32[7] + 0x432aff97, 10);
		MD5STEP(F4, c, d, a, b, in32[14] + 0xab9423a7, 15);
		MD5STEP(F4, b, c, d, a, in32[5] + 0xfc93a039, 21);
		MD5STEP(F4, a, b, c, d, in32[12] + 0x655b59c3, 6);
		MD5STEP(F4, d, a, b, c, in32[3] + 0x8f0ccc92, 10);
		MD5STEP(F4, c, d, a, b, in32[10] + 0xffeff47d, 15);
		MD5STEP(F4, b, c, d, a, in32[1] + 0x85845dd1, 21);
		MD5STEP(F4, a, b, c, d, in32[8] + 0x6fa87e4f, 6);
		MD5STEP(F4, d, a, b, c, in32[15] + 0xfe2ce6e0, 10);
		MD5STEP(F4, c, d, a, b, in32[6] + 0xa3014314, 15);
		MD5STEP(F4, b, c, d, a, in32[13] + 0x4e0811a1, 21);
		MD5STEP(F4, a, b, c, d, in32[4] + 0xf7537e82, 6);
		MD5STEP(F4, d, a, b, c, in32[11] + 0xbd3af235, 10);
		MD5STEP(F4, c, d, a, b, in32[2] + 0x2ad7d2bb, 15);
		MD5STEP(F4, b, c, d, a, in32[9] + 0xeb86d391, 21);

		m_buf[0] += a;
		m_buf[1] += b;
		m_buf[2] += c;
		m_buf[3] += d;
	}
};

}