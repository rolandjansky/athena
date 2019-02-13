/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

  // retrieve all track classifier sub tools
  virtual StatusCode initialize();
  // pass all tracks in the tau cone to all track classifier sub tools
  virtual StatusCode execute(xAOD::TauJet& pTau);
  virtual StatusCode executeShotFinder(xAOD::TauJet&, xAOD::CaloClusterContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
  virtual StatusCode executePi0CreateROI(xAOD::TauJet&, CaloCellContainer&) { return StatusCode::SUCCESS; }
  virtual StatusCode executePi0ClusterCreator(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&, xAOD::CaloClusterContainer&) { return StatusCode::SUCCESS; }
  virtual StatusCode executeVertexVariables(xAOD::TauJet&, xAOD::VertexContainer&) { return StatusCode::SUCCESS; }
  virtual StatusCode executePi0ClusterScaler(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
  virtual StatusCode executePi0nPFO(xAOD::TauJet&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
  virtual StatusCode executePanTau(xAOD::TauJet&, xAOD::ParticleContainer&) { return StatusCode::SUCCESS; }

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
  StatusCode initialize();
  StatusCode executeShotFinder(xAOD::TauJet&, xAOD::CaloClusterContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
  StatusCode executePi0CreateROI(xAOD::TauJet&, CaloCellContainer&) { return StatusCode::SUCCESS; }
  StatusCode executePi0ClusterCreator(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&, xAOD::CaloClusterContainer&) { return StatusCode::SUCCESS; }
  StatusCode executeVertexVariables(xAOD::TauJet&, xAOD::VertexContainer&) { return StatusCode::SUCCESS; }
  StatusCode executePi0ClusterScaler(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
  StatusCode executePi0nPFO(xAOD::TauJet&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
  StatusCode executePanTau(xAOD::TauJet&, xAOD::ParticleContainer&) { return StatusCode::SUCCESS; }
  StatusCode finalize();
  
  // executes MVA object to get the BDT score, makes the decision and resets
  // classification flags
  StatusCode classifyTrack(xAOD::TauTrack& xTrack, const xAOD::TauJet& xTau);
  // set BDT input variables in the corresponding map entries
  StatusCode setVars(const xAOD::TauTrack& xTrack, const xAOD::TauJet& xTau);

  // load the root weights file and configure the MVA object with the correct
  // variable addresses
  StatusCode addWeightsFile();
  // parse the TNamed object in the root file for the line showing the input variable used by that
  // particular BDT names and store them
  StatusCode parseVariableContent();
  
private:
  // configurable variables
  std::string m_sInputWeightsPath; 
  float m_fThreshold;
  int m_iSignalType;
  int m_iBackgroundType;
  int m_iExpectedFlag;
  
private:
  MVAUtils::BDT* m_rReader; //!
  
  //  std::map<int, std::string> m_mParsedVarsBDT; //!
  std::map<TString, float*> m_mAvailableVars; //!
  inline float& setVar(const TString& var) { return *(m_mAvailableVars[var]); } //!< not-stateless, many such examples need to be fixed for r22
  std::vector<float*> m_vars; //!< points to floats in m_mAvailableVars that are used in BDT

}; // class TrackMVABDT

} // namespace tauRecTools

#endif // not TAUTRACKCLASSIFIER
