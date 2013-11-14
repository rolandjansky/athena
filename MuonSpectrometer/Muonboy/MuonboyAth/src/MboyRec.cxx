/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "MboyAthToolInterfaces/IMboyFullRecTool.h"
#include "MboyAthToolInterfaces/IMboyAthToolHelper.h"
#include "MboyEDMToolInterfaces/IMboyGetPrepDataFromEventTool.h"
 
/////////////////////////////////////////////////////////
#include "MuonboyAth/MboyRec.h"

MboyRec::MboyRec(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
 p_IMboyGetPrepDataFromEventTool ( "MboyGetPrepDataFromEventTool" ) ,
 p_OwnEDMHelper                  ( "MboyAthToolHelper" ),
 p_IMboyFullRecTool              ( "MboyFullRecTool" )
{

  m_EvtTot = 0 ;

  declareProperty("MboyGetPrepDataFromEventTool" , p_IMboyGetPrepDataFromEventTool ) ;
  declareProperty("MboyFullRecTool" , p_IMboyFullRecTool ) ;
  declareProperty("MboyAthToolHelper"                 , p_OwnEDMHelper          ) ;

//Declare the properties

  declareProperty( "SwitchOff"                                , m_SwitchOff                                = 0                                      ) ;
  declareProperty( "DoDbg"                                    , m_DoDbg                                    = 0                                      ) ;


  declareProperty( "DigiStoreLocation"                        , m_DigiStoreLocation                        = "Muonboy_DigiStore"                    ) ;
  declareProperty( "PrintDigitsSummary"                       , m_PrintDigitsSummary                       = 0                                      ) ;
  
  declareProperty( "TrackStoreLocation"                       , m_TrackStoreLocation                       = "Muonboy_TrackStore"                   ) ;
  declareProperty( "ShortSummaryTrack"                        , m_ShortSummaryTrack                        = 0                                      ) ;

  declareProperty( "SegmentStoreLocation"                     , m_SegmentStoreLocation                     = "Muonboy_SegmentStore"                 ) ;
  declareProperty( "ShortSummarySegment"                      , m_ShortSummarySegment                      = 0                                      ) ;

  declareProperty( "SegmentCollectionLocation"                , m_SegmentCollectionLocation                = "ConvertedMBoySegments"                ) ;
   
  declareProperty( "TrackCollectionLocation"                  , m_TrackCollectionLocation                  = "ConvertedMBoyTracks"                  ) ;
  declareProperty( "TrackParticleContainerLocation"           , m_TrackParticleContainerLocation           = "MuonboyTrackParticles"                ) ;

  declareProperty( "TrackCollection_MSonlyLocation"           , m_TrackCollection_MSonlyLocation           = "ConvertedMBoyMuonSpectroOnlyTracks"   ) ;
  declareProperty( "TrackParticleContainer_MSonlyLocation"    , m_TrackParticleContainer_MSonlyLocation    = "MuonboyMuonSpectroOnlyTrackParticles" ) ;

  declareProperty( "AthPosOff"                                , m_AthPosOff                                = 0                                      ) ;

  declareProperty( "ProcessingStage"                          , m_ProcessingStage                          = 3                                      ) ;

  declareProperty( "DoDump"                                   , m_DoDump                                   = 0                                      ) ;      
  declareProperty( "MboyDumpSegmentCollectionOutputFile"      , m_MboyDumpSegmentCollectionOutputFile      = "Out.MboyDumpSegmentCollection"        ) ;      
  declareProperty( "MboyDumpTrackCollectionOutputFile"        , m_MboyDumpTrackCollectionOutputFile        = "Out.MboyDumpTrackCollection"          ) ;        
  declareProperty( "MboyDumpTrackParticleContainerOutputFile" , m_MboyDumpTrackParticleContainerOutputFile = "Out.MboyDumpTrackParticleContainer"   ) ; 

}
MboyRec::~MboyRec(){}


// Initialize
StatusCode MboyRec::initialize(){

  ATH_MSG_INFO( "Initialisation started	  " );


  ATH_MSG_INFO( "================================" );
  ATH_MSG_INFO( "=Proprieties are        " );
  if (m_SwitchOff == 0) {

    ATH_MSG_INFO( "= DigiStoreLocation                         " << m_DigiStoreLocation		 );
    ATH_MSG_INFO( "= TrackStoreLocation                        " << m_TrackStoreLocation  	 );
    ATH_MSG_INFO( "= ShortSummaryTrack                         " << m_ShortSummaryTrack    );
    ATH_MSG_INFO( "= SegmentStoreLocation                      " << m_SegmentStoreLocation	 );
    ATH_MSG_INFO( "= ShortSummarySegment                       " << m_ShortSummarySegment  );
    ATH_MSG_INFO( "= SegmentCollectionLocation                 " << m_SegmentCollectionLocation          );
    ATH_MSG_INFO( "= TrackCollectionLocation                   " << m_TrackCollectionLocation          );
    ATH_MSG_INFO( "= TrackParticleContainerLocation            " << m_TrackParticleContainerLocation           );
    ATH_MSG_INFO( "= TrackCollection_MSonlyLocation            " << m_TrackCollection_MSonlyLocation          );
    ATH_MSG_INFO( "= TrackParticleContainer_MSonlyLocation     " << m_TrackParticleContainer_MSonlyLocation     );

    ATH_MSG_INFO( "= AthPosOff                                 " << m_AthPosOff          );
    ATH_MSG_INFO( "= PrintDigitsSummary                        " << m_PrintDigitsSummary  );
    ATH_MSG_INFO( "= DoDbg                                     " << m_DoDbg  );
    ATH_MSG_INFO( "= ProcessingStage                           " << m_ProcessingStage            );
    ATH_MSG_INFO( "= DoDump                                    " << m_DoDump 		);
    ATH_MSG_INFO( "= MboyDumpSegmentCollectionOutputFile       " << m_MboyDumpSegmentCollectionOutputFile 		);
    ATH_MSG_INFO( "= MboyDumpTrackCollectionOutputFile         " << m_MboyDumpTrackCollectionOutputFile			);
    ATH_MSG_INFO( "= MboyDumpTrackParticleContainerOutputFile  " << m_MboyDumpTrackParticleContainerOutputFile		);

  }
  ATH_MSG_INFO( "= SwitchOff                 " << m_SwitchOff                   );
  ATH_MSG_INFO( "================================" );


//Retrieve p_IMboyGetPrepDataFromEventTool
  if ( p_IMboyGetPrepDataFromEventTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMboyGetPrepDataFromEventTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMboyGetPrepDataFromEventTool );

//Retrieve p_OwnEDMHelper
  if ( p_OwnEDMHelper.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_OwnEDMHelper );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_OwnEDMHelper );

//Retrieve p_IMboyFullRecTool 
  if ( p_IMboyFullRecTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMboyFullRecTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMboyFullRecTool );


//Set files for dumps for checks
  if ( m_DoDump ==1  && m_ProcessingStage >= 2 ){
    m_OutFile_MboyDumpSegmentCollectionOutputFile.open(m_MboyDumpSegmentCollectionOutputFile.c_str());
    m_OutFile_MboyDumpSegmentCollectionOutputFile << setiosflags(std::ios::fixed);
  }

  if ( m_DoDump ==1  && m_ProcessingStage >= 2 ){
    std::string MboyDumpTrackCollectionOutputFileMS = m_MboyDumpTrackCollectionOutputFile + "MS" ;

    m_OutFile_MboyDumpTrackCollectionOutputFile.open(m_MboyDumpTrackCollectionOutputFile.c_str());
    m_OutFile_MboyDumpTrackCollectionOutputFile << setiosflags(std::ios::fixed);

    m_OutFile_MboyDumpTrackCollectionOutputFile_MSonly.open(MboyDumpTrackCollectionOutputFileMS.c_str());
    m_OutFile_MboyDumpTrackCollectionOutputFile_MSonly << setiosflags(std::ios::fixed);

  }

  if ( m_DoDump ==1  && m_ProcessingStage >= 3 ){
    std::string MboyDumpTrackParticleContainerOutputFileChecks = m_MboyDumpTrackParticleContainerOutputFile + "MS" ;
    m_OutFile_MboyDumpTrackParticleContainerOutputFile.open(m_MboyDumpTrackParticleContainerOutputFile.c_str());
    m_OutFile_MboyDumpTrackParticleContainerOutputFile << setiosflags(std::ios::fixed);
    m_OutFile_MboyDumpTrackParticleContainerOutputFile_MSonly.open(MboyDumpTrackParticleContainerOutputFileChecks.c_str());
    m_OutFile_MboyDumpTrackParticleContainerOutputFile_MSonly << setiosflags(std::ios::fixed);
  }

  m_EvtTot = 0 ;
  m_KounterMB_SegmentStoreTDS.clear();
  m_KounterMB_TrackStoreTDS.clear();
  m_KounterSegmentCollection.clear();
  m_KounterTrackCollection.clear();


  ATH_MSG_INFO( "Initialisation ended     " );

  return StatusCode::SUCCESS;

}

