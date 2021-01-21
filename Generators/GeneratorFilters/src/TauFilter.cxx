/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration 
*/
#include "GeneratorFilters/TauFilter.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandomEngine.h"

TauFilter::TauFilter( const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter( name,pSvcLocator ),
    m_rand("AtRndmGenSvc", name),
    m_eventse(0), m_eventsmu(0), m_eventshad(0), 
    m_eventseacc(0), m_eventsmuacc(0), m_eventshadacc(0)
{
  declareProperty( "Ntaus", m_Ntau = 1 );
  declareProperty( "MaxNtaus", m_maxNtau = 100 );
  declareProperty( "EtaMaxe", m_etaMaxe = 2.5 );
  declareProperty( "EtaMaxmu", m_etaMaxmu = 2.5 );
  declareProperty( "EtaMaxhad", m_etaMaxhad = 2.5 );

  declareProperty( "Ptcute", m_pTmine = 12000.0 );
  declareProperty( "Ptcutmu", m_pTminmu = 12000.0 );
  declareProperty( "Ptcuthad", m_pTminhad = 12000.0 );

  // new options:
  declareProperty( "UseNewOptions", m_NewOpt = false );
  declareProperty( "UseMaxNTaus", m_useMaxNTaus = false );
  declareProperty( "Nhadtaus", m_Nhadtau = 0 );
  declareProperty( "MaxNhadtaus", m_maxNhadtau = 100 );
  declareProperty( "Nleptaus", m_Nleptau = 0 );
  declareProperty( "MaxNleptaus", m_maxNleptau = 100 );
  declareProperty( "EtaMaxlep", m_etaMaxlep = 2.6 );
  declareProperty( "Ptcutlep", m_pTminlep = 7000.0 );
  declareProperty( "Ptcutlep_lead", m_pTminlep_lead = 7000.0);
  declareProperty( "Ptcuthad_lead", m_pTminhad_lead = 12000.0 );
  declareProperty( "ReverseFilter", m_ReverseFilter = false);
  
  declareProperty( "HasTightRegion", m_HasTightRegion = false);
  declareProperty( "LooseRejectionFactor", m_LooseRejectionFactor = 1);
  declareProperty( "Ptcutlep_tight", m_pTminlep_tight = 7000.0 );
  declareProperty( "Ptcutlep_tight_lead", m_pTminlep_tight_lead = 7000.0 );
  declareProperty( "Ptcuthad_tight", m_pTminhad_tight = 12000.0 );
  declareProperty( "Ptcuthad_tight_lead", m_pTminhad_tight_lead = 12000.0 );

  declareProperty( "filterEventNumber", m_filterEventNumber = 0 );
}


StatusCode TauFilter::filterInitialize() {
  m_eventse = 0;
  m_eventsmu = 0;
  m_eventshad = 0;
  m_eventseacc = 0;
  m_eventsmuacc = 0;
  m_eventshadacc = 0;

  for(int i=0; i<6; i++) {
    m_events[i] = 0; m_events_sel[i] = 0;
  }

  if (m_rand.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve AtRndmGenSvc " << m_rand);
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}


StatusCode TauFilter::filterFinalize() {
  if(m_NewOpt) {
    ATH_MSG_INFO("Sum of Events total: " << m_events[0] << " , selected: " << m_events_sel[0]);
    ATH_MSG_INFO("Sum of Events with pos. weights total: " << m_events[1] << " , selected: " << m_events_sel[1]);
    ATH_MSG_INFO("Sum of Events with neg. weights total: " << m_events[2] << " , selected: " << m_events_sel[2]);
    ATH_MSG_INFO("Sum of weights total: " << m_events[3] << " , selected: " << m_events_sel[3]);
    ATH_MSG_INFO("Sum of pos. weights total: " << m_events[4] << " , selected: " << m_events_sel[4]);
    ATH_MSG_INFO("Sum of neg. weights total: " << m_events[5] << " , selected: " << m_events_sel[5]);
  }
  else {
    ATH_MSG_INFO(" , e: " << m_eventse << " , mu: " << m_eventsmu << " , had: " << m_eventshad <<
		 " , eacc: " << m_eventseacc << " , muacc: " << m_eventsmuacc << " , hadacc: " << m_eventshadacc);
  }

  return StatusCode::SUCCESS;
}


CLHEP::HepLorentzVector TauFilter::sumDaughterNeutrinos( HepMC::GenParticle *part ) {
  CLHEP::HepLorentzVector nu( 0, 0, 0, 0);
  if ( ( (abs( part->pdg_id() ) == 12 ) || ( abs( part->pdg_id() ) == 14 ) || ( abs( part->pdg_id() ) == 16 )) 
       && part->status() != 3) {
    nu.setPx(part->momentum().px());
    nu.setPy(part->momentum().py());
    nu.setPz(part->momentum().pz());
    nu.setE(part->momentum().e());
  }

  if (part->end_vertex() == 0) return nu;

  HepMC::GenVertex::particles_out_const_iterator begin = part->end_vertex()->particles_out_const_begin();
  HepMC::GenVertex::particles_out_const_iterator end = part->end_vertex()->particles_out_const_end();
  for ( ; begin != end; begin++ ) nu += sumDaughterNeutrinos( *begin );

  return nu;
}


StatusCode TauFilter::filterEvent() {
  // Get random number engine
  CLHEP::HepRandomEngine* rndm(0);
  if(m_HasTightRegion) {
    rndm = m_rand->GetEngine("TauFilter");
    if (!rndm) {
      ATH_MSG_ERROR("Failed to retrieve random number engine for TauFilter");
      setFilterPassed(false);
      return StatusCode::SUCCESS;
    }
  }
  
  HepMC::GenParticle *tau;
  CLHEP::HepLorentzVector mom_tauprod;   // will contain the momentum of the products of the tau decay
  CLHEP::HepLorentzVector tauvis;
  CLHEP::HepLorentzVector nutau;
  tau = 0;
  int ntau = 0;

  double ptlep_max = 0;
  double pthad_max = 0;
  int ntaulep = 0;
  int ntauhad = 0;
  int ntaulep_tight = 0;
  int ntauhad_tight = 0;
  double weight = 1;

  McEventCollection::const_iterator itr;
  for (itr = events_const()->begin(); itr!=events_const()->end(); ++itr) {
    int eventNumber = (*itr)->event_number();
    //ATH_MSG_INFO("eventNumber = " << eventNumber);
    if(m_filterEventNumber==1 && (eventNumber%2)==0) {
      setFilterPassed(false);
      return StatusCode::SUCCESS;
    }
    else if(m_filterEventNumber==2 && (eventNumber%2)==1) {
      setFilterPassed(false);
      return StatusCode::SUCCESS;
    }
    
    const HepMC::GenEvent* genEvt = (*itr);
    HepMC::WeightContainer wgtsC = genEvt->weights();
    weight = wgtsC.size() > 0 ? wgtsC[0] : 1;

    for (HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr) {
      // Look for the first tau with genstat != 3
      if (abs((*pitr)->pdg_id()) == 15 && (*pitr)->status() != 3) {
        tau = (*pitr);
        ATH_MSG_DEBUG("found tau with barcode " << tau->barcode() << " status " << (*pitr)->status());
        ATH_MSG_DEBUG("pT\t\teta\tphi\tid");
        ATH_MSG_DEBUG(tau->momentum().perp() << "\t" <<
                      tau->momentum().eta() << "\t" <<
                      tau->momentum().phi() << "\t" <<
                      tau->pdg_id() << "\t");

        HepMC::GenVertex::particles_out_const_iterator begin = tau->end_vertex()->particles_out_const_begin();
        HepMC::GenVertex::particles_out_const_iterator end = tau->end_vertex()->particles_out_const_end();
        int tauType = 0; 
        //TauType initialized as 0. tauType = 1 is an Tau_el, tauType = 2 is an Tau_mu, tauType = 0 is an Tau_had, tauType = 11 is a tau with tau parent, e.g a photon radiation event.
        
        for ( ; begin != end; begin++ ) {
          if ( (*begin)->production_vertex() != tau->end_vertex() ) continue; 
         
          else if ( abs( (*begin)->pdg_id() ) == 12 ) tauType = 1;   //Tau decays into an electron
         
          else if ( abs( (*begin)->pdg_id() ) == 14 ) tauType = 2;   //Tau decays into an muon
                  	
          else if ( abs( (*begin)->pdg_id() ) == 15 ) tauType = 11;  //Tau radiates a particle and decays into another tau
        
				}

				if (tauType == 11) {
          ATH_MSG_DEBUG("tau has a tau as daughter - skipping");
          continue;
        }
        nutau = sumDaughterNeutrinos(tau);

        ATH_MSG_DEBUG("pT\t\teta\tphi\tlh");
        ATH_MSG_DEBUG(nutau.perp() << "\t" << nutau.eta() << "\t" << nutau.phi() << "\t" << tauType);

        tauvis.setPx(tau->momentum().px()-nutau.px());
        tauvis.setPy(tau->momentum().py()-nutau.py());
        tauvis.setPz(tau->momentum().pz()-nutau.pz());
        tauvis.setE(tau->momentum().e()-nutau.e());

        ATH_MSG_DEBUG(tauvis.perp() << "\t" << tauvis.eta() << "\t" << tauvis.phi() << "\t" << tauType);

        if ( tauType == 1 ) {  
					if(!m_NewOpt) {
						m_eventse++;
						if ( tauvis.perp() < m_pTmine ) continue;
						if ( fabs( tauvis.eta() ) > m_etaMaxe ) continue;
						ntau++;
						m_eventseacc++;
					}
					else {
						if ( tauvis.perp() < m_pTminlep ) continue;
						if ( fabs( tauvis.eta() ) > m_etaMaxlep ) continue;
						ntaulep++;
						if ( tauvis.perp() >= ptlep_max ) ptlep_max = tauvis.perp();
						if ( tauvis.perp() >= m_pTminlep_tight ) ntaulep_tight++;
					}
        }
         
        else if ( tauType == 2 ) {
					if(!m_NewOpt) {
						m_eventsmu++;
						if ( tauvis.perp() < m_pTminmu ) continue;
						if ( fabs( tauvis.eta() ) > m_etaMaxmu ) continue;
						ntau++;
						m_eventsmuacc++;
					}
					else {
						if ( tauvis.perp() < m_pTminlep ) continue;
						if ( fabs( tauvis.eta() ) > m_etaMaxlep ) continue;
						ntaulep++;
						if ( tauvis.perp() >= ptlep_max ) ptlep_max = tauvis.perp();
						if ( tauvis.perp() >= m_pTminlep_tight ) ntaulep_tight++;
					}
        } 
        
        else if ( tauType == 0 ) {
          m_eventshad++;
          if ( tauvis.perp() < m_pTminhad ) continue;
          if ( fabs( tauvis.eta() ) > m_etaMaxhad ) continue;
          ntau++;
          m_eventshadacc++;
					ntauhad++;
					if ( tauvis.perp() >= pthad_max ) pthad_max = tauvis.perp();
					if ( tauvis.perp() >= m_pTminhad_tight ) ntauhad_tight++;
        }
        
        else {
        	 ATH_MSG_DEBUG("Could not find a tauType! Something went wrong.");
        	 std::cout << std::endl << std::endl <<"************    COULD NOT FIND A TAU TYPE   *****************" << std::endl << std::endl;
        }	 
      }
    }
  }

  bool pass1 = ( ntaulep + ntauhad >= m_Ntau    //For use with UseNewOptions
		 && ntaulep >= m_Nleptau
		 && ntauhad >= m_Nhadtau
		 && (ntaulep<2 || ptlep_max>=m_pTminlep_lead)
		 && (ntauhad<2 || pthad_max>=m_pTminhad_lead)
		 );
  bool pass2 = ( ntaulep_tight + ntauhad_tight >= m_Ntau   //For use with UseNewOptions + HssTightRegion
		 && ntaulep_tight >= m_Nleptau
		 && ntauhad_tight >= m_Nhadtau
		 && (ntaulep_tight<2 || ptlep_max>=m_pTminlep_tight_lead)
		 && (ntauhad_tight<2 || pthad_max>=m_pTminhad_tight_lead)
		 );
	
	bool pass3 = (ntaulep + ntauhad >= m_Ntau    //For use with UseNewOptions + UseMaxNTaus
		 && ntaulep + ntauhad <= m_maxNtau	
		 && ntaulep >= m_Nleptau
		 && ntauhad >= m_Nhadtau
		 && ntaulep <= m_maxNleptau
		 && ntauhad <= m_maxNhadtau
		 );
		 
	bool pass = false;  //Initializing the pass variable that will be checked to se if the filter is passed.
	
	if (m_NewOpt) pass = pass1;
	if (m_useMaxNTaus) pass = pass3;
		
	double extra_weight = 1;
  if(m_NewOpt && m_HasTightRegion) {
    if (pass2) {
      pass = pass2;
      extra_weight = 1/m_LooseRejectionFactor;
      weight *= extra_weight;
    }
    else if (pass1) {
      double rnd = rndm->flat(); // a random number between (0,1)
      if(rnd > 1/m_LooseRejectionFactor) pass = false;
      else pass = true;
    }
    else pass = false;
  }

  m_events[0]++;
  m_events[3] += weight;
  if(weight>=0) {
    m_events[1]++;
    m_events[4] += weight;
  }
  else {
    m_events[2]++;
    m_events[5] += weight;
  }

  if(pass) {
    m_events_sel[0]++;
    m_events_sel[3] += weight;
    if(weight>=0) {
      m_events_sel[1]++;
      m_events_sel[4] += weight;
    }
    else {
      m_events_sel[2]++;
      m_events_sel[5] += weight;
    }
  }

  if(m_NewOpt && m_HasTightRegion) {
    // Get MC event collection for setting weight
    const DataHandle<McEventCollection> mecc = 0;
    if ( evtStore()->retrieve( mecc ).isFailure() || !mecc ){
      setFilterPassed(false);
      ATH_MSG_ERROR("Could not retrieve MC Event Collection - weight might not work");
      return StatusCode::SUCCESS;
    }

    // Event passed.  Will weight events
    McEventCollection* mec = const_cast<McEventCollection*> (&(*mecc));
    for (unsigned int i = 0; i < mec->size(); ++i) {
      if (!(*mec)[i]) continue;
      double existingWeight = (*mec)[i]->weights().size()>0 ? (*mec)[i]->weights()[0] : 1.;
      if ((*mec)[i]->weights().size()>0) {
	(*mec)[i]->weights()[0] = existingWeight*extra_weight;
      } else {
	(*mec)[i]->weights().push_back( existingWeight*extra_weight );
      }
    }
  }

	if(!m_NewOpt) pass = (ntau >= m_Ntau);  //If UseNewOptions is not enabled, only look at total number of taus present.
	
	if (m_ReverseFilter) pass = !pass; //If reverse filter is active, flip the truth value of pass
	
	setFilterPassed(pass);
  
  return StatusCode::SUCCESS;
}
