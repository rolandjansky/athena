/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUORL_H
#define TRIGTAUORL_H

//Framework includes
#include "AsgTools/AsgTool.h"

//Local includes
#include "TrigTauEmulation/ITrigTauORLTool.h"

class TrigTauORLTool : public virtual ITrigTauORLTool, public asg::AsgTool
{
  ASG_TOOL_CLASS(TrigTauORLTool, ITrigTauORLTool)
  
 public:

  TrigTauORLTool(const std::string& name);
  TrigTauORLTool(const TrigTauORLTool& other);
  TrigTauORLTool& operator=(const TrigTauORLTool& other) = delete;
  ~TrigTauORLTool() {} 

  virtual StatusCode initialize();

  virtual StatusCode execute(const xAOD::EmTauRoIContainer* c1, 
			     const xAOD::EmTauRoIContainer* c2,
			     const std::string & sel1 = "",
			     const std::string & sel2 = "");

  virtual StatusCode execute(const xAOD::EmTauRoIContainer* c1, 
			     const xAOD::JetRoIContainer* c2,
			     const std::string & sel1 = "",
			     const std::string & sel2 = "");

  virtual StatusCode execute(const xAOD::EmTauRoIContainer* c1, 
			     const xAOD::EmTauRoIContainer* c2, 
			     const xAOD::JetRoIContainer* c3,
			     const std::string & sel1 = "",
			     const std::string & sel2 = "",
			     const std::string & sel3 = "");

  virtual bool decision(const std::string & item);

 private:
  
  std::map<std::string, bool> m_orl_decisions;

};

#endif
