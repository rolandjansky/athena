/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"

#include "MboySvcInterfaces/IMboySvc.h"

#include "MuonTrackFindingEvent/MuPatTrack.h"
#include "MuonTrackFindingEvent/MuPatCandidateBase.h"
#include "MuonTrackFindingEvent/MuPatSegment.h"
#include "MuonSegment/MuonSegment.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkParameters/TrackParameters.h"

/////////////////////////////////////////////////////////
#include "MboyAthTool/MboyAthToolHelper.h"

#include "MboyEDMToolInterfaces/IMboyEDMHelper.h"

#include "MboyCoreEvt/MboyVarByName.h"
#include "MboyCoreEvt/MboyPosDigiChoice.h"

#include "MboyCoreEvt/MbDRAT.h"
#include "MboyCoreEvt/MbProtoTrack.h"

#include "MboyAthEvt/MboyROAStoreAth.h"

#include "MboyAthEvt/MuonRecDigiStoreAth.h"
#include "MboyAthEvt/MB_SegmentStoreAth.h"
#include "MboyAthEvt/MB_TrackStoreAth.h"

#include "MboyEDMToolInterfaces/IMuonSegmentToDratTool.h"
#include "MboyEDMToolInterfaces/ITrackToProtoTrackTool.h"
#include "MboyEDMToolInterfaces/IMuonboyToTrackParametersTool.h"

MboyAthToolHelper::MboyAthToolHelper(const std::string& t, 
                             const std::string& n,
                             const IInterface*  p ):AthAlgTool(t,n,p),
 p_IMuonboyToTrackParametersTool ( "Trk::MuonboyToTrackParametersTool/MuonboyToTrackParametersTool" ),
 p_OwnEDMHelper ( "MboyEDMHelper" ),
 p_IMuonSegmentToDratTool ( "Trk::MuonSegmentToDratTool/MuonSegmentToDratTool" ) ,
 p_ITrackToProtoTrackTool ( "Trk::TrackToProtoTrackTool/TrackToProtoTrackTool" ) ,
p_IMboySvc ( "MboySvc",t ) 
{

  declareInterface<IMboyAthToolHelper>(this);

  declareProperty("MuonboyToTrackParametersTool"  , p_IMuonboyToTrackParametersTool ) ;
  declareProperty("MboyEDMHelper" , p_OwnEDMHelper ) ;
  declareProperty("MuonSegmentToDratTool" , p_IMuonSegmentToDratTool ) ;
  declareProperty("TrackToProtoTrackTool" , p_ITrackToProtoTrackTool ) ;
  declareProperty("IMboySvc", p_IMboySvc);

  declareProperty( "ForRefineUseCompleteTrack" , m_ForRefineUseCompleteTrack  = 0  ) ;      
}

MboyAthToolHelper::~MboyAthToolHelper(){}

// Initialize
StatusCode MboyAthToolHelper::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }

  ATH_MSG_INFO( "================================" );
  ATH_MSG_INFO( "=Proprieties are        " );
  ATH_MSG_INFO( "= ForRefineUseCompleteTrack                                    " << m_ForRefineUseCompleteTrack                );
  ATH_MSG_INFO( "================================" );

//Retrieve p_IMuonboyToTrackParametersTool
  if ( p_IMuonboyToTrackParametersTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMuonboyToTrackParametersTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMuonboyToTrackParametersTool );


