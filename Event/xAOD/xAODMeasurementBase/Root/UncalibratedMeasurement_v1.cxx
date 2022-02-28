/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"
// Local include(s):
#include "xAODMeasurementBase/versions/UncalibratedMeasurement_v1.h"

static const SG::AuxElement::Accessor< Identifier::value_type > identifierAcc( "identifier" );

void xAOD::UncalibratedMeasurement_v1::setIdentifier(Identifier& id) {
    identifierAcc(*this) = id.get_compact();
}

const Identifier xAOD::UncalibratedMeasurement_v1::identifier() const {
    Identifier::value_type id = identifierAcc(*this);
    return Identifier(id);
}

