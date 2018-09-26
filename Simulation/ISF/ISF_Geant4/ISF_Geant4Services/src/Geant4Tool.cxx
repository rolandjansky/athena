/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "./Geant4Tool.h"

// Remaining ISF include
#include "ISF_Event/ISFParticle.h"

// Geant4 classes
#include "G4Timer.hh"
#include "G4SDManager.hh"

#include "AtlasDetDescr/AtlasRegionHelper.h"

ISF::Geant4Tool::Geant4Tool(const std::string& type, const std::string& name,  const IInterface* parent)
  : BaseSimulatorTool(type, name, parent)
{
  declareProperty("PrintTimingInfo",      m_doTiming       );
}

ISF::Geant4Tool::~Geant4Tool()
{

}

StatusCode ISF::Geant4Tool::finalize()
{
  ATH_MSG_INFO ( "Finalizing ..." );

  if (m_doTiming) {
    m_runTimer->Stop();
    float runTime=m_runTimer->GetUserElapsed()+m_runTimer->GetSystemElapsed();
    float avgTimePerEvent=(m_nrOfEntries>1) ? m_accumulatedEventTime/(m_nrOfEntries-1.) : runTime;
    float sigma=( m_nrOfEntries>2) ? std::sqrt((m_accumulatedEventTimeSq/float(m_nrOfEntries-1)-
                                                avgTimePerEvent*avgTimePerEvent)/float(m_nrOfEntries-2)) : 0;
    ATH_MSG_INFO("*****************************************"<<endmsg<<
                 "**                                     **"<<endmsg<<
                 "    End of run - time spent is "<<std::setprecision(4) <<
                 runTime<<endmsg<<
                 "    Average time per event was "<<std::setprecision(4) <<
                 avgTimePerEvent <<" +- "<< std::setprecision(4) << sigma<<endmsg<<
                 "**                                     **"<<endmsg<<
                 "*****************************************");
  }

  return StatusCode::SUCCESS;
}

