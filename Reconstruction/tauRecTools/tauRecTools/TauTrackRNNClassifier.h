/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUTRACKRNNCLASSIFIER_H
#define TAUREC_TAUTRACKRNNCLASSIFIER_H

// ASG include(s)
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandleArray.h"

// xAOD include(s)
#include "xAODTau/TauJet.h"
#include "xAODTau/TauTrack.h"

// local include(s)
#include "tauRecTools/TauRecToolBase.h"
#include "tauRecTools/lwtnn/LightweightGraph.h"
#include "tauRecTools/lwtnn/parse_json.h"

/**
 * @brief Implementation of a TrackClassifier based on an MVA 
 * 
 * @author Max Maerker
 *                                                                              
 */

namespace tauRecTools
{
  
class TrackRNN;

// We currently allow several input types
// The "ValueMap" is for simple rank-1 inputs
typedef std::map<std::string, double> ValueMap;
// The "VectorMap" is for sequence inputs
typedef std::map<std::string, std::vector<double> > VectorMap;

typedef std::map<std::string, ValueMap> NodeMap;
typedef std::map<std::string, VectorMap> SeqNodeMap;

//______________________________________________________________________________
class TauTrackRNNClassifier
  : public TauRecToolBase
{
public:

  ASG_TOOL_CLASS2( TauTrackRNNClassifier, TauRecToolBase, ITauToolBase )

  TauTrackRNNClassifier(const std::string& sName="TauTrackRNNClassifier");
  ~TauTrackRNNClassifier();

  // retrieve all track classifier sub tools
  virtual StatusCode initialize() override;
 // pass all tracks in the tau cone to all track classifier sub tools
  virtual StatusCode executeRNNTrackClassifier(xAOD::TauJet& pTau, xAOD::TauTrackContainer& tauTrackContainer) override;

 private:
  ToolHandleArray<TrackRNN> m_vClassifier {this, "Classifiers", {}};
}; // class TauTrackRNNClassifier
  
//______________________________________________________________________________
class TrackRNN
  : public TauRecToolBase
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS2( TrackRNN,
                   TauRecToolBase,
		   ITauToolBase)
  
  public:
  
  TrackRNN(const std::string& sName);
  ~TrackRNN();

  // configure the MVA object and build a general map to store variables
  // for possible MVA inputs. Only Variables defined in the root weights file
  // are passed to the MVA object
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  
  // executes MVA object to get the BDT score, makes the decision and resets
  // classification flags
  StatusCode classifyTracks(std::vector<xAOD::TauTrack*> vTracks, xAOD::TauJet& xTau);
  
private:
  // set BDT input variables in the corresponding map entries
  StatusCode setVars(const std::vector<xAOD::TauTrack*> vTracks, const xAOD::TauJet& xTau);
  StatusCode resetVars();

  // equivalent for trigger
  // TRIGGER NOT YET IMPLEMENTET!
  //StatusCode classifyTriggerTrack(xAOD::TauTrack& vTrack, const xAOD::TauJet& xTau, const xAOD::TauTrack* lead_track, double mu);
  //StatusCode setTriggerVars(const xAOD::TauTrack& xTrack, const xAOD::TauJet& xTau, const xAOD::TauTrack* lead_track);

  // load the root weights file and configure the MVA object with the correct
  // variable addresses
  StatusCode addWeightsFile();
  
private:
  // configurable variables
  std::string m_sInputWeightsPath; 
  unsigned int m_nMaxNtracks;

  

private:
  VectorMap m_valueMap; //!
  lwtDev::LightweightGraph* m_RNNClassifier; //!
  lwtDev::GraphConfig m_NNconfig; //!

  std::vector<double> m_vClassProb; //!

}; // class TrackRNN

} // namespace tauRecTools

#endif // not TAUTRACKCLASSIFIER
