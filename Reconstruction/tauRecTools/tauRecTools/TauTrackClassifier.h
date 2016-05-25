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

// ROOT include(s)
#include "TMVA/Reader.h"

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
  : virtual public TauRecToolBase
{
public:

  ASG_TOOL_CLASS2( TauTrackClassifier, TauRecToolBase, ITauToolBase )

  TauTrackClassifier(const std::string& sName="TauTrackClassifier");
  ~TauTrackClassifier();

  // retrieve all track classifier sub tools
  virtual StatusCode initialize();
  // pass all tracks in the tau cone to all track classifier sub tools
  virtual StatusCode execute(xAOD::TauJet& pTau);

private:
  ToolHandleArray<TrackMVABDT> m_vClassifier;
  std::string m_tauTrackConName;

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

  // configure the TMVA reader object and build a general map to store variables
  // for possible TMVA inputs. Only Variables defined in the xml weights file
  // are passed to the TMVA reader
  StatusCode initialize();
  
  // executes TMVA reader to get the BDT score, makes the decision and resets
  // classification flags
  StatusCode classifyTrack(xAOD::TauTrack& xTrack, const xAOD::TauJet& xTau);
  // set BDT input variables in the corresponding map entries
  void setVars(const xAOD::TauTrack& xTrack, const xAOD::TauJet& xTau);

  // load the xml weights file and configure the TMVA reader with the correct
  // variable addresses
  StatusCode addWeightsFile();
  // parse the weights file for the line showing the input variable used by that
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
  TMVA::Reader* m_rReader; //!

  std::map<int, std::string> m_mParsedVarsBDT; //!
  std::map<std::string, float> m_mAvailableVars; //!

  // possible bdt input variables
  float tauPt;
  float tauEta;
  float trackEta;
  float z0sinThetaTJVA;
  float rConv;
  float rConvII;
  float DRJetSeedAxis;
  float d0;
  float qOverP;
  float theta;
  float numberOfInnermostPixelLayerHits;
  float numberOfPixelHits;
  float numberOfPixelDeadSensors;
  float numberOfPixelSharedHits;
  float numberOfSCTHits;
  float numberOfSCTDeadSensors;
  float numberOfSCTSharedHits;
  float numberOfTRTHighThresholdHits;
  float numberOfTRTHits;
  float nPixHits;
  float nSiHits;
  
}; // class TrackMVABDT


 xAOD::TauTrack::TrackFlagType isolateClassifiedBits(xAOD::TauTrack::TrackFlagType flag){
   static int flagsize=sizeof(flag)*8;
   flag=flag<<(flagsize-xAOD::TauJetParameters::classifiedFake-1);
   flag=flag>>(flagsize-xAOD::TauJetParameters::classifiedCharged+1);
   return flag;
 }

//bool sortTracks(xAOD::TauTrack* xTrack1, xAOD::TauTrack* xTrack2)
 bool sortTracks(const ElementLink<xAOD::TauTrackContainer> &l1, const ElementLink<xAOD::TauTrackContainer> &l2)
{
  //should we be safe and ask if the links are available?
  const xAOD::TauTrack* xTrack1 = *l1;
  const xAOD::TauTrack* xTrack2 = *l2;

  //return classified charged, then isolation (wide tracks), else by pt
  xAOD::TauTrack::TrackFlagType f1 = isolateClassifiedBits(xTrack1->flagSet());
  xAOD::TauTrack::TrackFlagType f2 = isolateClassifiedBits(xTrack2->flagSet());

  if(f1==f2)
    return xTrack1->pt()>xTrack2->pt();
  return f1<f2;
  
  //this somehow causes a crash
  /* static uint16_t flag1 = xTrack1->flagSet() >> (xAOD::TauJetParameters::classifiedCharged - 1); */
  /* static uint16_t flag2 = xTrack2->flagSet() >> (uint16_t(xAOD::TauJetParameters::classifiedCharged) - 1); */
  /* return (flag1<flag2) ||                            // sort by type, true tracks first */
  /*   ((flag1==flag2) && (xTrack1->pt()>xTrack2->pt())); // sort by pt if equal types */
}

} // namespace tauRecTools

#endif // not TAUTRACKCLASSIFIER
