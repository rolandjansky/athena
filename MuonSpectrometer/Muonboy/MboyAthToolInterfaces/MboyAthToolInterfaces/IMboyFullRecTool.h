/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMboyFullRecTool_H
#define IMboyFullRecTool_H

#include "GaudiKernel/IAlgTool.h" 

namespace Trk {
  class  PrepRawData;
}

/////////////////////////////////////////////////////////////////////////////
#include "MboyAthToolInterfaces/IMboyAthToolMisc.h"

  /**
   @class IMboyFullRecTool
   
  @author samusog@cern.ch
  
  */

static const InterfaceID IID_IIMboyFullRecTool("IMboyFullRecTool", 1, 0);

class IMboyFullRecTool:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();
                                     
   /** Get all stuff */
   virtual StatusCode DoRec(
                   const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData,
                   int PrintDigitsSummary , int AthPosOff , int SwitchOff , int ShortSummarySegment , int  ShortSummaryTrack, 
                   std::string DigiStoreLocation , std::string TrackStoreLocation , std::string SegmentStoreLocation ,
                   int ProcessingStage ,
                   std::string SegmentCollectionLocation,
                   std::string m_TrackCollectionLocation      , std::string TrackCollection_MSonlyLocation       ,
                   std::string TrackParticleContainerLocation , std::string TrackParticleContainer_MSonlyLocation
   
                 ) = 0 ;
   virtual StatusCode  KountMB_SegmentStoreTDS( std::vector<int>& KounterMB_TrackStoreTDS   , std::string& SegmentStoreLocation      ) = 0 ;
   virtual StatusCode  KountMB_TrackStoreTDS  ( std::vector<int>& KounterMB_SegmentStoreTDS , std::string& TrackStoreLocation        ) = 0 ;
   virtual StatusCode  KountSegmentCollection ( std::vector<int>& KounterSegmentCollection  , std::string& SegmentCollectionLocation ) = 0 ;
   virtual StatusCode  KountTrackCollection   ( std::vector<int>& KounterTrackCollection    , std::string& TrackCollectionLocation   ) = 0 ;

};

inline const InterfaceID& IMboyFullRecTool::interfaceID() { return IID_IIMboyFullRecTool; }

#endif

