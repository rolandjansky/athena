/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//abstract interface

#ifndef __ITRIG_FTK_TRACK_CONVERTER_TOOL_H__
#define __ITRIG_FTK_TRACK_CONVERTER_TOOL_H__

#include "GaudiKernel/IAlgTool.h"
#include "TrkTrack/TrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"

#include <vector>

class FTKTrack;

static const InterfaceID IID_ITrigFTKTrackConverter("ITrigFTKTrackConverter",1,0);

/** @class ITrigFTKTrackConverter

    provides the abstract interface for FTK track conversion into Trk::Track and Pixel/SCT clusters

    @author D.Emeliyanov <http://consult.cern.ch/xwho>
*/

class ITrigFTKTrackConverter : virtual public IAlgTool { 

 public:
  /** other standard AlgTool methods */

  static const InterfaceID& interfaceID ()   //!< the Tool's interface
  {  return IID_ITrigFTKTrackConverter; }  	
  
  virtual StatusCode convert(const std::vector<FTKTrack*>&, InDet::PixelClusterContainer*, 
			     InDet::SCT_ClusterContainer*, TrackCollection*) = 0;
  virtual StatusCode convert(const TrackCollection*, TrigInDetTrackCollection*) = 0;
  virtual StatusCode addSiHitInfo(const FTKTrack*, TrigInDetTrack*)  = 0;
};

#endif
