/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// StauTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "MuGirlStau/StauTool.h"
#include "MuGirlInterfaces/MdtSegmentMakerInfo.h"
#include "MuGirlInterfaces/RpcSegmentInfo.h"
#include "MuGirlInterfaces/SegmentManager.h"
#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuGirlStau/IStauBetaTofTool.h"
#include "StoreGate/StoreGateSvc.h"

#include "MuGirlStau/StauNTuple.h"
#include "MuGirlStau/StauFcn.h"
#include "MuGirlStau/StauCosmicFcn.h"
#include "MuGirlStau/StauCollection.h"
#include "TrkEventPrimitives/FitQuality.h"
//#include "Particle/TrackParticle.h"

#include "MuGirlStau/StauRPC.h"
#include "MuGirlStau/StauMDT.h"
#include "MuGirlStau/StauTRT.h"
#include "MuGirlStau/StauTileCal.h"
#include "MuGirlStau/StauGF.h"
#include "MuGirlStau/StauMDTT.h"

#include "EventInfo/TagInfo.h" 
#include "EventInfoMgt/ITagInfoMgr.h" 
#include "MdtCalibSvc/MdtCalibrationDbSvc.h"



//================ Constructor =================================================

MuGirlNS::StauTool::StauTool(const std::string& t,  const std::string& n, const IInterface*  p):
        AlgTool(t,n,p),
        m_log(msgSvc(),n),
        m_addMuToF(true),
        m_rpcBugFix(false),
        m_rpcTimeShift(5.),
        m_tileEnergyCut(0.5),
        m_doGlobalFit(true),
	m_doMdt(true),
	m_doTrt(false),
        m_doRpc(false),
	m_doTileCal(true),
        m_doConsistency(false),
        m_isData(true),
        m_mdtSmearFactor(0.9), //default mid smear 
        m_rpcSmearFactor(-1),
        m_tileSmearFactor(-1),
        m_doCalibration(false),
        m_runNumber(0),
        m_phaseShift(0),
        m_pCollection(NULL),
        m_pMdtSegmentMaker("Muon::DCMathSegmentMaker"),
        m_pMdtDriftCircleCreator("Muon::MdtDriftCircleOnTrackCreator"),
        m_pTofTool("MuGirlNS::StauBetaTofTool"),
        m_pGlobalFitTool("MuGirlNS::GlobalFitTool", 0),    // make this a public tool
	m_pToCalo("TrackInCaloTools/extrapolMuonInCaloTool"),
        m_particleCreatorTool("Trk::TrackParticleCreatorTool/MuidParticleCreatorTool"),
        m_pMdtCalibDbSvc("MdtCalibrationDbSvc", n),
        m_pMuonMgr(NULL),
        m_lowerBetaLimit(0.2),
        m_upperBetaLimit(1.5),
        m_betaTolerance(0.001),
        m_gfBetaTolerance(0.001),
        m_minBeta(0.1),
        m_maxBeta(1.5),
        m_lowerTTrackLimit(-50),
        m_upperTTrackLimit(50),
        m_doMuGirlNtuple(false),
        m_doStauNtuple(false),
        m_pStauRPC(NULL),
        m_pStauMDT(NULL),
        m_pStauTRT(NULL),
        m_pStauGF(NULL),
        m_pStauTileCal(NULL),
        m_pStauMDTT(NULL),
        m_pSegmentManager(NULL),
        m_skipStation(1000),
        m_beta(-1.),
        m_mass(-100.),
        m_tzero(-100.),
        m_p(0.),
        m_pNewMdtSegments(NULL),
        m_pRefittedTrack(NULL),
        m_pMuonRefittedTrack(NULL),
        m_hasSummary(false),
        m_pFcnSteps(NULL),
        m_numMdtSegs(-1),
        m_numMdtHitsInSeg(-1)
{
    declareInterface<IStauTool>(this);

    //  template for property decalration
    //  Tools
    declareProperty("StauMdtSegmentMaker",          m_pMdtSegmentMaker);
    declareProperty("StauMdtDriftCircleCreator",    m_pMdtDriftCircleCreator);
    declareProperty("StauBetaTofTool",              m_pTofTool);
    declareProperty("StauGlobalFitTool",            m_pGlobalFitTool);
    declareProperty("extrapolTrackToCaloToolMuon",  m_pToCalo);
    declareProperty("particleCreatorTool",          m_particleCreatorTool);
    declareProperty("MdtCalibrationDbSvc",          m_pMdtCalibDbSvc);

    //  other properties
    declareProperty("MinBeta",                  m_lowerBetaLimit);
    declareProperty("MaxBeta",                  m_upperBetaLimit);
    declareProperty("betaTolerance",            m_betaTolerance);
    declareProperty("gfBetaTolerance",          m_gfBetaTolerance);
    declareProperty("doStauNtuple",             m_doStauNtuple);
    declareProperty("doMdt",                    m_doMdt);
    declareProperty("doTrt",                    m_doTrt);
    declareProperty("doRpc",                    m_doRpc);
    declareProperty("doTileCal",                m_doTileCal);
    declareProperty("doConsistency",            m_doConsistency);
    declareProperty("doCalibration",            m_doCalibration);
    declareProperty("isData",                   m_isData);
    declareProperty("mdtSmearFactor",           m_mdtSmearFactor);
    declareProperty("rpcSmearFactor",           m_rpcSmearFactor);
    declareProperty("tileSmearFactor",          m_tileSmearFactor);
    declareProperty("rpcTimeShift",             m_rpcTimeShift);
    declareProperty("tileEnergyCut",            m_tileEnergyCut);

 /* calibration */
    declareProperty("mdtCalibFile",            m_mdtCalibFile);
    declareProperty("rpcCalibFile",            m_rpcCalibFile);
    declareProperty("rpcNewCalibFile",            m_rpcNewCalibFile);
    declareProperty("tileCalibFile",           m_tileCalibFile);
    declareProperty("phaseShiftFile",          m_phaseShiftFile);
}

//================ Destructor =================================================

MuGirlNS::StauTool::~StauTool()
{
}


//================ Initialisation =================================================

