/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypoUnitTests/JetHypoExerciserAlg.h"
#include "TrigHLTJetHypo/ITrigJetHypoToolHelper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "TrigHLTJetHypoUnitTests/../src/TLorentzVectorFactory.h"
#include "TrigHLTJetHypoUnitTests/../src/TLorentzVectorAsIJet.h"
#include "TrigHLTJetHypo/../src/DebugInfoCollector.h"
#include "TrigHLTJetHypo/../src/xAODJetCollector.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/lineSplitter.h"

#include <fstream>
#include <sstream>

std::ostream& operator<<( std::ostream& s, std::pair<double, double > p )
{
  s << p.first << " " << p.second;
  return s;
}

JetHypoExerciserAlg::JetHypoExerciserAlg(const std::string& name,
					 ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator){
}

JetHypoExerciserAlg::~JetHypoExerciserAlg(){}


StatusCode JetHypoExerciserAlg::initialize() {

  ATH_MSG_INFO ("initialize()");

  
  ATH_MSG_INFO 
    ("  " << m_helper.propertyName() 
     << " = " << m_helper.type());

  if ( m_helper.retrieve().isFailure() ) {
    ATH_MSG_FATAL 
      (m_helper.propertyName() << ": Failed to retrieve tool "
       << m_helper.type());
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO 
      (m_helper.propertyName() << ": Retrieved tool " 
       << m_helper.type());
  }

  DebugInfoCollector collector(name());
  CHECK(m_helper->getDescription(collector));
  auto s = collector.toString();
  s += '\n' + m_generator->toString();
  
  for(const auto& l : lineSplitter(s)){
    ATH_MSG_INFO(l);
  }
  
  if (m_visitDebug){
    std::ofstream outfile;
    outfile.open(m_logname);
    outfile << s;   
  }

  return StatusCode::SUCCESS;  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode JetHypoExerciserAlg::execute() {

  // Part 1: print where you are
  ATH_MSG_INFO ("execute()");
  ++m_ncall;

  // make a HypoJet, add to vector and send to helper tool
  
  HypoJetVector jv = m_generator->get();
  
  std::unique_ptr<ITrigJetHypoInfoCollector> debugInfoCollector(nullptr);
  if(m_visitDebug){
    auto collectorName = name() + "debugInfoCollector";
    debugInfoCollector.reset(new  DebugInfoCollector(collectorName));
  } 

  xAODJetCollector jetCollector;

  ATH_MSG_INFO ("Event pass "
                <<  m_helper->pass(jv, jetCollector, debugInfoCollector));

  std::stringstream ss;
  ss <<  "========= ncall: "<<m_ncall << "==========\n";
  ss << debugInfoCollector->toString() + '\n';
  auto s = ss.str();
  std::ofstream outfile;
  outfile.open(m_logname, std::ios_base::app);
  outfile << s;
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode JetHypoExerciserAlg::finalize() {

  // Part 1: print where you are
  ATH_MSG_INFO ("finalize()");

  return StatusCode::SUCCESS;
}

