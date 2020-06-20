//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBADCHANNELCONDALG_H
#define LARBADCHANNELCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "LArCabling/LArOnOffIdMapping.h"

class LArBadChannelCondAlg: public AthAlgorithm {
 public:
  //Delegate to base-class ctor
  using AthAlgorithm::AthAlgorithm;

  virtual ~LArBadChannelCondAlg()=default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override {return StatusCode::SUCCESS;}

 private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_BCInputKey{this,"ReadKey","/LAR/BadChannelsOfl/BadChannels",
      "Key of input CDO (AttrListCollection)"}; 
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","Key of cabling CDO"};   
  SG::WriteCondHandleKey<LArBadChannelCont> m_BCOutputKey{this,"WriteKey","LArBadChannel","Key of output CDO"};
  ServiceHandle<ICondSvc> m_condSvc{this,"CondSvc","CondSvc"};
  Gaudi::Property<std::string> m_inputFileName{this,"InputFileName","",
      "Optional file containing (supplemental) bad channels"};
};



#endif
