/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
    Authors: E. Reinherz-Aronis, A. Kreisel 
             based on MuGirl by Shlomit Tarem
***************************************************************************/
#include "TrigInterfaces/AlgoConfig.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigNavigation/Navigation.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include <iostream>
#include<iomanip>

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "GaudiKernel/NTuple.h"

#include "TrigMuGirl/TrigMuGirl.h"
#include "MuGirlInterfaces/ICandidateTool.h"
#include "MuGirlInterfaces/CandidateSummary.h"
#include "MuGirlInterfaces/IPerformanceTruthTool.h"
#include "MuGirlInterfaces/IGlobalFitTool.h"
#include "MuGirlInterfaces/IANNSelectionTool.h"
#include "MuGirlInterfaces/IMuGirlParticleCreatorTool.h"
#include "MuGirlInterfaces/IStauTool.h"
#include "MuGirlInterfaces/SegmentManager.h"
#include "MuGirlInterfaces/RpcSegmentInfo.h"

//#include "MuGirlEvent/MuGirlObject.h"
//#include "MuGirlEvent/MuGirlObjectCollection.h"
#include "StoreGate/StoreGateSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "RegionSelector/IRegSelSvc.h"

#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"

#include "TrkTrack/TrackCollection.h"
#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"
#include "TrkExInterfaces/IIntersector.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "RecoToolInterfaces/IExtrapolateToCaloTool.h"
#include "ICaloTrkMuIdTools/ICaloMuonLikelihoodTool.h"
#include "CaloEvent/CaloClusterContainer.h"

#include "Particle/TrackParticleContainer.h"


//<S>
#include "MuGirlInterfaces/MdtSegmentMakerInfo.h"

#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"

//for TEcreateParticle
//#include "TrkEventPrimitives/FitQuality.h"
//#include "TrkSurfaces/PerigeeSurface.h"
//#include "TrkTrack/TrackStateOnSurface.h"

// for performance 
#include "TrigMuGirl/TrigMuGirlNtuple.h"
#include "GeneratorObjects/McEventCollection.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"

using HepGeom::Point3D;
using CLHEP::Hep3Vector;
using CLHEP::GeV;

TrigMuGirl::TrigMuGirl(const std::string& name, ISvcLocator* pSvcLocator) :
  HLT::FexAlgo(name, pSvcLocator),
        m_ptLowerLimit(2.5*GeV),
        m_ntupleName("/NTUPLES/MUGIRL/MUGIRL"),
        m_ntupleTitle("Muon Candidates"),
        m_roadDeltaPhi(0.2),
        m_doNTuple(false),
        m_doLHR(false),
        m_doCSC(false),
        m_barrelNNCut(0.4),
        m_endcapNNCut(0.4),
        m_sSegmentCollection("MuGirlSegments"),
        m_sRefittedTrkCollection("MuGirlRefittedTracks"),
        m_RefittedTrkLocation("MuGirlRefittedTrackParticles"),
        m_inDetParticlesLocation("TrackParticleCandidate"),
        m_CaloCollection("CaloTopoCluster"),
        m_doStau(false),
        m_stauPtCut(20000.0),
        m_mfPtCut(20000.0),
        //m_pMuonMgr(NULL),
        m_pSegmentCollection(NULL),
        m_pRefittedTrkContainer(NULL),
        m_applyLHR("On"),
        m_idR(0.3),
        m_lhr(0.8),
        m_eOverP(0.3),
        m_doTruth(false),
        m_doGlobalFit(true),
        m_doSAFit(false),
        m_doMuonFeature(false),
        m_doANNSelection(true),
        m_doParticleCreator(false),
        m_pClusCollection(NULL),
        m_pCandNTuple(NULL),
        // Tool names
        m_pEventStore("StoreGateSvc", name),
        m_pCandidate("MuGirlNS::CandidateTool"),
  //        m_pToCalo("None"),
        m_pToCalo("ExtrapolateToCaloTool/ExtrapolateToCaloTool"),
        m_pMuLHR("CaloMuonLikelihoodTool"),
        m_pTruthTool("MuGirlNS::PerformanceTruthTool",0),                 // make this a public tool
  //        m_particleCreatorTool("Trk::TrackParticleCreatorTool"),
        //m_runOutlier(false),
        //m_matEffects(Trk::muon),
        m_pGlobalFitTool("MuGirlNS::GlobalFitTool",0),                    // make this a public tool
        m_pMuonFeatureGlobalFitTool("MuGirlNS::GlobalFitTool",0),         // make this a public tool
        m_pANNSelectionTool("MuGirlNS::ANNSelectionTool",0),              // make this a public tool
        m_pParticleCreatorTool("MuGirlNS::MuGirlParticleCreatorTool",0),  // make this a public tool
        //m_intersector("Trk::RungeKuttaIntersector")
        //<S>
        m_pStauTool("MuGirlNS::StauTool",0),                              // make this a public tool
        m_magFieldSvc("MagField::AtlasFieldSvc/AtlasFieldSvc",name),
        m_pSegmentManager(NULL),
  // for perform
        m_pTrigMuGirlNtuple(NULL),
        m_ntupleNamePerformance("/NTUPLES/TRIGMUGIRL/TRIGMUGIRL"),
        m_ntupleTitlePerformance("TrigMuGirlPer"),
        m_doNTuplePerformance(false),
        m_roi(0)
 
{

    // configure property values
    declareProperty("PtLowerLimit",                 m_ptLowerLimit);
    declareProperty("NtupleName",                   m_ntupleName);
    declareProperty("NtupleTitle",                  m_ntupleTitle);
    declareProperty("RoadDeltaPhi",                 m_roadDeltaPhi);
    declareProperty("doNTuple",                     m_doNTuple);
    declareProperty("doLHR",                        m_doLHR);
    declareProperty("doCSC",                        m_doCSC);
    declareProperty("BarrelNNCut",                  m_barrelNNCut);
    declareProperty("EndcapNNCut",                  m_endcapNNCut);
    declareProperty("SegmentCollection",            m_sSegmentCollection);
    declareProperty("RefittedTrkCollection",        m_sRefittedTrkCollection);
    declareProperty("RefittedTrkLocation",          m_RefittedTrkLocation);
    declareProperty("InDetTrackParticlesLocation",  m_inDetParticlesLocation);
    declareProperty("doStau",                       m_doStau);
    declareProperty("StauPtCut",                    m_stauPtCut);
    declareProperty("MFPtCut",                    m_mfPtCut);
    declareProperty("IdConeR",                      m_idR);
    declareProperty("LHRValue",                     m_lhr);
    declareProperty("eCaloOverPCut",                m_eOverP);
    declareProperty("InCaloCollection",             m_CaloCollection);
    declareProperty("doTruth",                      m_doTruth);
    declareProperty("doGlobalFit",                  m_doGlobalFit);
    declareProperty("doSAFit",                      m_doSAFit);
    declareProperty("doMuonFeature",                m_doMuonFeature);
    declareProperty("doANNSelection",               m_doANNSelection);
    declareProperty("doParticleCreator",            m_doParticleCreator);
    // Tools
    // in the old ver there was no
    declareProperty("StoreGateSvc",                 m_pEventStore);
    declareProperty("MuGirlCandidate",              m_pCandidate);
    declareProperty("MuGirlGlobalFIT",              m_pGlobalFitTool);
    declareProperty("MuGirlGlobalStauFIT",          m_pStauGlobalFitTool);
    declareProperty("MuGirlGlobalMuonFeatureFIT",   m_pMuonFeatureGlobalFitTool);
    declareProperty("ExtrapolTrackToCaloTool",      m_pToCalo);
    declareProperty("CaloMuonLikelihoodTool",       m_pMuLHR);
    //    declareProperty("TrackParticleCreatorTool", m_particleCreatorTool);
    //<S>
    declareProperty("MuGirlStauTool",               m_pStauTool);
    declareProperty("PerformanceTruthTool",         m_pTruthTool);
    declareProperty("ANNSelectionTool",             m_pANNSelectionTool);
    declareProperty("MuGirlParticleCreatorTool",             m_pParticleCreatorTool);
    // varibles for monitor
    //    declareMonitoredStdContainer ("MuonsPt", pt);
    //    declareMonitoredStdContainer ("MuonsCosTh", cotTh);
    //    declareMonitoredStdContainer ("MuonsPhi", phi);

    declareMonitoredStdContainer ("MuonsPt", muon_pT);
    declareMonitoredStdContainer ("MuonsCotTh", muon_cotTh);
    declareMonitoredStdContainer ("MuonsPhi", muon_phi);
    declareMonitoredStdContainer ("MuonsMdtHits", muon_nMDThits);
    declareMonitoredStdContainer ("MuonsRpcPhiHits", muon_nRPCPhihits);
    declareMonitoredStdContainer ("MuonsRpcEtaHits", muon_nRPCEtahits);
    declareMonitoredStdContainer ("MuonsCscPhiHits", muon_nCSCPhihits);
    declareMonitoredStdContainer ("MuonsCscEtaHits", muon_nCSCEtahits);
    declareMonitoredStdContainer ("MuonsTgcPhiHits", muon_nTGCPhihits);
    declareMonitoredStdContainer ("MuonsTgcEtaHits", muon_nTGCEtahits);
    declareMonitoredStdContainer ("MuonsTgcRpcPhiHits", muon_nTGCRPCPhihits);
    declareMonitoredStdContainer ("MuonsTgcRpcEtaHits", muon_nTGCRPCEtahits);
    declareMonitoredStdContainer ("MuonsMdtHitsInSeg", muon_nMDThitsInseg);
    declareMonitoredStdContainer ("MuonsRpcHitsInSeg", muon_nRPChitsInseg);
    declareMonitoredStdContainer ("MuonsTgcHitsInSeg", muon_nTGChitsInseg);
    declareMonitoredStdContainer ("MuonsTgcRpcHitsInSeg", muon_nTGCRPChitsInseg);
    declareMonitoredStdContainer ("MuonsMdtSegs", muon_nMDTsegs);
    declareMonitoredStdContainer ("MuonsRpcSegs", muon_nRPCsegs);
    declareMonitoredStdContainer ("MuonsCscSegs", muon_nCSCsegs);
    declareMonitoredStdContainer ("MuonsTgcSegs", muon_nTGCsegs);
    declareMonitoredStdContainer ("MuonsTgcRpcSegs", muon_nTGCRPCsegs);
    declareMonitoredStdContainer ("MuonsBeta", muon_beta);
    declareMonitoredStdContainer ("MuonsNNBarrel", muon_nnBarrel);
    declareMonitoredStdContainer ("MuonsNNEndcap", muon_nnEndcap);
    declareMonitoredStdContainer ("MuonsRPCHitsMeasTime", muon_RPCHitsMeasTime);
    declareMonitoredStdContainer ("MuonsRPCHitsPropTime", muon_RPCHitsPropTime);
    declareMonitoredStdContainer ("MuonsRPCHitsTof", muon_RPCHitsTof);
    declareMonitoredStdContainer ("MuonsRPCHitsDist", muon_RPCHitsDis);
    // for perform
    declareProperty("doNTuplePerformance",  m_doNTuplePerformance  = false);
    declareProperty("doTruthPerformance",   m_doTruthPerformance   = false);
    
}

