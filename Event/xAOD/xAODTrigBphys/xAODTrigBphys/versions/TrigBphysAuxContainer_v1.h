// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigBphysAuxContainer_v1.h 600800 2014-06-08 12:57:36Z jwalder $
#ifndef XAODTRIGBPHYS_VERSIONS_TRIGBPHYSAUXCONTAINER_V1_H
#define XAODTRIGBPHYS_VERSIONS_TRIGBPHYSAUXCONTAINER_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"
#include "xAODTrigBphys/versions/TrigBphysContainer_v1.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "AthLinks/ElementLink.h"

namespace xAOD {
    
    /// Auxiliary store for the TrigBphys composite container
    ///
    /// This auxiliary container can describe the properties of a container
    /// of TrigBphys
    ///
    /// @author James Walder <jwalder@cern.ch>
    ///
    /// $Revision: 600800 $
    /// $Date: 2014-06-08 14:57:36 +0200 (Sun, 08 Jun 2014) $

    class TrigBphysAuxContainer_v1 : public AuxContainerBase {
        
    public:
        /// Default constuctor
        TrigBphysAuxContainer_v1();

    private:
        std::vector<uint32_t>   roiId;
        std::vector<uint16_t> particleType;
        std::vector<uint16_t> level;

        std::vector<float> eta;
        std::vector<float> phi;
        std::vector<float> mass;
        std::vector<float> fitmass;
        std::vector<float> fitchi2;
        std::vector<int>   fitndof;
        std::vector<float> fitx;
        std::vector<float> fity;
        std::vector<float> fitz;

        std::vector<ElementLink< xAOD::TrigBphysContainer_v1 > > secondaryDecayLink;
        // @todo update to IParticles
        std::vector<std::vector<ElementLink<xAOD::TrackParticleContainer> > > trackParticleLinks;

    }; // TrigBphysAuxContainer_v1

} // namespace xAOD

// Set up a CLID and StoreGate inheritance for the class:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigBphysAuxContainer_v1, 1093324651, 1 ) // FIXME - JW what clid to use?
#include "SGTools/BaseInfo.h"
SG_BASE( xAOD::TrigBphysAuxContainer_v1, xAOD::AuxContainerBase );
#endif // not XAOD_STANDALONE



#endif // XAODTRIGBPHYS_VERSIONS_TRIGBPHYSAUXCONTAINER_V1_H
