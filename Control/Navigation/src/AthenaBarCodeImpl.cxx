/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>

#include "GaudiKernel/ServiceHandle.h"

#include "AthenaKernel/IJobIDSvc.h"

#include "Navigation/AthenaBarCodeImpl.h"

#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <iostream>
#include <cstdlib>

#include "uuid/uuid.h"

std::atomic<AthenaBarCode_t> AthenaBarCodeImpl::m_barcodeCounter = 0;

void AthenaBarCodeImpl::initABC() const {

  //This function will be call the first time any "access" function is called
  //using const type because those "access" functions might be const
  //Will only change mutable variables

  //  std::cout<<"Calling AthenaBarCodeImpl::AthenaBarCodeImpl() "<<std::endl;

  //* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  //Set counter part
  //* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

  const AthenaBarCode_t counter = m_barcodeCounter++;

  AthenaBarCode_t barcode = IAthenaBarCode::UNDEFINEDBARCODE;

  try {
    setBits(SCounterBits, CounterBits, counter & ((1u<<CounterBits)-1),
            barcode);
  }
  catch (const GaudiException& Exception) {
    throw std::runtime_error(
        "AthenaBarCodeImpl::Can not Set Counter Bit, Counter Overflow");
  }

  //* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  //Set reserve part
  //* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  try {
    setBits(SReserveBits, ReserveBits, 0, barcode);
  }
  catch (const GaudiException& Exception) {
    throw std::runtime_error(
        "AthenaBarCodeImpl::Can not initialize Reserve Bit");
  }

  //* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  //Set Version part
  //* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  try {
    setBits(SVersionBits, VersionBits, 0, barcode);
  }
  catch (const GaudiException& Exception) {
    throw std::runtime_error(
        "AthenaBarCodeImpl::Can not initialize Version Bit");
  }


  try {
    setBits(SUUIDBits, UUIDBits, getDefaultHash(), barcode);
  }
  catch (const GaudiException& Exception) {
    throw std::runtime_error("AthenaBarCodeImpl::Can not set UUID Hash Bits");
  }

  m_barcode = barcode;

  /*  std::cout << "[AthenaBarCodeImpl::m_barcode,m_defaultHash,m_barcodeCounter]= "
   << std::hex
   << (m_barcode)<< "\t"
   << m_defaultHash<< "\t"
   << (m_barcodeCounter)<<"\t"
   << std::dec
   << std::endl;
   */
}

AthenaBarCodeImpl::AthenaBarCodeImpl()
  : m_barcode (IAthenaBarCode::UNDEFINEDBARCODE)
{
}

AthenaBarCodeImpl::AthenaBarCodeImpl (const AthenaBarCodeImpl& other)
  : m_barcode (static_cast<AthenaBarCode_t>(other.m_barcode))
{
}

AthenaBarCodeImpl& AthenaBarCodeImpl::operator= (const AthenaBarCodeImpl& other)
{
  if (this != &other) {
    m_barcode = static_cast<AthenaBarCode_t>(other.m_barcode);
  }
  return *this;
}

bool
AthenaBarCodeImpl::hasSameAthenaBarCode(const IAthenaBarCode &obj) const {
  if (m_barcode==IAthenaBarCode::UNDEFINEDBARCODE) 
    initABC();

  if (obj.getAthenaBarCode() == m_barcode)
    return true;
  return false;
}

bool
AthenaBarCodeImpl::hasSameAthenaBarCodeExceptVersion(const IAthenaBarCode &obj) const {
  if (m_barcode==IAthenaBarCode::UNDEFINEDBARCODE) 
    initABC();

  if ((obj.getAthenaBarCode()) >> (ReserveBits + VersionBits) == m_barcode
      >> (ReserveBits + VersionBits))
    return true;
  return false;
}

AthenaBarCodeVersion_t
AthenaBarCodeImpl::getVersion() const {
  if (m_barcode==IAthenaBarCode::UNDEFINEDBARCODE) 
    initABC();
  return getBits(SVersionBits, VersionBits);
}

void
AthenaBarCodeImpl::newVersion() {

  if (m_barcode==IAthenaBarCode::UNDEFINEDBARCODE) 
    initABC();

  AthenaBarCodeVersion_t currversion = getVersion();

  try {
    AthenaBarCode_t bc = m_barcode;
    setBits(SVersionBits, VersionBits, currversion + 1, bc);
    m_barcode = bc;
  }
  catch (const GaudiException& Exception) {
    throw std::runtime_error(
        "AthenaBarCodeImpl::newVersion()::Version Overflow");
  }
}

void
AthenaBarCodeImpl::setVersion(AthenaBarCodeVersion_t ver) {
  if (m_barcode==IAthenaBarCode::UNDEFINEDBARCODE) 
    initABC();

  try {
    AthenaBarCode_t bc = m_barcode;
    setBits(SVersionBits, VersionBits, ver, bc);
    m_barcode = bc;
  }
  catch (const GaudiException& Exception) {
    throw std::runtime_error("AthenaBarCodeImpl::newVersion()::Version Too big");
  }
}

std::ostream&
AthenaBarCodeImpl::dump(std::ostream& out) const {
  if (m_barcode==IAthenaBarCode::UNDEFINEDBARCODE) 
    initABC();

  out << "\n[AthenaBarCode]= " << std::hex << (m_barcode) << std::dec << std::endl;
  return out;
}

bool
AthenaBarCodeImpl::createdInCurrentJob() const {
  if (m_barcode==IAthenaBarCode::UNDEFINEDBARCODE) 
    initABC();

  //FIXME: it is possible that m_defaultHash is not yet initialized.
  return getUUIDHash() == getDefaultHash();
}

