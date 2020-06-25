//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBADFEBCONDALG_H
#define LARBADFEBCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "LArRecConditions/LArBadChannelCont.h"

class LArBadFebCondAlg: public AthAlgorithm {
 public:
  //Delegate to base-class ctor
  using AthAlgorithm::AthAlgorithm;
  virtual ~LArBadFebCondAlg()=default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override {return StatusCode::SUCCESS;}

 private:
  SG::ReadCondHandleKey<AthenaAttributeList> m_BCInputKey{this,"ReadKey","/LAR/BadFebsOfl/BadFebs",
      "Key of input CDO (AttributeList)"}; 
  SG::WriteCondHandleKey<LArBadFebCont> m_BCOutputKey{this,"WriteKey","LArBadFeb","Key of output CDO"};

  ServiceHandle<ICondSvc> m_condSvc{this,"CondSvc","CondSvc"};
  Gaudi::Property<std::string> m_inputFileName{this,"InputFileName","",
      "Optional file containing (supplemental) bad Febs"};
};



#endif
