// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPOUNITTESTS_JETHYPOEXERCISERALG_H
#define TRIGHLTJETHYPOUNITTESTS_JETHYPOEXERCISERALG_H

#include "TrigHLTJetHypo/ITrigJetHypoToolHelperMT.h"
#include "TrigHLTJetHypoUnitTests/IHypoJetVectorGenerator.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "Gaudi/Property.h"


class JetHypoExerciserAlg : public AthAlgorithm {
public:
  JetHypoExerciserAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~JetHypoExerciserAlg();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();


private:


  // ToolHandles as Properties
  ToolHandle<ITrigJetHypoToolHelperMT> m_helper  {this, "JetHypoHelperTool", 
      {}, "private IJetTrigHypoToolHelper"};

  ToolHandle<IHypoJetVectorGenerator> m_generator  {this, "event_generator", 
      {}, "Generates test vector"};

  
  Gaudi::Property<bool>
  m_visitDebug {this, "visit_debug", false, "debug flag"};
  
  Gaudi::Property<std::string>
  m_logname {this, "logname", "JetHypoExerciser.log", "debug info output fn"};

  std::size_t m_ncall{0};
};

#endif // !#define TRIGHLTJETHYPOUNITTESTS_JETHYPOEXERCISER_H
