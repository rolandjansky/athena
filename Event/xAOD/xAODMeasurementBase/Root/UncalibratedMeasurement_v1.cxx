/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"
// Local include(s):
#include "xAODMeasurementBase/versions/UncalibratedMeasurement_v1.h"

static const SG::AuxElement::Accessor< IdentifierHash::value_type > identifierHashAcc( "identifierHash" );

void xAOD::UncalibratedMeasurement_v1::setIdentifierHash(IdentifierHash& id) {
    identifierHashAcc(*this) = id.value();
}

const IdentifierHash xAOD::UncalibratedMeasurement_v1::identifierHash() const {
    IdentifierHash::value_type id = identifierHashAcc(*this);
    return IdentifierHash(id);
}

