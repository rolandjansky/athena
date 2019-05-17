// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_TAUSMEARINGTOOL_H
#define TAUANALYSISTOOLS_TAUSMEARINGTOOL_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  documentation in: ../README.rst
                    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/tags/TauAnalysisTools-<tag>/README.rst
		    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/README.rst
*/

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/AnaToolHandle.h"

// Local include(s):
#include "TauAnalysisTools/Enums.h"
#include "TauAnalysisTools/ITauSmearingTool.h"
#include "TauAnalysisTools/CommonSmearingTool.h"

namespace TauAnalysisTools
{

class TauSmearingTool
  : public asg::AsgTool
  , public virtual ITauSmearingTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( TauSmearingTool, TauAnalysisTools::ITauSmearingTool )

public:
  /// Create a constructor for standalone usage
  TauSmearingTool( const std::string& sName );

  ~TauSmearingTool();

  /// Function initialising the tool
  virtual StatusCode initialize();

  /// Apply the correction on a modifyable object
  virtual CP::CorrectionCode applyCorrection( xAOD::TauJet& xTau );
  /// Create a corrected copy from a constant tau
  virtual CP::CorrectionCode correctedCopy( const xAOD::TauJet& input,
      xAOD::TauJet*& output );

  /// returns: whether this tool is affected by the given systematis
  virtual bool isAffectedBySystematic( const CP::SystematicVariation& systematic ) const;

  /// returns: the list of all systematics this tool can be affected by
  virtual CP::SystematicSet affectingSystematics() const;

  /// returns: the list of all systematics this tool recommends to use
  virtual CP::SystematicSet recommendedSystematics() const;

  virtual CP::SystematicCode applySystematicVariation( const CP::SystematicSet& systConfig );

private:
  asg::AnaToolHandle<ITauSmearingTool> m_tCommonSmearingTool;
  std::string m_sInputFilePath;
  std::string m_sRecommendationTag;
  bool m_bIsData;
  bool m_bSkipTruthMatchCheck;
  bool m_bApplyFading;

  bool m_bApplyMVATES;
  bool m_bApplyMVATESQualityCheck;
  bool m_bApplyCombinedTES;
  bool m_bApplyInsituCorrection;

}; // class TauSmearingTool

} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_TAUSMEARINGTOOL_H
