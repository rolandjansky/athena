/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMboyDetectorLimitsTool_H
#define IMboyDetectorLimitsTool_H

#include "GaudiKernel/IAlgTool.h" 

//Can not be forwarded due to typedef
#include "TrkParameters/TrackParameters.h"


/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"


namespace Muon {

static const InterfaceID IID_IMboyDetectorLimitsTool("IMboyDetectorLimitsTool", 1, 0);

  /**
   @class IMboyDetectorLimitsTool

  @author samusog@cern.ch
  
  */

class IMboyDetectorLimitsTool:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   virtual int AreaTag(
                       const Trk::TrackParameters* pTrackParameters
                      ) = 0 ;

   virtual int AreaTag(
                       double XPosi  ,
                       double YPosi  ,
                       double ZPosi  
                      ) = 0 ;

   virtual int IsOnCylinder(
                       const Trk::TrackParameters* pTrackParameters
                      ) = 0 ;

};

inline const InterfaceID& Muon::IMboyDetectorLimitsTool::interfaceID() { return IID_IMboyDetectorLimitsTool;}

}

#endif