StatusCode ISF::Geant4Tool::initialize()
{
  ATH_MSG_INFO ( "Initializing ...");

  ATH_CHECK( BaseSimulatorTool::initialize() );

  // create G4Timers if enabled
  if (m_doTiming) {
    m_runTimer   = new G4Timer();
    m_eventTimer = new G4Timer();
    m_runTimer->Start();
  }

  // retrieve simulation tool
  ATH_CHECK (m_g4TransportTool.retrieve());

  ATH_MSG_INFO ( m_g4TransportTool->name() );

  ATH_CHECK (m_detGeoSvc.retrieve());

  // retrieve (and run) the Geant4 Python Config tool
  if (!m_configTool.empty()) {
    ATH_CHECK (m_configTool.retrieve());
  }

  ATH_CHECK(m_senDetTool.retrieve());
  ATH_CHECK(m_fastSimTool.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode ISF::Geant4Tool::simulate( const ISFParticle& isp, ISFParticleContainer& secondaries ) {

  // give a screen output that you entered Geant4SimSvc
  ATH_MSG_VERBOSE( "Particle " << isp << " received for simulation." );

  ATH_MSG_INFO( "BEN BEN BEN gingersnap" );

  /** Process ParticleState from particle stack */
  StatusCode success = m_g4TransportTool->process(isp);
  ATH_MSG_VERBOSE( "Simulation done" );

  // return any secondaries associated with this particle
  auto searchResult = m_secondariesMap.find( &isp );
  if ( searchResult == m_secondariesMap.end() ) {

    ATH_MSG_VERBOSE( "Found no secondaries" );

  } else {

    ATH_MSG_VERBOSE( "Found secondaries: " << searchResult->second.size() );
    secondaries = searchResult->second;
    m_secondariesMap.erase( searchResult );
  }

  // Geant4 call done
  return success;
}

StatusCode ISF::Geant4Tool::simulateVector( const ISF::ConstISFParticleVector& particles, ISFParticleContainer& secondaries ) {

  ATH_MSG_DEBUG (name() << ".simulateVector(...) : Received a vector of " << particles.size() << " particles for simulation.");
  /** Process ParticleState from particle stack */
  ATH_CHECK (m_g4TransportTool->processVector(particles));
  ATH_MSG_VERBOSE( "Simulation done" );

  for (auto* cisp : particles) {
    auto* isp = const_cast<ISFParticle*>(cisp);
    // return any secondaries associated with this particle
    auto searchResult = m_secondariesMap.find( isp );
    if ( searchResult == m_secondariesMap.end() ) {

      ATH_MSG_VERBOSE( "Found no secondaries" );

    } else {

      ATH_MSG_VERBOSE( "Found secondaries: " << searchResult->second.size() );
      secondaries.splice( end(secondaries), std::move(searchResult->second) ); //append vector
      m_secondariesMap.erase( searchResult );
    }
  }
  // Geant4 call done
  return StatusCode::SUCCESS;
}

StatusCode ISF::Geant4Tool::setupEvent()
{
  ATH_MSG_DEBUG ( "setup Event" );

  ATH_CHECK(m_senDetTool->BeginOfAthenaEvent());

  m_nrOfEntries++;
  if (m_doTiming) m_eventTimer->Start();

  // make sure SD collections are properly initialized in every Athena event
  G4SDManager::GetSDMpointer()->PrepareNewEvent();

  return StatusCode::SUCCESS;
}

StatusCode ISF::Geant4Tool::setupEventST()
{
  return setupEvent();
}

StatusCode ISF::Geant4Tool::releaseEvent()
{
  ATH_MSG_DEBUG ( "release Event" );
  /** @todo : strip hits of the tracks ... */

  /* todo: ELLI: the following is copied in from the PyG4AtlasAlg:
     -> this somehow needs to be moved into C++
     and put into releaseEvent() ( or setupEvent() ?)

     from ISF_Geant4Example import AtlasG4Eng
     from ISF_Geant4Example.ISF_SimFlags import simFlags
     if self.doFirstEventG4SeedsCheck :
     if simFlags.SeedsG4.statusOn:
     rnd = AtlasG4Eng.G4Eng.menu_G4RandomNrMenu()
     rnd.set_Seed(simFlags.SeedsG4.get_Value())
     self.doFirstEventG4SeedsCheck = False
     if self.RndG4Menu.SaveStatus:
     self.RndG4Menu.Menu.saveStatus('G4Seeds.txt')
  */

  // print per-event timing info if enabled
  if (m_doTiming) {
    m_eventTimer->Stop();

    double eventTime=m_eventTimer->GetUserElapsed()+m_eventTimer->GetSystemElapsed();
    if (m_nrOfEntries>1) {
      m_accumulatedEventTime  +=eventTime;
      m_accumulatedEventTimeSq+=eventTime*eventTime;
    }

    float avgTimePerEvent=(m_nrOfEntries>1) ? m_accumulatedEventTime/(m_nrOfEntries-1.) : eventTime;
    float sigma=(m_nrOfEntries>2) ? std::sqrt((m_accumulatedEventTimeSq/float(m_nrOfEntries-1)-
                                               avgTimePerEvent*avgTimePerEvent)/float(m_nrOfEntries-2)) : 0.;

    ATH_MSG_INFO("\t Event nr. "<<m_nrOfEntries<<" took " << std::setprecision(4) <<
                 eventTime << " s. New average " << std::setprecision(4) <<
                 avgTimePerEvent<<" +- "<<std::setprecision(4) << sigma);
  }

  ATH_CHECK(m_senDetTool->EndOfAthenaEvent());
  ATH_CHECK(m_fastSimTool->EndOfAthenaEvent());

  return StatusCode::SUCCESS;
}

StatusCode ISF::Geant4Tool::releaseEventST()
{
  return releaseEvent();
}

// Act as particle broker for G4 secondaries
void ISF::Geant4Tool::push( ISF::ISFParticle *particle, const ISF::ISFParticle *parent )
{
  ATH_MSG_VERBOSE( "Caught secondary particle push() from Geant4" );

  m_secondariesMap[ parent ].push_back( particle );
}
