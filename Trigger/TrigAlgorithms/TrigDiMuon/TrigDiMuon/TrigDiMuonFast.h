/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// TrigDiMuon

#ifndef TRIGLOWPTMUON_TrigDiMuonFast_H
#define TRIGLOWPTMUON_TrigDiMuonFast_H

#include "TrigInterfaces/FexAlgo.h"
#include "GaudiKernel/IIncidentListener.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigDiMuon/Candidate.h"
#include "MuonRDO/TgcRdoContainer.h"
#include "MuonRDO/MdtCsmContainer.h"
#include "TrigBphysicsEvent/TrigJpsi.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrigT1RPClogic/RPCrawData.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigParticle/TrigL2Bphys.h"
#include "TrigParticle/TrigL2BphysContainer.h"
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "RPCcablingInterface/IRPCcablingSvc.h"
#include "RPCgeometry/IRPCgeometrySvc.h"
#include "TGCgeometry/TGCgeometrySvc.h"
#include "MDTcabling/IMDTcablingSvc.h"
#include "TrigDiMuon/TrigDiMuonExtrapolator.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/LorentzVector.h"

class MuonDigitPosition;
class TgcIdHelper;
class CablingRPCBase;
class MDTcablingSvc;
class MDTGeometry;
class Incident;
class StoreGateSvc;
class ActiveStoreSvc;
class IRegSelSvc;
class TgcDigit;
class RpcDigitContainer;
class RpcPadContainer;

class ITrigL2VertexFitter;
class ITrigVertexingTool;

namespace MuonGM
{
    class MuonDetectorManager;
}

namespace LVL1
{
    class RecMuonRoI;
}

namespace TrigDiMuon
{
    struct tgcLUTDigit
    {
        double eta, phi, r, z, width;
        int sta;
        bool isStrip, associated;
    };

    typedef std::vector<tgcLUTDigit> LUTDigitVec;

    struct mdtTube
    {
        uint32_t mdt_TdcId, mdt_ChannelId;
        float mdt_tube_r, mdt_tube_z, associated;
    };
    typedef std::vector<mdtTube> mdtTubeVec;
}

struct Encoding
{
    unsigned unused:18;
    unsigned mdt:5;
    unsigned tgc:5;
    unsigned rpc:4;
};

union floatToInt
{
  float f;
  uint32_t i;
};

//information about track association
// UNKNOWN: hit association wasn't performed yet
// NO_HITS_ON_ROD: there were no hits in ROD to which track is pointing
// MUON_ID_FAILED: number of associated hits is below threshold => track is not muon
// MUON_ID_SUCCESSFUL: number of associated hits is above threshold => track is muon
enum AssociationResult { UNKNOWN, NO_HITS_ON_ROD, MUON_ID_FAILED, MUON_ID_SUCCESSFUL };

//error histogram values
namespace TrigDiMuonErr {
  const int noEventInfo = 1;
  const int noTriggerInfo = 2;
  const int noRoI = 3;
  const int moreThanOneRoI = 4;
  const int noMdtCont = 5;
  const int noRpcCont = 6;
  const int noTgcCont = 7;
  const int noTrackColl = 8;
  const int muDetailsFails =  9;
  const int bPhysCollFails = 10;
  const int vtxCollFails = 11;
  const int convMdtFails = 12;
  const int convRpcFails = 13;
  const int convTgcFails = 14;
  const int addTrackFails = 15;
  const int calcMassFails = 16;

}
//debug histogram values
namespace TrigDiMuonDebug {
  const int trkCollEmpty = 1;
  const int moreTrkColl = 2;
  const int vtxFailed = 3;
}
//acceptance histogram values
namespace TrigDiMuonAcc {
  //acceptance flow for each RoI
  const int input = 1;
  const int gotRoI = 2;
  const int gotTracks = 3;
  const int gotTrkPair = 4;
  const int gotOSTrkPair = 5;
  const int passInvMass = 6;
  const int passMuonId = 7;
  const int passRoIAssoc = 8;
  const int passVtx = 9;

  //acceptance flow for each candidate
  const int cand_gotTrkPair = 11;
  const int cand_gotOSTrkPair = 12;
  const int cand_passInvMass = 13;
  const int cand_passMuonId = 14;
  const int cand_passRoIAssoc = 15;
  const int cand_passVtx = 16;
}
//settings histogram values
namespace TrigDiMuonSetings {
  const int doLutTgcRdo = 1;
  const int UseTiltedBField = 2;
  const int doFillESD = 3;
  const int doTagAndProbe = 4;
  const int doOppChargeCheck = 5;
  const int doVertexing = 6;
  const int doRoiCheck = 7;
  const int doTiming = 8;
  const int retrieveRoI = 9;
}

