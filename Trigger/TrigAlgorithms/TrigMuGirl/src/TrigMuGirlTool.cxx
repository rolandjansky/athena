/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
    Authors: E. Reinherz-Aronis, A. Kreisel 
             based on MuGirl by Shlomit Tarem
***************************************************************************/
#include "TrigMuGirl/TrigMuGirlTool.h"

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

#include "ICaloTrkMuIdTools/ICaloMuonLikelihoodTool.h"
#include "CaloEvent/CaloClusterContainer.h"

#include "Particle/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"

#include "MuGirlInterfaces/MdtSegmentMakerInfo.h"

#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"

#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "TrkCaloExtension/CaloExtension.h"

// for performance 
#include "TrigMuGirl/TrigMuGirlNtuple.h"
#include "GeneratorObjects/McEventCollection.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"

using HepGeom::Point3D;
using CLHEP::Hep3Vector;
using CLHEP::GeV;

//________________________________________________________________________
TrigMuGirlTool::TrigMuGirlTool(const std::string& type,
			       const std::string& name,
			       const IInterface* parent)
  : AthAlgTool(type,name,parent),
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
    m_pEventInfo(NULL),
    m_pEventStore("StoreGateSvc", name),
    m_pCandidate("MuGirlNS::CandidateTool"),
    m_pMuLHR("CaloMuonLikelihoodTool"),
    m_pTruthTool("MuGirlNS::PerformanceTruthTool",0),                 // make this a public tool
    m_pGlobalFitTool("MuGirlNS::GlobalFitTool",0),                    // make this a public tool
    m_pMuonFeatureGlobalFitTool("MuGirlNS::GlobalFitTool",0),         // make this a public tool
    m_pANNSelectionTool("MuGirlNS::ANNSelectionTool",0),              // make this a public tool
    m_pParticleCreatorTool("MuGirlNS::MuGirlParticleCreatorTool",0),  // make this a public tool
    m_pStauTool("MuGirlNS::StauTool",0),                              // make this a public tool
    m_magFieldSvc("MagField::AtlasFieldSvc/AtlasFieldSvc",name),
    m_pSegmentManager(NULL),
    // for perform
    m_pTrigMuGirlNtuple(NULL),
    m_ntupleNamePerformance("/NTUPLES/TRIGMUGIRL/TRIGMUGIRL"),
    m_ntupleTitlePerformance("TrigMuGirlPer"),
    m_doNTuplePerformance(false),
    m_caloExtensionTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool"),
    m_roi(0) 

{

  declareInterface<ITrigMuGirlTool>(this);

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
  declareProperty("doANNSelection",               m_doANNSelection);
  declareProperty("doParticleCreator",            m_doParticleCreator);
  // Tools
  // in the old ver there was no
  declareProperty("StoreGateSvc",                 m_pEventStore);
  declareProperty("MuGirlCandidate",              m_pCandidate);
  declareProperty("MuGirlGlobalFIT",              m_pGlobalFitTool);
  declareProperty("MuGirlGlobalStauFIT",          m_pStauGlobalFitTool);
  declareProperty("MuGirlGlobalMuonFeatureFIT",   m_pMuonFeatureGlobalFitTool);
  declareProperty("CaloMuonLikelihoodTool",       m_pMuLHR);
  //    declareProperty("TrackParticleCreatorTool", m_particleCreatorTool);
  declareProperty("ParticleCaloExtensionTool",m_caloExtensionTool );    
  //<S>
  declareProperty("MuGirlStauTool",               m_pStauTool);
  declareProperty("PerformanceTruthTool",         m_pTruthTool);
  declareProperty("ANNSelectionTool",             m_pANNSelectionTool);
  declareProperty("MuGirlParticleCreatorTool",             m_pParticleCreatorTool);
  // varibles for monitor
  //    declareMonitoredStdContainer ("MuonsPt", pt);
  //    declareMonitoredStdContainer ("MuonsCosTh", cotTh);
  //    declareMonitoredStdContainer ("MuonsPhi", phi);


  // for perform
  declareProperty("doNTuplePerformance",  m_doNTuplePerformance  = false);
  declareProperty("doTruthPerformance",   m_doTruthPerformance   = false);

  declareProperty("doMuonFeature",                m_doMuonFeature);

} 

//________________________________________________________________________
TrigMuGirlTool::~TrigMuGirlTool()
{

}

