/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//abstract interface

#ifndef __IFTK_VERTEXFINDER_TOOL_H__
#define __IFTK_VERTEXFINDER_TOOL_H__

#include "GaudiKernel/IAlgTool.h"	
//#include "AthenaKernel/IOVSvcDefs.h"
class VxContainer;
#include "TrigFTK_RawData/FTK_RawTrack.h"
#include "TrigFTK_RawData/FTK_RawTrackContainer.h"
#include "xAODTracking/VertexFwd.h"
//#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "TrkTrack/TrackCollection.h"
//#include "xAODTracking/TrackParticleContainerFwd.h"
//namespace Trk {
////  class Track;
//}
static const InterfaceID IID_IFTK_VertexFinderTool("IFTK_VertexFinderTool",1,0);

class IFTK_VertexFinderTool : virtual public IAlgTool { 

 public:
  /** other standard AlgTool methods */
  static const InterfaceID& interfaceID ()   //!< the Tool's interface
  {  return IID_IFTK_VertexFinderTool; }  	

 // virtual VxContainer* findVertex(const FTK_RawTrackContainer* trks) = 0;
  virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> findVertex(const FTK_RawTrackContainer* trks) = 0;
  virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> findVertex(const TrackCollection* trks) = 0;
 private:
};

#endif
