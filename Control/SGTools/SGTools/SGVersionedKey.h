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
  template <class T>  class ObjectWithVersion;

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

    ///quickly determine whether a string has the right format to be a VK
    ///with auto-generated version #
    static bool isAuto(const std::string&);

    /// version must be [0,98], 0 is the default version
    VersionedKey(const char* key, unsigned char version);
    /// version must be [0,98], 0 is the default version
    VersionedKey(const std::string& key, unsigned char version);

    /// make a VersionedKey from vkey string. If vkey has the
    /// VersionedKey format (;NN;key), it is simply copied, otherwise
    /// it is taken to be the real key, and default version is assigned
    explicit VersionedKey(const char* vkey);
    /// make a VersionedKey from vkey string. If vkey has the
    /// VersionedKey format (;NN;key), it is simply copied, otherwise
    /// it is taken to be the real key, and default version is assigned
    explicit VersionedKey(const std::string& vkey);
    
    VersionedKey(const VersionedKey& rhs);

    //no assignment
    VersionedKey& operator= (const VersionedKey&) = delete;

    ~VersionedKey();

    /// sets outKey to point to base key, and version to encoded version (0 is taken to mean default version). 
    void decode(std::string& outKey, unsigned char& version) const;
    operator std::string() const { return m_versionKey; }
    /// @returns version number
    unsigned char version() const;
    /// @returns base key
    const std::string& key() const;
    /// @returns access underlying encoded m_versionKey C string
    const std::string& rawVersionKey() const {
      return m_versionKey;
    }
    /// compare base keys
    bool sameKey(const VersionedKey& vkey) const;
    /// compare base keys
    bool sameKey(const std::string& baseKey) const;
    /// compare base keys
    bool sameKey(const char* baseKey) const;
    bool isAuto() const { return version() == VersionedKey::autoV(); }

  private:
    template <class U> friend class ObjectWithVersion;
    /// default constructor (invalid state, do not use)
    VersionedKey(): m_versionKey() {}

    void encode(const std::string& inKey, unsigned char version);
    void copyVK(const std::string& inKey);

    static char separator() {return ';';}
    static const char* versionFormatString() {return ";%02u;";}
    static const char* formatString() {return ";%02u;%s";}

    static unsigned char autoV() { return 99;}
    static unsigned char defaultV() { return 0;}
    static const char* autoVS() { return "_99_"; }
    static const char* defaultVS() { return "_00_"; }

    ///the encoded version/key. Mutable so that ownership can be transferred
    ///in the copy constructor
    std::string m_versionKey; 
    std::string m_baseKey;
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
