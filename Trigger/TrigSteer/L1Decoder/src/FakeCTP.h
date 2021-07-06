// -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1Decoder_FakeCTP_h
#define L1Decoder_FakeCTP_h

#include <vector>
#include "TrigConfHLTUtils/HLTUtils.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"


class FakeCTP : public AthAlgorithm {
public:
  FakeCTP(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  
private:
  
  typedef std::vector< std::vector<TrigConf::HLTHash> > ChainsInTheEvent;
  // @brief returns names of the chains (inner vector) to activate for each event (outer vector)
  StatusCode parseInputFile();
  
  std::string m_inputFileName;
  SG::WriteHandle< xAOD::TrigCompositeContainer > m_decisions;
  SG::WriteHandle< xAOD::TrigCompositeAuxContainer > m_decisionsAux;
  
  ChainsInTheEvent m_events;
  size_t m_currentEvent;
};
#endif 