//typedef std::vector<TrigDiMuon::tgcLUTDigit> LUTDigitVec;

class TrigDiMuonFast: public HLT::FexAlgo
{
public:

    TrigDiMuonFast(const std::string& name, ISvcLocator* pSvcLocator);
    ~TrigDiMuonFast();

    HLT::ErrorCode hltExecute(const HLT::TriggerElement*, HLT::TriggerElement*);
    HLT::ErrorCode hltInitialize();
    //void handle(const Incident &inc);
    HLT::ErrorCode hltFinalize();

private:

    //new tools for the Raw data conversion
    ToolHandle<Muon::IMuonRawDataProviderTool>   m_rpc_converter;
    ToolHandle<Muon::IMuonRawDataProviderTool>   m_mdt_converter;
    ToolHandle<Muon::IMuonRawDataProviderTool>   m_tgc_converter;
//     ToolHandle<Muon::IMuonRawDataProviderTool>   m_csc_converter;

    ToolHandle<TrigDiMuonExtrapolator> m_extrapolator;  //TrigDiMuonExtrapolator

    uint32_t m_current_run_id;
    uint32_t m_current_lbk_id;
    uint32_t m_current_bcg_id;
    uint32_t m_current_lvl1_id;

    int                                 m_max_pair;
    DoubleProperty                      m_MdtDigits_PhiCut;
    DoubleProperty                      m_MdtDigits_EtaCut;
    DoubleProperty                      m_MdtDelta_EtaCut;
    DoubleProperty                      m_MdtDelta_PhiCut;
    DoubleProperty                      m_RpcDigits_PhiCut;
    DoubleProperty                      m_RpcDigits_EtaCut;
    DoubleProperty                      m_RpcEtaDelta_PhiCut;
    DoubleProperty                      m_RpcEtaDelta_EtaCut;
    DoubleProperty                      m_RpcPhiDelta_PhiCut;
    DoubleProperty                      m_RpcPhiDelta_EtaCut;
    DoubleProperty                      m_RpcEtaFactor;
    DoubleProperty                      m_RpcPhiFactor;
    DoubleProperty                      m_TgcEtaFactor;
    DoubleProperty                      m_TgcPhiFactor;
    DoubleProperty                      m_MdtEtaFactor;
    DoubleProperty                      m_JpsiMassMin;
    DoubleProperty                      m_PtMin;
    BooleanProperty                     m_doLutTgcRdo;
    BooleanProperty                     m_tilted_B_field;
    DoubleProperty                      m_PHI_DELTA_TGC;
    DoubleProperty                      m_ETA_DELTA_TGC;
    IntegerProperty                     m_NHits;

    BooleanProperty                     m_fill_esd;
    IntegerProperty                     m_esd_ext_size;
    IntegerProperty                     m_esd_rob_size;
    IntegerProperty                     m_esd_csm_size;
    IntegerProperty                     m_esd_lv1_size;
    IntegerProperty                     m_esd_rpc_size;
    IntegerProperty                     m_esd_tgc_size;
    IntegerProperty                     m_esd_mdt_size;

    BooleanProperty                     m_doTagAndProbe;
    BooleanProperty                     m_doOppChargeCheck;
    BooleanProperty                     m_doVertexing;
    BooleanProperty                     m_doRoiCheck;

    DoubleProperty                      m_roiEndcap_dEtaCut;
    DoubleProperty                      m_roiEndcap_dPhiCut;
    DoubleProperty                      m_roiBarrel_dEtaCut;
    DoubleProperty                      m_roiBarrel_dPhiCut;

    BooleanProperty                     m_retrieveRoI;

    const MuonGM::MuonDetectorManager*  m_pMuonMgr;
    const TgcIdHelper*                  m_pTgcIdHelper;
    //const TGCcablingSvc*                m_pTgcCabling;
    const TGCgeometrySvc*               m_pTgcLUTSvc;
    IRegSelSvc*                         m_pRegionSelector;
    const LVL1::RecMuonRoI*             m_pMuonRoI;
    const MDTGeometry*                  m_pMdtGeometry;
    StoreGateSvc*                       m_pStoreGate;
    const RpcDigitContainer*            m_pRpcDigitContainer;
    const RpcPadContainer*              m_pRpcPadContainer;
    //const CablingRPCBase*               m_pRpcCabling;
    const IRPCcablingSvc*               pRpcCabling;
    const RPCGeometry*                  m_pRpcGeometry;
    ServiceHandle<IRPCgeometrySvc>      m_RPCgeometrySvc;
    ServiceHandle<TGCgeometrySvc>       m_TGCgeometrySvc;
    ServiceHandle<IMDTcablingSvc>       m_MDTcablingSvc;
    NTuple::Tuple*                      m_ntuple;
    std::vector<TgcDigit*>              m_TgcDigitVector;    /**< List of TGC digits. */
    TrigDiMuon::LUTDigitVec             m_tgc_lut_digits;
    TrigDiMuon::mdtTubeVec              m_mdt_tubes;
    std::vector<RPCrawData>             m_rpc_raw_data_list;
    const MdtCsmContainer*              pMdtContainer;
    const TgcRdoContainer*              pTgcRdoContainer;
    std::string                         m_roi;
    //int                               m_roi_num;
    //int                               m_jpsi_num;
    //pointer to the ROB data provider
    ROBDataProviderSvc*                 m_pROBDataProvider;

