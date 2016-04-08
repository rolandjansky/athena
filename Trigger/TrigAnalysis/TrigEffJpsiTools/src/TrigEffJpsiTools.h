/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFFJPSITOOLS_H
#define TRIGEFFJPSITOOLS_H
/**
 * @author    Hironori Kiyamura
 * @author    Takashi Matsushita
 * @date      $Date: 2016-02-10 17:47:59 +0100 (Wed, 10 Feb 2016) $
 * @version   $Revision: 723159 $
 */

/** @todo */
/** @warnings */


/*--------------------------------------------------------------------*
 * headers
 *--------------------------------------------------------------------*/
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigEffJpsiTools/ITrigEffJpsiTools.h"
#include "TrkParameters/TrackParameters.h"
#include <vector>
#include <string>

class INavigable4Momentum;
namespace Rec { class TrackParticle; }
namespace Trk { class IExtrapolator; }
namespace Trk { class Track; }
namespace Trk { class RIO_OnTrack; }
namespace Trk { class TrackStateOnSurface; }
namespace LVL1RPC { class RPCRecRoiSvc; }
namespace LVL1TGC { class TGCRecRoiSvc; }
namespace Muon { class MuonIdHelperTool; }
namespace Trig { class TrigDecisionTool; }
namespace Trig { class ChainGroup; }


class IRPCcablingSvc;
class MuonFeature;
class TrigL2Bphys;
class TrigEFBphys;
class TrigInDetTrack;

/*--------------------------------------------------------------------*
 * classes
 *--------------------------------------------------------------------*/
struct Encoding
{
  unsigned unused:18;
  unsigned mdt:5;
  unsigned tgc:5;
  unsigned rpc:4;
};

