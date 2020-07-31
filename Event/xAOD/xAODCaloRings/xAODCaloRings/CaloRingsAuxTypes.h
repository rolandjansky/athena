// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODCALORINGS_CALORINGSAUXTYPES_H
#define XAODCALORINGS_CALORINGSAUXTYPES_H

// Local include(s).
#include "xAODCaloRings/CaloRingsContainer.h"

// EDM include(s).
#include "AthContainers/AuxElement.h"

namespace xAOD {

/// The CaloRings element links decorator type:
typedef SG::AuxElement::Decorator< xAOD::CaloRingsLinks > caloRingsDeco_t;
/// The CaloRings element links reader type:
typedef SG::AuxElement::ConstAccessor< xAOD::CaloRingsLinks > caloRingsReader_t;

} // namespace xAOD

#endif // XAODCALORINGS_CALORINGSAUXTYPES_H
