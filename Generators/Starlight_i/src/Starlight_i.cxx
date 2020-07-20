/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
// ------------------------------------------------------------- 
// Generators/Starlight_i.cxx Description: Allows the user
// to generate Starlight events and store the result in the
// Transient Store.
//
// AuthorList:
//   Andrzej Olszewski:  Initial Code January 2011
//   Andrzej Olszewski:  Update for Starlight "r193" March 2016
//
// Random seed set via jo 
// Random numbers not saved by atlas engine mechanism event by event.

#include "Starlight_i/Starlight_i.h"

#include "TruthUtils/GeneratorName.h"

#include "GaudiKernel/MsgStream.h"

#include "GeneratorUtils/StringParse.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "StoreGate/StoreGateSvc.h"

namespace{
// Pointer On AtRndmGenSvc
  static IAtRndmGenSvc*  p_AtRndmGenSvc;
  static std::string     starlight_stream = "STARLIGHT";
}

#include "reportingUtils.h"
#include "starlightconstants.h"
#include "starlightparticlecodes.h"

Starlight_i::Starlight_i(const std::string& name, ISvcLocator* pSvcLocator): 
             GenModule(name,pSvcLocator), m_events(0), 
             m_lheOutput(false),
             m_maxevents(5500), 
	     m_starlight(),
	     m_inputParameters(),
	     m_randomGenerator(),
	     m_event(0),
	     m_configFileName(""),
	     m_beam1Z(0),
	     m_beam1A(0),
	     m_beam2Z(0),
	     m_beam2A(0),
	     m_beam1Gamma(0.),
	     m_beam2Gamma(0.),
	     m_maxW(0.),
	     m_minW(0.),
	     m_nmbWBins(0),
	     m_maxRapidity(0.),
	     m_nmbRapidityBins(0),
	     m_accCutPt(false),
	     m_minPt(0.),
	     m_maxPt(0.),
	     m_accCutEta(false),
	     m_minEta(0.),
	     m_maxEta(0.),
	     m_productionMode(0),
	     m_axionMass(1.),
	     m_nmbEventsTot(0),
	     m_prodParticleId(0),
	     m_randomSeed(0),
	     m_beamBreakupMode(0),
	     m_interferenceEnabled(false),
	     m_interferenceStrength(0.),
	     m_coherentProduction(false),
	     m_incoherentFactor(0.),
	     m_maxPtInterference(0.),
	     m_nmbPtBinsInterference(0),
	     m_ptBinWidthInterference(0),
	     m_xsecMethod(0),
	     m_nThreads(1),
	     m_pythFullRec(0),
	     m_storeGate(0)
{
  declareProperty("Initialize",	m_InitializeVector);
  declareProperty("ConfigFileName", m_configFileName);
  declareProperty("lheOutput",	m_lheOutput);
  declareProperty("maxevents",	m_maxevents);
}

Starlight_i::~Starlight_i()
{}

