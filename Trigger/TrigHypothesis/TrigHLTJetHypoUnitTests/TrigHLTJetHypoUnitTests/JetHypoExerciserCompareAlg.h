// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPOUNITTESTS_JETHYPOEXERCISERCOMPAREALG_H
#define TRIGHLTJETHYPOUNITTESTS_JETHYPOEXERCISERCOMPAREALG_H

#include "TrigHLTJetHypo/ITrigJetHypoToolHelperMT.h"
#include "TrigHLTJetHypoUnitTests/IHypoJetVectorGenerator.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/Property.h"


class JetHypoExerciserCompareAlg : public AthAlgorithm {
public:
  JetHypoExerciserCompareAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~JetHypoExerciserCompareAlg();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();


private:


  // ToolHandles as Properties
  ToolHandle<ITrigJetHypoToolHelperMT> m_helper0  {this, "JetHypoHelperTool0", 
      "Tool0", "private IJetTrigHypoToolHelper0"};

  ToolHandle<ITrigJetHypoToolHelperMT> m_helper1  {this, "JetHypoHelperTool1", 
      "Tool1", "private IJetTrigHypoToolHelper1"};

  ToolHandle<IHypoJetVectorGenerator> m_generator  {this, "event_generator", 
      {}, "Generates test vector"};

  
  Gaudi::Property<bool>
  m_visitDebug {this, "visit_debug", false, "debug flag"};
  
  Gaudi::Property<std::string>
  m_logname {this, "logname", "JetHypoExerciser.log", "debug info output fn"};

  std::size_t m_ncall{0};

  StatusCode initialize_(const ToolHandle<ITrigJetHypoToolHelperMT>&,
			 const std::string&);
  
  StatusCode execute_(ToolHandle<ITrigJetHypoToolHelperMT>&,
		      HypoJetVector&,
		      const std::string& collectorName,
		      const std::string& logname,
		      bool& pass);
};

#endif // !#define TRIGHLTJETHYPOUNITTESTS_JETHYPOEXERCISER_H
