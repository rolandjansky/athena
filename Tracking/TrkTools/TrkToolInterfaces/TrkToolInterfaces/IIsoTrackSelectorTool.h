/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrkToolInterfaces_IIsoTrackSelectorTool_H
#define TrkToolInterfaces_IIsoTrackSelectorTool_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"


namespace Trk
{
 
 class Track;
 class TrackParticleBase;
 
 static const InterfaceID IID_IIsoTrackSelectorTool("Trk::IIsoTrackSelectorTool", 1, 0);

 /** @brief The abstract interface base class for track selector tools targeted at isoloation.

     A track selector tool is designed to judge the track quality wrt to a given reference.

     @author Andreas Salzburger
 */

 
 class IIsoTrackSelectorTool : virtual public IAlgTool
 {
   public:
    
    static const InterfaceID& interfaceID() { return IID_IIsoTrackSelectorTool; }

    /** ESD type interface */
    virtual bool decision(const Trk::AtaStraightLine&, const Trk::Track& track) const = 0;

    /** AOD type interface */
    virtual bool decision(const Trk::AtaStraightLine&, const Trk::TrackParticleBase& trackParticle) const = 0;
  
    /** Work-horse interface */
    virtual bool decision(const Trk::AtaStraightLine&, const Trk::TrackParameters& trackPars) const = 0;
  
  
 };//end of class definitions
}//end of namespace definitions


#endif
