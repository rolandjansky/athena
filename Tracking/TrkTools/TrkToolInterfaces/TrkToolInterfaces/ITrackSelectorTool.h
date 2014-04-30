/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrkToolInterfaces_ITrackSelectorTool_H
#define TrkToolInterfaces_ITrackSelectorTool_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexFwd.h"

namespace Trk
{
 
 static const InterfaceID IID_ITrackSelectorTool("Trk::ITrackSelectorTool", 1, 0);

 class Track;
 class TrackParticleBase;
 class Vertex;

  

 /** @brief The abstract interface base class for track selector tools.

     A track selector tool is designed to judge the track quality
     with respect to a given analysis/reconstruction procedure. 
     Most evident clients: various vertex reconstruction 
     algorithms, physics analysis packages.

     @author Kirill Prokofiev, October 2007
 */

 
 class ITrackSelectorTool : virtual public IAlgTool
 {
   public:

    static const InterfaceID& interfaceID() 
    {
     return IID_ITrackSelectorTool;
    }
  
    virtual bool decision(const Trk::Track& track,const Vertex* vertex=0) const =0;
  
    virtual bool decision(const Trk::TrackParticleBase& track,const Vertex* vertex=0) const =0;

    virtual bool decision(const xAOD::TrackParticle& track,const xAOD::Vertex* vertex=0) const =0;
  
 };//end of class definitions
}//end of namespace definitions


#endif