AthenaBarCode_t
AthenaBarCodeImpl::hashUUID(const char *guid) {
  const int maxdigs = 16;
  AthenaBarCode_t tmp1 = static_cast<AthenaBarCode_t> (2166136261UL);

  for (int i = 0; i < maxdigs; i++) {
    tmp1 ^= (AthenaBarCode_t)(guid[i]);
    tmp1 *= 16777619UL;
  }

  return (tmp1 << (TotalBits - UUIDBits)) >> (TotalBits - UUIDBits);
}

AthenaBarCode_t
AthenaBarCodeImpl::getAthenaBarCode() const {
  if (m_barcode==IAthenaBarCode::UNDEFINEDBARCODE) 
    initABC();

  return m_barcode;
}

void
AthenaBarCodeImpl::setAthenaBarCode(AthenaBarCode_t id) {
  m_barcode = id;
}

AthenaBarCode_t
AthenaBarCodeImpl::getReserveBits() const {
  if (m_barcode==IAthenaBarCode::UNDEFINEDBARCODE) 
    initABC();
  return getBits(SReserveBits, ReserveBits);
}

void
AthenaBarCodeImpl::setReserveBits(AthenaBarCode_t id) {
  if (m_barcode==IAthenaBarCode::UNDEFINEDBARCODE) 
    initABC();

  try {
    AthenaBarCode_t bc = m_barcode;
    setBits(SReserveBits, ReserveBits, id, bc);
    m_barcode = bc;
  }
  catch (const GaudiException& Exception) {
    throw std::runtime_error(
        "AthenaBarCodeImpl::newVersion()::Version Overflow");
  }
}

AthenaBarCode_t
AthenaBarCodeImpl::hasUUIDHash() const {
  return getUUIDHash();
}

AthenaBarCode_t
AthenaBarCodeImpl::getUUIDHash() const {
  return getBits(SUUIDBits, UUIDBits);
}

AthenaBarCode_t
AthenaBarCodeImpl::getBits(unsigned short startbit, unsigned short nbits) const {

  AthenaBarCode_t tmp;
  //now m_barcode=aaaaaxxaa;
  tmp = m_barcode << (startbit); //tmp=xxaa00000;
  tmp = tmp >> (TotalBits - nbits); //tmp=0000000xx

  return tmp;
}

void
AthenaBarCodeImpl::setBits(unsigned short startbit, unsigned short nbits,
			   AthenaBarCode_t id,
                           AthenaBarCode_t& bc) const
{
  AthenaBarCode_t tmp;
  AthenaBarCode_t tmp2;
  tmp2 = 0;

  if ((id) >= ((~tmp2) >> (TotalBits - nbits))) {

    IMessageSvc *msgsvc = Athena::getMessageSvc();
    if (msgsvc) {
      MsgStream msg(msgsvc, "AthenaBarCodeImpl");
      msg << MSG::WARNING << "setBits::bit to be set:" << id
          << " is larger than limit:" << ((~tmp2) >> (TotalBits - nbits))
          << endmsg;
    }
    else {
      std::cout << "setBits::bit to be set:" << id << " is larger than limit:"
          << ((~tmp2) >> (TotalBits - nbits)) << std::endl;
    }
    throw std::runtime_error("AthenaBarCodeImpl::setBit Failed");
  }

  ///now id=0000000xx;
  tmp = (id << (TotalBits - nbits - startbit)); //tmp=00000xx00
  tmp2 = ((~tmp2) >> (TotalBits - nbits)); //tmp2=000000011

  tmp2 = (tmp2 << (TotalBits - nbits - startbit)); //tmp2=000001100
  tmp2 = (~tmp2); //tmp2=111110011

  //now m_barcode=aaaaayyaa
  bc &= tmp2; //m_barcode=aaaaa00aa
  bc |= tmp; //m_barcode=aaaaaxxaa

}

void
AthenaBarCodeImpl::setDefaultHash(const char* jobid) {
  getDefaultHash (jobid);
}


AthenaBarCode_t
AthenaBarCodeImpl::getDefaultHash (const char* jobid /*= nullptr*/)
{
  static const AthenaBarCode_t defaultHash = makeDefaultHash (jobid);
  return defaultHash;
}


AthenaBarCode_t
AthenaBarCodeImpl::makeDefaultHash (const char* jobid)
{
  if (jobid) {
    return AthenaBarCodeImpl::hashUUID(jobid);
  }

  // First look for a uuid as an environment variable.
  // This can be set in cases where we don't have the full
  // Gaudi/Athena environment available (eg, ARA), and we
  // don't want to try to create JobIDSvc.  This has to be
  // communicated in some way external to this library,
  // as we may end up here while initializing the dictionary
  // for this library.
  const char* env_uuid = getenv ("_ATHENABARCODEIMPL_JOBUUID");
  if (env_uuid) {
    return AthenaBarCodeImpl::hashUUID(env_uuid);
  }

  //    std::cout<<"no UUID stored, generating."<<std::endl;

  ServiceHandle<IJobIDSvc> p_jobidsvc("JobIDSvc", "JobIDSvc");
  StatusCode sc = p_jobidsvc.retrieve();
  if (!sc.isSuccess() || 0 == p_jobidsvc) {
    //FIXME
    //use uuid instead if service not available
    JobID_t JobID;
    uuid_generate(JobID);
    return AthenaBarCodeImpl::hashUUID((const char *) JobID);
    /*      std::cout << "Could not find JobIDSvc, using uuid directly"
            <<"jobid "<<o.str()
            << std::endl;*/
  }

  PJobID_t pjobid = p_jobidsvc->getJobID();
  return AthenaBarCodeImpl::hashUUID((const char *) pjobid);
}
