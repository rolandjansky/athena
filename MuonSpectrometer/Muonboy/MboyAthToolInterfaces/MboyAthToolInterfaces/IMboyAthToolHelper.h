/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMboyAthToolHelper_H
#define IMboyAthToolHelper_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h" 

namespace Rec {
  class  TrackParticleContainer;
}

namespace Trk {
  class  PrepRawData;
  class RIO_OnTrack;
  class MeasuredPerigee;
}

namespace Muon {
 class MdtDriftCircleOnTrack;
 class MuonClusterOnTrack;
}

//Can not be forwarded due to typedef
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

//Can not be forwarded due to typedef
#include "TrkSegment/Segment.h"
#include "TrkSegment/SegmentCollection.h"

//Can not be forwarded due to typedef
#include "TrkParameters/TrackParameters.h"

namespace Muon {
  class MuPatCandidateBase;
  class MuPatSegment;
  class MuPatTrack;  
  class MuonSegment;  
}

#include "MuonPrepRawData/CscPrepDataCollection.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"

class MboyPosDigiChoice;
class MboyVarByName;
class MuonRecDigiStoreAth;
class MB_TrackStoreAth;
class MB_SegmentStoreAth;
class MB_TrackAth;
class MB_Track;

class MbDRAT;
class MbProtoTrack;

class MbMuSc;

class MboyROAStoreAth ;

/////////////////////////////////////////////////////////////////////////////
#include "MboyAthToolInterfaces/IMboyAthToolMisc.h"

  /**
   @class IMboyAthToolHelper
   
  @author samusog@cern.ch
  
  */

static const InterfaceID IID_IIMboyAthToolHelper("IMboyAthToolHelper", 1, 0);

class IMboyAthToolHelper:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