//Retrieve p_OwnEDMHelper
  if ( p_OwnEDMHelper.retrieve().isFailure() ) {
    ATH_MSG_FATAL(  "Failed to retrieve tool " << p_OwnEDMHelper ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO(  "Retrieved tool " << p_OwnEDMHelper ) ;

//Retrieve p_IMuonSegmentToDratTool
  if ( p_IMuonSegmentToDratTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMuonSegmentToDratTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMuonSegmentToDratTool );

//Retrieve p_ITrackToProtoTrackTool
  if ( p_ITrackToProtoTrackTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_ITrackToProtoTrackTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_ITrackToProtoTrackTool );

//Set pointer on IMboySvc
  if ( p_IMboySvc.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve service " << p_IMboySvc ) ;
    return StatusCode::FAILURE;
  } 
  ATH_MSG_INFO( "Retrieved service " << p_IMboySvc ) ;


  ATH_MSG_INFO( "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}

StatusCode MboyAthToolHelper::finalize(){return StatusCode::SUCCESS;}

//---------------------------------------------------------------------------------------------------------------//
MB_Track* MboyAthToolHelper::MboyMuonBackTrack(MboyVarByName* pMboyVarByName,MB_Track* pMB_Track )
{
  return p_IMboySvc->MboyMuonBackTrack(pMboyVarByName,pMB_Track) ;
}


//---------------------------------------------------------------------------------------------------------------//
// Wrapped from MboyEDMHelper 
std::string MboyAthToolHelper::SegmentAssociatedStationName(const Trk::Segment*  pSegment){return p_OwnEDMHelper->SegmentAssociatedStationName(pSegment);}
void MboyAthToolHelper::SegmentAssociatedStationNames(const Trk::Segment*  pSegment, std::vector<std::string >& VectorOfStationNames ){  p_OwnEDMHelper->SegmentAssociatedStationNames(pSegment,VectorOfStationNames);}

//---------------------------------------------------------------------------------------------------------------//
StatusCode MboyAthToolHelper::DoCollectMatter( 
                              int AtCosmMatter,
                              const Trk::TrackParameters* pTrackParameters,
                              std::vector<MbMuSc*>* pVectorOfMbMuSc
){
  return p_IMboySvc->DoCollectMatter(AtCosmMatter , pTrackParameters, pVectorOfMbMuSc) ;
}
//---------------------------------------------------------------------------------------------------------------//
// Convertors RIO_OnTrack->PrepRawData 

void MboyAthToolHelper::CvMdtDriftCircleOnTrack( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector<const Muon::MdtDriftCircleOnTrack*>& mdts)
{

  std::vector<const Muon::MdtDriftCircleOnTrack*>::const_iterator it     = mdts.begin() ;
  std::vector<const Muon::MdtDriftCircleOnTrack*>::const_iterator it_end = mdts.end()   ;
  unsigned NewSize = VectorOfpPrepData.size() + mdts.size() ; VectorOfpPrepData.reserve(NewSize); 
  for (; it!=it_end; ++it){
    VectorOfpPrepData.push_back((*it)->prepRawData()) ;
  }

}

void MboyAthToolHelper::CvMdtDriftCircleOnTrack( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector< std::vector< const Muon::MdtDriftCircleOnTrack* > >& mdts)
{

  std::vector< std::vector< const Muon::MdtDriftCircleOnTrack* > >::const_iterator it     = mdts.begin() ;
  std::vector< std::vector< const Muon::MdtDriftCircleOnTrack* > >::const_iterator it_end = mdts.end()   ;
  for(; it!=it_end;++it ){
    CvMdtDriftCircleOnTrack(VectorOfpPrepData, (*it) );
  }

}

void MboyAthToolHelper::CvMuonClusterOnTrack( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector<const Muon::MuonClusterOnTrack*>& clusters)
{

  std::vector<const Muon::MuonClusterOnTrack*>::const_iterator it     = clusters.begin() ;
  std::vector<const Muon::MuonClusterOnTrack*>::const_iterator it_end = clusters.end()   ;
  unsigned NewSize = VectorOfpPrepData.size() + clusters.size() ; VectorOfpPrepData.reserve(NewSize); 
  for (; it!=it_end; ++it){
    VectorOfpPrepData.push_back((*it)->prepRawData()) ;
  }

}

void MboyAthToolHelper::CvMuonClusterOnTrack( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector< std::vector< const Muon::MuonClusterOnTrack* > >& clusters)
{

  std::vector< std::vector< const Muon::MuonClusterOnTrack* > >::const_iterator it     = clusters.begin() ;
  std::vector< std::vector< const Muon::MuonClusterOnTrack* > >::const_iterator it_end = clusters.end()   ;
  for(; it!=it_end;++it ){
    CvMuonClusterOnTrack(VectorOfpPrepData, (*it) );
  }

}

void MboyAthToolHelper::CvRIO_OnTrack( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector<const Trk::RIO_OnTrack*>& rios)
{

  std::vector<const Trk::RIO_OnTrack*>::const_iterator it     = rios.begin() ;
  std::vector<const Trk::RIO_OnTrack*>::const_iterator it_end = rios.end()   ;
  unsigned NewSize = VectorOfpPrepData.size() + rios.size() ; VectorOfpPrepData.reserve(NewSize); 
  for (; it!=it_end; ++it){
    VectorOfpPrepData.push_back((*it)->prepRawData()) ;
  }

}

//---------------------------------------------------------------------------------------------------------------//
// Convertors PrepRawDataColl->PrepRawData 

void MboyAthToolHelper::CvMdtPrepDataCollection( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const Muon::MdtPrepDataCollection* PrepCol)
{

  Muon::MdtPrepDataCollection::const_iterator it     = PrepCol->begin() ;
  Muon::MdtPrepDataCollection::const_iterator it_end = PrepCol->end()   ;
  unsigned NewSize = VectorOfpPrepData.size() + PrepCol->size() ; VectorOfpPrepData.reserve(NewSize); 
  for ( ; it != it_end ; ++it) {
    const Trk::PrepRawData* pPrepData = (*it) ;
    VectorOfpPrepData.push_back(pPrepData);
  }

}

void MboyAthToolHelper::CvCscPrepDataCollection( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const Muon::CscPrepDataCollection* PrepCol)
{

  Muon::CscPrepDataCollection::const_iterator it     = PrepCol->begin() ;
  Muon::CscPrepDataCollection::const_iterator it_end = PrepCol->end()   ;
  unsigned NewSize = VectorOfpPrepData.size() + PrepCol->size() ; VectorOfpPrepData.reserve(NewSize); 
  for ( ; it != it_end ; ++it) {
    const Trk::PrepRawData* pPrepData = (*it) ;
    VectorOfpPrepData.push_back(pPrepData);
  }
}

void MboyAthToolHelper::CvTgcPrepDataCollection( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const Muon::TgcPrepDataCollection* PrepCol)
{

  Muon::TgcPrepDataCollection::const_iterator it     = PrepCol->begin() ;
  Muon::TgcPrepDataCollection::const_iterator it_end = PrepCol->end()   ;
  unsigned NewSize = VectorOfpPrepData.size() + PrepCol->size() ; VectorOfpPrepData.reserve(NewSize); 
  for ( ; it != it_end ; ++it) {
    const Trk::PrepRawData* pPrepData = (*it) ;
    VectorOfpPrepData.push_back(pPrepData);
  }

}

void MboyAthToolHelper::CvRpcPrepDataCollection( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const Muon::RpcPrepDataCollection* PrepCol)
{

  Muon::RpcPrepDataCollection::const_iterator it     = PrepCol->begin() ;
  Muon::RpcPrepDataCollection::const_iterator it_end = PrepCol->end()   ;
  unsigned NewSize = VectorOfpPrepData.size() + PrepCol->size() ; VectorOfpPrepData.reserve(NewSize); 
  for ( ; it != it_end ; ++it) {
    const Trk::PrepRawData* pPrepData = (*it) ;
    VectorOfpPrepData.push_back(pPrepData);
  }

}

//---------------------------------------------------------------------------------------------------------------//
// Convertors VectorOfPrepRawDataColl->PrepRawData 

void MboyAthToolHelper::CvMdtPrepDataCollection( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector<const Muon::MdtPrepDataCollection*>& PrepCols)
{

  std::vector<const Muon::MdtPrepDataCollection*>::const_iterator it     = PrepCols.begin() ;
  std::vector<const Muon::MdtPrepDataCollection*>::const_iterator it_end = PrepCols.end()   ;
  for (; it!=it_end; ++it){
    CvMdtPrepDataCollection(VectorOfpPrepData,(*it)) ;
  }

}

void MboyAthToolHelper::CvCscPrepDataCollection( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector<const Muon::CscPrepDataCollection*>& PrepCols)
{

  std::vector<const Muon::CscPrepDataCollection*>::const_iterator it     = PrepCols.begin() ;
  std::vector<const Muon::CscPrepDataCollection*>::const_iterator it_end = PrepCols.end()   ;
  for (; it!=it_end; ++it){
    CvCscPrepDataCollection(VectorOfpPrepData,(*it)) ;
  }

}

void MboyAthToolHelper::CvTgcPrepDataCollection( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector<const Muon::TgcPrepDataCollection*>& PrepCols)
{

  std::vector<const Muon::TgcPrepDataCollection*>::const_iterator it     = PrepCols.begin() ;
  std::vector<const Muon::TgcPrepDataCollection*>::const_iterator it_end = PrepCols.end()   ;
  for (; it!=it_end; ++it){
    CvTgcPrepDataCollection(VectorOfpPrepData,(*it)) ;
  }

}

void MboyAthToolHelper::CvRpcPrepDataCollection( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector<const Muon::RpcPrepDataCollection*>& PrepCols)
{

  std::vector<const Muon::RpcPrepDataCollection*>::const_iterator it     = PrepCols.begin() ;
  std::vector<const Muon::RpcPrepDataCollection*>::const_iterator it_end = PrepCols.end()   ;
  for (; it!=it_end; ++it){
    CvRpcPrepDataCollection(VectorOfpPrepData,(*it)) ;
  }

}

//---------------------------------------------------------------------------------------------------------------//
// Convertors MuonSegment->PrepRawData  
void   MboyAthToolHelper::CvMuonSegment( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData , const std::vector<const Muon::MuonSegment*>& VectorOfMuonSegment )
{
  std::vector< const Muon::MuonSegment* >::const_iterator it     = VectorOfMuonSegment.begin() ; 
  std::vector< const Muon::MuonSegment* >::const_iterator it_end = VectorOfMuonSegment.end()   ;
  for(; it!=it_end;++it ){
    CvMuonSegment( VectorOfpPrepData ,(*it) );
  }

}
void   MboyAthToolHelper::CvMuonSegment( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData ,                   const Muon::MuonSegment*   pMuonSegment        )
{
  CvRIO_OnTrack( VectorOfpPrepData ,pMuonSegment->containedROTs() );
}


//---------------------------------------------------------------------------------------------------------------//
// Dumpers
StatusCode MboyAthToolHelper::MboyDumpSegmentCollectionToolDoDump     ( std::string SegmentCollectionLocation      , std::string TrackCollectionLocation, std::ofstream* pOutFile ) { return p_IMboySvc->MboyDumpSegmentCollectionToolDoDump     ( SegmentCollectionLocation      , TrackCollectionLocation, pOutFile ); }
StatusCode MboyAthToolHelper::MboyDumpSegmentCollectionToolDoDump     ( std::string SegmentCollectionLocation      ,                                      std::ofstream* pOutFile ) { return p_IMboySvc->MboyDumpSegmentCollectionToolDoDump     ( SegmentCollectionLocation      ,                          pOutFile ); }
StatusCode MboyAthToolHelper::MboyDumpTrackCollectionToolDoDump       ( std::string TrackCollectionLocation        ,                                      std::ofstream* pOutFile ) { return p_IMboySvc->MboyDumpTrackCollectionToolDoDump       ( TrackCollectionLocation        ,                          pOutFile ); }
StatusCode MboyAthToolHelper::MboyDumpTrackParticleContainerToolDoDump( std::string TrackParticleContainerLocation ,                                      std::ofstream* pOutFile ) { return p_IMboySvc->MboyDumpTrackParticleContainerToolDoDump( TrackParticleContainerLocation ,                          pOutFile ); }

//---------------------------------------------------------------------------------------------------------------//
// Converters in EDM objects 
StatusCode                             MboyAthToolHelper::GetSegmentCollection ( const MB_SegmentStoreAth* pMB_SegmentStoreAth, Trk::SegmentCollection* pSegmentCollection                      , int Clean ) { return p_IMboySvc->GetSegmentCollection ( pMB_SegmentStoreAth, pSegmentCollection                , Clean ); }
std::vector<const Muon::MuonSegment*>* MboyAthToolHelper::GetSegmentCollection ( const MB_SegmentStoreAth* pMB_SegmentStoreAth                                                                  , int Clean ) { return p_IMboySvc->GetSegmentCollection ( pMB_SegmentStoreAth                                    , Clean ); }
StatusCode                             MboyAthToolHelper::GetTrackCollection   ( const MB_TrackStoreAth*   pMB_TrackStoreAth  , TrackCollection* pTrackCollection, TrackCollection* pTrackCollection_MSonly ) { return p_IMboySvc->GetTrackCollection   ( pMB_TrackStoreAth  , pTrackCollection, pTrackCollection_MSonly ); }
StatusCode                             MboyAthToolHelper::GetTrackIP           ( const MB_TrackAth*        pMB_TrackAth       , Trk::Track*&     pTrack                                                     ) { return p_IMboySvc->GetTrackIP           ( pMB_TrackAth       , pTrack                                    ); }
StatusCode                             MboyAthToolHelper::GetTrackMS           ( const MB_TrackAth*        pMB_TrackAth       , Trk::Track*&     pTrack                                                     ) { return p_IMboySvc->GetTrackMS           ( pMB_TrackAth       , pTrack                                    ); }
StatusCode                             MboyAthToolHelper::GetParticleCollection( const TrackCollection*    pTrackCollection   , Rec::TrackParticleContainer* pTrackParticleContainer                        ) { return p_IMboySvc->GetParticleCollection( pTrackCollection   , pTrackParticleContainer                   ); }

//---------------------------------------------------------------------------------------------------------------//
// Fill a MuonRecDigiStoreAth with  PrepRawData set 
StatusCode MboyAthToolHelper::FillMuonRecDigiStoreWithVectorOfPrepData_Base( const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData, MuonRecDigiStoreAth* pMuonRecDigiStoreAth ){ return p_IMboySvc->FillMuonRecDigiStoreWithVectorOfPrepData_Base( VectorOfpPrepData, pMuonRecDigiStoreAth ) ;}

//---------------------------------------------------------------------------------------------------------------//
// Do reconstruction of Track/Segment  
StatusCode MboyAthToolHelper::DoRec( const MboyPosDigiChoice* pMboyPosDigiChoice, MuonRecDigiStoreAth* pMuonRecDigiStoreAth, MB_TrackStoreAth* pMB_TrackStoreAth, MB_SegmentStoreAth* pMB_SegmentStoreAth ){ return p_IMboySvc->DoRec( pMboyPosDigiChoice, pMuonRecDigiStoreAth, pMB_TrackStoreAth, pMB_SegmentStoreAth ) ; }
                  
//---------------------------------------------------------------------------------------------------------------//
// Define configuration variables to call the Svc

MboyVarByName* MboyAthToolHelper::SetSettingsForCall(
                                             std::string& ToolName                        ,  
                                             int    NberOfCtrlPpty                        , 
                                             std::vector<std::string>&  NameOfCtrlPpty    , 
                                             double* ValueOfCtrlPpty                      , 
                                             std::vector<std::string>&  CommentOfCtrlPpty   
)
{

  MboyVarByName* pMboyVarByName = new MboyVarByName ;

  const MboyVarByName* pMboyVarByName_SvcSetting =  p_IMboySvc->GetSvcSetting() ;
  int TheSize_SvcSetting = pMboyVarByName_SvcSetting->Size();
  
  for (int Index_Tool= 0 ; Index_Tool <NberOfCtrlPpty ; Index_Tool++){
    std::string Name1   = NameOfCtrlPpty[Index_Tool] ;
    int IOK = 0 ;
    for (int Index_Svc= 0 ; Index_Svc <TheSize_SvcSetting ; Index_Svc++){
      if ( Name1 == pMboyVarByName_SvcSetting->GetName1  (Index_Svc) ) {
        double      Value   = ValueOfCtrlPpty[Index_Tool] ;
        std::string Name2   = pMboyVarByName_SvcSetting->GetName2 (Index_Svc) ;
        std::string Comment = CommentOfCtrlPpty[Index_Tool] ;
        pMboyVarByName->Define(Name1,Name2,Comment,Value) ;
        ATH_MSG_INFO( "Following propriety "
                        << " of " << ToolName
                        <<" will be     used to set call to IMboySvc " << Name1 ) ;
        IOK = 1 ;
        break ;
      }
    } 
    if ( IOK == 0 ) ATH_MSG_INFO( "Following propriety "
                                    << " of " << ToolName
                                    <<" will be NOT used to set call to IMboySvc " << Name1 ) ;
  }    

  return pMboyVarByName ;
}

//---------------------------------------------------------------------------------------------------------------//
// Dump MboyVarByName

void MboyAthToolHelper::PrintOutVariables(MboyVarByName*  pMboyVarByName)
{
    
  int TheSizeVar = pMboyVarByName->Size();
  for (int IndexVar= 0 ; IndexVar <TheSizeVar ; IndexVar++){   
    std::string Name1   = pMboyVarByName->GetName1(IndexVar);
    double      Value   = pMboyVarByName->GetValue(IndexVar) ;
    std::string Comment = pMboyVarByName->GetComment(IndexVar);
    Name1.resize(30,' ');
    Comment.resize(70,' ');
    ATH_MSG_INFO( "= " << Name1  << " " << Comment << " " << Value ) ;
  }
  
}      

//---------------------------------------------------------------------------------------------------------------//
// Get Segments from PrepRawData

std::vector<const Muon::MuonSegment*>* MboyAthToolHelper::findFromPrep( MboyVarByName* pMboyVarByName, const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData, MboyROAStoreAth* pMboyROAExternalStoreAth, int Clean){

//PrintOutVariables(pMboyVarByName);
  
  StatusCode sc = StatusCode::SUCCESS;

  MB_SegmentStoreAth aMB_SegmentStoreAth  ; aMB_SegmentStoreAth.ResetMB_SegmentStoreAth();
  MuonRecDigiStoreAth aMuonRecDigiStoreAth ; aMuonRecDigiStoreAth.ResetMuonRecDigiStoreAth();
  sc = p_IMboySvc->FillMuonRecDigiStoreWithVectorOfPrepData_Base( VectorOfpPrepData, &aMuonRecDigiStoreAth );
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "FillMuonRecDigiStoreWithVectorOfPrepData_Base failed" ) ;
    return 0 ;
  }

  MboyPosDigiChoice aMboyPosDigiChoice ; 
  sc = p_IMboySvc->DoRec( pMboyVarByName, &aMboyPosDigiChoice , &aMuonRecDigiStoreAth, &aMB_SegmentStoreAth, pMboyROAExternalStoreAth );
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "DoRec failed" ) ;
    return 0 ;
  }
  aMB_SegmentStoreAth.Consistentify(&aMuonRecDigiStoreAth);

  return p_IMboySvc->GetSegmentCollection(&aMB_SegmentStoreAth,Clean);

}
//---------------------------------------------------------------------------------------------------------------//
// find track in the muon system 
MbProtoTrack* MboyAthToolHelper::find(
        MboyVarByName* pMboyVarByName,
        MbProtoTrack*  pMbProtoTrack ,
        MbProtoTrack*  pTargetSegmentsProtoTrack 
){
  if ( pMboyVarByName == 0 ) return 0 ;
  return p_IMboySvc->find( pMboyVarByName , pMbProtoTrack,  pTargetSegmentsProtoTrack) ;
}
//---------------------------------------------------------------------------------------------------------------//
// refine track in the muon system 
MbProtoTrack* MboyAthToolHelper::refine(
        MboyVarByName*        pMboyVarByName,
        MbProtoTrack*         pMbProtoTrack,
        Trk::Track*&          pTrackRefined ,
        MuonRecDigiStoreAth*  pMuonRecDigiStoreAth
){

  pTrackRefined = 0 ;

  if ( pMboyVarByName == 0 ) return 0 ;
  
//Create aMB_TrackStoreAth 
  MB_TrackStoreAth  aMB_TrackStoreAth ; aMB_TrackStoreAth.ResetMB_TrackStoreAth();
  MbProtoTrack* NEW_pMbProtoTrack =  p_IMboySvc->refine( pMboyVarByName , pMbProtoTrack, pMuonRecDigiStoreAth, &aMB_TrackStoreAth ) ;
  aMB_TrackStoreAth.Consistentify(pMuonRecDigiStoreAth);
  
  if ( NEW_pMbProtoTrack == 0 ) return 0 ;
  
  int NberOfTracks = aMB_TrackStoreAth.NberOfMB_Track();
  if ( NberOfTracks != 1 ){
    ATH_MSG_INFO( "= NberOfTracks NE to 1 " << NberOfTracks ) ;
    delete NEW_pMbProtoTrack ;
    return 0 ;
  }
  
  for (int TrackNber=0; TrackNber<NberOfTracks; TrackNber++){
    const MB_TrackAth* pMB_TrackAth = aMB_TrackStoreAth.GetMB_TrackAth(TrackNber) ;
    StatusCode Sc ;
    if ( m_ForRefineUseCompleteTrack == 1 ){
      Sc = GetTrackIP( pMB_TrackAth, pTrackRefined  );
    }else{
      Sc = GetTrackMS( pMB_TrackAth, pTrackRefined  );
    }
    if ( Sc.isFailure() ) {
      ATH_MSG_FATAL( "DoRec failed" ) ;
      delete pTrackRefined ;
      delete NEW_pMbProtoTrack ;
      return 0 ;
    }
    break ;
  }
  
  return NEW_pMbProtoTrack ;
}

