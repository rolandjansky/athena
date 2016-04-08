// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigBphysAuxContainer_v1.h 696633 2015-09-24 14:59:50Z jwalder $
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
    /// $Revision: 696633 $
    /// $Date: 2015-09-24 16:59:50 +0200 (Thu, 24 Sep 2015) $

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
        std::vector<float> pt;
        std::vector<float> mass;
        std::vector<float> fitmass;
        std::vector<float> fitchi2;
        std::vector<int>   fitndof;
        std::vector<float> fitx;
        std::vector<float> fity;
        std::vector<float> fitz;

        std::vector<float> lxy;
        std::vector<float> lxyError;
        std::vector<float> tau;
        std::vector<float> tauError;
        std::vector<float> fitmassError;

        
        std::vector<ElementLink< xAOD::TrigBphysContainer_v1 > > secondaryDecayLink;
        std::vector<std::vector<ElementLink<xAOD::TrackParticleContainer> > > trackParticleLinks;

        std::vector<std::vector<uint32_t> >   vecRoiIds;
        std::vector<ElementLink< xAOD::TrigBphysContainer_v1 > > lowerChainLink;
        std::vector<std::vector<ElementLink<xAOD::IParticleContainer> > > particleLinks;
                

    }; // TrigBphysAuxContainer_v1

} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TrigBphysAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRIGBPHYS_VERSIONS_TRIGBPHYSAUXCONTAINER_V1_H
