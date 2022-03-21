/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODINDETMEASUREMENT_VERSIONS_STRIPCLUSTERAUXCONTAINER_V1_H
#define XAODINDETMEASUREMENT_VERSIONS_STRIPCLUSTERAUXCONTAINER_V1_H

#include <vector>

#include "xAODCore/AuxContainerBase.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"

namespace xAOD {
    /// Auxiliary store for strip clusters
    ///
    class StripClusterAuxContainer_v1 : public AuxContainerBase {
    public:
        /// Default constructor
        StripClusterAuxContainer_v1();

    private:
        /// @name Defining uncalibrated measurement parameters
        /// @{
        std::vector < IdentifierHash::value_type > identifierHash;
        std::vector < std::array < float, 3 > > localPosition;
        std::vector < std::array < float, 9 > > localCovariance;
        /// @}

        /// @name Defining strip cluster parameters
        /// @{
        std::vector < std::array < float, 3 > > globalPosition;
        std::vector < std::vector < Identifier::value_type > > rdoList;
        std::vector < int > channelsInPhi;
        std::vector < uint16_t > hitsInThirdTimeBin;
        /// @}
    };
}

// Set up the StoreGate inheritance for the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::StripClusterAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODINDETMEASUREMENT_VERSIONS_STRIPCLUSTERAUXCONTAINER_V1_H
