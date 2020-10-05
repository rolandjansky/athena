/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetFex.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// ************************************************

#ifndef TRIGBJETFEX_H
#define TRIGBJETFEX_H

#include "FexAlgo.h"
#include "TrigTrackJetFinderTool.h"
#include "MsgStream.h"

#include "TrigBjetHypo/TrigBjetTagger.h"
#include "TrigBjetHypo/TuningLikelihood.h"
#include "TrigBjetHypo/TrigBjetDataHelper.h"

//#include "TrigInDetToolInterfaces/ITrigTrackJetFinderTool.h"
//#include "VxSecVertex/VxSecVertexInfo.h"

class TaggerHelper;
class TrigBjetTrackInfo;
class TrigBjetPrmVtxInfo;
class TrigBjetSecVtxInfo;
class TrigBjetJetInfo;

/**
 * @brief FEX class for the b-jet weight construction to be used by subsequent hypothesis algorithm.
 *
 * @author Andrea Coccaro <Andrea.Coccaro@ge.infn.it>
 *
 * This is the base feature extraction class for the HLT b-jet slice. 
 * It computes relevant quantities based on track impact parameters and secondary vertices, creates Bjet object and attaches it to TE.
 */


class TrigBjetFexStandalone: public HLT::FexAlgo {
  
 public:

  /** @brief For monitoring purposes the TrigBjetTagger class has been declared as friend. */
  friend class TrigBjetTagger;

  /** @brief Constructor. */
  TrigBjetFexStandalone(const std::string&, bool negativeWeights=false);
  /** @brief Destructor. */
  ~TrigBjetFexStandalone();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute();
  HLT::ErrorCode hltFinalize(); 

  // make this public to access results

  /** @brief Pointer to TrigBjetTagger class. */ 
  TrigBjetTagger* m_trigBjetTagger;

  // set information for analysis
    HLT::ErrorCode setRoI(float eta, float phi);
    HLT::ErrorCode setBeamSpot(float beamSpot[3],
                               float beamTilt[2],
                               float beamSigma[3],
                               int   beamStatus);
    HLT::ErrorCode setPrmVtx(float pos[3]);
    HLT::ErrorCode clearTracks();
    HLT::ErrorCode loadTrack(float pT, float eta, float phi, float d0, float z0,
                             float ed0, float ez0, int pixHits, int sctHits, int siHits,
                             float chi2, int blayer);
    HLT::ErrorCode setSV(float vtxMass, float sign3D, float eFrac, int nTracks, int n2TrkVtx);


    int m_useJetDirection;

 //private:

  /** @brief To retrieve tracks collection reconstructed at LVL2 and stored in TrigInDetTrackCollection. */
  //HLT::ErrorCode getCollection(const TrigInDetTrackCollection*&, const HLT::TriggerElement*);
  /** @brief To retrieve track collections reconstructed at EF and stored in TrackParticleContainer. */
  //HLT::ErrorCode getCollection(const Rec::TrackParticleContainer*&, const HLT::TriggerElement*);
  /** @brief To retrieve primary vertex collections reconstructed and stored in TrigVertexCollection. */
  //HLT::ErrorCode getPrmVtxCollection(const TrigVertexCollection*&, const HLT::TriggerElement*);
  /** @brief To retrieve primary vertex collections reconstructed and stored in VxContainer. */
  //HLT::ErrorCode getPrmVtxCollection(const VxContainer*&, const HLT::TriggerElement*);
  /** @brief To retrieve secondary vertex collections reconstructed at L2 */
  //HLT::ErrorCode getSecVtxCollection(const TrigVertexCollection*&, const HLT::TriggerElement*);
  /** @brief To retrieve secondary vertex collections reconstructed and stored in Trk::VxSecVertexInfoContainer. */
  //HLT::ErrorCode getSecVtxCollection(const Trk::VxSecVertexInfoContainer*&, const HLT::TriggerElement*);
  /** @brief To retrieve the L2 secondary vertex information */
  //HLT::ErrorCode getSecVtxInfo(const TrigVertexCollection*&); 
  /** @brief To retrieve the EF secondary vertex information. */
  //HLT::ErrorCode getSecVtxInfo(const Trk::VxSecVertexInfoContainer*&, const VxContainer*&, const TrigVertexCollection*&);

  /** @brief To select LVL2 tracks. */
  bool l2TrackSel(const TrigBjetTrackInfo &, unsigned int);
  /** @brief To select EF tracks. */
  bool efTrackSel(const TrigBjetTrackInfo &, unsigned int);

