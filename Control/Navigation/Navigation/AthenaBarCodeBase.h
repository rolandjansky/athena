// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file Navigation/AthenaBarCodeBase.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2011
 * @brief A mixin base class implementing @c IAthenaBarCode.
 *        @c IAthenaBarCode should be a virtual base of the class
 *        using this.
 */


#ifndef NAVIGATION_ATHENABARCODEBASE_H
#define NAVIGATION_ATHENABARCODEBASE_H


#include "AthenaKernel/IAthenaBarCode.h"
#include "Navigation/AthenaBarCodeImpl.h"


class AthenaBarCodeBase
  : public AthenaBarCodeImpl, virtual public IAthenaBarCode
{
public:
  // Forward all the virtual functions of the interface to the Impl class.
  virtual AthenaBarCode_t getAthenaBarCode() const;
  virtual void setAthenaBarCode(AthenaBarCode_t id);
  virtual bool hasSameAthenaBarCode(const IAthenaBarCode &obj) const;
  virtual bool hasSameAthenaBarCodeExceptVersion(const IAthenaBarCode &obj) const;
  virtual AthenaBarCodeVersion_t getVersion() const;
  virtual void newVersion();
  virtual void setVersion(AthenaBarCodeVersion_t newversion);
};


#endif // not NAVIGATION_ATHENABARCODEBASE_H
