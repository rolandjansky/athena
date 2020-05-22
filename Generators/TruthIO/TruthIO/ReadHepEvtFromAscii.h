/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AtlasHepMC/IO_GenEvent.h"

class StoreGateSvc;

class ReadHepEvtFromAscii:public AthAlgorithm {
public:
  ReadHepEvtFromAscii(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  StoreGateSvc* m_sgSvc;
  
  // Setable Properties:-
  std::string m_key; 
  std::string m_input_file;
  std::ifstream m_file;
  bool read_hepevt_particle( int i);
  bool read_hepevt_event_header();
  
};
