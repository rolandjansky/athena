/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////
// IVertexTrackUpdator, (c) ATLAS Inner Detector Software 2005 
///////////////////////////////////////////////////////////////

#ifndef TRKVERTEXFITTERINTERFACE_IVERTEXTRACKUPDATOR_H
#define TRKVERTEXFITTERINTERFACE_IVERTEXTRACKUPDATOR_H

#include "GaudiKernel/IAlgTool.h"

/**
 * @class Trk::IVertexTrackUpdator
 * An abstract base class for the vertex track updators
 * Updates the track with the knowledge of the 
 * position of the production vertex. 
 * Concrete implementations  can be many. In general, used by
 * Trk::IVertexSmoother-derived classes for sequential smoothing.
 *  
 * @author Kirill Prokofiev, Kirill.Prokofiefiev@cern.ch  November 2005
 */

namespace Trk
{
 static const InterfaceID IID_IVertexTrackUpdator("IVertexTrackUpdator", 1, 0);
 
 class RecVertex;
 class VxTrackAtVertex;

 class IVertexTrackUpdator : virtual public IAlgTool 
 {
   public:
   
   /**
    * Virtual destructor
    */ 
     virtual ~IVertexTrackUpdator(){}
     
   /**
    * AlgTool interface methods 
    */
    static const InterfaceID& interfaceID() { return IID_IVertexTrackUpdator; };

   
   /**
    * Actual update method
    */   
   virtual void  update(VxTrackAtVertex& trk, const RecVertex& vtx) const = 0;
   
 };

}//end of namespace definitions

#endif
