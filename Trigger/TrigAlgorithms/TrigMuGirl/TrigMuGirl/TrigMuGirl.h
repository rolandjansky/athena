/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
    Authors: E. Reinherz-Aronis, A. Kreisel 
             based on MuGirl by Shlomit Tarem
***************************************************************************/
#ifndef TRIGMUGIRL_H
#define TRIGMUGIRL_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"

//* Trigger includes *//
#include <string>
#include "TrigInterfaces/FexAlgo.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "TrkSegment/SegmentCollection.h"

//added for EFContainer 2008-03-10
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
// for the optional output object
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"

#include "GaudiKernel/NTuple.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrkTrack/TrackCollection.h"
#include "Particle/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"


namespace HLT {
  class TriggerElement;
}

class StoreGateSvc;
class IExtrapolateToCaloTool;
class ICaloMuonLikelihoodTool;
class CaloClusterContainer;

namespace Rec
{
class TrackParticle;
class TrackParticleContainer;
}


namespace MuonGM
{
class MuonDetectorManager;
}

namespace Muon
{
class IMdtDriftCircleOnTrackCreator;
class IMuonClusterOnTrackCreator;
class IMuonSegmentMaker;
class IMuGirlParticleCreatorTool;
class MuonSegment;

}

namespace NTuple
{
class Tuple;
}

namespace MuGirlNS
{
class ICandidateTool;
class IPerformanceTruthTool;
class IGlobalFitTool;
class CandidateSummary;
class IStauTool;
class IANNSelectionTool;
class IMuGirlParticleCreatorTool;
class SegmentManager;
}

namespace Trk
{
class IIntersector;
class Surface;
// 2008-12-13 downgrade
//class ITrackParticleCreatorTool;
class ParticleCreatorTool;
}

namespace MagField {
class IMagFieldSvc;
}


class INTupleSvc;

class TrigMuGirlNtuple;

//for 

class TrigMuGirl : public HLT::FexAlgo {
  
  public:
  
  TrigMuGirl(const std::string&, ISvcLocator*);
  ~TrigMuGirl();
  
  HLT::ErrorCode hltInitialize();
  //  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* , HLT::TriggerElement*);
  HLT::ErrorCode hltFinalize(); 
  
  private:
  /*
  template <class T>
    StatusCode retrieve(ToolHandle<T>& pHandle, bool bError = true)
    {
      StatusCode sc = pHandle.retrieve();
      if (sc.isFailure())
        {
	  if (bError)
	    msg() << MSG::ERROR << "Cannot retrieve " << pHandle << endreq;
	  else
	    msg() << MSG::WARNING << "Cannot retrieve " << pHandle << endreq;
        }
        msg() << MSG::INFO << "Retrieved " << pHandle << endreq;
        return sc;
    }
  */
    // took out 2008-02-27 
    // because MuGirl bring something else with the same name
  //  StoreGateSvc* m_pEventStore;      //< The event store
  //  ActiveStoreSvc* m_pActiveStore;     //< The active store
  

    struct CaloParticle
    {
        const xAOD::TrackParticle*   pTrackParticle;
        const Trk::TrackParameters*  pTrackParameters;
        double eta;
        double phi;
        bool isolated;
        float dR;

        CaloParticle() :
	  pTrackParticle(NULL), pTrackParameters(NULL), eta(0.0), phi(0.0), isolated(true), dR(-1)
        {}

        CaloParticle(const xAOD::TrackParticle* pTParticle, const Trk::TrackParameters* pTParameters, double eta, double phi) :
	  pTrackParticle(pTParticle), pTrackParameters(pTParameters), eta(eta), phi(phi), isolated(true), dR(99)
        {}
    }
    ;
    typedef std::vector<CaloParticle*> CaloParticleList;