//---------------------------------------------------------------------------------------------------------------//
// MuPatSegment to MbDRAT  
MbDRAT* MboyAthToolHelper::convertMuPatSegmentToMbDRAT (
                    MuonRecDigiStoreAth*      pMuonRecDigiStoreAth ,
                    const Muon::MuPatSegment* pMuPatSegment
){
  ATH_MSG_DEBUG( " convertMuPatSegmentToMbDRAT: In " );
  if ( pMuPatSegment == 0 ) return 0 ;
  ATH_MSG_DEBUG( " convertMuPatSegmentToMbDRAT: 1 " );
  const Muon::MuonSegment* pMuonSegment = pMuPatSegment->segment ;
  ATH_MSG_DEBUG( " convertMuPatSegmentToMbDRAT: 2 " );
  if ( pMuonSegment == 0 ) return 0 ;
  ATH_MSG_DEBUG( " convertMuPatSegmentToMbDRAT: 3 " );
  return p_IMuonSegmentToDratTool->convert(pMuonRecDigiStoreAth,pMuonSegment) ;
}

// Set a MbDRAT  
void MboyAthToolHelper::SetAMbDRAT (
        MboyVarByName* pMboyVarByName,
        MbDRAT*        pMbDRAT
){
  if ( pMboyVarByName == 0 ) return ;
  if ( pMbDRAT == 0 ) return ;
  p_IMboySvc->SetAMbDRAT(pMboyVarByName,pMbDRAT) ;
}