StatusCode MuGirlNS::StauTool::initialize()
{

    StatusCode sc = AlgTool::initialize();
    m_log.setLevel(outputLevel());
    if (sc.isFailure())
        return sc;
    
    /** initialize tool handles */
    if(retrieve(m_pMdtSegmentMaker).isFailure())
        return StatusCode::RECOVERABLE;
    if(retrieve(m_pMdtDriftCircleCreator).isFailure())
        return StatusCode::RECOVERABLE;
    if(retrieve(m_pTofTool).isFailure())
        return StatusCode::RECOVERABLE;
    if(retrieve(m_pGlobalFitTool).isFailure())
        return StatusCode::RECOVERABLE;
    if(m_doTileCal || m_doConsistency)
    {
        if(retrieve(m_pToCalo).isFailure())
            return StatusCode::RECOVERABLE;
        if(retrieve(m_particleCreatorTool).isFailure())
            return StatusCode::RECOVERABLE;
    }
    else
    {
        m_pToCalo = NULL;
        m_particleCreatorTool = NULL;
    }
    if(retrieve(m_pMdtCalibDbSvc).isFailure())
        return StatusCode::RECOVERABLE;
	
    /** initialize MuonDetectorManager */
    ServiceHandle<StoreGateSvc> pDetStore("DetectorStore",name());
    if(retrieve(pDetStore).isFailure()) return StatusCode::RECOVERABLE;
    sc = pDetStore->retrieve(m_pMuonMgr);
    if( sc.isFailure() || m_pMuonMgr==NULL)
    {
        m_log << MSG::ERROR << " Cannot retrieve MuonDetectorManager" << endreq;
        return StatusCode::RECOVERABLE;
    }

    /** Event store*/
    sc = service("StoreGateSvc", m_storeGate);
    if (sc.isFailure())
    {
       m_log << MSG::ERROR << "Unable to retrieve pointer to StoreGateSvc"  << endreq;
       return sc;
    }

     // **** **** **** TagInfo **** **** **** 
    const DataHandle<TagInfo> tagInfoH; 
    std::string tagInfoKey = ""; 
 
    ITagInfoMgr*  pTagInfoMgr = NULL; 
    sc = service("TagInfoMgr", pTagInfoMgr); 
    if(sc.isFailure() || pTagInfoMgr==0) 
    { 
       m_log << MSG::WARNING << " Unable to locate TagInfoMgr service" << endreq; 
    } 
    else 
       tagInfoKey = pTagInfoMgr->tagInfoKey(); 
         
    //register the call-back to matchTagInfo 
   sc = pDetStore->regFcn(&MuGirlNS::StauTool::readTagInfo,this,tagInfoH,tagInfoKey); 

   if( sc.isFailure() ) 
   { 
      m_log << MSG::WARNING 
            << "Cannot register readTagInfo function for key " 
            << tagInfoKey << endreq; 
   } 
   else 
   { 
      m_log << MSG::DEBUG 
            << "Registered matchTagInfo callback for key: " 
            << tagInfoKey << endreq; 
   } 

    
    m_log << MSG::DEBUG << "initialize() successful in " << name() << endreq;
    m_log << MSG::DEBUG << "addMuToF flag is " << m_addMuToF << endreq;
    m_log << MSG::DEBUG << "rpcBugFix flag is " << m_rpcBugFix << endreq;

    /** calibration */
    if(m_doCalibration)
    {
        const char* mdtCalibFile = m_mdtCalibFile.c_str();
        const char* rpcCalibFile = m_rpcCalibFile.c_str();
        const char* rpcNewCalibFile = m_rpcNewCalibFile.c_str();
        const char* tileCalibFile = m_tileCalibFile.c_str();
        const char* phaseShiftFile = m_phaseShiftFile.c_str();
        m_calibration = StauCalibration(mdtCalibFile,rpcCalibFile, rpcNewCalibFile, tileCalibFile, phaseShiftFile);
    }
    return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode MuGirlNS::StauTool::finalize()
{
    clearCandidate();
    StatusCode sc = AlgTool::finalize();
    return sc;
}

//============================================================================================

//=============== processStau ===================================================
/*
        @task: call the minimization program
*/
StatusCode MuGirlNS::StauTool::processStau(const xAOD::TrackParticle* trkParticle,
                                           const MuonSegmentList& muonSegments, 
                                           const MuGirlNS::MdtSegmentMakerInfoList& mdtSegmentMakerInfoList,
                                           const MuGirlNS::RIO_OnTrackLists& rpcHitsInSegments,
                                           const MuGirlNS::RIO_OnTrackLists& tgcHitsInSegments,
                                           const Trk::Track* pMuonRefittedTrack,
                                           const xAOD::TrackParticle* pMuonTrackParticle,
                                           Amg::Vector3D trigMomentum, const Trk::Perigee* trk0,bool isMuonBetaRefit)
{
    m_log.setLevel(outputLevel());
    //if(NULL!=trkParticle->originalTrack()) m_log << MSG::VERBOSE << "processStau()" << endreq;
    if(NULL!=trkParticle) m_log << MSG::DEBUG << "processStau()" << endreq;
    else m_log << MSG::DEBUG << "processStau() - no ID track" << endreq;
    
    //return processStauCosmic(trkParticle, muonSegments , mdtSegmentMakerInfoList, 
    		                 //rpcHitsInSegments, tgcHitsInSegments, trigMomentum );
    //clear the candidate
    clearCandidate();
    
    //initialize the candidate
    if(!(0!=mdtSegmentMakerInfoList.size() || 0!=rpcHitsInSegments.size() || 0!=tgcHitsInSegments.size() ))
    {
        m_log << MSG::DEBUG << "processStau()::Candidate with no MS info -  done" << endreq;
        return StatusCode::SUCCESS;
    }
    //if (isMuonBetaRefit) m_doGlobalFit = false;
    initializeCandidate(trkParticle,
                        muonSegments,
                        mdtSegmentMakerInfoList,
                        rpcHitsInSegments,
                        tgcHitsInSegments,
                        pMuonRefittedTrack,
                        pMuonTrackParticle,
                        trigMomentum);

    //call the minimization functions
    StauFcn stauFcn(this,m_log);
    m_minBeta = m_lowerBetaLimit;
    m_maxBeta = m_upperBetaLimit;
    if(m_doRpc || m_doMdt)
    {
        if(!stauFcn.selectRange(m_minBeta,m_maxBeta))
        {
            m_beta = StauBetaDefault;
	   m_log << MSG::DEBUG << "processStau() - beta out of range" << endreq;
	   if (isMuonBetaRefit == false)  return StatusCode::SUCCESS;
        }

    }

    m_beta = StauBetaDefault;
    Minimizer minim;

    if( m_doMdt || 
       (m_doRpc && m_pStauRPC->hasHits())    ||
       (m_doTileCal && m_pStauTileCal->hasCells()) )
    {
        double mina = minim.minimize(stauFcn, MuGirlNS::AOD_TECHS, m_minBeta, m_maxBeta, m_betaTolerance);
        if(fabs(m_minBeta-mina)<0.0005 || fabs(m_maxBeta-mina)<0.0005)
	{ 
		m_log << MSG::DEBUG << "processStau() - min is on the edge" << endreq;
                if(m_doRpc && m_pStauRPC->hasHits())
                {
                    m_beta = m_pStauRPC->avgBeta();
                }else m_beta = StauBetaOnEdge;
	}else
        {
            m_beta = mina;
            FcnStepsData fcnSteps = *m_pFcnSteps;
            FcnStepData* pLastStep = fcnSteps.back();
            m_numMdtSegs = (NULL!= pLastStep->mdtData ? (int)pLastStep->mdtData->numSegs : 0);
            m_numMdtHitsInSeg = (NULL!= pLastStep->mdtData ? (int)pLastStep->mdtData->hitsInSegments : 0);

        }
    }//else clearFcnSteps();
    //recalculate and store the mdt segments with the new beta
    if(m_beta>m_lowerBetaLimit) 
    {
        m_pStauMDT->findNewSegments(m_beta);
        m_numMdtSegs = 0;
        m_numMdtHitsInSeg = 0;
        for(std::vector<const Muon::MuonSegment*>::const_iterator itSegs = m_pNewMdtSegments->begin(); itSegs != m_pNewMdtSegments->end(); itSegs++)
        {
            const Muon::MuonSegment* pSeg = *itSegs;
            m_numMdtSegs++; 
            m_numMdtHitsInSeg += pSeg->numberOfContainedROTs(); 
        }
        //refit the track using global fit 
        m_pStauGF->setInitialTrackParameters(trk0);
        if(m_doGlobalFit) m_pRefittedTrack = m_pStauGF->refitTrack();
        m_p = stauMomentum();
        m_mass = stauMass();
    }
   
    updateTechBetaChi2(AOD_TECHS);
    //refit the track using global fit - if there is no beta there is no need to calculate the mass and momentum 
    //if(m_doGlobalFit) m_pRefittedTrack = m_pStauGF->refitTrack();
    //m_p = stauMomentum();
    //m_mass = stauMass();
 
    //do consistency check 
    if(m_doConsistency)
    {
        consistencyCheck();
        printTechBetaChi2();
        printTechContribution2Chi2();
    }    

    printFcnSteps();
    m_log << MSG::VERBOSE << "processStau() - done" << endreq;
    return StatusCode::SUCCESS;
}

void MuGirlNS::StauTool::updateTechBetaChi2(StauTechnology eTech)
{
    FcnStepData* pLastStep = m_pFcnSteps->back();
    m_techBetaChi2[eTech].beta = pLastStep->beta; 
    m_techBetaChi2[eTech].chi2 = pLastStep->chi2; 
    m_techBetaChi2[eTech].dof = pLastStep->dof; 
    if(NULL!=pLastStep->mdtData)
    {
        m_techBetaChi2[eTech].techContribution[MDT_TECH].chi2 = pLastStep->mdtData->chi2;
        m_techBetaChi2[eTech].techContribution[MDT_TECH].dof = pLastStep->mdtData->dof;
    } 
    if(NULL!=pLastStep->mdttData)
    {
        m_techBetaChi2[eTech].techContribution[MDTT_SP_TECH].chi2 = pLastStep->mdttData->chi2;
        m_techBetaChi2[eTech].techContribution[MDTT_SP_TECH].dof = pLastStep->mdttData->dof;
    } 
    if(NULL!=pLastStep->rpcData)
    {
        m_techBetaChi2[eTech].techContribution[RPC_TECH].chi2 = pLastStep->rpcData->chi2;
        m_techBetaChi2[eTech].techContribution[RPC_TECH].dof = pLastStep->rpcData->dof;
    } 
    if(NULL!=pLastStep->tileCalData)
    {
        m_techBetaChi2[eTech].techContribution[TILECAL_TECH].chi2 = pLastStep->tileCalData->chi2;
        m_techBetaChi2[eTech].techContribution[TILECAL_TECH].dof = pLastStep->tileCalData->dof;
    } 
}

void MuGirlNS::StauTool::updateTechContribution2Chi2()
{
    FcnStepData* pLastStep = m_pFcnSteps->back();
    
    m_techContribution2Chi2[MDTT_SP_TECH] = BetaChi2();
    m_techContribution2Chi2[RPC_TECH] = BetaChi2();
    m_techContribution2Chi2[TILECAL_TECH] = BetaChi2();
    m_techContribution2Chi2[TRT_TECH] = BetaChi2();
   
    if(NULL!=pLastStep->mdttData)
    {
        m_techContribution2Chi2[MDTT_SP_TECH].beta = pLastStep->beta; 
        m_techContribution2Chi2[MDTT_SP_TECH].chi2 = pLastStep->mdttData->chi2; 
        m_techContribution2Chi2[MDTT_SP_TECH].dof = pLastStep->mdttData->dof; 
    } 
    if(NULL!=pLastStep->rpcData)
    {
        m_techContribution2Chi2[RPC_TECH].beta = pLastStep->beta; 
        m_techContribution2Chi2[RPC_TECH].chi2 = pLastStep->rpcData->chi2; 
        m_techContribution2Chi2[RPC_TECH].dof = pLastStep->rpcData->dof; 
    } 
    if(NULL!=pLastStep->tileCalData)
    {
        m_techContribution2Chi2[TILECAL_TECH].beta = pLastStep->beta; 
        m_techContribution2Chi2[TILECAL_TECH].chi2 = pLastStep->tileCalData->chi2; 
        m_techContribution2Chi2[TILECAL_TECH].dof = pLastStep->tileCalData->dof; 
    } 
    if(NULL!=pLastStep->trtData)
    {
        m_techContribution2Chi2[TRT_TECH].beta = pLastStep->beta; 
        m_techContribution2Chi2[TRT_TECH].chi2 = pLastStep->trtData->chi2; 
        m_techContribution2Chi2[TRT_TECH].dof = pLastStep->trtData->dof; 
    } 
}

void MuGirlNS::StauTool::printTechContribution2Chi2()
{
    for(std::map<StauTechnology,BetaChi2>::iterator it =  m_techContribution2Chi2.begin(); it!=m_techContribution2Chi2.end(); it++)
    {
        StauTechnology eTech = (*it).first;
        BetaChi2 betaChi2 = (*it).second;
        m_log << MSG::VERBOSE << "processStau(): contribution to chi2:: tech " << technologyName(eTech) << " beta=" << betaChi2.beta << " chi2=" << betaChi2.chi2 << " dof=" << betaChi2.dof << endreq; 
    }
}
void MuGirlNS::StauTool::printTechBetaChi2()
{
    for(std::map<StauTechnology,BetaChi2>::iterator it =  m_techBetaChi2.begin(); it!=m_techBetaChi2.end(); it++)
    {
        StauTechnology eTech = (*it).first;
        BetaChi2 betaChi2 = (*it).second;
        m_log << MSG::VERBOSE << "processStau(): tech " << technologyName(eTech) << " beta=" << betaChi2.beta << " chi2=" << betaChi2.chi2 << " dof=" << betaChi2.dof << endreq; 
    }
}

void MuGirlNS::StauTool::setCandidateAnn(double ann)
{
   if (m_pCollection != NULL)
   {
      unsigned int nCol = m_pCollection->size();
      if(!m_pCollection || 0==nCol)
      {
         m_log << MSG::ERROR << "setCandidateAnn() is called when there is no stau collection" << endreq;
         return;
      }
    //set the ann value to the last entry in the collection
      (*m_pCollection)[nCol-1]->setAnn(ann);
   }
   else 
     return;
}

      
void MuGirlNS::StauTool::consistencyCheck()
{
    m_log << MSG::DEBUG << "processStau() - checking consistency" << endreq;
   
    //add a container for this candidate     
    StauContainer* cont = new StauContainer();
    m_pCollection->push_back(cont);
    cont->setHasRpcHits(m_pStauRPC->hasHits());
    if(m_pIdTrack) cont->setReconstructionParameters(StauContainer::RP_ID);
    else cont->setReconstructionParameters(StauContainer::RP_MF);
    if (m_pStauTileCal) 
    {
        cont->setHasTileCells(m_pStauTileCal->hasCells());
        cont->setTileCells(m_pStauTileCal->caloCells());
    }else if (m_pRefittedTrack !=NULL) 
    {
        m_pStauTileCal = new StauTileCal(this, m_log, m_pRefittedTrack);
        cont->setHasTileCells(m_pStauTileCal->hasCells());
        cont->setTileCells(m_pStauTileCal->caloCells());
    }else if (m_pMuonRefittedTrack !=NULL) 
    {
        m_pStauTileCal = new StauTileCal(this, m_log, m_pMuonRefittedTrack);
        cont->setHasTileCells(m_pStauTileCal->hasCells());
        cont->setTileCells(m_pStauTileCal->caloCells());

    }else cont->setHasTileCells(false);
   
    cont->setBeta(m_beta); 
    cont->setIdTrk( m_pIdTrack );    
    cont->setMuonTrk( m_pMuonRefittedTrack );    
    cont->setStauTrk( m_pRefittedTrack );    
    cont->setRpcHitsByStation( m_pStauRPC->rpcHitsByStations() );
  
    cont->setMfP(m_trigP);  
    if(NULL!=m_pMuonRefittedTrack)
    { 
        double px = (double)m_pMuonRefittedTrack->perigeeParameters()->momentum()[Trk::px];
        double py = (double)m_pMuonRefittedTrack->perigeeParameters()->momentum()[Trk::py];
        double pz = (double)m_pMuonRefittedTrack->perigeeParameters()->momentum()[Trk::pz]; 
        Amg::Vector3D muonP(px,py,pz);        
        cont->setMuonP(muonP);  
    }
    if(NULL!=m_pRefittedTrack)
    { 
        double px = (double)m_pRefittedTrack->perigeeParameters()->momentum()[Trk::px];
        double py = (double)m_pRefittedTrack->perigeeParameters()->momentum()[Trk::py];
        double pz = (double)m_pRefittedTrack->perigeeParameters()->momentum()[Trk::pz]; 
        Amg::Vector3D stauP(px,py,pz);        
        cont->setStauP(stauP);  
    }
 
       
    //Beta in the different technologies
 
    //MDT only
    double mdtBeta = computeTechnologyBeta(MDT_TECH, m_minBeta, m_maxBeta);
    m_pStauMDT->setBeta(mdtBeta);
    //MDTT_MU
    if (m_pMuonRefittedTrack == NULL || !m_pStauMDTT->initialize( m_pMuonRefittedTrack))
    {
        m_log << MSG::DEBUG << "StauMDTT: No muon refitted track or no hits on track. m_pMuonRefittedTrack=" << m_pMuonRefittedTrack  << endreq;
    }else
    {
        computeTechnologyBeta(MDTT_MU_TECH, m_lowerBetaLimit, m_upperBetaLimit);
        m_techBetaAvg[MDTT_MU_TECH].beta = m_pStauMDTT->avgBeta();
        m_techBetaAvg[MDTT_MU_TECH].rms = m_pStauMDTT->rmsBeta();
        if (m_trigP!=Amg::Vector3D(0., 0., 0.)) cont->addHits(m_pStauMDTT->getHits());
    }
    //MDTT_SP 
    if (m_pRefittedTrack == NULL || !m_pStauMDTT->initialize( m_pRefittedTrack))
    {
        m_log << MSG::DEBUG << "StauMDTT: No stau refitted track or no hits on track. m_pRefittedTrack=" << m_pRefittedTrack  << endreq;
    }else
    {
        computeTechnologyBeta(MDTT_SP_TECH, m_lowerBetaLimit, m_upperBetaLimit);
        m_techBetaAvg[MDTT_SP_TECH].beta = m_pStauMDTT->avgBeta();
        m_techBetaAvg[MDTT_SP_TECH].rms = m_pStauMDTT->rmsBeta();
        if (!(m_trigP != Amg::Vector3D(0., 0., 0.))) cont->addHits(m_pStauMDTT->getHits());
    } 
    //RPC only
    if(m_pStauRPC->hasHits())
    {
        double rpcBeta = computeTechnologyBeta(RPC_TECH, m_lowerBetaLimit, m_upperBetaLimit); 
        m_pStauRPC->setBeta(rpcBeta);
        cont->addHits(m_pStauRPC->getHits());
        m_techBetaAvg[RPC_TECH].beta = m_pStauRPC->avgBeta();
        m_techBetaAvg[RPC_TECH].rms = m_pStauRPC->rmsBeta();
    }
    
    //TileCal only
    if(NULL!=m_pStauTileCal && m_pStauTileCal->hasCells())
    {//if there is no ID track, there is no StauTileCal 
        double tileBeta = computeTechnologyBeta(TILECAL_TECH, m_lowerBetaLimit, m_upperBetaLimit); 
        m_pStauTileCal->setBeta(tileBeta);
        cont->addHits(m_pStauTileCal->getHits());
        m_techBetaAvg[TILECAL_TECH].beta = m_pStauTileCal->avgBeta();
        m_techBetaAvg[TILECAL_TECH].rms = m_pStauTileCal->rmsBeta();
        if (m_trigP!=Amg::Vector3D(0., 0., 0.))    
            m_log << MSG::DEBUG << "StauTile: hits " << (m_pStauTileCal->getHits()).size()  << endreq;
    }

    // TRT only
    if(NULL!=m_pStauTRT )
    { 
	double maxTrt = 1.8;
	double minTrt = 0.2;
        double trtBeta = computeTechnologyBeta(TRT_TECH, minTrt, maxTrt);
        m_pStauTRT->setBeta(trtBeta);
        updateTechBetaChi2(TRT_TECH);
        cont->addHits(m_pStauTRT->getHits());
    }
    
    //MS 
    computeTechnologyBeta(MS_TECHS, m_minBeta, m_maxBeta);

    //MST
    if(m_pRefittedTrack != NULL && m_pStauMDTT->initialize( m_pRefittedTrack))
    {
        computeTechnologyBeta(MST_TECHS, m_lowerBetaLimit, m_upperBetaLimit);
    } 

    //No RPC
    computeTechnologyBeta(NORPC_TECHS, m_minBeta, m_maxBeta);

    //No RPCT
    if(m_pRefittedTrack != NULL && m_pStauMDTT->initialize( m_pRefittedTrack))
    {
        computeTechnologyBeta(NORPCT_TECHS, m_lowerBetaLimit, m_upperBetaLimit);
    } 
    
    //No TRT
    computeTechnologyBeta(NOTRT_TECHS, m_minBeta, m_maxBeta);

    //No TRTT
    if(m_pRefittedTrack != NULL && m_pStauMDTT->initialize( m_pRefittedTrack))
    {
        computeTechnologyBeta(NOTRTT_TECHS, m_lowerBetaLimit, m_upperBetaLimit);   
    }
    /*********************************/ 
    updateTechContribution2Chi2(); //This function uses the last step so it has to be executed right after the common fit to which we want to compare the individual contributions
    /*********************************/

    //All
    computeTechnologyBeta(ALL_TECHS, m_minBeta, m_maxBeta);

    //ALLT
    if(m_pRefittedTrack != NULL && m_pStauMDTT->initialize( m_pRefittedTrack))
    {
        computeTechnologyBeta(ALLT_TECHS, m_lowerBetaLimit, m_upperBetaLimit);   
    } 

    StauContainer::Consistency* consistency = new StauContainer::Consistency();
    cont->consistencyList().push_back(consistency);
    if(m_doRpc) consistency->params = StauContainer::CP_WRPC;
    else consistency->params = StauContainer::CP_NORPC;
    consistency->betaAll = m_techBetaChi2[ALL_TECHS].beta; 
    consistency->betaMS = m_techBetaChi2[MS_TECHS].beta; 
    consistency->betaNoRpc = m_techBetaChi2[NORPC_TECHS].beta; 
    consistency->betaMdt = m_pStauMDT->beta(); 
    consistency->betaRpc = m_pStauRPC->beta(); 
    if (m_pStauTileCal)  consistency->betaTile = m_pStauTileCal->beta(); 
    else consistency->betaTile = 0.;
    if (m_pStauTRT) consistency->betaTrt = m_pStauTRT->beta();
    else consistency->betaTrt = 0.;
    consistency->betaMdtt_mu = m_techBetaChi2[MDTT_MU_TECH].beta;
    consistency->betaMdtt_sp = m_techBetaChi2[MDTT_SP_TECH].beta;

    cont->setTechBetaAvg(m_techBetaAvg);	
    cont->setTechBetaChi2(m_techBetaChi2);	
    cont->setTechContribution2Chi2(m_techContribution2Chi2);	
}

double MuGirlNS::StauTool::computeTechnologyBeta(StauTechnology eTech, double minBeta, double maxBeta)
{
    Minimizer minim;
    StauFcn stauFcn(this,m_log);

    m_techBetaChi2[eTech] = BetaChi2();
    double beta = StauBetaOnEdge;
    double tmpBeta =  minim.minimize(stauFcn, eTech, minBeta, maxBeta, m_betaTolerance);
    if(fabs(minBeta - tmpBeta) > 0.0005 && fabs(maxBeta - tmpBeta) > 0.0005) 
    { // not on the edge
        beta = tmpBeta;
        updateTechBetaChi2(eTech);
    }else m_techBetaChi2[eTech].beta = StauBetaOnEdge;
    this->tofTool()->setTShift(0);  
    return beta;
}

double MuGirlNS::StauTool::stauMomentum()
{
   if(NULL!=m_pRefittedTrack) return m_pStauGF->stauMomentum();
   else return m_idP;
}

double MuGirlNS::StauTool::stauMass()
{
    if(0.==m_p) m_p = stauMomentum();
    
    if(m_beta<=m_lowerBetaLimit || m_beta>=1.) return 105.7*CLHEP::MeV; // not a stau, probably muon
    else return m_p * sqrt(1-m_beta*m_beta) / m_beta;
    
}

void MuGirlNS::StauTool::setRefittedTrack(const Trk::Track* pTrack)
{
    //if(NULL!=m_pRefittedTrack) delete m_pRefittedTrack;
    m_pRefittedTrack = pTrack;
}

void MuGirlNS::StauTool::recordCollection()
{
    if(!m_doConsistency) return; 
    m_pCollection = new StauCollection();
//    delete m_pCollection;
    ServiceHandle<StoreGateSvc> pEvtStore("StoreGateSvc",name());
    StatusCode sc = pEvtStore->record(m_pCollection,"StauCandidateCollection",false);
    if(sc.isFailure())
    {
        m_log << MSG::VERBOSE << "Failed recording StauCandidateCollection" << endreq;
    }
}

 
//============================== NTuple functions =======================================
StatusCode MuGirlNS::StauTool::bookNTuple(NTuple::Tuple* pNTuple)
{
    m_doMuGirlNtuple = true; //this funtion is called form MuGirl.cxx if and only if muGirl ntuple is required
    if(!m_doStauNtuple) return StatusCode::SUCCESS;
    if(!m_pGlobalFitTool->bookNTuple(pNTuple,"STAU")) return StatusCode::RECOVERABLE;
    m_pStauNtuple = new StauNTuple(m_log);
    return m_pStauNtuple->book(pNTuple);
}

StatusCode MuGirlNS::StauTool::fillNTuple()
{
    if(!m_doStauNtuple) return StatusCode::SUCCESS;
    return m_pStauNtuple->fillStau(this);
}

//================= initializeCandidate ==========================================
/*
        @task: initiate stau candiate (using information from the candidate tool)
*/
void MuGirlNS::StauTool::initializeCandidate(const xAOD::TrackParticle* trkParticle, 
                                             const MuGirlNS::MuonSegmentList& muonSegments, 
                                             const MuGirlNS::MdtSegmentMakerInfoList& mdtSegmentMakerInfoList, 
                                             const MuGirlNS::RIO_OnTrackLists& rpcHitsInSegments,
                                             const MuGirlNS::RIO_OnTrackLists& tgcHitsInSegments,
                                             const Trk::Track* pMuonRefittedTrack,
                                             const xAOD::TrackParticle* pMuonTrackParticle,
                                             Amg::Vector3D trigMomentum)
{
    m_log << MSG::DEBUG << "initializeCandidate()" << endreq;
    
    //The data from MuGirl candidate
    m_pMdtSegmentMakerInfoList = &mdtSegmentMakerInfoList;
    m_pRpcHitsInSegments = &rpcHitsInSegments;
    m_pTgcHitsInSegments = &tgcHitsInSegments;
      m_pTrkParticle = trkParticle;
    if (NULL != trkParticle) 
       m_pIdTrack = trkParticle->track();
    else 
       m_pIdTrack = NULL;
    m_trigP = trigMomentum;
    m_pMuonRefittedTrack = pMuonRefittedTrack;

    if(m_doCalibration)
    {
        StatusCode sc =  m_storeGate->retrieve(m_pEventInfo);
	if (sc.isFailure())
        {
           m_log << MSG::ERROR << "Unable to retrieve pointer to event store"  << endreq;
        }
        const EventID* myEventID = m_pEventInfo->event_ID();
        m_runNumber = myEventID->run_number();
        std::map<int, double>::iterator itPhaseShift = m_calibration.getPhaseShiftMap()->find(m_runNumber);
        if(itPhaseShift == m_calibration.getPhaseShiftMap()->end())
        {
           m_phaseShift = 0;
        }
        else 
           m_phaseShift = itPhaseShift->second; 
    }
 
    //initiate the 'technologies'
    m_pStauRPC = new StauRPC(this, m_log, rpcHitsInSegments);
    m_pStauMDT = new StauMDT(this, m_log, mdtSegmentMakerInfoList);
    if (m_doGlobalFit) m_pStauGF =  new StauGF( this, m_log, muonSegments); 
    else m_pStauGF = NULL; 
    //ENRIQUE INSERT BACK
    if (m_pIdTrack != NULL && this->doTrt()) m_pStauTRT = new StauTRT(this, m_log, m_pIdTrack);
    
    if(m_pTrkParticle != NULL) m_pStauTileCal = new StauTileCal(this, m_log, m_pTrkParticle);
    else if(pMuonTrackParticle != NULL) m_pStauTileCal = new StauTileCal(this, m_log, pMuonTrackParticle);
    else m_pStauTileCal = NULL;
    m_pStauMDTT = new StauMDTT(this, m_log);

    if (m_pIdTrack !=NULL)
    {
       double px = m_pIdTrack->perigeeParameters()->momentum()[Trk::px];
       double py = m_pIdTrack->perigeeParameters()->momentum()[Trk::py];
       double pz = m_pIdTrack->perigeeParameters()->momentum()[Trk::pz];
       m_idP = sqrt(px*px+py*py+pz*pz);
    }
    else 
       m_idP = m_trigP.mag();

    m_skipStation = 1000;
    //Initiate the output parameters
    m_beta = StauBetaDefault;
    m_mass = -100.;
    m_tzero = -100.;
    m_numMdtSegs = -1;
    m_numMdtHitsInSeg = -1;
    
    //allocate memory for the step data;
    m_pFcnSteps = new FcnStepsData();
    
    //allocate memory to the new mdt segments
    m_pNewMdtSegments = new MdtSegments;
   
    //mark that thiws candidate does not have (yet) a summary
    m_hasSummary = false;
   
    m_log << MSG::DEBUG << "initializeCandidate() - done" << endreq;
}

    

//================= clearCandidate ==========================================
/*
    @task: delete stau candidate and prepare for the next candidate
*/
void MuGirlNS::StauTool::clearCandidate()

{
    m_log << MSG::VERBOSE << "clearCandidate()" << endreq;
    m_beta = StauBetaDefault;
    m_mass = -100.;
    m_tzero = -100.;
    m_techContribution2Chi2.clear();
    m_techBetaChi2.clear();
    m_techBetaAvg.clear();
    m_runNumber = 0;

    m_pRefittedTrack = NULL;
    //clear the vector of new mdt segments.
    // the segments are handles by theSegmentManager
    m_log << MSG::VERBOSE << "clearing new segments" << endreq;
    if(NULL!=m_pNewMdtSegments)
    {
        m_pNewMdtSegments->clear();
        delete m_pNewMdtSegments;
    }
    m_log << MSG::VERBOSE << "new segments cleared" << endreq;
    
    if(NULL!=m_pFcnSteps) clearFcnSteps();
    {
        delete m_pFcnSteps;
	m_pFcnSteps = NULL;
    }
    
    if(NULL != m_pStauRPC) 
    {
        m_pStauRPC->clear();
        delete m_pStauRPC;
    }	
	
    if(NULL != m_pStauTRT) 
    {
	
    	m_pStauTRT->clear();
	
        delete m_pStauTRT;
	m_pStauTRT = NULL;
    }   
    m_log << MSG::VERBOSE << "clearing MDT" << endreq;
    if(NULL != m_pStauMDT) 
    {
        m_pStauMDT->clear();
	delete m_pStauMDT;
    }
    m_log << MSG::VERBOSE << "MDT cleared" << endreq;
    if(NULL != m_pStauGF)
    {
        m_pStauGF->clear();
	delete m_pStauGF;
    }
    m_log << MSG::VERBOSE << "GF cleared" << endreq;
    if(NULL != m_pStauTileCal)
    {
        m_pStauTileCal->clear();
	delete m_pStauTileCal;
    }
    m_log << MSG::VERBOSE << "clearing MDTT" << endreq;
    if(NULL != m_pStauMDTT) 
    {
        m_pStauMDTT->clear();
        delete m_pStauMDTT;
    }
    
    m_log << MSG::VERBOSE << "clearCandidate() - done" << endreq;

}

void MuGirlNS::StauTool::clearNewMdtSegments()
{
    if(m_pNewMdtSegments==NULL) return;
    for(MdtSegments::const_iterator itSeg = m_pNewMdtSegments->begin(); itSeg != m_pNewMdtSegments->end(); itSeg++)
    {
        delete *itSeg;
    }
    m_pNewMdtSegments->clear();
    
}

void MuGirlNS::StauTool::clearFcnSteps()
{
    for(FcnStepsData::iterator it = m_pFcnSteps->begin(); it != m_pFcnSteps->end(); it++)
    {
        FcnStepData* pStepData = *it;
	
	if(NULL!=pStepData->tgcData) delete pStepData->tgcData;
	
	m_pStauMDT->clearStepData(pStepData->mdtData);
	m_pStauRPC->clearStepData(pStepData->rpcData);
	if (NULL!= m_pStauTRT)  m_pStauTRT->clearStepData(pStepData->trtData);
	m_pStauGF->clearStepData(pStepData->globalFitData);
	m_pStauTileCal->clearStepData(pStepData->tileCalData);
	m_pStauMDTT->clearStepData(pStepData->mdttData);
	
	delete pStepData;
    }
    m_pFcnSteps->clear();
    delete m_pFcnSteps;
    m_pFcnSteps = NULL;
}


void MuGirlNS::StauTool::printFcnSteps()
{
    if(NULL == m_pFcnSteps) return;
    for(FcnStepsData::iterator it = m_pFcnSteps->begin(); it != m_pFcnSteps->end(); it++)
    {
	FcnStepData* fcnData = *it;
	
	if(NULL!=m_pStauMDT) m_pStauMDT->printStepData(fcnData->mdtData);
	if(NULL!=m_pStauRPC) m_pStauRPC->printStepData(fcnData->rpcData);
	if(NULL!=m_pStauTRT) m_pStauTRT->printStepData(fcnData->trtData);
	if(NULL!=m_pStauGF) m_pStauGF->printStepData(fcnData->globalFitData);
	if(NULL!=m_pStauTileCal) m_pStauTileCal->printStepData(fcnData->tileCalData);
	if(NULL!=m_pStauMDTT) m_pStauMDTT->printStepData(fcnData->mdttData);
	
    }
}


//=============================== Summary functions ==========================
/*
    @task: prepare stau summary, to be used later in the particle creator tool
*/
StatusCode MuGirlNS::StauTool::fillStauSummary(CandidateSummary* summary,CandidateSummary* stauSummary)
{
    m_log.setLevel(outputLevel());
    m_log << MSG::DEBUG << "fillStauSummary()" << endreq;
   
    //Mark this candidate with hasSummary flag
    m_hasSummary = true;
 
    //paarmeters identical to the muon summary
    stauSummary->pTrackParticle = summary->pTrackParticle;
    stauSummary->pMuonFeature = summary->pMuonFeature;
    stauSummary->startFromMF = summary->startFromMF;
    stauSummary->pMuonEFTrack = summary->pMuonEFTrack;
    stauSummary->startFromEF = summary->startFromEF;
    stauSummary->pLowBetaTrack = summary->pLowBetaTrack;
    stauSummary->pTrkLowBeta   = summary->pTrkLowBeta;
    stauSummary->pLowBetaExtrpTrack = summary->pLowBetaExtrpTrack;
    stauSummary->pTrkLowBetaExtr    = summary->pTrkLowBetaExtr;
    stauSummary->startFromBetaRefit = summary->startFromBetaRefit;
    stauSummary->numRpcPhiHits = summary->numRpcPhiHits;
    stauSummary->numRpcEtaHits = summary->numRpcEtaHits;
    stauSummary->numCscPhiHits = summary->numCscPhiHits;
    stauSummary->numCscEtaHits = summary->numCscEtaHits;
    stauSummary->numRpcSegs = summary->numRpcSegs;
    stauSummary->numRpcHitsInSeg = summary->numRpcHitsInSeg;
    stauSummary->numTgcRpcPhiHits = summary->numTgcRpcPhiHits;
    stauSummary->numTgcRpcEtaHits = summary->numTgcRpcEtaHits;
    stauSummary->numTgcRpcSegs = summary->numTgcRpcSegs;
    stauSummary->numTgcRpcHitsInSeg = summary->numTgcRpcHitsInSeg;
    stauSummary->numTgcPhiHits = summary->numTgcPhiHits;
    stauSummary->numTgcEtaHits = summary->numTgcEtaHits;
    stauSummary->numTgcSegs = summary->numTgcSegs;
    stauSummary->numTgcHitsInSeg = summary->numTgcHitsInSeg;
    stauSummary->numCscSegs = summary->numCscSegs;
    stauSummary->innAngle = summary->innAngle;
    stauSummary->innDelta = summary->innDelta;
    stauSummary->midAngle = summary->midAngle;
    stauSummary->midDelta = summary->midDelta;
    stauSummary->lhr = summary->lhr;
    stauSummary->numMdtHits = summary->numMdtHits;
    stauSummary->nnBarrel = summary->nnBarrel;
    stauSummary->nnEndCap = summary->nnEndCap;
    stauSummary->PassNNsel = summary->PassNNsel;
    stauSummary->saveAnn = summary->saveAnn;
    stauSummary->saveStau = summary->saveStau;
    stauSummary->rpcHitsLists = (m_pStauRPC!=NULL ? m_pStauRPC->copyStauRpcHitsLists() : NULL);
    
    //stau related parameters
    for(std::vector<const Muon::MuonSegment*>::const_iterator itSegs = (summary->muonSegmentList).begin(); itSegs != (summary->muonSegmentList).end(); itSegs++)
    {
        if (m_pSegmentManager->isNotMDT(*itSegs))
            (stauSummary->muonSegmentList).push_back(*itSegs);
    }

    for(std::vector<const Muon::MuonSegment*>::const_iterator itSegs = m_pNewMdtSegments->begin(); itSegs != m_pNewMdtSegments->end(); itSegs++)
    {
        (stauSummary->muonSegmentList).push_back(*itSegs);
        m_pSegmentManager->addSegment(*itSegs, SegmentManager::MDT_STAU);
    }

   // FcnStepsData fcnSteps = *m_pFcnSteps;
   // FcnStepData* pLastStep = fcnSteps.back();
    stauSummary->numMdtSegs = m_numMdtSegs; //(int)pLastStep->mdtData->numSegs;
    stauSummary->numMdtHitsInSeg = m_numMdtHitsInSeg; //(int)pLastStep->mdtData->hitsInSegments;
    stauSummary->beta = m_beta;
    stauSummary->pRefittedTrack = NULL;
    stauSummary->pTrkRefitted   = NULL;
    stauSummary->pMSRefittedTrack = NULL;
    stauSummary->pTrkMSRefitted   = NULL;
    
 
    m_log << MSG::DEBUG << "fillStauSummary() - done" << endreq;
    return StatusCode::SUCCESS;
}

//=============== functions related to the minimization process================ 




/*
   Task: 
   return code: True = recalculate first steps
*/
bool MuGirlNS::StauTool::recalculateFirstSteps()
{
    m_log << MSG::VERBOSE << "recalculateFirstSteps()" << endreq;
    
    if(4!=m_pFcnSteps->size())
    {
        m_log << MSG::ERROR << "recalculateFirstSteps()::size of m_pFsnSteps != 4  --> BUG" << endreq;
        return false;
    }
    
    FcnStepsData firstSteps = *m_pFcnSteps;
    FcnStepData* pStep0 = firstSteps[0];
    FcnStepData* pStep1 = firstSteps[1];
    FcnStepData* pStep2 = firstSteps[2];
    FcnStepData* pStep3 = firstSteps[3];

    if( 1>=pStep0->mdtData->pStationDataList->size() ) return false;    
    
    if( ( pStep0->mdtData->pStationDataList->size() != pStep1->mdtData->pStationDataList->size() ) ||
        ( pStep0->mdtData->pStationDataList->size() != pStep2->mdtData->pStationDataList->size() ) ||
        ( pStep0->mdtData->pStationDataList->size() != pStep3->mdtData->pStationDataList->size() ) )
    {
        m_log << MSG::ERROR << "recalculateFirstSteps()::different number of stations  --> BUG" << endreq;
        return false;
    }
    
    for(unsigned int i=0; i<pStep0->mdtData->pStationDataList->size(); i++)
    {
        double step0ratio = (*(*pStep0->mdtData->pStationDataList)[i]).chi2 / 
	                    (pStep0->chi2 * ( pStep0->dof ) );
        double step1ratio = (*(*pStep1->mdtData->pStationDataList)[i]).chi2 /
	                    (pStep1->chi2 * ( pStep1->dof ) );
	double step2ratio = (*(*pStep2->mdtData->pStationDataList)[i]).chi2 /
	                    (pStep2->chi2 * ( pStep2->dof ) );
	double step3ratio = (*(*pStep3->mdtData->pStationDataList)[i]).chi2 /
	                    (pStep3->chi2 * ( pStep3->dof ) );
	double chi2ratio = step0ratio + step1ratio + step2ratio + step3ratio;
	
	if(chi2ratio<3) continue;
	//remove the station
	m_log << MSG::VERBOSE << "recalculateFirstSteps()::removing station #" << i << endreq;
	m_skipStation = i;
	clearFcnSteps();
	
	return true;
    }
    
    m_log << MSG::VERBOSE << "recalculateFirstSteps() - done" << endreq;
    return false;
}



bool MuGirlNS::StauTool::selectRange(double& min, double& max)
{
    //return true;
    m_log << MSG::VERBOSE << "selectRange() " << endreq;
    m_lowerLimitNoMdt = min;
    m_upperLimitNoMdt = max;   
 
    FcnStepsData firstSteps = *m_pFcnSteps;
    
    unsigned int iFirstStep = 0;
    unsigned int iLastStep = firstSteps.size()-1;
    unsigned int minRpcLoc = ( NULL!=m_pStauRPC && m_doRpc ? m_pStauRPC->rpcMinIsAt() : 0 );
    unsigned int minTileLoc = ( NULL!=m_pStauTileCal && m_pStauTileCal->hasCells()  ? m_pStauTileCal->tileMinIsAt() : 0 );
    unsigned int minRpcTileLoc = (0!=minRpcLoc ? minRpcLoc : minTileLoc);
    //unsigned int minRpcTileLoc = minRpcLoc; //The tile beta distribution seems wide. Do not select the range according to it. 
    if(0!=minRpcTileLoc)
    {
        iFirstStep = (minRpcTileLoc>3 ? minRpcTileLoc-3 : 0);
	iLastStep = (minRpcTileLoc+3<iLastStep ? minRpcTileLoc+3 : iLastStep);
	min = m_lowerLimitNoMdt = firstSteps[iFirstStep]->beta;// - 0.1;
	max = m_upperLimitNoMdt = firstSteps[iLastStep]->beta;//  + 0.1;
        if(!m_doMdt) return true;
    }

    m_log << MSG::VERBOSE << "selectRange() - minRpcLoc " << minRpcLoc << " minTileLoc " << minTileLoc << " minRpcTileLoc " << minRpcTileLoc << endreq;
    m_log << MSG::VERBOSE << "selectRange() - before mdt selection: min=" << min << " max=" << max << endreq;
    
//    return true;      
 
    double maxHits = 0;
    bool decrease = false;
    bool reduceMaxHits = false;
    int loc = 0;
    for(unsigned int i=iFirstStep; i<=iLastStep; i++)
    {
        FcnStepData* pStep = firstSteps[i];
        double numSegs = pStep->mdtData->numSegs;
	double numHits = pStep->mdtData->hitsInSegments;
        m_log << MSG::VERBOSE  << "beta=" << pStep->beta 
		  << " chi2=" << pStep->chi2
	          << " numHits=" << pStep->mdtData->hitsInSegments 
		  << " maxHits=" << maxHits 
		  << " min=" << min << " max=" << max  
                  << " numSegs=" << numSegs << endreq;
 
        if(0 == numSegs) continue;
      
	if(maxHits < numHits)
        {
           maxHits = numHits;
           loc = i;
           min = pStep->beta;
           max = pStep->beta;
           decrease = false;
        }else if(maxHits == numHits)
        {
            max = pStep->beta;
            loc = i;
            if(decrease && !reduceMaxHits)
            {
                min = pStep->beta;
            }
            decrease = false;
	    reduceMaxHits = false;
        }else
        { 
	    double extraHits = maxHits - numHits;
	    double diffChi2 = firstSteps[i-1]->mdtData->chi2 - pStep->mdtData->chi2;
	    if(diffChi2/extraHits > 15)
	    {//the additional hits contributes too much to the chi2
	        maxHits = numHits;
                min = pStep->beta;
                max = pStep->beta;
		reduceMaxHits = true;
	    }else reduceMaxHits = false;
            decrease = true;
        }
    }
   
    m_log << MSG::VERBOSE << "selectRange() - done: min=" << min << " max=" << max << endreq;
    if(min==max)
    {
         if(0==loc || (int)firstSteps.size()-1==loc) return false;
         double locChi2 = firstSteps[loc]->chi2;
         double prevChi2 = firstSteps[loc-1]->chi2;
         double nextChi2 = firstSteps[loc+1]->chi2; 
         if(locChi2>prevChi2 || locChi2>nextChi2) return false;
         min -= 0.05;
         max += 0.05;
    }
    return true;
}



StatusCode MuGirlNS::StauTool::readTagInfo(IOVSVC_CALLBACK_ARGS) 
{ 
    // Get TagInfo and retrieve tags
    m_log.setLevel(outputLevel()); 
    m_log << MSG::DEBUG 
          << "Starting readTagInfo" 
          << endreq; 
    
    if (m_isData)
    {
        m_log << MSG::DEBUG 
              << "This is data: exiting readTagInfo, use default configuration: " 
              << "addMuToF is set to  "<< m_addMuToF <<" timeShift is "<< m_rpcTimeShift <<
                  " rpcBugFix is " << m_rpcBugFix      
              << endreq; 
        return StatusCode::SUCCESS; 
    } 
    const TagInfo* tagInfo = 0; 
    ServiceHandle<StoreGateSvc> pDetStore("DetectorStore",name()); 
    if(retrieve(pDetStore).isFailure()) return StatusCode::RECOVERABLE; 
    StatusCode sc = pDetStore->retrieve(tagInfo); 
 
    std::string rpcDigit = ""; 
    std::string atlasRel = ""; 
    std::string timeShift = ""; 
    std::string atlasRel_main = "";
    if (sc.isFailure() || tagInfo==0) 
    { 
        m_log << MSG::DEBUG 
           << "No TagInfo in DetectorStore" 
            << endreq; 
        m_addMuToF=true; 
        return StatusCode::RECOVERABLE; 
     } 
     m_log << MSG::DEBUG
           << "Found TagInfo in DetectorStore"
           << endreq;
 
     TagInfo::NameTagPairVec aNameTagPairVec; 
     tagInfo->getInputTags(aNameTagPairVec); 
     for (unsigned int Item = 0; Item < aNameTagPairVec.size(); Item++)  
     { 
        if(aNameTagPairVec[Item].first == "RPC_TimeSchema")  
        { 
           rpcDigit = aNameTagPairVec[Item].second;   
           m_log << MSG::DEBUG << "RPC_Digitization from TagInfoVector is "<< rpcDigit <<endreq;    
        } 
        if(aNameTagPairVec[Item].first == "AtlasRelease")  
        { 
           atlasRel = aNameTagPairVec[Item].second;   
           m_log << MSG::DEBUG << "AtlasRelease from TagInfoVector is "<< atlasRel <<endreq;    
        }   
    } 
    if (rpcDigit == "") 
    { 
       if (atlasRel == "")
       {
          m_addMuToF = true;
          m_rpcTimeShift = 0;
	  m_rpcBugFix = false;
          m_log << MSG::DEBUG << "this is Data: addMuToF is set to  "<< m_addMuToF <<endreq;   

       }
       else 
       {
          atlasRel_main = atlasRel.substr(13,14);
          int atlRel_main = atoi(atlasRel_main.c_str());
          if (atlRel_main >= 15)
          {
              if(atlasRel.substr(0,19) =="AtlasOffline-15.3.1" && atlasRel.substr(0,21) != "AtlasOffline-15.3.1.8") 
              { 
                 m_addMuToF = true; 
                 m_rpcTimeShift = 0; 
	         m_rpcBugFix = false;
 	          
              } 
              else if( atlasRel.substr(0,21) == "AtlasOffline-15.3.1.8")
              { 
                 m_addMuToF = true; 
                 m_rpcTimeShift = 5; 
		 m_rpcBugFix = false;
              }else 
	      {
	          m_addMuToF = false;
		  m_rpcTimeShift = 0;
		  m_rpcBugFix = false;
	      }
          }
          else
          {
                m_addMuToF = false;
	        m_rpcTimeShift = 0;
		m_rpcBugFix = true;

                m_log << MSG::DEBUG << "Release "<<atlRel_main<< ": addMuToF is set to  "<< m_addMuToF <<endreq;   
          }  
       }
    }
    else
    {
       if (rpcDigit.substr(0,8) == "Datalike") 
       { 
          m_addMuToF = true; 
          timeShift=rpcDigit.substr(25,1); 
          m_rpcTimeShift=atoi(timeShift.c_str());  
	  m_rpcBugFix = false;
       }     
       if (rpcDigit.substr(0,6) == "G4like") 
       { 
          m_addMuToF = false; 
          timeShift=rpcDigit.substr(23,1); 
          m_rpcTimeShift=atoi(timeShift.c_str());  
	  m_rpcBugFix = false;
       } 
  
    }
    m_log << MSG::DEBUG << "addMuToF is set to  "<< m_addMuToF <<" timeShift is "<< m_rpcTimeShift <<
    " rpcBugFix is " << m_rpcBugFix <<  endreq;     
           
    return StatusCode::SUCCESS; 
}  


StatusCode MuGirlNS::StauTool::processStauCosmic(const xAOD::TrackParticle* trkParticle,
                                           const MuonSegmentList& muonSegments, 
                                           const MuGirlNS::MdtSegmentMakerInfoList& mdtSegmentMakerInfoList,
                                           const MuGirlNS::RIO_OnTrackLists& rpcHitsInSegments,
                                           const MuGirlNS::RIO_OnTrackLists& tgcHitsInSegments,
                                           Amg::Vector3D trigMomentum)
{
    m_log.setLevel(outputLevel());
    //if(NULL!=trkParticle->originalTrack()) m_log << MSG::VERBOSE << "processStau()" << endreq;
    if(NULL!=trkParticle) m_log << MSG::VERBOSE << "processStau()" << endreq;
    else m_log << MSG::VERBOSE << "processStauCosmic() - no ID track" << endreq;
    //clear the candidate
    clearCandidate();
    //double trigMomentum;
    double beta = (trkParticle->phi() > 0) ? -1.0 : 1.0; // TODO change to y coordinate of the hits
    //initialize the candidate
    if(!(0!=mdtSegmentMakerInfoList.size() || 0!=rpcHitsInSegments.size() || 0!=tgcHitsInSegments.size() ))
    {
        m_log << MSG::VERBOSE << "processStauCosmic()::Candidate with no MS info -  done" << endreq;
	return StatusCode::SUCCESS;
    }
    
    initializeCandidate(trkParticle, muonSegments, mdtSegmentMakerInfoList, rpcHitsInSegments, tgcHitsInSegments ,NULL,NULL,trigMomentum);
    
    //call the minimization functions
    StauCosmicFcn stauCosmicFcn(this,m_log,beta);
    m_minTTrack = m_lowerTTrackLimit;
    m_maxTTrack = m_upperTTrackLimit;
    if(m_doRpc || m_doMdt)
    {
        if(!stauCosmicFcn.selectRange(m_minTTrack,m_maxTTrack))
        {
            m_tTrack = StauTTrackDefault;
            m_beta = beta;
	    //std::cout << "E123: " << m_minTTrack << " " <<  m_maxTTrack << std::endl;
	    return StatusCode::SUCCESS;
        }

    }

    m_tTrack = StauTTrackDefault;
    Minimizer minim;

    if( m_doMdt || 
       (m_doRpc && m_pStauRPC->hasHits())    ||
       (m_doTileCal && m_pStauTileCal->hasCells()) )
    {
	//double max = 25.;
	//double min = -25.;
        m_minTTrack = -50.0; m_maxTTrack = 50.0;
	double mina = minim.minimize(stauCosmicFcn, MuGirlNS::ALL_TECHS, m_minTTrack,m_maxTTrack, m_betaTolerance);
	m_minTTrack = -50.0; m_maxTTrack = 50.0;
	double mina_noTrt =  minim.minimize(stauCosmicFcn, MuGirlNS::NOTRT_TECHS,  m_minTTrack,m_maxTTrack , m_betaTolerance);
	m_minTTrack = -50.0; m_maxTTrack = 50.0;
	double mina_trt =  minim.minimize(stauCosmicFcn, MuGirlNS::TRT_TECH, m_minTTrack,m_maxTTrack , m_betaTolerance);
        std::cout << "E123: MINA " << mina << " noTrt " << mina_noTrt << " onlyTrt " << mina_trt << " phi " << trkParticle->phi() << " eta " <<
	trkParticle->eta()  << " pt:  " <<  trkParticle->pt() <<  " d0 " << trkParticle->perigeeParameters().parameters()[Trk::d0] << " z0  " << trkParticle->perigeeParameters().parameters()[Trk::z0] << std::endl;
        if(fabs(m_minTTrack-mina)<0.0005 || fabs(m_maxTTrack-mina)<0.0005)
	{ 
		m_log << MSG::VERBOSE << "processStauCosmic() - min is on the edge" << endreq;
                if(m_doTrt && m_pStauTRT->hasHits())
                {
//                    m_pStauTRT->setAverageTTrack();
                    m_tTrack = m_pStauTRT->averageTTrack();
                }else m_tTrack = StauTTrackOnEdge;
	}else
        {
            m_tTrack = mina;
           // FcnStepsData fcnSteps = *m_pFcnSteps;
           // FcnStepData* pLastStep = fcnSteps.back();
           // m_numMdtSegs = (NULL!= pLastStep->mdtData ? (int)pLastStep->mdtData->numSegs : 0);
           // m_numMdtHitsInSeg = (NULL!= pLastStep->mdtData ? (int)pLastStep->mdtData->hitsInSegments : 0);

        }
    }//else clearFcnSteps();
    //recalculate and store the mdt segments with the new beta
   
    
   // printFcnSteps();

    //std::cout << "All betas:  AOD = " << m_beta << " ALL = " << m_betaAll << " MS = " << m_betaMS << " noRpc = " << m_betaNoRpc << " MDT = " << m_pStauMDT->beta() << " RPC = " << m_pStauRPC->beta() << " Tile = " << m_pStauTileCal->beta() << std::endl;

    m_log << MSG::VERBOSE << "processStauCosmic() - done" << endreq;
    return StatusCode::SUCCESS;
}

bool MuGirlNS::StauTool::selectRangeTTrack(double& min, double& max)
{
    
    //return true;
    m_log << MSG::VERBOSE << "selectRangeTTrack() " << endreq;
    if (noIDTrack()) return false;
    
    FcnStepsData firstSteps = *m_pFcnSteps;
    
    unsigned int iFirstStep = 0;
    unsigned int iLastStep = firstSteps.size()-1;
    unsigned int minRpcLoc = ( NULL!=m_pStauRPC && m_doRpc ? m_pStauRPC->rpcMinIsAt() : 0 );
    if(0!=minRpcLoc)
    {
        iFirstStep = (minRpcLoc>2 ? minRpcLoc-2 : 0);
	iLastStep = (minRpcLoc+2<iLastStep ? minRpcLoc+2 : iLastStep);
	min = firstSteps[iFirstStep]->tTrack - 2.0; 
	max = firstSteps[iLastStep]->tTrack  + 2.0; 
        if(!m_doMdt) return true;
    }
    //std::cout << "E123:: tTrack,mdtChi2,mdtDof,tileChi2,tileDof,trtChi2,trtDof,beta" << std::endl;
    //double maxHits = 0;
    //bool decrease = false;
    //bool reduceMaxHits = false;
    unsigned int loc = 0;
    Range bestRange;
    Range currentRange;
    for(unsigned int i=iFirstStep; i<=iLastStep; i++)
    {
        FcnStepData* pStep = firstSteps[i];
        double numSegs = pStep->mdtData->numSegs;
	double numHits = pStep->mdtData->hitsInSegments;
        //m_log << MSG::VERBOSE  // tTrack:mdtChi2:mdtSegs:mdtHits:
		  std::cout << "E123:: "  << pStep->tTrack 
		  << "," << pStep->mdtData->chi2
		  << "," << pStep->mdtData->totNumHits - 2*pStep->mdtData->pStationDataList->size() 
		  << "," << pStep->mdtData->hitsInSegments
	          << "," << pStep->tileCalData->chi2
		  << "," << pStep->tileCalData->numOfCells  
		  << "," << pStep->trtData->chi2 
		  << "," << pStep->trtData->nHits
		  << "," << pStep->trtData->beta 
		  << std::endl;
 
        if(0 == numSegs) continue;
      
        if (currentRange.nHits != numHits) currentRange.clear();

	if (!currentRange.addStepLast(i, numHits, pStep->mdtData->chi2 , pStep->tTrack))
	{
		std::cout << "E123: " << __LINE__ << " sanity check failed" << std::endl;
		
	}
	std::cout << "E123: " << __LINE__ << " size " << currentRange.size() << std::endl;
	if (currentRange.isBetter(bestRange)){ std::cout << "E123:  current range is better: " << std::endl; bestRange = currentRange;}	
    }
  
	
    if (bestRange.size() < 1 ) {  return false; std::cout << "E123: didnt find a range" << std::endl; }  
    if (bestRange.size() > 1 ) 
    {
	max = ( bestRange.max < ( bestRange.minChi2Param + 2.0 )) ?  bestRange.max :  bestRange.minChi2Param + 2.0;
	min = ( bestRange.min > ( bestRange.minChi2Param - 2.0 )) ?  bestRange.min :  bestRange.minChi2Param - 2.0;
	std::cout << "E123: " << "range" << max << " " << min << std::endl;
	return true;

    } 
    if ( bestRange.size() == 1)
    {

	if (loc < 2 || loc > firstSteps.size() - 2) return false;
	loc = bestRange.i_minChi2;
	double locChi2 = firstSteps[loc]->chi2;
        double prevChi2 = firstSteps[loc-1]->chi2;
        double nextChi2 = firstSteps[loc+1]->chi2; 
        if(locChi2>prevChi2 || locChi2>nextChi2) return false;
        min = bestRange.min - 2.0;
	max = bestRange.max + 2.0;
	return true;
    }
    m_log << MSG::VERBOSE << "selectRangeTTrack() - done: min=" << min << " max=" << max << endreq;
    
    return false;
}





