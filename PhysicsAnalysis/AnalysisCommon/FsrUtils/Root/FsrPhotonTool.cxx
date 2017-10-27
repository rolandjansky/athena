/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>

// Local include(s):
#include "FsrUtils/FsrPhotonTool.h"
#include "IsolationSelection/IsolationSelectionTool.h"
#include "AsgTools/Check.h"

namespace FSR {

    FsrPhotonTool::FsrPhotonTool( const std::string& name )
        : asg::AsgTool( name ),
          m_fsr_type(FsrCandidate::FsrUnknown),
          m_isoSelTool("")
    {

        declareProperty( "high_et_min", m_high_et_min = 3500. );
        declareProperty( "overlap_el_ph", m_overlap_el_ph = 0.01 );
        declareProperty( "overlap_el_mu", m_overlap_el_mu = 0.001 );
        declareProperty( "far_fsr_drcut", m_far_fsr_drcut = 0.15 );
        declareProperty( "far_fsr_etcut", m_far_fsr_etcut = 10000.0 );
        declareProperty( "far_fsr_isoWorkingPoint", m_far_fsr_isoWorkingPoint = "FixedCutLoose");
        declareProperty( "drcut", m_drcut = 0.15 );
        declareProperty( "etcut", m_etcut = 1000.0 );
        declareProperty( "f1cut", m_f1cut =  0.1 );
        declareProperty( "topo_drcut", m_topo_drcut =  0.08 );
        declareProperty( "topo_f1cut", m_topo_f1cut = 0.2 );
        declareProperty( "IsolationSelectionTool", m_isoSelTool);

    }

    StatusCode FsrPhotonTool::initialize() {

        // Greet the user:
        ATH_MSG_INFO( "Initialising..." );

        ATH_MSG_INFO( "high_et_min " << m_high_et_min);
        ATH_MSG_INFO( "overlap_el_ph " << m_overlap_el_ph);
        ATH_MSG_INFO( "overlap_el_mu " << m_overlap_el_mu);
        ATH_MSG_INFO( "far_fsr_drcut " << m_far_fsr_drcut);
        ATH_MSG_INFO( "far_fsr_etcut " << m_far_fsr_etcut);
        ATH_MSG_INFO( "far_fsr_isoWorkingPoint " << m_far_fsr_isoWorkingPoint);
        ATH_MSG_INFO( "drcut " << m_drcut);
        ATH_MSG_INFO( "etcut " << m_etcut);
        ATH_MSG_INFO( "f1cut " << m_f1cut);
        ATH_MSG_INFO( "topo_drcut " << m_topo_drcut);
        ATH_MSG_INFO( "topo_f1cut " << m_topo_f1cut);
          
        // Initialize the isolation selection tool
        if (m_isoSelTool.empty()){
            CP::IsolationSelectionTool* isoTool = new CP::IsolationSelectionTool("FsrIsoSelectionTool" + name());
            ASG_CHECK(isoTool->setProperty("PhotonWP", m_far_fsr_isoWorkingPoint));
            ASG_CHECK(isoTool->initialize());
            m_isoSelTool = ToolHandle<CP::IIsolationSelectionTool>("FsrIsoSelectionTool" + name());
            ATH_MSG_INFO("initialize - IsoSelectionTool initialized for " << m_far_fsr_isoWorkingPoint
                         << " IsoSelectionTool photon working point");
        }
        ASG_CHECK(m_isoSelTool.retrieve());


        // Return gracefully:
        return StatusCode::SUCCESS;
    }


    CP::CorrectionCode FsrPhotonTool::getFsrPhoton(const xAOD::IParticle* part, FsrCandidate& candidate,
                                                   const xAOD::PhotonContainer* photons,
                                                   const xAOD::ElectronContainer* electrons) 
    {

        std::vector<FsrCandidate>* cands = 
            (0 != photons && 0 != electrons) ? 
            getFsrCandidateList(part, photons, electrons) : getFsrCandidateList(part);

        ATH_MSG_DEBUG( "Fsr candidate size = " << cands->size() );

        // Return gracefully:
        if (cands->size() > 0) {
            candidate = cands->at(0);
            return CP::CorrectionCode::Ok;
        }
        candidate = FsrCandidate();
        ATH_MSG_DEBUG( "Fsr candidate  f1 = " << candidate.f1
                       << ", deltaR = " << candidate.deltaR
                       << ", Et = " << candidate.Et
                       << ", Eta = " << candidate.eta
                       << ", Phi = " << candidate.phi
                       << ", type = " <<candidate.type);
        return CP::CorrectionCode::Error;

    }
       

