/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISFBenchmarkHelper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_EVENT_ISFBENCHMARKHELPER_H
#define ISF_EVENT_ISFBENCHMARKHELPER_H 1

// ISF Includes
#include "ISFParticle.h"

// abs() definition
#include <cstdlib>

namespace ISF {

 /** @enum 

     This enum enhaces readablity for PDGCode profiling with the
     PMonUtils::CustomBenchmarkGuard

     @author Elmar.Ritsch -at- cern.ch
   */

   enum PDGCodeEnum {
     // usually used for codes exceeding the maximum value:
       fUndefinedPDGCode    = 0,
     // defining the maximum value of pdg codes used in profiling
       fMaxBenchmarkPDGCode = 5000
   };


 /** @class 

     ISFBenchmarkHelper class enhances readability for
     PMonUtils::CustomBenchmarkGuard usage with ISF::ISFParticle types

     @author Elmar.Ritsch -at- cern.ch
   */

   class ISFBenchmarkHelper {

     public:
       /** empty constructor */
       ISFBenchmarkHelper()  { }
       /** empty destructor */
       ~ISFBenchmarkHelper() { }

       /** get PMonUtils::CustomBenchmarkGuard ready PDGCode of the given ISFParticle
             -> returns abs(pdgCode) if it does not exceed fMaxBenchmarkPDGCode
             -> returns fUndefinedPDGCode otherwise */
       inline static unsigned int getBenchReadyPdgCode(const ISFParticle &p) {
          unsigned int pdgCode = abs( p.pdgCode() );
          return ( (pdgCode<=int(ISF::fMaxBenchmarkPDGCode) ) ? pdgCode : int(ISF::fUndefinedPDGCode) );
       }
     private:

   };

} // end of namespace

#endif  // ISF_EVENT_ISFBENCHMARKHELPER_H
