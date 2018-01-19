/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DITAUREC_HADELBUILDER_H
#define DITAUREC_HADELBUILDER_H

#include "AsgTools/AsgTool.h"

#include "DiTauRec/ILepHadBuilder.h"

namespace DiTauRec
{
  
class HadElBuilder
  : public DiTauRec::ILepHadBuilder
  , public asg::AsgTool
{

  ASG_TOOL_CLASS( HadElBuilder,
		  DiTauRec::ILepHadBuilder )
 
public:

  HadElBuilder( const std::string& name );

  virtual ~HadElBuilder() override;

  virtual StatusCode initialize() override;

  virtual StatusCode initializeEvent() override;

  virtual StatusCode execute() override;
private:
  std::string m_sHadHadDiTauContainerName;
  std::string m_sDiTauHadElContainerName;
  std::string m_sDiTauHadElAuxContainerName;
  std::string m_sElectronContainerName;
  int m_iElectronMinPt;
};
}
#endif // DITAUREC_HADELBUILDER_H
