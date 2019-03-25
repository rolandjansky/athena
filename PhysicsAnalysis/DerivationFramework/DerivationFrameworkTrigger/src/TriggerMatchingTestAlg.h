#ifndef TRIGGERMATCHINGTEST_TRIGGERMATCHINGTESTALG_H
#define TRIGGERMATCHINGTEST_TRIGGERMATCHINGTESTALG_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TriggerMatchingTool/IMatchingTool.h"
#include "xAODBase/ObjectType.h"

class TriggerMatchingTestAlg: public ::AthAlgorithm { 
 public: 
  TriggerMatchingTestAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TriggerMatchingTestAlg(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();


 private: 

  std::string m_chainName;

  std::map<xAOD::Type::ObjectType, std::string> m_inputColls;

  ToolHandle<Trig::IMatchingTool> m_oldMatchTool{
    "Trig::MatchingTool/TrigMatchingTool", this};
  ToolHandle<Trig::IMatchingTool> m_newMatchTool{
    "Trig::MatchFromCompositeTool/TrigMatchFromCompositeTool", this};

  std::map<xAOD::Type::ObjectType, std::size_t> m_legs;
  bool m_usesClusters = false;

}; 

#endif //> !TRIGGERMATCHINGTEST_TRIGGERMATCHINGTESTALG_H
