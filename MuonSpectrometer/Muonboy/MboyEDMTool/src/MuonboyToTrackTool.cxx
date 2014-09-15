/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

//----------------------------------------------------------------//
#include "CLHEP/Matrix/SymMatrix.h"

#include "TrkParameters/TrackParameters.h"

#include "TrkEventPrimitives/ParamDefs.h"
 
#include "Identifier/Identifier.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "MuonPrepRawData/CscStripPrepDataCollection.h"
#include "MuonPrepRawData/CscStripPrepData.h"

#include "TrkTrack/TrackInfo.h"

//----------------------------------------------------------------//
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"

#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"

#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"

/////////////////////////////////////////////////////////
#include "MboyEDMTool/MuonboyToTrackTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MboyEDMToolInterfaces/IMboyTrackUndressingTool.h"

//----------------------------------------------------------------//
#include "MboyEDMToolInterfaces/IMbMuScToTSOSTool.h"
#include "MboyEDMToolInterfaces/IMboyProperTrackRepresentationTransformTool.h"
#include "MboyEDMToolInterfaces/IMboyTrackRepresentationTransformTool.h"
#include "MboyEDMToolInterfaces/IMboySortingTool.h"
#include "MboyEDMToolInterfaces/IMuonboyToTrackParametersTool.h"
#include "MboyEDMToolInterfaces/IMboyEDMHelper.h"
#include "MboyAthEvt/MB_TrackAth.h"
#include "MboyAthEvt/MB_TrackStoreAth.h"
#include "MboyCoreEvt/MbPOT.h"
#include "MboyCoreEvt/MbIOObject.h"
#include "MboyCoreEvt/MbROT.h"
#include "MboyCoreEvt/MbROTMDT.h"

Trk::MuonboyToTrackTool::MuonboyToTrackTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p)
, p_IMbMuScToTSOSTool                 ( "Trk::MbMuScToTSOSTool/MbMuScToTSOSTool" ) 
, p_IMboyProperTrackRepresentationTransformTool                 ( "Muon::MboyProperTrackRepresentationTransformTool/MboyProperTrackRepresentationTransformTool" ) 
, p_IMboyTrackRepresentationTransformTool                 ( "Muon::MboyTrackRepresentationTransformTool/MboyTrackRepresentationTransformTool" ) 
, p_IMboySortingTool                 ( "Muon::MboySortingTool/MboySortingTool" ) 
, p_IMboyTrackUndressingTool         ( "Muon::MboyTrackUndressingTool/MboyTrackUndressingTool" ) 
, p_IMdtDriftCircleOnTrackCreator    ( "Muon::MdtDriftCircleOnTrackCreator" ) 
, p_IMuonClusterOnTrackCreatorRPC        ( "Muon::MuonClusterOnTrackCreator"       )
, p_IMuonClusterOnTrackCreatorTGC        ( "Muon::MuonClusterOnTrackCreator"       )
, p_IMuonClusterOnTrackCreatorCSCStrip   ( "Muon::MuonClusterOnTrackCreator"       )
, p_IMuonClusterOnTrackCreatorCSCCluster ( "Muon::MuonClusterOnTrackCreator"        ) 
//, p_IMuonClusterOnTrackCreatorCSCCluster ( "Muon::CscClusterOnTrackCreator"        ) 
, p_OwnEDMHelper                     ( "MboyEDMHelper" ) 
, p_IMuonboyToTrackParametersTool    ( "Trk::MuonboyToTrackParametersTool" ),
 m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool")
{

  declareInterface<IMuonboyToTrackTool>(this);

//Declare the properties

  declareProperty("IMbMuScToTSOSTool"            , p_IMbMuScToTSOSTool ) ;
  declareProperty("IMboyProperTrackRepresentationTransformTool"            , p_IMboyProperTrackRepresentationTransformTool ) ;
  declareProperty("IMboyTrackRepresentationTransformTool"            , p_IMboyTrackRepresentationTransformTool ) ;
  declareProperty("IMboySortingTool"            , p_IMboySortingTool ) ;
  declareProperty("IMboyTrackUndressingTool"    , p_IMboyTrackUndressingTool ) ;
  
  declareProperty("IMdtDriftCircleOnTrackCreator"    , p_IMdtDriftCircleOnTrackCreator ) ;
  declareProperty("IMuonClusterOnTrackCreatorRPC"         , p_IMuonClusterOnTrackCreatorRPC           ) ;
  declareProperty("IMuonClusterOnTrackCreatorTGC"         , p_IMuonClusterOnTrackCreatorTGC           ) ;
  declareProperty("IMuonClusterOnTrackCreatorCSCStrip"    , p_IMuonClusterOnTrackCreatorCSCStrip      ) ;
  declareProperty("IMuonClusterOnTrackCreatorCSCCluster"  , p_IMuonClusterOnTrackCreatorCSCCluster    ) ;
  declareProperty("MboyEDMHelper"                    , p_OwnEDMHelper ) ;
  declareProperty("MuonboyToTrackParametersTool"     , p_IMuonboyToTrackParametersTool ) ;

  declareProperty( "DoMdtConvertion"               , m_DoMdtConvertion               = 1 ) ;
  declareProperty( "DoRpcConvertion"               , m_DoRpcConvertion               = 1 ) ;
  declareProperty( "DoTgcConvertion"               , m_DoTgcConvertion               = 1 ) ;
  declareProperty( "DoCscConvertion"               , m_DoCscConvertion               = 1 ) ;

  declareProperty( "DoMbMuSc"               , m_DoMbMuSc              = 1  ) ;

  declareProperty( "DoMdtHole"              , m_DoMdtHole              = 1  ) ;
  declareProperty( "DoRpcHole"              , m_DoRpcHole              = 0  ) ;
  declareProperty( "DoTgcHole"              , m_DoTgcHole              = 1  ) ;
  declareProperty( "DoCscHole"              , m_DoCscHole              = 1  ) ;

  declareProperty( "AddMuonSystemEntranceToMain"   , m_AddMuonSystemEntranceToMain   = 1 ) ;
  declareProperty( "AddCaloEntranceToMain"         , m_AddCaloEntranceToMain         = 1 ) ;
  declareProperty( "AddUnMeasuredPerigeeeToMSOnly" , m_AddUnMeasuredPerigeeeToMSOnly = 1 ) ;

  declareProperty( "AddTrackParametersOnRotTSOS"   , m_AddTrackParametersOnRotTSOS   = 1 ) ;
 
  declareProperty( "DoCorrectPosition"             , m_DoCorrectPosition            = 1  ) ;
  
  declareProperty( "DoDbg"                         , m_DoDbg                         = 0 ) ;

  declareProperty( "AddPerigeeAtBeamSpot"          , m_AddPerigeeAtBeamSpot        = 0 ) ;

  declareProperty( "TEMPORARYNewTrackInfoEnumUse" , m_TEMPORARYNewTrackInfoEnumUse = 1 ) ;
  declareProperty( "MuonIdHelper" , m_idHelper);

  m_AddUnMeasuredPerigeee = 1 ;

}

