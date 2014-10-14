/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>

// Local include(s):
#include "FsrUtils/FsrPhotonTool.h"


namespace FSR {

   FsrPhotonTool::FsrPhotonTool( const std::string& name )
      : asg::AsgTool( name ) {

	   declareProperty( "high_et_min", m_high_et_min = 3500. );
	   declareProperty( "overlap_el_ph", m_overlap_el_ph = 0.01 );
	   declareProperty( "overlap_el_mu", m_overlap_el_mu = 0.001 );
	   declareProperty( "far_fsr_drcut", m_far_fsr_drcut = 0.15 );
	   declareProperty( "far_fsr_etcut", m_far_fsr_etcut = 10000.0 );
	   declareProperty( "far_fsr_etconecut", m_far_fsr_etconecut = 4000.0 );
	   declareProperty( "drcut", m_drcut = 0.15 );
	   declareProperty( "etcut", m_etcut = 1000.0 );
	   declareProperty( "f1cut", m_f1cut =  0.1 );
	   declareProperty( "topo_drcut", m_topo_drcut =  0.08 );
	   declareProperty( "topo_f1cut", m_topo_f1cut = 0.2 );

   }

   StatusCode FsrPhotonTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising..." );

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
                << ", topoEtcone40 = " << candidate.topoEtcone40
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

       if (0 == photons) {
           // Retrieve the photon container:
           if( evtStore()->retrieve( m_photons, "PhotonCollection" ).isFailure() ) {
               ATH_MSG_WARNING( "No PhotonCollection object could be retrieved" );
           }
       }
       else {
           // set the containers
           m_photons   = photons;
       }
       
       if (0 == electrons) {
           // Retrieve the electron container:
           if( evtStore()->retrieve( m_electrons, "ElectronCollection" ).isFailure() ) {
               ATH_MSG_WARNING( "No ElectronCollection object could be retrieved" );
           }
       }
       else {
           // set the containers
           m_electrons = electrons;
       }

   	/// Search for far fsr for for electron and muon
   	/// Electron : far fsr is the only option  - return the result
   	/// Muon     : In case collinar fsr not found, check the far fsr
   
   	/// Set FSR type to far
   	fsr_type = FsrCandidate::FsrType::FsrUnknown;
   
   	const xAOD::Electron* electron = dynamic_cast<const xAOD::Electron*>(part);
   	if (electron) {
   		return getFarFsrCandidateList(part);
   	}
   
   	const xAOD::Muon* muon = dynamic_cast<const xAOD::Muon*>(part);
   	if (muon) {
   		std::vector<FsrCandidate>* nearlist = getNearFsrCandidateList(part);
   		return (nearlist->size() > 0) ? nearlist : getFarFsrCandidateList(part);
   	}
   
