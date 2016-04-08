// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODPRIMITIVES_ISOLATIONHELPERS_H
#define XAODPRIMITIVES_ISOLATIONHELPERS_H

#include "xAODPrimitives/IsolationType.h"
#include "xAODPrimitives/IsolationConeSize.h"
#include "xAODPrimitives/IsolationFlavour.h"
#include <cmath>
#include <cassert>
#include <iostream>

namespace xAOD {

   namespace Iso {

     /// convert Isolation Type into cone size
     inline IsolationConeSize enumconeSize( IsolationType type ){
       return static_cast<IsolationConeSize>(type%10);
     }

     /// convert Isolation Size into cone size
     inline float coneSize( IsolationConeSize type ){
       return 0.1 + (static_cast<int>(type))*0.05;
     }
     
     /// convert float (e.g. 0.3) into IsolationConeSize. An assert is thrown  
     inline IsolationConeSize coneSize( float size ){
       int index = static_cast<int>(size*20)-2; // *20 equiv. to /0.05;
       assert ( (index>-1) && (index < static_cast<int>(numIsolationConeSizes) )  );
       IsolationConeSize rel = static_cast<IsolationConeSize>(index);
       assert ( fabs(size-coneSize(rel) )<0.0001 ); // this will fail if the input size is 0.3001...
       return rel;
     }

     /// convert Isolation Type into cone size
     inline float coneSize( IsolationType type ){
       IsolationConeSize size = enumconeSize(type);
       return coneSize(size);
     }

     /// convert Isolation Type into Isolation Flavour
     inline IsolationFlavour isolationFlavour( IsolationType type ){
       int subtype = type/10;
       if( subtype >= static_cast<int>(numIsolationFlavours) ) return numIsolationFlavours;
       return static_cast<IsolationFlavour>(subtype);
     }

     // create a IsolationType from IsolationFlavour and IsolationConeSize
     inline IsolationType isolationType( IsolationFlavour flavour, IsolationConeSize coneSize ) {
       return static_cast<IsolationType>( 10*static_cast<int>(flavour) + static_cast<int>(coneSize) );
     }
   
   } // namespace Iso


} // namespace xAOD

#endif // XAODPRIMITIVES_ISOLATIONHELPERS_H

