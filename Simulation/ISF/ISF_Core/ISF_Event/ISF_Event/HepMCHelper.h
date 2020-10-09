/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HepMCHelper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_EVENT_HEPMCHELPER_H
#define ISF_EVENT_HEPMCHELPER_H 1

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"

// HepMC includes
#include "AtlasHepMC/GenRanges.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/IteratorRange.h"

// forward declarations
#include "AtlasHepMC/GenParticle_fwd.h"

namespace ISF {

  /** @class HepMCHelper
  
      Simplistic Particle cut filter
  
      @author Elmar.Ritsch -at- cern.ch
     */
  class HepMCHelper {

    public: 
      /** Constructor */
      inline HepMCHelper();

      /** virtual Destructor */
      inline virtual ~HepMCHelper();

      /** convert an integer into HepMC::IteratorRange
           0 -> HepMC::parents
           1 -> HepMC::family
           2 -> HepMC::ancestors
           3 -> HepMC::relatives
       */
      static inline HepMC::IteratorRange convertIteratorRange( int intItRange );

      /** check the incoming particle history to determine whether
          at least one particle with one of the given PDG codes appears.
          returns pointer to first found particle that matches any of the
          given PDG codes in relativesPDG */
      static inline HepMC::ConstGenParticlePtr findRealtiveWithPDG( HepMC::ConstGenParticlePtr genParticle,
                                                                    const HepMC::IteratorRange &relation,
                                                                    const std::set<int> &relativesPDG     );
  };

}

// include inline functions file
#include "ISF_Event/HepMCHelper.icc"

#endif //> !ISF_EVENT_HEPMCHELPER_H