  /** @brief TrigTrackJetFinder tool. */
  TrigTrackJetFinderTool *m_trackJetFinderTool;
  /** @brief Pointer to TrigL2Bjet collection. */
  //TrigL2BjetContainer* m_trigL2BjetColl;
  /** @brief Pointer to TrigEFBjet collection. */
  //TrigEFBjetContainer* m_trigEFBjetColl;

  /** @brief Pointer to TaggerHelper class. */ 
  TaggerHelper* m_taggerHelper;

  /** @brief Const pointer to TrigBjetTagger class (for monitoring). */ 
  const TrigBjetTagger* m_constTrigBjetTagger;

  /** @brief Internal EDM class for primary vertex and beam-spot information, defined in TrigBjetDataHelper. */ 
  TrigBjetPrmVtxInfo* m_trigBjetPrmVtxInfo;
  /** @brief Internal EDM class for secondary vertex information, defined in TrigBjetDataHelper. */ 
  TrigBjetSecVtxInfo* m_trigBjetSecVtxInfo;
  /** @brief Internal EDM class for jet direction information, defined in TrigBjetDataHelper. */ 
  TrigBjetJetInfo*    m_trigBjetJetInfo;
  /** @brief Internal EDM class for track parameter information, defined in TrigBjetDataHelper. */ 
  std::vector<TrigBjetTrackInfo>* m_trigBjetTrackInfoVector;

  std::vector<TrigBjetTrackInfo>* m_trigBjetTrackInfoVectorPreSel;

  //AACC
  const std::vector<TrigBjetTrackInfo> m_constTrigBjetTrackInfoVector;
  //TrigInDetTrackCollection m_tracksCollection;

  /** @brief track reconstruction algorithm at LVL2. */
  int m_algo;
  /** @brief list of computed taggers. */
  std::vector<std::string> m_taggers;
  /** @brief switch to use impact parameter errors parametrization. */
  bool m_useErrIPParam;
  /** @brief switch to compute jet direction using a simple-cone algorithm. */
  bool m_jetDirection;
  /** @brief switch to use primary vertex computed with TrigT2HistoPrmVtx or InDetTrigPriVxFinder algorithm. */
  bool m_l2PrmVtxAtEF;
  /** @brief switch to perform track-RoI eta/phi matching when selecting reconstructed tracks. */
  bool m_useEtaPhiTrackSel;
  /** @brief string corresponding to the trigger level in which the algorithm is running. */
  std::string m_instance;

  /** @brief to check the beam spot flag status and start track selection accordingly. */
  bool m_useBeamSpotFlag;
  /** @brief to set the maximum beam spot width to enable the selection. */
  float m_setBeamSpotWidth;

  /** @brief to use JetProb parametrization derived from data. */
  bool m_useParamFromData;

  /** @brief lower bound of the chi square of the reconstructed track (to perform track selection). */
  float m_trkSelChi2;
  /** @brief lower bound of the number of hits on b-layer of reconstructed track (to perform track selection). */
  int   m_trkSelBLayer;
  /** @brief lower bound of the number of hits in pixel detector of reconstructed track (to perform track selection). */
  int   m_trkSelPixHits;
  /** @brief lower bound of the number of hits in silicon detectors of reconstructed track (to perform track selection). */
  int   m_trkSelSiHits;
  /** @brief upper bound of transverse impact parameter of reconstructed track (to perform track selection). */
  float m_trkSelD0;
  /** @brief upper bound of longitudinal impact parameter of reconstructed track (to perform track selection). */
  float m_trkSelZ0;
  /** @brief lower bound of pT of the reconstructed track (to perform track selection). */
  float m_trkSelPt;

  //////////////////////
  //* for monitoring *//
  //////////////////////

  /** @brief to monitor track selection. */
  std::vector<float> m_listCutApplied;

  /** @brief to monitor track quantities and effect ot the selection. */
  std::vector<float> m_mon_trk_a0, m_mon_trk_a0_sel, m_mon_trk_Sa0_sel;
  std::vector<float> m_mon_trk_z0, m_mon_trk_z0_sel, m_mon_trk_z0_sel_PV, m_mon_trk_Sz0_sel;
  std::vector<float> m_mon_trk_prob;

  /** @brief To retrieve selected tracks in percentage. */ 
  float totSelectedTracks() const {
    if (!m_totTracks) return -0.1;
    else return (float)m_totSelTracks/(float)m_totTracks;
  };