    std::vector<FsrCandidate>* 
    FsrPhotonTool::getFsrCandidateList(const xAOD::IParticle* part,
                                       const xAOD::PhotonContainer* photons,
                                       const xAOD::ElectronContainer* electrons) 
    {
        const xAOD::PhotonContainer* photons_cont = NULL;
        const xAOD::ElectronContainer* electrons_cont = NULL;

        if (0 == photons) {
            // Retrieve the photon container:
            if( evtStore()->retrieve( photons_cont, "Photons" ).isFailure() ) {
                ATH_MSG_WARNING( "No Photon Collection object could be retrieved" );
            }
        }
        else {
            // set the containers
            photons_cont   = photons;
        }
       
        if (0 == electrons) {
            // Retrieve the electron container:
            if( evtStore()->retrieve( electrons_cont, "Electrons" ).isFailure() ) {
                ATH_MSG_WARNING( "No Electron Collection object could be retrieved" );
            }
        }
        else {
            // set the containers
            electrons_cont = electrons;
        }

   	/// Search for far fsr for for electron and muon
   	/// Electron : far fsr is the only option  - return the result
   	/// Muon     : In case collinar fsr not found, check the far fsr
   
   	/// Set FSR type to far
   	m_fsr_type = FsrCandidate::FsrType::FsrUnknown;
   
   	const xAOD::Electron* electron = dynamic_cast<const xAOD::Electron*>(part);
   	if (electron) {
            return getFarFsrCandidateList(part, photons_cont);
   	}
   
   	const xAOD::Muon* muon = dynamic_cast<const xAOD::Muon*>(part);
   	if (muon) {
            std::vector<FsrCandidate>* nearlist = getNearFsrCandidateList(muon, photons_cont, electrons_cont);
            return (nearlist->size() > 0) ? nearlist : getFarFsrCandidateList(part, photons_cont);
   	}
   
   	ATH_MSG_WARNING( "The FSR search not done !" );
   	return NULL;
    }
    
    std::vector<FsrCandidate>* FsrPhotonTool::getFarFsrCandidateList(const xAOD::IParticle* part, 
                                                                     const xAOD::PhotonContainer* photons_cont)
    {
   	/// Set FSR type to far
   	m_fsr_type = FsrCandidate::FsrType::FsrFar;
   	std::vector< std::pair <const xAOD::IParticle*, double> > farFsrCandList;
   	farFsrCandList.clear();
   	farFsrCandList.reserve(photons_cont->size());
   
   	ATH_MSG_DEBUG( "In getFarFsrCandidateList function : photon size = " << photons_cont->size());
   
   	for (auto photon : *photons_cont) {
            bool is_tight_photon = false;
            photon->passSelection(is_tight_photon, "Tight");

            // old requirement to avoid topo cluster photons
            // if (   ((photon->author() == 4) || (photon->author() == 16))

            if ( (photon->p4().Et() > m_far_fsr_etcut) && is_tight_photon) {
                bool farPhIsoOK         = (bool)m_isoSelTool->accept(*photon);
                bool far_fsr_drcut_isOK = false;
                if (farPhIsoOK) {
                    double dr = deltaR(part->eta(), part->phi(), photon->eta(), photon->phi());
                    far_fsr_drcut_isOK = (dr > m_far_fsr_drcut);
                    if(far_fsr_drcut_isOK) farFsrCandList.push_back(std::make_pair(photon, dr));
                }
                    
                ATH_MSG_DEBUG( "Far Fsr candidate kinematics : author  " << photon->author()
                               << " Et = " << photon->p4().Et()
                               << " tight = " << is_tight_photon
                               << " farPhIsoOK = " << farPhIsoOK
                               << " far_fsr_drcut_isOK = " << far_fsr_drcut_isOK);
            }
   	}
   
   	/// return sorted list
   	return sortFsrCandidates(farFsrCandList);
    }


