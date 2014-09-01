/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenEventStackFiller.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_HEPMC_GENEVENTSTACKFILLER_H
#define ISF_HEPMC_GENEVENTSTACKFILLER_H 1

// STL includes
#include <string>
// ISF include
#include "ISF_Interfaces/IStackFiller.h"
// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

// forward declarations
namespace Barcode {
  class IBarcodeSvc;
}
namespace HepPDT {
  class ParticleDataTable;
}
namespace HepMC {
  class GenParticle;
}
class IPartPropSvc;
class PileUpMergeSvc;
class McEventCollection;

namespace ISF {

   class ISFParticle;
   class IGenEventManipulator;
   class IGenParticleFilter;

    /** make the code a bit cleaner by using an explicit datatype for the GenEvent manipulators */
    typedef ToolHandleArray<IGenEventManipulator>    GenEventManipulators;
    typedef ToolHandleArray<IGenParticleFilter>      GenParticleFilters;


  /** @class GenEventStackFiller

      Simple HepMC based particle stack filler with a simple particle filter.
      This tool is called in the initializeParticleStack() call to the IStackSvc.

      @author Andreas.Salzburger -at- cern.ch
     */
  class GenEventStackFiller : public AthAlgTool, virtual public IStackFiller {

    public:
      //** Constructor with parameters */
      GenEventStackFiller( const std::string& t, const std::string& n, const IInterface* p );

      // Athena algtool's Hooks
      StatusCode  initialize();
      StatusCode  finalize();

      /** Returns the Particle Stack, should register truth */
      StatusCode  fillStack(ISF::ISFParticleContainer& stack) const;

    private:
      /** get right GenParticle mass */
      double getParticleMass(const HepMC::GenParticle &p) const;

      /** loop over McEventCollection */
      StatusCode  processMergedColls(ISF::ISFParticleContainer& particleColl) const;
      StatusCode  processSingleColl(ISF::ISFParticleContainer& particleColl) const;
      StatusCode  mcEventCollLooper(ISF::ISFParticleContainer& particleColl, McEventCollection* mcCollection, int nPileupCounter=0) const;

      /** ParticlePropertyService and ParticleDataTable */
      ServiceHandle<IPartPropSvc>           m_particlePropSvc;          //!< particle properties svc to retrieve PDT
      const HepPDT::ParticleDataTable      *m_particleDataTable;        //!< PDT used to look up particle masses

      std::string                           m_inputMcEventCollection;   //!< name of input McEventCollection
      std::string                           m_outputMcEventCollection;  //!< name of output McEventCollection
      bool                                  m_recordOnlySimulated;      //!< record only simulated particles in output truth collection
      bool                                  m_useGeneratedParticleMass; //!< use GenParticle::generated_mass() in simulation

      GenEventManipulators                  m_genEventManipulators;     //!< event manipulators (executed in given order)
      GenParticleFilters                    m_genParticleFilters;       //!< HepMC::GenParticle filter

      ServiceHandle<Barcode::IBarcodeSvc>   m_barcodeSvc;               //!< the ISF barcode service
      ServiceHandle<PileUpMergeSvc>         m_pMergeSvc;

      mutable int     m_largestBc;
      mutable int     m_uniqueBc;
      mutable int     m_number_of_gen_minbias;
      mutable double  m_current_event_time;
      mutable int     m_current_event_index;
      mutable int     m_current_bcid;
      mutable double  m_bunch_spacing;
  };

}


#endif //> !ISF_HEPMC_GENEVENTSTACKFILLER_H
