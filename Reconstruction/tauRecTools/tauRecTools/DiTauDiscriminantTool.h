// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TAURECTOOLS_DITAUDISRIMINANTTOOL_H
#define  TAURECTOOLS_DITAUDISRIMINANTTOOL_H

/**
 * @brief Implementation of boosted di-tau ID.
 * 
 * @author David Kirchmeier (david.kirchmeier@cern.ch)
 *                                                                              
 */

// Framework include(s):
#include "AsgTools/AsgTool.h"

// Core include(s):

// EDM include(s):
#include "xAODTau/DiTauJetContainer.h"

// Local include(s):
#include "tauRecTools/IDiTauToolBase.h"

// MVAUtils includes
#include "MVAUtils/BDT.h"

#include <string>
#include <map>


namespace tauRecTools
{


class DiTauDiscriminantTool
  : public tauRecTools::IDiTauToolBase
  , public asg::AsgTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( DiTauDiscriminantTool,
                  tauRecTools::IDiTauToolBase )

public:

  DiTauDiscriminantTool( const std::string& name );

  virtual ~DiTauDiscriminantTool();

  // initialize the tool
  virtual StatusCode initialize() override;

  // get ID score depricated
  double getJetBDTScore(const xAOD::DiTauJet& xDiTau);

  // calculate and decorate BDTJetScore
  virtual StatusCode execute(const xAOD::DiTauJet& xDiTau) override;
  
  // get decay mode
  virtual std::string getDecayMode() override;
  
private:

  StatusCode parseWeightsFile();

  void setIDVariables(const xAOD::DiTauJet& xDiTau);

  // const xAOD::DiTauJetContainer* m_xDiTauContainer;

  // steering variables
  std::string m_sWeightsFile;
  std::string m_sBDTScoreName;
  
  MVAUtils::BDT* m_bdt; //!

  std::map<TString, float*> m_mIDVariables; //!
  std::map<TString, float*> m_mIDSpectators; //!

  inline float& setVar(const TString& var) { return *(m_mIDVariables[var]); } //!< not-stateless, many such examples need to be fixed for r22

  std::vector<std::string> m_vVarNames;
private:
  enum DecayMode{ HadHad, HadMu, HadEl, Default };
  std::string m_sDecayMode;
  DecayMode m_eDecayMode;
}; // class DiTauDiscriminantTool

}
#endif // TAURECTOOLS_DITAUDISRIMINANTTOOL_H
