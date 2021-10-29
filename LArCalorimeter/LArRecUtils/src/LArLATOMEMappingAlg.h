//Dear emacs, this is -*- C++ -*- 

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARLATOMEMAPPINGALG_H
#define LARLATOMEMAPPINGALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "LArCabling/LArLATOMEMapping.h"

/**
 * @brief class to fill SC mapping object from conditions DB
 */

class LArLATOMEMappingAlg: public AthAlgorithm {

public:
  //Delegate constructor:
  using AthAlgorithm::AthAlgorithm;
  
  virtual ~LArLATOMEMappingAlg() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;


 private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey {this,"ReadKey","/LAR/IdentifierSC/LatomeMapping"};
  SG::WriteCondHandleKey<LArLATOMEMapping>  m_writeKey{this,"WriteKey","LArLATOMEMap"};
  ServiceHandle<ICondSvc> m_condSvc{this,"CondSvc","CondSvc"};
  Gaudi::Property<bool> m_isSuperCell{this,"isSuperCell",false};

};

#endif