    bool generateMuGirl(MuGirlNS::CandidateSummary* summary);
    //    void fillMuonSegmentColl();
    void fillMuonSegmentColl(std::vector<const Muon::MuonSegment*> muonSegmentList);
    void testMuGirls();
    void createCaloParticles();
    void IsolatedIdTracks(double dR);
    void calculateLHR(CaloParticle* pParticle);
    Rec::TrackParticle* TEcreateParticle(const Trk::Track* track,const Trk::VxCandidate* vxCandidate,Trk::TrackParticleOrigin prtOrigin);

    template <class T>
    StatusCode retrieve(ToolHandle<T>& pHandle, bool bError = true)
    {
        StatusCode sc = pHandle.retrieve();
        if (sc.isFailure())
        {
            if (bError)
	      msg() << MSG::ERROR << "Cannot retrieve " << pHandle << endreq;
            else
	      msg() << MSG::WARNING << "Cannot retrieve " << pHandle << endreq;
        }
        else
	  msg() << MSG::INFO << "Retrieved " << pHandle << endreq;
        return sc;
    }
    template <class T>
    StatusCode retrieve(ServiceHandle<T>& pHandle, bool bError = true)
    {
        StatusCode sc = pHandle.retrieve();
        if (sc.isFailure())
        {
            if (bError)
                msg() << MSG::ERROR << "Cannot retrieve service " << pHandle << endreq;
            else
                msg() << MSG::WARNING << "Cannot retrieve service " << pHandle << endreq;
        }
        else
            msg() << MSG::INFO << "Retrieved service " << pHandle << endreq;
        return sc;
    }

//    MsgStream                               m_log;
    FloatProperty                           m_ptLowerLimit;     //< The pT cut on tracks  
    StringProperty                          m_ntupleName;       //< The name of the output NTuple  
    StringProperty                          m_ntupleTitle;      //< The label of the output NTuple  
    FloatProperty                           m_roadDeltaPhi;     //< The phi width of crude roads  
    BooleanProperty                         m_doNTuple;         //< Create the special NTuple  
    BooleanProperty                         m_doLHR;
    BooleanProperty                         m_doCSC;
    FloatProperty                           m_barrelNNCut;      //< Cut on NN function for barrel  
    FloatProperty                           m_endcapNNCut;      //< Cut on NN function for endcap  
    StringProperty                          m_sSegmentCollection; //< The name of the segment SG collection  

    StringProperty                          m_sRefittedTrkCollection; /**< The name of the RefittedTrk SG collection */
    StringProperty                          m_RefittedTrkLocation; /**< The name of the RefittedTrk SG collection */

    StringProperty                          m_inDetParticlesLocation;
    StringProperty                          m_CaloCollection;
    BooleanProperty                         m_doStau;
    FloatProperty                           m_stauPtCut;
    FloatProperty                           m_mfPtCut;

    //const MuonGM::MuonDetectorManager*      m_pMuonMgr;         //< The Muon manager  
    Trk::SegmentCollection*                 m_pSegmentCollection;
    //adeed 2008-07-21
    TrackCollection*                        m_pRefittedTrkCollection;
    Rec::TrackParticleContainer*            m_pRefittedTrkContainer;
    std::vector<CaloCell_ID::CaloSample>    m_caloLayers;
    std::string                             m_applyLHR;
    double                                  m_idR;
    double                                  m_lhr;
    double                                  m_eOverP;

    BooleanProperty                         m_doTruth;
    BooleanProperty                         m_doGlobalFit;
    BooleanProperty                         m_doSAFit;
    BooleanProperty                         m_doMuonFeature;
    BooleanProperty                         m_doANNSelection;
    BooleanProperty                         m_doParticleCreator;
    // const Trk::RunOutlierRemoval      m_runOutlier;
    //const Trk::ParticleHypothesis         m_matEffects;

    ElementLinkVector<xAOD::TrackParticleContainer>  m_inDetTrackParticleLinks;
    CaloParticleList                        m_caloParticles;
    const xAOD::CaloClusterContainer*       m_pClusCollection;
    NTuple::Tuple*                          m_pCandNTuple;

