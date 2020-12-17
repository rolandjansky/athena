// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   SimpleEncrypter.h
 * @author Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch>
 *
 * @brief  Provide simple asymmetric encryption for blinding of float values.
 */

#ifndef BPHYSTOOLS_SIMPLEENCRYPTER_H
#define BPHYSTOOLS_SIMPLEENCRYPTER_H

// Framework includes
#include "AsgMessaging/AsgMessaging.h"

// System includes
#include <string>
#include <set>

namespace xAOD {
  ///
  /// @class  SimpleEncrypter
  /// @author Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
  ///
  /// @brief  Provide simple asymmetric encryption for blinding of float values.
  ///
  /// Provides asymmetric key encryption for blinding of positive float
  /// values.  Internally it uses a simple RSA encryption of bits in
  /// the floating point numbers.
  /// This class is used by the BPhysBlindingTool.
  ///
  class SimpleEncrypter : public asg::AsgMessaging {
    
  public:
    /// @brief Useful typedefs
    typedef long long int          LLI_t;
    typedef unsigned long long int ULLI_t;

    ///
    /// @brief Main constructor
    ///
    /// @param[in] name of instance
    ///
    SimpleEncrypter(const std::string& name = "SimpleEncrypter");

    ///
    /// @brief Default destructor
    ///
    virtual ~SimpleEncrypter();

    ///
    /// @brief Generate private and public keys
    ///
    /// @returns key pair as string: [private key, public key]
    ///
    virtual std::pair<std::string, std::string> genKeyPair();
    
    ///
    /// @brief Set private key
    ///
    /// @param[in] hex string with private key
    ///
    virtual void setPrivKey(std::string keystr);

    ///
    /// @brief Set public key
    ///
    /// @param[in] hex string with public key
    ///
    virtual void setPubKey(std::string keystr);

    ///
    /// @brief Get private key
    ///
    /// @returns hex string with private key
    ///
    virtual std::string getPrivKey() const;

    ///
    /// @brief Get public key
    ///
    /// @returns hex string with public key
    ///
    virtual std::string getPubKey() const;

    ///
    /// @brief Encrypt a positive integer value
    ///
    /// @param[in] unsigned integer value to be encrypted
    ///
    /// @returns encrypted unsigned integer value
    ///
    virtual ULLI_t encrypt(ULLI_t x);
    
    ///
    /// @brief Decrypt a positive integer value
    ///
    /// @param[in] unsigned integer value to be decrypted
    ///
    /// @returns encrypted unsigned integer value
    ///
    virtual ULLI_t decrypt(ULLI_t x);
    
    ///
    /// @brief Encrypt a positive float value
    ///
    /// @param[in] positive float value to be encrypted
    ///
    /// @returns encrypted float value
    ///
    virtual float encrypt(float x);
    
    ///
    /// @brief Decrypt a positive float value
    ///
    /// @param[in] positive float value to be decrypted
    ///
    /// @returns encrypted float value
    ///
    virtual float decrypt(float x);
    

  private:
    ///
    /// @name Key generation utilities
    /// @{
    ///
    /// @brief Internally generate numeric representation of key pair
    ///
    virtual void genKeyPairInternal();
    ///
    /// @brief Find a prime number
    ///
    virtual ULLI_t genPrime() const;
    ///
    /// @brief Check for being a prime number
    ///
    virtual bool isPrime(ULLI_t n) const;
    ///
    /// @brief Find greatest common denominator
    ///
    virtual ULLI_t greatestCommonDenominator(ULLI_t n1, ULLI_t n2) const;
    ///
    /// @brief Find a coprime number
    ///
    virtual ULLI_t genCoprime(ULLI_t n) const;
    ///
    /// @brief Find decryption exponent
    ///
    virtual ULLI_t genDecryptionExponent(ULLI_t phi, ULLI_t e) const;
    ///
    /// @}
    ///
    /// @name Key conversion utilities
    /// @{
    ///
    /// @brief Convert key to hex string
    ///    
    virtual std::string keyToString(ULLI_t a, ULLI_t b) const;
    ///
    /// @brief Decode hex string to two integers
    ///
    virtual std::pair<ULLI_t, ULLI_t> decodeKeyString(std::string str) const;
    /// @}
    ///
    /// @name float <-> int conversion utilities
    /// @{
    ///
    /// @brief Interpret bits of floating point number as integer
    ///    
    virtual ULLI_t floatBitsToInt(float val) const;
    ///
    /// @brief Interpret bits of integer as floating point number
    ///    
    virtual float intBitsToFloat(ULLI_t val) const;
    /// @}
    ///
    /// @name Internal en-/decryption methods
    /// @{
    ///
    /// @brief Encrypt using format preserving encryption w.r.t. RSA modulus
    ///
    ULLI_t encryptFPECycle(ULLI_t a) const;
    ///
    /// @brief Decrypt using format preserving encryption w.r.t. RSA modulus
    ///
    ULLI_t decryptFPECycle(ULLI_t a) const; 
    ///
    /// @brief Encrypt integer (internal)
    ///
    ULLI_t encryptInternal(ULLI_t x) const;
    ///
    /// @brief Decrypt integer (internal)
    ///
    ULLI_t decryptInternal(ULLI_t x) const;
    ///
    /// @brief Exponentiate a with d observing modulus n
    ///
    ULLI_t powerMod(ULLI_t a, ULLI_t d, ULLI_t n) const;
    ///
    /// @brief Check setup readiness for encryption
    ///
    bool isOkForEnc();
    ///
    /// @brief Check setup readiness for decryption
    ///
    bool isOkForDec();
    ///
    /// @}
    
  private:
    ///
    /// @name Internal static consts
    ///
    /// @brief Approximate range for prime numbers to be generated in
    static const ULLI_t m_MAXRANGE;
    static const ULLI_t m_MINRANGE;
    /// @brief maximum number of hex digits for key parts
    static const unsigned int m_MAXHEXDIGITS;
    
    ///
    /// @name Internal member variables
    ///
    /// RSA modulus: common part of both keys
    ULLI_t m_n;
    /// encryption exponent: public key part II
    ULLI_t m_e;
    /// decryption exponent: private key part II
    ULLI_t m_d;

    /// indicates that keys are set and range checks are ok
    bool   m_isOkForEnc;
    bool   m_isOkForDec;

  }; // class
  
} // namespace xAOD

#endif // BPHYSTOOLS_SIMPLEENCRYPTER_H