    std::vector<FsrCandidate>* FsrPhotonTool::getNearFsrCandidateList(const xAOD::Muon* muon, const xAOD::PhotonContainer* photons_cont, const xAOD::ElectronContainer* electrons_cont)
    {
   	/// Set FSR type to far
   	m_fsr_type = FsrCandidate::FsrType::FsrNear;
   	/// Start looking for collinar FSR for muons
   	/// Loop over photon candidates and choose the FSR photons
   	std::vector< std::pair <const xAOD::IParticle*, double> >  nearFsrCandList;
   
   	nearFsrCandList.reserve(photons_cont->size()+electrons_cont->size());
   	ATH_MSG_DEBUG( "In getNearFsrCandidateList function : photon size = " << photons_cont->size()
                       << ", electron size = " << electrons_cont->size());
   
   	for (auto photon : *photons_cont) {
            float photon_f1;
            photon->showerShapeValue(photon_f1, xAOD::EgammaParameters::f1);

            // Selection is tighter for photons below high_et_min
            bool high_et_photon = (photon->p4().Et() > m_high_et_min);
            if(  ( high_et_photon && (photon->p4().Et() > m_etcut) && (photon_f1 > m_f1cut) )
                 ||( !high_et_photon && (photon_f1 > m_topo_f1cut)
                     && (m_etcut < photon->p4().Et())) ) {
   
                double dr = deltaR(muon->eta(), muon->phi(), photon->eta(), photon->phi());
   
                // ph_cl_eta/phi should be used in duplicate
                if (   (!high_et_photon && dr < m_topo_drcut)
                       || (high_et_photon && dr < m_drcut)  ) {
                    nearFsrCandList.push_back(std::make_pair(photon, dr));
                    ATH_MSG_DEBUG( "Near Fsr candidates ( photon ) kinematics ; author  "
                                   << photon->author()
                                   << " Et = " << photon->p4().Et()
                                   << " f1 = " << photon_f1
                                   << " dr = " << dr );
                }
            }
   	}
   
   	unsigned int nofPhFsr = nearFsrCandList.size();
   
        // look for fsr photons in electron container
        for (auto electron : *electrons_cont)
            {
                // remove overlap with fsr already found
                if ( (nearFsrCandList.size() > 0) && isOverlap(electron, nearFsrCandList, nofPhFsr) ) continue;
   
                const xAOD::TrackParticle* electron_track = electron->trackParticle();
                const xAOD::TrackParticle* muon_track     = muon->primaryTrackParticle();
   
                bool elmutrackmatch = ( (fabs(electron_track->theta()- muon_track->theta()) < m_overlap_el_mu) &&
                                        (deltaPhi(electron_track->phi(),  muon_track->phi())   < m_overlap_el_mu) );
                float electron_f1;
                electron->showerShapeValue(electron_f1, xAOD::EgammaParameters::f1);

                // Get the corrected cluster energy:
                //  if the electron energy has been calibrated, we apply the change applied to the
                //  electron four vector assuming that the uncorrected energy is
                //  Eclus/cosh(track_eta). This will not work if an e-p combination has also been
                //  applied, and so the following will have to change.  RDS 04/2015.
                float eCorr = electron->p4().Et()/(electron->caloCluster()->e()/cosh(electron->trackParticle()->eta()));
                float clEt  = eCorr*electron->caloCluster()->et();

                ATH_MSG_VERBOSE( "Near Fsr candidate ( electron ) Et = " << clEt << " eCorr " << eCorr);
           
                // Allow topo-clusters to come in as electrons - apply f1 sliding window cut for Et
                // > 3.5 GeV and f1 topo cut for Et 1-3.5 GeV
                if( elmutrackmatch &&
                    ((clEt < m_high_et_min && m_etcut < clEt && electron_f1 > m_topo_f1cut ) ||
                     (m_high_et_min < clEt                   && electron_f1 > m_f1cut )) ) {

                    double dr = deltaR(muon->eta(), muon->phi(), electron->caloCluster()->eta(), electron->caloCluster()->phi());
   
                    if ( dr < m_drcut ) {
                        nearFsrCandList.push_back(std::make_pair(electron, dr));
                        ATH_MSG_DEBUG( "Near Fsr candidates ( electron ) kinematics : author  "
                                       << electron->author()
                                       << " Et = " << clEt
                                       << " f1 = " << electron_f1
                                       << " dr = " << dr );
                    }
                    else ATH_MSG_VERBOSE( "FAILED Near Fsr candidates ( electron ) kinematics : author  "
                                          << electron->author()
                                          << " Et = " << clEt
                                          << " f1 = " << electron_f1
                                          << " dr = " << dr );
                }
                // else {
                //     double dr = deltaR(muon->eta(), muon->phi(), electron->caloCluster()->eta(), electron->caloCluster()->phi());
   
                //     if (elmutrackmatch && dr < m_drcut ) {
                //     ATH_MSG_INFO( "FAILED Near Fsr candidates ( electron ) kinematics : author  "
                //                      << electron->author()
                //                      << " Et = " << clEt
                //                      << " f1 = " << electron_f1
                //                      << " dr = " << deltaR(muon->eta(), muon->phi(), electron->caloCluster()->eta(), electron->caloCluster()->phi())
                //                      << " theta/phi el/mu " << electron_track->theta() << "/" << muon->p4().Theta()
                //                      << "/" << electron_track->phi() << "/" << muon->phi()
                //                      << " theta/phi mu trk " << muon_track->theta() << "/" << muon_track->phi()
                //                   );
                //     }
                // }
            }
   
        return sortFsrCandidates(nearFsrCandList);
   
    }  // end of getFsrPhotons


