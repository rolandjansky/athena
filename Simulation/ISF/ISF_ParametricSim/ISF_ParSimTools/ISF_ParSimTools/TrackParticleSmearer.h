/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackParticleSmearer.h
///////////////////////////////////////////////////////////////////

#ifndef ISF_PARSIMTOOLS_TRACKPARTICLESMEARER_H
#define ISF_PARSIMTOOLS_TRACKPARTICLESMEARER_H


// Fatras
#include "ISF_Interfaces/IParticleProcessor.h"
#include "ISF_ParSimInterfaces/IISPtoPerigeeTool.h"
// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"
// xAOD
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "ISF_ParSimInterfaces/IChargedSmearer.h"

#include "TrkExInterfaces/IExtrapolationEngine.h"

namespace ISF {
  class ISFParticle;
}


namespace iParSim {

  /**
     @class TrackParticleSmearer

     Create xAOD::TrackParticles from ISFParticles,
     the IIncidentListener handles the TrackParticleCollection

     @author Andreas.Salzburger -at- cern.ch
  */

  class TrackParticleSmearer : public extends<AthAlgTool, ISF::IParticleProcessor, IIncidentListener>
  {
  public:

    /**Constructor */
    TrackParticleSmearer(const std::string&,const std::string&,const IInterface*);

    /**Destructor*/
    ~TrackParticleSmearer();

    /** AlgTool initailize method.*/
    StatusCode initialize();

    /** AlgTool finalize method */
    StatusCode finalize();

    /** handle for incident service */
    void handle(const Incident& inc);

    /** Creates a new ISFParticle from a given ParticleState, universal transport tool */
    virtual ISF::ISFParticle* process(const ISF::ISFParticle& isp, CLHEP::HepRandomEngine *randomEngine);

  protected:

    /* Incident Service */
    ServiceHandle<IIncidentSvc>                   m_incidentSvc{this, "IncidentSvc", "IncidentSvc", ""};

    Gaudi::Property<std::string>                  m_tpContainerName{this, "TrackParticleCollection", "SmearedTrackParticles", "The collection to be written out"};            //!< output collection name
    xAOD::TrackParticleContainer*                 m_trackParticleContainer{};     //!< this is the output container
    xAOD::TrackParticleAuxContainer*              m_trackParticleAuxContainer{};  //!< this is the output container

    ToolHandleArray<IChargedSmearer>              m_chargedSmearers;            //!< the Charged smearers to be retrieved
    std::map<unsigned int, const IChargedSmearer*>      m_chargedSmearerMap;    //!< reordered in a map for look-up

    PublicToolHandle<Trk::IExtrapolationEngine>  m_extrapolationEngine{this, "ExtrapolationEngine", "", "The extrapolation engine to create the parameters"};        //!< the extrapolation engine to create the parameters

    PublicToolHandle<IISPtoPerigeeTool>          m_ISPtoPerigeeTool{this, "ISPtoPerigeeTool", "", ""};


  };

} // end of namespace

#endif