    //vertexing
    ToolHandle<ITrigL2VertexFitter> m_L2vertFitter;
    ToolHandle<ITrigVertexingTool>  m_vertexingTool;

    TrigTimer * m_timerTotal;
    TrigTimer * m_timerTotal1;
    TrigTimer * m_timerPerFun;
    TrigTimer * m_timerAllFun;
    TrigTimer * m_timerTotalMdt;
    TrigTimer * m_timerTotalRpc;
    TrigTimer * m_timerRpcProp;
    TrigTimer * m_timerRpcLUT;
    TrigTimer * m_timerRpcData;
    TrigTimer * m_timerRpcPoints;
    TrigTimer * m_timerTotalTgc;
    TrigTimer * m_timerTgcProp;
    TrigTimer * m_timerTgcLUT;
    TrigTimer * m_timerTgcPoints;

    ///Monitoring Var

    int barrel_mdt_mon_flag; //flag for mdt hits Barrel/End-Cap
    CLHEP::HepLorentzVector j_psi_mon_vector;

    //roi monitoring
    float m_roi_eta;
    float m_roi_phi;

    //muon variables
    std::vector<float> m_pos_muon_barrel_mdt_hits;
    std::vector<float> m_neg_muon_barrel_mdt_hits;
    std::vector<float> m_pos_muon_cap_mdt_hits;
    std::vector<float> m_neg_muon_cap_mdt_hits;
    std::vector<float> m_pos_muon_tgc_hits;
    std::vector<float> m_pos_muon_rpc_hits;
    std::vector<float> m_neg_muon_tgc_hits;
    std::vector<float> m_neg_muon_rpc_hits;
    std::vector<float> m_pos_muon_eta;
    std::vector<float> m_pos_muon_phi;
    std::vector<float> m_pos_muon_pt;
    std::vector<float> m_neg_muon_eta;
    std::vector<float> m_neg_muon_phi;
    std::vector<float> m_neg_muon_pt;

    //jpsi variables
    float m_j_psi_per_roi;

    std::vector<float> m_j_psi_mass;
    std::vector<float> m_j_psi_mass_wide;
    std::vector<float> m_j_psi_eta;
    std::vector<float> m_j_psi_phi;
    std::vector<float> m_j_psi_pt;
    std::vector<float> m_j_psi_chi2;
    std::vector<float> m_j_psi_lifetime;
    //float m_muon_pt_diff;

    //RoI association
    std::vector<float> m_roiEndcap_dEta;
    std::vector<float> m_roiEndcap_dPhi;
    std::vector<float> m_roiEndcap_dR;
    std::vector<float> m_roiBarrel_dEta;
    std::vector<float> m_roiBarrel_dPhi;
    std::vector<float> m_roiBarrel_dR;

    std::vector<float> m_roiEndcap_ne_dEta;
    std::vector<float> m_roiEndcap_ne_dPhi;
    std::vector<float> m_roiEndcap_ne_dR;
    std::vector<float> m_roiBarrel_ne_dEta;
    std::vector<float> m_roiBarrel_ne_dPhi;
    std::vector<float> m_roiBarrel_ne_dR;

    //tracks that are associated to RoI, but doesn't have any hits
    std::vector<float> m_roiNoHits_pt;
    std::vector<float> m_roiNoHits_charge;
    std::vector<float> m_roiNoHits_eta;
    std::vector<float> m_roiNoHits_phi;

    //tag and probe
    std::vector<float> m_tap_mass_trTr;     //mass of all di-tracks
    std::vector<float> m_tap_mass_trTr_os;  //mass of di-tracks after opposite-sign check
    std::vector<float> m_tap_mass_trRoI;    //one of the track is matched to RoI
    std::vector<float> m_tap_mass_muRoI;    //one track matched to RoI, the second one identified as muon (hits ass)
    std::vector<float> m_tap_mass_muMuRoI;  //one track matched to RoI, both identified as muons

    //error histogram
    std::vector<int> m_hist_error;           //error/warning/odd behaviour

    //error histogram
    std::vector<int> m_hist_debug;           //debug histogram

