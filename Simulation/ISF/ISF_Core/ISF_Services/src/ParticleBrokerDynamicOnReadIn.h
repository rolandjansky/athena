/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ParticleBrokerDynamicOnReadIn.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_SERVICES_PARTICLEBROKERDYNAMICONREADIN_H
#define ISF_SERVICES_PARTICLEBROKERDYNAMICONREADIN_H 1

// STL includes
#include <string>
#include <set>

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthService.h"

// ISF includes
#include "ISF_Interfaces/IParticleBroker.h"
#include "ISF_Interfaces/IParticleOrderingTool.h"
#include "ISF_Interfaces/SimulationFlavor.h"
#include "ISF_Event/EntryLayer.h"
#include "ISF_Event/ISFParticleContainer.h"
#include "ISF_Event/ISFParticleOrderedQueue.h"
#include "BarcodeInterfaces/IBarcodeSvc.h"

// AtlasDetDescr
#include "AtlasDetDescr/AtlasRegion.h"

// forward declarations
namespace PMonUtils {
  class CustomBenchmark;
}
class ITHistSvc;
// ROOT forward declarations
class TTree;

namespace ISF {

  class IGeoIDSvc;
  class IEntryLayerTool;
  class ISimulationSelector;

  typedef std::vector<ISimulationSelector*>      SimSelectorArray;
  typedef std::set<ISimulationSelector*>         SimSelectorSet;

  /** @class ParticleBrokerDynamicOnReadIn
  
      ISF particle broker responsible to flag particles with their corresponding
      simulator. On detector boundaries, simulators hand back all particles
      that pass them via the push() method.
  
      @author Andreas.Salzburger -at- cern.ch , Elmar.Ritsch -at- cern.ch
     */
  class ParticleBrokerDynamicOnReadIn : public extends<AthService, IParticleBroker> {
    public: 
      
      //** Constructor with parameters */
      ParticleBrokerDynamicOnReadIn( const std::string& name, ISvcLocator* pSvcLocator );
      
      /** Destructor */
      virtual ~ParticleBrokerDynamicOnReadIn(); 
      
      /** Athena algorithm's interface methods */
      StatusCode  initialize();
      StatusCode  finalize();

      /** Initialize the particle broker */
      StatusCode initializeEvent(ISFParticleContainer&& simParticles);

      /** Finalize the event in the broker service */
      virtual StatusCode finalizeEvent();

      /** add a new particle to the stack and inherit ISF properties from the given
          ancestor ISF particel (usually the particle which ISF handed over to the
          simulator) */
      virtual void push( ISFParticle *particle, const ISFParticle *ancestor);
            
      /** Get vectors of ISF particles from the broker */
      virtual const ConstISFParticleVector& popVector(size_t maxVectorSize);
      
      /** Get the current stack size */
      virtual size_t numParticles() const;

      /** Return the particle stack (not implemented) */
      //const ParticleContainer& particleStack() const;

      /* Dump the stack to the screen */
      StatusCode dump() const;

      /** Register an array of SimulationSelectors */
      StatusCode registerSimSelector(SimSelectorToolArray &simSelectorTools, AtlasDetDescr::AtlasRegion geoID);

    private:
      /** Default constructor */
      ParticleBrokerDynamicOnReadIn();

      /** setup the ROOT TTrees for THistSvc in validation mode */
      StatusCode registerPosValTree( const char *treeName,
                                     const char *treeDescr,
                                     TTree      *&tree );

      /** fill the TTree with the (x,y,z) coordinates of the given ISFParticle */
      void fillPosValTree( TTree *tree, const ISFParticle &particle);

      /** update all unfrozen simSelectors with the given particle */
      void updateAllSelectors(const ISFParticle &particle);

      /** go through the chain of SimulationSelectors and let the first one which selects the particle
          decide which simulation the particle will be assigned to */
      void selectAndStore( ISFParticle* p);

      /** go through the chain of SimulationSelectors and return the SimulatoID of the first
          SimulationSelector that selects the particle */
      ISF::SimSvcID identifySimID( const ISF::ISFParticle* p);

      /** AthenaTool responsible for writing Calo/Muon Entry/Exit Layer collection */
      ToolHandle<IEntryLayerTool>               m_entryLayerTool;

      /** AthenaTool responsible for proritizing the particles and determine their simulation order */
      ToolHandle<IParticleOrderingTool>         m_orderingTool;
      bool                                      m_hasOrderingTool;

      /** the geo identifier service used to route the particle into the right
          SimulationSelector chain */
      ServiceHandle<IGeoIDSvc>                  m_geoIDSvc;
      IGeoIDSvc                                *m_geoIDSvcQuick;         //!< minimize Gaudi overhead

      /** always use GeoIDSvc to determine GeoID of given particles */
      bool                                      m_forceGeoIDSvc;

      /** always use GeoIDSvc to check correctness of GeoIDs already assigned to particles */
      bool                                      m_validateGeoID;
         
      /** the particle container storing all particles which need to be simulated */
      ISFParticleOrderedQueue                   m_particles;

      /** the vector of particles returned for simulation (via popVector() ) */
      ConstISFParticleVector                    m_popParticles;

      /** the simulation selectors per geoID (the actual routing chain) */
      SimSelectorArray                          m_simSelector[AtlasDetDescr::fNumAtlasRegions]; //!< selectors per geoID
      SimSelectorSet                            m_simSelectorSet; //!< used to remove multiple uses of the same selector
     
      /** Screen output refinement */
      std::string                               m_screenOutputPrefix;
      std::string                               m_screenEmptyPrefix;

      /** barcode service */
      ServiceHandle<Barcode::IBarcodeSvc>       m_barcodeSvc; //!< barcode service      

       /** Benchmarking */
      bool                                      m_doSelectorCPUMon; //!< whether we use PMonUtils or not
      PMonUtils::CustomBenchmark               *m_benchPDGCode;
      PMonUtils::CustomBenchmark               *m_benchGeoID;

      /** validation mode: create ROOT Tree with additional information */
      bool                                      m_validationOutput; //!< turn validation mode on/off
      ServiceHandle<ITHistSvc>                  m_thistSvc;         //!< the histogram service
      std::string                               m_validationStream; //!< validation THist stream name
      /** the actual validation histograms and ROOT trees */
      TTree                                    *m_t_pushPosition;
      TTree                                    *m_t_entryLayerPos[ISF::fNumAtlasEntryLayers];
      /** memory containing the entries for the ROOT tree */
      int                                       m_val_pdg;
      float                                     m_val_p;
      float                                     m_val_x, m_val_y, m_val_z;
  };

  /** Get the current stack size */
  inline size_t ParticleBrokerDynamicOnReadIn::numParticles() const { return m_particles.size(); }
  
  /** Return the particle stack (not implemented) */
  /* inline const ParticleContainer& ParticleBrokerDynamicOnReadIn::particleStack() const
  { 
    // @TODO:
    //ATH_MSG_WARNING("TODO: the ParticleBrokerDynamicOnReadIn::particleStack() method only returns the active part of the partilce stack");
    return m_activeStack;
  } */
  
}

#endif //> !ISF_SERVICES_PARTICLEBROKERDYNAMICONREADIN_H

