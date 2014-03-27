/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PropDirection.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_PROPDIRECTION_H
#define TRKEVENTPRIMITIVES_PROPDIRECTION_H

namespace Trk {

      /** @enum PropDirection
        PropDirection, enum for direction of the propagation.
        
         @author Andreas.Salzburger@cern.ch
        */
      enum  PropDirection {
                    alongMomentum    = 1,
                    oppositeMomentum =-1,
                    anyDirection     = 0,
                    mappingMode      = 2
                 };
     /** @enum SearDirection
       Simple enum for searching Surfaces
       */
                    
     enum SearchDirection { outside=1, inside=-1,
                            bothway=0, undefinedDirection=0 };
    
} // end of namespace

#endif // TRKEVENTPRIMITIVES_PROPDIRECTION_H

