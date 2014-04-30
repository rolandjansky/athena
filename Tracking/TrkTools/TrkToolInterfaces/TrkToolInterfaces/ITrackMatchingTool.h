/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrkToolInterfaces_ITrackMatchingTool_H
#define TrkToolInterfaces_ITrackMatchingTool_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkEventUtils/TrackMatchingMultiMap.h"

namespace Trk
{

 static const InterfaceID IID_ITrackMatchingTool("Trk::ITrackMatchingTool", 1, 0);

 class Track;


 /** @brief The abstract interface base class for Track matching tools.

     The goal is to match track collections at a particular boundary surface.

     @author Manuel Kayl, November 2009
 */

 
 class ITrackMatchingTool : virtual public IAlgTool
 {
   public:

    static const InterfaceID& interfaceID() 
    {
     return IID_ITrackMatchingTool;
    }
  
    
  
    virtual Trk::TrackMatchingProperties doMatchingOfTracks(const Trk::Track& innerTrack, const Trk::Track& outerTrack) const =0;
    
 };//end of class definitions
}//end of namespace definitions


#endif