StatusCode Starlight_i::genInitialize()
{
    // Initialisation of input parameters
    //
  MsgStream log(messageService(), name());
  log << MSG::INFO 
      << "===> January 20 2011 STARLIGHT INTERFACE VERSION. \n"  << endreq;
  log << MSG::INFO 
      << "===> STARLIGHT INITIALISING. \n"  << endreq;

    StatusCode sc = service("StoreGateSvc", m_storeGate);
    if (sc.isFailure())
      {
	log << MSG::WARNING 
	    << "Unable to get pointer to StoreGate Service"  << endreq;
      	return sc;
      }

    
    // not working yet, postpone
    static const bool CREATEIFNOTTHERE(true);
    StatusCode RndmStatus = 
      service("AtRndmGenSvc", p_AtRndmGenSvc, CREATEIFNOTTHERE);
    if (!RndmStatus.isSuccess() || 0 == p_AtRndmGenSvc) {
      log << MSG::ERROR 
	  << " Could not initialize Random Number Service" << endreq;
      return RndmStatus;
    }   

    // Save seeds
    CLHEP::HepRandomEngine* engine = 
      p_AtRndmGenSvc->GetEngine(starlight_stream);
    const long* sip = engine->getSeeds();
    m_randomSeed = sip[0];
    

    // Create inputParameters and
    // set the users' initialisation parameters choices
    bool res = set_user_params();
    if( !res ) {
      return StatusCode::FAILURE;
    }
    
    
    m_randomGenerator.SetSeed(m_randomSeed);
    

    // create the starlight object
    m_starlight = new starlight();
    
    // adding explicit setting
    m_starlight->setInputParameters(&m_inputParameters);
    m_starlight->setRandomGenerator(&m_randomGenerator);
    
    // and initialize
    m_starlight->init();
    
    // dump events to lhef (needed for QED showering with Pythia8
    if(m_lheOutput){
        log << MSG::INFO
      << "===> dumping starlight events to lhef format. \n"  << endreq;
        if(!starlight2lhef()) return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode Starlight_i::callGenerator()
{
    if(m_lheOutput) return StatusCode::SUCCESS;
 
    MsgStream log(messageService(), name());
    log << MSG::DEBUG
	<< " STARLIGHT generating. \n"  << endreq;

    // Generate event
    m_event = new upcEvent; 
    (*m_event) = m_starlight->produceEvent();

    // update event counter
    ++m_events;

    int numberofTracks = m_event->getParticles()->size();
    int numberOfVertices = 1; //m_event->getVertices()->size();

    log << MSG::DEBUG
        << "EVENT: " << m_events << " " 
	<< " with " << numberOfVertices << " vertices "
	<< " and " << numberofTracks << " tracks" << endreq;
    log << MSG::DEBUG
	<< "VERTEX: "<< 0. << " " << 0. << " " << 0. 
	<< " with " << numberofTracks << " tracks" << endreq;
      
    int ipart = 0;
    std::vector<starlightParticle>::const_iterator part = 
      (m_event->getParticles())->begin();
    for (part = m_event->getParticles()->begin(); 
	 part != m_event->getParticles()->end(); part++, ipart++) {
      log << MSG::DEBUG
	  << "TRACK: " << " " 
	  << starlightParticleCodes::jetsetToGeant((*part).getCharge() * (*part).getPdgCode()) << " "
	  << (*part).GetPx() << " " << (*part).GetPy() << " "<< (*part).GetPz() 
	  << " " << m_events << " " << ipart << " " << 0 << " "
	  << (*part).getCharge() * (*part).getPdgCode() << endreq;
    }

    log << MSG::DEBUG
        << " Starlight generating done.  \n"  << endreq;

    return StatusCode::SUCCESS;  
}

StatusCode
Starlight_i::genFinalize()
{
    MsgStream log(messageService(), name());
    log << MSG::DEBUG
        << "  STARLIGHT Ending.  \n"  << endreq;

    return StatusCode::SUCCESS;
}

StatusCode
Starlight_i::fillEvt(HepMC::GenEvent* evt)
{


   if(m_lheOutput) return StatusCode::SUCCESS;
   
   MsgStream log(messageService(), name());
   log << MSG::DEBUG
       << "  STARLIGHT Filing.  \n"  << endreq;

    // Set the event number
    evt->set_event_number( m_events );

    // Set the generator id
    evt->set_signal_process_id(0);

    // Create the event vertex
    HepMC::GenVertex* v1 = new HepMC::GenVertex();
    evt->add_vertex( v1 );


    // Loop on all final particles and 
    // put them all as outgoing from the event vertex
    int ipart = 0;
    std::vector<starlightParticle>::const_iterator part =
      (m_event->getParticles())->begin();
    for (part = m_event->getParticles()->begin();
         part != m_event->getParticles()->end(); part++, ipart++) 
      {
	int pid = (*part).getPdgCode();
	int charge = (*part).getCharge();
        //AO special for pid sign stored in charge
        int pidsign = pid/abs(pid);
	int chsign = 0;
        if (charge !=0) chsign = charge/abs(charge);
        if( chsign != pidsign && chsign != 0) pid = -pid;

	double px = (*part).GetPx();
	double py = (*part).GetPy();
	double pz = (*part).GetPz();
	double e  = (*part).GetE();
	// mass fix implemented only for muons
	if(abs(pid)==13) {
//          float mass = starlightConstants::muonMass;
          float mass = m_inputParameters.muonMass();
	  e  = sqrt(px*px + py*py + pz*pz + mass*mass);
	}
	// mass fix for photons (ALPs)
	if(abs(pid)==22) {
	  e  = sqrt(px*px + py*py + pz*pz);
	}
	log << MSG::DEBUG
	    << "saving particle " << ipart << endreq;

	v1->add_particle_out( 
			     new HepMC::GenParticle(CLHEP::HepLorentzVector(px, py, pz, e), pid, 1) );
      }
    log << MSG::DEBUG
	<< "Saved " << ipart << " tracks " << endreq;

    // Convert cm->mm and GeV->MeV
    // 
    GeVToMeV(evt);
    
    return StatusCode::SUCCESS;
}

bool
Starlight_i::starlight2lhef()
{
        
    std::string lheFilename    = "events.lhe";
    std::ofstream lheStream;
    lheStream.open(lheFilename.c_str(), std::ofstream::trunc);
    if(!lheStream) { 
      ATH_MSG_ERROR("error: Failed to open  file "+lheFilename);
      return false;
    }
   
    lheStream << "<LesHouchesEvents version=\"1.0\">\n";
    lheStream << "<!--\n";
    lheStream << "File generated using Starlight \n";
    lheStream << "-->\n";
 
    lheStream << "<init>\n";
    lheStream << "  11  -11  2.510000e+03  2.510000e+03  0  0  0  0  3  1\n";
    lheStream << "  1.000000e+00  0.000000e+00  1.000000e+00   9999\n";
    lheStream << "</init>\n";
    
    
    std::unique_ptr<upcEvent> m_event(new upcEvent); 

    for(unsigned int i=0; i<m_maxevents; i++) {
      lheStream << "<event>\n";
      (*m_event) = m_starlight->produceEvent();
      int ipart = 0;
      CLHEP::HepLorentzVector photon_system(0);
      double ptscale =0;
      std::vector<starlightParticle>::const_iterator part = (m_event->getParticles())->begin();
      for (part = m_event->getParticles()->begin(); part != m_event->getParticles()->end(); part++, ipart++)
      {
         CLHEP::HepLorentzVector particle_sl((*part).GetPx(), (*part).GetPy(), (*part).GetPz(), (*part).GetE());
         photon_system += particle_sl;
         ptscale += sqrt((*part).GetPx()*(*part).GetPx() + (*part).GetPy()*(*part).GetPy()); 
      }
      // avg pt is the correct scale here
      ptscale /= (float) ipart;
      lheStream << "     4  9999  1.000000e+00  "<<ptscale<<"  7.297e-03  2.569093e-01\n";
      lheStream << " 22    -1     0     0     0     0  0.0000000000e+00  0.0000000000e+00  "
                << photon_system.m()/2.*exp(photon_system.rapidity())<<"  "<<photon_system.m()/2.*exp(photon_system.rapidity())<<"  0.0000000000e+00 0. 9.\n";
      lheStream << " 22    -1     0     0     0     0  0.0000000000e+00  0.0000000000e+00  "
                << -photon_system.m()/2.*exp(-photon_system.rapidity())<<"  "<<photon_system.m()/2.*exp(-photon_system.rapidity())<<"  0.0000000000e+00 0. 9.\n";
      
      for (part = m_event->getParticles()->begin(); part != m_event->getParticles()->end(); part++, ipart++) 
      {
	int pid = (*part).getPdgCode();
	int charge = (*part).getCharge();
        //AO special for pid sign stored in charge
        int pidsign = pid/abs(pid);
        int chsign = charge/abs(charge);
        if( chsign != pidsign ) pid = -pid;

	double px = (*part).GetPx();
	double py = (*part).GetPy();
	double pz = (*part).GetPz();
	double e  = (*part).GetE();
        double mass  = (*part).getMass();
        if(abs(pid)==11) mass = m_inputParameters.mel();
        if(abs(pid)==13) mass = m_inputParameters.muonMass();
        if(abs(pid)==15) mass = m_inputParameters.tauMass();

        lheStream << pid<<"     1     1     2     0     0  "<<px<<"  "<<py<<"  "<<pz<<"  "<<e<<"  "<<mass<<"  0. 9.\n";
      
       }    
    lheStream << "</event>\n";
    }


    lheStream << "</LesHouchesEvents>";
    lheStream.close();

    return true;

}

bool Starlight_i::set_user_params()
{
  // Set starlight user initialization parameters
  MsgStream log(messageService(), name());

  // write python starlight config parameters to tmp file
  // if external config file not specified
  if (m_configFileName.empty()) {
    m_configFileName = "tmp.slight.in";
    if (!prepare_params_file()) {
      printWarn << 
	"problems initializing input parameters. cannot initialize starlight.";
      return false;
    }
  }
  
  m_inputParameters.configureFromFile(m_configFileName);
  
  if (!m_inputParameters.init()) {
    log << MSG::WARNING
	<< "problems initializing input parameters. cannot initialize starlight. " << endreq;
    return false;
  }
  log << MSG::INFO << "after m_inputParameters->init()" << endreq; 
  
  return true;
}

bool Starlight_i::prepare_params_file()
{
    // Write initialization parameters to tmp file
    MsgStream log(messageService(), name());

    for(CommandVector::iterator i = m_InitializeVector.begin(); i != m_InitializeVector.end(); i++ )
    {
	log << MSG::INFO
	    << "  Command is: " << *i << endreq;

	StringParse mystring(*i);
	std::string myparam = mystring.piece(1);
	if (myparam == "beam1Z")
	{
	  m_beam1Z  = mystring.numpiece(2);
	}
	else if (myparam == "beam1A")
	{
	  m_beam1A  = mystring.numpiece(2);
	}
	else if (myparam == "beam2Z")
	{
	  m_beam2Z  = mystring.numpiece(2);
	}
	else if (myparam == "beam2A")
	{
	  m_beam2A  = mystring.numpiece(2);
	}
	else if (myparam == "beam1Gamma")
	{
	  m_beam1Gamma  = mystring.numpiece(2);
	}
	else if (myparam == "beam2Gamma")
	{
	  m_beam2Gamma  = mystring.numpiece(2);
	}
	else if (myparam == "maxW")
	{
	  m_maxW  = mystring.numpiece(2);
	}
	else if (myparam == "minW")
	{
	  m_minW  = mystring.numpiece(2);
	}
	else if (myparam == "nmbWBins")
	{
	  m_nmbWBins  = mystring.numpiece(2);
	}
	else if (myparam == "maxRapidity")
	{
	  m_maxRapidity  = mystring.numpiece(2);
	}
	else if (myparam == "nmbRapidityBins")
	{
	  m_nmbRapidityBins  = mystring.numpiece(2);
	}
	else if (myparam == "accCutPt")
	{
	  m_accCutPt = mystring.numpiece(2);
	}
	else if (myparam == "minPt")
	{
	  m_minPt = mystring.numpiece(2);
	}
	else if (myparam == "maxPt")
	{
	  m_maxPt = mystring.numpiece(2);
	}
	else if (myparam == "accCutEta")
	{
	  m_accCutEta = mystring.numpiece(2);
	}
	else if (myparam == "minEta")
	{
	  m_minEta = mystring.numpiece(2);
	}
	else if (myparam == "maxEta")
	{
	  m_maxEta = mystring.numpiece(2);
	}
	else if (myparam == "productionMode")
	{
	  m_productionMode  = mystring.numpiece(2);
	}
	else if (myparam == "axionMass")
	{
	  m_axionMass  = mystring.numpiece(2);
	}
	else if (myparam == "nmbEventsTot")
	{
	  m_nmbEventsTot  = mystring.numpiece(2);
	}
	else if (myparam == "prodParticleId")
	{
	  m_prodParticleId  = mystring.numpiece(2);
	}
	else if (myparam == "randomSeed")
	{
	  m_randomSeed  = mystring.numpiece(2);
	}
	else if (myparam == "outputFormat")
	{
	  m_outputFormat  = mystring.numpiece(2);
	}
	else if (myparam == "beamBreakupMode")
	{
	  m_beamBreakupMode  = mystring.numpiece(2);
	}
	else if (myparam == "interferenceEnabled")
	{
	  m_interferenceEnabled  = mystring.numpiece(2);
	}
	else if (myparam == "interferenceStrength")
	{
	  m_interferenceStrength  = mystring.numpiece(2);
	}
	else if (myparam == "coherentProduction")
	{
	  m_coherentProduction = mystring.numpiece(2);
	}
	else if (myparam == "incoherentFactor")
	{
	  m_incoherentFactor  = mystring.numpiece(2);
	}
	else if (myparam == "maxPtInterference")
	{
	  m_maxPtInterference  = mystring.numpiece(2);
	}
	else if (myparam == "nmbPtBinsInterference")
	{
	  m_nmbPtBinsInterference  = mystring.numpiece(2);
	}
	else if (myparam == "xsecMethod")
	{
	  m_xsecMethod = mystring.numpiece(2);
	}
	else if (myparam == "nThreads")
	{
	  m_nThreads = mystring.numpiece(2);
	}
	else if (myparam == "pythFullRec")
	{
	  m_pythFullRec = mystring.numpiece(2);
	}
	else
	{
	    log << MSG::ERROR 
		<< " ERROR in STARLIGHT INITIALIZATION PARAMETERS  " 
		<< myparam << " is an invalid parameter !" << endreq;
	    return false;
	}
    }

    std::ofstream configFile;
    configFile.open(m_configFileName.c_str());

    configFile << "BEAM_1_Z = " << m_beam1Z << std::endl; 
    configFile << "BEAM_1_A = " << m_beam1A << std::endl;
    configFile << "BEAM_2_Z = " << m_beam2Z << std::endl; 
    configFile << "BEAM_2_A = " << m_beam2A << std::endl;
    configFile << "BEAM_1_GAMMA = " << m_beam1Gamma << std::endl;
    configFile << "BEAM_2_GAMMA = " << m_beam2Gamma << std::endl;
    configFile << "W_MAX = " << m_maxW << std::endl; 
    configFile << "W_MIN = " << m_minW << std::endl;
    configFile << "W_N_BINS = " << m_nmbWBins << std::endl;
    configFile << "RAP_MAX = " << m_maxRapidity << std::endl;
    configFile << "RAP_N_BINS = " << m_nmbRapidityBins << std::endl;
    configFile << "CUT_PT = " << m_accCutPt << std::endl;
    configFile << "PT_MIN = " << m_minPt << std::endl;
    configFile << "PT_MAX = " << m_maxPt << std::endl;
    configFile << "CUT_ETA = " << m_accCutEta << std::endl;
    configFile << "ETA_MIN = " << m_minEta << std::endl;
    configFile << "ETA_MAX = " << m_maxEta << std::endl;
    configFile << "PROD_MODE = " << m_productionMode << std::endl;
    configFile << "AXION_MASS = " << m_axionMass << std::endl;
    configFile << "N_EVENTS = " << m_nmbEventsTot << std::endl;
    configFile << "PROD_PID = " << m_prodParticleId << std::endl;
    configFile << "RND_SEED = " << m_randomSeed << std::endl;
    configFile << "BREAKUP_MODE = " << m_beamBreakupMode << std::endl;
    configFile << "INTERFERENCE = " << m_interferenceEnabled << std::endl;
    configFile << "IF_STRENGTH = " << m_interferenceStrength << std::endl;
    configFile << "INT_PT_MAX = " << m_maxPtInterference << std::endl;
    configFile << "INT_PT_N_BINS = " << m_nmbPtBinsInterference << std::endl;
    configFile << "COHERENT = " << m_coherentProduction << std::endl;
    configFile << "INCO_FACTOR = " << m_incoherentFactor << std::endl;
    configFile << "XSEC_METHOD = " << m_xsecMethod << std::endl;
    configFile << "N_THREADS = " << m_nThreads << std::endl;
    configFile << "PYTHIA_FULL_EVENTRECORD = " << m_pythFullRec << std::endl;

    configFile.close();
    return true;
}