    // Tool Handles
    // didn't compaile trying "old ver"
    ServiceHandle<StoreGateSvc>                         m_pEventStore;      //< The event store  
    ToolHandle<MuGirlNS::ICandidateTool>                m_pCandidate;       //< The Muon candidate  
    ToolHandle<IExtrapolateToCaloTool>                  m_pToCalo;
    ToolHandle<ICaloMuonLikelihoodTool>                 m_pMuLHR;
    ToolHandle<MuGirlNS::IPerformanceTruthTool>         m_pTruthTool;
    // 2008-12-13 downgrade
    //    ToolHandle< Trk::ITrackParticleCreatorTool >              m_particleCreatorTool;
    //    ToolHandle< Trk::ParticleCreatorTool >              m_particleCreatorTool;

    ToolHandle<MuGirlNS::IGlobalFitTool>                m_pStauGlobalFitTool;
    ToolHandle<MuGirlNS::IGlobalFitTool>                m_pGlobalFitTool;
    ToolHandle<MuGirlNS::IGlobalFitTool>                m_pMuonFeatureGlobalFitTool;
    ToolHandle<MuGirlNS::IANNSelectionTool>             m_pANNSelectionTool;
    ToolHandle<MuGirlNS::IMuGirlParticleCreatorTool>    m_pParticleCreatorTool;
    //<S>
    ToolHandle<MuGirlNS::IStauTool>                     m_pStauTool;        //< The Stau tool  

    ServiceHandle<MagField::IMagFieldSvc>               m_magFieldSvc;

    MuGirlNS::SegmentManager*                           m_pSegmentManager;


    TrigMuonEFInfoContainer* eMuonEFInfoCont;
    Rec::TrackParticleContainer* eTrkParCont;

    TrigTimer* m_timerExecTotal;

    std::vector<float> muon_pT;
    std::vector<float> muon_phi;
    std::vector<float> muon_cotTh;
    std::vector<int> muon_nMDThits;
    std::vector<int> muon_nMDThitsInseg;
    std::vector<int> muon_nRPChitsInseg;
    std::vector<int> muon_nTGChitsInseg;
    std::vector<int> muon_nTGCRPChitsInseg;
    std::vector<int> muon_nCSCPhihits;
    std::vector<int> muon_nCSCEtahits;
    std::vector<int> muon_nRPCPhihits;
    std::vector<int> muon_nRPCEtahits;
    std::vector<int> muon_nTGCPhihits;
    std::vector<int> muon_nTGCEtahits;
    std::vector<int> muon_nTGCRPCPhihits;
    std::vector<int> muon_nTGCRPCEtahits;
    std::vector<int> muon_nMDTsegs;
    std::vector<int> muon_nRPCsegs;
    std::vector<int> muon_nCSCsegs;
    std::vector<int> muon_nTGCsegs;
    std::vector<int> muon_nTGCRPCsegs;
    std::vector<float> muon_beta;
    std::vector<float> muon_nnBarrel;
    std::vector<float> muon_nnEndcap;
    std::vector<float> muon_RPCHitsMeasTime;
    std::vector<float> muon_RPCHitsPropTime;
    std::vector<float> muon_RPCHitsTof;
    std::vector<float> muon_RPCHitsDis;

    // for perform
    TrigMuGirlNtuple*                   m_pTrigMuGirlNtuple;
    HLT::ErrorCode doMuTruthPerformance();
    NTuple::Tuple*                      m_ntuplePerformance;
    StringProperty                      m_ntupleNamePerformance;       /**< The name of the output NTuple */
    StringProperty                      m_ntupleTitlePerformance;      /**< The label of the output NTuple */
    BooleanProperty                     m_doTruthPerformance;
    BooleanProperty                     m_doNTuplePerformance;

    int eRunNumber;
    int eEventNumber;

    float NN[6];
    float dRmu;
    unsigned short int m_roi;
};

#endif
