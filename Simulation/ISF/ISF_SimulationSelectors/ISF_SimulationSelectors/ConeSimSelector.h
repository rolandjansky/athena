/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ConeSimSelector.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_TOOLS_CONESIMSELECTOR_H
#define ISF_TOOLS_CONESIMSELECTOR_H 1

// ISF includes
#include "ISF_Event/ConeParticleCuts.h"
#include "ISF_Interfaces/ISimulationSelector.h"
#include "ISF_Event/HepMCHelper.h"

// Framework
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {
  class IExtrapolator;
  class ITrackingGeometrySvc;
  class TrackingGeometry;
  class TrackingVolume;
  class PdgToParticleHypothesis;
}

namespace ISF {

  /** @class ConeSimSelector
  
      This SimulationSelector implementation registers cones around particles that
      are given to the updateInsideSubDet(..) or the updateOutsideSubDet(..) method
      and pass certain cuts (pT, pdg, HepMC ancestor). PassSelectorCuts(..) determines
      whether the given particle is inside such a cone or not.
  
      @author Andreas.Salzburger -at- cern.ch , Elmar.Ritsch -at- cern.ch
     */
  class ConeSimSelector : public ISimulationSelector, public ConeParticleCuts {
      
    public: 
     /** Constructor with parameters */
     ConeSimSelector( const std::string& t, const std::string& n, const IInterface* p );

     /** Destructor */
     ~ConeSimSelector();

     /** Athena AlgTool initialize */
     StatusCode  initialize();

     /** Athena AlgTool finalize */
     StatusCode  finalize();

     /** called at the beginning of each event (used for resetting dynamic selectors) */
     virtual void beginEvent();

     /** called at the beginning of each event (used for resetting dynamic selectors) */
     virtual void endEvent();

      /** update internal event representation (create cones in our case)*/
      virtual void update(const ISFParticle& p );

      /** check whether given particle is within any of the registered cones
          -> will be used for routing decision*/
      virtual bool passSelectorCuts(const ISFParticle& particle) const;

    private:

      bool retrieveTrackingGeometry() const;

      std::vector<int>                          m_absPDGVector;  //!< abs(PDG) for particles to create cones around
      /** ISFParticle has to have a relative which is in this list to create a cone*/
      bool                                      m_checkRelatives;//!< on/off for checking relatives
      std::vector<int>                          m_relativesVec;  //!< Python property
      std::set<int>                             m_relatives;     //!< used during runtime (faster)
      
      /** ISFParticle relation to the given pdg codes */
      int                                       m_relationProp;  //!< Python property
      HepMC::IteratorRange                      m_relation;      //!< HepMC

      /** tracking geometry for geometry signature */
      mutable const Trk::TrackingGeometry*      m_trackingGeometry;     //!< the tracking geometry owned by the navigator      
      ServiceHandle<Trk::ITrackingGeometrySvc>  m_trackingGeometrySvc;  //!< ServiceHandle to the TrackingGeometrySvc
      std::string                               m_trackingGeometryName; //!< default name of the TrackingGeometry  

      /** extrapolation to calo entry */
      ToolHandle<Trk::IExtrapolator>       m_extrapolator;              //!< ToolHandle for track extrapolator

      bool                                      m_extrapolateToCalo; //!< on/off for extrapolating track to CaloEntry prior before building cone

      mutable const Trk::TrackingVolume*        m_caloEntrance;

      Trk::PdgToParticleHypothesis*             m_pdgToParticleHypothesis; //!< converts PDG ID to hypothesis for TrackParameters

  };

}

#endif //> !ISF_TOOLS_CONESIMSELECTOR_H
