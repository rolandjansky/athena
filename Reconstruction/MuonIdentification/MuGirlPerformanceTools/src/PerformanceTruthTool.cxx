/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PerformanceTruthTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "MuGirlPerformanceTools/PerformanceTruthTool.h"
#include "MuGirlInterfaces/CandidateSummary.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "MuGirlCandidate/Intersection.h"
#include "GeneratorObjects/McEventCollection.h"
#include "GenInterfaces/ITruthSelector.h"
#include "iPatInterfaces/ITruthParameters.h"
#include "iPatTrackParameters/PerigeeParameters.h"

#include "iPatTruthTrajectory/TruthParameters.h"

#include <limits>


using MuGirlNS::PerformanceTruthTool;
//================ Constructor =================================================

PerformanceTruthTool::PerformanceTruthTool(const std::string& t,
        const std::string& n,
        const IInterface*  p )
        :
        AthAlgTool(t,n,p),
        m_truthParameters("TruthParameters/TruthParameters")
{
    declareInterface<IPerformanceTruthTool>(this);

    //  template for property decalration
    declareProperty("TrackRecordCollection", m_sTrackRecordCollection = "MuonEntryLayerFilter");
    declareProperty("TruthEventCollection",  m_sMcEventCollection     = "TruthEvent");
    declareProperty("TruthParameters",  m_truthParameters);
    declareProperty("DoOnlyMuonEntry",  m_doOnlyMuonEntry = false);
}

//================ Destructor =================================================

PerformanceTruthTool::~PerformanceTruthTool()
{}


//================ Initialisation =================================================

