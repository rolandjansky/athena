/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <cassert>
#include "SGTools/SGVersionedKey.h"
using namespace SG;
using namespace std;

VersionedKey::~VersionedKey() {
  //  if (m_versionKey) std::cout << "about to free " << 
  //  m_versionKey << "@" << (void*)m_versionKey << 
  // "@this " << this << std::endl;
  free(m_versionKey); 
}

bool
VersionedKey::isVersionedKey(const char * vkey) { 
  return (vkey[0]==separator() && vkey[3]==separator());
}

bool 
VersionedKey::isVersionedKey(const std::string& vkey) {
  return isVersionedKey(vkey.c_str());
}

VersionedKey::VersionedKey(const char* key, unsigned char version) {
  encode(key, version);
}
VersionedKey::VersionedKey(const std::string& key, unsigned char version) {
  encode(key.c_str(), version);
}
VersionedKey::VersionedKey(const std::string& versionedKey) {
  copyVK(versionedKey.c_str());
}
VersionedKey::VersionedKey(const char* versionedKey) {
  copyVK(versionedKey);
}
VersionedKey::VersionedKey(const VersionedKey& versionedKey) :
  m_versionKey(versionedKey.m_versionKey) {
  versionedKey.m_versionKey=0; //a la auto-ptr
}
void VersionedKey::decode(char*& key, unsigned char& version) const {
  key = m_versionKey+4;
  version = (unsigned char)strtol(m_versionKey+1, NULL, 10);
  assert(version <= 99);
  // possibly faster, disgusting, version decoding
  // equivalent to version = atoi(m_versionKey[2:3]);
  //   char *cptr(m_versionKey+1);
  //   if (*cptr>='0' && *cptr<='9') {
  //     version = 10 * (*cptr - '0');
  //     ++cptr;
  //     if (*cptr>='0' && *cptr<='9') version += *cptr - '0';
  //     else version = 0;
  //   } else version = 0;
}

void VersionedKey::encode(const char* key, unsigned char version) {
  assert(version <= 99);
  m_versionKey = (char *)malloc(strlen(key)+5);
  sprintf(m_versionKey, formatString(), version, key);
}

void VersionedKey::copyVK(const char* versionedKey) {
  if (isVersionedKey(versionedKey)) {
    m_versionKey = (char *)malloc(strlen(versionedKey)+1);
    strcpy(m_versionKey, versionedKey);
  }
  else {
    encode(versionedKey, 0);
  }
}
unsigned char VersionedKey::version() const {
  return (unsigned char)strtol(m_versionKey+1, NULL, 10);
}
/// @returns base key
const char* VersionedKey::key() const { 
  return m_versionKey+4; 
}

bool 
VersionedKey::sameKey(const VersionedKey& vkey) const {
  return (0 == strcmp(this->key(),vkey.key())); 
}

bool 
VersionedKey::sameKey(const std::string& baseKey) const {
  return (0 == strcmp(this->key(),baseKey.c_str())); 
}

bool 
VersionedKey::sameKey(const char* baseKey) const {
  return (0 == strcmp(this->key(),baseKey)); 
}

/// sort according to highest key version
bool operator < (const SG::VersionedKey& lhs, const SG::VersionedKey& rhs) {
  char *lhskey(0);
  unsigned char lhsVersion(0);
  lhs.decode(lhskey, lhsVersion);
  unsigned char rhsVersion(0);
  char *rhskey(0);
  rhs.decode(rhskey, rhsVersion);
  int keyCompare(strcmp(lhskey, rhskey));
  return ( ( keyCompare < 0) ||
	   ( (keyCompare == 0) && (lhsVersion < rhsVersion) ) ) ;
	  
}
