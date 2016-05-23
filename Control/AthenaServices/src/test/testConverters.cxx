/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dummy converters for testing AthTPCnvSvc

#include "AthenaKernel/TPCnvFactory.h"
#include "AthenaKernel/ITPCnvBase.h"


namespace AthenaServicesTestConverters {


class TestConverterBase
  : public ITPCnvBase
{
public:
  virtual 
  void 
  persToTransUntyped(const void*, void*, MsgStream&) override {}

  virtual
  void 
  transToPersUntyped(const void*, void*, MsgStream&) override {}

  virtual 
  const std::type_info& transientTInfo() const override
  { return typeid(*this); }

  virtual
  const std::type_info& persistentTInfo() const override
  { return typeid(*this); }
};


class TestConverter_TA_PA1 : public TestConverterBase {};
class TestConverter_TA_PA2 : public TestConverterBase {};

class TestConverter_TB_PB1 : public TestConverterBase {};
class TestConverter_TB_PB1_ARA : public TestConverterBase {};
class TestConverter_TBTRIG_PB1 : public TestConverterBase {};


} // namespace AthenaServicesTestConverters


DECLARE_TPCNV_FACTORY (AthenaServicesTestConverters::TestConverter_TA_PA1,
                       AthenaServicesTestConverters::TA,
                       AthenaServicesTestConverters::PA1,
                       Athena::TPCnvVers::Old)
DECLARE_TPCNV_FACTORY (AthenaServicesTestConverters::TestConverter_TA_PA2,
                       AthenaServicesTestConverters::TA,
                       AthenaServicesTestConverters::PA2,
                       Athena::TPCnvVers::Current)


DECLARE_TPCNV_FACTORY (AthenaServicesTestConverters::TestConverter_TB_PB1,
                       AthenaServicesTestConverters::TB,
                       AthenaServicesTestConverters::PB1,
                       Athena::TPCnvVers::Current)
DECLARE_ARATPCNV_FACTORY (AthenaServicesTestConverters::TestConverter_TB_PB1_ARA,
                          AthenaServicesTestConverters::TB,
                          AthenaServicesTestConverters::PB1,
                          Athena::TPCnvVers::Current)
DECLARE_TRIGTPCNV_FACTORY (AthenaServicesTestConverters::TestConverter_TBTRIG_PB1,
                          AthenaServicesTestConverters::TBTRIG,
                          AthenaServicesTestConverters::PB1,
                          Athena::TPCnvVers::Current)

