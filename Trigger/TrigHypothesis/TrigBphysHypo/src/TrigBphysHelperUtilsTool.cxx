///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigBphysHelperUtilsTool.cxx 
// Implementation file for class TrigBphysHelperUtilsTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// TrigBphysHypo includes
#include "TrigBphysHelperUtilsTool.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"

// StoreGate
#include "StoreGate/StoreGateSvc.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "xAODTrigMuon/L2StandAloneMuon.h"


/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
TrigBphysHelperUtilsTool::TrigBphysHelperUtilsTool( const std::string& type, 
		      const std::string& name, 
		      const IInterface* parent ) : 
  ::AthAlgTool  ( type, name, parent   )
,  m_fitterSvc("Trk::TrkVKalVrtFitter/VertexFitterTool",this)
, m_massMuon(105.6583715)
{
  declareInterface< TrigBphysHelperUtilsTool >(this);
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );
    declareProperty("VertexFitterTool", m_fitterSvc);
}

// Destructor
///////////////
TrigBphysHelperUtilsTool::~TrigBphysHelperUtilsTool()
{}

// Athena algtool's Hooks
////////////////////////////
StatusCode TrigBphysHelperUtilsTool::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << name() << "...");
    
    if (m_fitterSvc.retrieve().isFailure()) {
        ATH_MSG_ERROR("Can't find Trk::TrkVKalVrtFitter" );
        return StatusCode::FAILURE;
    } else {
        ATH_MSG_DEBUG("Trk::TrkVKalVrtFitter found" );
        m_VKVFitter = dynamic_cast<Trk::TrkVKalVrtFitter*>(&(*m_fitterSvc));
    }
  if(m_beamSpotKey.initialize().isFailure()){
    msg() << MSG::ERROR << "Beamspot error" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigBphysHelperUtilsTool::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////


double TrigBphysHelperUtilsTool::deltaPhi( double phi1, double phi2) const {
    double dphi = phi1 - phi2;
    while ( dphi >  M_PI ) dphi -= 2*M_PI;
    while ( dphi < -M_PI ) dphi += 2*M_PI;
    return dphi;
}

double TrigBphysHelperUtilsTool::deltaEta( double eta1, double eta2) const {
    return eta1 - eta2;
}

double TrigBphysHelperUtilsTool::deltaR(double eta1, double phi1, double eta2, double phi2) const {
    return deltaR( deltaEta(eta1,eta2),deltaPhi(phi1,phi2));
}

double TrigBphysHelperUtilsTool::deltaR(double deta, double dphi) const {
    return sqrt(deta*deta + dphi*dphi);
}


void TrigBphysHelperUtilsTool::addUnique(std::vector<const Trk::Track*>& tracks, const Trk::Track* trkIn) const {
    // from the the original run-1 code
    if (!trkIn) return;
    std::vector<const Trk::Track*>::iterator tItr     = tracks.begin();
    std::vector<const Trk::Track*>::iterator tItr_end = tracks.end();

    double phi = trkIn->perigeeParameters()->parameters()[Trk::phi];
    double eta = (*tItr)->perigeeParameters()->eta();
    double pT  = fabs(trkIn->perigeeParameters()->pT());
    
    for (;tItr != tItr_end; ++tItr) {
        if (trkIn == *tItr) continue; // don't consider if already included
        // match on eta and phi
        double dphi = absDeltaPhi((*tItr)->perigeeParameters()->parameters()[Trk::phi], phi);
        double deta = absDeltaEta((*tItr)->perigeeParameters()->eta(), eta);
        double dpt  = fabs(fabs((*tItr)->perigeeParameters()->pT()) - pT);
        
        if (dphi < 0.005 &&
            deta < 0.005 &&
            dpt  < 10.) return; // found a matching track, so return out of the function
    } // for
    
    // if here then track is added to list
    tracks.push_back(trkIn);
}

bool TrigBphysHelperUtilsTool::areUnique(const xAOD::TrackParticle* t0, const xAOD::TrackParticle* t1, double dEtaCut , double dPhiCut, double dPtCut) const {
    if (!t0 || !t1) {
        ATH_MSG_DEBUG("nullptr for inputs " << t0 << " " << t1 );
        return false; // if nullptr then return false
    }
    double pt0  = t0->pt();
    double eta0 = t0->eta();
    double phi0 = t0->phi();
    double pt1  = t1->pt();
    double eta1 = t1->eta();
    double phi1 = t1->phi();
    ATH_MSG_DEBUG("Test Uniqueness of: pT1/pT2, eta1/eta2, phi1/phi2: "
        << pt0  << " / " << pt1  << ",   " << eta0 << " / " << eta1  << ",   " << phi0 << " / " << phi1  );
    double dphi = absDeltaPhi(phi0, phi1);
    double deta = absDeltaEta(eta0, eta1);
    double dpt  = fabs( pt0 - pt1);

    if (dphi < dPhiCut &&
        deta < dEtaCut &&
        (dPtCut < 0 && dpt < dPtCut)
        ) {
        ATH_MSG_DEBUG(" is a Match" );
        return false; // consider tracks to be unique
    } else {
        ATH_MSG_DEBUG("Tracks are diferent" );
    }
    // if here then the tracks are unique
    return true;
} // areUnique


void TrigBphysHelperUtilsTool::addUnique(const xAOD::Muon* muon, std::vector<const xAOD::Muon*> & output,
                                         double dEtaCut, double dPhiCut, double dPtCut,
                                         xAOD::Muon::TrackParticleType ptype ) const {
    if (!muon) {
        ATH_MSG_DEBUG("Muon has nullptr " );
        return;
    }
    
    const xAOD::TrackParticle* muontp = muon->trackParticle(ptype);
    if (!muontp) {
        ATH_MSG_DEBUG("Muon does nove tp of type: " << ptype );
        return;
    }
    
    
    double pt  = muontp->pt();
    double eta = muontp->eta();
    double phi = muontp->phi();
    
    ATH_MSG_DEBUG("to Match: " << muon << " " << muontp << " "
        << pt << " " << eta << " " << phi );

    if (!output.size()) {
        ATH_MSG_DEBUG("No previous muons - is unique" << ptype );
        output.push_back(muon); // if no entries, then by definition, unique
        return; // done
    }

    
    for (const auto muin : output) {
        const xAOD::TrackParticle* muintp = muin->trackParticle(ptype);
        if (!muintp) continue; // shouldn't really happen
        
        double ptin  = muintp->pt();
        double etain = muintp->eta();
        double phiin = muintp->phi();

        double dphi = absDeltaPhi(phiin, phi);
        double deta = absDeltaEta(etain, eta);
        double dpt  = fabs( pt - ptin);

        ATH_MSG_DEBUG(" against: " << muin << " " << muintp << " "
            << ptin << " " << etain << " " << phiin );
        
        if (dphi < dPhiCut &&
            deta < dEtaCut &&
            (dPtCut < 0 || dpt < dPtCut)
            ) {
            ATH_MSG_DEBUG(" is a Match" );
            return; // found a matching track, so return out of the function
        }

    } // loop over already unoique muons
    
    ATH_MSG_DEBUG("No match - is unique" );
    // if here, have found no match amongst > 0 possibles
    output.push_back(muon);
    
} //addUnique



const xAOD::EventInfo* TrigBphysHelperUtilsTool::getEventInfo() const {
    // get the event info; return nullptr if not there
    const xAOD::EventInfo *evtInfo(0);
    if ( evtStore()->retrieve(evtInfo).isFailure() ) {
        ATH_MSG_DEBUG("Failed to get xAOD::EventInfo " );
        return nullptr;
    }
    return evtInfo;
}

StatusCode TrigBphysHelperUtilsTool::getRunEvtLb(uint32_t & run, uint32_t & evt,uint32_t & lb) const {
    run = ~0;
    evt = ~0;
    lb  = ~0;
    // JW - Try to get the xAOD event info
    const EventInfo* pEventInfo(0);
    const xAOD::EventInfo *evtInfo(0);
    if ( evtStore()->retrieve(evtInfo).isFailure() ) {
        ATH_MSG_DEBUG("Failed to get xAOD::EventInfo " );
        // now try the old event ifo
        if ( evtStore()->retrieve(pEventInfo).isFailure() ) {
            ATH_MSG_DEBUG("Failed to get EventInfo " );
            return StatusCode::FAILURE;
        } else {
            run   = pEventInfo->event_ID()->run_number();
            evt   = pEventInfo->event_ID()->event_number();
            lb    = pEventInfo->event_ID()->lumi_block();
            ATH_MSG_DEBUG(" Run " << run << " Event " << evt );
        }// found old event info
    }else { // found the xAOD event info
        run   = evtInfo->runNumber();
        evt   = evtInfo->eventNumber();
        lb    = evtInfo->lumiBlock();
        ATH_MSG_DEBUG(" Run " << run << " Event " << evt );
    } // get event ifo

    return StatusCode::SUCCESS;
}


StatusCode TrigBphysHelperUtilsTool::buildDiMu(const std::vector<ElementLink<xAOD::TrackParticleContainer> > & particles,
                                               xAOD::TrigBphys *& result,
                                               xAOD::TrigBphys::pType ptype,
                                               xAOD::TrigBphys::levelType plevel) {
    ///Note - if sucess, then caller is responsible for the memory created in result
    ATH_MSG_DEBUG("In buildDiMu" );
    result = nullptr;
    
    if (particles.size() != 2) {
        ATH_MSG_DEBUG("Found " << particles.size() << " inputs. Needed 2" );
        return StatusCode::FAILURE;
    }
    if (!particles[0].isValid() | !particles[1].isValid() ) {
        ATH_MSG_DEBUG("Invalid inputs" );
        return StatusCode::FAILURE;
    }

    
    xAOD::TrackParticle::FourMom_t fourMom = (*particles[0])->p4() + (*particles[1])->p4();
    double massMuMu = invariantMass( *particles[0], *particles[1], m_massMuon,m_massMuon); 
    
    double rap      = fourMom.Rapidity();
    double phi      = fourMom.Phi();
    double pt       = fourMom.Pt();
    
    result = new xAOD::TrigBphys;
    result->makePrivateStore();
    
    result->initialise(0, rap, phi, pt,ptype, massMuMu, plevel );
    bool doFit(true); // set false if problematic TP
    
    // check the TrackParticles for a covariance matrix
    if ((*particles[0])->definingParametersCovMatrixVec().size() == 0) {
        doFit = false;
        ATH_MSG_DEBUG("Fit not allowed, Problems with TP0" );
    }
    if ((*particles[1])->definingParametersCovMatrixVec().size() == 0) {
        doFit = false;
        ATH_MSG_DEBUG("Fit not allowed, Problems with TP1" );
    }

    //const Trk::Vertex startingPoint(Amg::Vector3D(0.,0.,0.)); // #FIXME use beamline for starting point?
    const Amg::Vector3D startingPoint(0.,0.,0.);
    std::vector<const xAOD::TrackParticle*> trks;
    trks.push_back(*particles[0]);
    trks.push_back(*particles[1]);
    xAOD::Vertex * vx(0);
    if (doFit) vx =  m_fitterSvc->fit(trks,startingPoint);

    if (!vx){
        ATH_MSG_DEBUG("No Vertex returned from fit / fitting not allowed" );
        
        result->setFitmass     (-9999);
        result->setFitchi2     (-9999);
        result->setFitndof     (-9999);
        result->setFitx        (-9999);
        result->setFity        (-9999);
        result->setFitz        (-9999);
        
    } else {
        //std::vector<int> trkIndices(particles.size(),1);
        double invariantMass(0.), invariantMassError(0.); // #FIXME what about the input masses?
        std::vector<double> masses(particles.size(), m_massMuon);
        m_VKVFitter->setMassInputParticles(masses); // give input tracks muon mass
        //if (!(m_VKVFitter->VKalGetMassError(trkIndices,invariantMass,invariantMassError).isSuccess())) {
        if (!(m_VKVFitter->VKalGetMassError(invariantMass,invariantMassError).isSuccess())) {
            ATH_MSG_DEBUG("Warning from VKaVrt - cannot calculate uncertainties!");
        } // if
        
        result->setFitmass     (invariantMass);
        result->setFitmassError(invariantMassError);
        result->setFitchi2     (vx->chiSquared());
        result->setFitndof     (vx->numberDoF());
        result->setFitx        (vx->x());
        result->setFity        (vx->y());
        result->setFitz        (vx->z());
        
        delete vx; vx = 0;
    } // if vx
    
    // now add in the element links - note that they need the reshuffling applied

    
    ATH_MSG_DEBUG("Print for obj: " << result << "\n\t  " <<
        "roiId:         " << result->roiId()  << "\n\t  " <<
        "particleType:  " << result->particleType() << "\n\t  " <<
        "level:         " << result->level() << "\n\t  " <<
        "eta:           " << result->eta() << "\n\t  " <<
        "phi:           " << result->phi() << "\n\t  " <<
        "mass:          " << result->mass() << "\n\t  " <<
        "fitmass:       " << result->fitmass() << "\n\t  " <<
        "fitchi2:       " << result->fitchi2() << "\n\t  " <<
        "fitndof:       " << result->fitndof() << "\n\t  " <<
        "fitx:          " << result->fitx() << "\n\t  " <<
        "fity:          " << result->fity() << "\n\t  " <<
        "fitz:          " << result->fitz() << "\n\t  " );

    result->addTrackParticleLink(particles[0]);
    result->addTrackParticleLink(particles[1]);


    
    return StatusCode::SUCCESS;
} //buildDiMu

StatusCode TrigBphysHelperUtilsTool::vertexFit(xAOD::TrigBphys * result,
                                               const std::vector<ElementLink<xAOD::TrackParticleContainer> > &particles,
                                               std::vector<double>& inputMasses) { // inputmasses not const, as vertex code doesn't allowit
    ATH_MSG_DEBUG("In vertexFit" );
    if (!result) {
        ATH_MSG_DEBUG("Need to provide valid TrigBphys object" );
        return StatusCode::FAILURE;
    }
    if (particles.size() != inputMasses.size()) {
        ATH_MSG_WARNING("Mismatch in particle and mass vector sizes" );
        return StatusCode::FAILURE;
    }
    bool doFit(true); // set false if problematic TP
    std::vector<const xAOD::TrackParticle*> trks;
    
    for ( auto ptlEL : particles) {
        if (!ptlEL.isValid()) {
            ATH_MSG_DEBUG("Non valid TPEL" );
            doFit = false;
        }
        if ((*ptlEL)->definingParametersCovMatrixVec().size() == 0) {
            doFit = false;
            ATH_MSG_DEBUG("Fit not allowed, Problems with TP in vertexFit" );
        }
        trks.push_back(*ptlEL);
    } // loop over particle ELs
    
    const Amg::Vector3D startingPoint(0.,0.,0.); // #FIXME use beamline for starting point?
    //const Trk::Vertex startingPoint(Amg::Vector3D(0.,0.,0.)); // #FIXME use beamline for starting point?
    xAOD::Vertex * vx(0);
    if (doFit) vx =  m_fitterSvc->fit(trks,startingPoint);
    TLorentzVector tracks_p;

    if (!vx){
        ATH_MSG_DEBUG("No Vertex returned from fit / fitting not allowed" );
        
        result->setFitmass     (-9999);
        result->setFitchi2     (-9999);
        result->setFitndof     (-9999);
        result->setFitx        (-9999);
        result->setFity        (-9999);
        result->setFitz        (-9999);
        result->setEta         (-9999);
        result->setPhi         (-9999);
    
    } else {
        //std::vector<int> trkIndices(particles.size(),1);
        double invariantMass(0.), invariantMassError(0.); // #FIXME what about the input masses?
        m_VKVFitter->setMassInputParticles( inputMasses); // give input tracks muon mass
        //if (!(m_VKVFitter->VKalGetMassError(trkIndices,invariantMass,invariantMassError).isSuccess())) {
        if (!(m_VKVFitter->VKalGetMassError(invariantMass,invariantMassError).isSuccess())) {
            ATH_MSG_DEBUG("Warning from VKaVrt - cannot calculate uncertainties!");
            invariantMass = -9999.;
        } // if
        
        for (unsigned int i_track =0; i_track < vx->nTrackParticles(); i_track++)
            tracks_p = vx->trackParticle(i_track)->p4() + tracks_p;

        result->setFitmass     (invariantMass);
        result->setFitchi2     (vx->chiSquared());
        result->setFitndof     (vx->numberDoF());
        result->setFitx        (vx->x());
        result->setFity        (vx->y());
        result->setFitz        (vx->z());
        result->setEta         (tracks_p.Eta());
        result->setPhi         (tracks_p.Phi()); 
        
        delete vx; vx = 0;
    } // if vx
    ATH_MSG_DEBUG(
        "Print for obj: " << result << "\n\t  " <<
        "roiId:         " << result->roiId()  << "\n\t  " <<
        "particleType:  " << result->particleType() << "\n\t  " <<
        "level:         " << result->level() << "\n\t  " <<
        "eta:           " << result->eta() << "\n\t  " <<
        "phi:           " << result->phi() << "\n\t  " <<
        "mass:          " << result->mass() << "\n\t  " <<
        "fitmass:       " << result->fitmass() << "\n\t  " <<
        "fitchi2:       " << result->fitchi2() << "\n\t  " <<
        "fitndof:       " << result->fitndof() << "\n\t  " <<
        "fitx:          " << result->fitx() << "\n\t  " <<
        "fity:          " << result->fity() << "\n\t  " <<
        "fitz:          " << result->fitz() << "\n\t  " );
    
    for ( auto ptlEL : particles) {
        result->addTrackParticleLink(ptlEL);
    }
    return StatusCode::SUCCESS;
} //vertexFit

StatusCode TrigBphysHelperUtilsTool::vertexFit(xAOD::TrigBphys * result,
                                               const std::vector<const xAOD::TrackParticle*> &trks,
                                               std::vector<double>& inputMasses) { // inputmasses not const, as vertex code doesn't allowit
    ATH_MSG_DEBUG("In vertexFit" );
    if (!result) {
        ATH_MSG_DEBUG("Need to provide valid TrigBphys object" );
        return StatusCode::FAILURE;
    }
    if (trks.size() != inputMasses.size()) {
        ATH_MSG_WARNING("Mismatch in particle and mass vector sizes" );
        return StatusCode::FAILURE;
    }
    bool doFit(true); // set false if problematic TP
    
    const Amg::Vector3D startingPoint(0.,0.,0.); // #FIXME use beamline for starting point?
    //const Trk::Vertex startingPoint(Amg::Vector3D(0.,0.,0.)); // #FIXME use beamline for starting point?
    xAOD::Vertex * vx(0);
    if (doFit) vx =  m_fitterSvc->fit(trks,startingPoint);
    TLorentzVector tracks_p;

    if (!vx){
        ATH_MSG_DEBUG("No Vertex returned from fit / fitting not allowed" );
        
        result->setFitmass     (-9999);
        result->setFitchi2     (-9999);
        result->setFitndof     (-9999);
        result->setFitx        (-9999);
        result->setFity        (-9999);
        result->setFitz        (-9999);
        result->setEta         (-9999);
        result->setPhi         (-9999);

    } else {
        std::vector<int> trkIndices(trks.size(),1);
        double invariantMass(0.), invariantMassError(0.); // #FIXME what about the input masses?
        m_VKVFitter->setMassInputParticles( inputMasses); // give input tracks muon mass
        //if (!(m_VKVFitter->VKalGetMassError(trkIndices,invariantMass,invariantMassError).isSuccess())) {
        if (!(m_VKVFitter->VKalGetMassError(invariantMass,invariantMassError).isSuccess())) {
            ATH_MSG_DEBUG("Warning from VKaVrt - cannot calculate uncertainties!");
            invariantMass = -9999.;
        } // if
        
        for (unsigned int i_track =0; i_track < vx->nTrackParticles(); i_track++)
            tracks_p = vx->trackParticle(i_track)->p4() + tracks_p;

        result->setFitmass     (invariantMass);
        result->setFitchi2     (vx->chiSquared());
        result->setFitndof     (vx->numberDoF());
        result->setFitx        (vx->x());
        result->setFity        (vx->y());
        result->setFitz        (vx->z());
        result->setEta         (tracks_p.Eta());
        result->setPhi         (tracks_p.Phi());

        delete vx; vx = 0;
    } // if vx
    ATH_MSG_DEBUG(
        "Print for obj: " << result << "\n\t  " <<
        "roiId:         " << result->roiId()  << "\n\t  " <<
        "particleType:  " << result->particleType() << "\n\t  " <<
        "level:         " << result->level() << "\n\t  " <<
        "eta:           " << result->eta() << "\n\t  " <<
        "phi:           " << result->phi() << "\n\t  " <<
        "mass:          " << result->mass() << "\n\t  " <<
        "fitmass:       " << result->fitmass() << "\n\t  " <<
        "fitchi2:       " << result->fitchi2() << "\n\t  " <<
        "fitndof:       " << result->fitndof() << "\n\t  " <<
        "fitx:          " << result->fitx() << "\n\t  " <<
        "fity:          " << result->fity() << "\n\t  " <<
        "fitz:          " << result->fitz() << "\n\t  " );
    
    return StatusCode::SUCCESS;
} //vertexFit



double TrigBphysHelperUtilsTool::invariantMass(const xAOD::IParticle *p1, const xAOD::IParticle* p2, double m1, double m2) const {
    return invariantMassIP( {p1,p2},{m1,m2});
} // invariantMass

double TrigBphysHelperUtilsTool::invariantMass(const std::vector<const xAOD::TrackParticle*>&ptls, const std::vector<double> & masses) const {
    // 're-cast the vector in terms of the iparticle'
    std::vector<const xAOD::IParticle*> i_ptls;
    for ( auto tp : ptls) i_ptls.push_back(tp);
    return invariantMassIP(i_ptls,masses);
}


double TrigBphysHelperUtilsTool::invariantMassIP(const std::vector<const xAOD::IParticle*>&ptls, const std::vector<double> & masses) const {
    if (ptls.size() != masses.size()) {
        ATH_MSG_DEBUG("Mismatch of vector sizes in invariantMass" );
        return -1;
    }
    
    
    double px(0.),py(0.),pz(0.),E(0.);
    const unsigned int nPtls(ptls.size());
    for (unsigned int i =0; i < nPtls; ++i) {
        if (!ptls[i]) {
            ATH_MSG_DEBUG("Null ptr in invariantMass; return -1" );
            return -1.;
        }
        // mass correction factors (i.e to turn from GeV to MeV
        // if l2 standalone muon, pt is in GeV, convert to MeV
        double cFactor(1.);
        if ( dynamic_cast<const xAOD::L2StandAloneMuon*>(ptls[i]) ) {
            cFactor = 1000.;
            ATH_MSG_DEBUG("Found L2StandAlone muon for IParticle: " << i << " Treating as having units of GeV" );
        } // if L2 muon
        
                
            
        px += ptls[i]->p4().Px()*cFactor;
        py += ptls[i]->p4().Py()*cFactor;
        pz += ptls[i]->p4().Pz()*cFactor;
        E  += sqrt(masses[i]*masses[i] +
                   ptls[i]->p4().Px()*ptls[i]->p4().Px()*cFactor*cFactor +
                   ptls[i]->p4().Py()*ptls[i]->p4().Py()*cFactor*cFactor +
                   ptls[i]->p4().Pz()*ptls[i]->p4().Pz()*cFactor*cFactor
                   );
        
    } // for
    double m2 = E*E - px*px - py*py -pz*pz;
    if (m2 < 0) return 0.;
    else        return sqrt(m2);
} // invariantMass


void TrigBphysHelperUtilsTool::fillTrigObjectKinematics(xAOD::TrigBphys* bphys,
                                                        const std::vector<const xAOD::TrackParticle*>& ptls)
 {
     if (!bphys) {
         ATH_MSG_WARNING("Null pointer of trigger object provided." );
         return;
     }
     
     //     if (ptls.size() != masses.size()) {
     //         if ( msg().level() <= MSG::WARNING ) {
     //             msg()  << MSG::WARNING << "Nptls != nMasses; no information will be populated." );
     //         }
     //         return;
     //     } // if invalid prequesits
     
     
     
     xAOD::TrackParticle::FourMom_t fourMom;
     
     for (const auto& ptl : ptls) {
         fourMom += ptl->p4();
     }// add fourVectors
     
     
     double rap      = fourMom.Rapidity();
     double phi      = fourMom.Phi();
     double pt       = fourMom.Pt();
     
     bphys->setEta (rap);
     bphys->setPhi (phi);
     bphys->setPt  (pt);
     
 } // fillTrigObjectKinematics


void TrigBphysHelperUtilsTool::setBeamlineDisplacement(xAOD::TrigBphys* bphys,
                             const std::vector<const xAOD::TrackParticle*> &ptls) {
    
    if (!bphys) {
        ATH_MSG_WARNING("Null pointer of trigger object provided." );
        return;
    }
    
    Amg::Vector3D beamSpot(0.,0.,0.);
    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
    if ( !beamSpotHandle.isValid() )
    {
        ATH_MSG_DEBUG("Could not retrieve Beam Conditions Service. " );
    }else {
        beamSpot = beamSpotHandle->beamPos();
        int beamSpotBitMap = beamSpotHandle->beamStatus();
        //* Check if beam spot is from online algorithms *//
        int beamSpotStatus = ((beamSpotBitMap & 0x4) == 0x4);
        ATH_MSG_DEBUG("  beamSpotBitMap= "<< beamSpotBitMap<<" beamSpotStatus= "<<beamSpotStatus);
    }
    
    static const double CONST = 1000./299.792; // unit conversion for lifetime

    
    double Dx     = bphys->fitx() - beamSpot.x();
    double Dy     = bphys->fity() - beamSpot.y();
    double BsMass = bphys->mass();
    
    double sumPx(0.), sumPy(0.), sumPt(0.);
    for (const auto& ptl: ptls) {
        sumPx += ptl->p4().Px(); // FIXME - is there a more optimal way
        sumPy += ptl->p4().Py();
    }
    sumPt = sqrt(sumPx*sumPx + sumPy*sumPy);
    double BsLxy(-9999.);
    double BsTau(-9999.);
    if (sumPt != 0.0) {
       BsLxy = (sumPx*Dx+sumPy*Dy)/sumPt;
       BsTau = BsLxy * BsMass/sumPt * CONST;
    }
    double BsLxyError(-1.);
    double BsTauError(-1.);
    
    bphys->setLxy     (BsLxy);
    bphys->setLxyError(BsLxyError);
    bphys->setTau     (BsTau);
    bphys->setTauError(BsTauError);
} // setBeamlineDisplacement