Trk::MuonboyToTrackTool::~MuonboyToTrackTool(){}

// Initialize
StatusCode Trk::MuonboyToTrackTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" );
    return( StatusCode::FAILURE );
  }


  ATH_MSG_INFO( "================================" );
  ATH_MSG_INFO( "=Proprieties are     " );
  ATH_MSG_INFO( "= DoMdtConvertion                 " << m_DoMdtConvertion );
  ATH_MSG_INFO( "= DoRpcConvertion                 " << m_DoRpcConvertion );
  ATH_MSG_INFO( "= DoTgcConvertion                 " << m_DoTgcConvertion );
  ATH_MSG_INFO( "= DoCscConvertion                 " << m_DoCscConvertion );
  ATH_MSG_INFO( "= DoMbMuSc                        " << m_DoMbMuSc );
  ATH_MSG_INFO( "= DoMdtHole                       " << m_DoMdtHole );
  ATH_MSG_INFO( "= DoRpcHole                       " << m_DoRpcHole );
  ATH_MSG_INFO( "= DoTgcHole                       " << m_DoTgcHole );
  ATH_MSG_INFO( "= DoCscHole                       " << m_DoCscHole );
  ATH_MSG_INFO( "= AddMuonSystemEntranceToMain     " << m_AddMuonSystemEntranceToMain );
  ATH_MSG_INFO( "= AddCaloEntranceToMain           " << m_AddCaloEntranceToMain       );
  ATH_MSG_INFO( "= AddUnMeasuredPerigeee           " << m_AddUnMeasuredPerigeee       );
  ATH_MSG_INFO( "= AddUnMeasuredPerigeeeToMSOnly   " << m_AddUnMeasuredPerigeeeToMSOnly       );
  ATH_MSG_INFO( "= AddTrackParametersOnRotTSOS     " << m_AddTrackParametersOnRotTSOS       );
  ATH_MSG_INFO( "= DoCorrectPosition               " << m_DoCorrectPosition       );
  ATH_MSG_INFO( "= DoDbg                           " << m_DoDbg       );
  ATH_MSG_INFO( "= AddPerigeeAtBeamSpot            " << m_AddPerigeeAtBeamSpot );
  ATH_MSG_INFO( "= TEMPORARYNewTrackInfoEnumUse    " << m_TEMPORARYNewTrackInfoEnumUse       );
  ATH_MSG_INFO( "================================" );


//Retrieve p_IMbMuScToTSOSTool
  if ( p_IMbMuScToTSOSTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMbMuScToTSOSTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMbMuScToTSOSTool );

//Retrieve p_IMboyProperTrackRepresentationTransformTool
  if ( p_IMboyProperTrackRepresentationTransformTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMboyProperTrackRepresentationTransformTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMboyProperTrackRepresentationTransformTool );

//Retrieve p_IMboyTrackRepresentationTransformTool
  if ( p_IMboyTrackRepresentationTransformTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMboyTrackRepresentationTransformTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMboyTrackRepresentationTransformTool );

//Retrieve p_IMboySortingTool
  if ( p_IMboySortingTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMboySortingTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMboySortingTool );

//Retrieve p_IMuonboyToTrackParametersTool
  if ( p_IMuonboyToTrackParametersTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMuonboyToTrackParametersTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMuonboyToTrackParametersTool );

//Retrieve p_OwnEDMHelper
  if ( p_OwnEDMHelper.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_OwnEDMHelper );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_OwnEDMHelper );

//Retrieve p_IMboyTrackUndressingTool
  if ( p_IMboyTrackUndressingTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMboyTrackUndressingTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMboyTrackUndressingTool );

//Retrieve p_IMdtDriftCircleOnTrackCreator
  if ( p_IMdtDriftCircleOnTrackCreator.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMdtDriftCircleOnTrackCreator );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMdtDriftCircleOnTrackCreator );

//Retrieve p_IMuonClusterOnTrackCreatorRPC
  if ( p_IMuonClusterOnTrackCreatorRPC.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMuonClusterOnTrackCreatorRPC );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMuonClusterOnTrackCreatorRPC );

//Retrieve p_IMuonClusterOnTrackCreatorTGC
  if ( p_IMuonClusterOnTrackCreatorTGC.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMuonClusterOnTrackCreatorTGC );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMuonClusterOnTrackCreatorTGC );

//Retrieve p_IMuonClusterOnTrackCreatorCSCStrip
  if ( p_IMuonClusterOnTrackCreatorCSCStrip.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMuonClusterOnTrackCreatorCSCStrip );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMuonClusterOnTrackCreatorCSCStrip );

//Retrieve p_IMuonClusterOnTrackCreatorCSCCluster
  if ( p_IMuonClusterOnTrackCreatorCSCCluster.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMuonClusterOnTrackCreatorCSCCluster );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMuonClusterOnTrackCreatorCSCCluster );


  ATH_MSG_INFO( "Initialisation ended     " );

  return StatusCode::SUCCESS;

}

StatusCode Trk::MuonboyToTrackTool::finalize(){return StatusCode::SUCCESS;}