    //acceptance histogram
    std::vector<int> m_hist_lastStage;       //last stage of the algorithm executed for each RoI
    std::vector<int> m_hist_cand_lastStage;       //last stage of the algorithm executed for each candidate

    //settings histogram indicating current setting of the algorithm
    std::vector<int> m_hist_settings;

    //counters
    int m_cnt_allTrk;       // all input tracks
    int m_cnt_accTrk;       // accepted tracks (pt>cut)
    int m_cnt_trTr;         // di-track candidates
    int m_cnt_muMu;         // di-muon tracks
    int m_cnt_muMuRoI;      // di-muon with one track matched to RoI
    int m_cnt_muMuVtx;      // di-muon passing vertexing
    int m_cnt_muMuRoIVtx;   // di-muon passing vertexing with one track matched to RoI

    //hit association
    std::vector<float> m_hit_Endcap_mdt_inner_dEta ;
    std::vector<float> m_hit_Endcap_mdt_middle_dEta;
    std::vector<float> m_hit_Barrel_mdt_inner_dEta ;
    std::vector<float> m_hit_Barrel_mdt_middle_dEta;
    std::vector<float> m_hit_rpc_middle_dEta       ;
    std::vector<float> m_hit_rpc_middle_dPhi       ;
    std::vector<float> m_hit_tgc_wire_inner_dEta   ;
    std::vector<float> m_hit_tgc_wire_middle_dEta  ;
    std::vector<float> m_hit_tgc_wire_inner_dPhi   ;
    std::vector<float> m_hit_tgc_wire_middle_dPhi  ;
    std::vector<float> m_hit_tgc_strip_inner_dPhi  ;
    std::vector<float> m_hit_tgc_strip_middle_dPhi ;

    //vertexing monitoring
    std::vector<float> m_vtx_mass;
    std::vector<float> m_vtx_chi2;
    std::vector<float> m_vtx_ndof;
    std::vector<float> m_vtx_x   ;
    std::vector<float> m_vtx_y   ;
    std::vector<float> m_vtx_z   ;

    ///private methods

    double invariant_mass( double pt1, double phi1, double eta1, double pt2, double phi2, double eta2);
    //bool HitAssociation( const TrigInDetTrack* track, Candidate & t, int & n_mdt_hits, int & n_rpc_eta_hits, int & n_rpc_phi_hits, int & n_tgc_eta_hits, int & n_tgc_phi_hits);
    bool HitAssociation( TrigInDetTrack* track, Candidate & t, int & n_mdt_hits, int & n_rpc_eta_hits, int & n_rpc_phi_hits, int & n_tgc_eta_hits, int & n_tgc_phi_hits);
    bool MdtAssociation( const TrigInDetTrack* track, HepGeom::Point3D<double>& gPos, double& track_phi_ext);
    //void handle(const Incident &);
    void makeRPC(Candidate& t);
    void makeTGC(Candidate& t);
    void makeMDT(Candidate& t);
    void prepareRpcPoints( Candidate& t, RPCrawData rpc_digits);
    void prepareTgcPoints( Candidate& t);

    HLT::ErrorCode processTrackColl(const TrigInDetTrackCollection * theTrackColl,
                                    TrigL2BphysContainer * trigBphysColl,
                                    TrigVertexCollection * trigVertexColl,
                                    const LVL1::RecMuonRoI* m_pMuonRoI,
                                    const HLT::TriggerElement* inputTE,
                                    HLT::TriggerElement* outputTE);
    //void setDetails(const Candidate & t, const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);
    void setDetails(const Candidate & t, MuonFeatureDetails* m_det);
    AssociationResult processTrack(TrigInDetTrack* track,
                                   int i_track,
                                   int firstOrSecond,
                                   const LVL1::RecMuonRoI* m_pMuonRoI,
                                   const HLT::TriggerElement* inputTE,
                                   HLT::TriggerElement* outputTE);
    void tagAndProbe(std::vector<std::pair<TrigInDetTrack*, AssociationResult> >& myCollection,
                     const LVL1::RecMuonRoI* m_pMuonRoI,
                     const HLT::TriggerElement* inputTE,
                     HLT::TriggerElement* outputTE);
    void getDEtaDPhiDR(TrigInDetTrack* track,
                       const LVL1::RecMuonRoI* m_pMuonRoI,
                       double& endcap_dEta, double& endcap_dPhi, double& endcap_dR,
                       double& barrel_dEta, double& barrel_dPhi, double& barrel_dR ,
                       double& endcap_ne_dEta, double& endcap_ne_dPhi, double& endcap_ne_dR,
                       double& barrel_ne_dEta, double& barrel_ne_dPhi, double& barrel_ne_dR);
    bool roiAssociation(TrigInDetTrack* track, const LVL1::RecMuonRoI* m_pMuonRoI);
};

#endif
