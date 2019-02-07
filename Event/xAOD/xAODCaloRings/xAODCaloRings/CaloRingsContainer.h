// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODCALORINGS_CALORINGSCONTAINER_H
#define XAODCALORINGS_CALORINGSCONTAINER_H

// STL include(s):
#include <vector>

// Core include(s):
#include "AthLinks/ElementLink.h"
#ifndef XAOD_STANDALONE
#   include "StoreGate/ReadDecorHandle.h"
#   include "StoreGate/WriteDecorHandle.h"
#endif // XAOD_STANDALONE

// Local include(s):
#include "xAODCaloRings/CaloRings.h"
#include "xAODCaloRings/versions/CaloRingsContainer_v1.h"

namespace xAOD {
/// Definition of the current "CaloRings container version"
typedef CaloRingsContainer_v1 CaloRingsContainer;
/// Declare element links vector
typedef std::vector< ElementLink< CaloRingsContainer > > CaloRingsLinks;
/// The CaloRings element links decorator type:
typedef SG::AuxElement::Decorator< xAOD::CaloRingsLinks > caloRingsDeco_t;
/// The CaloRings element links reader type:
typedef SG::AuxElement::ConstAccessor< xAOD::CaloRingsLinks > caloRingsReader_t;
#ifndef XAOD_STANDALONE
/// The CaloRings element links write decorator type:
template<typename T> using caloRingsDecoH_t = SG::WriteDecorHandle<T, CaloRingsLinks>;
/// The CaloRings element links write decorator type:
template<typename T> using caloRingsReaderH_t = SG::ReadDecorHandle<T, CaloRingsLinks>;
#endif // XAOD_STANDALONE
} // namespace xAOD

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CaloRingsContainer , 1146041125 , 1 )

#endif // XAODCALORINGS_CALORINGSCONTAINER_H