DataVector<const Trk::TrackStateOnSurface>* 
Trk::MuonboyToTrackTool::GiveAllTosForFullTrack(const MB_TrackAth* pMB_TrackAth,int AddCaloEntranceToMain,int AddMuonSystemEntranceToMain){

  if (pMB_TrackAth->IsValidVT() == 0 ) return 0;
  
  int IsValidMS = pMB_TrackAth->IsValidMS();
  int IsValidKA = pMB_TrackAth->IsValidKA();
  int IsValidVT = pMB_TrackAth->IsValidVT();

  DataVector<const Trk::TrackStateOnSurface>* pTrackStateOnSurfaceS = 0 ;

  if ( IsValidVT == 1 ) {
  
    const Trk::TrackStateOnSurface* pTrackStateOnSurfacePerigee = 0 ;
    
    const Trk::TrackParameters* pTrackParameters = 
      p_IMboyProperTrackRepresentationTransformTool->TrkParPerigee(pMB_TrackAth->pGetTrackAtPerigee());
      
    if ( pTrackParameters != 0) {
      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> pattern;
      pattern.set(Trk::TrackStateOnSurface::Perigee);
      pTrackStateOnSurfacePerigee = new TrackStateOnSurface(0 , pTrackParameters, 0,  0, pattern );
    }else{
      ATH_MSG_INFO( " p_IMboyProperTrackRepresentationTransformTool->TrkParPerigee Failed"  );
      return 0 ;
    }
    
    if ( pTrackStateOnSurfacePerigee == 0) {
      ATH_MSG_INFO( " Failed to convert"  );
      return 0 ;
    }
    
    if ( pTrackStateOnSurfaceS == 0 ) pTrackStateOnSurfaceS = new DataVector<const Trk::TrackStateOnSurface>;
    
//     if (m_AddPerigeeAtBeamSpot == 1){
//       const Trk::MeasuredPerigee* pMeasuredPerigeeTOBEPROPAGATED = dynamic_cast<const Trk::MeasuredPerigee*>(pPerigee);
//       if (pMeasuredPerigeeTOBEPROPAGATED) {
//         const Trk::MeasuredPerigee* pMeasuredPerigeeATBEAMSPOT = 0 ;            
//         const Trk::TrackParameters* pTrackParametersATBEAMSPOT = 
//                                     p_OwnEDMHelper->PropagateToTrackTarget( 
//                                                                            pMeasuredPerigeeTOBEPROPAGATED,
//                                                                            p_OwnEDMHelper->GivePerigeeSurfaceAtBeamSpotOrOtherwiseAtOrigin()
//                                                                           );
//         if ( pTrackParametersATBEAMSPOT != 0 ) {         
//           pMeasuredPerigeeATBEAMSPOT = dynamic_cast<const Trk::MeasuredPerigee*>(pTrackParametersATBEAMSPOT);
//           if ( pMeasuredPerigeeATBEAMSPOT != 0 ) { 
//             std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> pattern(0);
//             pattern.set(Trk::TrackStateOnSurface::Perigee);
//             const Trk::TrackStateOnSurface* pTrackStateOnSurfaceATBEAMSPOT = 
//                                             new TrackStateOnSurface(0 , pMeasuredPerigeeATBEAMSPOT, 0,  0, pattern );
//             pTrackStateOnSurfaceS->push_back( pTrackStateOnSurfaceATBEAMSPOT );
//           }else{
//             delete pTrackParametersATBEAMSPOT ;
//           }        
//         }
//       }
//     }

    pTrackStateOnSurfaceS->push_back( pTrackStateOnSurfacePerigee );
    
    
  }
  
//
  if ( IsValidKA == 1 ) {
    if (AddCaloEntranceToMain == 1){
    
      const Trk::TrackStateOnSurface* pTrackStateOnSurfaceCaloEntrance = 0 ;
   
      const Trk::TrackParameters* pTrackParameters = 
        p_IMboyProperTrackRepresentationTransformTool->TrkParMboyToAtaCylinderOrAtaDisc(pMB_TrackAth->pGetTrackAtCalorimeterEntrance()) ; 

      if ( pTrackParameters != 0) {
        std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> pattern;
        pattern.set(Trk::TrackStateOnSurface::Unknown);
        pTrackStateOnSurfaceCaloEntrance= new TrackStateOnSurface(0 , pTrackParameters, 0,  0, pattern );
      }else{
        ATH_MSG_INFO( " p_IMboyProperTrackRepresentationTransformTool->TrkParMboyToAtaCylinderOrAtaDisc Failed"  );
        delete pTrackStateOnSurfaceS;
        return 0 ;
      }

      if ( pTrackStateOnSurfaceCaloEntrance == 0) {
        ATH_MSG_INFO( " Failed to convert"  );
        return 0 ;
      }

      if ( pTrackStateOnSurfaceS == 0 ) pTrackStateOnSurfaceS = new DataVector<const Trk::TrackStateOnSurface>;
      pTrackStateOnSurfaceS->push_back( pTrackStateOnSurfaceCaloEntrance );

    }
  }

//
  int Selection = 2 ;
  AddRotToss(Selection,pTrackStateOnSurfaceS,pMB_TrackAth);

  if ( IsValidMS == 1 ) {
    if (AddMuonSystemEntranceToMain == 1){
    
      const Trk::TrackStateOnSurface* pTrackStateOnSurfaceMuonSystemEntrance = 0 ;
      
      const Trk::TrackParameters* pTrackParameters = 
        p_IMboyProperTrackRepresentationTransformTool->TrkParMboyToAtaCylinderOrAtaDisc(pMB_TrackAth->pGetTrackAtSpectrometerEntrance()) ; 

      if ( pTrackParameters != 0) {
        std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> pattern;
        pattern.set(Trk::TrackStateOnSurface::Unknown);
        pTrackStateOnSurfaceMuonSystemEntrance= new TrackStateOnSurface(0 , pTrackParameters, 0,  0, pattern );
      }else{
        ATH_MSG_INFO( " p_IMboyProperTrackRepresentationTransformTool->TrkParMboyToAtaCylinderOrAtaDisc Failed"  );
        delete pTrackStateOnSurfaceS;
        return 0 ;
      }

      if ( pTrackStateOnSurfaceMuonSystemEntrance == 0) {
        ATH_MSG_INFO( " Failed to convert"  );
        return 0 ;
      }
      
      if ( pTrackStateOnSurfaceS == 0 ) pTrackStateOnSurfaceS = new DataVector<const Trk::TrackStateOnSurface>;
      pTrackStateOnSurfaceS->push_back( pTrackStateOnSurfaceMuonSystemEntrance );
//
      if ( m_AddUnMeasuredPerigeee == 1 ){
  
        const Trk::TrackStateOnSurface* pTrackStateOnSurfaceMuonSystemEntranceAsPerigee = 0 ;
        
        const Trk::TrackParameters* pTrackParameters = 
              p_IMboyTrackRepresentationTransformTool->AtACOrAtAD2MeasuredPerigee( pTrackStateOnSurfaceMuonSystemEntrance->trackParameters() ) ;
        if ( pTrackParameters != 0) {
          std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> pattern;
          pattern.set(Trk::TrackStateOnSurface::Perigee);
          pTrackStateOnSurfaceMuonSystemEntranceAsPerigee= new TrackStateOnSurface(0 , pTrackParameters, 0,  0, pattern );
        }else{
          ATH_MSG_INFO( " p_IMboyTrackRepresentationTransformTool->AtACOrAtAD2MeasuredPerigee Failed"  );
          delete pTrackStateOnSurfaceS;
          return 0 ;
        }
        
        if ( pTrackStateOnSurfaceMuonSystemEntranceAsPerigee == 0) {
          ATH_MSG_INFO( " Failed to convert"  );
          return 0 ;
        }
        
        pTrackStateOnSurfaceS->push_back( pTrackStateOnSurfaceMuonSystemEntranceAsPerigee );
        
      }
//
      
    }
  }

//
  Selection = 3 ;
  AddRotToss(Selection,pTrackStateOnSurfaceS,pMB_TrackAth);

  return pTrackStateOnSurfaceS;

}

