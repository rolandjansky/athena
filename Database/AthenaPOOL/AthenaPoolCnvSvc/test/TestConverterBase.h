// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthenaPoolCnvSvc/TestConverterBase.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2019
 * @brief Dummy base class for test converters.
 */


#ifndef ATHENAPOOLCNVSVC_TESTCONVERTERBASE_H
#define ATHENAPOOLCNVSVC_TESTCONVERTERBASE_H


#include "AthenaPoolCnvSvc/ITPConverter.h"


class TestConverterBase
  : public ITPConverter
{
public:
  virtual void persToTransUntyped(const void*, void*, MsgStream&) override
  { std::abort(); }
  virtual void transToPersUntyped(const void*, void*, MsgStream&) override
  { std::abort(); }
  virtual void persToTransWithKeyUntyped(const void*, void*, const std::string&, MsgStream&) override
  { std::abort(); }
  virtual void transToPersWithKeyUntyped(const void*, void*, const std::string&, MsgStream&) override
  { std::abort(); }
  virtual const std::type_info& transientTInfo() const override
  { std::abort(); }
  virtual const std::type_info& persistentTInfo() const override
  { std::abort(); }
  virtual void setTopConverter(TopLevelTPCnvBase*, const TPObjRef::typeID_t&) override
  { std::abort(); }
  virtual void setRuntimeTopConverter(TopLevelTPCnvBase*) override
  { std::abort(); }
  virtual const TPObjRef::typeID_t& typeID() const override
  { std::abort(); }
  virtual const std::type_info& transBaseTInfo() const override
  { std::abort(); }
  virtual void reservePStorage(size_t) override
  { std::abort(); }
};


#endif // not ATHENAPOOLCNVSVC_TESTCONVERTERBASE_H
