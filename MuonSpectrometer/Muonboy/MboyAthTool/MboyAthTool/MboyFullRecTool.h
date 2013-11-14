/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyFullRecTool_H
#define MboyFullRecTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MboyAthToolInterfaces/IMboyFullRecTool.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyAthTool/MboyAthToolMisc.h"

 class IMboyAthToolHelper;

 /**
   @class MboyFullRecTool
   
  @author samusog@cern.ch
  
  */

class MboyFullRecTool:public AthAlgTool, virtual public IMboyFullRecTool{
public:
    MboyFullRecTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyFullRecTool 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /** Get all stuff */
   virtual StatusCode DoRec(
                   const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData,
                   int PrintDigitsSummary , int AthPosOff , int SwitchOff , int ShortSummarySegment , int  ShortSummaryTrack, 
                   std::string DigiStoreLocation , std::string TrackStoreLocation , std::string SegmentStoreLocation ,
                   int ProcessingStage ,
                   std::string SegmentCollectionLocation,
                   std::string m_TrackCollectionLocation      , std::string TrackCollection_MSonlyLocation       ,
                   std::string TrackParticleContainerLocation , std::string TrackParticleContainer_MSonlyLocation
   
                 );
   virtual StatusCode  KountMB_SegmentStoreTDS( std::vector<int>& KounterMB_TrackStoreTDS   , std::string& SegmentStoreLocation      ) ;
   virtual StatusCode  KountMB_TrackStoreTDS  ( std::vector<int>& KounterMB_SegmentStoreTDS , std::string& TrackStoreLocation        ) ;
   virtual StatusCode  KountSegmentCollection ( std::vector<int>& KounterSegmentCollection  , std::string& SegmentCollectionLocation ) ;
   virtual StatusCode  KountTrackCollection   ( std::vector<int>& KounterTrackCollection    , std::string& TrackCollectionLocation   ) ;
   
private:
///////////////////////////////////

   ToolHandle< IMboyAthToolHelper >   p_OwnEDMHelper          ; //!< Pointer On IMboyAthToolHelper

};

#endif

