/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHHELPER_GENACCESSIO_H
#define TRUTHHELPER_GENACCESSIO_H

#include "GeneratorObjects/McEventCollection.h"
#include "TruthHelper/GenIMCselector.h"
#include "AtlasHepMC/GenParticle.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "StoreGate/StoreGateSvc.h"

#include <vector>
#include <string>


typedef std::vector<HepMC::ConstGenParticlePtr> MCParticleCollection;
typedef std::vector<HepMC::ConstGenParticlePtr>::const_iterator MCParticleCollectionCIter;


namespace TruthHelper {


  /// @todo Can we convert to an Athena AthTool or similar? Or just replace with calls to MC::particles()
  /// @deprecated Use the functions in TruthUtils instead
  class GenAccessIO {
  public:

    GenAccessIO() : m_sgSvc(0) {
      if (Gaudi::svcLocator()->service("StoreGateSvc", m_sgSvc).isFailure()) {
        throw StatusCode::FAILURE;
      }
    }

    StatusCode getMC(MCParticleCollection& mc, const std::string& key="GEN_EVENT") const;
    StatusCode getMC(MCParticleCollection& mc,
                     const GenIMCselector* selector, const std::string& key="GEN_EVENT") const;

    StatusCode getDH(const McEventCollection*& dh) const {
      return m_sgSvc->retrieve(dh);
    }

    StatusCode getDH(const McEventCollection*& dh, const std::string& key) const {
      return m_sgSvc->retrieve(dh, key);
    }

    StatusCode store (McEventCollection* storee, const std::string& key) const {
      return m_sgSvc->record(storee, key);
    }


  private:

    StoreGateSvc* m_sgSvc;

  };


}

#endif
