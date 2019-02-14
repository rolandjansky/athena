// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterAccessors_v1.h 579810 2014-01-23 16:08:13Z krasznaa $
#ifndef XAODCALOEVENT_CALOCLUSTERACCESSORS_V1_H
#define XAODCALOEVENT_CALOCLUSTERACCESSORS_V1_H

// Local include(s):
#include "xAODCaloEvent/versions/CaloCluster_v1.h"

namespace xAOD {

   /// Helper function for managing cluster moment Accessor objects
   ///
   /// This function holds on to Accessor objects that can be used by each
   /// CaloCluster_v1 object at runtime to get/set moment values on themselves.
   ///
   /// @param moment The cluster moment for which an Accessor should be returned
   /// @returns A pointer to an Accessor if successful, <code>0</code> if not
   ///
   const SG::AuxElement::Accessor< float >*
   momentAccessorV1( xAOD::CaloCluster_v1::MomentType moment );

} // namespace xAOD

#endif // XAODCALOEVENT_CALOCLUSTERACCESSORS_V1_H
