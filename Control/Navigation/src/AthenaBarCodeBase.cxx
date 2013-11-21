/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file Navigation/src/AthenaBarCodeBase.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2011
 * @brief A mixin base class implementing @c IAthenaBarCode.
 *        @c IAthenaBarCode should be a virtual base of the class
 *        using this.
 */


#include "Navigation/AthenaBarCodeBase.h"


//*** Forward all the virtual functions of the interface to the Impl class.


AthenaBarCode_t AthenaBarCodeBase::getAthenaBarCode() const
{
  return AthenaBarCodeImpl::getAthenaBarCode();
}


void AthenaBarCodeBase::setAthenaBarCode(AthenaBarCode_t id)
{
  return AthenaBarCodeImpl::setAthenaBarCode (id);
}


bool AthenaBarCodeBase::hasSameAthenaBarCode(const IAthenaBarCode &obj) const
{
  return AthenaBarCodeImpl::hasSameAthenaBarCode (obj);
}


bool
AthenaBarCodeBase::hasSameAthenaBarCodeExceptVersion(const IAthenaBarCode &obj) const
{
  return AthenaBarCodeImpl::hasSameAthenaBarCodeExceptVersion (obj);
}


AthenaBarCodeVersion_t AthenaBarCodeBase::getVersion() const
{
  return AthenaBarCodeImpl::getVersion();
}


void AthenaBarCodeBase::newVersion()
{
  AthenaBarCodeImpl::newVersion();
}


void AthenaBarCodeBase::setVersion(AthenaBarCodeVersion_t newversion)
{
  AthenaBarCodeImpl::setVersion (newversion);
}