  /** @brief Number of reconstructed tracks per RoI. */
  float m_totTracks;
  /** @brief Number of selected tracks per RoI. */
  float m_totSelTracks;

  //////////////////////
  //* for calibration *//
  //////////////////////

  /** @brief Pointer to TuningLikelihood class for IP1D likelihood tagger. */ 
  TuningLikelihood* m_tuningLikelihoodIP1D;
  /** @brief Pointer to TuningLikelihood class for IP2D likelihood tagger. */ 
  TuningLikelihood* m_tuningLikelihoodIP2D;
  /** @brief Pointer to TuningLikelihood class for IP3D likelihood tagger. */ 
  TuningLikelihood* m_tuningLikelihoodIP3D;
  /** @brief Pointer to TuningLikelihood class for IP1D likelihood tagger. */ 
  TuningLikelihood* m_tuningLikelihoodIP1D_lowSiHits;
  /** @brief Pointer to TuningLikelihood class for IP2D likelihood tagger. */ 
  TuningLikelihood* m_tuningLikelihoodIP2D_lowSiHits;
  /** @brief Pointer to TuningLikelihood class for IP3D likelihood tagger. */ 
  TuningLikelihood* m_tuningLikelihoodIP3D_lowSiHits;
  /** @brief Pointer to TuningLikelihood class for MVtx likelihood tagger. */ 
  TuningLikelihood* m_tuningLikelihoodMVtx;
  /** @brief Pointer to TuningLikelihood class for EVtx likelihood tagger. */ 
  TuningLikelihood* m_tuningLikelihoodEVtx;
  /** @brief Pointer to TuningLikelihood class for NVtx likelihood tagger. */ 
  TuningLikelihood* m_tuningLikelihoodNVtx;
  /** @brief Pointer to TuningLikelihood class for SV likelihood tagger. */ 
  TuningLikelihood* m_tuningLikelihoodSV;

  /** @brief necessary for calibration constants for one-dimensional likelihood taggers based on impact parameters. */
  std::vector<float> m_sizeIP1D, m_bIP1D, m_uIP1D;  
  /** @brief necessary for calibration constants for two-dimensional likelihood taggers based on impact parameters. */
  std::vector<float> m_sizeIP2D, m_bIP2D, m_uIP2D;
  /** @brief necessary for calibration constants for three-dimensional likelihood taggers based on impact parameters. */
  std::vector<float> m_sizeIP3D, m_bIP3D, m_uIP3D;

  /** @brief necessary for calibration constants for one-dimensional likelihood taggers based on impact parameters. */
  std::vector<float> m_sizeIP1D_lowSiHits, m_bIP1D_lowSiHits, m_uIP1D_lowSiHits;  
  /** @brief necessary for calibration constants for two-dimensional likelihood taggers based on impact parameters. */
  std::vector<float> m_sizeIP2D_lowSiHits, m_bIP2D_lowSiHits, m_uIP2D_lowSiHits;
  /** @brief necessary for calibration constants for three-dimensional likelihood taggers based on impact parameters. */
  std::vector<float> m_sizeIP3D_lowSiHits, m_bIP3D_lowSiHits, m_uIP3D_lowSiHits;

  bool m_useLowSiHits;

  /** @brief necessary for calibration constants for one-dimensional likelihood taggers based on mass of secondary vertex. */
  std::vector<float> m_sizeMVtx, m_bMVtx, m_uMVtx;
  /** @brief necessary for calibration constants for one-dimensional likelihood taggers based on energy secondary vertex. */
  std::vector<float> m_sizeEVtx, m_bEVtx, m_uEVtx;
  /** @brief necessary for calibration constants for one-dimensional likelihood taggers based on number of tracks linked to secondary vertex. */
  std::vector<float> m_sizeNVtx, m_bNVtx, m_uNVtx;
  /** @brief necessary for calibration constants for three-dimensional likelihood taggers based on secondary vertex. */
  std::vector<float> m_sizeSV, m_bSV, m_uSV;

  /** @brief necessary for calibration constants for BjetProbability */
  std::vector<float> m_par_0_MC, m_par_1_MC, m_par_0_DT, m_par_1_DT;


  float m_pvX;
  float m_pvY;
  float m_pvZ;

  /** @brief switch for negati vweight evaluation */
  bool m_negativeWeights;

};

#endif