public:
///////////////////////////////////

   /**BackTrack */
   virtual MB_Track* MboyMuonBackTrack(MboyVarByName* pMboyVarByName, MB_Track* pMB_Track ) = 0  ;

   /**Collect Matter the Field connection is provided by the service) */
   virtual StatusCode DoCollectMatter( 
                              int AtCosmMatter,
                              const Trk::TrackParameters* pTrackParameters,
                              std::vector<MbMuSc*>* pVectorOfMbMuSc
                                            ) = 0 ;

   /** Convertors VectorOfPrepRawDataColl->PrepRawData */
   virtual void  CvMdtPrepDataCollection( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector<const Muon::MdtPrepDataCollection*>& PrepCols) = 0 ;
   virtual void  CvCscPrepDataCollection( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector<const Muon::CscPrepDataCollection*>& PrepCols) = 0 ;
   virtual void  CvTgcPrepDataCollection( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector<const Muon::TgcPrepDataCollection*>& PrepCols) = 0 ;
   virtual void  CvRpcPrepDataCollection( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector<const Muon::RpcPrepDataCollection*>& PrepCols) = 0 ;

   /** Convertors PrepRawDataColl->PrepRawData */
   virtual void  CvMdtPrepDataCollection( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const Muon::MdtPrepDataCollection* PrepCol) = 0 ;
   virtual void  CvCscPrepDataCollection( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const Muon::CscPrepDataCollection* PrepCol) = 0 ;
   virtual void  CvTgcPrepDataCollection( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const Muon::TgcPrepDataCollection* PrepCol) = 0 ;
   virtual void  CvRpcPrepDataCollection( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const Muon::RpcPrepDataCollection* PrepCol) = 0 ;

   /** Convertors RIO_OnTrack->PrepRawData */
   virtual void  CvMdtDriftCircleOnTrack( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector<              const Muon::MdtDriftCircleOnTrack*   >& mdts)   = 0 ;
   virtual void  CvMdtDriftCircleOnTrack( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector< std::vector< const Muon::MdtDriftCircleOnTrack* > >& mdts)   = 0 ;
   virtual void  CvMuonClusterOnTrack   ( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector<              const Muon::MuonClusterOnTrack*   >& clusters ) = 0 ;
   virtual void  CvMuonClusterOnTrack   ( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector< std::vector< const Muon::MuonClusterOnTrack* > >& clusters)  = 0 ;
   virtual void  CvRIO_OnTrack          ( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector<const Trk::RIO_OnTrack*>& rios           )                    = 0 ;

   /** Convertors MuonSegment->PrepRawData */
   virtual void  CvMuonSegment( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector<const Muon::MuonSegment*>& VectorOfMuonSegment )   = 0 ;
   virtual void  CvMuonSegment( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData ,                   const Muon::MuonSegment*   pMuonSegment        )   = 0 ;

   /**Dumpers */
   virtual StatusCode MboyDumpSegmentCollectionToolDoDump     ( std::string SegmentCollectionLocation      , std::string TrackCollectionLocation, std::ofstream* pOutFile ) = 0 ; 
   virtual StatusCode MboyDumpSegmentCollectionToolDoDump     ( std::string SegmentCollectionLocation      ,                                      std::ofstream* pOutFile ) = 0 ;
   virtual StatusCode MboyDumpTrackCollectionToolDoDump       ( std::string TrackCollectionLocation        ,                                      std::ofstream* pOutFile ) = 0 ;
   virtual StatusCode MboyDumpTrackParticleContainerToolDoDump( std::string TrackParticleContainerLocation ,                                      std::ofstream* pOutFile ) = 0 ;

   /**Converters in EDM objects*/
   virtual StatusCode                             GetSegmentCollection ( const MB_SegmentStoreAth* pMB_SegmentStoreAth, Trk::SegmentCollection* pSegmentCollection                  , int Clean = 1 ) = 0 ;
   virtual std::vector<const Muon::MuonSegment*>* GetSegmentCollection ( const MB_SegmentStoreAth* pMB_SegmentStoreAth                                                              , int Clean = 1 ) = 0 ;
   virtual StatusCode                             GetTrackCollection   ( const MB_TrackStoreAth*   pMB_TrackStoreAth  , TrackCollection* pTrackCollection, TrackCollection* pTrackCollection_MSonly ) = 0 ;
   virtual StatusCode                             GetTrackIP           ( const MB_TrackAth*        pMB_TrackAth       , Trk::Track*&     pTrack                                                     ) = 0 ;
   virtual StatusCode                             GetTrackMS           ( const MB_TrackAth*        pMB_TrackAth       , Trk::Track*&     pTrack                                                     ) = 0 ;
   virtual StatusCode                             GetParticleCollection( const TrackCollection*    pTrackCollection   , Rec::TrackParticleContainer* pTrackParticleContainer                        ) = 0 ;
            
   /**Fill a MuonRecDigiStoreAth with  PrepRawData set */
   virtual StatusCode FillMuonRecDigiStoreWithVectorOfPrepData_Base( const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData, MuonRecDigiStoreAth* pMuonRecDigiStoreAth ) = 0 ;

   /**Do reconstruction of Track/Segment */
   virtual StatusCode DoRec( const MboyPosDigiChoice* pMboyPosDigiChoice, MuonRecDigiStoreAth* pMuonRecDigiStoreAth, MB_TrackStoreAth* pMB_TrackStoreAth, MB_SegmentStoreAth* pMB_SegmentStoreAth ) = 0 ;
                  
   /** Define configuration variables to call the Svc */
   virtual MboyVarByName* SetSettingsForCall(
                                             std::string& ToolName                        ,  
                                             int    NberOfCtrlPpty                        , 
                                             std::vector<std::string>&  NameOfCtrlPpty    , 
                                             double* ValueOfCtrlPpty                      , 
                                             std::vector<std::string>&  CommentOfCtrlPpty   
                                            ) = 0 ;

   /** Dump MboyVarByName */
   virtual void PrintOutVariables(MboyVarByName*  pMboyVarByName) = 0 ;

   /** Get Segments from PrepRawData */
   virtual std::vector<const Muon::MuonSegment*>* findFromPrep( MboyVarByName* pMboyVarByName, const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData,MboyROAStoreAth* pMboyROAExternalStoreAth , int Clean ) = 0;

    /** find track in the muon system */
   virtual MbProtoTrack* find(
        MboyVarByName* pMboyVarByName,
        MbProtoTrack*  pMbProtoTrack ,
        MbProtoTrack*  pTargetSegmentsProtoTrack 
                                            ) = 0 ;
    /** refine */
   virtual MbProtoTrack* refine(
        MboyVarByName*        pMboyVarByName,
        MbProtoTrack*         pMbProtoTrack,
        Trk::Track*&           pTrackRefined ,
        MuonRecDigiStoreAth*  pMuonRecDigiStoreAth
                                            ) = 0 ;
   /** Convert MuPatSegment to MbDRAT */
   virtual MbDRAT* convertMuPatSegmentToMbDRAT (
                    MuonRecDigiStoreAth*      pMuonRecDigiStoreAth ,
                    const Muon::MuPatSegment* pMuPatSegment
                                            ) = 0 ;
   /** Set a MbDRAT */
   virtual void SetAMbDRAT (
        MboyVarByName* pMboyVarByName,
        MbDRAT*        pMbDRAT
                                            ) = 0 ;
   /** Load the Event */
   virtual void LoadEvent(
        MboyVarByName* pMboyVarByName,
        MuonRecDigiStoreAth*  pMuonRecDigiStoreAth 
                                            ) = 0 ;

   virtual MbDRAT* GiveMuPatSegmentToMbDRAT     ( const Muon::MuPatSegment* pMuPatSegment, int& kounter ) = 0 ;
   virtual void    CopyMbDRATToMuPatSegment     ( const MbDRAT*             pMbDRAT      , Muon::MuPatSegment* pMuPatSegment ) = 0 ;
   
   virtual void CopyMuPatSegmentToMbProtoTrack  ( 
                                                 int& Iok, 
                                                 const Muon::MuPatSegment*   pMuPatSegment  , 
                                                 MbProtoTrack* pMbProtoTrack 
                                                 ) = 0 ;
   virtual void CopyMuPatTrackToMbProtoTrack ( 
                                              int& Iok, 
                                              MboyVarByName* pMboyVarByName,
                                              MuonRecDigiStoreAth*  pMuonRecDigiStoreAth ,
                                              Muon::MuPatTrack*   pMuPatTrack  , 
                                              MbProtoTrack* pMbProtoTrack 
                                             ) = 0 ;
   virtual void UpdateMuPatTrackFromMbProtoTrack ( const MbProtoTrack*     pMbProtoTrack, Muon::MuPatTrack* pMuPatTrack   ) = 0 ;
   virtual Trk::Track* MbProtoTrack2Track(
                                          MbProtoTrack* pMbProtoTrack ,
                                          std::vector<Muon::MuPatSegment*>& MuPatSegmentS_for_New_pMuPatTrack 
                                         ) = 0 ;

   /** Wrapped from MboyEDMHelper */
   virtual std::string SegmentAssociatedStationName(const Trk::Segment*  pSegment) = 0 ;
   virtual void SegmentAssociatedStationNames(const Trk::Segment*  pSegment, std::vector<std::string >& VectorOfStationNames ) = 0 ;

};

inline const InterfaceID& IMboyAthToolHelper::interfaceID() { return IID_IIMboyAthToolHelper; }

#endif
