///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENABARCODEIMPL_H
#define ATHENABARCODEIMPL_H

///////////////////////////////////////////////////////////////////////////////
///
///              AthenaBarCodeImpl interface class
///
/// \author  Yushu Yao
///
/// \brief barcode for all INav4Mom classes
///
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

// STL includes

#include <inttypes.h>

#include <iosfwd>
#include <atomic>

#include <AthenaKernel/IAthenaBarCode.h> //for static const data memebers
///////////////////////////////////////////////////////////////////////////////
//
// Universal Object Identifier for all INav4Mom Objects
//
///////////////////////////////////////////////////////////////////////////////

class StatusCode;

class AthenaBarCodeImpl {
  friend class AthenaBarCodeCnv_p1;
  
public:
  static const unsigned short TotalBits   = 64;
  
  static const unsigned short UUIDBits    = 32;
  static const unsigned short CounterBits = 26;
  static const unsigned short VersionBits = 4;
  static const unsigned short ReserveBits = 2;
  
  static const unsigned short SUUIDBits    = 0;
  static const unsigned short SCounterBits = UUIDBits;
  static const unsigned short SVersionBits = UUIDBits+CounterBits;
  static const unsigned short SReserveBits = UUIDBits+CounterBits+VersionBits;
  
  
public:
  //Constructors
  
  virtual ~AthenaBarCodeImpl() = default;
  
  AthenaBarCodeImpl();
  AthenaBarCodeImpl (const AthenaBarCodeImpl&);
  AthenaBarCodeImpl& operator= (const AthenaBarCodeImpl&);
  
public:
  //Public setter and getters
  AthenaBarCode_t getAthenaBarCode() const;
  void setAthenaBarCode(AthenaBarCode_t id);
  
public:
  //Comparing & Versioning Control
  bool hasSameAthenaBarCode(const IAthenaBarCode &obj) const;
  bool hasSameAthenaBarCodeExceptVersion(const IAthenaBarCode &obj) const;
  
  AthenaBarCodeVersion_t getVersion() const;
  void newVersion();
  void setVersion(AthenaBarCodeVersion_t newversion);
  
  std::ostream& dump(std::ostream& out) const;
  
  //True if the athenabarcode is created in this job, meaning a newly created object. False if the object is read from a file.
  bool createdInCurrentJob() const;
  
  //Handle reserved bits, mainly for read back of older version of AthenaBarCode when the configuration of #bits for UUID,counter and version bits are different from the current one.
  AthenaBarCode_t getReserveBits() const;
  void setReserveBits(AthenaBarCode_t id);

  // Used to set the default hash when JobIDSvc is not being used.
  // Should not be called within Athena.
  static void setDefaultHash (const char* jobid);
  
protected:
  void setBits(unsigned short startbit,unsigned short nbits,AthenaBarCode_t id,
               AthenaBarCode_t& bc) const; 
  AthenaBarCode_t getBits(unsigned short startbit,unsigned short nbits) const ;
  void initABC() const ;

private:
  //UUID related
  AthenaBarCode_t combineWithUUIDHash(const AthenaBarCode_t &) const;
  static AthenaBarCode_t hashUUID(const char *);
  
  void setUUIDHash(AthenaBarCode_t uuidhash);
  AthenaBarCode_t hasUUIDHash() const;
  AthenaBarCode_t getUUIDHash() const;

  static AthenaBarCode_t getDefaultHash (const char* jobid = nullptr);
  static AthenaBarCode_t makeDefaultHash (const char* jobid);
  
  //data fields
  static std::atomic<AthenaBarCode_t> m_barcodeCounter;
  mutable std::atomic<AthenaBarCode_t> m_barcode;
  
};

inline void AthenaBarCodeImpl::setUUIDHash(AthenaBarCode_t uuidhash) {
  m_barcode = combineWithUUIDHash(uuidhash);
}


inline AthenaBarCode_t AthenaBarCodeImpl::combineWithUUIDHash(
							      const AthenaBarCode_t &hash) const {
  
  AthenaBarCode_t tmp = 0;
  AthenaBarCode_t lowerh = ~tmp >> (TotalBits
				    - UUIDBits);
  AthenaBarCode_t higherh = ~tmp << (UUIDBits);
  
  AthenaBarCode_t lower = m_barcode & lowerh;
  AthenaBarCode_t higher = hash & higherh;
  
  /*
    std::cout << "AthenaBarCodeImpl::combineWithHash::hash=" << std::hex
    << hash << " lower=" << lower << " higher=" << higher << " lowerh="
    << lowerh << " higherh=" << higherh << " barcode=" << (lower
    | higher) << std::endl;
  */
  return lower | higher;
	
}

#endif // not NAVIGATION_ATHENABARCODE_H