// Execute
StatusCode MboyRec::execute(){

  StatusCode sc = StatusCode::SUCCESS;


//Collect PrepRawData
  std::vector<const Trk::PrepRawData*> VectorOfpPrepData ;
  VectorOfpPrepData.clear();

  sc=p_IMboyGetPrepDataFromEventTool->CollectPrepData(VectorOfpPrepData);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " CollectPrepData failed" );
    return( StatusCode::FAILURE );
  }

  if ( m_DoDbg == 1 ){ 
    ATH_MSG_INFO(" VectorOfpPrepData size " << VectorOfpPrepData.size() );
  }

  sc = p_IMboyFullRecTool->DoRec(
                             VectorOfpPrepData,
                             m_PrintDigitsSummary , m_AthPosOff , m_SwitchOff , m_ShortSummarySegment  , m_ShortSummaryTrack ,
                             m_DigiStoreLocation , m_TrackStoreLocation , m_SegmentStoreLocation ,
                             m_ProcessingStage ,
                             m_SegmentCollectionLocation,
                             m_TrackCollectionLocation      , m_TrackCollection_MSonlyLocation       ,
                             m_TrackParticleContainerLocation , m_TrackParticleContainer_MSonlyLocation
                            );
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " DoRec failed " );
    return( StatusCode::FAILURE);
  }

