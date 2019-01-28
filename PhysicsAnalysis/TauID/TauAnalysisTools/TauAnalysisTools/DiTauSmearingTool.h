/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_DITAUSMEARINGTOOL_H
#define TAUANALYSISTOOLS_DITAUSMEARINGTOOL_H

/*
  author: David Kirchmeier
  mail: david.kirchmeier@cern.ch
  documentation in: ../doc/README-DiTauSmearingTool.rst
*/


// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/AnaToolHandle.h"

// Local include(s):
#include "TauAnalysisTools/IDiTauSmearingTool.h"
#include "TauAnalysisTools/CommonDiTauSmearingTool.h"

namespace TauAnalysisTools
{

class DiTauSmearingTool
  : public asg::AsgTool
  , public virtual IDiTauSmearingTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( DiTauSmearingTool, TauAnalysisTools::IDiTauSmearingTool )

public:
  /// Create a constructor for standalone usage
  DiTauSmearingTool( const std::string& sName );

  ~DiTauSmearingTool();

  /// Function initialising the tool
  virtual StatusCode initialize() override;

  /// Apply the correction on a modifyable object
  virtual CP::CorrectionCode applyCorrection( xAOD::DiTauJet& xDiTau ) override;
  /// Create a corrected copy from a constant ditau
  virtual CP::CorrectionCode correctedCopy( const xAOD::DiTauJet& input,
      xAOD::DiTauJet*& output ) override;

  /// returns: whether this tool is affected by the given systematis
  virtual bool isAffectedBySystematic( const CP::SystematicVariation& systematic ) const override;

  /// returns: the list of all systematics this tool can be affected by
  virtual CP::SystematicSet affectingSystematics() const override;

  /// returns: the list of all systematics this tool recommends to use
  virtual CP::SystematicSet recommendedSystematics() const override;

  virtual CP::SystematicCode applySystematicVariation( const CP::SystematicSet& systConfig ) override;

private:
  asg::AnaToolHandle<IDiTauSmearingTool> m_tCommonDiTauSmearingTool;
  std::string m_sInputFilePath;
  std::string m_sRecommendationTag;
  bool m_bSkipTruthMatchCheck;

}; // class DiTauSmearingTool

} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_DITAUSMEARINGTOOL_H
