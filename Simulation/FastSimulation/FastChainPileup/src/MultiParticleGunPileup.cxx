/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MultiPY8Pileup.cxx - extension of GenModule_i to generate multiple pileup
// events in one go for fast simulation chain
// Richard Hawkings, started 20/2/15, + contributions from Vladimir Lyubushkin

#include <algorithm>
#include <numeric>
#include <functional>
#include <iterator>


#include "CLHEP/Random/RandPoisson.h"
#include "FastChainPileup/MultiParticleGunPileup.h"

MultiParticleGunPileup::MultiParticleGunPileup(const std::string& name, ISvcLocator* pSvcLocator) :
  GenModule( name, pSvcLocator ),
  m_ngen(0),
  m_nbad(0),
  // m_evnumber(1),
  m_randomEngine(NULL),
  m_file(NULL),
  m_htgPileupProfile(NULL),
  m_htgPileupMu(NULL),
  m_htgPileupEvents(NULL),
  m_partSampler(NULL)
{
  declareProperty("NCollPerEvent",m_ncollevent=20,"Collisons per event (-1 to use profile)");
  declareProperty("PileupProfile",m_pileupProfile,"Pileup profile array");
  declareProperty("MultBCID",m_multbcid,"Pileup multiplier per BCID array");
  declareProperty("HistFile",m_filename="","Filename for histograms");
}

MultiParticleGunPileup::~MultiParticleGunPileup() {

}

StatusCode MultiParticleGunPileup::genInitialize() {
  ATH_MSG_INFO( "In MultiParticleGunPileup::genInitialize" );

  // should be initialized after Pythia8_i::genInitialize() ???
  // m_randomEngine = atRndmGenSvc().GetEngine( Pythia8_i::pythia_stream );

  // if filename set, initialise histograms for mu/nevt profiles
  if (m_filename!="") {
    m_file = new TFile( m_filename.c_str(), "recreate" );
    m_file->cd();
    m_htgPileupProfile = new TH1D( "pileupProfile", "input pileupProfile", 100, 0., 100. );
    m_htgPileupMu = new TH1D( "generatedMu", "generated pileup mu", 100, 0., 100. );
    m_htgPileupEvents = new TH1D( "generatedEvents", "generated pileup events", 100, 0., 100. );
  }

  m_pileupProfileIntegral.clear();
  m_pileupProfileIntegral.reserve( m_pileupProfile.size() );
  for ( std::vector< int >::iterator itr = m_pileupProfile.begin(); itr != m_pileupProfile.end(); ++itr ) {
    m_pileupProfileIntegral.push_back( std::accumulate( m_pileupProfile.begin(), itr + 1, 0. ) );
    if (m_file)
      m_htgPileupProfile->SetBinContent( std::distance( m_pileupProfile.begin(), itr ) + 1, *itr );
  }

  if ( m_ncollevent > 0 ) {
    ATH_MSG_INFO("Generate fixed number of " << m_ncollevent << " minbias collisions per bunch crossing");
  }
  else {
    ATH_MSG_INFO("Generate varying minbias collisions per event according to given pileup profile");
  }
  m_evts.reserve( 50 );

  ATH_MSG_INFO("Simulate " << m_multbcid.size() << " bunch crossings per event");
  int ibc=1;
  for (std::vector<float>::const_iterator itr=m_multbcid.begin();
       itr!=m_multbcid.end();++itr,++ibc) {
    ATH_MSG_INFO("Bunch crossing BCID " << ibc << " mu multiplier " << *itr);
  }

  //Initialize the ParticleGun generator
  m_partSampler = new ParticleSampler(new CyclicSeqSampler("-11,11,"), new PtEtaMPhiSampler(10000.,10000.,-3,3),1);

  // exit via the base class initialisation
  return StatusCode::SUCCESS;
}

