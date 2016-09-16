/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InputConverter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_INPUTCONVERTER_H
#define ISF_INPUTCONVERTER_H 1

// STL includes
#include <string>
// ISF include
#include "ISF_Interfaces/IInputConverter.h"
// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthService.h"

// forward declarations
namespace Barcode {
  class IBarcodeSvc;
}
namespace HepPDT {
  class ParticleDataTable;
}
namespace HepMC {
  class GenParticle;
  class GenEvent;
}
class IPartPropSvc;
class McEventCollection;
namespace ISFTesting {
  class InputConverter_test;
}
namespace ISF {
  class ISFParticle;
  class IGenParticleFilter;
}

namespace ISF {

  /** @class InputConverter

      Convert simulation input collections to ISFParticles for subsequent ISF simulation.

      @author Elmar.Ritsch -at- cern.ch
     */
  class InputConverter : public IInputConverter, public AthService {

    // allow test to access private data
    friend ISFTesting::InputConverter_test;

    public:
      InputConverter(const std::string& name, ISvcLocator* svc);
      virtual ~InputConverter();

      /** Athena algtool Hooks */
      StatusCode  initialize();
      StatusCode  finalize();

      /** ReturnGaudi InterfaceID */
      static const InterfaceID& interfaceID() { return IID_IInputConverter; }

      /** Convert selected particles from the given McEventCollection into ISFParticles
          and push them into the given ISFParticleContainer */
      virtual StatusCode convert(const McEventCollection& inputGenEvents,
                                 ISF::ISFParticleContainer& simParticles,
                                 bool isPileup=false) const override final;

      /** Query the interfaces. **/
      StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

    private:
      /** get right GenParticle mass */
      double getParticleMass(const HepMC::GenParticle& p) const;

      /** get all generator particles which pass filters */
      std::vector<HepMC::GenParticle*> getSelectedParticles(const HepMC::GenEvent& evnt, bool legacyOrdering=false) const;

      /** check if the given particle passes all filters */
      bool passesFilters(const HepMC::GenParticle& p) const;

      /** convert GenParticle to ISFParticle */
      ISF::ISFParticle* convertParticle(HepMC::GenParticle* genPartPtr, int bcid) const;

      /** ParticlePropertyService and ParticleDataTable */
      ServiceHandle<IPartPropSvc>           m_particlePropSvc;          //!< particle properties svc to retrieve PDT
      const HepPDT::ParticleDataTable      *m_particleDataTable;        //!< PDT used to look up particle masses

      bool                                  m_useGeneratedParticleMass; //!< use GenParticle::generated_mass() in simulation

      ToolHandleArray<IGenParticleFilter>   m_genParticleFilters;       //!< HepMC::GenParticle filters
  };

}


#endif //> !ISF_INPUTCONVERTER_H