class TrigEffJpsiTools : virtual public ITrigEffJpsiTools,
                         public AthAlgTool { 

  public:
    TrigEffJpsiTools(const std::string& type,
                     const std::string& name,
                     const IInterface* parent);
    ~TrigEffJpsiTools();

    virtual StatusCode initialize();
    virtual StatusCode finalize();


    bool isTriggeredMuonEF(const Rec::TrackParticle* track,
                           const std::string& chainName) const;

    bool isTriggeredMuonL2(const Rec::TrackParticle* track,
                           const std::string& chainName) const;

    bool isTriggeredMuonL1(const Rec::TrackParticle* track,
                           const std::string& triggerName) const;

    bool isTriggeredMuon(const Rec::TrackParticle* track,
                         int thresholdNumber) const;

    bool isJPsiCandidate(const Rec::TrackParticle* track1,
                         const Rec::TrackParticle* track2) const;

    bool isJPsiCandidate(const INavigable4Momentum* mom1,
                         const INavigable4Momentum* mom2) const;

    int getMatchedRoI(const Rec::TrackParticle* track,
                      bool* barrel = 0,
                      bool* endcap = 0,
                      Amg::Vector2D* eta = 0,
                      Amg::Vector2D* phi = 0) const;
  

    StatusCode getRoiSize(const unsigned int roiWord,
                          double* etaSize,
                          double* phiSize) const;

    StatusCode getRioOnTriggerChamber(const Trk::Track* track,
                                      std::vector<const Trk::RIO_OnTrack*>& rots,
                                      bool pivotOnly=true) const;

    bool extrapolateToRio(const Rec::TrackParticle* track,
                          const Trk::RIO_OnTrack* rot,
                          Amg::Vector2D* eta,
                          Amg::Vector2D* phi,
                          Amg::Vector2D* ptpz,
                          bool getTsos,
                          const std::vector<const Trk::TrackStateOnSurface*>* &tsos) const;

    bool extrapolateToPoint(const Rec::TrackParticle* track,
                            const Amg::Vector3D* pos,
                            const int detector,
                            Amg::Vector2D* eta,
                            Amg::Vector2D* phi,
                            Amg::Vector3D* mom) const;

    std::vector<HLT::TriggerElement*> getExpertTEs(const std::string& chain = "EF_mu4") const;

    bool getTriggerObjects(const std::string& chain,
                           TrigObjectMap* muonsEF = NULL,
                           TrigObjectMap* muonsL2 = NULL,
                           TrigObjectMap* muonsL1 = NULL) const;
    bool getL1TriggerObjects(const std::string& chain,
                             TrigObjectMap* muonsL1 = NULL) const;
    bool getL2TriggerObjects(const std::string& chain,
                             TrigObjectMap* muonsL2 = NULL,
                             TrigObjectMap* muonsL1 = NULL) const;
    bool getEFTriggerObjects(const std::string& chain,
                             TrigObjectMap* muonsEF = NULL,
                             TrigObjectMap* muonsL2 = NULL,
                             TrigObjectMap* muonsL1 = NULL) const;
    bool getEFAllTriggerObjects(const std::string& chain,
			      TrigObjectMap* muonsEF = NULL,
			      TrigObjectMap* muonsL2 = NULL,
			      TrigObjectMap* muonsL1 = NULL) const;

    bool getEFIDTriggerObjects(const std::string& chain,
				TrigObjectMap* muonsEF = NULL,
				TrigObjectMap* muonsL2 = NULL,
				TrigObjectMap* muonsL1 = NULL) const;

    bool getEFMSTriggerObjects(const std::string& chain,
				TrigObjectMap* muonsEF = NULL,
				TrigObjectMap* muonsL2 = NULL,
				TrigObjectMap* muonsL1 = NULL) const;

    bool getBPhysL2TriggerObjects(const std::string& chain,
                                  TrigObjectMap* muonsL2 = NULL,
                                  TrigObjectMap* muonsL1 = NULL) const;
    bool getBPhysEFTriggerObjects(const std::string& chain,
                                  TrigObjectMap* muonsEF = NULL,
                                  TrigObjectMap* muonsL2 = NULL,
                                  TrigObjectMap* muonsL1 = NULL) const;

    int muctpiBcidDiff(const int roiBcid) const;
    int getMuonRoiBcidDiff(const uint32_t roiWord) const;

    bool isOffTimeTgcTrigger() const;
    bool EF_mu4_MSOnly_from_EF_mu0_outOfTime1() const;


  private:
    // methods to get extrapolation error on each plane 
    void getExtrapolationErrorOnEndcap(const Trk::AtaDisc* measurement,
                                       Amg::Vector2D* dEtaPhi) const;
    void getExtrapolationErrorOnBarrel(const Trk::AtaCylinder* measurement,
                                       Amg::Vector2D* dEtaPhi) const;
                                       
    // methods to get search range on each plane 
    double getSearchRangeOnEndcap(const Trk::AtaDisc* measurement) const;
    double getSearchRangeOnBarrel(const Trk::AtaCylinder* measurement) const;

    // methods to get eta-phi distance
    double getEtaPhiDistanceAtIp(const Rec::TrackParticle* track1,
                                 const Rec::TrackParticle* track2) const;
    double getEtaPhiDistanceOnEndcap(const Trk::AtaDisc* measurement1,
                                     const Trk::AtaDisc* measurement2) const;
    double getEtaPhiDistanceOnBarrel(const Trk::AtaCylinder* measurement1,
                                     const Trk::AtaCylinder* measurement2) const;
    double getEtaPhiDistance(double eta1,
                             double eta2,
                             double phi1,
                             double phi2) const;
    double getEtaDistance(double eta1,
                          double eta2) const;
    double getPhiDistance(double phi1,
                          double phi2) const;

    // methods to check isolation
    bool isSeparated(const Rec::TrackParticle* track1,
                     const Rec::TrackParticle* track2) const;
    bool isSeparatedOnEndcap(const Rec::TrackParticle* track1,
                             const Rec::TrackParticle* track2) const;
    bool isSeparatedOnBarrel(const Rec::TrackParticle* track1,
                             const Rec::TrackParticle* track2) const;

    // methods to convert local errors to global one
    double getEtaErrFromRPhi(const Trk::AtaDisc* measurement) const;
    double getEtaErrFromRPhiZ(const Trk::AtaCylinder* measurement) const;

    // methods for track extrapolation to target plane
    const Trk::AtaDisc*
      extrapolateToEndcap(const Rec::TrackParticle* track,
                          const double pivotZ,
                          const bool useCache = true) const;
    const Trk::AtaCylinder*
      extrapolateToBarrel(const Rec::TrackParticle* track,
                          const double pivotRadius,
                          const bool useCache = true) const;

    const std::vector<const Trk::TrackStateOnSurface*>*
      extrapolateToEndcapM(const Rec::TrackParticle* track,
                           const double pivotZ) const;
    const std::vector<const Trk::TrackStateOnSurface*>*
      extrapolateToBarrelM(const Rec::TrackParticle* track,
                           const double pivotRadius) const;

    // struct to store center of search and serch range
    struct Search {
      double eta;
      double phi;
      double searchRange;
    };

    // methods to check if track has associated muon roi or not
    bool isTriggered(const struct Search& centre,
                     const int thresholdNumber,
                     const LVL1_ROI::muons_type& muonRoIs) const;
    bool isTriggeredOnEndcap(const Rec::TrackParticle* track,
                             LVL1_ROI::muons_type& muonRoIs,
                             int thresholdNumber) const;
    bool isTriggeredOnBarrel(const Rec::TrackParticle* track,
                             LVL1_ROI::muons_type& muonRoIs,
                             int thresholdNumber) const;

    // methods to check the roi region
    int getRoiRegion(int roiWord) const;
    unsigned int getBitMaskValue(const unsigned int value,
                                 const unsigned int mask) const;

    // methods to get roi size
    StatusCode getRoiSizeTgc(const unsigned int roiWord,
                             double* etaSize,
                             double* phiSize) const;

    StatusCode getRoiSizeRpc(const unsigned int roiWord,
                             double* etaSize,
                             double* phiSize) const;

    // method to extract pt value from chain name
    int getL1PtThreshold(const std::string& l1TriggerName) const;

    // method to check if the given chain is configured or not
    bool isConfigured(const std::string& chainName,
                      const std::string& prefix) const;

    // struct to store matched roi information
    struct Match {
      int roiIndex;
      unsigned int roiWord;
      int roiThrNumber;
      int roiThrValue;
      float roiEta;
      float roiPhi;
      Amg::Vector2D* etaExTrk;
      Amg::Vector2D* phiExTrk;
      bool barrel;
      bool endcap;

      Match() : roiIndex(-1), roiWord(0), roiThrNumber(-1), roiThrValue(-1),
                roiEta(0.), roiPhi(0.), etaExTrk(0), phiExTrk(0),
                barrel(false), endcap(false) {}
    };

    bool getMatchedRoI(const Rec::TrackParticle* track,
                       struct Match& match) const;
    bool isTriggeredMuonHLT(const Rec::TrackParticle* track,
                            const std::string& chainName,
                            const Trig::ChainGroup *cg) const;
    bool isTriggeredMuonHLTwMuonFeature(const Rec::TrackParticle* track,
                                        const std::string& chainName,
                                        const Trig::ChainGroup *cg) const;

    // struct for data cache
    typedef std::map<const Rec::TrackParticle*,
                     const Trk::TrackParameters*> CacheMap;
    typedef std::pair<const Rec::TrackParticle*,
                      const Trk::TrackParameters*> CachePair;
    struct Cache {
      unsigned int runNo;
      unsigned int evtNo;
      CacheMap tp;
    };

    void validateCache(Cache& map) const;

    bool addTrigObject(TrigObject& trig, TrigObjectMap* map, bool appendIfExists = true) const;

    uint32_t RDOtoRoI(uint32_t data_word) const;

    bool EF_mu4_MSOnly_with_Muid(const MuonFeature* mf) const;
    bool hasPassedMuFast(const MuonFeature* mf) const;
    bool hasPassedTrigMuonEF(const Rec::TrackParticle* tp) const;

    void Btrig_L2_initCuts();
    void Btrig_EF_initCuts();

    bool selectL2TrigDiMuon(const TrigL2Bphys* trigger,
			    const TrigInDetTrack* track1,
			    const TrigInDetTrack* track2,
			    bool checkOS, // if true, tracks must have opposite charge
			    double massMin, // invariant mass lower limit
			    double massMax, // invariant mass upper limit
			    bool applyMassMax, // apply upper invariant mass cut
			    int nHits, // number of matching muon hits
			    double chi2) const;

    bool selectEFTrigDiMuon(const TrigEFBphys* trigger,
			    const Rec::TrackParticle* track1,
			    const Rec::TrackParticle* track2,
			    bool checkOS, // if true, tracks must have opposite charge
			    double massMin, // invariant mass lower limit
			    double massMax, // invariant mass upper limit
			    bool applyMassMax, // apply upper invariant mass cut
			    double chi2) const;

    // attributes

    // service handles
    ServiceHandle<LVL1RPC::RPCRecRoiSvc> m_rpcRoiService;
    ServiceHandle<LVL1TGC::TGCRecRoiSvc> m_tgcRoiService;

    // tool handles
    ToolHandle<Muon::MuonIdHelperTool> m_muonIdHelper;
    ToolHandle<Trk::IExtrapolator> m_extrapolator;
    ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;

    std::string m_Lvl1RoiName;
    double m_endcapPivotPlaneZ;
    double m_endcapPivotPlaneMinimumRadius;
    double m_endcapPivotPlaneMaximumRadius;
    double m_barrelPivotPlaneRadius;
    double m_barrelPivotPlaneHalfLength;
    double m_invariantMassUpperLimit;
    double m_invariantMassLowerLimit;
    double m_trackPtCut;
    double m_trackEtaCut;
    double m_roiSizeBarrel;
    double m_roiSizeEndcap;
    double m_roiSizeForward;
    double m_endcapForwardBoundary;
    double m_searchRangeTolerance;
    double m_searchRangeOffset;
    bool   m_appendL1IfExists;
    bool   m_appendL2IfExists;
    bool   m_appendEFIfExists;
    bool   m_useManualHypoCuts;

    const IRPCcablingSvc* p_rpcCablingSvc;

    mutable Cache m_cacheEndcap;
    mutable Cache m_cacheBarrel;

    // constants
    const double m_MUON_MASS;
    enum RoiRegion {
      BARREL,
      ENDCAP,
      NUM_REGION
    };

    std::vector<std::string> m_Btrig_L2_chain;
    std::vector<bool> m_Btrig_L2_checkOS;
    std::vector<double> m_Btrig_L2_massMin;
    std::vector<double> m_Btrig_L2_massMax;
    std::vector<bool> m_Btrig_L2_applyMassMax;
    std::vector<int> m_Btrig_L2_nHits;
    std::vector<double> m_Btrig_L2_chi2;

    std::vector<std::string> m_Btrig_EF_chain;
    std::vector<bool> m_Btrig_EF_checkOS;
    std::vector<double> m_Btrig_EF_massMin;
    std::vector<double> m_Btrig_EF_massMax;
    std::vector<bool> m_Btrig_EF_applyMassMax;
    std::vector<double> m_Btrig_EF_chi2;
   
};

#endif // TRIGEFFJPSITOOLS_H
