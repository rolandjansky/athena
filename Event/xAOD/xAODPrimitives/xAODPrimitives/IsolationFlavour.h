// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IsolationType.h 618834 2014-09-27 16:40:39Z christos $
#ifndef XAODPRIMITIVES_ISOLATIONFLAVOUR_H
#define XAODPRIMITIVES_ISOLATIONFLAVOUR_H

namespace xAOD {

   namespace Iso {

     /// Enumeration for different ways of calculating isolation in xAOD files
     ///
     enum IsolationFlavour {
       /// Calorimeter isolation 
       etcone = 0,

       /// Track isolation
       ptcone = 1,

       /// Topo-cluster E<sub>T</sub>-sum 
       topoetcone = 2,

       ///mini isolation
       ptvarcone = 3,

       ///neutral eflow
       neflowisol = 4,

       numIsolationFlavours=5

     }; // enum IsolationFlavour

     static inline const char* toString(IsolationFlavour type) {
       switch (type) {
       case etcone:     return "etcone";
       case ptcone:     return "ptcone";
       case topoetcone: return "topoetcone";
       case ptvarcone:  return "ptvarcone";
       case neflowisol: return "neflowisol";
       default:         return "[Unknown IsolationFlavour]";	
       }
     }

   
   } // namespace Iso


} // namespace xAOD

#endif 

