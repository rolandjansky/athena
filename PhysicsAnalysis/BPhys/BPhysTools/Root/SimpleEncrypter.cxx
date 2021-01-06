/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// system include:
#include <climits>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>

// ROOT includes
#include <TString.h>

// Local include(s):
#include "BPhysTools/SimpleEncrypter.h"

namespace xAOD {

  //--------------------------------------------------------------------------
  // Private static constants
  //--------------------------------------------------------------------------
  // This gives 0x10000 on a 64-bit platform.
  // ??? Would probably be better to write these using bit operations,
  // rather than FP, to avoid potential rounding issues.
  //  (eg. the maximum uint64_t cannot be represented exactly as a double)
  const SimpleEncrypter::ULLI_t SimpleEncrypter::m_MAXRANGE =
    (SimpleEncrypter::ULLI_t)pow(static_cast<double>(std::numeric_limits<ULLI_t>::max()), 0.25);
  const SimpleEncrypter::ULLI_t SimpleEncrypter::m_MINRANGE =
    (SimpleEncrypter::ULLI_t)SimpleEncrypter::m_MAXRANGE/10;
  const unsigned int SimpleEncrypter::m_MAXHEXDIGITS =
    (unsigned int)(log(pow(SimpleEncrypter::m_MAXRANGE,2))/log(16.))+3;
  
  //--------------------------------------------------------------------------
  // Public methods
  //--------------------------------------------------------------------------

  //--------------------------------------------------------------------------
  // Constructor
  //--------------------------------------------------------------------------
  SimpleEncrypter::SimpleEncrypter(const std::string& name) :
    asg::AsgMessaging(name), m_n(0), m_e(0), m_d(0),
    m_isOkForEnc(false), m_isOkForDec(false) {

    // initialize random number generator
    srand(static_cast<unsigned>(time(0)));
  }
  
  //--------------------------------------------------------------------------
  // Destructor
  //--------------------------------------------------------------------------
  SimpleEncrypter::~SimpleEncrypter() {

  }
  
  //--------------------------------------------------------------------------
  // Generation of key pair as pair of hex strings
  //--------------------------------------------------------------------------
  std::pair<std::string, std::string> SimpleEncrypter::genKeyPair() {

    // default preset
    std::pair<std::string, std::string> keys =
      std::make_pair("__NO_PRIV_KEY__", "__NO_PUB_KEY__");

    // generate keys
    genKeyPairInternal();

    if ( isOkForEnc() && isOkForDec() ) {
      keys = std::make_pair(getPrivKey(), getPubKey());
    }
    return keys;
  }

  //--------------------------------------------------------------------------
  // Set private key
  //--------------------------------------------------------------------------
  void SimpleEncrypter::setPrivKey(std::string keystr) {
  
    std::pair<ULLI_t, ULLI_t> keys = decodeKeyString(keystr);
    
    if ( m_n > 0 && m_n != keys.first ) {
      ATH_MSG_WARNING("RSA module already set!");
    }
    m_n = keys.first;
    m_d = keys.second;
    m_isOkForDec = false;
  }
  //--------------------------------------------------------------------------
  // Set public key
  //--------------------------------------------------------------------------
  void SimpleEncrypter::setPubKey(std::string keystr) {
  
    std::pair<ULLI_t, ULLI_t> keys = decodeKeyString(keystr);
    
    if ( m_n > 0 && m_n != keys.second ) {
      ATH_MSG_WARNING("RSA module already set!");
    }
    m_e = keys.first;
    m_n = keys.second;
    m_isOkForEnc = false;
  }
  //--------------------------------------------------------------------------
  // Get private key
  //--------------------------------------------------------------------------
  std::string SimpleEncrypter::getPrivKey() const {
  
    return keyToString(m_n, m_d);
  }
  //--------------------------------------------------------------------------
  // Get public key
  //--------------------------------------------------------------------------
  std::string SimpleEncrypter::getPubKey() const {

    return keyToString(m_e, m_n);    
  }
  //--------------------------------------------------------------------------
  // Encrypt unsigned integer value
  //--------------------------------------------------------------------------
  SimpleEncrypter::ULLI_t SimpleEncrypter::encrypt(ULLI_t a) {

    ULLI_t b = a;

    if ( isOkForEnc() ) {
      b = encryptFPECycle(a);
    }
    return b;
  }
  //--------------------------------------------------------------------------
  // Decrypt unsigned integer value
  //--------------------------------------------------------------------------
  SimpleEncrypter::ULLI_t SimpleEncrypter::decrypt(ULLI_t a) {

    ULLI_t b = a;

    if ( isOkForDec() ) {
      b = decryptFPECycle(a);
    }
    return b;
  }
  //--------------------------------------------------------------------------
  // Encrypt positive float value
  //--------------------------------------------------------------------------
  float SimpleEncrypter::encrypt(float a) {

    float b = a;

    if ( a > 0. ) {
      if ( isOkForEnc() ) {
        ULLI_t ia = floatBitsToInt(a);
        ULLI_t ib = encryptFPECycle(ia);
        b = intBitsToFloat(ib);
      }
    } else {
      ATH_MSG_WARNING("Encrypt: Float value not positive: "
                      << a << Form(" (%a) !", a));
    } // if a > 0
    return b;
  }