   	ATH_MSG_WARNING( "The FSR search not done !" );
   	return NULL;
   }
    
   std::vector<FsrCandidate>* FsrPhotonTool::getFarFsrCandidateList(const xAOD::IParticle* part)
   {
   	/// Set FSR type to far
   	fsr_type = FsrCandidate::FsrType::FsrFar;
   	std::vector< std::pair <const xAOD::IParticle*, double> > farFsrCandList;
   	farFsrCandList.clear();
   	farFsrCandList.reserve(m_photons->size());
   
   	ATH_MSG_DEBUG( "In getFarFsrCandidateList function : photon size = " << m_photons->size());
   
   	for (auto photon : *m_photons) {
   		bool is_tight_photon = false;
   		photon->passSelection(is_tight_photon, "Tight");
   
   		float topoetcone40;
   		photon->isolationValue(topoetcone40, xAOD::Iso::topoetcone40);
   
   		if (   ((photon->author() == 4) || (photon->author() == 16))
   			&& (photon->p4().Et() > m_far_fsr_etcut)
   			&& is_tight_photon
   			&& (topoetcone40 < m_far_fsr_etconecut) ) {
   
   			double dr = deltaR(part->eta(), part->phi(), photon->eta(), photon->phi());
   
   			ATH_MSG_DEBUG( "Far Fsr candidate kinematics : aouthor  " << photon->author()
   								<< " Et = " << photon->p4().Et()
   								<< " tight = " << is_tight_photon
   								<< " etcone = " << topoetcone40
                                     );
   			if(dr > m_far_fsr_drcut) {
   				farFsrCandList.push_back(std::make_pair(photon, dr));
   			}
   		}
   	}
   
   	/// return sorted list
   	return sortFsrCandidates(farFsrCandList);
   }


   std::vector<FsrCandidate>* FsrPhotonTool::getNearFsrCandidateList(const xAOD::IParticle* part)
   {
   	/// Set FSR type to far
   	fsr_type = FsrCandidate::FsrType::FsrNear;
   	/// Start looking for collinar FSR for muons
   	/// Loop over photon candidates and choose the FSR photons
   	std::vector< std::pair <const xAOD::IParticle*, double> >  nearFsrCandList;
   
   	nearFsrCandList.reserve(m_photons->size()+m_electrons->size());
   	ATH_MSG_DEBUG( "In getNearFsrCandidateList function : photon size = " << m_photons->size()
   						<< ", electron size = " << m_electrons->size());
   
   	for (auto photon : *m_photons) {
   		float photon_f1;
   		photon->showerShapeValue(photon_f1, xAOD::EgammaParameters::f1);
   
   		bool std_photon_author = (photon->author() == 4 || photon->author()==16 );
   		if(  (    std_photon_author  && (photon->p4().Et() > m_high_et_min)
   				&& (photon->p4().Et() > m_etcut) && (photon_f1 > m_f1cut) )
   		   ||(    (photon->author() == 128) && (photon_f1 > m_topo_f1cut)
   			   && ((m_etcut < photon->p4().Et()) && (photon->p4().Et() <= m_high_et_min)) )  ) {
   
   			double dr = deltaR(part->eta(), part->phi(), photon->eta(), photon->phi());
   
   			// ph_cl_eta/phi should be used in duplicate
   			if (   (photon->author() == 128 && dr < m_topo_drcut)
   			    || (std_photon_author && dr < m_drcut)  ) {
   				nearFsrCandList.push_back(std::make_pair(photon, dr));
   				ATH_MSG_DEBUG( "Near Fsr candidates ( photon ) kinematics ; aouthor  " << photon->author()
   												<< " Et = " << photon->p4().Et()
   												<< " f1 = " << photon_f1
   												<< " dr = " << dr );
   			}
   		}
   	}
   
   	unsigned int nofPhFsr = nearFsrCandList.size();
   
       // look for fsr photons in electron container
       for (auto electron : *m_electrons)
       {
           // remove overlap with fsr already found
           if ( (nearFsrCandList.size() > 0) && isOverlap(electron, nearFsrCandList, nofPhFsr) ) continue;
   
           const xAOD::TrackParticle* electron_track = electron->trackParticle();
   
           bool elmutrackmatch = (    (fabs(electron_track->theta()-part->p4().Theta()) < m_overlap_el_mu)
                                   && (fabs(electron_track->phi()-part->phi())     < m_overlap_el_mu) );
           float electron_f1;
           electron->showerShapeValue(electron_f1, xAOD::EgammaParameters::f1);
   
           if(    (elmutrackmatch) && (electron->p4().Et() > m_etcut) && (electron->p4().Et() > m_high_et_min)
               && ( electron_f1 > m_f1cut ) ) {
               double dr = deltaR(part->eta(), part->phi(), electron->eta(), electron->phi());
   
               if ( dr < m_drcut ) {
               	nearFsrCandList.push_back(std::make_pair(electron, dr));
               	ATH_MSG_DEBUG( "Near Fsr candidates ( electron ) kinematics : aouthor  "
               			        << electron->author()
   								<< " Et = " << electron->p4().Et()
   								<< " f1 = " << electron_f1
   								<< " dr = " << dr );
               }
           }
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
           float part_f1, part_isolation;
           if ( electron )
           {
           	c.container =   "electron" ;
           	electron->showerShapeValue(part_f1, xAOD::EgammaParameters::f1);
           	electron->isolationValue(part_isolation, xAOD::Iso::topoetcone40);
           } else {
           	c.container =   "photon";
           	photon->showerShapeValue(part_f1, xAOD::EgammaParameters::f1);
           	photon->isolationValue(part_isolation, xAOD::Iso::topoetcone40);
           }
           c.f1 = part_f1;
           c.topoEtcone40 = part_isolation;
           c.particle     = particle;
           c.deltaR       = FsrCandList.at(i).second;
           if(c.deltaR < 0.05) c.Et = particle->p4().Et()-(400./cosh(particle->eta()));
           c.eta = particle->eta();
           c.phi = particle->phi();
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
   		double dr = deltaR(electron->eta(), electron->phi(), ph_cl->eta(), ph_cl->phi());
   		if ( dr < m_overlap_el_ph ) return true;
   	}
   	return false;
   }
   
   double FsrPhotonTool::deltaR(float meta, float mphi, float peta, float pphi) {
   
       double deta= fabs(meta - peta);
       double dphi= fabs(mphi - pphi);
       if (dphi > M_PI) dphi=2*M_PI-dphi;
       double dR=sqrt((dphi*dphi)+(deta*deta));
   
       return dR;
   }

} // namespace FSR
