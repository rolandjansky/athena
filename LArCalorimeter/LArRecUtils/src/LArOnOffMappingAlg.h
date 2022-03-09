//Dear emacs, this is -*- C++ -*- 

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRECCONDITIONS_LARONOFFMAPPINGALG_H
#define LARRECCONDITIONS_LARONOFFMAPPINGALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "LArCabling/LArOnOffIdMapping.h"

class LArOnOffMappingAlg: public AthAlgorithm {

public:
  //Delegate constructor:
  using AthAlgorithm::AthAlgorithm;
  
  virtual ~LArOnOffMappingAlg() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;


 private:
  SG::ReadCondHandleKey<AthenaAttributeList> m_readKey {this,"ReadKey","/LAr/Identifier/OnOnffMap"};
  SG::WriteCondHandleKey<LArOnOffIdMapping>  m_writeKey{this,"WriteKey","LArOnOffIdMap"};
  Gaudi::Property<bool> m_isSuperCell{this,"isSuperCell",false};
};



#endif
