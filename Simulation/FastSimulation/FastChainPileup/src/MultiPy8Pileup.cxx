/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MultiPY8Pileup.cxx - extension of Pythia8_i to generate multiple pileup
// events in one go for fast simulation chain
// Richard Hawkings, started 20/2/15, + contributions from Vladimir Lyubushkin

#include <algorithm>
#include <numeric>
#include <functional>
#include <iterator>

#include "CLHEP/Random/RandPoisson.h"
#include "FastChainPileup/MultiPy8Pileup.h"

MultiPy8Pileup::MultiPy8Pileup(const std::string& name, ISvcLocator* pSvcLocator) :
  Pythia8_i::Pythia8_i( name, pSvcLocator ),
  m_ngen(0),
  m_nbad(0),
  // m_evnumber(1),
  m_randomEngine(NULL),
  m_file(NULL),
  m_htgPileupProfile(NULL),
  m_htgPileupMu(NULL),
  m_htgPileupEvents(NULL)
{
  declareProperty("NCollPerEvent",m_ncollevent=20,"Collisons per event (-1 to use profile)");
  declareProperty("PileupProfile",m_pileupProfile,"Pileup profile array");
  declareProperty("MultBCID",m_multbcid,"Pileup multiplier per BCID array");
  declareProperty("HistFile",m_filename="","Filename for histograms");
}

MultiPy8Pileup::~MultiPy8Pileup() {

}

StatusCode MultiPy8Pileup::genInitialize() {
  ATH_MSG_INFO( "In MultiPy8Pileup::genInitialize" );

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

  // exit via the base class initialisation
  return Pythia8_i::genInitialize();
}

StatusCode MultiPy8Pileup::callGenerator() {
  // initialise random generators the first time - delayed after Pythia init
  if ( !m_randomEngine)
    m_randomEngine = atRndmGenSvc().GetEngine( Pythia8_i::pythia_stream );

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
      StatusCode sc=Pythia8_i::callGenerator();
      if (sc==StatusCode::SUCCESS) {
	HepMC::GenEvent* evt=new HepMC::GenEvent();
	if (Pythia8_i::fillEvt(evt)==StatusCode::SUCCESS) {
	  // change the process ID to incorporate the BCID * 10000
	  int pid=HepMC::signal_process_id(evt);
	  HepMC::set_signal_process_id(evt,pid+10000*bcid);
	  ATH_MSG_DEBUG("Signal process ID " << pid << " set to " <<
			HepMC::signal_process_id(evt) << " for BCID " << bcid);
	  m_evts.push_back(evt);
	  ++m_ngen;
	} else {
	  ATH_MSG_WARNING("Failed to fillEvt for event seq " << ievt);
	  ++m_nbad;
	}
      } else {
	++m_nbad;
	ATH_MSG_WARNING("Failed to generate event seq " << ievt);
      }
    }
  }
  ATH_MSG_DEBUG("callGenerator finished with " << m_evts.size() <<
		" pileup events in buffer");
  return StatusCode::SUCCESS;
}

StatusCode MultiPy8Pileup::fillEvt(HepMC::GenEvent* evt) {
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

StatusCode MultiPy8Pileup::genFinalize() {
  ATH_MSG_INFO("MultiPy8Pileup finished with " << m_ngen << " good events and " << m_nbad <<  " failures" );

  if (m_file) {
    m_file->cd();
    m_file->Write();
    m_file->Close();
  }

  return Pythia8_i::genFinalize();
}


int MultiPy8Pileup::nPileupEvents() {
  // return the instantaneous mu value according to the profile
  double threshold = m_randomEngine->flat() * m_pileupProfileIntegral.back();
  std::vector< double >::iterator itr = std::lower_bound( m_pileupProfileIntegral.begin(), m_pileupProfileIntegral.end(), threshold );
  return std::distance( m_pileupProfileIntegral.begin(), itr );
}
