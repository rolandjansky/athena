/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Geant4SimSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_GEANT4SIMSVC_H
#define ISF_GEANT4SIMSVC_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthService.h"

// ISF includes
#include "ISF_Interfaces/BaseSimulationSvc.h"

class IDetectorGeometrySvc;
class IGeoModelSvc;
class G4Timer;

namespace ISF {
  class ISFParticle;
  class ITruthSvc;
}

namespace iGeant4 {

  class ITransportTool;

  /** @class Geant4SimSvc

      @author Andreas.Salzburger -at- cern.ch
  */
  class Geant4SimSvc : public ISF::BaseSimulationSvc {

  public:
    //** Constructor with parameters */
    Geant4SimSvc( const std::string& name, ISvcLocator* pSvcLocator );

    /** Destructor */
    virtual ~Geant4SimSvc();

    /** Athena algorithm's interface methods */
    StatusCode  initialize();
    StatusCode  finalize();

    /** Simulation Call  */
    StatusCode simulate(const ISF::ISFParticle& isp);


    /** Simulation Call for vector of ISF particles */
    StatusCode simulateVector(const ISF::ConstISFParticleVector& particles);

    /** callback after geometry initialization,
        needs to be registered in ::initialize() */
    StatusCode geoInit(IOVSVC_CALLBACK_ARGS);

    /** Setup Event chain - in case of a begin-of event action is needed */
    StatusCode setupEvent();

    /** Release Event chain - in case of an end-of event action is needed */
    StatusCode releaseEvent();

  private:
    /** Default constructor */
    Geant4SimSvc();

    /** GeoModelSvc for callback */
    ServiceHandle<IGeoModelSvc>         m_geoModelSvc;
    ServiceHandle<IDetectorGeometrySvc> m_detGeoSvc;

    /** Track Creation & transport */
    ToolHandle<iGeant4::ITransportTool>  m_simulationTool;            //!< Pointer to the TrackCreator AlgTool

    /** Geant4 Python Configuration Tool */
    ToolHandle<IAlgTool>                 m_configTool;                //!< Pointer to the G4 Python Config Tool

    // timing checks
    bool  m_doTiming;
    //float m_runTime;
    float m_accumulatedEventTime;
    float m_accumulatedEventTimeSq;
    unsigned int m_nrOfEntries;

    G4Timer* m_runTimer;
    G4Timer* m_eventTimer;

  };
}


#endif //> !ISF_Geant4SimSvc_H
