/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUDISCRIMINANTPROCESSOR_H
#define TAUDISCRIMINANTPROCESSOR_H
//#ifdef XAOD_STANDALONE

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "tauRecTools/TauRecToolBase.h"
#include "xAODTau/TauJet.h"
#include "PATInterfaces/CorrectionCode.h"

class TauDiscriminantProcessor : public asg::AsgTool
{
 public:

  TauDiscriminantProcessor( const std::string& name );

  ~TauDiscriminantProcessor();

  StatusCode initialize();

  StatusCode eventInitialize();
  
  CP::CorrectionCode applyDiscriminant(xAOD::TauJet& tau);

  void setToolArray(const ToolHandleArray<TauRecToolBase>& array) {m_tools = array; }
  
 private:
  ToolHandleArray<TauRecToolBase> m_tools;
};

//#endif //XAOD_STANDALONE
#endif
