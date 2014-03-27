/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITrackToProtoTrackTool_H
#define ITrackToProtoTrackTool_H

#include "GaudiKernel/IAlgTool.h" 

//Can not be forwarded due to typedef
#include "TrkTrack/Track.h"

class MbProtoTrack ;
class MuonRecDigiStoreAth;

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"

namespace Trk {

  /**
   @class ITrackToProtoTrackTool
   
  @author samusog@cern.ch
  
  */

static const InterfaceID IID_IITrackToProtoTrackTool("ITrackToProtoTrackTool", 1, 0);

class ITrackToProtoTrackTool:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   /** Convert single object */
   virtual MbProtoTrack* convert (
                    MuonRecDigiStoreAth*     pMuonRecDigiStoreAth ,
                    const Trk::Track* pTrack
                               ) = 0;

   /** Update fit parameters */
   virtual MbDRAT*  MbDRATToUpdateFitParameters(
                    MuonRecDigiStoreAth*     pMuonRecDigiStoreAth ,
                    const Trk::Track* pTrack
                               ) = 0;
};

inline const InterfaceID& Trk::ITrackToProtoTrackTool::interfaceID(){return IID_IITrackToProtoTrackTool;}

}

#endif

