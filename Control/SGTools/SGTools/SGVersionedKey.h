/* -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file SGVersionedKey.h
 *  defines  a StoreGateSvc key with a version number 
 */
#ifndef SGTOOLS_VERSIONEDKEY_H
#define SGTOOLS_VERSIONEDKEY_H 1
#include <iostream>
#include <string>
class StoreGateSvc; //our friend
class SGImplSvc; //our friend
namespace SG {
  /** @class SG::VersionedKey
   * @brief  a StoreGateSvc key with a version number.
   * Notice that StoreGate does not order multiple instances of an object with
   * a given key by version number. A generic retrieve will always return
   * the first version recorded. For example, if object ("MyKey",1) is recorded 
   * _before__ ("MyKey", 2) a sg.retrieve(pObj,"MyKey") will return ("MyKey",1).
   * StoreGateSvc provides retrieveAllVersions and retrieveHighestVersion,
   * which offer a better-defined behaviour.
   *
   * @author pcalafiura@lbl.gov - ATLAS Collaboration
   * $Id: SGVersionedKey.h,v 1.4 2008-07-10 15:51:04 dquarrie Exp $
   **/
  class VersionedKey {
    friend class ::StoreGateSvc; //call copyVK
    friend class ::SGImplSvc; //call copyVK
  public:
    ///quickly determine whether a string has the right format to be a VK
    static bool isVersionedKey(const char *);
    ///quickly determine whether a string has the right format to be a VK
    static bool isVersionedKey(const std::string&);

    /// version must be [0,99], 0 is the default version
    VersionedKey(const char* key, unsigned char version);
    /// version must be [0,99], 0 is the default version
    VersionedKey(const std::string& key, unsigned char version);

    /// make a VersionedKey from vkey string. If vkey has the
    /// VersionedKey format (;NN;key), it is simply copied, otherwise
    /// it is taken to be the real key, and version=0 is assigned
    explicit VersionedKey(const char* vkey);
    /// make a VersionedKey from vkey string. If vkey has the
    /// VersionedKey format (;NN;key), it is simply copied, otherwise
    /// it is taken to be the real key, and version=0 is assigned
    explicit VersionedKey(const std::string& vkey);
    
    /// copy constructor transfers ownership a la auto_ptr (hence rhs
    /// is not really const).
    VersionedKey(const VersionedKey& rhs);

    /// default constructor (invalid state, do not use)
    VersionedKey(): m_versionKey(0) {}

    ~VersionedKey();

    /// sets key to point to encoded key, and version to encoded version (0 is taken to mean default version). 
    void decode(char*& key, unsigned char& version) const;
    /// convert underlying encoded m_versionKey C string
    operator std::string() const { return std::string(m_versionKey); }
    /// @returns version number
    unsigned char version() const;
    /// @returns base key
    const char* key() const;
    /// @returns access underlying encoded m_versionKey C string
    const char* rawVersionKey() const {
      return m_versionKey;
    }
    /// compare base keys
    bool sameKey(const VersionedKey& vkey) const;
    /// compare base keys
    bool sameKey(const std::string& baseKey) const;
    /// compare base keys
    bool sameKey(const char* baseKey) const;

  private:
    void encode(const char* key, unsigned char version);
    void copyVK(const char* versionedKey);
    static char separator() {return ';';}
    static const char* formatString() {return ";%02u;%s";}
    ///the encoded version/key. Mutable so that ownership can be transferred
    ///in the copy constructor
    mutable char* m_versionKey; 

    VersionedKey& operator= (const VersionedKey&);
  };
} // ns SG

/// sort according to highest key version
bool operator < (const SG::VersionedKey& lhs, const SG::VersionedKey& rhs);

namespace SG {
  inline
    std::ostream& operator <<(std::ostream& ost, const SG::VersionedKey& k) {
    return ost << k.rawVersionKey();
    //  return ost.write(k.rawVersionKey(), strlen(k.rawVersionKey()));
  }
}

#endif