//Containers count 
  sc = KountThem();
  if ( sc.isFailure() ) ATH_MSG_WARNING( "KountThem failed ");


//Containers dump 
  if ( m_DoDump ==1  ){
    sc = DumpContainers();
    if ( sc.isFailure() ) ATH_MSG_WARNING( "DumpContainers failed ");
  }

  return StatusCode::SUCCESS;

}

// Finalize
StatusCode MboyRec::finalize(){

  if (m_SwitchOff == 1) return StatusCode::SUCCESS;
  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Finalisation started	" );


//Print Containers count
  sc = PrintKountThem();
  if ( sc.isFailure() ) ATH_MSG_WARNING( "PrintKountThem failed ");


  ATH_MSG_INFO( "Finalisation ended     " );

  return StatusCode::SUCCESS;

}

StatusCode MboyRec::KountThem(){

  StatusCode sc = StatusCode::SUCCESS;


//Count all evts
  m_EvtTot = m_EvtTot + 1 ;


//Kount MB_SegmentStoreTDS 
  sc = p_IMboyFullRecTool->KountMB_SegmentStoreTDS( m_KounterMB_SegmentStoreTDS   , m_SegmentStoreLocation ) ;
  if ( sc.isFailure() ) ATH_MSG_WARNING( "KountMB_SegmentStoreTDS failed ");


//Kount MB_TrackStoreTDS 
  sc = p_IMboyFullRecTool->KountMB_TrackStoreTDS( m_KounterMB_TrackStoreTDS   ,m_TrackStoreLocation) ;
  if ( sc.isFailure() ) ATH_MSG_WARNING( "KountMB_TrackStoreTDS failed ");


//Kount SegmentCollection 
  if (m_ProcessingStage >= 1 ){
    sc = p_IMboyFullRecTool->KountSegmentCollection( m_KounterSegmentCollection   , m_SegmentCollectionLocation) ; 
    if ( sc.isFailure() ) ATH_MSG_WARNING( "KountSegmentCollection failed ");
  }


//Kount TrackCollection 
  if (m_ProcessingStage >= 2 ){
    sc = p_IMboyFullRecTool->KountTrackCollection( m_KounterTrackCollection   , m_TrackCollectionLocation) ;
    if ( sc.isFailure() ) ATH_MSG_WARNING( "KountTrackCollection failed ");
  }

  return StatusCode::SUCCESS;

}

