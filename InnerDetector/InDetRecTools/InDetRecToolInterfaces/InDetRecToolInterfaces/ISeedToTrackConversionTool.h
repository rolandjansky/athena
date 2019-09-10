// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISeedToTrackConversionTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IINDETSEEDTOTRACKCONVERSIONTOOL_H
#define IINDETSEEDTOTRACKCONVERSIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSpacePoint/SpacePoint.h"
#include "TrkTrack/TrackInfo.h"

class MsgStream;

namespace InDet 
{
  class SeedToTrackConversionData;

  class ISeedToTrackConversionTool : virtual public IAlgTool {
  public:
    DeclareInterfaceID(ISeedToTrackConversionTool, 1, 0);
    virtual void executeSiSPSeedSegments(SeedToTrackConversionData& data, const Trk::TrackParameters*,const int&, const std::list<const Trk::SpacePoint*>&) const =0;
    virtual void newEvent(SeedToTrackConversionData& data, const Trk::TrackInfo&, const std::string&) const =0;
    virtual void endEvent(SeedToTrackConversionData& data) const =0;

    //////////////////////////////////////////////////////////////////
    // Print internal tool parameters and status
    ///////////////////////////////////////////////////////////////////
      
    virtual MsgStream& dump(SeedToTrackConversionData& data, MsgStream& out) const=0;

  };
  
} // end of namespace

#endif 
