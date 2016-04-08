// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IsolationType.h 618834 2014-09-27 16:40:39Z christos $
#ifndef XAODPRIMITIVES_ISOLATIONCONESIZE_H
#define XAODPRIMITIVES_ISOLATIONCONESIZE_H

namespace xAOD {

   namespace Iso {

     /// Enumeration for different cone sizes 
     ///
     enum IsolationConeSize {
       cone10 = 0, 
       cone15 = 1,
       cone20 = 2,
       cone25 = 3,
       cone30 = 4,
       cone35 = 5,
       cone40 = 6,
       cone45 = 7,
       cone50 = 8,
       numIsolationConeSizes=9
     };
     
     static inline const char* toString(IsolationConeSize conesize)
     {
       switch (conesize)
       {
         case cone10:    return "cone10";
         case cone15:    return "cone15";
         case cone20:    return "cone20";             
         case cone25:    return "cone25";
         case cone30:    return "cone30";
         case cone35:    return "cone35";
         case cone40:    return "cone40";
         case cone45:    return "cone45";
         case cone50:    return "cone50";
         case numIsolationConeSizes:
         default:
           return "Unknown size";
       }
     }
   } // namespace Iso


} // namespace xAOD

#endif // XAODPRIMITIVES_ISOLATIONCONESIZE_H