  //--------------------------------------------------------------------------
  // Decrypt positive float value
  //--------------------------------------------------------------------------
  float SimpleEncrypter::decrypt(float a) {

    float b = a;

    // As nan is a valid encrypted value, decrypt it as well.
    if ( a > 0. || std::isnan(a) ) {
      if ( isOkForDec() ) {
        ULLI_t ia = floatBitsToInt(a);
        ULLI_t ib = decryptFPECycle(ia);
        b = intBitsToFloat(ib);
      }
    } else {
      ATH_MSG_WARNING("Decrypt: Float value not positive: "
                      << a << Form(" (%a) !", a));
    } // if a > 0
    return b;
  }

  //--------------------------------------------------------------------------
  // Private methods
  //--------------------------------------------------------------------------

  //--------------------------------------------------------------------------
  // Generate numeric representation of the keys
  //--------------------------------------------------------------------------
  void SimpleEncrypter::genKeyPairInternal() {

    // Generate prime numbers p != q
    ULLI_t p(1);
    ULLI_t q(1);
    // Euler's phi function
    ULLI_t phi(1);
    
    // reset encryption and decryption exponent
    m_e = 0;
    m_d = 0;
    while ( p == q || m_e < 2 || m_e >= phi || m_d < 2
            || m_e*m_d % phi != 1 ) {
      double dlog2 = 0.;
      while ( p == q || dlog2 < 0.1 || dlog2 > 30. ) {
        p = genPrime();
        q = genPrime();
        dlog2 = fabs(log2(p)-log2(q));
      } // inner while loop
      phi = (p-1)*(q-1);
      m_n = p*q;
      m_e = genCoprime(phi);
      m_d = genDecryptionExponent(phi, m_e);
    } // outer while loop
    m_isOkForDec = false;
    m_isOkForEnc = false;
  }
  //--------------------------------------------------------------------------
  // Find a prime number
  //--------------------------------------------------------------------------
  SimpleEncrypter::ULLI_t SimpleEncrypter::genPrime() const {
    
    ULLI_t t = (m_MINRANGE + rand()) % (m_MAXRANGE-1);
    do {
      t++;
    } while ( !isPrime(t) || t < m_MINRANGE );
    return t;
  }
  //--------------------------------------------------------------------------
  // Test for being a prime number
  //--------------------------------------------------------------------------
  bool SimpleEncrypter::isPrime(ULLI_t n) const {
    
    bool isPrime = true;
    if (n != 2) {
      for (LLI_t i = 2; i < (LLI_t)sqrt(n) + 1; ++i) {
        if (n % i == 0) {
          isPrime = false;
          break;
        }
      }
    }
    return isPrime;
  }
  //--------------------------------------------------------------------------
  // Greatest common denominator
  //--------------------------------------------------------------------------
  SimpleEncrypter::ULLI_t
  SimpleEncrypter::greatestCommonDenominator(ULLI_t n1, ULLI_t n2) const {

    std::vector<LLI_t> r;
    LLI_t i = 1;
    r.push_back(std::max(n1, n2));
    r.push_back(std::min(n1, n2));
    while (r[i] != 0) {
      ++i;
      r.push_back(r[i-2] % r[i-1]);
    }
    return r[i-1];
  }
  //--------------------------------------------------------------------------
  // Find coprime number
  //--------------------------------------------------------------------------
  SimpleEncrypter::ULLI_t SimpleEncrypter::genCoprime(ULLI_t n) const {
    
    // make sure coprime is larger than 5th Fermat number (2^16+1 = 65537)
    ULLI_t i = (65537 + rand()) % (m_MAXRANGE -1);
    do {
      ++i;
    } while (greatestCommonDenominator(n, i) != 1);
    return i;
  }
  //--------------------------------------------------------------------------
  // Find decryption exponent
  //--------------------------------------------------------------------------
  SimpleEncrypter::ULLI_t
  SimpleEncrypter::genDecryptionExponent(ULLI_t phi, ULLI_t e) const {
    
    for (ULLI_t i=1; i<m_MAXRANGE; ++i) {
      if ( ((phi * i + 1) % e) == 0 ) {
        return (ULLI_t)((phi * i + 1) / e);
      }
    }
    return 0;
  }
  //--------------------------------------------------------------------------
  // Convert key to a hex string
  //--------------------------------------------------------------------------
  std::string SimpleEncrypter::keyToString(ULLI_t a, ULLI_t b) const {

    // length of keys w.r.t. hex digits
    unsigned int ra = (unsigned int)(log(a)/log(16.))+1;
    unsigned int rb = (unsigned int)(log(b)/log(16.))+1;

    // random numbers for padding
    unsigned int r1 = rand() & ((1 << 4*(m_MAXHEXDIGITS-ra))-1);
    unsigned int r2 = rand() & ((1 << 4*(m_MAXHEXDIGITS-rb))-1);

    // format string
    TString tstr = Form("%02x%02x%02x%0*x%0*llx%0*x%0*llx",
                        m_MAXHEXDIGITS, ra, rb,
                        m_MAXHEXDIGITS-ra, r1, ra, a,
                        m_MAXHEXDIGITS-rb, r2, rb, b);
    
    return std::string(tstr.Data());
  }
  //--------------------------------------------------------------------------
  // Convert hex string to two integers
  //--------------------------------------------------------------------------
  std::pair<SimpleEncrypter::ULLI_t, SimpleEncrypter::ULLI_t>
  SimpleEncrypter::decodeKeyString(std::string hstr) const {
    
    std::pair<ULLI_t, ULLI_t> keys(0,0);
    
    TString str(hstr);
    if (str.IsHex() && str.Length() > 3) {
      str.ToLower();
      unsigned int ndigits = strtoul(TString(str(0,2)).Data(), nullptr, 16);
      unsigned int ra = strtoul(TString(str(2,2)).Data(), nullptr, 16); 
      unsigned int rb = strtoul(TString(str(4,2)).Data(), nullptr, 16);
      if ( str.Length() == (int)(2*ndigits + 6) ) {
        keys.first  = strtoll(TString(str(ndigits+6-ra, ra)).Data(),
                              nullptr, 16);
        keys.second = strtoll(TString(str(2*ndigits+6-rb, rb)).Data(),
                              nullptr, 16);
      } else {
        ATH_MSG_ERROR("Private/public key must be a hex string of " <<
                      2*m_MAXHEXDIGITS+6 << " digits!");
      } // if Length()
    } else {
      ATH_MSG_ERROR("Private/public key must be a hex string of " <<
                    2*m_MAXHEXDIGITS+6 << " digits!");
    } // if IsHex() ...
    
    return keys;
  }
  //--------------------------------------------------------------------------
  // Interpret bits of positive floating point number as integer
  //--------------------------------------------------------------------------
  SimpleEncrypter::ULLI_t SimpleEncrypter::floatBitsToInt(float val) const {

    ULLI_t res(0);

    if ( val < 0. ) {
      ATH_MSG_ERROR("Float value needs to be positive!");
    } else {
      // convert floating point number to ULLI_t if size fits
      if ( sizeof(float) <= sizeof(ULLI_t) ) {
        // check whether a quick conversion is possible
        if ( sizeof(float) == sizeof(int) ) {
          int* p = reinterpret_cast<int*>(&val);
          res = *p;
        } else {
        // do a slow conversion
          char* pval = reinterpret_cast<char*>(&val);
          // loop over bytes
          for (unsigned int i=0; i<sizeof(float); ++i) {
            // loop over bits
            for (unsigned int j=0; j<CHAR_BIT; ++j) {
              unsigned int n = i*CHAR_BIT + j;
              unsigned int bit = (*(pval+i) >> j) & 1;
              if ( bit > 0 ) res |= 1 << n;
            } // for bits
          } // for bytes
        } // if sizeof
      } else {
        ATH_MSG_ERROR("sizeof(float) > sizeof(ULLI_t): "
                      << sizeof(float) << " > " << sizeof(LLI_t));
      } // if sizeof
    } // if val < 0.

    return res;
  }
  //--------------------------------------------------------------------------
  // Interpret bits of positive integer as floating point number
  //--------------------------------------------------------------------------
  float SimpleEncrypter::intBitsToFloat(ULLI_t val) const {

    float res(0.);

    // number of bits needed
    unsigned int r = (int)(std::log2(val))+1;
    
    // convert ULLI_t to floating point number if size fits
    if ( sizeof(float)*CHAR_BIT >= r ) {
      // check whether a quick conversion is possible
      if ( sizeof(float) == sizeof(int) ) {
        float* p = reinterpret_cast<float*>(&val);
        res = *p;
      } else {
        // do a slow conversion
        char* pres = reinterpret_cast<char*>(&res);
        // loop over bytes
        for (unsigned int i=0; i<sizeof(float); ++i) {
          // loop over bits
          for (unsigned int j=0; j<CHAR_BIT; ++j) {
            unsigned int n = i*CHAR_BIT + j;
            unsigned int bit = (val >> n) & 1;
            if ( bit > 0 ) *(pres+i) |= 1 << j;
          } // for bits
        } // for bytes
      } // if sizeof
    } else {
      ATH_MSG_WARNING("sizeof(float)*CHAR_BIT < r: "
                      << sizeof(float)*CHAR_BIT << " < " << r);
    } // if sizeof
    
    return res;
  }
  //--------------------------------------------------------------------------
  // Encrypt using format preserving encryption w.r.t. RSA modulus
  // via cycling
  //--------------------------------------------------------------------------
  SimpleEncrypter::ULLI_t SimpleEncrypter::encryptFPECycle(ULLI_t a) const {
    
    ULLI_t enc = 0;
    if ( a > 0 ) {
      ULLI_t r = (int)(std::log2(m_n));
      ULLI_t rmask = pow(2,r)-1;
      ULLI_t c = a & rmask;
      ULLI_t b = a - c;
      do {
        c = encryptInternal(c);
      } while ( c > rmask );    
      enc = b + c;
    } // if
    return enc;
  }
  //--------------------------------------------------------------------------
  // Decrypt using format preserving encryption w.r.t. RSA modulus
  // via cycling
  //--------------------------------------------------------------------------
  SimpleEncrypter::ULLI_t SimpleEncrypter::decryptFPECycle(ULLI_t enc) const {

    ULLI_t dec = 0;
    if ( enc > 0 ) {
      ULLI_t r = (int)(std::log2(m_n));
      ULLI_t rmask = pow(2,r)-1;
      ULLI_t d = enc & rmask;
      ULLI_t b = enc - d;
      do {
        d = decryptInternal(d);
      } while ( d > rmask );
      dec = d + b;
    } // if
    return dec;
  }
  //--------------------------------------------------------------------------
  // Encrypt integer
  //--------------------------------------------------------------------------
  SimpleEncrypter::ULLI_t SimpleEncrypter::encryptInternal(ULLI_t x) const {
    
    return powerMod(x, m_e, m_n);
  }
  //--------------------------------------------------------------------------
  // Decrypt integer
  //--------------------------------------------------------------------------
  SimpleEncrypter::ULLI_t SimpleEncrypter::decryptInternal(ULLI_t x) const {
    
    return powerMod(x, m_d, m_n);
  }
  //--------------------------------------------------------------------------
  // Exponentiate a with d observing modulus n
  //--------------------------------------------------------------------------
  SimpleEncrypter::ULLI_t
  SimpleEncrypter::powerMod(ULLI_t a, ULLI_t d, ULLI_t n) const {

    int    bin[sizeof(ULLI_t)*CHAR_BIT];
    ULLI_t dec[sizeof(ULLI_t)*CHAR_BIT];
    
    ULLI_t r = (ULLI_t)(std::log2(d))+1;
    ULLI_t tmp = d;
    // decompose exponent into binary number (reverse order!)
    for (ULLI_t i=0; i < r; ++i) {
      bin[r-i-1] = tmp % 2;
      tmp = (LLI_t)(tmp/2);
    } // for i
    
    // perform the exponentiation taking modulus into account
    dec[0] = a;
    for (ULLI_t i=1; i < r; ++i) {
      ULLI_t d2 = dec[i-1]*dec[i-1] % n;
      if ( bin[i] > 0 ) d2 *= a;
      dec[i] = d2 % n;
    } // for i
    
    return dec[r-1];
  }
  //--------------------------------------------------------------------------
  // Check setup readiness for encryption
  //--------------------------------------------------------------------------
  bool SimpleEncrypter::isOkForEnc() {

    if ( !m_isOkForEnc ) {
      if ( m_n > 0 && m_e > 1 && m_e < m_n ) {
        m_isOkForEnc = true;
      } else {
        ATH_MSG_ERROR("Setup not OK for encryption: public key set?");
      }
    } // if ! m_isOkForEnc
    
    return m_isOkForEnc;
  }

  //--------------------------------------------------------------------------
  // Check setup readiness for decryption
  //--------------------------------------------------------------------------
  bool SimpleEncrypter::isOkForDec() {

    if ( !m_isOkForDec ) {
      if ( m_n > 0 && m_d > 1 && m_d < m_n ) {
        m_isOkForDec = true;
      } else {
        ATH_MSG_ERROR("Setup not OK for decryption: private key set?");
      }
    } // if ! m_isOkForDec
    
    return m_isOkForDec;
  }

  //--------------------------------------------------------------------------
} // namespace xAOD
