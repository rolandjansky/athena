/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMuonboyToTrackParametersTool_H
#define IMuonboyToTrackParametersTool_H

#include "GaudiKernel/IAlgTool.h" 

namespace Trk {
  class  Surface;
}

//Can not be forwarded due to typedef
#include "TrkParameters/TrackParameters.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"

class MbPOT;

static const InterfaceID IID_IMuonboyToTrackParametersTool("Trk::IMuonboyToTrackParametersTool", 1, 0);

namespace Trk {

  /**
   @class IMuonboyToTrackParametersTool

   This tools converts MbPOT objects into TrackParameters
   
  @author samusog@cern.ch
  
  */

class IMuonboyToTrackParametersTool:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   /** compute the TrackParameters associated to a MbPOT, correct for small distance to the PrepRawData surface */
   virtual void DoTheTrackParametersCorrected(
                             int DoCorrectPosition ,
                             int DoDbg ,
                             const MbPOT* pMbPOT                 ,
                             const Trk::Surface*& pSurface,
                             const Trk::TrackParameters*& pTrackParameters 
                            ) = 0 ;

   /** compute the TrackParameters associated to a MbPOT, correct for small distance to the PrepRawData surface */
   virtual void DoTheTrackParametersCorrectedSurface(
                             const Trk::Surface* pSurface,
                             double XdcaL  ,
                             double YdcaL  ,
                             double ZdcaL  ,
                             double XdirL  ,
                             double YdirL  ,
                             double ZdirL  ,
                             double TheMom ,  
                             double TheCharge  ,
                             const TrackParameters*& pTrackParameters 
                            ) = 0 ;
};

inline const InterfaceID& Trk::IMuonboyToTrackParametersTool::interfaceID() { return IID_IMuonboyToTrackParametersTool;}

}

#endif

