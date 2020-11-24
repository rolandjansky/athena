// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
TauOverlappingElectronLLHDecorator depends on an EGamma tool which is
not avaible in AnalysisBase. To make TauAnalysisTools compiel, this
class is ignored in AnalysisBase.
*/
#ifndef XAOD_STANDALONE

#ifndef TAUANALYSISTOOLS_TAUOVERLAPPINGELECTRONLLHDECORATOR_H
#define TAUANALYSISTOOLS_TAUOVERLAPPINGELECTRONLLHDECORATOR_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  documentation in: https://gitlab.cern.ch/atlas/athena/-/blob/master/PhysicsAnalysis/TauID/TauAnalysisTools/doc/README-TauOverlappingElectronLLHDecorator.rst
*/

// Framework include(s):
#include "AsgTools/AsgMetadataTool.h"
#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"

// EDM include(s):
#include "xAODEgamma/ElectronContainer.h"

// Local include(s):
#include "TauAnalysisTools/ITauOverlappingElectronLLHDecorator.h"
#include "StoreGate/ReadHandleKey.h"

// ROOT include(s):
#include "TH2D.h"

namespace TauAnalysisTools
{

class SelectionCutEleOLR;

class TauOverlappingElectronLLHDecorator
  : public virtual ITauOverlappingElectronLLHDecorator
  , public asg::AsgMetadataTool
{

  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( TauOverlappingElectronLLHDecorator,
                  TauAnalysisTools::ITauOverlappingElectronLLHDecorator )

  friend class SelectionCutEleOLR;

public:
  /// Create a constructor for standalone usage
  TauOverlappingElectronLLHDecorator( const std::string& name );

  ~TauOverlappingElectronLLHDecorator( );

  /// Function initialising the tool
  virtual StatusCode initialize();

  virtual StatusCode decorate(const xAOD::TauJet& xTau) const;

  virtual StatusCode setEleOlrPassDecorationName(const std::string& name);
  virtual StatusCode setEleOlrLhScoreDecorationName(const std::string& name);

private:
  AsgElectronLikelihoodTool* m_tEMLHTool;
  SG::ReadHandleKey<xAOD::ElectronContainer> m_sElectronContainerName { this, "ElectronContainerName", "Electrons", "Input electron container"};
  bool m_bElectonsAvailable;
  std::string m_sEleOLRFilePath;
  TH2D* m_hCutValues;

private:
  float getCutVal(float fEta, float fPt) const;
  virtual StatusCode beginEvent();

private:
  mutable bool m_bEleOLRMatchAvailable;
  mutable bool m_bNewEvent;

  std::string m_sElectronPhotonSelectorToolsConfigFile;
  std::string m_sEleOlrPassDecorationName;
  std::string m_sEleOlrLhScoreDecorationName;

}; // class TauOverlappingElectronLLHDecorator

} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_TAUOVERLAPPINGELECTRONLLHDECORATOR_H

#endif