StatusCode MultiParticleGunPileup::callGenerator() {
  // initialise random generators the first time - delayed after Pythia init
  if ( !m_randomEngine)
    m_randomEngine = atRndmGenSvc().GetEngine( "ParticleGun" );

  // decide how many events to generate

  int muval = ( m_ncollevent > 0 ? m_ncollevent : nPileupEvents() );
  if (m_file) m_htgPileupMu->Fill(muval+0.1);
  m_evts.clear();

  // loop over all BCID - starting from 1 for in-time pileup
  int bcid=1;
  for (std::vector<float>::const_iterator bcitr=m_multbcid.begin();
       bcitr!=m_multbcid.end();++bcitr,++bcid) {
    // actual number of minbias in this BCID
    // Poisson around instantaneous_mu * multiplier value
    const int nevtraw=CLHEP::RandPoisson::shoot(muval);
    if (m_file) m_htgPileupEvents->Fill(nevtraw+0.1);
    const int nevt=(float)nevtraw*(*bcitr);

    for (int ievt=0;ievt<nevt;++ievt) {
      ATH_MSG_DEBUG("Request generation of event " << ievt << " of " << nevt
		    << " for BCID " << bcid);

//--------------------------- CALL GUN HERE
      HepMC::GenEvent* evt=new HepMC::GenEvent();
      evt->weights().push_back(1.0);
      //evt->set_event_number(ievt); //Maybe dangerous to do this since the first event gets stacked last
      // Make and fill particles
      std::vector<SampledParticle> particles = m_partSampler->shoot();
      for (auto p : particles){
	// Debug printout of particle properties
	std::cout << ievt << " DEBUG0," <<  p.m_pid << ", " << p.m_mom.E()<< ", " << p.m_mom.Pt()<< ", " << p.m_mom.M() << std::endl;
	std::cout << ievt << " DEBUG1 (px,py,pz,E) = (" << p.m_mom.Px()<< ", " << p.m_mom.Py()<< ", " << p.m_mom.Pz()<< ", " << p.m_mom.E() <<")" << std::endl;
	std::cout << ievt << " DEBUG2 (eta,phi,pt,m) = ()" << p.m_mom.Eta()<< ", " << p.m_mom.Phi()<< ", " << p.m_mom.Pt()<< ", " << p.m_mom.M() <<")" << std::endl;
	std::cout << ievt << " DEBUG3 (x,y,z,t) = ()" << p.m_pos.X()<< ", " << p.m_pos.Y()<< ", " << p.m_pos.Z()<< ", " << p.m_pos.T() << ")" << std::endl;
	// Make particle-creation vertex
	// TODO: do something cleverer than one vertex per particle?
	HepMC::FourVector pos(p.m_pos.X(), p.m_pos.Y(), p.m_pos.Z(), p.m_pos.T());
	HepMC::GenVertexPtr   gv = new HepMC::GenVertex(pos);
	evt->add_vertex(gv);
	// Make particle with status == 1
	HepMC::FourVector mom(p.m_mom.Px(), p.m_mom.Py(), p.m_mom.Pz(), p.m_mom.E());
	HepMC::GenParticlePtr   gp = new HepMC::GenParticle;
	gp->set_status(1);
	gp->set_pdg_id(p.m_pid);
	gp->set_momentum(mom);
	if (p.m_mass)
	  gp->set_generated_mass(p.m_mass);
	gv->add_particle_out(gp);
      }
//-----------------------------------------

      // change the process ID to incorporate the BCID * 10000
      int pid=evt->signal_process_id();
      evt->set_signal_process_id(pid+10000*bcid);
      ATH_MSG_DEBUG("Signal process ID " << pid << " set to " <<
		    evt->signal_process_id() << " for BCID " << bcid);
      m_evts.push_back(evt);
      ++m_ngen;
    }
  }
  ATH_MSG_DEBUG("callGenerator finished with " << m_evts.size() <<
		" pileup events in buffer");
  return StatusCode::SUCCESS;
}

StatusCode MultiParticleGunPileup::fillEvt(HepMC::GenEvent* evt) {
  int nbuf=m_evts.size();
  if (nbuf>1) {
    // send extra events into McEventCollection via backdoor
    for (int i=1;i<nbuf;++i) {
      events()->push_back(m_evts[i]);
    }
  }
  // send first event back via usual method
  // have to copy it to the passed in GenEvent
  if (nbuf>0) {
    *evt=*m_evts[0];
    delete m_evts[0];
  }
  return StatusCode::SUCCESS;
}

StatusCode MultiParticleGunPileup::genFinalize() {
  ATH_MSG_INFO("MultiParticleGunPileup finished with " << m_ngen << " good events and " << m_nbad <<  " failures" );

  if (m_file) {
    m_file->cd();
    m_file->Write();
    m_file->Close();
  }

  return StatusCode::SUCCESS;
}


int MultiParticleGunPileup::nPileupEvents() {
  // return the instantaneous mu value according to the profile
  double threshold = m_randomEngine->flat() * m_pileupProfileIntegral.back();
  std::vector< double >::iterator itr = std::lower_bound( m_pileupProfileIntegral.begin(), m_pileupProfileIntegral.end(), threshold );
  return std::distance( m_pileupProfileIntegral.begin(), itr );
}
