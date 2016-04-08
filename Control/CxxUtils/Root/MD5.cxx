/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CxxUtils/MD5.h"

#include <cassert>
#include <string>
#include <cstdio>
#include <iostream>
#include <string.h>

// MD5 simple initialization method
MD5::MD5()    {
  init();
  memset (m_buffer, 0, sizeof(m_buffer));
  memset (m_digest, 0, sizeof(m_digest));
}

// MD5 simple initialization method
MD5::MD5(unsigned char* buffer, unsigned long len)    {
  init();
  update(buffer, len);
  finalize();
}

// MD5 block update operation. Continues an MD5 message-digest
// operation, processing another message block, and updating the
// context.
void MD5::update (unsigned char *input, unsigned int input_length) {
  unsigned int idx, index, space;
  if ( m_finalized ) {  // so we can't update!
    std::cerr << "MD5::update:  Can't update a finalized digest!" << std::endl;
    return;
  }
  // Compute number of bytes mod 64
  index = (unsigned int)((m_count[0] >> 3) & 0x3F);
  // Update number of bits
  if (  (m_count[0] += ((unsigned int) input_length << 3))<((unsigned int) input_length << 3) )
    m_count[1]++;

  m_count[1] += ((unsigned int)input_length >> 29);
  space = 64 - index;  // how much space is left in buffer
  // Transform as many times as possible.
  if (input_length >= space) { // ie. we have enough to fill the buffer
    // fill the rest of the buffer and transform
    memcpy (m_buffer + index, input, space);
    transform (m_buffer);
    // now, transform each 64-byte piece of the input, bypassing the buffer
    for (idx = space; idx + 63 < input_length; idx += 64)
      transform (input+idx);

    index = 0;  // so we can buffer remaining
  }
  else  {
    idx = 0;    // so we can buffer the whole input
  }

  // and here we do the buffering:
  memcpy(m_buffer+index, input+idx, input_length-idx);
}

// MD5 finalization. Ends an MD5 message-digest operation, writing the
// the message digest and zeroizing the context.
void MD5::finalize ()     {
  unsigned char bits[8];
  unsigned int index, padLen;
  static unsigned char PADDING[64]={
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
  if (m_finalized){
    std::cerr << "MD5::finalize:  Already finalized this digest!" << std::endl;
    return;
  }
  // Save number of bits
  encode (bits, m_count, 8);
  // Pad out to 56 mod 64.
  index = (unsigned int) ((m_count[0] >> 3) & 0x3f);
  padLen = (index < 56) ? (56 - index) : (120 - index);
  update (PADDING, padLen);
  // Append length (before padding)
  update (bits, 8);
  // Store state in digest
  encode (m_digest, m_state, 16);
  // Zeroize sensitive information
  memset (m_buffer, 0, sizeof(*m_buffer));
  m_finalized=1;
}

void MD5::raw_digest(unsigned char *s){
  if (m_finalized){
    memcpy(s, m_digest, 16);
    return;
  }
  std::cerr << "MD5::raw_digest:  Can't get digest if you haven't "<<
    "finalized the digest!" << std::endl;
}

std::string MD5::hex_digest()   {
  char s[33];
  if (!m_finalized){
    std::cerr << "MD5::hex_digest:  Can't get digest if you haven't "<<
      "finalized the digest!" <<std::endl;
    return "";
  }
  for (int i=0; i<16; i++)
    sprintf(s+i*2, "%02x", m_digest[i]);
  s[32]='\0';
  return s;
}

// PRIVATE METHODS:
void MD5::init(){
  m_finalized=0;  // we just started!
  // Nothing counted, so count=0
  m_count[0] = 0;
  m_count[1] = 0;
  // Load magic initialization constants.
  m_state[0] = 0x67452301;
  m_state[1] = 0xefcdab89;
  m_state[2] = 0x98badcfe;
  m_state[3] = 0x10325476;
}

// MD5 basic transformation. Transforms state based on block.
void MD5::transform (unsigned char* block){
  unsigned int a = m_state[0], b = m_state[1], c = m_state[2], d = m_state[3], x[16];
  decode (x, block, 64);
  assert(!m_finalized);  // not just a user error, since the method is private
  /* Round 1                                     */
  FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1  */
  FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2  */
  FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3  */
  FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4  */
  FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5  */
  FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6  */
  FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7  */
  FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8  */
  FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9  */
  FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
  FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
  FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
  FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
  FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
  FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
  FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */
  /* Round 2                                     */
  GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
  GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
  GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
  GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
  GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
  GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
  GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
  GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
  GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
  GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
  GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
  GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
  GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
  GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
  GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
  GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */
  /* Round 3                                     */
  HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
  HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
  HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
  HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
  HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
  HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
  HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
  HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
  HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
  HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
  HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
  HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
  HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
  HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
  HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
  HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */
  /* Round 4                                     */
  II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
  II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
  II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
  II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
  II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
  II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
  II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
  II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
  II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
  II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
  II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
  II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
  II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
  II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
  II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
  II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */
  m_state[0] += a;
  m_state[1] += b;
  m_state[2] += c;
  m_state[3] += d;
  // Zeroize sensitive information.
  memset ( (unsigned char *) x, 0, sizeof(x));
}

// Encodes input (unsigned int) into output (unsigned char). Assumes len is
// a multiple of 4.
void MD5::encode (unsigned char *output, unsigned int *input, unsigned int len) {
  for (unsigned int i = 0, j = 0; j < len; i++, j += 4) {
    output[j]   = (unsigned char)  (input[i] & 0xff);
    output[j+1] = (unsigned char) ((input[i] >> 8) & 0xff);
    output[j+2] = (unsigned char) ((input[i] >> 16) & 0xff);
    output[j+3] = (unsigned char) ((input[i] >> 24) & 0xff);
  }
}

// Decodes input (unsigned char) into output (unsigned int). Assumes len is
// a multiple of 4.
void MD5::decode (unsigned int *output, unsigned char *input, unsigned int len){
  for (unsigned int i = 0, j = 0; j < len; i++, j += 4)
    output[i] = ((unsigned int)input[j]) | (((unsigned int)input[j+1]) << 8) |
      (((unsigned int)input[j+2]) << 16) | (((unsigned int)input[j+3]) << 24);
}
