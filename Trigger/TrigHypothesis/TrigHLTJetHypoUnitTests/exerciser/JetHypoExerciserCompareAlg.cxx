/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypoUnitTests/JetHypoExerciserCompareAlg.h"
#include "TrigHLTJetHypo/ITrigJetHypoToolHelperMT.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "TrigHLTJetHypoUnitTests/../src/TLorentzVectorFactory.h"
#include "TrigHLTJetHypoUnitTests/../src/TLorentzVectorAsIJet.h"
#include "TrigHLTJetHypo/../src/DebugInfoCollector.h"
#include "TrigHLTJetHypo/../src/xAODJetCollector.h"
#include "TrigHLTJetHypo/../src/JetTrigTimer.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/lineSplitter.h"

#include <fstream>
#include <sstream>


JetHypoExerciserCompareAlg::JetHypoExerciserCompareAlg(const std::string& name,
						       ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator){
}

JetHypoExerciserCompareAlg::~JetHypoExerciserCompareAlg(){}


StatusCode
JetHypoExerciserCompareAlg::initialize_(const ToolHandle<ITrigJetHypoToolHelperMT>& helper,
					const std::string& logname) {
  
  ATH_MSG_INFO ("initialize_()");

  ATH_MSG_INFO 
    ("  " << helper.propertyName() 
     << " = " << helper.type());

  if ( helper.retrieve().isFailure() ) {
    ATH_MSG_FATAL 
      (helper.propertyName() << ": Failed to retrieve tool "
       << helper.type());
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO 
      (helper.propertyName() << ": Retrieved tool " 
       << helper.type());
  }

  DebugInfoCollector collector(name());
  CHECK(helper->getDescription(collector));
  auto s = collector.toString();
  s += '\n' + m_generator->toString();
  
  for(const auto& l : lineSplitter(s)){
    ATH_MSG_INFO(l);
  }
  
  if (m_visitDebug){
    std::ofstream outfile;
    outfile.open(logname);
    outfile << s;   
  }

  return StatusCode::SUCCESS;
}

StatusCode JetHypoExerciserCompareAlg::initialize() {
  
  ATH_MSG_INFO ("initialize()");
  auto logname = m_logname + "_config0.log";
  auto sc = initialize_(m_helper0, logname);
  if(!(sc == StatusCode::SUCCESS)){return sc;}

  logname = m_logname + "_config1.log";
  return initialize_(m_helper1, logname);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode
JetHypoExerciserCompareAlg::execute_(ToolHandle<ITrigJetHypoToolHelperMT>& helper,
				     HypoJetVector& jv,
				     const std::string& collectorName,
				     const std::string& logname,
				     bool& pass) {
  
  // make a HypoJet, add to vector and send to helper tool
  
  std::unique_ptr<ITrigJetHypoInfoCollector> debugInfoCollector(nullptr);
  if(m_visitDebug){
    debugInfoCollector.reset(new  DebugInfoCollector(collectorName));
  } 

  if (debugInfoCollector){
    std::stringstream ss0;
    ss0 << "n jets " << jv.size() << '\n';
    debugInfoCollector-> collect("JetHypoExerciserCompareAlg", ss0.str());


    std::stringstream ss1;
    ss1 <<'\n';
    for(const auto& j : jv){
      ss1 << "e: " << j->e() << " et " << j->et() << " eta " << j->eta()
	  << '\n';
    }
    debugInfoCollector-> collect("JetHypoExerciserCompareAlg", ss1.str());
  }
  
  xAODJetCollector jetCollector;

  pass = helper->pass(jv, jetCollector, debugInfoCollector);

  std::stringstream ss;
  ss <<  "========= ncall: "<<m_ncall << "==========\n";
  if(debugInfoCollector){
    ss << debugInfoCollector->toString() + '\n';
  }
  auto s = ss.str();
  std::ofstream outfile;
  // outfile.open(logname, std::ios_base::app);
  outfile.open(logname);
  outfile << s;
  outfile.close();
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode JetHypoExerciserCompareAlg::execute() {
  
  // Part 1: print where you are
  ATH_MSG_INFO ("execute()");
  ++m_ncall;

  // make a HypoJet, add to vector and send to helper tool
  
  HypoJetVector jv = m_generator->get();
  
  auto timer = JetTrigTimer();
  std::string collectorName = name() + "debugInfoCollector0";
  std::string logname = m_logname + "_ev_" + std::to_string(m_ncall) + "tool0";
  bool pass0{false};

  timer.start();
  execute_(m_helper0, jv, collectorName, logname, pass0);
  timer.stop();
  
  if(!m_visitDebug){
    std::stringstream ss;
    ss << timer.elapsed() <<  " tool0 " << std::boolalpha << pass0 << '\n';
    timer.reset();
    std::ofstream outfile;
    outfile.open(logname);
    outfile << ss.str();
    outfile.close();
  }
  
  collectorName = name() + "debugInfoCollector1";
  logname = m_logname + "_ev_" + std::to_string(m_ncall) + "tool1";
  bool pass1;
  timer.start();
  execute_(m_helper1, jv, collectorName, logname, pass1);
  timer.stop();

  if(!m_visitDebug){
    std::stringstream ss;
    ss << timer.elapsed() <<  " tool1 "<< std::boolalpha << pass1 << '\n';
    timer.reset();
    std::ofstream outfile;
    outfile.open(logname);
    outfile << ss.str();
    outfile.close();
  }

  
  std::string spass0 = (pass0 ? "True": "False");
  std::string spass1 = (pass1 ? "True": "False");
  if (pass0 != pass1){
    ++m_differ;
    ATH_MSG_INFO ("hypo helper tools DISAGREE: "
		  << "pass 0 "<< spass0 << " pass 1 " << spass1);
  } else {
    if(pass0){

      ++m_agree_true;
    } else {
      ++m_agree_false;
    }
    ATH_MSG_INFO ("hypo helper tools agree on result which is: " << spass0);
  }

    
  for (auto p : jv) {delete p;}
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode JetHypoExerciserCompareAlg::finalize() {

  // Part 1: print where you are
  ATH_MSG_INFO ("finalize()");
  ATH_MSG_INFO ("agree and false: " << m_agree_false <<
		" agree and true: " << m_agree_true <<
		" differ: "       << m_differ
		);

  return StatusCode::SUCCESS;
}

