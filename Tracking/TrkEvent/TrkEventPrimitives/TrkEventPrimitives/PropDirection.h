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
                            

   
     /** This is a steering enum to tell which material update stage:
         - preUpdate  : when reaching a layer before layer is resolved
         - fullUpdate : just pass through the layer
         - postUpdate : when leaving the layer 
     */
     enum MaterialUpdateStage 
     {
     
         preUpdate   = -1,
         fullUpdate  =  0,
         postUpdate  =  1
     };                            
                            
    
} // end of namespace

#endif // TRKEVENTPRIMITIVES_PROPDIRECTION_H

