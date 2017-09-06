/**
 * @file CxxUtils/MD5.h
 * @author scott snyder <snyder@bnl.gov>, from public domain code.
 * @date Aug 2017
 * @brief solar's public-domain MD5, wrapped for C++.
 */


#ifndef CXXUTILS_MD5_H
#define CXXUTILS_MD5_H


#include <string>
#include <cstdlib>
#include <cstdint>
#include <uuid/uuid.h>


class MD5
{
public:
  MD5 (const unsigned char* buffer, unsigned long len);
  MD5 (const char* buffer, size_t len);
  MD5 (const std::string& buffer);
  void raw_digest (unsigned char* s) const; // 16-byte buffer
  void uuid_digest (uuid_t& uuid) const;
  std::string hex_digest() const;

private:
  typedef uint32_t MD5_u32plus;
  struct Context {
    MD5_u32plus lo, hi;
    MD5_u32plus a, b, c, d;
    unsigned char buffer[64];
    MD5_u32plus block[16];
  };

  Context m_ctx;
  unsigned char m_digest[16];

  void init();
  void update (const void* data, size_t size);
  void digest (unsigned char* result);

  const void* body (const void* data, size_t size);
};


#endif // not CXXUTILS_MD5_H