StatusCode MboyRec::PrintKountThem(){

  StatusCode sc = StatusCode::SUCCESS;


  ATH_MSG_INFO( "================================" );

  ATH_MSG_INFO( "*Total number of Evts: MboyEvtTot " << m_EvtTot );

//Print kounting of  MB_SegmentStoreTDS 
  sc = PrintKountMB_SegmentStoreTDS();
  if ( sc.isFailure() ) ATH_MSG_WARNING( "PrintKountMB_SegmentStoreTDS failed ");

//Print kounting of  MB_TrackStoreTDS 
  sc = PrintKountMB_TrackStoreTDS();
  if ( sc.isFailure() ) ATH_MSG_WARNING( "PrintKountMB_SegmentStoreTDS failed ");

//Print kounting of  SegmentCollection 
  if (m_ProcessingStage >= 1 ){
    sc = PrintKountSegmentCollection();
    if ( sc.isFailure() ) ATH_MSG_WARNING( "PrintKountSegmentCollection failed ");
  }

//Print kounting of  TrackCollection 
  if (m_ProcessingStage >= 2 ){
    sc = PrintKountTrackCollection();
    if ( sc.isFailure() ) ATH_MSG_WARNING( "PrintKountTrackCollection failed ");
  }

  ATH_MSG_INFO( "================================" );

  return StatusCode::SUCCESS;

}

StatusCode MboyRec::PrintKountMB_SegmentStoreTDS(){

  ATH_MSG_INFO( "   " ); ;
  ATH_MSG_INFO( "*Nber of Segments in " << m_SegmentStoreLocation); ;
  int ItemNberStart = 0 ;
  int ItemNberEnd	= m_KounterMB_SegmentStoreTDS.size() ;
  ATH_MSG_INFO( "   " ); ;
  ATH_MSG_INFO( "  Nber Of Segments/Nber Of Evts" ); ;
  for (int ItemNber=ItemNberStart; ItemNber<ItemNberEnd ; ItemNber++){
    if (m_KounterMB_SegmentStoreTDS[ItemNber] !=0 ){
      ATH_MSG_INFO( " " << ItemNber 
          << " " << m_KounterMB_SegmentStoreTDS[ItemNber]
          << " (" << (100.*m_KounterMB_SegmentStoreTDS[ItemNber])/m_EvtTot<< ")" 
          );
    }
  }

  return StatusCode::SUCCESS;

}

StatusCode MboyRec::PrintKountMB_TrackStoreTDS(){

  ATH_MSG_INFO( "   " ); ;
  ATH_MSG_INFO( "*Nber of Tracks in " << m_TrackStoreLocation); ;
  int ItemNberStart = 0 ;
  int ItemNberEnd   = m_KounterMB_TrackStoreTDS.size() ;
  ATH_MSG_INFO( "   " ); ;
  ATH_MSG_INFO( "  Nber Of Tracks/Nber Of Evts" ); ;
  for (int ItemNber=ItemNberStart; ItemNber<ItemNberEnd ; ItemNber++){
    if (m_KounterMB_TrackStoreTDS[ItemNber] !=0 ){
      ATH_MSG_INFO( " " << ItemNber 
          << " " << m_KounterMB_TrackStoreTDS[ItemNber]
          << " (" << (100.*m_KounterMB_TrackStoreTDS[ItemNber])/m_EvtTot<< ")" 
          );
    }
  }

  return StatusCode::SUCCESS;

}




