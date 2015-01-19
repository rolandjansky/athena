/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


                                                                     
                                                                     
                                             
//////////////////////////////////////////////////////////////////
// MuGirlParticleCreatorTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "MuGirlParticleCreatorTool/MuGirlParticleCreatorTool.h"
#include "MuGirlParticleCreatorTool/ParticleCreatorNTuple.h"
#include "MuonSegment/MuonSegment.h"
#include "GaudiKernel/NTuple.h"
#include "xAODEventInfo/EventInfo.h"
#include "muonEvent/MuonContainer.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"

//#include "MuidInterfaces/IMuonIdentificationHelper.h"
#include "MuidInterfaces/IMuonTrackQuery.h"

#include "xAODTracking/TrackParticleContainer.h"

namespace MuGirlNS
{
//================ Constructor =================================================

MuGirlParticleCreatorTool::MuGirlParticleCreatorTool(const std::string& t,
        const std::string& n,
        const IInterface* p) :
        AthAlgTool(t,n,p),
        //m_muGirlCombinedMuonLocation("MuidMuonCollection"),
        m_muGirlCombinedMuonLocation("MuGirlCombinedMuonContainer"),
        m_inDetParticlesLocation("InDetTrackParticles"), //"TrackParticleCandidate"),
	m_inExtrpParticlesLocation("MuGirlRefittedTrackParticles"),
        //m_msRefitParticlesLocation("MuGirlMuonSpectrometerTrackParticles"),
        //m_muonIsolationTool("MuonIsolationTool"),
        m_pCaloEnergyTool("Rec::MuidCaloEnergyTool/MuidCaloEnergyTool"),
        //m_helper("Rec::MuonIdentificationHelper/MuonIdentificationHelper"),
        m_trackQuery("Rec::MuonTrackQuery/MuonTrackQuery"),
        m_pTruthTool("MuGirlNS::PerformanceTruthTool"),
        m_doTruth(false),
        m_doFill(true),
        m_doNTuple(false),
        m_doMSRefit(false), 
        m_sOverlap("MuGirlOverlap.txt"),
        m_fOverlap(NULL),
        m_pRawNTuple(NULL),
        m_pNTuple(NULL)
{
    declareInterface<IMuGirlParticleCreatorTool>(this);
    declareProperty("MuonContainerLocation",         m_muGirlCombinedMuonLocation);
    declareProperty("inDetTrackParticlesLocation",   m_inDetParticlesLocation);
    declareProperty("inExtrpTrackParticlesLocation", m_inExtrpParticlesLocation);
    //declareProperty("msRefitParticlesLocation",      m_msRefitParticlesLocation);
    //declareProperty("MuonIsolationTool",             m_muonIsolationTool);
    declareProperty("MuidCaloEnergyTool",            m_pCaloEnergyTool);
    //declareProperty("MuonIdentificationHelper",                 m_helper);
    declareProperty("PerformanceTruthTool",          m_pTruthTool);
    declareProperty("doTruth",                       m_doTruth);
    declareProperty("doFill",                        m_doFill);
    declareProperty("doNTuple",                      m_doNTuple);
    declareProperty("doMSRefit",                     m_doMSRefit);
    declareProperty("OverlapFileName",               m_sOverlap);
}

//================ Destructor =================================================

MuGirlParticleCreatorTool::~MuGirlParticleCreatorTool()
{}

//================ Initialisation =================================================

StatusCode MuGirlParticleCreatorTool::initialize()
{
    msg(MSG::INFO) << "Initializing " << name() << " - package version " << PACKAGE_VERSION << endreq;
    StatusCode sc = AthAlgTool::initialize();
    if (sc.isFailure())
        return sc;

    if (msgLvl(MSG::DEBUG))
        msg() << "MuGirlParticleCreatorTool::initialize: doTruth=" << m_doTruth << ", doNTuple=" << m_doNTuple << " doFill "<<m_doFill<< endreq;

    if (service("NTupleSvc", m_pNTupleSvc, true).isFailure() || m_pNTupleSvc == NULL)
    {
        msg(MSG::WARNING) << "NTupleSvc service not found" << endreq;
        return StatusCode::RECOVERABLE;
    }
/*
    if (m_doFill)
    {
        if (retrieve(m_muonIsolationTool).isFailure())
            return StatusCode::RECOVERABLE;
    }
*/ 
    if (retrieve(m_pCaloEnergyTool).isFailure())
        return StatusCode::RECOVERABLE;
    //if (retrieve(m_helper).isFailure())
    //    return StatusCode::RECOVERABLE;

    if (m_doTruth)
    {
        if (retrieve(m_pTruthTool).isFailure())
            return StatusCode::RECOVERABLE;
    }

    if (m_doNTuple)
    {
        std::string ntupleName = "/NTUPLES/MUGIRL/AOD";
        m_pRawNTuple = m_pNTupleSvc->book(ntupleName, CLID_ColumnWiseTuple, "Muons in the AOD");
        if (m_pRawNTuple == NULL)
        {
            msg(MSG::WARNING) << "Could not create NTuple " << ntupleName << endreq;
            return StatusCode::RECOVERABLE;
        }
        m_pNTuple = new ParticleCreatorNTuple(msg());
        if (m_pNTuple->book(m_pRawNTuple).isFailure())
        {
            msg(MSG::WARNING) << "Could not book NTuple " << ntupleName << endreq;
            return StatusCode::RECOVERABLE;
        }

        m_fOverlap = fopen(((std::string)m_sOverlap).c_str(), "w");
        fprintf(m_fOverlap, "HitsI/I:HitsJ:OverlapHits:Dropped:"
                "IDEtaI/D:IDPhiI:IDPtI:RefitEtaI:RefitPhiI:RefitPtI:ChiSqI:DofI/I:AnnBarrelI/D:AnnEndcapI:InnAngleI:MidAngleI:PdgIDI/I:"
                "IDEtaJ/D:IDPhiJ:IDPtJ:RefitEtaJ:RefitPhiJ:RefitPtJ:ChiSqJ:DofJ/I:AnnBarrelJ/D:AnnEndcapJ:InnAngleJ:MidAngleJ:PdgIDJ/I\n");
        fflush(m_fOverlap);
    }


    if (retrieve(m_trackQuery).isFailure())
    {
       msg(MSG::WARNING)<< "Failed to retrieve tool " << m_trackQuery <<endreq;
       return StatusCode::RECOVERABLE;
    }

    msg(MSG::INFO) << "initialize() successful in " << name() << endreq;
    return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode MuGirlParticleCreatorTool::finalize()
{
    if (m_fOverlap != NULL)
    {
        fclose(m_fOverlap);
        m_fOverlap = NULL;
    }
    if (m_pNTuple != NULL)
        delete m_pNTuple;

    return AlgTool::finalize();
}

//============================================================================

StatusCode MuGirlParticleCreatorTool::fillContainer(CandidateSummaryList* SummaryList, bool onlyNTupleFill)
{

    if (msgLvl(MSG::DEBUG)) msg() << "fillContainer " << name() << endreq;
    if ( !m_doFill )
    {
       if (msgLvl(MSG::DEBUG)) msg() << "doFill is false! Will not fill Muon Container " << endreq;
       return StatusCode::SUCCESS;  
    }
    Analysis::MuonContainer* pMuonContainer = NULL;

    const Trk::SegmentCollection* pSegmentContainer=NULL;

    StatusCode sc;
    sc = evtStore()->retrieve(pSegmentContainer, "MuGirlSegments");
    if (sc.isFailure() || pSegmentContainer == NULL)
    {
        msg(MSG::WARNING) << "Could not get MuGirlSegments" << endreq;
        return sc;
    }


    const xAOD::TrackParticleContainer* inExtrpParticleContainer = NULL;
    if (!onlyNTupleFill) 
    {
      pMuonContainer = new Analysis::MuonContainer;
      sc = evtStore()->retrieve(inExtrpParticleContainer, "MuGirlRefittedTrackParticles");
      if (sc.isFailure() || inExtrpParticleContainer == NULL)
        {
  	  msg(MSG::WARNING) << " Could not retrieve RefittedTrackParticleCandidate container " << endreq;
	  return sc;
        }
    }
    //const Rec::TrackParticleContainer* msParticleContainer = NULL;
    //if (m_doMSRefit)
    //  {
    //	//sc = evtStore()->retrieve(msParticleContainer, "MuGirlMuonSpectrometerTrackParticles");
    //	sc = evtStore()->retrieve(msParticleContainer, "MuGirlRefittedTrackParticles");
    //	if (sc.isFailure() || msParticleContainer == NULL)
    //	  {
    //	    msg(MSG::WARNING) << " Could not retrieve MSTrackParticleCandidate container " << endreq;
    //	    return sc;
    //	  }
    //  }
    const xAOD::TrackParticleContainer * inDetParticleContainer = NULL;
    if(!onlyNTupleFill)
    {
      sc = evtStore()->retrieve(inDetParticleContainer, m_inDetParticlesLocation);
      if (sc.isFailure() || inDetParticleContainer == NULL)
      {
          msg(MSG::WARNING) << " Could not retrieve TrackParticleCandidate container " << endreq;
          return sc;
      }
    }
    std::vector<bool> IncludeList;

    if (!(SummaryList->empty()))
    {
        removeRedundantCandidates(SummaryList, IncludeList);

        int iCand = -1;
        for (CandidateSummaryList::iterator itSummary = SummaryList->begin(); itSummary !=SummaryList->end(); itSummary++)
        {
            iCand++;
            if (!IncludeList[iCand])
                continue;
    
     
            const CandidateSummary* pSummary = *itSummary;
/*
            const xAOD::TrackParticle* pTrackParticle = NULL;
            if (pSummary->startFromMF == false && pSummary->startFromEF == false)  
                pTrackParticle = pSummary->pTrackParticle;
            int nMDT = pSummary->numMdtHits;
            int nEtaCSC = pSummary->numCscEtaHits;
            int nPhiCSC = pSummary->numCscPhiHits;
            int nEtaTGC = pSummary->numTgcEtaHits;
            int nPhiTGC = pSummary->numTgcPhiHits;
            int nEtaRPC = pSummary->numRpcEtaHits;
            int nPhiRPC = pSummary->numRpcPhiHits;
            double nnBarrel = pSummary->nnBarrel;
            double nnEndCap = pSummary->nnEndCap;
            double innAng = pSummary->innAngle;
            double midAng = pSummary->midAngle;
            double b = pSummary->beta;
            int saveStau = pSummary->saveStau;

            std::vector<const Muon::MuonSegment*> muonSeg = pSummary->muonSegmentList;
            std::vector<const Trk::Segment*> newSegments;

            for (std::vector<const Muon::MuonSegment*>::iterator it = muonSeg.begin();
                    it != muonSeg.end();
                    it++)
            {
                newSegments.push_back(*it); // The pointer is converted by the compiler to the base-class
            }

            Analysis::Muon* pCBMuon = 0;
            double  Mass= 105.7;
	    
            if (pSummary->startFromMF == false && pSummary->startFromEF== false && pSummary->startFromBetaRefit == false)
            {
	      const xAOD::TrackParticle* primaryTP = pTrackParticle;
	      if (pSummary->pRefittedTrack!=NULL) primaryTP = pSummary->pRefittedTrack; 
	      
                if (1==saveStau)
                {   
                    msg(MSG::DEBUG) << "Candidate is ID STAU"<<endreq;
		    pCBMuon = new Analysis::Muon(MuonParameters::MuGirlLowBeta,primaryTP,pSegmentContainer,newSegments);
                    const xAOD::TrackParticle* pTrack = pSummary->pRefittedTrack;
                    if (pTrack ==NULL)
                       pTrack=pSummary->pTrackParticle;
                    if (pTrack != NULL)
                    {
                       
                       
                       const Trk::Perigee pMeasuredPerigee = pTrack->perigeeParameters();
                       //if (pMeasuredPerigee !=NULL)
                       //{ 
                          double px = pMeasuredPerigee.parameters()[Trk::px];
                          double py = pMeasuredPerigee.parameters()[Trk::py];
                          double pz = pMeasuredPerigee.parameters()[Trk::pz];
                          double Mom = sqrt(px*px +py*py+pz*pz);
                          msg(MSG::DEBUG) << " Filling mass " << Mom << " beta " << b << endreq;
                          if(b<=0. || b>=1.) Mass        = 105.7;
                          else   Mass = Mom *sqrt(1-b*b)/b;
                       //}
                       //else Mass = 105.7; 
                    }
                    pCBMuon->setM(Mass);
                }
                else
                {
                    msg(MSG::DEBUG) << " Candidate is ID MUON"<<endreq;
                    pCBMuon = new Analysis::Muon(MuonParameters::MuGirl,primaryTP,pSegmentContainer,newSegments); 
                    pCBMuon->setM(Mass);
                }
                pCBMuon->set_inDetTrackParticle(inDetParticleContainer,pTrackParticle); 
                const xAOD::TrackParticle* pTrack = pSummary->pRefittedTrack;
                if (pTrack !=NULL)
                    pCBMuon->set_innerExtrapolatedTrackParticle(inExtrpParticleContainer,pTrack);

            }//end of if (pSummary->startFromMF == false && pSummary->startFromEF== false)
            else
            {
                const xAOD::TrackParticle* pTrack = pSummary->pRefittedTrack;
                if (pTrack !=NULL)
                {
                    msg(MSG::DEBUG) << " Candidate is MF WITH  REFIT"<<endreq;
                    pCBMuon = new Analysis::Muon(MuonParameters::MuGirlLowBeta,pTrack,true);
                    pCBMuon->set_muonExtrapolatedTrackParticleContainer(inExtrpParticleContainer);
 
                    if (1==saveStau)
                    {   
                       double px = pCBMuon->px();
                       double py = pCBMuon->py();
                       double pz = pCBMuon->pz();
                       double Mom = sqrt(px*px +py*py+pz*pz);
                       msg(MSG::DEBUG) << " Filling mass: Mom "<<Mom<<" beta "<<b<< endreq;
              
                       if (b>0.2 && b<1.) 
                         Mass= Mom *sqrt(1-b*b)/b;
                    }
                }

                else 
                {
                    msg(MSG::DEBUG) << " Candidate is MF WITHOUT  REFIT"<<endreq;
                    pCBMuon = new Analysis::Muon(MuonParameters::MuGirlLowBeta);
                    const MuonFeature* muonFeature = pSummary->pMuonFeature;
                    const TrigMuonEFTrack* muonEFTrack = pSummary->pMuonEFTrack;
                    const xAOD::TrackParticle* muonBetaRefitTrack = pSummary->pLowBetaTrack;
                    const xAOD::TrackParticle* muonBetaRefitExtrpTrack = pSummary->pLowBetaExtrpTrack;

                    double mf_cot = 0.0;
                    double mf_px = 0.0;
                    double mf_py = 0.0;
                    double mf_pz = 0.0;

                    if (muonFeature != NULL)
                    { 
                       mf_cot  = 1/tan(2*atan(exp (-muonFeature->eta())));
                       mf_px   = 1000*fabs(muonFeature->pt())*cos(muonFeature->phi());
                       mf_py   = 1000*fabs(muonFeature->pt())*sin(muonFeature->phi());
                       mf_pz   = 1000*fabs(muonFeature->pt())*mf_cot;
                    } 
                    if (muonEFTrack != NULL)
                    { 
                       mf_cot  = muonEFTrack->cotTh();
                       mf_px   = fabs(muonEFTrack->pt())*cos(muonEFTrack->phi());
                       mf_py   = fabs(muonEFTrack->pt())*sin(muonEFTrack->phi());
                       mf_pz   = fabs(muonEFTrack->pt())*mf_cot;
                    } 
                    if (muonBetaRefitExtrpTrack != NULL)
                    { 
                       mf_cot  = 1/tan(2*atan(exp (-muonBetaRefitExtrpTrack->eta()))); //muonBetaRefitExtrpTrack->cotTh();
                       mf_px   = fabs(muonBetaRefitExtrpTrack->pt())*cos(muonBetaRefitExtrpTrack->phi());
                       mf_py   = fabs(muonBetaRefitExtrpTrack->pt())*sin(muonBetaRefitExtrpTrack->phi());
                       mf_pz   = fabs(muonBetaRefitExtrpTrack->pt())*mf_cot;
                       pCBMuon->set_muonExtrapolatedTrackParticle(inExtrpParticleContainer,muonBetaRefitExtrpTrack);
                    } 
                    else if (muonBetaRefitTrack != NULL)
                    { 
                       mf_cot  = 1/tan(2*atan(exp (-muonBetaRefitTrack->eta()))); //muonBetaRefitTrack->cotTh();
                       mf_px   = fabs(muonBetaRefitTrack->pt())*cos(muonBetaRefitTrack->phi());
                       mf_py   = fabs(muonBetaRefitTrack->pt())*sin(muonBetaRefitTrack->phi());
                       mf_pz   = fabs(muonBetaRefitTrack->pt())*mf_cot;
                       pCBMuon->set_muonSpectrometerTrackParticle(inExtrpParticleContainer,muonBetaRefitTrack);
                    } 
                    
                    double   mf_mom = sqrt(mf_px*mf_px +mf_py*mf_py+mf_pz*mf_pz);
                    if (1==saveStau && (b>0.2 &&b<1.))
                    {
                       double E = mf_mom/b;
                       //CLHEP::Hep3Vector v(mf_px,mf_py,mf_pz); 
                       CLHEP::HepLorentzVector v(mf_px,mf_py,mf_pz,E); 
                       pCBMuon->set4Mom(v);
                    }
                }
                pCBMuon->setM(Mass);
                pCBMuon->set_muonSegments(pSegmentContainer,newSegments);
            }
            pCBMuon->set_numberOfAssociatedEtaDigits(nMDT, nEtaCSC, nEtaRPC, nEtaTGC);
            pCBMuon->set_numberOfAssociatedPhiDigits(0, nPhiCSC, nPhiRPC, nPhiTGC);
            pCBMuon->set_annBarrel(nnBarrel);
            pCBMuon->set_annEndCap(nnEndCap);
            pCBMuon->set_innAngle(innAng);
            pCBMuon->set_midAngle(midAng);
            pCBMuon->set_parameter(MuonParameters::beta,b);
            pCBMuon->set_isMuonLikelihood(pSummary->lhr);
 

	    if (m_doMSRefit)
	      {        
		const xAOD::TrackParticle* pMSRefittedTrack = pSummary->pMSRefittedTrack;
		if (pMSRefittedTrack !=NULL)
		  {
		    pCBMuon->set_muonExtrapolatedTrackParticle(inExtrpParticleContainer,pMSRefittedTrack);
		    //pCBMuon->set_muonExtrapolatedTrackParticle(msParticleContainer,pMSRefittedTrack);
		    //pCBMuon->set_muonExtrapolatedTrackParticle(pMSRefittedTrack);
		  }
	      }


            if (pTrackParticle !=NULL)
            {
               const Trk::Perigee pMeasuredPerigee = pTrackParticle->perigeeParameters();
               double  Etamu    = asinh(1./tan(pMeasuredPerigee.parameters()[Trk::theta]));
               double  Etaphi    = asinh(1./tan(pMeasuredPerigee.parameters()[Trk::phi]));
               double  Momentum = fabs(1./pMeasuredPerigee.parameters()[Trk::qOverP]);

               const xAOD::TrackParticle* pRefittedTrackParticle = pSummary->pRefittedTrack;
               if (pRefittedTrackParticle !=NULL)
               {
                 const Trk::Track* refitted_track = *(pRefittedTrackParticle->track());
                 const CaloEnergy* caloEnergy     = m_trackQuery->caloEnergy(*refitted_track);
                 // m_helper->dressMuon(pCBMuon, caloEnergy);
                 sc = m_muonIsolationTool->dressing( pCBMuon, caloEnergy );

                 if ( sc.isFailure() )
                   {
                       msg(MSG::WARNING) << "Could not dress muon " << endreq;
                   }
               }
               else
               {
               
                 CaloEnergy* caloe = m_pCaloEnergyTool->energyLoss(Momentum,Etamu,Etaphi);
                 double paramEnergy =  caloe->deltaE();
                 delete caloe;
                 //double paramEnergy = (m_pCaloEnergyTool->energyLoss(Momentum,Etamu,Etaphi))->deltaE();
                 double paramEnergyErr = 0.20 * paramEnergy ;

                 Trk::EnergyLoss aEnergyLoss (paramEnergy, paramEnergyErr);

                 if (m_muonIsolationTool)
                 {
                   sc = m_muonIsolationTool->dressing( pCBMuon, &aEnergyLoss  );

                   if ( sc.isFailure() )
                   {
                       msg(MSG::WARNING) << "Could not dress muon " << endreq;
                   }
                 }
                 else
                   msg(MSG::WARNING) << " IsolationTool not available, energy is meaningless " << endreq;
               }
            }

            //pCBMuon->set_innerExtrapolatedTrackParticleContainer(inExtrpParticleContainer);
            pMuonContainer->push_back(pCBMuon);
            msg(MSG::DEBUG) << " Muon in MuonContainer has mass "<< (pCBMuon)->m()<<endreq;
*/
            if (m_doNTuple)
            {
	        const xAOD::EventInfo* pEventInfo = NULL;
                if (evtStore()->retrieve(pEventInfo).isFailure() || pEventInfo == NULL)
                    msg(MSG::WARNING) << "Cannot retrieve EventInfo" << endreq;
                else
                {
                    if (m_pNTuple->fillEventInfo(pEventInfo->runNumber(), pEventInfo->eventNumber()).isFailure())
                        msg(MSG::WARNING) << "Cannot fill event info" << endreq;
                }
                if (m_pNTuple->fill(pSummary, m_doTruth, m_pTruthTool).isFailure())
                    msg(MSG::WARNING) << "Cannot fill NTuple" << endreq;

                if (m_pRawNTuple->writeRecord().isFailure())
                    msg(MSG::WARNING) << "Cannot write NTuple" << endreq;
                msg(MSG::DEBUG) << " Ntuple entry recorded "<< endreq;
            }
        }

    }
    if (!onlyNTupleFill)
    {
        sc = evtStore()->record(pMuonContainer, m_muGirlCombinedMuonLocation);
        if (sc.isFailure())
        {
            ATH_MSG_WARNING("Could not save muons!!! ");
            return sc;
        }
    }

    //if (m_log.level() <= MSG::DEBUG)
    if (msgLvl(MSG::DEBUG)) msg() << "Muons saved " <<endreq;

    //if (m_log.level() <= MSG::DEBUG)
    if (msgLvl(MSG::DEBUG)) msg() << "fillContainer " << name() << " ended" << endreq;

    return StatusCode::SUCCESS;
}

enum FeatureType
{
    RECO_MUON, RECO_STAU, TRIG_MUON_FEATURE, TRIG_EVENT_FILTER
};

inline FeatureType featureType(const CandidateSummary* pSummary)
{
    if (pSummary->startFromMF) return TRIG_MUON_FEATURE;
    if (pSummary->startFromEF) return TRIG_EVENT_FILTER;	
    if (pSummary->saveStau)    return RECO_STAU;
    return RECO_MUON;
}

static std::string summaryToString(const CandidateSummary* pSummary,
                                   ToolHandle<IPerformanceTruthTool>& truthTool,
                                   bool doTruth)
{
    std::ostringstream oss;
    
    std::string sFT;
    FeatureType iFT = featureType(pSummary);	
    if (iFT ==  RECO_STAU) sFT = "RECO_STAU";
    else if (iFT == TRIG_MUON_FEATURE) sFT = "TRIG_MUON_FEATURE";
    else if (iFT == TRIG_EVENT_FILTER) sFT = "TRIG_EVENT_FILTER";
    else if (iFT == RECO_MUON) sFT = "RECO_MUON";

    if (pSummary->pTrackParticle != NULL)
    {
        const Trk::Perigee prg = pSummary->pTrackParticle->perigeeParameters();
        //if (prg != NULL)
        //{
            double phi = prg.parameters()[Trk::phi];
            double eta = asinh(1.0 / tan(prg.parameters()[Trk::theta]));
            oss << eta << " " << phi << " " << prg.pT() << " " <<  sFT << " "  ;
        //}
        //else
        //    oss << "-100.0 -100.0 -100.0 ";
    }
    else if(  pSummary->pMuonEFTrack != NULL )
    {
        double phi = pSummary->pMuonEFTrack->phi();
        double eta = pSummary->pMuonEFTrack->eta();
        double pt = 1./ pSummary->pMuonEFTrack->iPt();
        oss << eta << " " << phi << " " << pt << " " << sFT << " 2 " ;
    }
    else if (pSummary->pMuonFeature != NULL )
    {
        double phi = pSummary->pMuonFeature->phi();
        double eta = pSummary->pMuonFeature->eta();
        double pt =  pSummary->pMuonFeature->pt();
        oss << eta << " " << phi << " " << pt << " " << sFT << " 3 " ;
    }
    else
        oss << "-100.0 -100.0 -100.0 ";

    const xAOD::TrackParticle* refit = pSummary->pRefittedTrack;
    if (refit != NULL)
    {
        const Trk::Perigee prg  = refit->perigeeParameters();
        //if (prg != NULL)
        //{
            double phi = prg.parameters()[Trk::phi];
            double eta = asinh(1.0 / tan(prg.parameters()[Trk::theta]));
            oss << eta << " " << phi << " " << prg.pT() << " ";
        //}
        //else
        //    oss << "-100.0 -100.0 -100.0 ";

        //const Trk::FitQuality* fq = refit->fitQuality();
        //if (fq != NULL)
            oss << refit->chiSquared() << " " << refit->numberDoF() << " ";
        //else
        //    oss << "-100.0 -100 ";
    }
    else
        oss << "-100.0 -100.0 -100.0 -100.0 -100 ";

    oss << pSummary->nnBarrel << " " << pSummary->nnEndCap << " ";
    if (pSummary->innAngle != 999.0) oss << pSummary->innAngle << " "; else oss << "-100.0 ";
    if (pSummary->midAngle != 999.0) oss << pSummary->midAngle << " "; else oss << "-100.0 ";
    if (doTruth)  
    {
        IPerformanceTruthTool::TruthTrack truthTrack;
        if (pSummary->pTrackParticle != NULL)
        { 
            if (truthTool->getTruthTrack(pSummary->pTrackParticle, truthTrack,const_cast<CandidateSummary* >(pSummary)).isSuccess())
                oss << truthTrack.PDG_ID << " ";
            else
                oss << "-100 ";
        }
        else if ( pSummary->pMuonEFTrack != NULL)
        {
            if (truthTool->getTruthTrack(pSummary->pMuonEFTrack, truthTrack,const_cast<CandidateSummary* >(pSummary)).isSuccess())
                oss << truthTrack.PDG_ID << " ";
            else
                oss << "-100 ";

        }
        else 
        {
           oss << "-100 ";
        } 
    }
  
    else
        oss << "-100 ";

    return(oss.str());
}


void MuGirlParticleCreatorTool::removeRedundantCandidates(CandidateSummaryList* SummaryList,
        std::vector<bool>& IncludeList)
{
    if (msgLvl(MSG::DEBUG)) msg() << "removeRedundantCandidates " << name() << endreq;

    const double MIN_OVERLAP = 0.5;
    typedef std::vector<size_t> CandList;
    typedef std::map<Identifier, CandList > HitMap;

    HitMap hits;
    size_t nCand = SummaryList->size();
    IncludeList.resize(nCand, true);
    std::vector<std::vector<size_t> > hitCount(nCand);
    size_t totalHits = 0;
    for (size_t iCand = 0; iCand < nCand; iCand++)
    {
        hitCount[iCand].resize(nCand, 0);
        const CandidateSummary* pSummary = (*SummaryList)[iCand];
        FeatureType iFT = featureType(pSummary);
//        if (pSummary->saveStau || pSummary->startFromMF || pSummary->startFromEF)
//            continue;
        if (pSummary->startFromBetaRefit) continue;

	ATH_MSG_DEBUG("MuGirlParticleCreatorTool removeRedundant "<<pSummary->muonSegmentList.size());

        for (size_t iSeg = 0; iSeg < pSummary->muonSegmentList.size(); iSeg++)
        {
            const Muon::MuonSegment* pSeg = pSummary->muonSegmentList[iSeg];
            for (size_t iHit = 0; iHit < pSeg->numberOfContainedROTs(); iHit++)
            {
                totalHits++;
                const Trk::PrepRawData* prd = pSeg->rioOnTrack(iHit)->prepRawData();
                Identifier id = prd->identify();
                HitMap::iterator itHit = hits.find(id);
                if (itHit == hits.end())
                {
                    std::pair<HitMap::iterator, bool> p = hits.insert(HitMap::value_type(id, CandList()));
                    itHit = p.first;
                }
                CandList& cands = itHit->second;
                cands.push_back(iCand);
                for (size_t candIdx = 0; candIdx < cands.size(); candIdx++)
                {
                    size_t jCand = cands[candIdx];
                    if (featureType((*SummaryList)[jCand]) == iFT)
                    {
                        hitCount[iCand][jCand]++;
                        if (iCand != jCand)
                            hitCount[jCand][iCand]++;
                    }
                }
            }
        }
    }
    
    for (size_t iCand = 0; iCand < nCand; iCand++)
    {
        if (!IncludeList[iCand])
            continue;
        int nHits = hitCount[iCand][iCand];
        if (nHits == 0)
            continue;
        for (size_t jCand = 0; jCand < nCand; jCand++)
        {
            if (jCand == iCand || !IncludeList[jCand])
                continue;
            const CandidateSummary* iSummary = (*SummaryList)[iCand];
            const CandidateSummary* jSummary = (*SummaryList)[jCand];
            double overlap = (double)hitCount[iCand][jCand] / (double)nHits;
            if (overlap <= MIN_OVERLAP)
            {
                // there is no enough overlap with iCand, so we don't need to do anything with this jCand
                continue;
            } 

            size_t removed = nCand; // default value so we don't remove something by mistake 
            const xAOD::TrackParticle* iTP = iSummary->pRefittedTrack;
            const xAOD::TrackParticle* jTP = jSummary->pRefittedTrack;
            double iAnn = (iSummary->nnBarrel > iSummary->nnEndCap) ? iSummary->nnBarrel : iSummary->nnEndCap; 
            double jAnn = (jSummary->nnBarrel > jSummary->nnEndCap) ? jSummary->nnBarrel : jSummary->nnEndCap;
            //const Trk::FitQuality* iFQ = (iTP != NULL) ? iTP->fitQuality() : NULL;  
            //const Trk::FitQuality* jFQ = (jTP != NULL) ? jTP->fitQuality() : NULL; 
            //int diffHits = (iFQ != NULL && jFQ != NULL ) ? fabs(iFQ->doubleNumberDoF() - jFQ->doubleNumberDoF()) : -1;   
            int diffHits = (iTP!=NULL && jTP!=NULL)? fabs(iTP->numberDoF() - jTP->numberDoF()) : 0;
            
	    //try discriminating based on whether there are standalone fits
            //const Rec::TrackParticle* iMSTP = iSummary->pMSRefittedTrack;
            //const Rec::TrackParticle* jMSTP = jSummary->pMSRefittedTrack;


	    removed = iCand;

            if (iTP == NULL && jTP != NULL )
            {
                removed = iCand;
            }
            else if (jTP == NULL && iTP != NULL )
            {
                removed = jCand; 
            }
	    //insert here the standalone fit test
	    //else if (iMSTP == NULL && jMSTP != NULL)
	    //  {
	    //	removed = iCand;
	    //	if (msgLvl(MSG::DEBUG)) msg() << "ms req led to removal" << endreq;
	    //  }
	    //else if (jMSTP == NULL && iMSTP != NULL)
	    //  {
	    //	removed = jCand;
	    //	if (msgLvl(MSG::DEBUG)) msg() << "ms req led to removal" << endreq;
	    //  }

            else if (jTP == NULL && iTP == NULL)
            {
                removed = (iAnn < jAnn) ? iCand : jCand;
            }  
                        
            else if (iTP->numberDoF()==0 && jTP->numberDoF() != 0)
            {
                removed = iCand;
            }
            else if (jTP->numberDoF()==0 && iTP->numberDoF() != 0)
            {
                removed = jCand;
            }
            else if (jTP->numberDoF()==0 && iTP->numberDoF() == 0)
            {
                removed = (iAnn < jAnn) ? iCand : jCand;
            } 

            else if (iTP->chiSquared() / iTP->numberDoF() > jTP->chiSquared() / jTP->numberDoF() )
            {
                  // i's chi2/ dof is larger 
                  if (diffHits <= 5)
                  {      // diff hits is smaller than 5, we want to remove the one with the largest chi2/dof
                         removed = iCand;
	          }
                  // diff hits is larger than 5, we want
                  else if (iTP->numberDoF() - jTP->numberDoF() > 0 && 
	                  (iTP->chiSquared() - jTP->chiSquared())/(iTP->numberDoF() - jTP->numberDoF()) < 1.5 ) 
                  {
                          removed = jCand;
                  } 
                  else
                  {
                          removed = iCand;
                  }
			
            }
	    //else if (iMSTP == NULL && jMSTP != NULL)
	    //  {
	    //	removed = iCand;
	    //	if (msgLvl(MSG::DEBUG)) msg() << "ms req led to removal" << endreq;
	    //  }
	    //else if (jMSTP == NULL && iMSTP != NULL)
	    //  {
	    //	removed = jCand;
	    //	if (msgLvl(MSG::DEBUG)) msg() << "ms req led to removal" << endreq;
	    //  }
            else  //j's chi2/dof is larger
            {
                  if (diffHits <= 5)
                  {
                          removed = jCand;
                  }
                  else if (jTP->numberDoF() - iTP->numberDoF() > 0 && 
                          (jTP->chiSquared() - iTP->chiSquared())/(jTP->numberDoF() - iTP->numberDoF()) < 1.5 ) 
                  {
                          removed = iCand;
                  } 
                  else
                  {
                          removed = jCand; 
                  }  
             }

             IncludeList[removed] = false;

             if (m_doNTuple)
             {
                 fprintf(m_fOverlap, "%lu %lu %lu %d %s%s\n",
                         (unsigned long) hitCount[iCand][iCand],
                         (unsigned long) hitCount[jCand][jCand],
                         (unsigned long) hitCount[iCand][jCand], 
                         (removed == iCand ? 1 : 2),
                         summaryToString(iSummary, m_pTruthTool, m_doTruth).c_str(),
                         summaryToString(jSummary, m_pTruthTool, m_doTruth).c_str());
                 fflush(m_fOverlap);
             }
            
        } // jCandidate
    } //iCandidate

    if (msgLvl(MSG::DEBUG)) msg() << "removeRedundantCandidates ended" << endreq;
}

}

