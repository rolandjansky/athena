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

VersionedKey::~VersionedKey() {}

bool
VersionedKey::isVersionedKey(const char * vkey) { 
  return (vkey[0]==separator() && vkey[3]==separator());
}

bool 
VersionedKey::isVersionedKey(const std::string& vkey) {
  return isVersionedKey(vkey.c_str());
}

bool 
VersionedKey::isAuto(const std::string& vkey) {
  return (vkey.substr(0,4) == VersionedKey::autoVS());
}

VersionedKey::VersionedKey(const char* key, unsigned char version) {
  encode(std::string(key), version);
}
VersionedKey::VersionedKey(const std::string& key, unsigned char version) {
  encode(key, version);
}
VersionedKey::VersionedKey(const std::string& versionedKey) {
  copyVK(versionedKey);
}
VersionedKey::VersionedKey(const char* versionedKey) {
  copyVK(std::string(versionedKey));
}
VersionedKey::VersionedKey(const VersionedKey& versionedKey) :
  m_versionKey(versionedKey.m_versionKey),
  m_baseKey (versionedKey.m_baseKey)
{
}

void VersionedKey::decode(std::string& outKey, unsigned char& version) const {
  outKey = this->key();
  version = (unsigned char)atoi(m_versionKey.substr(1,2).c_str());
  assert(version <= 99);
}

void VersionedKey::encode(const std::string& inKey, unsigned char version) {
  assert(version <= 99);
  char vers[5];
  snprintf(vers, 5, versionFormatString(), version);
  m_versionKey = vers + inKey;
  m_baseKey = inKey;
}

void VersionedKey::copyVK(const std::string& inKey) {
  if (isVersionedKey(inKey)) {
    m_versionKey = inKey;
    m_baseKey = m_versionKey.substr(4);
  } else {
    encode(inKey, 0);  //FIXME should autoincrement
  }
}
unsigned char VersionedKey::version() const {
  return (unsigned char) std::stoul(m_versionKey.substr(1,2), nullptr, 0);
}
/// @returns base key
const std::string& VersionedKey::key() const { 
  return m_baseKey;
}

bool 
VersionedKey::sameKey(const VersionedKey& vkey) const {
  return (this->key() == vkey.key()); 
}

bool 
VersionedKey::sameKey(const std::string& baseKey) const {
  return (this->key() == baseKey); 
}

bool 
VersionedKey::sameKey(const char* baseKey) const {
  return (this->key() == std::string(baseKey)); 
}

/// sort according to highest key version
bool operator < (const SG::VersionedKey& lhs, const SG::VersionedKey& rhs) {
  std::string lhskey;
  unsigned char lhsVersion(0);
  lhs.decode(lhskey, lhsVersion);
  unsigned char rhsVersion(0);
  std::string rhskey;
  rhs.decode(rhskey, rhsVersion);
  int keyCompare(strcmp(lhskey.c_str(), rhskey.c_str()));
  return ( ( keyCompare < 0) ||
	   ( (keyCompare == 0) && (lhsVersion < rhsVersion) ) ) ;
	  
}
