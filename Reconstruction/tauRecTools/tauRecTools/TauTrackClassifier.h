/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUTRACKCLASSIFIER_H
#define TAUREC_TAUTRACKCLASSIFIER_H

// ASG include(s)
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandleArray.h"

// xAOD include(s)
#include "xAODTau/TauJet.h"
#include "xAODTau/TauTrack.h"

// local include(s)
#include "tauRecTools/TauRecToolBase.h"

#include "MVAUtils/BDT.h"

/**
 * @brief Implementation of a TrackClassifier based on an MVA 
 * 
 * @author Dirk Duschinger
 *                                                                              
 */

namespace tauRecTools
{
  
class TrackMVABDT;
  
//______________________________________________________________________________
class TauTrackClassifier
  : public TauRecToolBase
{
public:

  ASG_TOOL_CLASS2( TauTrackClassifier, TauRecToolBase, ITauToolBase )

  TauTrackClassifier(const std::string& sName="TauTrackClassifier");
  ~TauTrackClassifier();

  virtual StatusCode initialize() override;
  virtual StatusCode executeTrackClassifier(xAOD::TauJet& pTau, xAOD::TauTrackContainer& tauTrackContainer ) const override;

private:
  ToolHandleArray<TrackMVABDT> m_vClassifier;
  std::vector<std::string> m_vClassifierNames;//optional
  
}; // class TauTrackClassifier
  
//______________________________________________________________________________
class TrackMVABDT
  : public TauRecToolBase
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS2( TrackMVABDT,
                   TauRecToolBase,
		   ITauToolBase)
  
  public:
  
  TrackMVABDT(const std::string& sName);
  ~TrackMVABDT();

  // configure the MVA object and build a general map to store variables
  // for possible MVA inputs. Only Variables defined in the root weights file
  // are passed to the MVA object
  StatusCode initialize() override;
  
  // executes MVA object to get the BDT score, makes the decision and resets
  // classification flags
  StatusCode classifyTrack(xAOD::TauTrack& xTrack, const xAOD::TauJet& xTau) const;
  // calculate all input variables
  StatusCode calculateVariables(const xAOD::TauTrack& xTrack, const xAOD::TauJet& xTau, std::vector<float>& values) const;

  // load the root weights file and configure the MVA object with the correct
  // variable addresses
  StatusCode addWeightsFile();
  
private:
  // configurable variables
  std::string m_sInputWeightsPath; 
  float m_fThreshold;
  int m_iSignalType;
  int m_iBackgroundType;
  int m_iExpectedFlag;
  
  std::unique_ptr<MVAUtils::BDT> m_rReader; //!

  std::vector<TString> m_inputVariableNames; //!

}; // class TrackMVABDT

} // namespace tauRecTools

#endif // not TAUTRACKCLASSIFIER