DataVector<const Trk::TrackStateOnSurface>* 
Trk::MuonboyToTrackTool::GiveAllTosForMuSysTrack(const MB_TrackAth* pMB_TrackAth,int AddUnMeasuredPerigeeeToMSOnly){

  if (pMB_TrackAth->IsValidMS() == 0 ) return 0;
  
  DataVector<const Trk::TrackStateOnSurface>* pTrackStateOnSurfaceS = 0 ;

//
  const Trk::TrackStateOnSurface* pTrackStateOnSurfaceMuonSystemEntrance = 0 ;

  const Trk::TrackParameters* pTrackParameters = 
    p_IMboyProperTrackRepresentationTransformTool->TrkParMboyToAtaCylinderOrAtaDisc(pMB_TrackAth->pGetTrackAtSpectrometerEntrance()) ; 

  if ( pTrackParameters != 0) {
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> pattern;
    pattern.set(Trk::TrackStateOnSurface::Unknown);
    pTrackStateOnSurfaceMuonSystemEntrance= new TrackStateOnSurface(0 , pTrackParameters, 0,  0, pattern );
  }else{
    ATH_MSG_INFO( " p_IMboyProperTrackRepresentationTransformTool->TrkParMboyToAtaCylinderOrAtaDisc Failed"  );
    return 0 ;
  }
  
  if ( pTrackStateOnSurfaceMuonSystemEntrance == 0) {
    ATH_MSG_INFO( " Failed to convert"  );
    return 0 ;
  }
  
  if( pTrackStateOnSurfaceS == 0 ) pTrackStateOnSurfaceS = new DataVector<const Trk::TrackStateOnSurface>;
  pTrackStateOnSurfaceS->push_back( pTrackStateOnSurfaceMuonSystemEntrance );

//
  if (AddUnMeasuredPerigeeeToMSOnly == 1){
  
    const Trk::TrackStateOnSurface* pTrackStateOnSurfaceMuonSystemEntranceAsPerigee = 0 ;
    
    const Trk::TrackParameters* pTrackParameters = 
          p_IMboyTrackRepresentationTransformTool->AtACOrAtAD2MeasuredPerigee( pTrackStateOnSurfaceMuonSystemEntrance->trackParameters() ) ;
    if ( pTrackParameters != 0) {
      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> pattern;
      pattern.set(Trk::TrackStateOnSurface::Perigee);
      pTrackStateOnSurfaceMuonSystemEntranceAsPerigee= new TrackStateOnSurface(0 , pTrackParameters, 0,  0, pattern );
    }else{
      ATH_MSG_INFO( " p_IMboyTrackRepresentationTransformTool->AtACOrAtAD2MeasuredPerigee Failed"  );
      delete pTrackStateOnSurfaceS;
      return 0 ;
    }
    
    if ( pTrackStateOnSurfaceMuonSystemEntranceAsPerigee == 0) {
      ATH_MSG_INFO( " Failed to convert"  );
      return 0 ;
    }
    
    pTrackStateOnSurfaceS->push_back( pTrackStateOnSurfaceMuonSystemEntranceAsPerigee );
    
  }

//
  int Selection = 3 ;
  AddRotToss(Selection,pTrackStateOnSurfaceS,pMB_TrackAth);

  return pTrackStateOnSurfaceS;

}

void  
Trk::MuonboyToTrackTool::AddRotToss(int Selection, DataVector<const Trk::TrackStateOnSurface>* pTrackStateOnSurfaceS, const MB_TrackAth* pMB_TrackAth){

    std::vector<int>  LinearIndexSorted ;
    std::vector<int>  AreaTagSorted ;
    double XrefStart = 0. ;
    double YrefStart = 0. ;
    double ZrefStart = 0. ;
    int IsValidVT = pMB_TrackAth->IsValidVT();
    int IsValidMS = pMB_TrackAth->IsValidMS();
    if ( IsValidMS == 1 ){
      const MB_Track_MboyCyl* pMB_Track_MboyCyl_MS =pMB_TrackAth->pGetTrackAtSpectrometerEntrance();
      XrefStart = pMB_Track_MboyCyl_MS->XCrossingPoint() ;
      YrefStart = pMB_Track_MboyCyl_MS->YCrossingPoint() ;
      ZrefStart = pMB_Track_MboyCyl_MS->ZCrossingPoint() ;
    }
    if ( IsValidVT == 1 ){
      const MB_Track_Perigee* pMB_Track_Perigee_VT =pMB_TrackAth->pGetTrackAtPerigee();
      XrefStart = pMB_Track_Perigee_VT->XPerigeePoint() ;
      YrefStart = pMB_Track_Perigee_VT->YPerigeePoint() ;
      ZrefStart = pMB_Track_Perigee_VT->ZPerigeePoint() ;
    }
    SortElements(
              pMB_TrackAth,
              LinearIndexSorted ,
              AreaTagSorted ,
              XrefStart,
              YrefStart,
              ZrefStart
             );
    int TotalNberOfElementsSorted = LinearIndexSorted.size(); 
    for (int iSorted=0; iSorted<TotalNberOfElementsSorted; iSorted++){
      int iDig = LinearIndexSorted[iSorted] ;
      const MbIOObject* pMbIOObject = pMB_TrackAth->LiIdGetMbIOObject(iDig);
      if (    
           ( pMbIOObject->IsTypeROT  ()&& ( pMbIOObject->IsTechMDT   ()   
	                                 || pMbIOObject->IsTechRPC   ()        
	                                 || pMbIOObject->IsTechTGC   ()       
	                                 || pMbIOObject->IsTechCSC   ()    
	                                 || pMbIOObject->IsTechCSCclu()    
                                          )
           )                              
           ||
           ( pMbIOObject->IsTypeHOLE ()&& ( pMbIOObject->IsTechMDT   ()   
	                                 || pMbIOObject->IsTechRPC   ()        
	                                 || pMbIOObject->IsTechTGC   ()       
	                                 || pMbIOObject->IsTechCSC   ()    
	                                 || pMbIOObject->IsTechCSCclu()    
                                          )
           )
           ||
           ( pMbIOObject->IsTypeMUSC ()
           )
	 ) {
        if (    
                ( pMbIOObject->IsTypeROT () && pMbIOObject->IsTechMDT   () && m_DoMdtConvertion == 1 )
             || ( pMbIOObject->IsTypeROT () && pMbIOObject->IsTechRPC   () && m_DoRpcConvertion == 1 )
             || ( pMbIOObject->IsTypeROT () && pMbIOObject->IsTechTGC   () && m_DoTgcConvertion == 1 )
             || ( pMbIOObject->IsTypeROT () && pMbIOObject->IsTechCSC   () && m_DoCscConvertion == 1 )
             || ( pMbIOObject->IsTypeROT () && pMbIOObject->IsTechCSCclu() && m_DoCscConvertion == 1 )
             || ( pMbIOObject->IsTypeHOLE() && pMbIOObject->IsTechMDT   () && m_DoMdtHole == 1 )
             || ( pMbIOObject->IsTypeHOLE() && pMbIOObject->IsTechRPC   () && m_DoRpcHole == 1 )
             || ( pMbIOObject->IsTypeHOLE() && pMbIOObject->IsTechTGC   () && m_DoTgcHole == 1 )
             || ( pMbIOObject->IsTypeHOLE() && pMbIOObject->IsTechCSC   () && m_DoCscHole == 1 )
             || ( pMbIOObject->IsTypeHOLE() && pMbIOObject->IsTechCSCclu() && m_DoCscHole == 1 )
             || ( pMbIOObject->IsTypeMUSC() && m_DoMbMuSc == 1 )
	   ) {
  
            int IsSelected = 1;
            if ( Selection !=0 )  {
              if ( Selection != AreaTagSorted[iSorted] ) IsSelected = 0 ;
            } 
            
            if ( IsSelected == 1 ){
              const Trk::TrackStateOnSurface* trackStateOnSurface = GiveTSOS(pMB_TrackAth,iDig);
  
              if (trackStateOnSurface !=0 ){
                pTrackStateOnSurfaceS->push_back(trackStateOnSurface);
                if ( m_DoDbg == 1 ) {
                  ATH_MSG_INFO( " AddRotToss TSoS Id " 
                                 << " TP  " << (&((trackStateOnSurface->trackParameters())->associatedSurface()))->associatedDetectorElementIdentifier() ) ;
                  if ( trackStateOnSurface->measurementOnTrack() )
                  ATH_MSG_INFO( " MoT " << (trackStateOnSurface->measurementOnTrack()->associatedSurface()).associatedDetectorElementIdentifier() ) ;
                }
                
              }else{
                ATH_MSG_INFO( " Conversion failed for iDig "<< iDig 
                                << " and GetType() "<< pMbIOObject->GetType() << " and GetTech() "<< pMbIOObject->GetTech() 
                                );
              }
            }
        }else{
          ATH_MSG_INFO( " Known but not allowed to be converted type for iDig "<< iDig 
                          << " and GetType() "<< pMbIOObject->GetType() << " and GetTech() "<< pMbIOObject->GetTech() 
                          );
        }
      }else{
        ATH_MSG_INFO( " Unknown type for iDig "<< iDig 
                        << " and GetType() "<< pMbIOObject->GetType() << " and GetTech() "<< pMbIOObject->GetTech() 
                        );
      }
    }

}