StatusCode PerformanceTruthTool::initialize()
{
    msg(MSG::INFO) << "Initializing " << name() << " - package version " << PACKAGE_VERSION << endreq;
    StatusCode sc = AlgTool::initialize();

    if (m_truthParameters.retrieve().isFailure())
    {
        msg(MSG::INFO) << "Failed to retrieve tool m_truthParameters" <<endreq;
    }
    else
    {
        msg(MSG::INFO) << "Retrieved tool m_truthParameters"  << endreq;
    }

    if (sc.isFailure()) return sc;
    return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode PerformanceTruthTool::finalize()
{
    msg(MSG::INFO) << "Finalizing " << name() << " - package version " << PACKAGE_VERSION << endreq;
    StatusCode sc = AlgTool::finalize();
    return sc;
}

//============================================================================================

StatusCode PerformanceTruthTool::printParticle(int icode)
{

    const TrackRecordCollection* pRecordCollection = NULL;
    StatusCode sc =evtStore()->retrieve(pRecordCollection, "MuonEntryLayerFilter") ;
    if( sc.isFailure() || pRecordCollection == NULL)
    {
        msg(MSG::WARNING) << "Cannot retrieve TrackRecordCollection" << endreq;
        return sc;
    }


       for (TrackRecordConstIterator itTrkRec = pRecordCollection->begin();
            itTrkRec != pRecordCollection->end();
            itTrkRec++)
       {
           TrackRecord pTrkRec = *itTrkRec;

           if (fabs(pTrkRec.GetPDGCode()) == icode )
              std::cout<<" printParticle: found particle at the entrance of MS: PDGID "<<pTrkRec.GetPDGCode()<<" eta "<<pTrkRec.GetMomentum().eta()<<std::endl;
       }


       sc = evtStore()->retrieve(m_pMcEventCollection, m_sMcEventCollection);
    if (sc.isFailure() || m_pMcEventCollection == NULL)
    {
        msg(MSG::WARNING) << "Cannot retrieve McEventCollection" << endreq;
        return StatusCode::FAILURE;
    }
    for (McEventCollection::const_iterator itMc = m_pMcEventCollection->begin();
            itMc != m_pMcEventCollection->end();
            itMc++)
    {
        const HepMC::GenEvent* pGenEvent = *itMc;
        if (pGenEvent==NULL) continue;
        for (HepMC::GenEvent::particle_const_iterator itPart = pGenEvent->particles_begin();
                itPart != pGenEvent->particles_end();
                itPart++)
        {
            const HepMC::GenParticle* pPart = *itPart;

            if ( fabs(pPart->pdg_id()) == icode && pPart->status()==1)
              std::cout<<" printParticle:.found  particle in GenEvent: PDGID "<<pPart->pdg_id()<<" eta "<<pPart->momentum().eta()<<std::endl;
        }
    }
    return sc;
}










//============================================================================================

StatusCode PerformanceTruthTool::getTruthMuons(TruthMuonList& truthMuonList)
{
    if (msgLvl(MSG::DEBUG)) msg() << "in PerformanceTruthTool::getTruthMuons" << endreq;
    truthMuonList.clear();

    int nTruthMu = 0;
    const TrackRecordCollection* pRecordCollection = NULL;
    //StatusCode sc = evtStore()->retrieve(pRecordCollection, m_sTrackRecordCollection);
    StatusCode sc = evtStore()->retrieve(pRecordCollection, "MuonEntryLayerFilter");
    if (sc.isFailure() || pRecordCollection == NULL)
    {
        msg(MSG::WARNING) << "Cannot retrieve TrackRecordCollection" << endreq;
        return StatusCode::FAILURE;
    }
    for (TrackRecordConstIterator itTrkRec = pRecordCollection->begin();
            itTrkRec != pRecordCollection->end();
            itTrkRec++)
    {
        TrackRecord pTrkRec = *itTrkRec;

        if (fabs(pTrkRec.GetPDGCode()) == 13 || fabs(pTrkRec.GetPDGCode()) > 1000000 )
        {
            TruthMuon* pTruthMuon = new TruthMuon(1,
                                                  pTrkRec.GetMomentum().phi(),
                                                  pTrkRec.GetMomentum().eta(),
                                                  pTrkRec.GetMomentum().perp(),
                                                  pTrkRec.GetPDGCode(),
                                                  999);
            truthMuonList.push_back(pTruthMuon);
            nTruthMu++;
            if (msgLvl(MSG::DEBUG)) msg() << "nTruthMu = " << nTruthMu
                << " pt = "  << pTruthMuon->perp
                << " eta = " << pTruthMuon->eta
                << " phi = " << pTruthMuon->phi
                << " pdg = " << pTruthMuon->pdgCode << endreq;
        }
    }

    m_pMcEventCollection = NULL;
    sc = evtStore()->retrieve(m_pMcEventCollection, m_sMcEventCollection);
    if (sc.isFailure() || m_pMcEventCollection == NULL)
    {
        msg(MSG::WARNING) << "Cannot retrieve McEventCollection" << endreq;
        return StatusCode::FAILURE;
    }
    for (McEventCollection::const_iterator itMc = m_pMcEventCollection->begin();
            itMc != m_pMcEventCollection->end();
            itMc++)
    {
        const HepMC::GenEvent* pGenEvent = *itMc;
        if (pGenEvent==NULL) continue;
        for (HepMC::GenEvent::particle_const_iterator itPart = pGenEvent->particles_begin();
                itPart != pGenEvent->particles_end();
                itPart++)
        {
            const HepMC::GenParticle* pPart = *itPart;

            if (fabs(pPart->pdg_id()) == 13 || fabs(pPart->pdg_id()) > 1000000)
            {
                int parent_pdg = 0;
                if (pPart->production_vertex() != NULL)
                {
                    for (HepMC::GenVertex::particle_iterator itVxPart = pPart->production_vertex()->particles_begin(HepMC::parents);
                            itVxPart != pPart->production_vertex()->particles_end(HepMC::parents);
                            itVxPart++)
                    {
                        parent_pdg = (*itVxPart)->pdg_id();
                    }
                }
                TruthMuon* pTruthMuon = new TruthMuon(0,
                                                      pPart->momentum().phi(),
                                                      pPart->momentum().eta(),
                                                      pPart->momentum().perp(),
                                                      pPart->pdg_id(),
                                                      parent_pdg);
                truthMuonList.push_back(pTruthMuon);
                nTruthMu++;
                if (msgLvl(MSG::DEBUG)) msg() << "IP: nTruthMu = " << nTruthMu
                    << " pt = "  << pPart->momentum().perp()
                    << " eta = " << pPart->momentum().eta()
                    << " phi = " << pPart->momentum().phi()
                    << " pdg = " << pPart->pdg_id()
                    << " parent pdg = " << parent_pdg << endreq;
            }
        }
    }
    return StatusCode::SUCCESS;
}

StatusCode PerformanceTruthTool::getTruthTrack(const xAOD::TrackParticle* pParticle, TruthTrack& truthTrack, CandidateSummary* summary)
{
    if (msgLvl(MSG::DEBUG)) msg() << "in PerformanceTruthTool::getTruthTrack" << endreq;

    bool found_match = false;

    double cand_momentum = 0.;
    double abs_qOverP = fabs(summary->qOverP);
    // conditions to avoid a float overflow
    if (summary->qOverP != 0. &&                                                //remove infinity
        (abs_qOverP>1.0 || abs_qOverP*std::numeric_limits<double>::max()>=1.0)  //remove float overflow
       ) cand_momentum = 1.0/fabs(summary->qOverP);

    const TrackRecordCollection* pRecordCollection = NULL;
    StatusCode sc =evtStore()->retrieve(pRecordCollection, "MuonEntryLayerFilter") ;
    if( sc.isFailure() || pRecordCollection == NULL)
    {
        msg(MSG::WARNING) << "Cannot retrieve TrackRecordCollection" << endreq;
        goto mcEvent;
    }

    if (msgLvl(MSG::DEBUG)) msg() << "Found "<<summary->innerIsectPositions.size()<<" inner intersections" << endreq;
    for (unsigned int i=0;i<summary->innerIsectPositions.size();i++)
    {
       double isect_eta = (summary->innerIsectPositions[i]).eta();
       double isect_theta = 2*atan(exp(-isect_eta));
       double isect_phi = (summary->innerIsectPositions[i]).phi();
       double isect_pt = cand_momentum * sin(isect_theta);

       if (msgLvl(MSG::DEBUG)) msg() << "Found InnerDist intersection: p "<<cand_momentum<<" pt " 
                                      <<isect_pt<<" phi "<<isect_phi<<" eta "<<isect_eta << endreq;

       for (TrackRecordConstIterator itTrkRec = pRecordCollection->begin();
            itTrkRec != pRecordCollection->end();
            itTrkRec++)
       {
           TrackRecord pTrkRec = *itTrkRec;

           if (fabs(pTrkRec.GetPDGCode())!=13 && fabs(pTrkRec.GetPDGCode())<1000000) continue;

           double      charge   = pTrkRec.GetPDGCode() < 0 ? 1. : -1.;
 
           double dpt_pt = (pTrkRec.GetMomentum().perp() -isect_pt) / pTrkRec.GetMomentum().perp();
           double dp_p = (pTrkRec.GetMomentum().mag() -cand_momentum) / pTrkRec.GetMomentum().mag();
           double dphi = pTrkRec.GetMomentum().phi() - isect_phi;
           double deta = pTrkRec.GetMomentum().eta() - isect_eta;
           if (dphi < -M_PI) dphi += 2.0 * M_PI;

           if (msgLvl(MSG::DEBUG)) msg() <<" Processing a muon in the TrackRecordCollection: p "<<pTrkRec.GetMomentum().mag()
                                         <<" pt " <<pTrkRec.GetMomentum().perp()<<" phi "<<pTrkRec.GetMomentum().phi()
                                         <<" eta "<<pTrkRec.GetMomentum().eta() << endreq;
           if (msgLvl(MSG::DEBUG)) msg() << "dp_p "<<dp_p<<" dpt_pt " 
                                      <<dpt_pt<<" dphi "<<dphi<<" deta "<<deta << endreq;
        //const HepGeom::Point3D<double> vertex(0.,0.,0.);
        // const PerigeeParameters *pPartPerigeeParameters=m_truthParameters->perigeeParameters(*pPart,vertex);
        // if (charge == pPerigee->charge() &&

           if (fabs(deta) < 0.06 &&
               fabs(dphi) < 0.06 ) //&&
               // fabs(dp_p) < 0.2)  remove the check on the muon pt for the time being
           {
               double x = pTrkRec.GetPosition().x();
               double y = pTrkRec.GetPosition().y();
               double z = pTrkRec.GetPosition().z();
               double phi_pos = std::atan2(y,x);
               if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TrackRecordCollection truth Association Found" << endreq;
               truthTrack.PDG_ID=pTrkRec.GetPDGCode();
               truthTrack.PDG_Mother=999;
               truthTrack.Phi = pTrkRec.GetMomentum().phi();
               truthTrack.Eta = pTrkRec.GetMomentum().eta();
               truthTrack.Theta =  2*atan(exp(-(pTrkRec.GetMomentum().eta())));
               truthTrack.Pt = pTrkRec.GetMomentum().perp();
               truthTrack.d0 = std::sqrt( x*x + y*y );
               truthTrack.z0 = z;
               truthTrack.Charge = (int)charge;
               truthTrack.QOverP = charge/pTrkRec.GetMomentum().perp();
               truthTrack.DPhi = phi_pos - pTrkRec.GetMomentum().phi(); // dphi;
               truthTrack.DEta = deta;
               truthTrack.DPt_Pt = dpt_pt;
               found_match = true;
               if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TruthTrack parameters   :"
                        << " PDG_ID="   << truthTrack.PDG_ID
                        << " pT="       << truthTrack.Pt
                        << " eta="      << truthTrack.Eta
                        << " phi="      << truthTrack.Phi
                        << " cotTheta=" << truthTrack.Theta
                        << " charge="   << truthTrack.Charge
                        << " z0="       << truthTrack.z0
                        << " d0="       << truthTrack.d0 << endreq;

               goto mcEvent;
            }
        }
    }
    mcEvent:
    if ( m_doOnlyMuonEntry ) {
      if (found_match == false) return StatusCode::RECOVERABLE;
      else                      return StatusCode::SUCCESS;
    }

    if (found_match == false)  
       if (msgLvl(MSG::DEBUG)) 
           msg(MSG::DEBUG) << "Could not find a match in the TrackRecordCollection, moving to McEvent" << endreq;

    const Trk::Perigee pPerigee = pParticle->perigeeParameters();
    double perigee_pt   = pPerigee.pT();
    double perigee_phi  = pPerigee.parameters()[Trk::phi];
    double perigee_eta  = pPerigee.eta();
    if (msgLvl(MSG::DEBUG)) msg() << "perigeeParameters: pt " 
                                      <<perigee_pt<<" phi "<<perigee_phi<<" eta "<<perigee_eta << endreq;

    m_pMcEventCollection = NULL;
    sc = evtStore()->retrieve(m_pMcEventCollection, m_sMcEventCollection);
    if( sc.isFailure() || m_pMcEventCollection == NULL)
    {
        msg(MSG::WARNING) << "Cannot retrieve McEventCollection" << endreq;
        if (found_match) 
            return StatusCode::SUCCESS;
        else
            return StatusCode::RECOVERABLE;
    }

    for (McEventCollection::const_iterator itMc = m_pMcEventCollection->begin();
            itMc != m_pMcEventCollection->end();
            itMc++)
    {
        const HepMC::GenEvent* pGenEvent = *itMc;
        if (pGenEvent==NULL) continue;

        for (HepMC::GenEvent::particle_const_iterator itPart = pGenEvent->particles_begin();
                itPart != pGenEvent->particles_end();
                itPart++)
        {
            const HepMC::GenParticle* pPart = *itPart;

            if(fabs(pPart->pdg_id())!=13 && fabs(pPart->pdg_id())<1000000) continue;

            if(pPart->status() == 1&&pPart->momentum().perp()>1000.)
            {
                double dpt_pt = (pPart->momentum().perp() - perigee_pt) / pPart->momentum().perp();
                double dphi = pPart->momentum().phi() - perigee_phi;
                double deta = pPart->momentum().eta() - perigee_eta;
                if (dphi < -M_PI) dphi += 2.0 * M_PI;

                const Amg::Vector3D vertex(0.,0.,0.);
                const PerigeeParameters *pPartPerigeeParameters=m_truthParameters->perigeeParameters(*pPart,vertex);
                if (pPartPerigeeParameters==0) {
                    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Failed to retrieve the Truth parameters, skipping this track!" << endreq;
                    continue;
                }
                double charge = pPartPerigeeParameters->charge();
                if (charge == pPerigee.charge() &&
                        fabs(deta) < 0.006 &&
                        fabs(dphi) < 0.006 &&
                        fabs(dpt_pt) < 0.2) 
                {
                    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "McEventCollection truth Association Found" << endreq;
                    truthTrack.PDG_ID=pPart->pdg_id();
                    truthTrack.PDG_Mother=-100;
                    if (pPart->production_vertex()!=NULL)
                        if (pPart->production_vertex()->particles_in_size()>0)
                            truthTrack.PDG_Mother=(*pPart->production_vertex()->particles_begin(HepMC::parents))->pdg_id();
                    truthTrack.Phi = pPart->momentum().phi();
                    truthTrack.Eta = pPart->momentum().eta();
                    truthTrack.Theta = 1./tan(pPart->momentum().theta());
                    truthTrack.Pt = pPart->momentum().perp();
                    truthTrack.d0=pPartPerigeeParameters->transverseImpact();
                    truthTrack.z0=pPartPerigeeParameters->position().z();
                    truthTrack.Charge = (int)charge;
                    truthTrack.QOverP = charge/pPart->momentum().perp();
                    truthTrack.DPhi = dphi;
                    truthTrack.DEta = deta;
                    truthTrack.DPt_Pt = dpt_pt;

                    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "This track is associated to muon " << endreq;
                    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Truth parameters   :"
                        << " PDG_ID=" << pPart->pdg_id()
                        << " pT=" << pPart->momentum().perp()
                        << " eta=" << pPart->momentum().eta()
                        << " phi=" << pPart->momentum().phi()
                        << " cotTheta=" <<truthTrack.Theta
                        << " charge=" << charge
                        << " z0=" << truthTrack.z0
                        << " d0=" <<truthTrack.d0<<endreq;
                    if (msgLvl(MSG::DEBUG)) msg() << "ID track parameters:"
                        << " pT=" << pPerigee.pT()
                        << " eta=" << pPerigee.eta()
                        << " phi=" << pPerigee.parameters()[Trk::phi]
                        << " cotTheta=" << 1./tan(pPerigee.parameters()[Trk::theta])
                        << " charge=" << pPerigee.charge()
                        << " z0=" << pPerigee.parameters()[Trk::z0]
                        << " d0=" <<pPerigee.parameters()[Trk::d0]<<endreq;
                    
                    return StatusCode::SUCCESS;
                }
            }
        }
    }
    if (found_match) 
       return StatusCode::SUCCESS;
    else
    {
       if (msgLvl(MSG::DEBUG)) 
           msg(MSG::DEBUG) <<"This trackParticle is not associated to truth " << endreq;
       return StatusCode::RECOVERABLE;
    }
}

/////////////////// sofia
StatusCode PerformanceTruthTool::getTruthTrack(const MuonFeature* muonFeature, TruthTrack& truthTrack,CandidateSummary* summary)
{
    if (msgLvl(MSG::DEBUG)) msg() << "in PerformanceTruthTool::getTruthTrack from mFeature" << endreq;


    double mf_pt = fabs(muonFeature->pt())*1000;
    double mf_phi = muonFeature->phi();
    double mf_eta = muonFeature->eta();
    bool found_match = false;
    double cand_momentum = 0.;
    if (summary->qOverP != 0.) cand_momentum = 1./fabs(summary->qOverP);


    const TrackRecordCollection* pRecordCollection = NULL;
    StatusCode sc = evtStore()->retrieve(pRecordCollection, "MuonEntryLayerFilter");
    if (sc.isFailure() || pRecordCollection == NULL)
    {
        msg(MSG::WARNING) << "Cannot retrieve TrackRecordCollection" << endreq;
        goto mcEvent;
    }

 
    for (unsigned int i=0;i<summary->innerIsectPositions.size();i++)
    {
       double isect_eta = (summary->innerIsectPositions[i]).eta();
       double isect_theta = 2*atan(exp(-isect_eta));
       double isect_phi = (summary->innerIsectPositions[i]).phi();
       double isect_pt = cand_momentum * sin(isect_theta);
     
       if (msgLvl(MSG::DEBUG)) msg() << "Found InnerDist intersection: p "<<cand_momentum<<" pt "
                                      <<isect_pt<<" phi "<<isect_phi<<" eta "<<isect_eta << endreq;

       for (TrackRecordConstIterator itTrkRec = pRecordCollection->begin();
            itTrkRec != pRecordCollection->end();
            itTrkRec++)
       {
           TrackRecord pTrkRec = *itTrkRec;
 
           if (fabs(pTrkRec.GetPDGCode())!= 13 && fabs(pTrkRec.GetPDGCode())<1000000) continue;

          double dpt_pt = (pTrkRec.GetMomentum().perp() - isect_pt) / pTrkRec.GetMomentum().perp();
          double dphi = pTrkRec.GetMomentum().phi() - isect_phi;
          double deta = pTrkRec.GetMomentum().eta() - isect_eta;

          if (dphi < -M_PI) dphi += 2.0 * M_PI;

          const Amg::Vector3D vertex(0.,0.,0.);
          // const PerigeeParameters *pPartPerigeeParameters=m_truthParameters->perigeeParameters(*pPart,vertex);
          double charge = 1.;
          if (pTrkRec.GetPDGCode()>0) charge = -1.;
          if (fabs(deta) < 0.006*50 &&
              fabs(dphi) < 0.006*50)
          {
              if (msgLvl(MSG::DEBUG)) msg() << "Truth Association Found" << endreq;
              truthTrack.PDG_ID=pTrkRec.GetPDGCode();
              truthTrack.PDG_Mother=999;
              truthTrack.Phi = pTrkRec.GetMomentum().phi();
              truthTrack.Eta = pTrkRec.GetMomentum().eta();
              truthTrack.Theta = 1./tan(pTrkRec.GetMomentum().theta());
              truthTrack.Pt = pTrkRec.GetMomentum().perp();
              // truthTrack.d0=pPartPerigeeParameters->transverse_impact();
              // truthTrack.z0=pPartPerigeeParameters->position().z();
              // truthTrack.Charge = (int)charge;
              truthTrack.QOverP = charge/pTrkRec.GetMomentum().perp();
              truthTrack.DPhi = dphi;
              truthTrack.DEta = deta;
              truthTrack.DPt_Pt = dpt_pt;
              found_match = true;
              break;
          }
       }
    }

    mcEvent:

    m_pMcEventCollection = NULL;
    sc = evtStore()->retrieve(m_pMcEventCollection, m_sMcEventCollection);
    if( sc.isFailure() || m_pMcEventCollection == NULL)
    {
        if (msgLvl(MSG::DEBUG)) msg(MSG::WARNING) <<"Cannot retrieve McEventCollection" << endreq;
        if (found_match) 
            return StatusCode::SUCCESS;
        else
            return StatusCode::RECOVERABLE;
    }
    
    for (McEventCollection::const_iterator itMc = m_pMcEventCollection->begin();
            itMc != m_pMcEventCollection->end();
            itMc++)
    {
        const HepMC::GenEvent* pGenEvent = *itMc;
        if (pGenEvent==NULL) continue;

        for (HepMC::GenEvent::particle_const_iterator itPart = pGenEvent->particles_begin();
                itPart != pGenEvent->particles_end();
                itPart++)
        {
            const HepMC::GenParticle* pPart = *itPart;
            if (fabs(pPart->pdg_id())>1000000 && fabs(pPart->momentum().eta())>1.0)
                if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"True R-Hadron out of barrel   :"
                      << " PDG_ID=" << pPart->pdg_id()
		      << " Eta=" << pPart->momentum().eta()<<endreq;

            if (fabs(pPart->pdg_id()) !=13 && fabs(pPart->pdg_id())<1000000) continue;

	    if(pPart->status() == 1&&pPart->momentum().perp()>1000.)
	    {
		double dpt_pt = (pPart->momentum().perp() - mf_pt*1000) / pPart->momentum().perp();
		double dphi = pPart->momentum().phi() - mf_phi;
		double deta = pPart->momentum().eta() - mf_eta;
		if (dphi < -M_PI) dphi += 2.0 * M_PI;
		
		const Amg::Vector3D vertex(0.,0.,0.);
		const PerigeeParameters *pPartPerigeeParameters=m_truthParameters->perigeeParameters(*pPart,vertex);
                if (pPartPerigeeParameters==0) {
                    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Failed to retrieve the Truth parameters, skipping this track!" << endreq;
                    continue;
                }
		int charge = pPartPerigeeParameters->charge() > 0 ? 1 : -1;
                //double sign = muonFeature->pt()> 0. ? 1. : -1.;
		//if (charge == sign &&
		if(fabs(deta) < 0.006*50 &&
		   fabs(dphi) < 0.006*50)
		//	if (charge == sign &&
		//		fabs(deta) < 0.006*10 &&
		//		fabs(dphi) < 0.006*10 &&
		//		fabs(dpt_pt) < 0.2*10)
	        {	
		     //const PerigeeParameters *pPartPerigeeParameters=m_truthParameters->perigeeParameters(*pPart,vertex);
		     //double charge = pPartPerigeeParameters->charge();
		     //if (charge == pPerigee->charge()) {
                     if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"Truth Association Found" << endreq;
		     truthTrack.PDG_ID=pPart->pdg_id();
		     truthTrack.PDG_Mother=-100;
		     if (pPart->production_vertex()!=NULL) 
                         if (pPart->production_vertex()->particles_in_size()>0) 
                             truthTrack.PDG_Mother=(*pPart->production_vertex()->particles_begin(HepMC::parents))->pdg_id();
	             truthTrack.Phi = pPart->momentum().phi();
		     truthTrack.Eta = pPart->momentum().eta();
		     truthTrack.Theta = 1./tan(pPart->momentum().theta());
		     truthTrack.Pt = pPart->momentum().perp();
		     truthTrack.d0=pPartPerigeeParameters->transverseImpact();
		     truthTrack.z0=pPartPerigeeParameters->position().z();
		     truthTrack.Charge = charge;
		     truthTrack.QOverP = charge/pPart->momentum().perp();
		     truthTrack.DPhi = dphi;
		     truthTrack.DEta = deta;
		     truthTrack.DPt_Pt = dpt_pt;
		
                     if (msgLvl(MSG::DEBUG)) 
		     {
                         msg(MSG::DEBUG) <<"This mFeature is associated to muon " << endreq;
		         msg(MSG::DEBUG) << "Truth parameters   :"
					<< " PDG_ID=" << pPart->pdg_id()
			          	<< " pT=" << pPart->momentum().perp()
					<< " eta=" << pPart->momentum().phi()
					<< " phi=" << pPart->momentum().eta()
					<< " deta=" << deta
					<< " dphi=" << dphi
					<< " cotTheta=" <<truthTrack.Theta
					<< " charge=" << charge 
					<< " z0=" << truthTrack.z0
					<< " d0=" <<truthTrack.d0<<endreq;
			  msg(MSG::DEBUG) << "muonFeature parameters:"
					<< " pT=" << muonFeature->pt()
					<< " eta=" << muonFeature->eta()
					<< " phi=" << muonFeature->phi()<<endreq;
		     }
		     return StatusCode::SUCCESS;
			
               }

	    }

	}
    }
    if (found_match) 
       return StatusCode::SUCCESS;
    else
    {
       if (msgLvl(MSG::DEBUG)) 
           msg(MSG::DEBUG) <<"This mFeature is not associated to truth " << endreq;
       return StatusCode::RECOVERABLE;
    }
}
			

//========================================

StatusCode PerformanceTruthTool::getTruthTrack(const TrigMuonEFTrack* muonEFTrack, TruthTrack& truthTrack,CandidateSummary* summary)
{
    if (msgLvl(MSG::DEBUG)) msg() << "in PerformanceTruthTool::getTruthTrack from mFeature" << endreq;


    double mf_pt = muonEFTrack->pt();
    double mf_phi = muonEFTrack->phi();
    double mf_eta = muonEFTrack->eta();
    bool found_match = false;
    double cand_momentum = 0.;
    if (summary->qOverP != 0.) cand_momentum = 1./fabs(summary->qOverP);


    const TrackRecordCollection* pRecordCollection = NULL;
    StatusCode sc = evtStore()->retrieve(pRecordCollection, "MuonEntryLayerFilter");
    if (sc.isFailure() || pRecordCollection == NULL)
    {
        msg(MSG::WARNING) << "Cannot retrieve TrackRecordCollection" << endreq;
        goto mcEvent;
    }

 
    for (unsigned int i=0;i<summary->innerIsectPositions.size();i++)
    {
       double isect_eta = (summary->innerIsectPositions[i]).eta();
       double isect_theta = 2*atan(exp(-isect_eta));
       double isect_phi = (summary->innerIsectPositions[i]).phi();
       double isect_pt = cand_momentum * sin(isect_theta);
     
       if (msgLvl(MSG::DEBUG)) msg() << "Found InnerDist intersection: p "<<cand_momentum<<" pt "
                                      <<isect_pt<<" phi "<<isect_phi<<" eta "<<isect_eta << endreq;

       for (TrackRecordConstIterator itTrkRec = pRecordCollection->begin();
            itTrkRec != pRecordCollection->end();
            itTrkRec++)
       {
           TrackRecord pTrkRec = *itTrkRec;
 
           if (fabs(pTrkRec.GetPDGCode())!= 13 && fabs(pTrkRec.GetPDGCode())<1000000) continue;

          double dpt_pt = (pTrkRec.GetMomentum().perp() - isect_pt) / pTrkRec.GetMomentum().perp();
          double dphi = pTrkRec.GetMomentum().phi() - isect_phi;
          double deta = pTrkRec.GetMomentum().eta() - isect_eta;

          if (dphi < -M_PI) dphi += 2.0 * M_PI;

          const Amg::Vector3D vertex(0.,0.,0.);
          // const PerigeeParameters *pPartPerigeeParameters=m_truthParameters->perigeeParameters(*pPart,vertex);
          double charge = 1.;
          if (pTrkRec.GetPDGCode()>0) charge = -1.;
          if (fabs(deta) < 0.006*50 &&
              fabs(dphi) < 0.006*50)
          {
              if (msgLvl(MSG::DEBUG)) msg() << "Truth Association Found" << endreq;
              truthTrack.PDG_ID=pTrkRec.GetPDGCode();
              truthTrack.PDG_Mother=999;
              truthTrack.Phi = pTrkRec.GetMomentum().phi();
              truthTrack.Eta = pTrkRec.GetMomentum().eta();
              truthTrack.Theta = 1./tan(pTrkRec.GetMomentum().theta());
              truthTrack.Pt = pTrkRec.GetMomentum().perp();
              // truthTrack.d0=pPartPerigeeParameters->transverse_impact();
              // truthTrack.z0=pPartPerigeeParameters->position().z();
              // truthTrack.Charge = (int)charge;
              truthTrack.QOverP = charge/pTrkRec.GetMomentum().perp();
              truthTrack.DPhi = dphi;
              truthTrack.DEta = deta;
              truthTrack.DPt_Pt = dpt_pt;
              found_match = true;
              break;
          }
       }
    }

    mcEvent:

    m_pMcEventCollection = NULL;
    sc = evtStore()->retrieve(m_pMcEventCollection, m_sMcEventCollection);
    if( sc.isFailure() || m_pMcEventCollection == NULL)
    {
        if (msgLvl(MSG::DEBUG)) msg(MSG::WARNING) <<"Cannot retrieve McEventCollection" << endreq;
        if (found_match) 
            return StatusCode::SUCCESS;
        else
            return StatusCode::RECOVERABLE;
    }
    
    for (McEventCollection::const_iterator itMc = m_pMcEventCollection->begin();
            itMc != m_pMcEventCollection->end();
            itMc++)
    {
        const HepMC::GenEvent* pGenEvent = *itMc;
        if (pGenEvent==NULL) continue;

        for (HepMC::GenEvent::particle_const_iterator itPart = pGenEvent->particles_begin();
                itPart != pGenEvent->particles_end();
                itPart++)
        {
            const HepMC::GenParticle* pPart = *itPart;
            if (fabs(pPart->pdg_id())>1000000 && fabs(pPart->momentum().eta())>1.0)
                if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"True R-Hadron out of barrel   :"
                      << " PDG_ID=" << pPart->pdg_id()
		      << " Eta=" << pPart->momentum().eta()<<endreq;

            if (fabs(pPart->pdg_id()) !=13 && fabs(pPart->pdg_id())<1000000) continue;

	    if(pPart->status() == 1&&pPart->momentum().perp()>1000.)
	    {
		double dpt_pt = (pPart->momentum().perp() - mf_pt*1000) / pPart->momentum().perp();
		double dphi = pPart->momentum().phi() - mf_phi;
		double deta = pPart->momentum().eta() - mf_eta;
		if (dphi < -M_PI) dphi += 2.0 * M_PI;
		
		const Amg::Vector3D vertex(0.,0.,0.);
		const PerigeeParameters *pPartPerigeeParameters=m_truthParameters->perigeeParameters(*pPart,vertex);
                if (pPartPerigeeParameters==0) {
                    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Failed to retrieve the Truth parameters, skipping this track!" << endreq;
                    continue;
                }
		int charge = pPartPerigeeParameters->charge() > 0 ? 1 : -1;
		//if (charge == sign &&
		if(fabs(deta) < 0.006*50 &&
		   fabs(dphi) < 0.006*50)
		//	if (charge == sign &&
		//		fabs(deta) < 0.006*10 &&
		//		fabs(dphi) < 0.006*10 &&
		//		fabs(dpt_pt) < 0.2*10)
	        {	
		     //const PerigeeParameters *pPartPerigeeParameters=m_truthParameters->perigeeParameters(*pPart,vertex);
		     //double charge = pPartPerigeeParameters->charge();
		     //if (charge == pPerigee->charge()) {
                     if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"Truth Association Found" << endreq;
		     truthTrack.PDG_ID=pPart->pdg_id();
		     truthTrack.PDG_Mother=-100;
		     if (pPart->production_vertex()!=NULL) 
                         if (pPart->production_vertex()->particles_in_size()>0) 
                             truthTrack.PDG_Mother=(*pPart->production_vertex()->particles_begin(HepMC::parents))->pdg_id();
	             truthTrack.Phi = pPart->momentum().phi();
		     truthTrack.Eta = pPart->momentum().eta();
		     truthTrack.Theta = 1./tan(pPart->momentum().theta());
		     truthTrack.Pt = pPart->momentum().perp();
		     truthTrack.d0=pPartPerigeeParameters->transverseImpact();
		     truthTrack.z0=pPartPerigeeParameters->position().z();
		     truthTrack.Charge = charge;
		     truthTrack.QOverP = charge/pPart->momentum().perp();
		     truthTrack.DPhi = dphi;
		     truthTrack.DEta = deta;
		     truthTrack.DPt_Pt = dpt_pt;
		
                     if (msgLvl(MSG::DEBUG)) 
		     {
                         msg(MSG::DEBUG) <<"This muonEFTrack is associated to muon " << endreq;
		         msg(MSG::DEBUG) << "Truth parameters   :"
					<< " PDG_ID=" << pPart->pdg_id()
			          	<< " pT=" << pPart->momentum().perp()
					<< " eta=" << pPart->momentum().phi()
					<< " phi=" << pPart->momentum().eta()
					<< " deta=" << deta
					<< " dphi=" << dphi
					<< " cotTheta=" <<truthTrack.Theta
					<< " charge=" << charge 
					<< " z0=" << truthTrack.z0
					<< " d0=" <<truthTrack.d0<<endreq;
			  msg(MSG::DEBUG) << "muonEFTrack parameters:"
					<< " pT=" << muonEFTrack->pt()
					<< " eta=" << muonEFTrack->eta()
					<< " phi=" << muonEFTrack->phi()<<endreq;
		     }
		     return StatusCode::SUCCESS;
			
               }

	    }

	}
    }
    if (found_match) 
       return StatusCode::SUCCESS;
    else
    {
       if (msgLvl(MSG::DEBUG)) 
           msg(MSG::DEBUG) <<"This muonEFTrack is not associated to truth " << endreq;
       return StatusCode::RECOVERABLE;
    }
}
			

//========================================
StatusCode PerformanceTruthTool::bookNTuple(NTuple::Tuple* pNtuple)
{
    msg(MSG::INFO) << " Adding Candidate Truth Parameters to NTuple" << endreq;
    if (pNtuple->addItem("MUGIRL/TruthPdgId" , m_pTruthPdgId).isFailure() ||
            pNtuple->addItem("MUGIRL/TruthPdgMother" , m_pTruthPdgMother).isFailure() ||
            pNtuple->addItem("MUGIRL/TruthPt" , m_pTruthPt).isFailure() ||
            pNtuple->addItem("MUGIRL/TruthQOverP" ,  m_pTruthQOverP).isFailure() ||
            pNtuple->addItem("MUGIRL/TruthEta" ,     m_pTruthEta).isFailure() ||
            pNtuple->addItem("MUGIRL/TruthPhi" ,     m_pTruthPhi).isFailure() ||
            pNtuple->addItem("MUGIRL/TruthTheta" ,   m_pTruthTheta).isFailure() ||
            pNtuple->addItem("MUGIRL/Trutha0" ,      m_pTrutha0).isFailure() ||
            pNtuple->addItem("MUGIRL/Truthz0" ,      m_pTruthz0).isFailure() ||
            pNtuple->addItem("MUGIRL/TruthCharge",   m_pTruthCharge).isFailure() ||
            pNtuple->addItem("MUGIRL/TruthDPt" ,     m_pTruthDPt).isFailure() ||
            pNtuple->addItem("MUGIRL/TruthDEta" ,    m_pTruthDEta).isFailure() ||
            pNtuple->addItem("MUGIRL/TruthDPhi" ,    m_pTruthDPhi).isFailure())
    {
        msg(MSG::WARNING) << "Cannot addItem(MUGIRL/<Candidate Truth Parameters>)" << endreq;
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

//StatusCode PerformanceTruthTool::fillNTuple(const Rec::TrackParticle* pParticle)
StatusCode PerformanceTruthTool::fillNTuple(CandidateSummary* summary)
{
    TruthTrack truthTrack;
    StatusCode sc;
   if (summary->startFromMF == true)
       sc = getTruthTrack(summary->pMuonFeature, truthTrack, summary);
   else if (summary->startFromEF == true)
       sc = getTruthTrack(summary->pMuonEFTrack, truthTrack, summary);
   else
       sc = getTruthTrack(summary->pTrackParticle, truthTrack, summary);

    if (sc.isSuccess())
    {
        m_pTruthPdgId     = truthTrack.PDG_ID;
        m_pTruthPdgMother = truthTrack.PDG_Mother;
        m_pTruthEta       = truthTrack.Eta;
        m_pTruthPt        = truthTrack.Pt;
        m_pTruthEta       = truthTrack.Eta;
        m_pTruthPhi       = truthTrack.Phi;
        m_pTruthQOverP    = truthTrack.QOverP;
        m_pTruthTheta     = truthTrack.Theta;
        m_pTrutha0        = truthTrack.d0;
        m_pTruthz0        = truthTrack.z0;
        m_pTruthCharge    = truthTrack.Charge;
        m_pTruthDPt       = truthTrack.DPt_Pt;
        m_pTruthDEta      = truthTrack.DEta;
        m_pTruthDPhi      = truthTrack.DPhi;
    }
    else
    {
        msg(MSG::WARNING) << "Truth Particle not Found" << endreq;
        m_pTruthPdgId     = -100.0;
        m_pTruthPdgMother = -100.0;
        m_pTruthPt        = -100.0;
        m_pTruthEta       = -100.0;
        m_pTruthPhi       = -100.0;
        m_pTruthQOverP    = -100.0;
        m_pTruthTheta     = -100.0;
        m_pTrutha0        = -100.0;
        m_pTruthz0        = -100.0;
        m_pTruthCharge    = -100.0;
        m_pTruthDPt       = -100.0;
        m_pTruthDEta      = -100.0;
        m_pTruthDPhi      = -100.0;
    }
    return StatusCode::SUCCESS;
}
/*
StatusCode PerformanceTruthTool::fillNTuple(const MuonFeature* muonFeature)
{
    if (msgLvl(MSG::DEBUG)) 
        msg(MSG::DEBUG) << "in PerformanceTruthTool::fillNTuple from mFeature" << endreq;
	TruthTrack truthTrack;
	StatusCode sc = getTruthTrack(muonFeature, truthTrack, summary);
        if (sc.isSuccess())
        {
	    m_pTruthPdgId     = truthTrack.PDG_ID;
	    m_pTruthPdgMother = truthTrack.PDG_Mother;
            m_pTruthEta       = truthTrack.Eta;
            m_pTruthPt        = truthTrack.Pt;
            m_pTruthEta       = truthTrack.Eta;
            m_pTruthPhi       = truthTrack.Phi;
	    m_pTruthQOverP    = truthTrack.QOverP;
	    m_pTruthTheta     = truthTrack.Theta;
	    m_pTrutha0	      = truthTrack.d0;
	    m_pTruthz0	      = truthTrack.z0;
            m_pTruthCharge    = truthTrack.Charge;
            m_pTruthDPt       = truthTrack.DPt_Pt;
            m_pTruthDEta      = truthTrack.DEta;
            m_pTruthDPhi      = truthTrack.DPhi;
	    
		
        }
        else
        {
	    msg(MSG::WARNING) << "Truth Particle not Found" << endreq;
	    m_pTruthPdgId     = -100.0;
	    m_pTruthPdgMother = -100.0;
            m_pTruthPt        = -100.0;
            m_pTruthEta       = -100.0;
            m_pTruthPhi       = -100.0;
	    m_pTruthQOverP    = -100.0;
	    m_pTruthTheta     = -100.0;
	    m_pTrutha0	      = -100.0;
	    m_pTruthz0	      = -100.0;
            m_pTruthCharge    = -100.0;
            m_pTruthDPt       = -100.0;
            m_pTruthDEta      = -100.0;
            m_pTruthDPhi      = -100.0;
        }
	return StatusCode::SUCCESS;
}
*/
