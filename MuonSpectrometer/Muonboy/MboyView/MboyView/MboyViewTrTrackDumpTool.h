/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyViewTrTrackDumpTool_H
#define MboyViewTrTrackDumpTool_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"

#include "TrkMeasurementBase/MeasurementBase.h"

class AtlasDetectorID ;
class MdtIdHelper ;
class RpcIdHelper ;
class TgcIdHelper ;
class CscIdHelper ;

/////////////////////////////////////////////////////////////////////////////
#include "MboyView/MboyViewMisc.h"

  /**
   @class MboyViewTrTrackDumpTool

   This class 
    is a tool to dump Trk::Track content
    
  @author samusog@cern.ch
  
  */

class MboyViewTrTrackDumpTool:public AthAlgTool{
public:
    MboyViewTrTrackDumpTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyViewTrTrackDumpTool 	 ();

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();
   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /**View */
   void ViewTrkTrack(
               const Trk::Track* pTrack ,
               std::ofstream* pOut
              );
   void ViewMeasurementBase(
               const Trk::MeasurementBase* pMeasurementBase ,
               std::ofstream* pOut
              );
   void ViewTrackParameters(
               const Trk::TrackParameters* pTrackParameters ,
               std::ofstream* pOut
              );

private:
///////////////////////////////////

   int m_DoCom    ; //!< Message Control 

   int m_KounterErrorMessage    ; //!< Message Control 
   int m_KounterErrorMessageMax ; //!< Message Control 
   
   const MdtIdHelper * m_mdtId ; //!< IdHelper
   const CscIdHelper * m_cscId ; //!< IdHelper
   const RpcIdHelper * m_rpcId ; //!< IdHelper
   const TgcIdHelper * m_tgcId ; //!< IdHelper

   const AtlasDetectorID* m_detID ; //!< Pointer On AtlasDetectorID

};

#endif