const Trk::TrackStateOnSurface*
Trk::MuonboyToTrackTool::GiveTSOS(const MB_TrackAth* pMB_TrackAth,int iDig){
  
  const MbIOObject* pMbIOObject = pMB_TrackAth->LiIdGetMbIOObject(iDig);
  
  if ( pMbIOObject->IsTypeROT () )  return  GiveTSOSforMbROT (pMB_TrackAth,iDig);
  
  if ( pMbIOObject->IsTypeHOLE() )  return  GiveTSOSforMbHole( pMB_TrackAth->LiIdGetMbPOT(iDig) );
  
  if ( pMbIOObject->IsTypeMUSC() )  return  p_IMbMuScToTSOSTool->GiveTSOSforMbMuSc( pMB_TrackAth->LiIdGetMbPOT(iDig) );
  
  return 0 ;

}

const Trk::TrackStateOnSurface*
Trk::MuonboyToTrackTool::GiveTSOSforMbROT(const MB_TrackAth* pMB_TrackAth,int iDig){

  const Trk::RIO_OnTrack*     pRIO_OnTrack     = 0 ;
  const Trk::TrackParameters* pTrackParameters = 0 ;

  pRIO_OnTrack  = GiveROTforMbROT(pMB_TrackAth,iDig,pTrackParameters);

  if (m_AddTrackParametersOnRotTSOS != 1 ) {
    delete pTrackParameters;
    pTrackParameters = 0 ;
  }

  if ( pRIO_OnTrack == 0) {
    delete pTrackParameters ;
    const MbIOObject* pMbIOObject = pMB_TrackAth->LiIdGetMbIOObject(iDig);
    ATH_MSG_INFO( " No pRIO_OnTrack with iDig "<< iDig 
                    << " and GetType() "<< pMbIOObject->GetType() << " and GetTech() "<< pMbIOObject->GetTech() 
                    );
    return 0 ;
  }
  
  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> pattern;
  if (pMB_TrackAth->LiIdGetMbIOObject(iDig)->IsOutlier()){
    pattern.set(Trk::TrackStateOnSurface::Outlier);
  }else{
    pattern.set(Trk::TrackStateOnSurface::Measurement);
  }

  const Trk::FitQualityOnSurface* pFitQualityOnSurface = 0;

  return  ( new TrackStateOnSurface(pRIO_OnTrack, pTrackParameters, pFitQualityOnSurface,  0 , pattern) ) ;

}

const Trk::TrackStateOnSurface*
Trk::MuonboyToTrackTool::GiveTSOSforMbHole(const MbPOT* pMbPOT){
  
  if ( pMbPOT == 0 ) {
    ATH_MSG_INFO( " GiveTSOSforMbHole called with a null pMbPOT  "
                    );
    return 0 ;
  }
  if ( !(pMbPOT->IsTypeHOLE ()) ) {
    ATH_MSG_INFO( " GiveTSOSforMbHole called for a nonHOLE pMbPOT with " 
                    << " GetType() "<< pMbPOT->GetType() << " GetTech() "<< pMbPOT->GetTech() 
                    );
    return 0 ;
  }
  
  const Trk::TrackParameters* pTrackParameters = 0 ;
  const Trk::Surface*         pSurface         = 0 ;
  p_IMuonboyToTrackParametersTool->DoTheTrackParametersCorrected( m_DoCorrectPosition , m_DoDbg ,pMbPOT,pSurface,pTrackParameters) ;

  if ( pTrackParameters == 0) {
    ATH_MSG_INFO( " No pTrackParameters for a pMbPOT with " 
                    << " GetType() "<< pMbPOT->GetType() << " GetTech() "<< pMbPOT->GetTech() 
                    );
    return 0 ;
  }
    
  if ( m_DoDbg == 1 &&  !(pMbPOT->IsTechMDT()) && pTrackParameters ) CheckAdequation(pSurface,pTrackParameters->position() );

  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> pattern;
  pattern.set(Trk::TrackStateOnSurface::Hole);

  const Trk::FitQualityOnSurface* pFitQualityOnSurface = 0;
  const Trk::MeasurementBase*     pMeasurementBase     = 0;
 
  return  ( new TrackStateOnSurface(pMeasurementBase, pTrackParameters, pFitQualityOnSurface,  0 , pattern) ) ;

}


