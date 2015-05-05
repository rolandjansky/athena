/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "HepMC/IO_GenEvent.h"

class StoreGateSvc;

class ReadHepEvtFromAscii:public AthAlgorithm {
public:
  ReadHepEvtFromAscii(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  StoreGateSvc* m_sgSvc;
  HepMC::IO_GenEvent* m_ascii_in;
  
  // Setable Properties:-
  std::string m_key; 
  std::string m_input_file;
  
};
