//Dear emacs, this is -*- C++ -*- 

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRECCONDITIONS_LARFEBRODMAPPINGALG_H
#define LARRECCONDITIONS_LARFEBRODMAPPINGALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "LArRecConditions/LArFebRodMapping.h"

class LArFebRodMappingAlg: public AthAlgorithm {

public:

  using AthAlgorithm::AthAlgorithm;

  virtual ~LArFebRodMappingAlg() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;


 private:
  SG::ReadCondHandleKey<AthenaAttributeList> m_readKey  {this,"ReadKey","/LAR/Identifier/FebRodMap"};
  SG::WriteCondHandleKey<LArFebRodMapping>   m_writeKey {this,"WriteKey","LArFebRodMap"};
};



#endif