const Trk::RIO_OnTrack*
Trk::MuonboyToTrackTool::GiveROTforMbROT(const MB_TrackAth* pMB_TrackAth,int iDig,const TrackParameters*& pTrackParameters){

  const MbPOT*        pMbPOT   = pMB_TrackAth->LiIdGetMbPOT(iDig);
  if ( pMbPOT == 0 ) {
    ATH_MSG_INFO( " GiveROTforMbROT called with a null pMbPOT  "
                    );
    return 0 ;
  }
  if ( !(pMbPOT->IsTypeROT ()) ) {
    ATH_MSG_INFO( " GiveROTforMbROT called for a nonROT pMbPOT with " 
                    << " GetType() "<< pMbPOT->GetType() << " GetTech() "<< pMbPOT->GetTech() 
                    );
    return 0 ;
  }

  const Trk::RIO_OnTrack* pRIO_OnTrack  =  0 ;
  
  if ( pMbPOT->IsTechMDT () ) {
    pRIO_OnTrack  = MdtROT(pMbPOT,pTrackParameters,pMB_TrackAth->LiIdGetSurface(iDig),pMB_TrackAth->LiIdGetMdtPrepData(iDig));
  }else{
    if ( pMbPOT->IsTechRPC () ) {
      pRIO_OnTrack  = RpcROT(pMbPOT,pTrackParameters,pMB_TrackAth->LiIdGetSurface(iDig),pMB_TrackAth->LiIdGetRpcPrepData(iDig));
    }else{
      if ( pMbPOT->IsTechTGC () ) {
        pRIO_OnTrack  = TgcROT(pMbPOT,pTrackParameters,pMB_TrackAth->LiIdGetSurface(iDig),pMB_TrackAth->LiIdGetTgcPrepData(iDig));
      }else{
        if ( pMbPOT->IsTechCSC () ) {
          pRIO_OnTrack  = CscROT(pMbPOT,pTrackParameters,pMB_TrackAth->LiIdGetSurface(iDig),pMB_TrackAth->LiIdGetCscStripPrepData(iDig));
        }else{
          if ( pMbPOT->IsTechCSCclu () ) {
            pRIO_OnTrack  = CscCluROT(pMbPOT,pTrackParameters,pMB_TrackAth->LiIdGetSurface(iDig),pMB_TrackAth->LiIdGetCscClusterPrepData(iDig));
          }else{
            ATH_MSG_INFO( " GiveROTforMbROT:  GetTech() "<< pMbPOT->GetTech() << " unknown " 
                            );
            return 0 ;
          }
        }
      }
    }
  }

//Recompute pTrackParameters using RIO surface
  if ( pRIO_OnTrack !=0){
    const Trk::Surface* pSurface = &(pRIO_OnTrack->associatedSurface()) ;
    if ( pSurface != 0 ){
      delete pTrackParameters;
      p_IMuonboyToTrackParametersTool->DoTheTrackParametersCorrected( m_DoCorrectPosition , m_DoDbg ,pMbPOT,pSurface,pTrackParameters) ;
    }
  }

  if ( m_DoDbg == 1 &&  !(pMbPOT->IsTechMDT()) && pTrackParameters ) CheckAdequation(pMB_TrackAth->LiIdGetSurface(iDig),pTrackParameters->position() );

  return pRIO_OnTrack;

}
const Trk::RIO_OnTrack* 
Trk::MuonboyToTrackTool::MdtROT(const MbPOT* pMbPOT ,const TrackParameters*& pTrackParameters,const Trk::Surface* pSurface,const Muon::MdtPrepData* pPrepData){

  pTrackParameters = 0 ;
  
  if ( pMbPOT    == 0 ) return 0 ;
  if ( pPrepData == 0 ) return 0 ;

  const MbROT*    pMbROT    = dynamic_cast<const MbROT*   > (pMbPOT); if ( pMbROT    == 0 )  return 0 ;
  const MbROTMDT* pMbROTMDT = dynamic_cast<const MbROTMDT*> (pMbROT); if ( pMbROTMDT == 0 )  return 0 ;
 
  p_IMuonboyToTrackParametersTool->DoTheTrackParametersCorrected( m_DoCorrectPosition , m_DoDbg ,pMbPOT,pSurface,pTrackParameters) ;
  
  Muon::MuonDriftCircleErrorStrategy errStrategy = p_IMdtDriftCircleOnTrackCreator->errorStrategy();
  errStrategy.setStrategy(Muon::MuonDriftCircleErrorStrategy::Muonboy);
  errStrategy.setParameter(Muon::MuonDriftCircleErrorStrategy::ParameterisedErrors, true);
  errStrategy.setParameter(Muon::MuonDriftCircleErrorStrategy::FixedError, true);
  errStrategy.setParameter(Muon::MuonDriftCircleErrorStrategy::Segment, false);
  errStrategy.setParameter(Muon::MuonDriftCircleErrorStrategy::ErrorAtPredictedPosition, true);
  // For case 2, add ScaledError.
  // See https://twiki.cern.ch/twiki/bin/view/Atlas/MuonISR#Error_Strategy for more info.
  
  ATH_MSG_DEBUG( "About to call createRIO_OnTrack with the following strategy: "<<errStrategy);
  
  const Muon::MdtDriftCircleOnTrack* pMdtDriftCircleOnTrack_const =
                                           p_IMdtDriftCircleOnTrackCreator->createRIO_OnTrack( 
                                                                                              (*pPrepData), 
                                                                                              (*pTrackParameters).position() , 
                                                                                             &(*pTrackParameters).momentum(), 
                                                                                             &errStrategy 
                                                                                             ) ;

//std::cout<<*pTrackParameters<<std::endl;
//std::cout<<*pPrepData<<std::endl;

  if (!pMdtDriftCircleOnTrack_const) return pMdtDriftCircleOnTrack_const;
  
  int m_DoSetError = 1 ;
  if ( m_DoSetError > 0 ){
    Muon::MdtDriftCircleOnTrack* pMdtDriftCircleOnTrack = const_cast<Muon::MdtDriftCircleOnTrack*>( pMdtDriftCircleOnTrack_const );
    Amg::MatrixX* pCovarianceMatrix = const_cast<Amg::MatrixX*>( &( pMdtDriftCircleOnTrack->localCovariance() ) );
    double OldValue = (*pCovarianceMatrix)(0,0) ;
    double NewValue = pMbROTMDT->GetReso() * pMbROTMDT->GetReso() ;
    (*pCovarianceMatrix)(0,0) = NewValue ;
    // fabs(sqrt(OldValue) -sqrt(NewValue))>0.001
    if  ( m_DoSetError > 1 ){
      ATH_MSG_INFO( setiosflags(std::ios::fixed) );
      ATH_MSG_INFO( " RadiusROT/RadiusPRD/RadiusParam/ROTc/MB/diff/channel "  
        << std::setw(10)<<std::setprecision(1) 
        << std::setw(10)<<pMdtDriftCircleOnTrack->driftRadius()
        << std::setw(10)<<pPrepData->localPosition()[0]
        << std::setw(10)<<pTrackParameters->localPosition()[0]
        << std::setw(10)<<sqrt(OldValue)                   * 1000.
                      << std::setw(10)<<std::setprecision(1) <<                   sqrt(NewValue)   * 1000.
                      << std::setw(10)<<std::setprecision(1) << ( sqrt(OldValue) -sqrt(NewValue) ) * 1000.
                      << " "<<m_idHelper->toString(pMdtDriftCircleOnTrack->identify())
                      );
    }
  }
  
  return pMdtDriftCircleOnTrack_const;

}

