
#ifndef CXXUTILS_MD5_H
#define CXXUTILS_MD5_H 1

// MD5.CC - source code for the C++/object oriented translation and 
//          modification of MD5.

// Translation and modification (c) 1995 by Mordechai T. Abzug 

// This translation/ modification is provided "as is," without express or 
// implied warranty of any kind.

// The translator/ modifier does not claim (1) that MD5 will do what you think 
// it does; (2) that this translation/ modification is accurate; or (3) that 
// this software is "merchantible."  (Language for this disclaimer partially 
// copied from the disclaimer below).

/* based on:

   MD5C.C - RSA Data Security, Inc., MD5 message-digest algorithm
   MDDRIVER.C - test driver for MD2, MD4 and MD5


   Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.

*/
#include <string>

// Constants for MD5Transform routine.
// Although we could use C++ style constants, defines are actually better,
// since they let us easily evade scope clashes.
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

class MD5 {
  public:
    MD5              (unsigned char* buffer, unsigned long len);
    MD5              ();  
    void  update     (unsigned char *input, unsigned int input_length);
    // Finalize MD5 check-sum
    void  finalize   ();


    void raw_digest (unsigned char *buff);

    std::string       hex_digest ();

  private:
    // next, the private data:
    unsigned int m_state[4];
    unsigned int m_count[2];     // number of *bits*, mod 2^64
    unsigned char m_buffer[64];   // input buffer
    unsigned char m_digest[16];
    unsigned char m_finalized;

    // last, the private methods, mostly static:
    void init             ();               // called by all constructors
    void transform        (unsigned char *buffer);  // does the real update work.  Note 
                                            // that length is implied to be 64.

    static void encode    (unsigned char *dest, unsigned int *src, unsigned int length);
    static void decode    (unsigned int *dest, unsigned char *src, unsigned int length);

    // ROTATE_LEFT rotates x left n bits.
    static inline unsigned int  rotate_left (unsigned int x, unsigned int n)
    {  return (x << n) | (x >> (32-n));                             }
    // F, G, H and I are basic MD5 functions.
    static inline unsigned int  F(unsigned int x, unsigned int y, unsigned int z)
    {  return (x & y) | (~x & z);                                   }
    static inline unsigned int  G(unsigned int x, unsigned int y, unsigned int z)
    {  return (x & z) | (y & ~z);                                   }
    static inline unsigned int  H(unsigned int x, unsigned int y, unsigned int z)
    {  return x ^ y ^ z;                                            }
    static inline unsigned int  I(unsigned int x, unsigned int y, unsigned int z)
    {  return y ^ (x | ~z);                                         }

    // FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
    // Rotation is separate from addition to prevent recomputation.
    static inline void   FF  (unsigned int& a, unsigned int b, unsigned int c, unsigned int d, unsigned int x, unsigned int s, unsigned int ac)
    {
     a += F(b, c, d) + x + ac;
     a = rotate_left(a, s) + b;
    }
    static inline void   GG  (unsigned int& a, unsigned int b, unsigned int c, unsigned int d, unsigned int x, unsigned int s, unsigned int ac)
    {
     a += G(b, c, d) + x + ac;
     a = rotate_left(a, s) + b;
    }
    static inline void   HH  (unsigned int& a, unsigned int b, unsigned int c, unsigned int d, unsigned int x, unsigned int s, unsigned int ac)
    {
     a += H(b, c, d) + x + ac;
     a = rotate_left(a, s) + b;
    }
    static inline void   II  (unsigned int& a, unsigned int b, unsigned int c, unsigned int d, unsigned int x, unsigned int s, unsigned int ac)
    {
     a += I(b, c, d) + x + ac;
     a = rotate_left(a, s) + b;
    }
};

struct MD5_digest {
  unsigned long long val[2];
  
  MD5_digest() {};    
  MD5_digest( const std::string& msg ) {
    MD5 checkSum( (unsigned char*)(msg.c_str()), msg.length());
    checkSum.raw_digest( (unsigned char*) &val );
  }
  
  bool operator< (const MD5_digest& rhs) const {
    if (val[0] == rhs.val[0]) {
      return (val[1] < rhs.val[1]);
    } else {
      return (val[0] < rhs.val[0]);
    }
  }
};

#endif
