/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUEMULATION_TTETESTALG_H
#define TRIGTAUEMULATION_TTETESTALG_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "TrigTauEmulation/ILevel1EmulationTool.h"


class TTETestAlg : public AthAlgorithm

{

 public:
  
  TTETestAlg(const std::string& name, ISvcLocator* svcLoc);

  //Init alg
  virtual StatusCode initialize();
  
  //Exe algo
  virtual StatusCode execute();


 private:
  
  ToolHandle<TrigTauEmul::ILevel1EmulationTool> m_emul;


};


#endif
  
