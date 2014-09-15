/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MeasurementBaseToDratTool_H
#define MeasurementBaseToDratTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MboyEDMToolInterfaces/IMeasurementBaseToDratTool.h"

//Can not be forwarded due to typedef
#include "TrkTrack/Track.h"

namespace Trk {
  class MeasurementBase ;
  class RIO_OnTrack;
}

class MbDRAT ;
class MuonRecDigiStoreAth;

class MbIDTagMDT    ;
class MbIDTagCSCclu ;
class MbIDTagRPC ;
class MbIDTagTGC ;

class AtlasDetectorID ;
class MdtIdHelper ;
class RpcIdHelper ;
class TgcIdHelper ;
class CscIdHelper ;

#include "Identifier/Identifier.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

class IMboyEDMHelper;

namespace Trk {

  /**
   @class MeasurementBaseToDratTool
   
  @author samusog@cern.ch
  
  */

class MeasurementBaseToDratTool:public AthAlgTool, virtual public IMeasurementBaseToDratTool{
public:
    MeasurementBaseToDratTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MeasurementBaseToDratTool 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   virtual int  CouldBeMboySeed(const Trk::MeasurementBase* pMeasurementBase) ;
   virtual int  GetISC(const Trk::MeasurementBase* pMeasurementBase) ;
   virtual int  GetISC(const Trk::RIO_OnTrack* pRIO_OnTrack);
   virtual int  GetISC(const Identifier& channelId);
   virtual int  GetISC(int StationNber);
   virtual void MeasurementBaseToMbDRAT( 
                   MbDRAT* pMbDRAT,
                   MuonRecDigiStoreAth*     pMuonRecDigiStoreAth ,
                   const Trk::MeasurementBase* pMeasurementBase
                  );
   virtual void PickUpMeasurementBaseAndCluster(
                                       std::vector<int>* pVectorOfISC  ,
                                       std::vector< const Trk::TrackParameters* >* pVectorOfTrackParameters ,
                                       std::vector< std::vector<const Trk::MeasurementBase* > >* pVectorOfVectorOfMeasurementBase,
                                       const Trk::Track* pTrack
                                       );
   virtual void PickUpMeasurementBase(
                                       std::vector< const Trk::TrackParameters* >* pVectorOfTrackParameters ,
                                       std::vector< const Trk::MeasurementBase* >* pVectorOfMeasurementBase ,
                                       const Trk::Track* pTrack
                                       );
   virtual void PickUpMeasurementBase (
                                       std::vector< const Trk::TrackParameters* >* pVectorOfTrackParameters ,
                                       std::vector< const Trk::MeasurementBase* >* pVectorOfMeasurementBase,
                                       const Trk::TrackParameters* pTrackParameters,
                                       const Trk::MeasurementBase* pMeasurementBase
                                       );

private:
///////////////////////////////////

   MbIDTagMDT*    GetMbIDTagMDT   (const Identifier& channelId) ;
   MbIDTagCSCclu* GetMbIDTagCSCclu(const Identifier& channelId) ;
   MbIDTagRPC*    GetMbIDTagRPC   (const Identifier& channelId) ;
   MbIDTagTGC*    GetMbIDTagTGC   (const Identifier& channelId) ;
  
   int m_AddIdent ; //!< Control addition of ident
   
   const MdtIdHelper * m_mdtId ; //!< IdHelper
   const CscIdHelper * m_cscId ; //!< IdHelper
   const RpcIdHelper * m_rpcId ; //!< IdHelper
   const TgcIdHelper * m_tgcId ; //!< IdHelper

   const AtlasDetectorID* m_detID ; //!< Pointer On AtlasDetectorID

   ToolHandle< IMboyEDMHelper > p_OwnEDMHelper ; //!< Pointer On IMboyEDMHelper

};
}

#endif