const Trk::RIO_OnTrack* 
Trk::MuonboyToTrackTool::RpcROT(const MbPOT* pMbPOT ,const TrackParameters*& pTrackParameters,const Trk::Surface* pSurface,const Muon::RpcPrepData* pPrepData){

  pTrackParameters = 0 ;
  
  if ( pMbPOT    == 0 ) return 0 ;
  if ( pPrepData == 0 ) return 0 ;
  
  p_IMuonboyToTrackParametersTool->DoTheTrackParametersCorrected( m_DoCorrectPosition , m_DoDbg ,pMbPOT,pSurface,pTrackParameters) ;

  return p_IMuonClusterOnTrackCreatorRPC->createRIO_OnTrack( (*pPrepData), pTrackParameters->position() );

}

const Trk::RIO_OnTrack* 
Trk::MuonboyToTrackTool::TgcROT(const MbPOT* pMbPOT ,const TrackParameters*& pTrackParameters,const Trk::Surface* pSurface,const Muon::TgcPrepData* pPrepData){

  pTrackParameters = 0 ;
  
  if ( pMbPOT    == 0 ) return 0 ;
  if ( pPrepData == 0 ) return 0 ;
  
  p_IMuonboyToTrackParametersTool->DoTheTrackParametersCorrected( m_DoCorrectPosition , m_DoDbg ,pMbPOT,pSurface,pTrackParameters) ;

  return p_IMuonClusterOnTrackCreatorTGC->createRIO_OnTrack( (*pPrepData), pTrackParameters->position() );

}

const Trk::RIO_OnTrack* 
Trk::MuonboyToTrackTool::CscROT(const MbPOT* pMbPOT ,const TrackParameters*& pTrackParameters,const Trk::Surface* pSurface,const Muon::CscStripPrepData* pPrepData){

  pTrackParameters = 0 ;
  
  if ( pMbPOT    == 0 ) return 0 ;
  if ( pPrepData == 0 ) return 0 ;
  
  p_IMuonboyToTrackParametersTool->DoTheTrackParametersCorrected( m_DoCorrectPosition , m_DoDbg ,pMbPOT,pSurface,pTrackParameters) ;
  
  return p_IMuonClusterOnTrackCreatorCSCStrip->createRIO_OnTrack( (*pPrepData), pTrackParameters->position() );

}

const Trk::RIO_OnTrack* 
Trk::MuonboyToTrackTool::CscCluROT(const MbPOT* pMbPOT ,const TrackParameters*& pTrackParameters,const Trk::Surface* pSurface,const Muon::CscPrepData* pPrepData){

  pTrackParameters = 0 ;
  
  if ( pMbPOT    == 0 ) return 0 ;
  if ( pPrepData == 0 ) return 0 ;
  
  p_IMuonboyToTrackParametersTool->DoTheTrackParametersCorrected( m_DoCorrectPosition , m_DoDbg ,pMbPOT,pSurface,pTrackParameters) ;
  
  return p_IMuonClusterOnTrackCreatorCSCCluster->createRIO_OnTrack( (*pPrepData), pTrackParameters->position() );

}

void Trk::MuonboyToTrackTool::CheckAdequation( const Trk::Surface* /*pSurface*/, const Amg::Vector3D& /*GP*/ ) {

  // Amg::Vector3D lpos = pSurface->transform().inverse()*GP ;

}


const Trk::FitQuality*                     
Trk::MuonboyToTrackTool::GiveFitQuality(const MB_TrackAth* pMB_TrackAth){

  return  new Trk::FitQuality(pMB_TrackAth->GetChi2(0),pMB_TrackAth->GetNberOfDOF());

}


Trk::Track* 
Trk::MuonboyToTrackTool::convert(const MB_TrackAth* pMB_TrackAth){

  DataVector<const Trk::TrackStateOnSurface>* pTrackStateOnSurfaceS = GiveAllTosForFullTrack(pMB_TrackAth,m_AddCaloEntranceToMain,m_AddMuonSystemEntranceToMain);
  if ( pTrackStateOnSurfaceS == 0) {
    if ( pMB_TrackAth->IsValidVT() != 0 ) {
      ATH_MSG_INFO( " GiveAllTosForFullTrack Failed"  );
    }
    return 0 ;
  }

  const Trk::FitQuality* fitQuality = GiveFitQuality( pMB_TrackAth );

  TrackInfo aTrackInfo(Trk::TrackInfo::Unknown, Trk::muon) ;
  aTrackInfo.setPatternRecognitionInfo( Trk::TrackInfo::Muonboy);
  
  if (m_TEMPORARYNewTrackInfoEnumUse == 1 ){
  aTrackInfo.setTrackFitter(Trk::TrackInfo::MuonboyFitter);
  if ( pMB_TrackAth->IsValidMS() == 0){
    aTrackInfo.setPatternRecognitionInfo( Trk::TrackInfo::MuonNotHittingTileVolume);
  }
  if ( pMB_TrackAth->IsToBeReversed() == 1){
    aTrackInfo.setPatternRecognitionInfo( Trk::TrackInfo::ReverseOrderedTrack);
  }
  if ( pMB_TrackAth->IsStraightLine() == 1){
    aTrackInfo.setTrackProperties( Trk::TrackInfo::StraightTrack);
  }
  }

  return new Trk::Track(aTrackInfo, pTrackStateOnSurfaceS, fitQuality);
}  

