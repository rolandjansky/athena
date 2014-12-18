// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: getIsolationAccessor.h 582418 2014-02-10 15:03:46Z krasznaa $
#ifndef XAODPRIMITIVES_TOOLS_GETISOLATIONCORRECTIONACCESSOR_H
#define XAODPRIMITIVES_TOOLS_GETISOLATIONCORRECTIONACCESSOR_H

// EDM include(s):
#include "AthContainers/AuxElement.h"

// Local include(s):
#include "xAODPrimitives/IsolationCorrection.h"
#include "xAODPrimitives/IsolationType.h"
#include "xAODPrimitives/IsolationFlavour.h"

namespace xAOD {
   /// Returns an accessor for the correction bitset corresponding to this IsolationType. 
   /// The bitset encodes which corrections were applied for this specific type - 
   /// use getIsolationCorrectionAccessor(Iso::IsolationType, Iso::IsolationCaloCorrection type) to get Accessors for the correction values.
   SG::AuxElement::Accessor< uint32_t >*
   getIsolationCorrectionBitsetAccessor( Iso::IsolationFlavour type );

   SG::AuxElement::Accessor< float >*
   getIsolationCorrectionAccessor( Iso::IsolationFlavour, Iso::IsolationCaloCorrection type, Iso::IsolationCorrectionParameter param );
   SG::AuxElement::Accessor< float >*
   getIsolationCorrectionAccessor( Iso::IsolationType, Iso::IsolationCaloCorrection type);
   SG::AuxElement::Accessor< float >*
   getIsolationCorrectionAccessor( Iso::IsolationFlavour, Iso::IsolationTrackCorrection type);   
} // namespace xAOD

#endif
