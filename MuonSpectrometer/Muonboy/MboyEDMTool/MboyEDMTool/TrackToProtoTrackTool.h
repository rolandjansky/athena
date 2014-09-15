/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrackToProtoTrackTool_H
#define TrackToProtoTrackTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MboyEDMToolInterfaces/ITrackToProtoTrackTool.h"

//Can not be forwarded due to typedef
#include "TrkTrack/Track.h"

class MbProtoTrack ;
class MuonRecDigiStoreAth;

namespace Trk {
  class IMeasurementBaseToDratTool ;
}

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

namespace Trk {

  /**
   @class TrackToProtoTrackTool
   
  @author samusog@cern.ch
  
  */

class TrackToProtoTrackTool:public AthAlgTool, virtual public ITrackToProtoTrackTool{
public:
    TrackToProtoTrackTool(const std::string&,const std::string&,const IInterface*);
    virtual ~TrackToProtoTrackTool 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /** Convert single object */
   virtual MbProtoTrack* convert (
                    MuonRecDigiStoreAth*     pMuonRecDigiStoreAth ,
                    const Trk::Track* pTrack
                               );

   /** Update fit parameters */
   virtual MbDRAT*  MbDRATToUpdateFitParameters(
                    MuonRecDigiStoreAth*     pMuonRecDigiStoreAth ,
                    const Trk::Track* pTrack
                               );

private:
///////////////////////////////////

   ToolHandle< Trk::IMeasurementBaseToDratTool > p_IMeasurementBaseToDratTool ; //!< Pointer On IMeasurementBaseToDratTool

};
}

#endif