Trk::Track* 
Trk::MuonboyToTrackTool::convertAtMuonSystemEntrance(const MB_TrackAth* pMB_TrackAth){

  DataVector<const Trk::TrackStateOnSurface>* pTrackStateOnSurfaceS  = GiveAllTosForMuSysTrack(pMB_TrackAth,m_AddUnMeasuredPerigeeeToMSOnly);
  if ( pTrackStateOnSurfaceS == 0) {
    if ( pMB_TrackAth->IsValidMS() != 0 ) {
      ATH_MSG_INFO( " GiveAllTosForMuSysTrack Failed"  );
    }
    return 0 ;
  }

  const Trk::FitQuality* fitQuality = GiveFitQuality( pMB_TrackAth );

  TrackInfo aTrackInfo(Trk::TrackInfo::Unknown, Trk::muon) ;
  aTrackInfo.setPatternRecognitionInfo( Trk::TrackInfo::Muonboy);
  
  if (m_TEMPORARYNewTrackInfoEnumUse == 1 ){
  aTrackInfo.setTrackFitter(Trk::TrackInfo::MuonboyFitter);
  if ( pMB_TrackAth->IsValidMS() == 0){
    aTrackInfo.setPatternRecognitionInfo( Trk::TrackInfo::MuonNotHittingTileVolume);
  }
  if ( pMB_TrackAth->IsToBeReversed() == 1){
    aTrackInfo.setPatternRecognitionInfo( Trk::TrackInfo::ReverseOrderedTrack);
  }
  if ( pMB_TrackAth->IsStraightLine() == 1){
    aTrackInfo.setTrackProperties( Trk::TrackInfo::StraightTrack);
  }
  }

  return new Trk::Track(aTrackInfo, pTrackStateOnSurfaceS, fitQuality);
}  

StatusCode Trk::MuonboyToTrackTool::convertCollection(
                                const MB_TrackStoreAth* pMB_TrackStoreAth,
                                TrackCollection* pTrackCollection,
                                TrackCollection* pTrackCollection_MSonly
){
  
  int NberOfTracks = pMB_TrackStoreAth->NberOfMB_Track();
  for (int TrackNber=0; TrackNber<NberOfTracks; TrackNber++){

    const MB_TrackAth* pMB_TrackAth = pMB_TrackStoreAth->GetMB_TrackAth(TrackNber) ;

//     Trk::Track* TheConvertedTrack        = convert(pMB_TrackAth) ;
//     Trk::Track* TheConvertedTrack_MSonly = convertAtMuonSystemEntrance(pMB_TrackAth) ;
//  
//     if (!TheConvertedTrack  && pMB_TrackAth->IsValidVT() != 0       ) {
//       ATH_MSG_INFO( " convert failed for track  TrackNber " << TrackNber  );
//     }
//     if (!TheConvertedTrack_MSonly && pMB_TrackAth->IsValidMS() != 0 ) {
//       ATH_MSG_INFO( " convertAtMuonSystemEntrance failed for track  TrackNber " << TrackNber  );
//     }
//     
//     if (TheConvertedTrack       ) pTrackCollection->push_back(TheConvertedTrack);
//     if (TheConvertedTrack_MSonly) pTrackCollection_MSonly->push_back(TheConvertedTrack_MSonly);

    Trk::Track* TheConvertedTrack        = convert(pMB_TrackAth) ;
    if (!TheConvertedTrack  && pMB_TrackAth->IsValidVT() != 0       ) {
      ATH_MSG_INFO( " convert failed for track  TrackNber " << TrackNber  );
    }
    if (TheConvertedTrack       ) pTrackCollection->push_back(TheConvertedTrack);
    
    Trk::Track* TheConvertedTrack_MSonly = p_IMboyTrackUndressingTool->UndressTrack(TheConvertedTrack) ;
    if (!TheConvertedTrack_MSonly) TheConvertedTrack_MSonly = convertAtMuonSystemEntrance(pMB_TrackAth) ;
    if (!TheConvertedTrack_MSonly && pMB_TrackAth->IsValidMS() != 0 ) {
      ATH_MSG_INFO( " convertAtMuonSystemEntrance failed for track  TrackNber " << TrackNber  );
    }    
    if (TheConvertedTrack_MSonly) pTrackCollection_MSonly->push_back(TheConvertedTrack_MSonly);

  }


  return StatusCode::SUCCESS;

}
StatusCode Trk::MuonboyToTrackTool::convertIndividualAtIP(
                                const MB_TrackAth* pMB_TrackAth,
                                Trk::Track*& pTrack
){
  
  pTrack        = convert(pMB_TrackAth) ;

  return StatusCode::SUCCESS;

}
StatusCode Trk::MuonboyToTrackTool::convertIndividualAtMS(
                                const MB_TrackAth* pMB_TrackAth,
                                Trk::Track*& pTrack
){
  
  pTrack        = convertAtMuonSystemEntrance(pMB_TrackAth) ;

  return StatusCode::SUCCESS;

}
void Trk::MuonboyToTrackTool::SortElements(
                  const MB_TrackAth* pMB_TrackAth,
                  std::vector<int>&  LinearIndexSorted ,
                  std::vector<int>&  AreaTagSorted ,
                  double XrefStart,
                  double YrefStart,
                  double ZrefStart
){

  int TotalNberOfElements=pMB_TrackAth->GetNberOfElements(); 
  
  std::vector<double> PointXdca ;
  std::vector<double> PointYdca ;
  std::vector<double> PointZdca ;
  std::vector<int>    PointInde ;
  
  for (int Index=0; Index<TotalNberOfElements; Index++){

    const MbPOT*        pMbPOT   = pMB_TrackAth->LiIdGetMbPOT(Index);
    const Trk::Surface* pSurface = pMB_TrackAth->LiIdGetSurface(Index);
    
    if ( pMbPOT )   {
      if ( !pMbPOT->IsTypeMUSC() ) {
        const Trk::TrackParameters*   pTrackParameters  = 0;
        p_IMuonboyToTrackParametersTool->DoTheTrackParametersCorrected( m_DoCorrectPosition , m_DoDbg ,pMbPOT,pSurface,pTrackParameters);
        if (pTrackParameters){
          double Xdca = 0. ;
          double Ydca = 0. ;
          double Zdca = 0. ;
          Xdca = (pTrackParameters->position()).x() ;
          Ydca = (pTrackParameters->position()).y() ;
          Zdca = (pTrackParameters->position()).z() ;
          PointXdca.push_back(Xdca);
          PointYdca.push_back(Ydca);
          PointZdca.push_back(Zdca);
          PointInde.push_back(Index);
        }else{
          ATH_MSG_INFO( "SortElements: pTrackParameters null for Index " << Index
                         );
        }
        delete pTrackParameters;
      }else{
        PointXdca.push_back(pMbPOT->GetXdca());
        PointYdca.push_back(pMbPOT->GetYdca());
        PointZdca.push_back(pMbPOT->GetZdca());
        PointInde.push_back(Index);
      }
    }else{
      ATH_MSG_INFO( "SortElements: pMbPOT null for Index " << Index
                     );
    }
  }
  
  p_IMboySortingTool->SortPositions(
                              m_DoDbg ,
                              LinearIndexSorted ,
                              AreaTagSorted ,
                              XrefStart,
                              YrefStart,
                              ZrefStart,
                              PointXdca ,
                              PointYdca ,
                              PointZdca ,
                              PointInde  
                             ) ;
  
}



