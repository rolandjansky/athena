/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMeasurementBaseToDratTool_H
#define IMeasurementBaseToDratTool_H

#include "GaudiKernel/IAlgTool.h" 

//Can not be forwarded due to typedef
#include "TrkTrack/Track.h"

namespace Trk {
  class MeasurementBase ;
  class RIO_OnTrack;
}

class MbDRAT ;
class MuonRecDigiStoreAth;

#include "Identifier/Identifier.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"

namespace Trk {

  /**
   @class IMeasurementBaseToDratTool
   
  @author samusog@cern.ch
  
  */

static const InterfaceID IID_IIMeasurementBaseToDratTool("IMeasurementBaseToDratTool", 1, 0);

class IMeasurementBaseToDratTool:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   virtual int  CouldBeMboySeed(const Trk::MeasurementBase* pMeasurementBase) = 0;
   virtual int  GetISC(const Trk::MeasurementBase* pMeasurementBase) = 0;
   virtual int  GetISC(const Trk::RIO_OnTrack* pRIO_OnTrack) = 0;
   virtual int  GetISC(const Identifier& channelId) = 0;
   virtual int  GetISC(int StationNber) = 0;
   virtual void MeasurementBaseToMbDRAT( 
                   MbDRAT* pMbDRAT,
                   MuonRecDigiStoreAth*     pMuonRecDigiStoreAth ,
                   const Trk::MeasurementBase* pMeasurementBase
                  ) = 0;
   virtual void PickUpMeasurementBaseAndCluster(
                                       std::vector<int>* pVectorOfISC  ,
                                       std::vector< const Trk::TrackParameters* >* pVectorOfTrackParameters ,
                                       std::vector< std::vector<const Trk::MeasurementBase* > >* pVectorOfVectorOfMeasurementBase,
                                       const Trk::Track* pTrack
                  ) = 0;
   virtual void PickUpMeasurementBase(
                                       std::vector< const Trk::TrackParameters* >* pVectorOfTrackParameters ,
                                       std::vector< const Trk::MeasurementBase* >* pVectorOfMeasurementBase ,
                                       const Trk::Track* pTrack
                  ) = 0;
   virtual void PickUpMeasurementBase (
                                       std::vector< const Trk::TrackParameters* >* pVectorOfTrackParameters ,
                                       std::vector< const Trk::MeasurementBase* >* pVectorOfMeasurementBase,
                                       const Trk::TrackParameters* pTrackParameters,
                                       const Trk::MeasurementBase* pMeasurementBase
                  ) = 0;
};

inline const InterfaceID& Trk::IMeasurementBaseToDratTool::interfaceID(){return IID_IIMeasurementBaseToDratTool;}

}

#endif

