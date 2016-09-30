/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TransportSimSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "Geant4SimSvc.h"

// StoreGate
#include "StoreGate/StoreGateSvc.h"
// Remaining ISF include
#include "ISF_Interfaces/ITruthSvc.h"
#include "ISF_Event/ISFParticle.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"
#include "ISF_Geant4Interfaces/ITransportTool.h"

// Geant4 classes
#include "G4Timer.hh"
#include "G4SDManager.hh"


/** Constructor **/
iGeant4::Geant4SimSvc::Geant4SimSvc(const std::string& name,ISvcLocator* svc) :
  BaseSimulationSvc(name, svc),
  m_geoModelSvc("GeoModelSvc",name),
  m_detGeoSvc("DetectorGeometrySvc", name),
  m_simulationTool("ISFG4TransportTool"),
  m_configTool(""),
  m_doTiming(true),
  m_accumulatedEventTime(0.),
  m_accumulatedEventTimeSq(0.),
  m_nrOfEntries(0),
  m_runTimer(0),
  m_eventTimer(0)
{
  declareProperty("SimulationTool" ,      m_simulationTool );
  declareProperty("GeoModelService",      m_geoModelSvc    );
  declareProperty("DetectorGeometrySvc",  m_detGeoSvc      );
  declareProperty("G4ConfigTool"   ,      m_configTool     );

  declareProperty("PrintTimingInfo",      m_doTiming       );
}

iGeant4::Geant4SimSvc::~Geant4SimSvc()
{}

/** framework methods */
StatusCode iGeant4::Geant4SimSvc::initialize()
{
  ATH_MSG_INFO ( m_screenOutputPrefix << "Initializing ...");

  // create G4Timers if enabled
  if (m_doTiming) {
    m_runTimer   = new G4Timer();
    m_eventTimer = new G4Timer();
    m_runTimer->Start();
  }

  // retrieve simulation tool
  if ( retrieveTool<iGeant4::ITransportTool>(m_simulationTool).isFailure() ) {
    ATH_MSG_FATAL(  m_screenOutputPrefix << " Cannot retrieve simulation tool! Abort." );
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO ( m_screenOutputPrefix << m_simulationTool->name() );

  // Need to make sure that trackingGeometryInit gets called after IGeoModelSvc::geoInit
  if ( m_geoModelSvc.retrieve().isFailure() ){
    ATH_MSG_FATAL(  m_screenOutputPrefix << " Cannot retrieve GeoModelSvc! Abort." );
    return StatusCode::FAILURE;
  }

  const IGeoModelSvc* geoModelSvc = &(*m_geoModelSvc);
  if (!geoModelSvc->geoInitialized()) {
    // register the callback from the IGeoModelSvc
    ATH_MSG_INFO("registering callback to ISimulationSvc::geoInit");
    if ( (detStore()->regFcn(&IGeoModelSvc::geoInit,
                             geoModelSvc,
                             &ISimulationSvc::geoInit,
                             dynamic_cast<ISimulationSvc*>(this))).isFailure() ){
      ATH_MSG_FATAL( m_screenOutputPrefix << " Cannot register Callback to GeoModelSvc! Abort." );
      return StatusCode::FAILURE;
    }
  } else {
    // call with dummy parameters
    int par1 = 0;
    std::list<std::string> par2;
    ATH_MSG_INFO("calling geoInit");
    if ( geoInit(par1,par2).isFailure() ){
      ATH_MSG_FATAL( m_screenOutputPrefix << " Cannot call geometry init although GeoModelSvc is initialized." );
      return StatusCode::FAILURE;
    }
  }

  // retrieve (and run) the Geant4 Python Config tool
  if ( !m_configTool.empty() && m_configTool.retrieve().isFailure() ){
    ATH_MSG_FATAL(  m_screenOutputPrefix << " Cannot retrieve Geant4 Python Config Tool! Abort." );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

/** framework methods */
StatusCode iGeant4::Geant4SimSvc::finalize()
{
  ATH_MSG_INFO ( m_screenOutputPrefix << "Finalizing ..." );

  if (m_doTiming) {
    m_runTimer->Stop();
    float runTime=m_runTimer->GetUserElapsed()+m_runTimer->GetSystemElapsed();
    float avgTimePerEvent=(m_nrOfEntries>1) ? m_accumulatedEventTime/(m_nrOfEntries-1.) : runTime;
    float sigma=( m_nrOfEntries>2) ? std::sqrt((m_accumulatedEventTimeSq/float(m_nrOfEntries-1)-
                                                avgTimePerEvent*avgTimePerEvent)/float(m_nrOfEntries-2)) : 0;
    ATH_MSG_INFO("*****************************************"<<endreq<<
                 "**                                     **"<<endreq<<
                 "    End of run - time spent is "<<std::setprecision(4) <<
                 runTime<<endreq<<
                 "    Average time per event was "<<std::setprecision(4) <<
                 avgTimePerEvent <<" +- "<< std::setprecision(4) << sigma<<endreq<<
                 "**                                     **"<<endreq<<
                 "*****************************************");
  }

  return StatusCode::SUCCESS;
}

StatusCode iGeant4::Geant4SimSvc::setupEvent()
{
  ATH_MSG_DEBUG ( m_screenOutputPrefix << "setup Event" );

  m_nrOfEntries++;
  if (m_doTiming) m_eventTimer->Start();

  // make sure SD collections are properly initialized in every Athena event
  G4SDManager::GetSDMpointer()->PrepareNewEvent();

  return StatusCode::SUCCESS;
}

StatusCode iGeant4::Geant4SimSvc::releaseEvent()
{
  ATH_MSG_DEBUG ( m_screenOutputPrefix << "release Event" );
  /**@TODO : strip hits of the tracks ... */

  /* TODO: ELLI: the following is copied in from the PyG4AtlasAlg:
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

  return StatusCode::SUCCESS;
}

/** Simulation Call */
StatusCode iGeant4::Geant4SimSvc::simulate(const ISF::ISFParticle& isp)
{
  // give a screen output that you entered Geant4SimSvc
  ATH_MSG_VERBOSE( m_screenOutputPrefix << "Particle " << isp << " received for simulation." );

  /** Process ParticleState from particle stack */
  StatusCode success = m_simulationTool->process(isp);
  ATH_MSG_VERBOSE( m_screenOutputPrefix << "Simulation done" );

  // Geant4 call done
  return success;
}

/** Simulation Call */
StatusCode iGeant4::Geant4SimSvc::simulateVector(const ISF::ConstISFParticleVector& particles)
{
  // give a screen output that you entered Geant4SimSvc
  ATH_MSG_VERBOSE( m_screenOutputPrefix << "Vector of particles received for simulation." );


  /** Process ParticleState from particle stack */
  StatusCode success = m_simulationTool->processVector(particles);
  ATH_MSG_VERBOSE( m_screenOutputPrefix << "Simulation done for vector of particles" );


  // Geant4 call done
  return success;
}

StatusCode iGeant4::Geant4SimSvc::geoInit(IOVSVC_CALLBACK_ARGS)
{
  ATH_MSG_INFO( m_screenOutputPrefix << "ATLAS Geometry is initialized, calling Geo2G4 converter." );
  if (m_detGeoSvc.retrieve().isFailure())
    {
      ATH_MSG_FATAL("Could not retrieve DetectorGeometrySvc.");
      return StatusCode::FAILURE;
    }

  return StatusCode::SUCCESS;
}
