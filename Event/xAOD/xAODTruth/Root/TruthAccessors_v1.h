// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthAccessors_v1.h 623284 2014-10-22 14:07:48Z krasznaa $
#ifndef XAODTRUTH_TRUTHACCESSORS_V1_H
#define XAODTRUTH_TRUTHACCESSORS_V1_H

// EDM include(s):
#include "AthContainers/AuxElement.h"

// Local include(s):
#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthParticle.h"

namespace xAOD {

   /// This function holds on to Accessor objects that can be used by each
   /// TruthParticle_v1 object at runtime to get/set parameter values on
   /// themselves.
   const SG::AuxElement::Accessor< float >*
   polarizationAccessorV1( TruthParticle_v1::PolParam type );

   /// Helper function for getting accessors for integer type PDF information
   const SG::AuxElement::Accessor< int >*
   pdfInfoAccessorV1Int( TruthEvent_v1::PdfParam type );

   /// Helper function for getting accessors for floating point PDF information
   const SG::AuxElement::Accessor< float >*
   pdfInfoAccessorV1Float( TruthEvent_v1::PdfParam type );

   /// Helper function for getting accessors for integer type HI information
   const SG::AuxElement::Accessor< int >*
   heavyIonAccessorV1Int( TruthEvent_v1::HIParam type );

   /// Helper function for getting accessors for floating point HI information
   const SG::AuxElement::Accessor< float >*
   heavyIonAccessorV1Float( TruthEvent_v1::HIParam type );

} // namespace xAOD

#endif // XAODTRUTH_TRUTHACCESSORS_V1_H
