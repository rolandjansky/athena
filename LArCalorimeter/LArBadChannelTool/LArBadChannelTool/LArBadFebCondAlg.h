//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBADFEBCONDALG_H
#define LARBADFEBCONDALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "LArRecConditions/LArBadChannelCont.h"

class LArBadFebCondAlg: public AthReentrantAlgorithm {
 public:
  //Delegate to base-class ctor
  using AthReentrantAlgorithm::AthReentrantAlgorithm;
  virtual ~LArBadFebCondAlg()=default;

  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;
  virtual StatusCode finalize() override final{return StatusCode::SUCCESS;}
  virtual bool isReEntrant() const override final { return false; }

 private:
  SG::ReadCondHandleKey<AthenaAttributeList> m_BCInputKey{this,"ReadKey","/LAR/BadFebsOfl/BadFebs",
      "Key of input CDO (AttributeList)"}; 
  SG::WriteCondHandleKey<LArBadFebCont> m_BCOutputKey{this,"WriteKey","LArBadFeb","Key of output CDO"};

  Gaudi::Property<std::string> m_inputFileName{this,"InputFileName","",
      "Optional file containing (supplemental) bad Febs"};
};



#endif