//________________________________________________________________________
StatusCode TrigMuGirlTool::initialize()
{
  
  if (msgLvl(MSG::DEBUG) )
    msg() << MSG::DEBUG << "Initializing TrigMuGirlTool " << endreq;    
  
  
  m_pSegmentManager = new MuGirlNS::SegmentManager;
  if (m_magFieldSvc.retrieve().isFailure())
    {
        msg(MSG::ERROR) <<"Could not retrieve magFieldSvc"<<endreq;
        return StatusCode::RECOVERABLE;
    }

  
  if (retrieve(m_pEventStore).isFailure())
    msg() << MSG::DEBUG << "Failed to get the StoreGate" << endreq;    
  
  // Initialize tools

  if (retrieve(m_caloExtensionTool).isFailure())
    msg() << MSG::DEBUG << "Failed to get the CaloExtensionTool" << endreq;    


  
  if (m_doTruth)
    {
      if (retrieve(m_pTruthTool).isFailure())
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

 
  return StatusCode::SUCCESS;
}

//________________________________________________________________________
HLT::ErrorCode TrigMuGirlTool::initNtuple(HLT::FexAlgo* fexAlgo)
{
  if (m_doNTuple) 
    {
      // Initialize NTuple
      msg()<<MSG::DEBUG<<"in doNTuple"<<endreq;
      
      m_pCandNTuple = fexAlgo->ntupleSvc()->book(m_ntupleName, CLID_ColumnWiseTuple, m_ntupleTitle);
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
  
  if (m_doNTuplePerformance)
    {
      if (!m_pTrigMuGirlNtuple)
	{
	  msg()<< MSG::ERROR << "Could not book NTuple " << m_ntupleName << endreq;
	  return HLT::BAD_JOB_SETUP;
	}

      INTupleSvc* ntupleSvc=fexAlgo->ntupleSvc();
      m_pTrigMuGirlNtuple = new TrigMuGirlNtuple(fexAlgo, ntupleSvc);
      StatusCode SCstatus = m_pTrigMuGirlNtuple->book(m_ntupleNamePerformance, m_ntupleTitlePerformance);
      if (SCstatus.isFailure())
	{
	  msg()<< MSG::ERROR << "Could not book NTuple " << m_ntupleName << endreq;
	  return HLT::BAD_JOB_SETUP;
	}
    }
  
  return HLT::OK;
}

//________________________________________________________________________
StatusCode TrigMuGirlTool::finalize()
{
  return StatusCode::SUCCESS;
}

//________________________________________________________________________
HLT::ErrorCode TrigMuGirlTool::initRoiProcessing(std::vector<TrigTimer*>& timers, 
						 TrigMuonEFInfoContainer* eMuonEFInfoCont)
{
  TrigTimer* timerExecTotal = 0;
  if (timers.size() >= 1) timerExecTotal=timers[0];

  if (timerExecTotal) timerExecTotal->start();

  ++m_roi;
  
  if (eMuonEFInfoCont->size() == 0) {
    TrigMuonEFInfo* eMuoninfo = new TrigMuonEFInfo(m_roi);
    eMuonEFInfoCont->push_back( eMuoninfo );
  }

  if (m_doNTuplePerformance) {    
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
  
  m_pEventInfo = NULL;
  sc = m_pEventStore->retrieve(m_pEventInfo);
  if (sc.isFailure() || m_pEventInfo == NULL)
    msg() << MSG::WARNING << "Cannot retrieve EventInfo" << endreq;
  else if (m_doNTuplePerformance) {
    eRunNumber   = m_pEventInfo->event_ID()->run_number();
    eEventNumber = m_pEventInfo->event_ID()->event_number();
  }

  m_summaryList.clear();

  return HLT::OK;
}

//________________________________________________________________________
HLT::ErrorCode TrigMuGirlTool::processMuonFeatures(const std::vector<const MuonFeature*>& vectorOfMuonFeature,
						   const ElementLink<CombinedMuonFeatureContainer>& CombinedMuonFeatureEL)
{

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
	  
          //// please fix me	  
          // if (m_pCandidate->fillMF(iMuF, pMuonFeature, true,has_combined).isFailure()) 
	  //   {
	  //     msg()<< MSG::DEBUG << "MuonFeature Cannot fill CANDIDATE" << endreq;
	  //     continue;
	  //   }
          // fillMuonSegmentColl(m_pCandidate->vectorMuonSegments());

          MuGirlNS::CandidateSummary* summary_mf = new MuGirlNS::CandidateSummary();
          if ( msgLvl(MSG::DEBUG) ) 
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
	      if (m_pEventInfo != NULL)
		{
		  if ((m_pCandidate->fillNTupleEventInfo(m_pEventInfo->event_ID()->run_number(),
							 m_pEventInfo->event_ID()->event_number())).isFailure())
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
		    msg() << MSG::WARNING << "TrigMuGirlTool::execute Cannot fill Truth in NTuple for Muonfeature processing" << endreq;
		}
	    }
          
          if ( msgLvl(MSG::DEBUG) ) 
	    msg()<< MSG::DEBUG << "MuonFeature do Stau hits" << summary_mf->numMdtHits
		 <<" "<<summary_mf->numTgcEtaHits<<" "<<summary_mf->numTgcPhiHits
		 <<" "<<summary_mf->numRpcEtaHits<<" "<< summary_mf->numRpcPhiHits
		 << endreq;
          if (summary_mf->numMdtHits >4 && 
	      (summary_mf->numTgcEtaHits>2 || summary_mf->numTgcPhiHits>2 || summary_mf->numRpcEtaHits>2 || summary_mf->numRpcPhiHits>2))
	    {
	      if ( msgLvl(MSG::DEBUG) ) 
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

              //// please fix me
	      // if (m_pStauTool->fillStauSummary(summary_mf,stauSummary).isFailure())
	      //   msg() << MSG::DEBUG << "MuonFeature::execute Cannot fill stausummary" << endreq;
	      stauSummary->qOverP = summary_mf->qOverP;
	      stauSummary->startFromMF = 1;
	      bool ANNstau = generateMuGirl(stauSummary);

	      msg() << MSG::DEBUG << "MuonFeature::execute ANNstau"<< ANNstau << endreq;
	      //if (ANNstau)
	      // {
	      stauSummary->saveStau = 1;
	      m_summaryList.push_back(stauSummary);
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
                      xAOD::TrackParticle* tp = NULL;
                      ATH_MSG_DEBUG("found SA track");
                      /** add the refitted track to the refitted track container and create the element link */
		      m_pCandidate-> fillRefittedTrack(tp,RefittedTrack,stauSummary);
                    }
                    else
                    {	
                      delete RefittedTrack;
                      m_pCandidate-> fillRefittedTrack(NULL,NULL,stauSummary);
                    }
		      //    m_pRefittedTrkCollection->push_back(const_cast<Trk::Track*> (RefittedTrack));
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
  
  return HLT::OK;
}

//________________________________________________________________________
HLT::ErrorCode TrigMuGirlTool::processTrackCollections(	const ElementLinkVector<xAOD::TrackParticleContainer>& idTrackParticleLinks,
                                                        const std::vector<const xAOD::CaloClusterContainer*>& vectorOfClusterContainers)
{
  
  m_pStauTool->initTool(m_doGlobalFit);
  
  const VxContainer* vxCandidatesContainer = 0;
  int iTrack = 0;
  // for performance ntuple
  m_TrackIso=-1;

  if(msgLvl(MSG::DEBUG))
    msg() << MSG::DEBUG << "Got vector with " << vectorOfClusterContainers.size()  << " ClusterContainers" << endreq;
  
  // if no containers were found, just leave the vector empty and leave
  if ( vectorOfClusterContainers.size() < 1) {
    if(msgLvl(MSG::DEBUG)) 
      msg() << MSG::DEBUG << "No Cluster container available !" << endreq;
  }
  else
    {
      // get last container to be put in vector (should also be the only one)
      m_pClusCollection = vectorOfClusterContainers.back();
      if (!m_pClusCollection)
	{
	  if(msgLvl(MSG::WARNING)) 
	    msg() << MSG::WARNING << "No Clusters found in the RoI" << endreq;
	}
    }
  
  if (!m_magFieldSvc->solenoidOn()) 
    { 
      if (msg().level()<=MSG::DEBUG) msg()<<MSG::DEBUG << "MuGirl::Solenoid is off. Execute done" << endreq; 
      return HLT::OK;
    } 
  else
    if (msg().level()<=MSG::DEBUG) msg()<<MSG::DEBUG << "MuGirl::Solenoid is ON" << endreq; 
  
  m_inDetTrackParticleLinks = idTrackParticleLinks;
  
  // out for timing
  //    if (msg().level() <= MSG::DEBUG)
  //        msg() << MSG::DEBUG << "Size of TrackParticleCandidate = " << m_inDetTrackParticleLinks.size() << endreq;
  if (m_pEventStore->contains<VxContainer>("VxPrimaryCandidate"))
    {
      if (m_pEventStore->retrieve(vxCandidatesContainer, "VxPrimaryCandidate").isFailure())
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
      eRunNumber   = m_pEventInfo->event_ID()->run_number();
      eEventNumber = m_pEventInfo->event_ID()->event_number();
    }

  createCaloParticles();
   
  IsolatedIdTracks(m_idR);


  for (unsigned int i = 0; i < m_caloParticles.size(); i++)
    {
      CaloParticle* pParticle = m_caloParticles[i];
      const xAOD::TrackParticle* pTrackParticle = pParticle->pTrackParticle;
      const Trk::Perigee perigee = pTrackParticle->perigeeParameters();

      bool doStau = m_doStau && perigee.pT() >= m_stauPtCut;
      msg() << MSG::DEBUG << "TrigMuGirlTool::execute doStau "<<doStau << endreq;
      if (!m_pCandidate->fill(iTrack, pTrackParticle, &perigee, doStau).isSuccess())
        {
	  msg() << MSG::DEBUG << "TrigMuGirlTool::execute failed candidate->fill " << endreq;
   
	  fillMuonSegmentColl(m_pCandidate->vectorMuonSegments());
	  continue;
	}

      //for performance ntuple
      if (pParticle->isolated)
	{
	  m_pCandidate->setIsolation(1);
	  m_pCandidate->setLHR(-2);
	}
      m_TrackIso=pParticle->isolated;

      MuGirlNS::CandidateSummary* summary = new MuGirlNS::CandidateSummary();

      bool ANNaccept = false;
      if (m_pCandidate->fillSummary(summary).isFailure())
	{
	  msg() << MSG::DEBUG << "TrigMuGirlTool::execute Cannot fill summary" << endreq;
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

	  if (m_doGlobalFit&&( pTrackParticle->track()!=NULL )&&
	      (m_pCandidate->vectorMuonSegments()).size()>0)
            {
	      const Trk::Track* RefittedTrack = 0;
	      RefittedTrack = m_pGlobalFitTool->globalFit( pTrackParticle->track(),m_pCandidate->vectorMuonSegments(),m_doNTuple);
	      if (RefittedTrack != NULL)
		{
		  //m_pRefittedTrkCollection->push_back(const_cast<Trk::Track*> (RefittedTrack));
		  //const Trk::VxCandidate* pVxCandidate = 0 ;
		  //pVxCandidate=pTrackParticle->reconstructedVertex();
		  xAOD::TrackParticle* tp = 0;
/*
		  if (pVxCandidate != NULL && vxCandidatesContainer != 0)
		    tp =TEcreateParticle(RefittedTrack, pVxCandidate, pVxCandidate->vertexType());
		  else
		    // this didn't worked so we rewrite this routine for TE
		    //                   tp =m_particleCreatorTool->createParticle(RefittedTrack, pVxCandidate, Trk::NoVtx);
		    tp =TEcreateParticle(RefittedTrack, pVxCandidate, Trk::NoVtx);
*/
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
          if (m_pEventInfo != NULL)
	    {
	      if (m_pCandidate->fillNTupleEventInfo(m_pEventInfo->event_ID()->run_number(),
						    m_pEventInfo->event_ID()->event_number()).isFailure())
		msg() << MSG::WARNING << "TrigMuGirlTool::execute Cannot fill NTuple" << endreq;
	    }
	  if (msg().level() <= MSG::DEBUG)
	    msg() << MSG::DEBUG << "Calling m_pCandidate->fillNTuple" << endreq;
	    
	  if (m_pCandidate->fillNTuple().isFailure())
	    msg() << MSG::WARNING << "TrigMuGirlTool::execute Cannot fill NTuple" << endreq;
	  if (m_doNTuple&&m_doTruth&&pTrackParticle!=NULL)
	    {
	      if (msg().level() <= MSG::DEBUG)
		msg() << MSG::DEBUG << "Calling m_pTruthTool->fillNTuple" << endreq;
	      if (m_pTruthTool->fillNTuple(summary).isFailure())
		msg() << MSG::WARNING << "TrigMuGirlTool::execute Cannot fill Truth in NTuple" << endreq;
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
          StatusCode sc = m_pStauTool->processStau(pTrackParticle, m_pCandidate->vectorMuonSegments(), mdtSegmentMakerInfoList,rpcHitsInSegments,tgcHitsInSegments,pMuonRefittedTrack);
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
                  //// please fix me
	          // if (m_pStauTool->fillStauSummary(summary,stauSummary).isFailure())
		  //   msg() << MSG::DEBUG << "TrigMuGirlTool::execute Cannot fill stausummary" << endreq;

                  bool ANNstau = generateMuGirl(stauSummary);
                  if (stauSummary->nnBarrel >0.2 || stauSummary->nnEndCap >0.2) ANNstau = true;
                  msg()<<MSG::DEBUG << "MuGirl::execute: stau passANN "<<ANNstau << endreq;

                  const Trk::Track* stauRefittedTrack = m_pStauTool->refittedTrack();
                  if (stauRefittedTrack != NULL)
		    {
                      //const Trk::VxCandidate* pVxCandidate = 0 ;
                      //pVxCandidate=pTrackParticle->reconstructedVertex();
                      xAOD::TrackParticle* stauTp = 0;
                      //if (pVxCandidate != NULL && vxCandidatesContainer != 0)
			//stauTp =TEcreateParticle(stauRefittedTrack, pVxCandidate, pVxCandidate->vertexType());
                      //else
			//stauTp =TEcreateParticle(stauRefittedTrack, pVxCandidate, Trk::NoVtx);
                      //if (stauTp != NULL)
			//{
                          //if (msg().level() <= MSG::DEBUG)
			    //msg() << MSG::DEBUG << "Trk::Track converted to Rec::TrackParticle p() "<<stauTp->p() << endreq;
                      stauSummary->pRefittedTrack = stauTp;  
                      stauSummary->pTrkRefitted   = stauRefittedTrack;
			//}
                      //else
			//delete stauTp;
		    }
                  else
                    {
		      delete stauRefittedTrack;
                      stauSummary->pRefittedTrack = NULL;    
                      stauSummary->pTrkRefitted   = NULL;
                    }
	          if (m_doNTuple)
		    {
	              sc = m_pStauTool->fillNTuple();
	              if(sc.isFailure())
			msg() << MSG::DEBUG << "TrigMuGirlTool::execute stauTool->fillNtuple failed " << endreq; 
		    }

                  if (ANNstau)
		    {
	              stauSummary->saveStau = 1;
              
	              passedStau =true;
	              m_summaryList.push_back(stauSummary);
		    }     
		}
	    }
	} //if doStau

      msg() << MSG::DEBUG << "TrigMuGirlTool::execute passedStau "<<passedStau << endreq;
    
      if (summary->saveAnn == 1)
	{
	  summary->saveStau = 0;
	  m_summaryList.push_back(summary);
	}
      else
	{

	  delete summary; 
        }

      if (m_doNTuple)
	{
	  if (m_pCandNTuple->writeRecord().isFailure())
	    msg() << MSG::WARNING << "TrigMuGirlTool::execute Cannot write NTuple" << endreq;
	}
      // out for timing
      //	if (msg().level() <= MSG::DEBUG)
      //	  msg() << MSG::DEBUG << "Done processing track " << iTrack << endreq;
      iTrack++;

    }  //loop on CaloParticle


  // for perform
  if (m_doNTuplePerformance && m_pTrigMuGirlNtuple)
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

  return HLT::OK;
}

//________________________________________________________________________
HLT::ErrorCode TrigMuGirlTool::finalizeRoiProcessing(std::vector<TrigTimer*>& timers,
						     TrigMuonEFInfoContainer* eMuonEFInfoCont,
						     Rec::TrackParticleContainer*& eTrkParCont,
						     TrigMuGirlMonVars& monVars)
{
  TrigTimer* timerExecTotal = 0;
  if (timers.size() >= 1) timerExecTotal=timers[0];

  //added a container of ID tracks for future use
  eTrkParCont = new Rec::TrackParticleContainer;

  /*
  TrigMuonEFInfo* eMuoninfo = 0;
  if ( eMuonEFInfoCont->size() == 0 ) {
  eMuoninfo = new TrigMuonEFInfo(m_roi);
  eMuonEFInfoCont->push_back( eMuoninfo );
  ATH_MSG_DEBUG("creating new TrigMuonEFInfo with m_roi_num=" << eMuoninfo->RoINum());
  } else {
  eMuoninfo = eMuonEFInfoCont->front();
  ATH_MSG_DEBUG("adding to existing TrigMuonEFInfo with m_roi_num=" << eMuoninfo->RoINum());    
  }
  */

  TrigMuonEFInfoContainer::const_iterator MuonItr = eMuonEFInfoCont->begin();
  TrigMuonEFInfo* eMuoninfo = (*MuonItr);

  int tot_seg = 0; 
  int tot_mdt = 0; 
  int tot_csc = 0; 
  int tot_rpc = 0; 
  int tot_tgc = 0; 

  std::vector<bool> IncludeList;
  MuGirlNS::CandidateSummaryList* SummaryList=&m_summaryList;

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
          monVars.nMdt.push_back(pSummary->numMdtHits);
          monVars.nRpcPhi.push_back(pSummary->numRpcPhiHits);
          monVars.nRpcEta.push_back(pSummary->numRpcEtaHits);
          monVars.nCscPhi.push_back(pSummary->numCscPhiHits);
          monVars.nCscEta.push_back(pSummary->numCscEtaHits);
          monVars.nTgcPhi.push_back(pSummary->numTgcPhiHits);
          monVars.nTgcEta.push_back(pSummary->numTgcEtaHits);
          monVars.nTgcRpcEta.push_back(pSummary->numTgcRpcEtaHits);
          monVars.nTgcRpcPhi.push_back(pSummary->numTgcRpcPhiHits);
 
          monVars.nMdtInseg.push_back(pSummary->numMdtHitsInSeg);
          monVars.nRpcInseg.push_back(pSummary->numRpcHitsInSeg);
          monVars.nTgcInseg.push_back(pSummary->numTgcHitsInSeg);
          monVars.nTgcRpcInseg.push_back(pSummary->numTgcRpcHitsInSeg);

          monVars.nMdtSegs.push_back(pSummary->numMdtSegs);
          monVars.nRpcSegs.push_back(pSummary->numRpcSegs);
          monVars.nCscSegs.push_back(pSummary->numCscSegs);
          monVars.nTgcSegs.push_back(pSummary->numTgcSegs);
          monVars.nTgcRpcSegs.push_back(pSummary->numTgcRpcSegs);

          monVars.beta.push_back(pSummary->beta);
          monVars.nnBarrel.push_back(pSummary->nnBarrel);
          monVars.nnEndcap.push_back(pSummary->nnEndCap);
          MuGirlNS::StauRpcHitsLists* rpcHitsLists = pSummary->rpcHitsLists;
          if(NULL!=rpcHitsLists)
	    {
	      for(MuGirlNS::StauRpcHitsLists::iterator it = rpcHitsLists->begin(); it != rpcHitsLists->end(); it++)
		{
		  MuGirlNS::StauRpcHitsList* pRpcHits = *it;
		  for(MuGirlNS::StauRpcHitsList::iterator it1 = pRpcHits->begin(); it1 != pRpcHits->end(); it1++)
		    {
		      MuGirlNS::StauRpcHit* hit = (*it1);
		      monVars.rpcHitsMeasTime.push_back( hit->measuredTime);
		      monVars.rpcHitsPropTime.push_back( hit->propagationTime);
		      monVars.rpcHitsTof.push_back( hit->muonToF);
		      monVars.rpcHitsDis.push_back( hit->distance);
		    }
	
		}
	
	    }
	  //const xAOD::TrackParticle* etp;
          const Trk::Track* etp = NULL;

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
	  double eCharge = emp->charge();
	  double eMass = 105.7;
	  if (pSummary->beta >0.0 && pSummary->beta < 1.0)
	    {
              double BetaCand = pSummary->beta;
              double mom = (emp->pT())/sin(theta);
              eMass = mom * sqrt(1.-BetaCand*BetaCand)/BetaCand; 
	    }

	  monVars.pt.push_back(1/ePt);
	  monVars.cotTh.push_back(eTheta);
	  monVars.phi.push_back(ePhi);
	    
	  //      msg() << MSG::DEBUG <<"ePt="<<ePt<< endreq;
	  //      msg() << MSG::DEBUG <<"eTheta="<<eTheta<< endreq;
	  //      msg() << MSG::DEBUG <<"ePhi="<<ePhi<< endreq;
	  //      msg() << MSG::DEBUG <<"eCharge="<<eCharge<< endreq;
	  //	   msg() << MSG::DEBUG << " testiso pSummary->lhr = " <<pSummary->lhr<< endreq;
	  //	   msg() << MSG::DEBUG << " testiso TrackIso = " <<m_TrackIso<< endreq;

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
	      for(ElementLinkVector<xAOD::TrackParticleContainer>::const_iterator elit= m_inDetTrackParticleLinks.begin(); elit!=m_inDetTrackParticleLinks.end(); ++elit) {
		if ( !elit->isValid() ) {
		  ATH_MSG_WARNING("Invalid EF ID TrackParticle ElementLink");
		  continue;
		}
		if(pSummary->pTrackParticle==(**elit)) {
		  if (msgLvl(MSG::DEBUG)) msg() << MSG::DEBUG << "Assigning TrackParticle" << endreq;
		  // removed until we get new EDM that supports xAOD  ==>  combinedTrk->setIDTrackElementLink( *elit );
		  if (msgLvl(MSG::DEBUG)) msg()  << MSG::DEBUG << " MuonEF  - ID TrackParticle parameters are:  pt " << combinedTrk->getIDTrackParticle()->pt()
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
	  if(m_doNTuplePerformance && m_pTrigMuGirlNtuple)
	    {
	      ++nMuons;
	      // tan(x/2)=sqrt(1+cot**2)-cot
	      double eta=-log(sqrt(1+(eTheta)*(eTheta))-(eTheta)); 
	      StatusCode statusfill = m_pTrigMuGirlNtuple->fillReco(nMuons,ePhi,eTheta,eta,(1/ePt), (int)eCharge, NN[0], NN[1], NN[2], NN[3], NN[4], NN[5], pSummary->lhr, m_TrackIso, pSummary->innDelta );
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
  
  //for perform
  if (m_doNTuplePerformance)
    {
      if (msg().level() <= MSG::DEBUG)
	msg() << MSG::DEBUG << " Write Ntuple " << endreq;

      if (!m_pTrigMuGirlNtuple)
	{
	  msg() << MSG::ERROR << "TrigMuGirlFex::execute Cannot write NTuple" << endreq;
	  return HLT::BAD_JOB_SETUP;
	}

      StatusCode statusNT = m_pTrigMuGirlNtuple->writeRecord();
      if (statusNT.isFailure())
	{
	  msg() << MSG::ERROR << "TrigMuGirlFex::execute Cannot write NTuple" << endreq;
	  //delete di_muon;
	  return HLT::BAD_JOB_SETUP;
	}
    }
  
  if (m_doParticleCreator && !m_summaryList.empty()) 
    {
      if (msg().level() <= MSG::DEBUG) 
	msg()<< MSG::DEBUG << "Calling m_pParticleCreatorTool->fillContainer" << endreq; 
      if (m_pParticleCreatorTool->fillContainer(&m_summaryList).isFailure()) 
	{
	  for (MuGirlNS::CandidateSummaryList::iterator itSummary = m_summaryList.begin(); itSummary !=m_summaryList.end(); itSummary++)
	    {
	      delete *itSummary;
	    }
	  m_summaryList.clear();
	  for (unsigned i = 0; i < m_caloParticles.size(); i++)
	    {
	      delete m_caloParticles[i];
	    }
	  m_caloParticles.clear();
	  return StatusCode::RECOVERABLE;
	}
    }
  
  msg() << MSG::DEBUG << "TrigMuGirlTool::clearing caloParticles" << endreq;
  
  for (unsigned i = 0; i < m_caloParticles.size(); i++)
    {
      delete m_caloParticles[i];
    }
  m_caloParticles.clear();

  msg() << MSG::DEBUG << "TrigMuGirlTool::clearing summaries" << endreq;
  for (MuGirlNS::CandidateSummaryList::iterator itSummary = m_summaryList.begin(); itSummary !=m_summaryList.end(); itSummary++)
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
  
  msg() << MSG::DEBUG << "TrigMuGirlTool::clearing segments" << endreq;
  m_pSegmentManager->clear();
  m_summaryList.clear();
  
  
  if (msg().level() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "TrigMuGirlTool::finalizeRoiProcessing done" << endreq;
  
  if (timerExecTotal) timerExecTotal->stop();    
  
  return HLT::OK;
}

//________________________________________________________________________
void TrigMuGirlTool::addTimers(HLT::FexAlgo* fexAlgo, std::vector<TrigTimer*>& timers)
{
  timers.push_back( fexAlgo->addTimer("eExecTotal") );
  
}


//________________________________________________________________________
void TrigMuGirlTool::declareMonitoringVariables(HLT::FexAlgo* fexAlgo, TrigMuGirlMonVars& monVars)
{
  ATH_MSG_DEBUG("declaring monitoring variables");

  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_pt",            monVars.pt, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_cotTh",         monVars.cotTh, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_phi",           monVars.phi, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_mdtHits",       monVars.nMdt, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_rpcPhiHits",    monVars.nRpcPhi, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_rpcEtaHits",    monVars.nRpcEta, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_cscPhiHits",    monVars.nCscPhi, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_cscEtaHits",    monVars.nCscEta, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_tgcPhiHits",    monVars.nTgcPhi, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_tgcEtaHits",    monVars.nTgcEta, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_tgcRpcPhiHits", monVars.nTgcRpcPhi, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_tgcRpcEtaHits", monVars.nTgcRpcEta, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_mdtHitsInSeg",  monVars.nMdtInseg, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_rpcHitsInSeg",  monVars.nRpcInseg, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_tgcHitsInSeg",  monVars.nTgcInseg, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_tgcRpcHitsInSeg", monVars.nTgcRpcInseg, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_mdtSegs",       monVars.nMdtSegs, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_rpcSegs",       monVars.nRpcSegs, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_cscSegs",       monVars.nCscSegs, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_tgcSegs",       monVars.nTgcSegs, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_tgcRpcSegs",    monVars.nTgcRpcSegs, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_beta",          monVars.beta, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_nnBarrel",      monVars.nnBarrel, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_nnEndcap",      monVars.nnEndcap, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_rpcHitsMeasTime", monVars.rpcHitsMeasTime, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_rpcHitsPropTime", monVars.rpcHitsPropTime, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_rpcHitsTof",  monVars.rpcHitsTof, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mgtrks_rpcHitsDist", monVars.rpcHitsDis, IMonitoredAlgo::AutoClear);


  return;
}
void TrigMuGirlTool::createCaloParticles()
{
  Amg::Vector3D pt_calo_local(0,0,0);

  // out for timing
  //    if (msg().level() <= MSG::DEBUG)
  //        msg() << MSG::DEBUG << "createCaloParticles: Pre-selection from " << m_inDetTrackParticleLinks.size() << " particles." << endreq;

  
  for (ElementLinkVector<xAOD::TrackParticleContainer>::const_iterator ipIt = m_inDetTrackParticleLinks.begin();
       ipIt != m_inDetTrackParticleLinks.end();
       ++ipIt)
    {
      if ( !ipIt->isValid() )
	{
	  ATH_MSG_WARNING("Invalid EF ID TrackParticle ElementLink");
	  continue;
	}

      const xAOD::TrackParticle* tp_id = **ipIt;
      const Trk::Perigee pPerigee = tp_id->perigeeParameters();
      const xAOD::TrackParticle& tp = ***ipIt;

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

      msg() << MSG::DEBUG << "Perigee "<<px<<endreq;


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


      if (numhits_id < 5)
        {
	  if (msg().level() <= MSG::DEBUG)
	    msg() << MSG::DEBUG << "Cut on #hits=" << numhits_id << endreq;
	  continue;
        }

        const Trk::TrackParameters* extrParameters = NULL;

        /**
           get the extrapolation to the MS entry
          */
        const Trk::CaloExtension* extension = 0;

	if( !m_caloExtensionTool.empty() && 
	    (!m_caloExtensionTool->caloExtension( tp,extension ) ||
	    extension->caloLayerIntersections().empty() ) ) {
          ATH_MSG_DEBUG("Not seen by calo");
          continue;
        }
        
        // if available use muon entry intersection
        if( extension->muonEntryLayerIntersection() ){
          extrParameters = extension->muonEntryLayerIntersection();
          pt_calo_local = extrParameters->position();
        }else{
          // use last calo layer
          pt_calo_local = extension->caloLayerIntersections().back()->position();
        }

      // out for timing
      //        if (msg().level() <= MSG::DEBUG)
      //            msg() << MSG::DEBUG << "This TrackParticle is seen by Calo!" << endreq;

      CaloParticle* pParticle = new CaloParticle(tp_id, pt_calo_local.eta(), pt_calo_local.phi());
      m_caloParticles.push_back(pParticle);
    }
}

void TrigMuGirlTool::IsolatedIdTracks(double dR)
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

void TrigMuGirlTool::calculateLHR(CaloParticle* pParticle)
{
  const xAOD::TrackParticle* pTrackParticle = pParticle->pTrackParticle;
  const Trk::Perigee pPerigee = pTrackParticle->perigeeParameters();
  double charge_id = pPerigee.charge();
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


bool TrigMuGirlTool::generateMuGirl(MuGirlNS::CandidateSummary* summary)
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
      //eta = (summary->pTrackParticle)->measuredPerigee()->eta();
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

void TrigMuGirlTool::fillMuonSegmentColl(std::vector<const Muon::MuonSegment*> muonSegmentList)
{
  for (MuGirlNS::MuonSegmentList::const_iterator MuonSegItr=muonSegmentList.begin();  MuonSegItr!=muonSegmentList.end(); MuonSegItr++)
    {
      const Muon::MuonSegment* seg = *MuonSegItr;
      //m_pSegmentCollection->push_back(const_cast <Muon::MuonSegment*>(seg));
      m_pSegmentManager->setInStore(seg, false);
    }
}


Rec::TrackParticle* TrigMuGirlTool::TEcreateParticle(const Trk::Track* track,const Trk::VxCandidate* vxCandidate,Trk::TrackParticleOrigin prtOrigin)
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
HLT::ErrorCode TrigMuGirlTool::doMuTruthPerformance()
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
	      if (m_doNTuplePerformance && m_pTrigMuGirlNtuple)
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
