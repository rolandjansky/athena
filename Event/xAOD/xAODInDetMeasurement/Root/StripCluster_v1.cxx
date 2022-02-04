/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"
// Local include(s):
#include "xAODInDetMeasurement/versions/StripCluster_v1.h"

static const SG::AuxElement::Accessor< std::array< float, 3 > > globalPosAcc( "globalPosition" );
static const SG::AuxElement::Accessor< std::vector< Identifier::value_type > >  rdoListAcc( "rdoList" );

xAOD::StripCluster_v1::ConstVectorMap xAOD::StripCluster_v1::globalPosition() const {
    const auto& values = globalPosAcc(*this);
    return ConstVectorMap{values.data()};
}

xAOD::StripCluster_v1::VectorMap xAOD::StripCluster_v1::globalPosition() {
    auto& values = globalPosAcc(*this);
    return VectorMap{values.data()};
}

void xAOD::StripCluster_v1::setRDOlist(const std::vector< Identifier >& rdoList) {
    std::vector< Identifier::value_type > rdos;
    rdos.reserve(rdoList.size());
    for (auto& rdo : rdoList)
        rdos.push_back(rdo.get_compact());
    rdoListAcc(*this) = rdos;
}

const std::vector< Identifier > xAOD::StripCluster_v1::rdoList() const {
    std::vector< Identifier::value_type > values = rdoListAcc(*this);
    std::vector< Identifier > rdos;
    rdos.reserve(values.size());
    for (auto& value : values)
        rdos.push_back(Identifier(value));
    return rdos;
}

AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( xAOD::StripCluster_v1, int,
                                      channelsInPhi, setChannelsInPhi)

AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( xAOD::StripCluster_v1, uint16_t,
                                      hitsInThirdTimeBin, setHitsInThirdTimeBin)