// Load the Event  
void MboyAthToolHelper::LoadEvent(
        MboyVarByName* pMboyVarByName,
        MuonRecDigiStoreAth*  pMuonRecDigiStoreAth 
){
  if ( pMboyVarByName == 0 ) return ;
  if ( pMuonRecDigiStoreAth == 0 ) return ;
  p_IMboySvc->LoadEvent(pMboyVarByName , pMuonRecDigiStoreAth) ;
}
//---------------------------------------------------------------------------------------------------------------//
// Copy 
MbDRAT*  MboyAthToolHelper::GiveMuPatSegmentToMbDRAT( const Muon::MuPatSegment* pMuPatSegment, int& kounter )
{
 if ( pMuPatSegment == 0 ) return 0 ;
 if ( (pMuPatSegment->mboyInfo) == 0 ) return 0 ;

  MbDRAT* pMbDRAT = new MbDRAT() ;
  
  pMbDRAT->m_kounter = kounter    ;
  pMbDRAT->m_ISC0    = (pMuPatSegment->mboyInfo)->m_ISC0    ;
  pMbDRAT->m_FFDRAT  = (pMuPatSegment->mboyInfo)->m_FFDRAT  ;
  pMbDRAT->m_GGDRAT  = (pMuPatSegment->mboyInfo)->m_GGDRAT  ;
  pMbDRAT->m_EZDRAT  = (pMuPatSegment->mboyInfo)->m_EZDRAT  ;
  pMbDRAT->m_JFDRAT  = (pMuPatSegment->mboyInfo)->m_JFDRAT  ;
  pMbDRAT->m_IWDRAT1 = (pMuPatSegment->mboyInfo)->m_IWDRAT1 ;
  pMbDRAT->m_ZZDRAT1 = (pMuPatSegment->mboyInfo)->m_ZZDRAT1 ;
  pMbDRAT->m_TTDRAT1 = (pMuPatSegment->mboyInfo)->m_TTDRAT1 ;
  pMbDRAT->m_SSDRAT1 = (pMuPatSegment->mboyInfo)->m_SSDRAT1 ;
  pMbDRAT->m_IWDRAT2 = (pMuPatSegment->mboyInfo)->m_IWDRAT2 ;
  pMbDRAT->m_ZZDRAT2 = (pMuPatSegment->mboyInfo)->m_ZZDRAT2 ;
  pMbDRAT->m_TTDRAT2 = (pMuPatSegment->mboyInfo)->m_TTDRAT2 ;
  pMbDRAT->m_SSDRAT2 = (pMuPatSegment->mboyInfo)->m_SSDRAT2 ;

  pMbDRAT->m_STRICT =  0 ;
  const Muon::MuonSegment* pMuonSegment = pMuPatSegment->segment ;
  if (pMuonSegment){
    pMbDRAT->m_CCDRAT = (pMuonSegment->fitQuality())->chiSquared() ;
    const Muon::MuonSegmentQuality* pMuonSegmentQuality = dynamic_cast<const Muon::MuonSegmentQuality*>(pMuonSegment->fitQuality());
    if( pMuonSegmentQuality ){
      if ( pMuonSegmentQuality->isStrict() ) pMbDRAT->m_STRICT = 1 ;
    }
  }
  
  return pMbDRAT ;
  
}
void MboyAthToolHelper::CopyMbDRATToMuPatSegment     ( const MbDRAT*             pMbDRAT      , Muon::MuPatSegment* pMuPatSegment )
{
 if ( pMbDRAT == 0 ) return ;
 if ( pMuPatSegment == 0 ) return ;
 
 if ( (pMuPatSegment->mboyInfo) == 0 ) pMuPatSegment->mboyInfo = new Muon::MuPatSegment::MboyInfo();

  (pMuPatSegment->mboyInfo)->m_ISC0    = pMbDRAT->m_ISC0    ;
  (pMuPatSegment->mboyInfo)->m_FFDRAT  = pMbDRAT->m_FFDRAT  ;
  (pMuPatSegment->mboyInfo)->m_GGDRAT  = pMbDRAT->m_GGDRAT  ;
  (pMuPatSegment->mboyInfo)->m_EZDRAT  = pMbDRAT->m_EZDRAT  ;
  (pMuPatSegment->mboyInfo)->m_JFDRAT  = pMbDRAT->m_JFDRAT  ;
  (pMuPatSegment->mboyInfo)->m_IWDRAT1 = pMbDRAT->m_IWDRAT1 ;
  (pMuPatSegment->mboyInfo)->m_ZZDRAT1 = pMbDRAT->m_ZZDRAT1 ;
  (pMuPatSegment->mboyInfo)->m_TTDRAT1 = pMbDRAT->m_TTDRAT1 ;
  (pMuPatSegment->mboyInfo)->m_SSDRAT1 = pMbDRAT->m_SSDRAT1 ;
  (pMuPatSegment->mboyInfo)->m_IWDRAT2 = pMbDRAT->m_IWDRAT2 ;
  (pMuPatSegment->mboyInfo)->m_ZZDRAT2 = pMbDRAT->m_ZZDRAT2 ;
  (pMuPatSegment->mboyInfo)->m_TTDRAT2 = pMbDRAT->m_TTDRAT2 ;
  (pMuPatSegment->mboyInfo)->m_SSDRAT2 = pMbDRAT->m_SSDRAT2 ;
}
void  MboyAthToolHelper::CopyMuPatSegmentToMbProtoTrack  ( 
                                                 int& Iok, 
                                                 const Muon::MuPatSegment*   pMuPatSegment  , 
                                                 MbProtoTrack* pMbProtoTrack 
){

  ATH_MSG_DEBUG( "CopyMuPatSegmentToMbProtoTrack: In " );
  Iok = 0 ;
 
  if ( pMbProtoTrack == 0 ) return ;
  if ( pMuPatSegment == 0 ) return ;
  ATH_MSG_DEBUG( "CopyMuPatSegmentToMbProtoTrack: Before test mboyInfo " );
  if ( (pMuPatSegment->mboyInfo) == 0 ) return ;
  ATH_MSG_DEBUG( "CopyMuPatSegmentToMbProtoTrack: After  test mboyInfo " );

// Set Starting status
  pMbProtoTrack->m_MboyStatus = 0 ;
  
  pMbProtoTrack->m_ISC0 = (pMuPatSegment->mboyInfo)->m_ISC0 ;

  pMbProtoTrack->m_DZT1 = 0 ;
  pMbProtoTrack->m_DS1  = 0 ;
  pMbProtoTrack->m_DZT2 = 0 ;
  pMbProtoTrack->m_DS2  = 0 ;

  pMbProtoTrack->m_CFI = cos((pMuPatSegment->mboyInfo)->m_FFDRAT) ;
  pMbProtoTrack->m_SFI = sin((pMuPatSegment->mboyInfo)->m_FFDRAT) ;
  pMbProtoTrack->m_CFA = cos((pMuPatSegment->mboyInfo)->m_GGDRAT) ;
  pMbProtoTrack->m_SFA = sin((pMuPatSegment->mboyInfo)->m_GGDRAT) ;

  pMbProtoTrack->m_SMU = 0;
  
  pMbProtoTrack->m_Z1 = (pMuPatSegment->mboyInfo)->m_ZZDRAT1 ;
  pMbProtoTrack->m_T1 = (pMuPatSegment->mboyInfo)->m_TTDRAT1 ;
  pMbProtoTrack->m_S1 = (pMuPatSegment->mboyInfo)->m_SSDRAT1 ;
  
  pMbProtoTrack->m_Z2 = (pMuPatSegment->mboyInfo)->m_ZZDRAT2 ;
  pMbProtoTrack->m_T2 = (pMuPatSegment->mboyInfo)->m_TTDRAT2 ;
  pMbProtoTrack->m_S2 = (pMuPatSegment->mboyInfo)->m_SSDRAT2 ;

  Iok = 1 ;

}
void  MboyAthToolHelper::CopyMuPatTrackToMbProtoTrack  ( 
                                              int& Iok, 
                                              MboyVarByName* pMboyVarByName,
                                              MuonRecDigiStoreAth*  pMuonRecDigiStoreAth ,
                                              Muon::MuPatTrack*   pMuPatTrack  , 
                                              MbProtoTrack* pMbProtoTrack 
){

  ATH_MSG_DEBUG( "CopyMuPatTrackToMbProtoTrack: In " );
  Iok = 0 ;
 
  if ( pMbProtoTrack == 0 ) return ;
  if ( pMuPatTrack == 0 ) return ;

  ATH_MSG_DEBUG( "CopyMuPatTrackToMbProtoTrack: 1 " );
  pMbProtoTrack->m_MboyStatus = 0 ;
  if ( (pMuPatTrack->segments()).size() > 1 )pMbProtoTrack->m_MboyStatus = 1 ;

// If there is a mboyInfo
  if ( (pMuPatTrack->mboyInfo) != 0 ) {
    ATH_MSG_DEBUG( "CopyMuPatTrackToMbProtoTrack: there is a mboyInfo " );
    pMbProtoTrack->m_MboyStatus = (pMuPatTrack->mboyInfo)->m_MboyStatus ;
    
    pMbProtoTrack->m_ISC0 = (pMuPatTrack->mboyInfo)->m_ISC0 ;
      
    pMbProtoTrack->m_DZT1 = (pMuPatTrack->mboyInfo)->m_DZT1 ;
    pMbProtoTrack->m_DS1  = (pMuPatTrack->mboyInfo)->m_DS1  ;
    pMbProtoTrack->m_DZT2 = (pMuPatTrack->mboyInfo)->m_DZT2 ;
    pMbProtoTrack->m_DS2  = (pMuPatTrack->mboyInfo)->m_DS2  ;

    pMbProtoTrack->m_SMU  = (pMuPatTrack->mboyInfo)->m_SMU  ;

    pMbProtoTrack->m_CFI  = (pMuPatTrack->mboyInfo)->m_CFI  ;
    pMbProtoTrack->m_SFI  = (pMuPatTrack->mboyInfo)->m_SFI  ;
    pMbProtoTrack->m_CFA  = (pMuPatTrack->mboyInfo)->m_CFA  ;
    pMbProtoTrack->m_SFA  = (pMuPatTrack->mboyInfo)->m_SFA  ;
      
    pMbProtoTrack->m_Z1   = (pMuPatTrack->mboyInfo)->m_Z1   ;
    pMbProtoTrack->m_T1   = (pMuPatTrack->mboyInfo)->m_T1   ;
    pMbProtoTrack->m_S1   = (pMuPatTrack->mboyInfo)->m_S1   ;

    pMbProtoTrack->m_Z2   = (pMuPatTrack->mboyInfo)->m_Z2   ;
    pMbProtoTrack->m_T2   = (pMuPatTrack->mboyInfo)->m_T2   ;
    pMbProtoTrack->m_S2   = (pMuPatTrack->mboyInfo)->m_S2   ;
    
    Iok = 1 ;
    return ;
    
  }
 
//
//From here there is no mboyInfo 
  if ( (pMuPatTrack->segments()).size() == 1 ){
    CopyMuPatSegmentToMbProtoTrack ( 
                                   Iok, 
                                   *((pMuPatTrack->segments()).begin())  , 
                                   pMbProtoTrack 
                                  );
    return ;
  } 

//From here there is no mboyInfo 
//and there is more than 1 segments
  ATH_MSG_DEBUG( "CopyMuPatTrackToMbProtoTrack: there is NO mboyInfo " );
  MbDRAT*  pMbDRAT = p_ITrackToProtoTrackTool->MbDRATToUpdateFitParameters( 
                                                                           pMuonRecDigiStoreAth ,
                                                                           &(pMuPatTrack->track())
                                                                          );
  if ( pMbDRAT == 0 ) return ;
  SetAMbDRAT( pMboyVarByName , pMbDRAT );

  pMbProtoTrack->m_ISC0 = pMbDRAT->m_ISC0 ;
  pMbProtoTrack->m_DZT1 = 0 ;
  pMbProtoTrack->m_DS1  = 0 ;
  pMbProtoTrack->m_DZT2 = 0 ;
  pMbProtoTrack->m_DS2  = 0 ;
  pMbProtoTrack->m_CFI = cos(pMbDRAT->m_FFDRAT) ;
  pMbProtoTrack->m_SFI = sin(pMbDRAT->m_FFDRAT) ;
  pMbProtoTrack->m_CFA = cos(pMbDRAT->m_GGDRAT) ;
  pMbProtoTrack->m_SFA = sin(pMbDRAT->m_GGDRAT) ;
  
  double TheSmu = pMbDRAT->m_Charge / pMbDRAT->m_Mome ;
  double ProdSca = pMbDRAT->m_Xpoint * pMbDRAT->m_Xdir
                 + pMbDRAT->m_Ypoint * pMbDRAT->m_Ydir
                 + pMbDRAT->m_Zpoint * pMbDRAT->m_Zdir ;
  if ( ProdSca > 0 ) TheSmu = -TheSmu ;
  pMbProtoTrack->m_SMU = TheSmu;

  pMbProtoTrack->m_Z1 = pMbDRAT->m_ZZDRAT1 ;
  pMbProtoTrack->m_T1 = pMbDRAT->m_TTDRAT1 ;
  pMbProtoTrack->m_S1 = pMbDRAT->m_SSDRAT1 ;
  pMbProtoTrack->m_Z2 = pMbDRAT->m_ZZDRAT2 ;
  pMbProtoTrack->m_T2 = pMbDRAT->m_TTDRAT2 ;
  pMbProtoTrack->m_S2 = pMbDRAT->m_SSDRAT2 ;
  
  delete pMbDRAT;
  
  Iok = 1 ;
  return ;

}
void MboyAthToolHelper::UpdateMuPatTrackFromMbProtoTrack ( const MbProtoTrack* pMbProtoTrack, Muon::MuPatTrack*   pMuPatTrack )
{
 if ( pMbProtoTrack == 0 ) return ;
 if ( pMuPatTrack == 0 ) return ;

 if ( (pMuPatTrack->mboyInfo) == 0 ) pMuPatTrack->mboyInfo = new Muon::MuPatTrack::MboyInfo();
 
  (pMuPatTrack->mboyInfo)->m_MboyStatus = pMbProtoTrack->m_MboyStatus ;
 
  (pMuPatTrack->mboyInfo)->m_ISC0 = pMbProtoTrack->m_ISC0 ;
 
  (pMuPatTrack->mboyInfo)->m_DZT1 = pMbProtoTrack->m_DZT1 ;
  (pMuPatTrack->mboyInfo)->m_DS1  = pMbProtoTrack->m_DS1  ;
  (pMuPatTrack->mboyInfo)->m_DZT2 = pMbProtoTrack->m_DZT2 ;
  (pMuPatTrack->mboyInfo)->m_DS2  = pMbProtoTrack->m_DS2  ;

  (pMuPatTrack->mboyInfo)->m_SMU  = pMbProtoTrack->m_SMU  ;

  (pMuPatTrack->mboyInfo)->m_CFI  = pMbProtoTrack->m_CFI  ;
  (pMuPatTrack->mboyInfo)->m_SFI  = pMbProtoTrack->m_SFI  ;
  (pMuPatTrack->mboyInfo)->m_CFA  = pMbProtoTrack->m_CFA  ;
  (pMuPatTrack->mboyInfo)->m_SFA  = pMbProtoTrack->m_SFA  ;
 
  (pMuPatTrack->mboyInfo)->m_Z1   = pMbProtoTrack->m_Z1   ;
  (pMuPatTrack->mboyInfo)->m_T1   = pMbProtoTrack->m_T1   ;
  (pMuPatTrack->mboyInfo)->m_S1   = pMbProtoTrack->m_S1   ;

  (pMuPatTrack->mboyInfo)->m_Z2   = pMbProtoTrack->m_Z2   ;
  (pMuPatTrack->mboyInfo)->m_T2   = pMbProtoTrack->m_T2   ;
  (pMuPatTrack->mboyInfo)->m_S2   = pMbProtoTrack->m_S2   ;

}
Trk::Track* MboyAthToolHelper::MbProtoTrack2Track(
                                          MbProtoTrack* pMbProtoTrack ,
                                          std::vector<Muon::MuPatSegment*>& MuPatSegmentS_for_New_pMuPatTrack 
){

  
  DataVector<const Trk::TrackStateOnSurface>* pTrackStateOnSurface_DataVector 
       = new DataVector<const Trk::TrackStateOnSurface> ;

  int IsThereTSOS = 0 ;
  std::vector< MbDRAT* >::iterator             itDRAT     = (pMbProtoTrack->m_VectorOfMbDRATTRAKDO).begin() ;
  std::vector< MbDRAT* >::iterator             itDRAT_end = (pMbProtoTrack->m_VectorOfMbDRATTRAKDO).end()   ;
  std::vector< Muon::MuPatSegment* >::iterator itSeg      = MuPatSegmentS_for_New_pMuPatTrack.begin()       ;
  for(; itDRAT!=itDRAT_end;++itDRAT,++itSeg){
    IsThereTSOS = 1 ;
    const Muon::MuonSegment* pMuonSegment   = ((*itSeg )->segment)->clone() ;
    const Trk::TrackParameters* pTrackParameters = 0 ;
//     std::cout << "***********************" << std::endl ;
//     std::cout << " (*itDRAT)->m_Xpoint " << (*itDRAT)->m_Xpoint << std::endl ;
//     std::cout << " (*itDRAT)->m_Ypoint " << (*itDRAT)->m_Ypoint << std::endl ;
//     std::cout << " (*itDRAT)->m_Zpoint " << (*itDRAT)->m_Zpoint << std::endl ;
//     std::cout << " (*itDRAT)->m_Xdir   " << (*itDRAT)->m_Xdir   << std::endl ;
//     std::cout << " (*itDRAT)->m_Ydir   " << (*itDRAT)->m_Ydir   << std::endl ;
//     std::cout << " (*itDRAT)->m_Zdir   " << (*itDRAT)->m_Zdir   << std::endl ;
//     std::cout << " (*itDRAT)->m_Mome   " << (*itDRAT)->m_Mome   << std::endl ;
//     std::cout << " (*itDRAT)->m_Charge " << (*itDRAT)->m_Charge << std::endl ;
//     double ProdSca = (*itDRAT)->m_Xpoint * (*itDRAT)->m_Xdir
//                    + (*itDRAT)->m_Ypoint * (*itDRAT)->m_Ydir
//                    + (*itDRAT)->m_Zpoint * (*itDRAT)->m_Zdir ;
//     std::cout << " ProdSca             " << ProdSca << std::endl ;
    p_IMuonboyToTrackParametersTool->DoTheTrackParametersCorrectedSurface(
                                                                          &(pMuonSegment->associatedSurface()),
                                                                          (*itDRAT)->m_Xpoint ,
                                                                          (*itDRAT)->m_Ypoint ,
                                                                          (*itDRAT)->m_Zpoint ,
                                                                          (*itDRAT)->m_Xdir   ,
                                                                          (*itDRAT)->m_Ydir   ,
                                                                          (*itDRAT)->m_Zdir   ,
                                                                          (*itDRAT)->m_Mome   ,
                                                                          (*itDRAT)->m_Charge ,
                                                                          pTrackParameters
                                                                         );
   const Trk::FitQualityOnSurface* pFitQualityOnSurface = 0;
   
   std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> pattern;
   pattern.set(Trk::TrackStateOnSurface::Measurement);
   
   
   Trk::TrackStateOnSurface* pTrackStateOnSurface = new  Trk::TrackStateOnSurface( pMuonSegment ,pTrackParameters, pFitQualityOnSurface,  0 , pattern) ;
   pTrackStateOnSurface_DataVector->push_back(pTrackStateOnSurface);

  }

  Trk::Track* pTrack = 0 ;
  if ( IsThereTSOS == 1 ){
    Trk::TrackInfo aTrackInfo(Trk::TrackInfo::Unknown, Trk::muon) ;
    aTrackInfo.setPatternRecognitionInfo( Trk::TrackInfo::Muonboy);
    aTrackInfo.setTrackFitter(Trk::TrackInfo::MuonboyFitter);
    
    const Trk::FitQuality* fitQuality = 0 ;

    pTrack = new Trk::Track(aTrackInfo, pTrackStateOnSurface_DataVector, fitQuality);
  }else{
    delete pTrackStateOnSurface_DataVector ;
  }

  return pTrack ;
}