    std::vector<FsrCandidate>* FsrPhotonTool::sortFsrCandidates(std::vector< std::pair <const xAOD::IParticle*, double> > FsrCandList, std::string option)
    {
   
        m_fsrPhotons.clear();
        m_fsrPhotons.reserve(FsrCandList.size());
   
        for (unsigned int i=0; i < FsrCandList.size(); i++ ) {
            FsrCandidate c;
            const xAOD::IParticle* particle = FsrCandList.at(i).first;
            const xAOD::Electron* electron = dynamic_cast<const xAOD::Electron*>(particle);
            const xAOD::Photon* photon = dynamic_cast<const xAOD::Photon*>(particle);
            float part_f1(-999);
            if ( electron ){
           	c.container =   "electron" ;
                // Use the cluster pt/eta/phi when considering the electron to be an FSR photon
                // setup accessor for electron cluster corrected energy

                // Get the corrected cluster energy:
                //  if the electron energy has been calibrated, we apply the change applied to the
                //  electron four vector assuming that the uncorrected energy is
                //  Eclus/cosh(track_eta). This will not work if an e-p combination has also been
                //  applied, and so the following will have to change.  RDS 04/2015.
                float eCorr = electron->p4().Et()/(electron->caloCluster()->e()/cosh(electron->trackParticle()->eta()));
                float clEt  = eCorr*electron->caloCluster()->et();
           	if ( electron->showerShapeValue(part_f1, xAOD::EgammaParameters::f1) ) c.f1 = part_f1; 
                c.Et  = clEt;
                c.eta = electron->caloCluster()->eta();
                c.phi = electron->caloCluster()->phi();
            }
            else if ( photon ) {
                c.container =   "photon";
           	if ( photon->showerShapeValue(part_f1, xAOD::EgammaParameters::f1) ) c.f1 = part_f1; 
                c.Et  = photon->p4().Et();
                c.eta = photon->eta();
                c.phi = photon->phi();
            }
            else {
                ATH_MSG_WARNING( "sortFsrCandidates: undefined particle - NOT electron nor photon. Should never get here!" );
           	c.container =   "";
                c.Et  = 0;
                c.eta = 0;
                c.phi = 0;
            }
            c.particle     = particle;
            c.deltaR       = FsrCandList.at(i).second;
            if(c.deltaR < 0.05) c.Et -= 400./cosh(particle->eta());
            c.type         = (c.deltaR > m_far_fsr_drcut) ? FsrCandidate::FsrFar : FsrCandidate::FsrNear;
            m_fsrPhotons.push_back(c);
        }
   
        // sort FSR candidates w.r.t min dR or ET
        if (option=="ET") std::sort(m_fsrPhotons.begin(), m_fsrPhotons.end(), compareEt);
        else std::sort(m_fsrPhotons.begin(), m_fsrPhotons.end());
        return &m_fsrPhotons;
    }

    bool FsrPhotonTool::isOverlap(const xAOD::Electron_v1* electron, std::vector< std::pair <const xAOD::IParticle*, double> > phfsr, unsigned int nofPhFsr)
    {
   	for (unsigned int indx=0; indx < nofPhFsr; indx++ ) {
            const xAOD::Photon* ph = dynamic_cast<const xAOD::Photon*>(phfsr.at(indx).first);
            const xAOD::CaloCluster* ph_cl = ph->caloCluster();
            const xAOD::CaloCluster* el_cl = electron->caloCluster();
            double dr = deltaR(el_cl->eta(), el_cl->phi(),
                               ph_cl->eta(), ph_cl->phi());
            ATH_MSG_VERBOSE( "isOverlap dr, ets " << (dr < m_overlap_el_ph) << "/" << dr << "/"
                             << ph_cl->et() << "/" << el_cl->et());
            if ( dr < m_overlap_el_ph ) return true;
   	}
   	return false;
    }
   
    double FsrPhotonTool::deltaPhi(float phi1, float phi2) const 
    {
        double dphi= fabs(phi1 - phi2);
        if (dphi > M_PI) dphi=2*M_PI-dphi;
        return dphi;
    }
    

    double FsrPhotonTool::deltaR(float meta, float mphi, float peta, float pphi) const {
   
        double deta = fabs(meta - peta);
        double dphi = deltaPhi(mphi, pphi);
        double dR   = sqrt((dphi*dphi)+(deta*deta));
   
        return dR;
    }

} // namespace FSR
