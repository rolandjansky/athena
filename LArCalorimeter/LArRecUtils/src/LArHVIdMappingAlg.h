/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHVIDMAPPINGALG_H
#define LARHVIDMAPPINGALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "LArRecConditions/LArHVIdMapping.h"

class LArHVIdMappingAlg: public AthAlgorithm {

public:
  using AthAlgorithm::AthAlgorithm;
  virtual ~LArHVIdMappingAlg() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

 private:

  SG::ReadCondHandleKey<AthenaAttributeList>   m_readKey{this, "ReadKey", "/LAR/IdentifierOfl/HVLineToElectrodeMap", "Cool folder with HV mapping"};
  SG::WriteCondHandleKey<LArHVIdMapping>  m_writeKey{this, "WriteKey","LArHVIdMap", "Key for output mapping object"} ;
  //bool m_isSuperCell;

};
#endif
