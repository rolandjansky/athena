/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenParticle.h"

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
	     m_starlight(0),
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
	     m_pythFullRec(0)
{
  declareProperty("Initialize",	m_InitializeVector);
  declareProperty("ConfigFileName", m_configFileName);
  
}

Starlight_i::~Starlight_i()
{}

StatusCode Starlight_i::genInitialize()
{
    // Initialisation of input parameters
    //
    ATH_MSG_INFO( "===> January 20 2011 STARLIGHT INTERFACE VERSION. \n"   );
    ATH_MSG_INFO( "===> STARLIGHT INITIALISING. \n"   );

    // not working yet, postpone
    static const bool CREATEIFNOTTHERE(true);
    ATH_CHECK( service("AtRndmGenSvc", p_AtRndmGenSvc, CREATEIFNOTTHERE) );

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

    // create the starlight object
    m_starlight = new starlight();
    // set input parameters
    m_starlight->setInputParameters(&m_inputParameters);
    // and initialize
    m_starlight->init();

    return StatusCode::SUCCESS;
}

StatusCode Starlight_i::callGenerator()
{
    ATH_MSG_DEBUG( " STARLIGHT generating. \n"   );

    // Generate event
    m_event = new upcEvent; 
    (*m_event) = m_starlight->produceEvent();

    // update event counter
    ++m_events;

    int numberofTracks = m_event->getParticles()->size();
    int numberOfVertices = 1; //m_event->getVertices()->size();

    ATH_MSG_DEBUG( "EVENT: " << m_events << " " 
                   << " with " << numberOfVertices << " vertices "
                   << " and " << numberofTracks << " tracks"  );
    ATH_MSG_DEBUG( "VERTEX: "<< 0. << " " << 0. << " " << 0. 
                   << " with " << numberofTracks << " tracks"  );
      
    int ipart = 0;
    std::vector<starlightParticle>::const_iterator part = 
      (m_event->getParticles())->begin();
    for (part = m_event->getParticles()->begin(); 
	 part != m_event->getParticles()->end(); part++, ipart++) {
      ATH_MSG_DEBUG( "TRACK: " << " " 
                     << starlightParticleCodes::jetsetToGeant((*part).getCharge() * (*part).getPdgCode()) << " "
                     << (*part).GetPx() << " " << (*part).GetPy() << " "<< (*part).GetPz() 
                     << " " << m_events << " " << ipart << " " << 0 << " "
                     << (*part).getCharge() * (*part).getPdgCode()  );
    }

    ATH_MSG_DEBUG( " Starlight generating done.  \n"   );

    return StatusCode::SUCCESS;  
}

StatusCode
Starlight_i::genFinalize()
{
    ATH_MSG_DEBUG( "  STARLIGHT Ending.  \n"   );

    return StatusCode::SUCCESS;
}

StatusCode
Starlight_i::fillEvt(HepMC::GenEvent* evt)
{
    ATH_MSG_DEBUG( "  STARLIGHT Filing.  \n"   );

    // Set the event number
    evt->set_event_number( m_events );

    // Set the generator id
    HepMC::set_signal_process_id(evt,0);

    // Create the event vertex
    HepMC::GenVertexPtr v1 = HepMC::newGenVertexPtr();
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
        int chsign = charge/abs(charge);
        if( chsign != pidsign ) pid = -pid;

	double px = (*part).GetPx();
	double py = (*part).GetPy();
	double pz = (*part).GetPz();
	double e  = (*part).GetE();
	// mass fix implemented only for muons
	if(std::abs(pid)==13) {
          float mass = m_inputParameters.muonMass();//0.1056583715;// starlightConstants::muonMass;
	  e  = std::sqrt(px*px + py*py + pz*pz + mass*mass);
	}
	ATH_MSG_DEBUG( "saving particle " << ipart  );

	v1->add_particle_out( 
			     HepMC::newGenParticlePtr(HepMC::FourVector(px, py, pz, e), pid, 1) );
      }
    ATH_MSG_DEBUG( "Saved " << ipart << " tracks "  );

    // Convert cm->mm and GeV->MeV
    // 
    GeVToMeV(evt);
    
    return StatusCode::SUCCESS;
}

bool Starlight_i::set_user_params()
{
  // Set starlight user initialization parameters

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
    ATH_MSG_WARNING( "problems initializing input parameters. cannot initialize starlight. "  );
    return false;
  }
  
  return true;
}

bool Starlight_i::prepare_params_file()
{
    // Write initialization parameters to tmp file

    for(CommandVector::iterator i = m_InitializeVector.begin(); i != m_InitializeVector.end(); i++ )
    {
        ATH_MSG_INFO( "  Command is: " << *i  );

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
          ATH_MSG_ERROR( " ERROR in STARLIGHT INITIALIZATION PARAMETERS  " 
                         << myparam << " is an invalid parameter !"  );
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
