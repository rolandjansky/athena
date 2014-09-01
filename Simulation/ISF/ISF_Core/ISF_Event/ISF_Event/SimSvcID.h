/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SimSvcID.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_EVENT_SIMSVCID_H
#define ISF_EVENT_SIMSVCID_H 1

// stl includes
#include <limits>

// Gaudi Kernel
#include "GaudiKernel/MsgStream.h"

// use these macros to check a given SimSvcID for its validity
#define validSimID(simID) ( (simID<ISF::fMaxNumAtlasSimIDs) && (simID>=ISF::fFirstAtlasSimID) )
#define assertSimID(simID) ( assert(validSimGeoID(simID)) )

namespace ISF {

 /** @enum SimSvcID
   
     @author Andreas.Salzburger -at- cern.ch , Elmar.Ritsch -at- cern.ch
   */

   /** Simulation service ID datatype */
   typedef uint8_t SimSvcID;

   enum SimSvcIDs {       
        // Not yet set, not decided
        fUndefinedSimID      = ISF::SimSvcID(  0  ),
        // Event Generator (not actually a simulator per se but may be used for routing decisions)
        fEventGeneratorSimID = ISF::SimSvcID(  1  ),
        // to be used for space allocation (e.g. simulator arrays)
        fFirstAtlasSimID     = ISF::SimSvcID(  2  ),

        // to be used to allocate space (e.g. simulator arrays)
        fMaxNumAtlasSimIDs   = ISF::SimSvcID( 32 )
   };

   /** Overload of << operator for correct output on MsgStream and std::ostream */
   inline MsgStream& operator << ( MsgStream& sl, const SimSvcID& id) { sl << (int)(id); return sl; }
   inline std::ostream& operator << ( std::ostream& sl, const SimSvcID& id) { sl << (int)(id) ; return sl; }
  
} // end of namespace

#endif  // ISF_EVENT_SIMSVCID_H