//** ----------------------------------------------------------------------------------------------------------------- **//


TrigMuGirl::~TrigMuGirl() {delete m_pSegmentManager;}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigMuGirl::hltInitialize() {
  
  if (msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "Initializing TrigMuGirl " << endreq;    

  //  StatusCode status;
  //  StatusCode sc = StatusCode::SUCCESS;

  //int timers
  m_timerExecTotal = addTimer("eExecTotal");
  
    m_pSegmentManager = new MuGirlNS::SegmentManager;
    if (m_magFieldSvc.retrieve().isFailure())
    {
        msg(MSG::ERROR) <<"Could not retrieve magFieldSvc"<<endreq;
        return StatusCode::RECOVERABLE;
    }

/*
    ServiceHandle<StoreGateSvc> pDetStore("DetectorStore", name());
    if (retrieve(pDetStore).isFailure())
        return StatusCode::RECOVERABLE;
    if (pDetStore->retrieve(m_pMuonMgr).isFailure() || m_pMuonMgr == NULL)
    {
      msg() << MSG::ERROR << "Cannot retrieve MuonDetectorManager" << endreq;
        return StatusCode::RECOVERABLE;
    }

*/
  if (retrieve(m_pEventStore).isFailure())
    //      return StatusCode::RECOVERABLE;
    msg() << MSG::DEBUG << "Failed to get the StoreGate" << endreq;    

  // Initialize tools
  if (retrieve(m_pToCalo, false).isFailure())
    {
      msg() << MSG::DEBUG << "Failed to get pToCalo" << endreq;    
      m_pToCalo = NULL;
    }
  
  if (m_doTruth)
    {
      if (retrieve(m_pTruthTool).isFailure())
	//            return StatusCode::RECOVERABLE;
	msg() << MSG::DEBUG << "Failed to get the TruthTool" << endreq;    
    }

  if (m_doLHR)
    {
      if (retrieve(m_pMuLHR, false).isFailure())
	{
	  msg() << MSG::DEBUG << "Failed to get the pMuLHR" << endreq;    
	  m_pMuLHR = NULL;
	}
    }
  
  if (m_doStau)
    {
      if (retrieve(m_pStauTool).isFailure())
	{
	  msg() << MSG::DEBUG << "Failed to get the StauTool" << endreq;    
	  //            return StatusCode::RECOVERABLE;
	}
        m_pStauTool->setSegmentManager(m_pSegmentManager);
    }
  
  if (m_doGlobalFit )
    {
      if (retrieve(m_pGlobalFitTool, false).isFailure())
	{
	  msg() << MSG::DEBUG << "Failed to get the GlobalFitTool" << endreq;
	  m_pGlobalFitTool = NULL;
	}
    }
  if (m_doMuonFeature && m_doSAFit )
    {
      if (retrieve(m_pMuonFeatureGlobalFitTool, false).isFailure())
	{
	  msg() << MSG::DEBUG << "Failed to get the GlobalFitTool for SA fit" << endreq;
	  m_pMuonFeatureGlobalFitTool = NULL;
	}
    }

  if (m_doANNSelection)
    {
      if (retrieve(m_pANNSelectionTool, false).isFailure())
	{
	  msg() << MSG::DEBUG << "no m_pANNSelection" << endreq;    
	  m_pANNSelectionTool = NULL;
	}
    }
  
    // this is done anyway to have the function removeRedundantCandidates avilable
    //    if (m_doParticleCreator)
    {
      if (retrieve(m_pParticleCreatorTool).isFailure())
	{
	  msg()<<MSG::DEBUG<<"no m_pParticleCreatorTool"<<endreq;    
	  m_pParticleCreatorTool = NULL;
	}
    }

    m_caloLayers.push_back( CaloCell_ID::PreSamplerB );
    m_caloLayers.push_back( CaloCell_ID::EMB1 );
    m_caloLayers.push_back( CaloCell_ID::EMB2 );
    m_caloLayers.push_back( CaloCell_ID::EMB3 );
    m_caloLayers.push_back( CaloCell_ID::TileBar0 );
    m_caloLayers.push_back( CaloCell_ID::TileBar1 );
    m_caloLayers.push_back( CaloCell_ID::TileBar2 );
    m_caloLayers.push_back( CaloCell_ID::PreSamplerE );
    m_caloLayers.push_back( CaloCell_ID::EME1 );
    m_caloLayers.push_back( CaloCell_ID::EME2 );
    m_caloLayers.push_back( CaloCell_ID::EME3 );
    m_caloLayers.push_back( CaloCell_ID::HEC0 );
    m_caloLayers.push_back( CaloCell_ID::HEC1 );
    m_caloLayers.push_back( CaloCell_ID::HEC2 );
    m_caloLayers.push_back( CaloCell_ID::FCAL0 );
    m_caloLayers.push_back( CaloCell_ID::FCAL1 );
    m_caloLayers.push_back( CaloCell_ID::FCAL2 );
    m_caloLayers.push_back( CaloCell_ID::TileExt0 );
    m_caloLayers.push_back( CaloCell_ID::TileExt1 );
    m_caloLayers.push_back( CaloCell_ID::TileExt2 );


    if (retrieve(m_pCandidate).isFailure())
      {
	msg()<<MSG::ERROR<<"Could not get pCandidate"<<endreq;
	return StatusCode::RECOVERABLE;
      }
    m_pCandidate->setSegmentManager(m_pSegmentManager);

    // Initialize NTuple
    if (m_doNTuple)
    {
      msg()<<MSG::DEBUG<<"in doNTuple"<<endreq;

      m_pCandNTuple = ntupleSvc()->book(m_ntupleName, CLID_ColumnWiseTuple, m_ntupleTitle);
      if (m_pCandNTuple == NULL)
      {
	  msg() << MSG::ERROR << "Could not create NTuple " << m_ntupleName << endreq;
	  return StatusCode::RECOVERABLE;
      }
      if (m_pCandidate->bookNTuple(m_pCandNTuple).isFailure())
      {
	  msg() << MSG::ERROR << "Could not book Candidate NTuple " << m_ntupleName << endreq;
	  return StatusCode::RECOVERABLE;
      }

      if(m_doStau)
      {
	  if(m_pStauTool->bookNTuple(m_pCandNTuple).isFailure())
            {
	      msg() << MSG::ERROR << "Could not book Stau NTuple " << m_ntupleName << endreq;
	      return StatusCode::RECOVERABLE;
            }
      }
      if(m_doMuonFeature && m_doSAFit)
      {
           if (m_pMuonFeatureGlobalFitTool->bookNTuple(m_pCandNTuple,"StandAlone").isFailure())
	       msg() << MSG::ERROR << "Could not book GlobalFit NTuple for StandAlone fit" << m_ntupleName << endreq;
           else 
	       msg() << MSG::DEBUG << "Book GlobalFit NTuple for StandAlone fit" << m_ntupleName << endreq;
      }
      if (m_doTruth&& m_pTruthTool->bookNTuple(m_pCandNTuple).isFailure())
      {
	  msg() << MSG::ERROR << "Could not book Truth NTuple " << m_ntupleName << endreq;
	  return StatusCode::RECOVERABLE;
      }
      if (m_doGlobalFit&& m_pGlobalFitTool->bookNTuple(m_pCandNTuple,"").isFailure())
      {
            msg() << MSG::ERROR << "Could not book GlobalFit NTuple " << m_ntupleName << endreq;
            return StatusCode::RECOVERABLE;
      }
    }

    // this shouldn't work but we can work without it too
    //    if (retrieve(m_particleCreatorTool).isFailure())
    //      {
    //	msg()<<MSG::ERROR<<"Could not get m_particleCreatorTool"<<endreq;
    //	//return StatusCode::RECOVERABLE;
    //      }

    if (m_doNTuplePerformance)
      {
        m_pTrigMuGirlNtuple = new TrigMuGirlNtuple(this, ntupleSvc());
        StatusCode SCstatus = m_pTrigMuGirlNtuple->book(m_ntupleNamePerformance, m_ntupleTitlePerformance);
        if (SCstatus.isFailure() || m_pTrigMuGirlNtuple == NULL)
	  {
            msg()<< MSG::ERROR << "Could not book NTuple " << m_ntupleName << endreq;
            return HLT::BAD_JOB_SETUP;
	  }
      }


    if (msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "Initializing TrigMuGirl done successfully" << endreq;    
    return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigMuGirl::hltFinalize() {

  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Finalizing TrigMuGirl" << endreq;

    return HLT::OK;
}

//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigMuGirl::hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE) {

  if (doTiming()) 
    {
      m_timerExecTotal->start();
      //      m_timerTotal1->start();
      //      m_timerAllFun->start();
      //      m_timerAllFun->pause();
    }
    ++m_roi;
  //  if (msg().level() <= MSG::DEBUG) msg() << MSG::DEBUG << "Executing TrigMuGirl <--*--> hello erez" << endreq;
    if (m_doNTuplePerformance)
      {
	eRunNumber=0;
	eEventNumber=0;
	NN[0]=0;
	NN[1]=0;
	NN[2]=0;
	NN[3]=0;
	NN[4]=0;
	NN[5]=0;
      }

  //  StatusCode status;
  StatusCode sc = StatusCode::SUCCESS;

  const EventInfo* pEventInfo = NULL;
  sc = m_pEventStore->retrieve(pEventInfo);
  if (sc.isFailure() || pEventInfo == NULL)
	msg() << MSG::WARNING << "Cannot retrieve EventInfo" << endreq;
  else if (m_doNTuplePerformance)
  {
	eRunNumber   = pEventInfo->event_ID()->run_number();
	eEventNumber = pEventInfo->event_ID()->event_number();
  }

  if ( msgLvl() <= MSG::DEBUG ) 
    msg()<<MSG::DEBUG<<"Executing "<<name()<<" for types "
	 <<inputTE->getId()<<" -> "<<outputTE->getId()<<endreq;
  
  //m_pRefittedTrkCollection = new TrackCollection();
  //m_pRefittedTrkContainer = new Rec::TrackParticleContainer();
  //m_pSegmentCollection = new Trk::SegmentCollection();

  HLT::ErrorCode status;
  MuGirlNS::CandidateSummaryList summaryList;

  /** initialize old EDM still used by the HLT Muon Trigger Elemen */
  ElementLinkVector<Rec::TrackParticleContainer> elv_idtrks;

/* sofia */

  if (m_doMuonFeature)
  {
     std::vector<const MuonFeature*> vectorOfMuonFeature;
     status = getFeatures(inputTE, vectorOfMuonFeature, "");
     if (status != HLT::OK)
        goto IDtrack;

 
     int iMuF=0;  
     if (vectorOfMuonFeature.size() != 0)
     {
       m_pStauTool->initTool(m_doSAFit);

       if (msg().level() <= MSG::DEBUG)
           msg()<<MSG::DEBUG<<"Start MuonFeature processing for "<<vectorOfMuonFeature.size()<<endreq;
     
          //const MuonFeature* pMuonFeature = vectorOfMuonFeature.front();
       for(unsigned int i=0; i<vectorOfMuonFeature.size(); i++)
     
       {
          const MuonFeature* pMuonFeature = vectorOfMuonFeature[i];
          if (!pMuonFeature)
          {
             msg() << MSG::ERROR
                   << "Retrieval of MuonFeature from vector failed!"
                   << endreq;
             continue;
          }
          if (fabs(pMuonFeature->pt())<m_mfPtCut)  //MuonFeature momentum is in GeV
             continue;

          bool has_combined = false;

          ElementLink<CombinedMuonFeatureContainer> CombinedMuonFeatureEL;
          if ( HLT::OK == getFeatureLink<CombinedMuonFeatureContainer, 
                   CombinedMuonFeature>(inputTE, CombinedMuonFeatureEL) )            
          { 
             if ( CombinedMuonFeatureEL.isValid() )     
             { 

                const CombinedMuonFeature* pMuon = *CombinedMuonFeatureEL; 
                if(pMuon)
                {
                  ElementLink<MuonFeatureContainer> mfastEL = pMuon->muFastTrackLink();
                  ElementLink<TrigInDetTrackCollection> trackEL = pMuon->IDTrackLink();           
                  if (!mfastEL) continue;

                  const MuonFeature* pMF = *mfastEL;
                   
                  if (!pMF) continue; 
                  if (pMF == pMuonFeature && trackEL.isValid())                                                          
                  { 
                    if (msg().level() <= MSG::DEBUG)
                        msg()<<MSG::DEBUG<<"HAS IDTrack! moving to IDTrack processing"<<endreq;
                    has_combined = true;
                    continue;
                  }
                }
             }
          } 
 
          if (m_pCandidate->fillMF(iMuF, pMuonFeature, true,has_combined).isFailure()) 
          {
             msg()<< MSG::DEBUG << "MuonFeature Cannot fill CANDIDATE" << endreq;
             continue;
          }
          fillMuonSegmentColl(m_pCandidate->vectorMuonSegments());

          MuGirlNS::CandidateSummary* summary_mf = new MuGirlNS::CandidateSummary();
          if ( msgLvl() <= MSG::DEBUG ) 
          {
             msg()<< MSG::DEBUG << "MuonFeature "<<pMuonFeature->pt()<<" eta "<<pMuonFeature->eta()<<" phi "<<pMuonFeature->phi()  << endreq;
             msg()<< MSG::DEBUG << "MuonFeature fill SUMMARY" << endreq;
          }

          if (m_pCandidate->fillSummary(summary_mf).isFailure())
          {
              msg()<< MSG::DEBUG << "MuonFeature Cannot fill SUMMARY" << endreq;
              continue;
          }

          if (m_doNTuple)
          {
             if (pEventInfo != NULL)
             {
                if ((m_pCandidate->fillNTupleEventInfo(pEventInfo->event_ID()->run_number(),
                    pEventInfo->event_ID()->event_number())).isFailure())
                    msg() << MSG::WARNING << "MuonFeature::execute Cannot fill NTuple" << endreq;
             }
	      if (msg().level() <= MSG::DEBUG)
	          msg() << MSG::DEBUG << "Calling m_pCandidate->fillNTuple for MF" << endreq;
             if (m_pCandidate->fillNTuple().isFailure())
                 msg() << MSG::WARNING << "MuonFeature::execute Cannot fill NTuple" << endreq;
             if (m_doTruth&&pMuonFeature!=0)
             {
                 msg() << MSG::DEBUG << "Calling m_pTruthTool->fillNTuple for MuonFeature processing" << endreq;
                 if (m_pTruthTool->fillNTuple(summary_mf).isFailure())
                     msg() << MSG::WARNING << "TrigMuGirl::execute Cannot fill Truth in NTuple for Muonfeature processing" << endreq;
             }
          }
          
          if ( msgLvl() <= MSG::DEBUG ) 
              msg()<< MSG::DEBUG << "MuonFeature do Stau hits" << summary_mf->numMdtHits
                   <<" "<<summary_mf->numTgcEtaHits<<" "<<summary_mf->numTgcPhiHits
                   <<" "<<summary_mf->numRpcEtaHits<<" "<< summary_mf->numRpcPhiHits
                   << endreq;
          if (summary_mf->numMdtHits >4 && 
             (summary_mf->numTgcEtaHits>2 || summary_mf->numTgcPhiHits>2 || summary_mf->numRpcEtaHits>2 || summary_mf->numRpcPhiHits>2))
          {
             if ( msgLvl() <= MSG::DEBUG ) 
                msg()<< MSG::DEBUG << "MuonFeature do Stau" << endreq;
             summary_mf->saveStau = 1;
             m_pCandidate->setPassdoStau(true);

             const MuGirlNS::MdtSegmentMakerInfoList mdtSegmentMakerInfoList = m_pCandidate->getMdtSegmentMakerInfo();
             const MuGirlNS::RIO_OnTrackLists& rpcHitsInSegments = m_pCandidate->getRpcHitsInSegments();
             const MuGirlNS::RIO_OnTrackLists& tgcHitsInSegments = m_pCandidate->getTgcHitsInSegments();

             float mf_cot  = 1/tan(2*atan(exp (-pMuonFeature->eta())));
             float mf_px   = 1000*fabs(pMuonFeature->pt())*cos(pMuonFeature->phi());
             float mf_py   = 1000*fabs(pMuonFeature->pt())*sin(pMuonFeature->phi());
             float mf_pz   = 1000*fabs(pMuonFeature->pt())*mf_cot;
             //double mf_p    = sqrt(mf_px*mf_px+ mf_py*mf_py + mf_pz*mf_pz);
             Amg::Vector3D v(mf_px,mf_py,mf_pz);
             if ( m_pStauTool->processStau(NULL,m_pCandidate->vectorMuonSegments(),mdtSegmentMakerInfoList,rpcHitsInSegments,tgcHitsInSegments,NULL,NULL,v).isFailure())
                  msg() << MSG::WARNING << "MuonFeature::cannot execute processStau" << endreq;
             for(std::vector<const Muon::MuonSegment*>::const_iterator itSegs = (*m_pStauTool->newMdtSegments()).begin(); itSegs != (*m_pStauTool->newMdtSegments()).end(); itSegs++)
             {
                 m_pSegmentManager->addSegment(*itSegs, MuGirlNS::SegmentManager::MDT_STAU);
             }
             fillMuonSegmentColl(*m_pStauTool->newMdtSegments());
             if (m_pStauTool->beta() <0.2 || m_pStauTool->beta()>1.2)
             {
                 delete m_pStauTool->refittedTrack();
                 continue;
             }  
             summary_mf->beta = m_pStauTool->beta();

             if (m_doNTuple)
             {
                if (m_pStauTool->fillNTuple().isFailure())
                    msg() << MSG::WARNING << "MuonFeature::execute Cannot fill Stau NTuple" << endreq;
             }

             MuGirlNS::CandidateSummary* stauSummary = new MuGirlNS::CandidateSummary();

             if (m_pStauTool->fillStauSummary(summary_mf,stauSummary).isFailure())
                 msg() << MSG::DEBUG << "MuonFeature::execute Cannot fill stausummary" << endreq;
             stauSummary->qOverP = summary_mf->qOverP;
             stauSummary->startFromMF = 1;
             bool ANNstau = generateMuGirl(stauSummary);

             msg() << MSG::DEBUG << "MuonFeature::execute ANNstau"<< ANNstau << endreq;
             //if (ANNstau)
            // {
             stauSummary->saveStau = 1;
             summaryList.push_back(stauSummary);
             //}
	     if ( m_doSAFit && stauSummary->muonSegmentList.size()>0)
	     //if ( m_doSAFit && m_pCandidate->vectorMuonSegments().size()>0)
             {
               msg() << MSG::DEBUG << "MuonFeature:: stau segments "<<stauSummary->muonSegmentList.size()
                                   << " Muon segments "<<m_pCandidate->vectorMuonSegments().size()<< endreq;
               const Trk::Track* RefittedTrack = 0;
               RefittedTrack = m_pMuonFeatureGlobalFitTool->globalFit(stauSummary->muonSegmentList,summary_mf->qOverP,m_doNTuple);
               //RefittedTrack = m_pMuonFeatureGlobalFitTool->globalFit(m_pCandidate->vectorMuonSegments(),summary_mf->qOverP,m_doNTuple);

                 if (RefittedTrack != NULL)
                 {
                    ATH_MSG_DEBUG("found SA track");
                    /** add only the refitted track to the MuGirl summary */
                    xAOD::TrackParticle* tp = NULL;
                    m_pCandidate-> fillRefittedTrack(tp,RefittedTrack,stauSummary);
                //  m_pRefittedTrkCollection->push_back(const_cast<Trk::Track*> (RefittedTrack));
                 }
                 else
                 { 
                    delete RefittedTrack;
                    m_pCandidate-> fillRefittedTrack(NULL,NULL,stauSummary);
                 }
             }
          }   // enter stau selection

          iMuF++;
       } // loop on vector of MuonFeatures
    }   // if vectorOfMuonFeature.size()>0
    if (m_doNTuple)
    {
       if (m_pCandNTuple->writeRecord().isFailure())
           msg() << MSG::WARNING << "MuonFeature::execute Cannot write NTuple" << endreq;
    }

    if (msg().level() <= MSG::DEBUG)
       msg() << MSG::DEBUG << "MF processing is done"<<endreq;
      
   }   //if doMuonFeature

   IDtrack:
   m_pStauTool->initTool(m_doGlobalFit);
   const VxContainer* vxCandidatesContainer = 0;
   int iTrack = 0;
   // for performance ntuple
   float TrackIso=-1;


   std::vector<const xAOD::CaloClusterContainer*> vectorOfClusterContainers;
    
    //if(getFeatures(inputTE, vectorOfClusterContainers,"HLT_TrigCaloClusterMaker")!=HLT::OK) 
    if(getFeatures(inputTE, vectorOfClusterContainers,"HLT")!=HLT::OK) 
      {
        if(msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Failed to get ClusterContainers" << endreq;
        //      return "";
      }
    if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Got vector with " << vectorOfClusterContainers.size()  << " ClusterContainers" << endreq;
    
    // if no containers were found, just leave the vector empty and leave
    if ( vectorOfClusterContainers.size() < 1) {
      if(msgLvl() <= MSG::DEBUG) 
        msg() << MSG::DEBUG << "No Cluster container available !" << endreq;
      //      return "";
    }
    else
      {
        // get last container to be put in vector (should also be the only one)
        m_pClusCollection = vectorOfClusterContainers.back();
        if (!m_pClusCollection)
          {
            if(msgLvl() <= MSG::WARNING) 
              msg() << MSG::WARNING << "No Clusters found in the RoI" << endreq;
            //  return "";
          }
      }

    if (!m_magFieldSvc->solenoidOn()) 
    { 
        if (msg().level()<=MSG::DEBUG) msg()<<MSG::DEBUG << "MuGirl::Solenoid is off. Execute done" << endreq; 
        goto notrack;
     } 
      else
        if (msg().level()<=MSG::DEBUG) msg()<<MSG::DEBUG << "MuGirl::Solenoid is ON" << endreq; 

   // get element links to the ID tracks
   m_inDetTrackParticleLinks.clear();
   if( HLT::OK != getFeaturesLinks<Rec::TrackParticleContainer,Rec::TrackParticleContainer>(inputTE, elv_idtrks, "") ) {
    msg() << MSG::WARNING << "Failed to get ElementLinkVector<Rec::TrackParticleContainer>, aborting this ROI" << endreq;
    goto notrack;
  }

    // out for timing
    //    if (msg().level() <= MSG::DEBUG)
    //        msg() << MSG::DEBUG << "Size of TrackParticleCandidate = " << m_inDetTrackParticleLinks->size() << endreq;
   if (m_pEventStore->contains<VxContainer>("VxPrimaryCandidate"))
   {
      sc = m_pEventStore->retrieve(vxCandidatesContainer, "VxPrimaryCandidate");
       if (sc.isFailure())
       {
          if (msg().level() <= MSG::DEBUG) 
              msg()<<MSG::DEBUG << "Cannot retrieve " << "VxPrimaryCandidate" << endreq;
       }
    }
    else
         if (msg().level() <= MSG::DEBUG)
               msg()<<MSG::DEBUG << "Cannot retrieve " << "VxPrimaryCandidate" << endreq;

    if (m_doNTuplePerformance)
    {
	eRunNumber   = pEventInfo->event_ID()->run_number();
	eEventNumber = pEventInfo->event_ID()->event_number();
    }
    // in case we want to look for Calo information
    /*
    std::vector<const CaloClusterContainer*> vectorOfClusterContainers;
    
    if(getFeatures(inputTE, vectorOfClusterContainers, "")!=HLT::OK) 
      {
	if(msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Failed to get ClusterContainers" << endreq;
	//	return "";
      }
    if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Got vector with " << vectorOfClusterContainers.size()  << " ClusterContainers" << endreq;
    
    // if no containers were found, just leave the vector empty and leave
    if ( vectorOfClusterContainers.size() < 1) {
      if(msgLvl() <= MSG::DEBUG) 
	msg() << MSG::DEBUG << "No Cluster container available !" << endreq;
      //      return "";
    }
    else
      {
	// get last container to be put in vector (should also be the only one)
	m_pClusCollection = vectorOfClusterContainers.back();
	if (!m_pClusCollection)
	  {
	    if(msgLvl() <= MSG::WARNING) 
	      msg() << MSG::WARNING << "No Clusters found in the RoI" << endreq;
	    //	return "";
	  }
      }
    */
    /*  
    const DataHandle<CaloClusterContainer> resultContainer, lastContainer;
    sc =m_pEventStore->retrieve(resultContainer, lastContainer);
    if (sc.isFailure()) {
      msg() << MSG::DEBUG << " didn't find any CaloClusterContainer " << endreq;
    } else {
      msg() << MSG::DEBUG << " found  CaloClusterContainers " << endreq;
      int ncl=0;
      for (; resultContainer != lastContainer; ++resultContainer) {
	ncl++;	
	std::string name(resultContainer.key());
	msg() << MSG::DEBUG << "Container "<< resultContainer.key()<< " ncl= "<<ncl<<endreq;
	//	if( name.find("TrigCaloClusterMaker") != std::string::npos ){
	if( name.find("HLT_TrigCaloClusterMaker") != std::string::npos ){
	  msg() << MSG::DEBUG <<  " in if ncl= "<<ncl<<endreq;
 	  m_pClusCollection = resultContainer;
 	  msg() << MSG::DEBUG << "clus size = "<<m_pClusCollection->size()<<endreq;


         }
      }
    }
*/


    createCaloParticles();
      // out for timing
      //    if (msg().level() <= MSG::DEBUG)
      //      msg() << MSG::DEBUG << "Size of CaloParticles " << m_caloParticles.size() << endreq;
    
    IsolatedIdTracks(m_idR);


    for (unsigned int i = 0; i < m_caloParticles.size(); i++)
    {
        CaloParticle* pParticle = m_caloParticles[i];
        const xAOD::TrackParticle* pTrackParticle = pParticle->pTrackParticle;
        const Trk::TrackParameters* extrMuonLayer = pParticle->pTrackParameters;

        bool doStau = m_doStau && pTrackParticle->perigeeParameters().pT() >= m_stauPtCut;
        msg() << MSG::DEBUG << "TrigMuGirl::execute doStau "<<doStau << endreq;
        if (!m_pCandidate->fill(iTrack, pTrackParticle, extrMuonLayer, doStau).isSuccess())
        {
            msg() << MSG::DEBUG << "TrigMuGirl::execute failed candidate->fill " << endreq;
   
            fillMuonSegmentColl(m_pCandidate->vectorMuonSegments());
            continue;
        }
	//	if (m_pClusCollection != NULL)
	//	  {
	    //	    msg()<<MSG::DEBUG<<"pParticle->isolated ="<<pParticle->isolated<<endreq;
	    //	    if (m_doLHR && m_pMuLHR && m_pClusCollection->size() > 0)
	//	    if (m_doLHR && m_pMuLHR && pParticle->isolated && m_pClusCollection->size() > 0)
	//	      calculateLHR(pParticle);
	//	  }

	//for performance ntuple
	if (pParticle->isolated)
	  {
	    m_pCandidate->setIsolation(1);
	    m_pCandidate->setLHR(-2);
	  }
	TrackIso=pParticle->isolated;

        MuGirlNS::CandidateSummary* summary = new MuGirlNS::CandidateSummary();

        bool ANNaccept = false;
        if (m_pCandidate->fillSummary(summary).isFailure())
	{
            msg() << MSG::DEBUG << "TrigMuGirl::execute Cannot fill summary" << endreq;
	}
        else
	{
	   if (m_doANNSelection)
	   {
               ANNaccept = generateMuGirl(summary);
	   }
	}

	if ((m_doANNSelection && ANNaccept) || !(m_doANNSelection)) 
        {
	   summary->saveAnn = 1;

	   if (m_doGlobalFit&&pTrackParticle->track()!=NULL&&
	      (m_pCandidate->vectorMuonSegments()).size()>0)
            {
               const Trk::Track* RefittedTrack = 0;
               RefittedTrack = m_pGlobalFitTool->globalFit(pTrackParticle->track(),m_pCandidate->vectorMuonSegments(),m_doNTuple);
               if (RefittedTrack != NULL)
               {
                   //m_pRefittedTrkCollection->push_back(const_cast<Trk::Track*> (RefittedTrack));
                   //const xAOD::Vertex* pVxCandidate = 0 ;
		   //pVxCandidate = *(pTrackParticle->vertex());
                   xAOD::TrackParticle* tp = 0;

                   m_pCandidate-> fillRefittedTrack(tp,RefittedTrack,summary);
	       }
	       else
               {
		  delete RefittedTrack;
	          m_pCandidate-> fillRefittedTrack(NULL,NULL,summary);
               } 

	       if (msg().level() <= MSG::DEBUG)
	           msg() << MSG::DEBUG << "Global Fit was ran " << endreq;
           }  //if doGlobalFit

       }//if ANN     
       else
	  m_pCandidate-> fillRefittedTrack(NULL,NULL,summary);

       if (m_doNTuple)
       {
          if (pEventInfo != NULL)
          {
	      sc = m_pCandidate->fillNTupleEventInfo(pEventInfo->event_ID()->run_number(),
	    				             pEventInfo->event_ID()->event_number());

	      if (sc.isFailure())
	          msg() << MSG::WARNING << "TrigMuGirl::execute Cannot fill NTuple" << endreq;
	  }
	  if (msg().level() <= MSG::DEBUG)
	      msg() << MSG::DEBUG << "Calling m_pCandidate->fillNTuple" << endreq;
	    
	  if (m_pCandidate->fillNTuple().isFailure())
	      msg() << MSG::WARNING << "TrigMuGirl::execute Cannot fill NTuple" << endreq;
	  if (m_doNTuple&&m_doTruth&&pTrackParticle!=NULL)
	  {
	      if (msg().level() <= MSG::DEBUG)
	          msg() << MSG::DEBUG << "Calling m_pTruthTool->fillNTuple" << endreq;
	      if (m_pTruthTool->fillNTuple(summary).isFailure())
	          msg() << MSG::WARNING << "TrigMuGirl::execute Cannot fill Truth in NTuple" << endreq;
	  }
       } //if doNTuple
       fillMuonSegmentColl(summary->muonSegmentList);
        //<S>
	// needs to be updated - there is a branch
       bool passedStau = false;
       if(doStau && summary->numMdtHits >4 &&(summary->numTgcEtaHits>2 || summary->numTgcPhiHits>2 || summary->numRpcEtaHits>2 || summary->numRpcPhiHits>2))
       {   
	  m_pCandidate->setPassdoStau(true);
  //retrieve segment maker info list
	  const MuGirlNS::MdtSegmentMakerInfoList mdtSegmentMakerInfoList = m_pCandidate->getMdtSegmentMakerInfo();
	  //retrieve the rpc hits
	  const MuGirlNS::RIO_OnTrackLists& rpcHitsInSegments = m_pCandidate->getRpcHitsInSegments();
          const MuGirlNS::RIO_OnTrackLists& tgcHitsInSegments = m_pCandidate->getTgcHitsInSegments();

	  //process stau

          const Trk::Track* pMuonRefittedTrack = NULL;
          if (summary->pRefittedTrack != NULL)
              pMuonRefittedTrack = summary->pRefittedTrack->track(); 
          sc = m_pStauTool->processStau(pTrackParticle, m_pCandidate->vectorMuonSegments(), mdtSegmentMakerInfoList,rpcHitsInSegments,tgcHitsInSegments,pMuonRefittedTrack);
	  summary->beta = m_pStauTool->beta();

          for(std::vector<const Muon::MuonSegment*>::const_iterator itSegs = (*m_pStauTool->newMdtSegments()).begin(); itSegs != (*m_pStauTool->newMdtSegments()).end(); itSegs++)
          {
             m_pSegmentManager->addSegment(*itSegs, MuGirlNS::SegmentManager::MDT_STAU);
          }
	
          fillMuonSegmentColl(*m_pStauTool->newMdtSegments());
	  if (!sc.isFailure())
          {
             
              if (m_pStauTool->beta() <0.2 || m_pStauTool->beta() >1.2)
                 delete m_pStauTool->refittedTrack();
              else
              {
	          MuGirlNS::CandidateSummary* stauSummary = new MuGirlNS::CandidateSummary();
	          if (m_pStauTool->fillStauSummary(summary,stauSummary).isFailure())
	          msg() << MSG::DEBUG << "TrigMuGirl::execute Cannot fill stausummary" << endreq;

                  bool ANNstau = generateMuGirl(stauSummary);
                  if (stauSummary->nnBarrel >0.2 || stauSummary->nnEndCap >0.2) ANNstau = true;
                  msg()<<MSG::DEBUG << "MuGirl::execute: stau passANN "<<ANNstau << endreq;

                  const Trk::Track* stauRefittedTrack = m_pStauTool->refittedTrack();
                  if (stauRefittedTrack != NULL)
                  {
                      //const Trk::VxCandidate* pVxCandidate = 0 ;
                      //pVxCandidate=pTrackParticle->reconstructedVertex();
                      xAOD::TrackParticle* stauTp = NULL;

                      stauSummary->pRefittedTrack = stauTp;  
                      stauSummary->pTrkRefitted   = stauRefittedTrack; 
                  }
                  else
                  {
                      stauSummary->pRefittedTrack = NULL;    
                      stauSummary->pTrkRefitted   = NULL;
                      delete stauRefittedTrack;
                  }
	          if (m_doNTuple)
                  {
	              sc = m_pStauTool->fillNTuple();
	              if(sc.isFailure())
                          msg() << MSG::DEBUG << "TrigMuGirl::execute stauTool->fillNtuple failed " << endreq; 
                  }

                  if (ANNstau)
                  {
	              stauSummary->saveStau = 1;
              
	              passedStau =true;
	              summaryList.push_back(stauSummary);
                  }     
              }
          }
       } //if doStau

       msg() << MSG::DEBUG << "TrigMuGirl::execute passedStau "<<passedStau << endreq;
    
        if (summary->saveAnn == 1)
	{
	    summary->saveStau = 0;
            summaryList.push_back(summary);
	}
        else
	{
/*
            if (passedStau)
            {
                std::vector<const Muon::MuonSegment*> muonSegmentList_noMDT;
                for (MuGirlNS::MuonSegmentList::iterator itMuonSeg = (summary->muonSegmentList).begin();
                    itMuonSeg != (summary->muonSegmentList).end();
                    itMuonSeg++)
                {
                  if (m_pSegmentManager->isNotMDT(*itMuonSeg))
                      muonSegmentList_noMDT.push_back(*itMuonSeg);
                }
                //fillMuonSegmentColl(muonSegmentList_noMDT);
            }
            //else
	    //   fillMuonSegmentColl(summary->muonSegmentList);
 */
	    delete summary; 
        }

        if (m_doNTuple)
	{
            if (m_pCandNTuple->writeRecord().isFailure())
	      msg() << MSG::WARNING << "TrigMuGirl::execute Cannot write NTuple" << endreq;
	}
	// out for timing
	//	if (msg().level() <= MSG::DEBUG)
	//	  msg() << MSG::DEBUG << "Done processing track " << iTrack << endreq;
        iTrack++;

    }  //loop on CaloParticle


    // for perform
    if (m_doNTuplePerformance)
    {
        StatusCode statusfill = m_pTrigMuGirlNtuple->fillEvent(eEventNumber,eRunNumber);
	if (statusfill.isFailure())
	{
	    msg() << MSG::ERROR << " Cannot fill NTuple event # " << endreq;
	    return HLT::BAD_JOB_SETUP;
	}
    }

    
    //    if (msg().level() <= MSG::DEBUG) msg()<<MSG::DEBUG << "Number of summaries=" << summaryList.size() << endreq; 

    
    if (m_doTruthPerformance)
    {
	if (doMuTruthPerformance() != HLT::OK)
	  msg() << MSG::ERROR  << "The doMuTruth status is not Ok" << endreq;
    }

    notrack:

    //another output object option
    eMuonEFInfoCont = new TrigMuonEFInfoContainer();
    //added a container of ID tracks for future use
    eTrkParCont = new Rec::TrackParticleContainer;
 
   TrigMuonEFInfo* eMuoninfo = new TrigMuonEFInfo(m_roi);
   eMuonEFInfoCont->push_back(eMuoninfo);

   //TrigMuonEFInfoTrackContainer* infoTrkCont=eMuoninfo->TrackContainer();
   
    int tot_seg = 0; 
    int tot_mdt = 0; 
    int tot_csc = 0; 
    int tot_rpc = 0; 
    int tot_tgc = 0; 
   
    //clear buffers
    muon_pT.clear();
    muon_phi.clear();
    muon_cotTh.clear();
    muon_nMDThits.clear();
    muon_nRPCPhihits.clear();
    muon_nRPCEtahits.clear();
    muon_nCSCPhihits.clear();
    muon_nCSCEtahits.clear();
    muon_nTGCPhihits.clear();
    muon_nTGCEtahits.clear();
    muon_nTGCRPCPhihits.clear();
    muon_nTGCRPCEtahits.clear();
    muon_nMDThitsInseg.clear();
    muon_nRPChitsInseg.clear();
    muon_nTGChitsInseg.clear();
    muon_nTGCRPChitsInseg.clear();
    muon_nMDTsegs.clear();
    muon_nRPCsegs.clear();
    muon_nTGCsegs.clear();
    muon_nTGCRPCsegs.clear();
    muon_nCSCsegs.clear();
    muon_beta.clear();
    muon_nnBarrel.clear();
    muon_nnEndcap.clear();
    muon_RPCHitsMeasTime.clear();
    muon_RPCHitsPropTime.clear();
    muon_RPCHitsTof.clear();
    muon_RPCHitsDis.clear();

    std::vector<bool> IncludeList;
    MuGirlNS::CandidateSummaryList* SummaryList=&summaryList;

    m_pParticleCreatorTool->removeRedundantCandidates(SummaryList, IncludeList);

   int iCand = -1;
   int nMuons=0;
   for (MuGirlNS::CandidateSummaryList::iterator itSummary = SummaryList->begin(); itSummary !=SummaryList->end(); itSummary++)
     {
       iCand++;
       if (!IncludeList[iCand])
	  continue;
       const MuGirlNS::CandidateSummary* pSummary = *itSummary;
       if (!pSummary->startFromMF && pSummary->pTrackParticle ==NULL)
       {
          msg()<<MSG::DEBUG<<"This is not a RHadron candidate and ID track is NULL! candidate skipped"<< endreq;
          continue;
       }
       if (pSummary->saveAnn == 1 || pSummary->saveStau == 1)
       {
          muon_nMDThits.push_back(pSummary->numMdtHits);
          muon_nRPCPhihits.push_back(pSummary->numRpcPhiHits);
          muon_nRPCEtahits.push_back(pSummary->numRpcEtaHits);
          muon_nCSCPhihits.push_back(pSummary->numCscPhiHits);
          muon_nCSCEtahits.push_back(pSummary->numCscEtaHits);
          muon_nTGCPhihits.push_back(pSummary->numTgcPhiHits);
          muon_nTGCEtahits.push_back(pSummary->numTgcEtaHits);
          muon_nTGCRPCEtahits.push_back(pSummary->numTgcRpcEtaHits);
          muon_nTGCRPCPhihits.push_back(pSummary->numTgcRpcPhiHits);
 
          muon_nMDThitsInseg.push_back(pSummary->numMdtHitsInSeg);
          muon_nRPChitsInseg.push_back(pSummary->numRpcHitsInSeg);
          muon_nTGChitsInseg.push_back(pSummary->numTgcHitsInSeg);
          muon_nTGCRPChitsInseg.push_back(pSummary->numTgcRpcHitsInSeg);

          muon_nMDTsegs.push_back(pSummary->numMdtSegs);
          muon_nRPCsegs.push_back(pSummary->numRpcSegs);
          muon_nCSCsegs.push_back(pSummary->numCscSegs);
          muon_nTGCsegs.push_back(pSummary->numTgcSegs);
          muon_nTGCRPCsegs.push_back(pSummary->numTgcRpcSegs);

          muon_beta.push_back(pSummary->beta);
          muon_nnBarrel.push_back(pSummary->nnBarrel);
          muon_nnEndcap.push_back(pSummary->nnEndCap);
          MuGirlNS::StauRpcHitsLists* rpcHitsLists = pSummary->rpcHitsLists;
          if(NULL!=rpcHitsLists)
          {
               for(MuGirlNS::StauRpcHitsLists::iterator it = rpcHitsLists->begin(); it != rpcHitsLists->end(); it++)
               {
	               MuGirlNS::StauRpcHitsList* pRpcHits = *it;
	               for(MuGirlNS::StauRpcHitsList::iterator it1 = pRpcHits->begin(); it1 != pRpcHits->end(); it1++)
	               {
                           MuGirlNS::StauRpcHit* hit = (*it1);
	                   muon_RPCHitsMeasTime.push_back( hit->measuredTime);
	                   muon_RPCHitsPropTime.push_back( hit->propagationTime);
	                   muon_RPCHitsTof.push_back( hit->muonToF);
	                   muon_RPCHitsDis.push_back( hit->distance);
	               }
	
	       }
	
	  }
	  //const Rec::TrackParticle* etp;
          const Trk::Track* etp;

	  if (pSummary->pTrkRefitted==NULL)
          {   
              if (pSummary->startFromMF) continue;
              etp = pSummary->pTrackParticle->track();
          }
	  else etp = pSummary->pTrkRefitted;

          const Trk::TrackSummary* trackSummary = etp->trackSummary();
          int numhits_pix = trackSummary->get(Trk::numberOfPixelHits);
          int numhits_sct = trackSummary->get(Trk::numberOfSCTHits);
          int numhits_trt = trackSummary->get(Trk::numberOfSCTHits);


	  const Trk::Perigee* emp = etp->perigeeParameters();
	  double ePhi = emp->parameters()[Trk::phi];
	  double eTheta = 1/tan(emp->parameters()[Trk::theta]);
	  double theta = emp->parameters()[Trk::theta];
	  double ePt = 1/(emp->pT());
	  double eCharge = etp->perigeeParameters()->charge();
	  double eMass = 105.7;
	  if (pSummary->beta >0.0 && pSummary->beta < 1.0)
          {
              double BetaCand = pSummary->beta;
              double mom = (emp->pT())/sin(theta);
              eMass = mom * sqrt(1.-BetaCand*BetaCand)/BetaCand; 
          }

	  muon_pT.push_back(1/ePt);
	  muon_cotTh.push_back(eTheta);
	  muon_phi.push_back(ePhi);
	    
      //      msg() << MSG::DEBUG <<"ePt="<<ePt<< endreq;
      //      msg() << MSG::DEBUG <<"eTheta="<<eTheta<< endreq;
      //      msg() << MSG::DEBUG <<"ePhi="<<ePhi<< endreq;
      //      msg() << MSG::DEBUG <<"eCharge="<<eCharge<< endreq;
	   //	   msg() << MSG::DEBUG << " testiso pSummary->lhr = " <<pSummary->lhr<< endreq;
	   //	   msg() << MSG::DEBUG << " testiso TrackIso = " <<TrackIso<< endreq;

          TrigMuonEFInfoTrack * infoTrack = new TrigMuonEFInfoTrack();
          if (pSummary->pRefittedTrack!=NULL)
	     infoTrack->setMuonType(2);
          else
	     infoTrack->setMuonType(3);
  

          eMuoninfo->addTrack(infoTrack);
	   // another output object option
	  TrigMuonEFCbTrack* combinedTrk = infoTrack->CombinedTrack();
	  combinedTrk->setPerigee(ePt,eTheta,ePhi,eMass);
	  combinedTrk->setCharge(eCharge);
          combinedTrk->setNIdSctHits(numhits_sct); 
          combinedTrk->setNIdPixelHits(numhits_pix); 
          combinedTrk->setNTrtHits(numhits_trt);
           
          combinedTrk->setNMdtHits(pSummary->numMdtHits);
          combinedTrk->setNRpcHitsPhi(pSummary->numRpcPhiHits);
          combinedTrk->setNRpcHitsEta(pSummary->numRpcEtaHits);
          combinedTrk->setNTgcHitsPhi(pSummary->numTgcPhiHits);
          combinedTrk->setNTgcHitsEta(pSummary->numTgcEtaHits);
          combinedTrk->setNCscHitsPhi(pSummary->numCscPhiHits);
          combinedTrk->setNCscHitsEta(pSummary->numCscEtaHits);
          if (!pSummary->startFromMF)
          {
	     bool foundellink=false;
	     // find track particle corresponding to the ID track used in the combined muon
	     for(ElementLinkVector<xAOD::TrackParticleContainer>::const_iterator elit=m_inDetTrackParticleLinks.begin(); 
		 elit!=m_inDetTrackParticleLinks.end(); ++elit) {
	       if ( !elit->isValid() ) {
		 ATH_MSG_WARNING("Invalid EF ID TrackParticle ElementLink");
		 continue;
	       }
	       if(pSummary->pTrackParticle==(**elit)) {
		 if ( msgLvl(MSG::DEBUG) ) msg() << MSG::DEBUG << "Assigning TrackParticle" << endreq;
		 // removed until we get new EDM that supports xAOD  ==> combinedTrk->setIDTrackElementLink( *elit );
		 if ( msgLvl(MSG::DEBUG) ) msg()  << MSG::DEBUG << " MuonEF  - ID TrackParticle parameters are:  pt " << combinedTrk->getIDTrackParticle()->pt()
				     << " MeV - eta " << combinedTrk->getIDTrackParticle()->eta()
				     << " phi " <<  combinedTrk->getIDTrackParticle()->phi()<< endreq;
		 foundellink=true;
		 break;
	       }
	       
	     }//ID ElementLink loop
	     
	     if(!foundellink) {
	       msg() << MSG::WARNING << "Did not find matching EF ID track - cannot store in EDM" << endreq;
	     }
          }
          tot_mdt = tot_mdt + pSummary->numMdtHits; 
          tot_rpc = tot_rpc + pSummary->numRpcEtaHits + pSummary->numRpcPhiHits; 
          tot_tgc = tot_tgc + pSummary->numTgcEtaHits + pSummary->numTgcPhiHits; 
          tot_csc = tot_csc + pSummary->numCscEtaHits + pSummary->numCscPhiHits; 
          tot_seg = tot_seg + pSummary->numMdtSegs + pSummary->numRpcSegs + pSummary->numTgcSegs
                            + pSummary->numCscSegs + pSummary->numTgcRpcSegs;
	  
	  // xAOD::TrackParticle* etp2 = NULL; //new Rec::TrackParticle(*etp);
	  // removed until we the migration to the new EDM is performed.  ==> eTrkParCont->push_back(etp2);	   
	   //for perform
         if(m_doNTuplePerformance)
         {
	      ++nMuons;
	       // tan(x/2)=sqrt(1+cot**2)-cot
	      double eta=-log(sqrt(1+(eTheta)*(eTheta))-(eTheta)); 
	      StatusCode statusfill = m_pTrigMuGirlNtuple->fillReco(nMuons,ePhi,eTheta,eta,(1/ePt), (int)eCharge, NN[0], NN[1], NN[2], NN[3], NN[4], NN[5], pSummary->lhr, TrackIso, pSummary->innDelta );
	      if (statusfill.isFailure())
	      {
	          msg() << MSG::ERROR << " Cannot fill NTuple Reco " << endreq;
	          return HLT::BAD_JOB_SETUP;
	      }
	    
	 } 

      }
   }

   eMuoninfo->setNMdtHits(tot_mdt); 
   eMuoninfo->setNRpcHits(tot_rpc); 
   eMuoninfo->setNTgcHits(tot_tgc); 
   eMuoninfo->setNCscHits(tot_csc); 
   eMuoninfo->setNSegments(tot_seg); 
  
   status = attachFeature(outputTE, eMuonEFInfoCont,"eMuonEFInfo");
   if(status != HLT::OK) 
   {
       msg() << MSG::WARNING << "Failed to attach TrigMuonEFInfoContainer." << endreq;
       //     return status;
   } 
   else 
   {
       msg() << MSG::DEBUG << "Successfully attached to TEout a TrigMuonEFInfoContainer with size " << eMuonEFInfoCont->size() << endreq;
   }


   //for perform
   if (m_doNTuplePerformance)
   {
       if (msg().level() <= MSG::DEBUG)
           msg() << MSG::DEBUG << " Write Ntuple " << endreq;
       StatusCode statusNT = m_pTrigMuGirlNtuple->writeRecord();
       if (statusNT.isFailure() || m_pTrigMuGirlNtuple == NULL)
       {
	   msg() << MSG::ERROR << "TrigMuGirlFex::execute Cannot write NTuple" << endreq;
	   //delete di_muon;
	   return HLT::BAD_JOB_SETUP;
       }
   }


   //attached ID track container
   //   msg()<<MSG::DEBUG<<"number of eTrkParCont="<<eTrkParCont->size()<<endreq;
   status = attachFeature(outputTE, eTrkParCont, "eGirlID");
   if (status!= HLT::OK) 
     {
       msg() << MSG::WARNING << "Problems with attachFeature TrackParticles  Container"<< endreq;
     }
   else 
     {
       msg()<<MSG::DEBUG<<"attached GirlIDTrk container with size="<<eTrkParCont->size()<<endreq;
     }


   if (m_doParticleCreator && !summaryList.empty()) 
   {
       if (msg().level() <= MSG::DEBUG) 
  	   msg()<< MSG::DEBUG << "Calling m_pParticleCreatorTool->fillContainer" << endreq; 
       if (m_pParticleCreatorTool->fillContainer(&summaryList).isFailure()) 
       {
           for (MuGirlNS::CandidateSummaryList::iterator itSummary = summaryList.begin(); itSummary !=summaryList.end(); itSummary++)
	   {
	       delete *itSummary;
	   }
	   summaryList.clear();
	   for (unsigned i = 0; i < m_caloParticles.size(); i++)
	   {
	       delete m_caloParticles[i];
	   }
	   m_caloParticles.clear();
	   return StatusCode::RECOVERABLE;
       }
    }

   msg() << MSG::DEBUG << "TrigMuGirl::clearing caloParticles" << endreq;

   for (unsigned i = 0; i < m_caloParticles.size(); i++)
   {
      delete m_caloParticles[i];
   }
   m_caloParticles.clear();

   //    msg() << MSG::DEBUG <<"muon_pT="<<muon_pT<< endreq;
   //    msg() << MSG::DEBUG <<"muon_phi="<<muon_phi<< endreq;
   //    msg() << MSG::DEBUG <<"muon_cotTh="<<muon_cotTh<< endreq;

   // delete all m_sSegmentCollection
   //   for (Trk::SegmentCollection::iterator segitr=m_pSegmentCollection->begin();segitr< m_pSegmentCollection->end(); segitr++)
   //     {
   //       delete *segitr;
   //     }

        

   msg() << MSG::DEBUG << "TrigMuGirl::clearing summaries" << endreq;
   for (MuGirlNS::CandidateSummaryList::iterator itSummary = summaryList.begin(); itSummary !=summaryList.end(); itSummary++)
   {
      if ((*itSummary)->startFromMF) delete *itSummary;  
      else 
      {
         if ((*itSummary)->pRefittedTrack != NULL)
            delete ((*itSummary)->pRefittedTrack)->track(); 
         delete (*itSummary)->pRefittedTrack; 
         if ((*itSummary)->pTrkRefitted != NULL)
            delete (*itSummary)->pTrkRefitted;
         delete *itSummary;
      }
   }
 
   msg() << MSG::DEBUG << "TrigMuGirl::clearing segments" << endreq;
   m_pSegmentManager->clear();
   summaryList.clear();
 

   if (msg().level() <= MSG::DEBUG)
        msg() << MSG::DEBUG << "TrigMuGirl::execute done" << endreq;

   if (doTiming()) 
   {
	m_timerExecTotal->stop();
   }

   return HLT::OK;
}


void TrigMuGirl::createCaloParticles()
{
    Amg::Vector3D pt_calo_local(0,0,0);

    // out for timing
    //    if (msg().level() <= MSG::DEBUG)
    //        msg() << MSG::DEBUG << "createCaloParticles: Pre-selection from " << m_inDetTrackParticleLinks->size() << " particles." << endreq;

    for (ElementLinkVector<xAOD::TrackParticleContainer>::const_iterator ipIt = m_inDetTrackParticleLinks.begin();
            ipIt != m_inDetTrackParticleLinks.end();
            ++ipIt)
    {
        if ( !ipIt->isValid() ) {
            ATH_MSG_WARNING("Invalid EF ID TrackParticle ElementLink");
            continue;
        }

        const xAOD::TrackParticle* tp_id = **ipIt;
        const Trk::Perigee& pPerigee = tp_id->perigeeParameters();
/*
        if (pPerigee == NULL)
        {
            if (msg().level() <= MSG::DEBUG)
                msg() << MSG::DEBUG << "Missing perigee" << endreq;
            continue;
        }
*/
        double px = pPerigee.momentum()[Trk::px];
        double py = pPerigee.momentum()[Trk::py];
        double pt = sqrt(px * px + py * py);

        if (pt < m_ptLowerLimit)
        {
	  // out for riming
	  //            if (msg().level() <= MSG::DEBUG)
	  //                msg() << "pT=" << pPerigee->pT() << " is lower than limit = " << m_ptLowerLimit << ". Skipping track." << endreq;
            continue;
        }
/*
        if (tp_id->trackSummary() == NULL)
        {
            if (msg().level() <= MSG::DEBUG)
                msg() << MSG::DEBUG << "Missing trackSummary" << endreq;
            continue;
        }
*/
        uint8_t pixel_hits = 0;
        uint8_t sct_hits = 0;
        tp_id->summaryValue(pixel_hits,xAOD::numberOfPixelHits);
        tp_id->summaryValue(pixel_hits,xAOD::numberOfSCTHits);

        int numhits_id = pixel_hits + sct_hits; 
                         //tp_id->trackSummary()->get(Trk::numberOfPixelHits) + tp_id->trackSummary()->get(Trk::numberOfSCTHits);

        if (numhits_id < 5)
        {
            if (msg().level() <= MSG::DEBUG)
                msg() << MSG::DEBUG << "Cut on #hits=" << numhits_id << endreq;
            continue;
        }

        bool seenByCalo = false;
        if (m_pToCalo)
        {
            double offset = 0.;
            const Trk::TrackParameters* calo_local = 0;
            for (unsigned i = 0; i < m_caloLayers.size(); ++i)
            {
                if ( (calo_local = m_pToCalo->extrapolate(*(tp_id->track()), m_caloLayers[i],offset))!=0 ) {
                    seenByCalo=true;
                    pt_calo_local = calo_local->position();
                }
                //seenByCalo = m_pToCalo->TrackSeenByCalo(tp_id->originalTrack(), m_caloLayers[i], offset, &pt_calo_ctb, &pt_calo_local);
                if (seenByCalo)
                    break;
            }
        }
        else
            seenByCalo = true;
        if (!seenByCalo)
        {
            if (msg().level() <= MSG::DEBUG)
                msg() << MSG::DEBUG << "Not seen by calo" << endreq;
            continue;
        }

	// out for timing
	//        if (msg().level() <= MSG::DEBUG)
	//            msg() << MSG::DEBUG << "This TrackParticle is seen by Calo!" << endreq;

        CaloParticle* pParticle = new CaloParticle(tp_id, &pPerigee, pt_calo_local.eta(), pt_calo_local.phi());
        m_caloParticles.push_back(pParticle);
    }
}

void TrigMuGirl::IsolatedIdTracks(double dR)
{
    for (unsigned int i = 0; i < m_caloParticles.size(); i++)
    {
        CaloParticle* pParticle1 = m_caloParticles[i];
        if (!pParticle1->isolated)
            continue;
        const xAOD::TrackParticle* tp1 = pParticle1->pTrackParticle;
        const Trk::Perigee mp1 = tp1->perigeeParameters();
        double phi1 = mp1.parameters()[Trk::phi];
        double eta1 = asinh(1. / tan(mp1.parameters()[Trk::theta]));
        for (unsigned int j = i + 1; j < m_caloParticles.size(); j++)
        {
            CaloParticle* pParticle2 = m_caloParticles[j];
            if (!pParticle2->isolated)
                continue;
            const xAOD::TrackParticle* tp2 = pParticle2->pTrackParticle;
            const Trk::Perigee mp2 = tp2->perigeeParameters();
            double phi2 = mp2.parameters()[Trk::phi];
            double eta2 = asinh(1. / tan(mp2.parameters()[Trk::theta]));
            double deta = fabs(eta2 - eta1);
            double dphi = fabs(phi2 - phi1);
            if (dphi > M_PI)
                dphi = 2 * M_PI - dphi;
	    float dRmu=sqrt(deta * deta + dphi * dphi);
            if (sqrt(deta * deta + dphi * dphi) < dR)
            {
                pParticle1->isolated = false;
		if (pParticle1->dR > dRmu) pParticle1->dR = dRmu;
                pParticle2->isolated = false;
		if (pParticle2->dR > dRmu) pParticle2->dR = dRmu;
            }
        }
    }
}

void TrigMuGirl::calculateLHR(CaloParticle* pParticle)
{
    const xAOD::TrackParticle* pTrackParticle = pParticle->pTrackParticle;
    const Trk::Perigee pPerigee = pTrackParticle->perigeeParameters();
    double charge_id = pTrackParticle->charge();
    double qOverP_id = pPerigee.parameters()[Trk::qOverP];
    double p_id      = fabs(1.0/qOverP_id);
    double phi_id    = pPerigee.parameters()[Trk::phi];
    double eta_id    = asinh(1./tan(pPerigee.parameters()[Trk::theta]));
    double pt_id     = pPerigee.pT();

    if (msg().level() <= MSG::DEBUG)
        msg() << MSG::DEBUG << "Parameters of isolated tracks seen by calo: " <<
        " pt =  " << pt_id  <<
        " eta = " << eta_id <<
        " phi = " << phi_id <<
        " charge = " << charge_id << endreq;

    double theLHR = m_pMuLHR->getLHR(m_pClusCollection, eta_id, p_id, pParticle->eta, pParticle->phi);
    if (msg().level() <= MSG::DEBUG)
        msg() << MSG::DEBUG << "theLHR = " << theLHR << endreq;

    m_pCandidate->setIsolation(1);
    m_pCandidate->setLHR(theLHR);
}


bool TrigMuGirl::generateMuGirl(MuGirlNS::CandidateSummary* summary)
{
    double eta = 999.;
    //double pt = 0.;
    if (summary->startFromMF)
    {
        if( summary->pMuonFeature != NULL)
        {
            eta = (summary->pMuonFeature)->eta();
         //   pt = (summary->pMuonFeature)->pt();
        }
    }
    else
    {
       Trk::Perigee seed_perigee = summary->pTrackParticle->perigeeParameters();
       eta = asinh(1./tan(seed_perigee.parameters()[Trk::theta]));
       //pt = (summary->pTrackParticle)->measuredPerigee()->pT();
    }

    if (fabs(eta)>2.0 && (summary->numMdtHits <= 2 && summary->numCscSegs == 0 && summary->numTgcEtaHits <= 2 && summary->numTgcPhiHits <= 2 ))
        return false;
    if (fabs(eta)<2.0 && summary->numMdtHits<=2)
        return false;

    if ((summary->innAngle == 999.) && (summary->midAngle == 999.))
        return false;
    double value[8];
    double ANN[2];

    msg()<<MSG::DEBUG<<"generateMuGirl LHR"<<summary->lhr<<"setting it to 0.8"<<endreq;
    summary->lhr = 0.8;

    m_pANNSelectionTool->evaluateANN(summary,value,ANN);
    m_pCandidate->setAnn(ANN[0], ANN[1]);
    summary->nnBarrel = ANN[0];
    summary->nnEndCap = ANN[1];

    msg() << MSG::DEBUG << " ANN [0] "<<value[0]<< " ANN [1] "<<value[1]<< " ANN [2] "<<value[2]<< " ANN [3] "<<value[3]<< " ANN [4] "<<value[4]<< " ANN [5] "<<value[5] << endreq;
    //    msg() << MSG::DEBUG << "arikerez "<<value[0]<< " "<<value[1]<< " "<<value[2]<< " "<<value[3]<< " "<<value[4]<< " "<<value[5] << endreq;
    if (m_doNTuplePerformance)
      {
	NN[0]=value[0];
	NN[1]=value[1];
	NN[2]=value[2];
	NN[3]=value[3];
	NN[4]=value[4];
	NN[5]=value[5];
      }

    /*        
    if (value[0]>0.08 || //0.004 ||    // BARREL GOOD  PT<10GeV
	value[1]>0.11 || //0.004  ||    // BARREL BAD
	value[2]>0.15 || //0.004 ||    // REGION OVERLAP
            value[3]>0.004 ||     // MAGNET OVERLAP
            value[4]>0.004 ||    // ENDCAP
            value[5]>0.004 )      // FORWARD
      {
        msg()<<MSG::DEBUG<<"eTrigEFMuon pass=true"<<endreq;
	summary->PassNNsel = true;
	m_pCandidate->setPassANN(true);
	return true;
      }
      else 
	  {
	  msg()<<MSG::DEBUG<<"eTrigEFMuon pass=false"<<endreq;
	    summary->PassNNsel = false;
	    m_pCandidate->setPassANN(false);
	    return false;
	  }
    
    */
    
    if (value[0]<0.2 && //0.004 ||    // BARREL GOOD  PT<10GeV
        value[1]<0.2 && //0.004  ||    // BARREL BAD
        value[2]<0.2 && //0.004 ||    // REGION OVERLAP
        value[3]<0.2 &&     // MAGNET OVERLAP
        value[4]<0.2 &&    // ENDCAP
        value[5]<0.2 )      // FORWARD
    {
        summary->PassNNsel = false;
	m_pCandidate->setPassANN(false);
	return false;
    }
    else
    {
      summary->PassNNsel = true;
      m_pCandidate->setPassANN(true);
      return true;
    }
    
}

void TrigMuGirl::fillMuonSegmentColl(std::vector<const Muon::MuonSegment*> muonSegmentList)
{
    for (MuGirlNS::MuonSegmentList::const_iterator MuonSegItr=muonSegmentList.begin();  MuonSegItr!=muonSegmentList.end(); MuonSegItr++)
    {
        const Muon::MuonSegment* seg = *MuonSegItr;
        //m_pSegmentCollection->push_back(const_cast <Muon::MuonSegment*>(seg));
	m_pSegmentManager->setInStore(seg, false);
    }
}


Rec::TrackParticle* TrigMuGirl::TEcreateParticle(const Trk::Track* track,const Trk::VxCandidate* vxCandidate,Trk::TrackParticleOrigin prtOrigin)
 {
     if (track == 0) return 0;
     //     MsgStream log(msgSvc(), name());
     const Trk::Perigee* aMeasPer(0);
     
     aMeasPer = dynamic_cast<const Trk::Perigee*>( track->perigeeParameters() );
     aMeasPer = aMeasPer->clone(); 
    if (aMeasPer==0) return 0;        
 
     //Creating TrackParticle with a dummy TrackSummary
     const Trk::TrackSummary* summary=new Trk::TrackSummary;
     //     summary=new Trk::TrackSummary;
                
     // find the first and the last hit in track
     // we do that the same way as in the track slimming tool!
     // that way it is also ok on not slimmed tracks!
     std::vector<const Trk::TrackParameters*> parameters;
     
     //     if (m_keepParameters)
     {
       const DataVector<const Trk::TrackStateOnSurface>* trackStates = track->trackStateOnSurfaces();
       
       const Trk::TrackParameters* first(0) ; 
       
       // search first valid TSOS first
       for ( DataVector<const Trk::TrackStateOnSurface>::const_iterator itTSoS = trackStates->begin(); itTSoS != trackStates->end(); ++itTSoS)
       {
         if ( (*itTSoS)->type(Trk::TrackStateOnSurface::Measurement) && (*itTSoS)->trackParameters()!=0 && (*itTSoS)->measurementOnTrack()!=0 )
         {
             first = (*itTSoS)->trackParameters();
             parameters.push_back((*itTSoS)->trackParameters()->clone());
             break;
         }
       }
         
       // search last valid TSOS first
       for ( DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator rItTSoS = trackStates->rbegin(); rItTSoS != trackStates->rend(); ++rItTSoS)
       {
         if ( (*rItTSoS)->type(Trk::TrackStateOnSurface::Measurement) && (*rItTSoS)->trackParameters()!=0 && (*rItTSoS)->measurementOnTrack()!=0 )
         {
             if (!(first == (*rItTSoS)->trackParameters())) parameters.push_back((*rItTSoS)->trackParameters()->clone());
             break;
         }
       }
       
       // security check:
       if (parameters.size() > 2) msg() << MSG::WARNING << "More than two additional track parameters to be stored in TrackParticle!" << endreq;
     }   
     const Trk::FitQuality* fitQuality = new Trk::FitQuality( (*track->fitQuality()) );
     return new Rec::TrackParticle(track, prtOrigin, vxCandidate, summary, parameters, aMeasPer,  fitQuality);
 }


// for perfo ntuple
HLT::ErrorCode TrigMuGirl::doMuTruthPerformance()
{
    int nTruthMu = 0;
    const McEventCollection* pMcEventCollection = NULL;
    StatusCode status = m_pEventStore->retrieve(pMcEventCollection, "TruthEvent");
    if (status.isFailure() || pMcEventCollection == NULL)
    {
      msg() << MSG::WARNING << " Cannot retrieve McEventCollection " << endreq;
      return HLT::SG_ERROR;
    }

    McEventCollection::const_iterator itMc = pMcEventCollection->begin(), itMc_e = pMcEventCollection->end();
    for (; itMc != itMc_e; ++itMc)
      {
        const HepMC::GenEvent* pGenEvent = *itMc;

	HepMC::GenEvent::particle_const_iterator itPart = pGenEvent->particles_begin(), itPart_e = pGenEvent->particles_end();
        for (; itPart != itPart_e; ++itPart)
	  {
            const HepMC::GenParticle* pPart = *itPart;

            if (pPart->pdg_id() == -13 || pPart->pdg_id() == 13)
	      {
                int parent_pdg = 0;
                if (pPart->production_vertex() != NULL)
		  {
		    HepMC::GenVertex::particle_iterator itVxPart = pPart->production_vertex()->particles_begin(HepMC::parents), itVxPart_e = pPart->production_vertex()->particles_end(HepMC::parents);
                    for (; itVxPart != itVxPart_e; ++itVxPart)
		      {
                        parent_pdg = (*itVxPart)->pdg_id();
		      }
		  }

                ++nTruthMu;
                if (m_doNTuplePerformance)
		  {
		    double cot=1/tan(2*atan(exp(-pPart->momentum().eta())));

                    status = m_pTrigMuGirlNtuple->fillTruth(nTruthMu,
                                                        pPart->momentum().phi(),
                                                        cot,
                                                        pPart->momentum().eta(),
                                                        pPart->momentum().perp(),
                                                        pPart->pdg_id(),
							    parent_pdg);
                    if (status.isFailure())
                    {
		      msg() << MSG::ERROR << " Cannot fill NTuple TRUTH " << endreq;
		      //delete di_muon; 
		      return HLT::BAD_JOB_SETUP;
                    }
		  }
	      }
	  }
      }
    return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//