StatusCode MboyRec::PrintKountSegmentCollection(){

  ATH_MSG_INFO( "   " ); ;
  ATH_MSG_INFO( "*Nber of Trk::Segment in " << m_SegmentCollectionLocation); ;
  int ItemNberStart = 0 ;
  int ItemNberEnd	= m_KounterSegmentCollection.size() ;
  ATH_MSG_INFO( "   " ); ;
  ATH_MSG_INFO( "  Nber Of Segments/Nber Of Evts" ); ;
  for (int ItemNber=ItemNberStart; ItemNber<ItemNberEnd ; ItemNber++){
   if (m_KounterSegmentCollection[ItemNber] !=0 ){
   ATH_MSG_INFO( " " << ItemNber 
      << " " << m_KounterSegmentCollection[ItemNber]
      << " (" << (100.*m_KounterSegmentCollection[ItemNber])/m_EvtTot<< ")" 
      );
   }
  }

  return StatusCode::SUCCESS;

}

StatusCode MboyRec::PrintKountTrackCollection(){

  ATH_MSG_INFO( "   " ); ;
  ATH_MSG_INFO( "*Nber of Trk::Track in " << m_TrackCollectionLocation); ;
  int ItemNberStart = 0 ;
  int ItemNberEnd   = m_KounterTrackCollection.size() ;
  ATH_MSG_INFO( "   " ); ;
  ATH_MSG_INFO( "  Nber Of Tracks/Nber Of Evts" ); ;
  for (int ItemNber=ItemNberStart; ItemNber<ItemNberEnd ; ItemNber++){
   if (m_KounterTrackCollection[ItemNber] !=0 ){
   ATH_MSG_INFO( " " << ItemNber 
      << " " << m_KounterTrackCollection[ItemNber]
      << " (" << (100.*m_KounterTrackCollection[ItemNber])/m_EvtTot<< ")" 
      );
   }
  }

  return StatusCode::SUCCESS;

}


StatusCode MboyRec::DumpContainers(){

  StatusCode sc = StatusCode::SUCCESS;

  std::ofstream* pOutFile;

  if ( m_DoDump ==1  && m_ProcessingStage >= 2 ){
    pOutFile = &m_OutFile_MboyDumpSegmentCollectionOutputFile ;
    sc = p_OwnEDMHelper->MboyDumpSegmentCollectionToolDoDump( m_SegmentCollectionLocation, m_TrackCollectionLocation, pOutFile );
    if ( sc.isFailure() )ATH_MSG_WARNING( "DoDump failed " );
  }
  
  if ( m_DoDump ==1  && m_ProcessingStage >= 2 ){
    pOutFile = &m_OutFile_MboyDumpTrackCollectionOutputFile ;
    sc = p_OwnEDMHelper->MboyDumpTrackCollectionToolDoDump(  m_TrackCollectionLocation, pOutFile );
    if ( sc.isFailure() )ATH_MSG_WARNING( "DoDump failed " );
    pOutFile = &m_OutFile_MboyDumpTrackCollectionOutputFile_MSonly ;
    sc = p_OwnEDMHelper->MboyDumpTrackCollectionToolDoDump( m_TrackCollection_MSonlyLocation, pOutFile );
    if ( sc.isFailure() )ATH_MSG_WARNING( "DoDump failed " );
  }
  
  if ( m_DoDump ==1  && m_ProcessingStage >= 3 ){
    pOutFile = &m_OutFile_MboyDumpTrackParticleContainerOutputFile ;
    sc = p_OwnEDMHelper->MboyDumpTrackParticleContainerToolDoDump( m_TrackParticleContainerLocation, pOutFile );
    if ( sc.isFailure() )ATH_MSG_WARNING( "DoDump failed " );
    pOutFile = &m_OutFile_MboyDumpTrackParticleContainerOutputFile_MSonly ;
    sc = p_OwnEDMHelper->MboyDumpTrackParticleContainerToolDoDump(  m_TrackParticleContainer_MSonlyLocation, pOutFile );
    if ( sc.isFailure() )ATH_MSG_WARNING( "DoDump failed " );
  }
  
  if ( sc.isFailure() ) ATH_MSG_WARNING( "DumpContainers failed " );

  return StatusCode::SUCCESS;

}
