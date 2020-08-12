/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MAGFIELDUTILS_MAGFIELDCONDREADER_H
#define MAGFIELDUTILS_MAGFIELDCONDREADER_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "StoreGate/ReadHandleKey.h"

namespace MagField {

  /**
   * Testing algorithm to read magnetic field
   */
  class CondReader : public AthReentrantAlgorithm {

  public:
    CondReader(const std::string& name, ISvcLocator* pSvcLocator);
    StatusCode initialize() override;
    StatusCode execute(const EventContext& ctx) const override;

  private:
    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheKey{
        this, "AtlasFieldCacheCondObj", "fieldCondObj", "Magnetic Field conditions object key"};
  };

} // namespace MagField

#endif
