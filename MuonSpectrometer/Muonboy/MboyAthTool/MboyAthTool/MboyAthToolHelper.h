/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyAthToolHelper_H
#define MboyAthToolHelper_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h" 
#include "GaudiKernel/ToolHandle.h"

/////////////////////////////////////////////////////////////////////////////
class IMboyEDMHelper ;
class IMboySvc ;

namespace Trk {
  class IMuonboyToTrackParametersTool ;
}

namespace Trk {
  class IMuonSegmentToDratTool;
  class ITrackToProtoTrackTool;
}

#include "MboyAthToolInterfaces/IMboyAthToolHelper.h"

  /**
   @class MboyAthToolHelper
   
  @author samusog@cern.ch
  
  */

class MboyAthToolHelper:public AthAlgTool, virtual public IMboyAthToolHelper{
public:
    MboyAthToolHelper(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyAthToolHelper 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

public:
///////////////////////////////////

   /**BackTrack */
   virtual MB_Track* MboyMuonBackTrack(MboyVarByName* pMboyVarByName,MB_Track* pMB_Track ) ;

   /**Collect Matter the Field connection is provided by the service) */
   virtual StatusCode DoCollectMatter( 
                              int AtCosmMatter,
                              const Trk::TrackParameters* pTrackParameters,
                              std::vector<MbMuSc*>* pVectorOfMbMuSc
                                            ) ;

   /** Convertors PrepRawDataColl->PrepRawData */
   virtual void  CvMdtPrepDataCollection( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector<const Muon::MdtPrepDataCollection*>& PrepCols) ;
   virtual void  CvCscPrepDataCollection( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector<const Muon::CscPrepDataCollection*>& PrepCols) ;
   virtual void  CvTgcPrepDataCollection( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector<const Muon::TgcPrepDataCollection*>& PrepCols) ;
   virtual void  CvRpcPrepDataCollection( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector<const Muon::RpcPrepDataCollection*>& PrepCols) ;

   /** Convertors PrepRawDataColl->PrepRawData */
   virtual void  CvMdtPrepDataCollection( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const Muon::MdtPrepDataCollection* PrepCol) ;
   virtual void  CvCscPrepDataCollection( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const Muon::CscPrepDataCollection* PrepCol) ;
   virtual void  CvTgcPrepDataCollection( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const Muon::TgcPrepDataCollection* PrepCol) ;
   virtual void  CvRpcPrepDataCollection( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const Muon::RpcPrepDataCollection* PrepCol) ;

   /** Convertors RIO_OnTrack->PrepRawData */
   virtual void  CvMdtDriftCircleOnTrack( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector<              const Muon::MdtDriftCircleOnTrack*   >& mdts)   ;
   virtual void  CvMdtDriftCircleOnTrack( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector< std::vector< const Muon::MdtDriftCircleOnTrack* > >& mdts)   ;
   virtual void  CvMuonClusterOnTrack   ( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector<              const Muon::MuonClusterOnTrack*   >& clusters ) ;
   virtual void  CvMuonClusterOnTrack   ( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector< std::vector< const Muon::MuonClusterOnTrack* > >& clusters)  ;
   virtual void  CvRIO_OnTrack          ( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector<const Trk::RIO_OnTrack*>& rios           )                    ;

   /** Convertors MuonSegment->PrepRawData */
   virtual void  CvMuonSegment( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector<const Muon::MuonSegment*>& VectorOfMuonSegment ) ;
   virtual void  CvMuonSegment( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData ,                   const Muon::MuonSegment*   pMuonSegment        ) ;

   /**Dumpers */
   virtual StatusCode MboyDumpSegmentCollectionToolDoDump     ( std::string SegmentCollectionLocation      , std::string TrackCollectionLocation, std::ofstream* pOutFile ) ; 
   virtual StatusCode MboyDumpSegmentCollectionToolDoDump     ( std::string SegmentCollectionLocation      ,                                      std::ofstream* pOutFile ) ;
   virtual StatusCode MboyDumpTrackCollectionToolDoDump       ( std::string TrackCollectionLocation        ,                                      std::ofstream* pOutFile ) ;
   virtual StatusCode MboyDumpTrackParticleContainerToolDoDump( std::string TrackParticleContainerLocation ,                                      std::ofstream* pOutFile ) ;
   
   /**Converters in EDM objects*/
   virtual StatusCode                             GetSegmentCollection ( const MB_SegmentStoreAth* pMB_SegmentStoreAth, Trk::SegmentCollection* pSegmentCollection                  , int Clean = 1 ) ;
   virtual std::vector<const Muon::MuonSegment*>* GetSegmentCollection ( const MB_SegmentStoreAth* pMB_SegmentStoreAth                                                              , int Clean = 1 ) ;
   virtual StatusCode                             GetTrackCollection   ( const MB_TrackStoreAth*   pMB_TrackStoreAth  , TrackCollection* pTrackCollection, TrackCollection* pTrackCollection_MSonly ) ;
   virtual StatusCode                             GetTrackIP           ( const MB_TrackAth*        pMB_TrackAth       , Trk::Track*&     pTrack                                                     )  ;
   virtual StatusCode                             GetTrackMS           ( const MB_TrackAth*        pMB_TrackAth       , Trk::Track*&     pTrack                                                     )  ;
   virtual StatusCode                             GetParticleCollection( const TrackCollection*    pTrackCollection   , Rec::TrackParticleContainer* pTrackParticleContainer                        ) ;
            
   /**Fill a MuonRecDigiStoreAth with  PrepRawData set */
   virtual StatusCode FillMuonRecDigiStoreWithVectorOfPrepData_Base( const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData, MuonRecDigiStoreAth* pMuonRecDigiStoreAth ) ;

   /**Do reconstruction of Track/Segment */
   virtual StatusCode DoRec( const MboyPosDigiChoice* pMboyPosDigiChoice, MuonRecDigiStoreAth* pMuonRecDigiStoreAth, MB_TrackStoreAth* pMB_TrackStoreAth, MB_SegmentStoreAth* pMB_SegmentStoreAth ) ;
                  
   /** Define configuration variables to call the Svc */
   virtual MboyVarByName* SetSettingsForCall(
                                             std::string& ToolName                        ,  
                                             int    NberOfCtrlPpty                        , 
                                             std::vector<std::string>&  NameOfCtrlPpty    , 
                                             double* ValueOfCtrlPpty                      , 
                                             std::vector<std::string>&  CommentOfCtrlPpty   
                                            ) ;

   /** Dump MboyVarByName */
   virtual void PrintOutVariables(MboyVarByName*  pMboyVarByName);

   /** Get Segments from PrepRawData */
   virtual std::vector<const Muon::MuonSegment*>* findFromPrep( MboyVarByName* pMboyVarByName, const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData,MboyROAStoreAth* pMboyROAExternalStoreAth , int Clean ) ;

    /** find track in the muon system */
   virtual MbProtoTrack* find(
        MboyVarByName* pMboyVarByName,
        MbProtoTrack*  pMbProtoTrack ,
        MbProtoTrack*  pTargetSegmentsProtoTrack 
                                            ) ;
    /** refine */
   virtual MbProtoTrack* refine(
        MboyVarByName*        pMboyVarByName,
        MbProtoTrack*         pMbProtoTrack,
        Trk::Track*&          pTrackRefined ,
        MuonRecDigiStoreAth*  pMuonRecDigiStoreAth
                                            ) ;
   /** Convert MuPatSegment to MbDRAT */
   virtual MbDRAT* convertMuPatSegmentToMbDRAT (
                    MuonRecDigiStoreAth*      pMuonRecDigiStoreAth ,
                    const Muon::MuPatSegment* pMuPatSegment
                                            ) ;
   /** Set a MbDRAT */
   virtual void SetAMbDRAT (
        MboyVarByName* pMboyVarByName,
        MbDRAT*        pMbDRAT
                                            ) ;
   /** Load the Event */
   virtual void LoadEvent(
        MboyVarByName* pMboyVarByName,
        MuonRecDigiStoreAth*  pMuonRecDigiStoreAth 
                                            ) ;

   virtual MbDRAT* GiveMuPatSegmentToMbDRAT     ( const Muon::MuPatSegment* pMuPatSegment, int& kounter ) ; 
   virtual void    CopyMbDRATToMuPatSegment     ( const MbDRAT*             pMbDRAT      , Muon::MuPatSegment* pMuPatSegment ) ;
   
   virtual void CopyMuPatSegmentToMbProtoTrack  ( 
                                                 int& Iok, 
                                                 const Muon::MuPatSegment*   pMuPatSegment  , 
                                                 MbProtoTrack* pMbProtoTrack 
                                                ) ;
   virtual void CopyMuPatTrackToMbProtoTrack ( 
                                              int& Iok, 
                                              MboyVarByName* pMboyVarByName,
                                              MuonRecDigiStoreAth*  pMuonRecDigiStoreAth ,
                                              Muon::MuPatTrack*   pMuPatTrack  , 
                                              MbProtoTrack* pMbProtoTrack 
                                             ) ;
   virtual void UpdateMuPatTrackFromMbProtoTrack ( const MbProtoTrack*     pMbProtoTrack, Muon::MuPatTrack* pMuPatTrack   ) ;
   virtual Trk::Track* MbProtoTrack2Track(
                                          MbProtoTrack* pMbProtoTrack ,
                                          std::vector<Muon::MuPatSegment*>& MuPatSegmentS_for_New_pMuPatTrack 
                                         ) ;

   /** Wrapped from MboyEDMHelper */
   virtual std::string SegmentAssociatedStationName(const Trk::Segment*  pSegment);
   virtual void SegmentAssociatedStationNames(const Trk::Segment*  pSegment, std::vector<std::string >& VectorOfStationNames ) ;

private:
///////////////////////////////////
   ToolHandle< Trk::IMuonboyToTrackParametersTool > p_IMuonboyToTrackParametersTool ; //!< Pointer On IMuonboyToTrackParametersTool
   ToolHandle< IMboyEDMHelper > p_OwnEDMHelper ; //!< Pointer On IMboyEDMHelper
   ToolHandle< Trk::IMuonSegmentToDratTool > p_IMuonSegmentToDratTool ; //!< Pointer On IMuonSegmentToDratTool
   ToolHandle< Trk::ITrackToProtoTrackTool > p_ITrackToProtoTrackTool ; //!< Pointer On ITrackToProtoTrackTool

   ServiceHandle<IMboySvc>             p_IMboySvc               ;  //!< Pointer On IMboySvc

   int  m_ForRefineUseCompleteTrack ; //!< Use the complete or the at Spectro only track
};

#endif
