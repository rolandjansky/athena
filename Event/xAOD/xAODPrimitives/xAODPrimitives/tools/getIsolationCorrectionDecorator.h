// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: getIsolationAccessor.h 582418 2014-02-10 15:03:46Z krasznaa $
#ifndef XAODPRIMITIVES_TOOLS_GETISOLATIONCORRECTIONDECORATOR_H
#define XAODPRIMITIVES_TOOLS_GETISOLATIONCORRECTIONDECORATOR_H

// EDM include(s):
#include "AthContainers/AuxElement.h"

// Local include(s):
#include "xAODPrimitives/IsolationCorrection.h"
#include "xAODPrimitives/IsolationType.h"
#include "xAODPrimitives/IsolationFlavour.h"

namespace xAOD {
   /// Returns an decorator for the correction bitset corresponding to this IsolationType. 
   /// The bitset encodes which corrections were applied for this specific type - 
   /// use getIsolationCorrectionAccessor(Iso::IsolationType, Iso::IsolationCaloCorrection type) to get Accessors for the correction values.

   SG::AuxElement::Decorator< uint32_t >*
   getIsolationCorrectionBitsetDecorator( Iso::IsolationFlavour type );

   SG::AuxElement::Decorator< float >*
   getIsolationCorrectionDecorator( Iso::IsolationFlavour, Iso::IsolationCaloCorrection type, Iso::IsolationCorrectionParameter param );
   SG::AuxElement::Decorator< float >*
   getIsolationCorrectionDecorator( Iso::IsolationType, Iso::IsolationCaloCorrection type);
   SG::AuxElement::Decorator< float >*
   getIsolationCorrectionDecorator( Iso::IsolationFlavour, Iso::IsolationTrackCorrection type);   
} // namespace xAOD

#endif
