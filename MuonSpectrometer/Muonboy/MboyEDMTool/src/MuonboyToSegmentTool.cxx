/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

//----------------------------------------------------------------//
#include "TrkEventPrimitives/ParamDefs.h"

#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "TrkEventPrimitives/LocalDirection.h"
 
#include "TrkParameters/TrackParameters.h"

#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"

#include "TrkEventPrimitives/DefinedParameter.h"
#include "TrkEventPrimitives/LocalParameters.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "MuonPrepRawData/CscStripPrepDataCollection.h"
#include "MuonPrepRawData/CscStripPrepData.h"

#include "MuonSegment/MuonSegmentQuality.h"

//#include "TrkMaterialOnTrack/ScatteringAngleOnTrack.h"

//----------------------------------------------------------------//
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"

#include "MuonRIO_OnTrack/CscClusterOnTrack.h"

//////////////////////////////////////////////////////////
#include "MboyEDMTool/MuonboyToSegmentTool.h"

//----------------------------------------------------------------//
#include "MboyEDMToolInterfaces/IMboySortingTool.h"
#include "MboyEDMToolInterfaces/IMuonboyToTrackParametersTool.h"
#include "MboyEDMToolInterfaces/IMuonboyToIdentifier.h"
#include "MboyAthEvt/MB_SegmentAth.h"
#include "MboyAthEvt/MB_SegmentStoreAth.h"
#include "MboyCoreEvt/MbPOT.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

Trk::MuonboyToSegmentTool::MuonboyToSegmentTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p)
, p_IMboySortingTool                 ( "Muon::MboySortingTool/MboySortingTool" ) 
, p_IMdtDriftCircleOnTrackCreator ( "Muon::MdtDriftCircleOnTrackCreator" ) 
, p_IMuonClusterOnTrackCreatorRPC        ( "Muon::MuonClusterOnTrackCreator"       )
, p_IMuonClusterOnTrackCreatorTGC        ( "Muon::MuonClusterOnTrackCreator"       )
, p_IMuonClusterOnTrackCreatorCSCStrip   ( "Muon::MuonClusterOnTrackCreator"       )
, p_IMuonClusterOnTrackCreatorCSCCluster ( "Muon::MuonClusterOnTrackCreator"        ) 
//, p_IMuonClusterOnTrackCreatorCSCCluster ( "Muon::CscClusterOnTrackCreator"        ) 
, p_IMuonboyToTrackParametersTool ( "Trk::MuonboyToTrackParametersTool" )
, p_IMuonboyToIdentifier          ( "MuonboyToIdentifier" ) ,
 m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool")
{

  declareInterface<IMuonboyToSegmentTool>(this);

//Declare the properties

  declareProperty("IMboySortingTool"            , p_IMboySortingTool ) ;

  declareProperty("IMdtDriftCircleOnTrackCreator" , p_IMdtDriftCircleOnTrackCreator ) ;
  declareProperty("IMuonClusterOnTrackCreatorRPC"         , p_IMuonClusterOnTrackCreatorRPC           ) ;
  declareProperty("IMuonClusterOnTrackCreatorTGC"         , p_IMuonClusterOnTrackCreatorTGC           ) ;
  declareProperty("IMuonClusterOnTrackCreatorCSCStrip"    , p_IMuonClusterOnTrackCreatorCSCStrip      ) ;
  declareProperty("IMuonClusterOnTrackCreatorCSCCluster"  , p_IMuonClusterOnTrackCreatorCSCCluster    ) ;
  declareProperty("MuonboyToTrackParametersTool"  , p_IMuonboyToTrackParametersTool ) ;
  declareProperty("IMuonboyToIdentifier"          , p_IMuonboyToIdentifier          ) ;

  declareProperty( "DoMdtConvertion"              , m_DoMdtConvertion              = 1  ) ;
  declareProperty( "DoRpcConvertion"              , m_DoRpcConvertion              = 1  ) ;
  declareProperty( "DoTgcConvertion"              , m_DoTgcConvertion              = 1  ) ;
  declareProperty( "DoCscConvertion"              , m_DoCscConvertion              = 1  ) ;
       
  declareProperty( "DoMdtHole"              , m_DoMdtHole              = 1  ) ;
  declareProperty( "DoRpcHole"              , m_DoRpcHole              = 0  ) ;
  declareProperty( "DoTgcHole"              , m_DoTgcHole              = 1  ) ;
  declareProperty( "DoCscHole"              , m_DoCscHole              = 1  ) ;

  declareProperty( "LetCSCGo"                     , m_LetCSCGo                     = 1  ) ;

  declareProperty( "ApplyQualityFactorCut"        , m_ApplyQualityFactorCut        = 1  ) ;
  declareProperty( "QualityFactorCut"             , m_QualityFactorCut             = 9. ) ;
  
  declareProperty( "ApplyNberofDigitsCut"         , m_ApplyNberofDigitsCut         = 1  ) ;
  declareProperty( "NberofDigitsCut"              , m_NberofDigitsCut              = 3  ) ;

  declareProperty( "ApplyNberofPrecisonDigitsCut" , m_ApplyNberofPrecisonDigitsCut = 1  ) ;
  declareProperty( "NberofPrecisonDigitsCut"      , m_NberofPrecisonDigitsCut      = 3  ) ;

  declareProperty( "DoCorrectPosition"            , m_DoCorrectPosition            = 1  ) ;
  
  declareProperty( "DoDbg"                        , m_DoDbg                        = 0  ) ;
  declareProperty( "MuonIdHelper" , m_idHelper);
  
  m_Clean = 1 ;

}

Trk::MuonboyToSegmentTool::~MuonboyToSegmentTool(){}

// Initialize
StatusCode Trk::MuonboyToSegmentTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" );
    return( StatusCode::FAILURE );
  }

  ATH_MSG_INFO( "================================" );
  ATH_MSG_INFO( "=Proprieties are     " );
  ATH_MSG_INFO( "= DoMdtConvertion               " << m_DoMdtConvertion );
  ATH_MSG_INFO( "= DoRpcConvertion               " << m_DoRpcConvertion );
  ATH_MSG_INFO( "= DoTgcConvertion               " << m_DoTgcConvertion );
  ATH_MSG_INFO( "= DoCscConvertion               " << m_DoCscConvertion );
  ATH_MSG_INFO( "= DoMdtHole                     " << m_DoMdtHole );
  ATH_MSG_INFO( "= DoRpcHole                     " << m_DoRpcHole );
  ATH_MSG_INFO( "= DoTgcHole                     " << m_DoTgcHole );
  ATH_MSG_INFO( "= DoCscHole                     " << m_DoCscHole );
  ATH_MSG_INFO( "= LetCSCGo                      " << m_LetCSCGo );
  ATH_MSG_INFO( "= ApplyQualityFactorCut         " << m_ApplyQualityFactorCut );
  ATH_MSG_INFO( "= QualityFactorCut              " << m_QualityFactorCut      );
  ATH_MSG_INFO( "= ApplyNberofDigitsCut          " << m_ApplyNberofDigitsCut  );
  ATH_MSG_INFO( "= NberofDigitsCut               " << m_NberofDigitsCut       );
  ATH_MSG_INFO( "= ApplyNberofPrecisonDigitsCut  " << m_ApplyNberofPrecisonDigitsCut  );
  ATH_MSG_INFO( "= NberofPrecisonDigitsCut       " << m_NberofPrecisonDigitsCut       );
  ATH_MSG_INFO( "= DoCorrectPosition             " << m_DoCorrectPosition       );
  ATH_MSG_INFO( "= DoDbg                         " << m_DoDbg       );
  ATH_MSG_INFO( "================================" );


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

//Retrieve p_IMuonboyToIdentifier
  if ( p_IMuonboyToIdentifier.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMuonboyToIdentifier );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMuonboyToIdentifier );


  ATH_MSG_INFO( "Initialisation ended     " );

  return StatusCode::SUCCESS;

}

StatusCode Trk::MuonboyToSegmentTool::finalize(){return StatusCode::SUCCESS;}

Trk::Segment* 
Trk::MuonboyToSegmentTool::convert(const MB_SegmentAth* pMB_SegmentAth){

 return convertToMuonSegment(pMB_SegmentAth);
 
}

Muon::MuonSegment* 
Trk::MuonboyToSegmentTool::convertToMuonSegment(const MB_SegmentAth* pMB_SegmentAth){

//Select segments
  if( RecSeg_IsOk(pMB_SegmentAth) == 0 ) return 0;

  // Get Segment Point (Global coordinates)
      double X_pS, Y_pS , Z_pS ;
      pMB_SegmentAth->GetPoint(X_pS, Y_pS , Z_pS);
      Amg::Translation3D pS(X_pS, Y_pS , Z_pS);

  // Get Segment Direction (Global coordinates)
      double X_dS, Y_dS , Z_dS ;
      pMB_SegmentAth->GetDirection(X_dS, Y_dS , Z_dS);
      Amg::Vector3D dS(X_dS, Y_dS , Z_dS);
      dS.normalize();

  // Get dX vector (Global coordinates)
      double X_dX, Y_dX , Z_dX ;
      pMB_SegmentAth->Getdx(X_dX, Y_dX , Z_dX);
      Amg::Vector3D dX(X_dX, Y_dX , Z_dX);
      dX.normalize();

  // Get dY vector (Global coordinates)
      double X_dY, Y_dY , Z_dY ;
      pMB_SegmentAth->Getdy(X_dY, Y_dY , Z_dY);
      Amg::Vector3D dY(X_dY, Y_dY , Z_dY);
      dY.normalize();

  // Get dZ vector (Global coordinates)
      Amg::Vector3D dZ = dX.cross(dY);

  // Get Segment Direction in local coordinates
      Amg::Vector3D LocdS(
                       dS.dot(dX),
                       dS.dot(dY),
                       dS.dot(dZ)
                       );

  // Get Angles
      double ThealphaXZ = atan2(LocdS.z(),LocdS.x());
      double ThealphaYZ = atan2(LocdS.z(),LocdS.y());
      
  // Get the set of RIO_OnTrack from the collection of PrepData
      DataVector<const Trk::MeasurementBase>* pMeasurementBaseSet = new DataVector<const Trk::MeasurementBase>;

  // Get the set of Identifies for the holes
      std::vector<Identifier> holeVec; 
      
        std::vector<int>  LinearIndexSorted ;
        std::vector<int>  AreaTagSorted ;
        double XrefStart = 0. ;
        double YrefStart = 0. ;
        double ZrefStart = 0. ;
        SortElements(
                  pMB_SegmentAth,
                  LinearIndexSorted ,
                  AreaTagSorted ,
                  XrefStart,
                  YrefStart,
                  ZrefStart
                 );
        int TotalNberOfElementsSorted = LinearIndexSorted.size(); 
        for (int iSorted=0; iSorted<TotalNberOfElementsSorted; iSorted++){
          int iDig = LinearIndexSorted[iSorted] ;
          const MbIOObject* pMbIOObject = pMB_SegmentAth->LiIdGetMbIOObject(iDig);
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
             ) {
            if (    
                    ( pMbIOObject->IsTypeROT () && pMbIOObject->IsTechMDT   () && m_DoMdtConvertion == 1 )
                 || ( pMbIOObject->IsTypeROT () && pMbIOObject->IsTechRPC   () && m_DoRpcConvertion == 1 )
                 || ( pMbIOObject->IsTypeROT () && pMbIOObject->IsTechTGC   () && m_DoTgcConvertion == 1 )
                 || ( pMbIOObject->IsTypeROT () && pMbIOObject->IsTechCSC   () && m_DoCscConvertion == 1 )
                 || ( pMbIOObject->IsTypeROT () && pMbIOObject->IsTechCSCclu() && m_DoCscConvertion == 1 )
	       ) {
              const Trk::RIO_OnTrack* pRIO_OnTrack = GiveRots(pMB_SegmentAth,iDig);
              if ( pRIO_OnTrack != 0 ) {
                pMeasurementBaseSet->push_back(pRIO_OnTrack);
              }else{
                ATH_MSG_INFO( " Conversion failed for iDig "<< iDig 
                                << " and GetType() "<< pMbIOObject->GetType() << " and GetTech() "<< pMbIOObject->GetTech() 
                                );
              }
            }else{
              if (    
                      ( pMbIOObject->IsTypeHOLE () && pMbIOObject->IsTechMDT   () && m_DoMdtHole == 1 )
                   || ( pMbIOObject->IsTypeHOLE () && pMbIOObject->IsTechRPC   () && m_DoRpcHole == 1 )
                   || ( pMbIOObject->IsTypeHOLE () && pMbIOObject->IsTechTGC   () && m_DoTgcHole == 1 )
                   || ( pMbIOObject->IsTypeHOLE () && pMbIOObject->IsTechCSC   () && m_DoCscHole == 1 )
                   || ( pMbIOObject->IsTypeHOLE () && pMbIOObject->IsTechCSCclu() && m_DoCscHole == 1 )
	         ) {
                bool isValid = false ;
                Identifier aIdentifier =p_IMuonboyToIdentifier->GetIdentifierForMbIOObject(pMbIOObject,isValid);
                if (isValid){
                  holeVec.push_back(aIdentifier);
                }else{
                  ATH_MSG_INFO( " false isValid for iDig "<< iDig 
                                  << " and GetType() "<< pMbIOObject->GetType() << " and GetTech() "<< pMbIOObject->GetTech() 
                                  );
                }
              }else{
                ATH_MSG_INFO( " Known but not allowed to be converted type for iDig "<< iDig 
                                << " and GetType() "<< pMbIOObject->GetType() << " and GetTech() "<< pMbIOObject->GetTech() 
                                );
              }
            }
          }else{
            ATH_MSG_INFO( " Unknown type for iDig "<< iDig 
                            << " and GetType() "<< pMbIOObject->GetType() << " and GetTech() "<< pMbIOObject->GetTech() 
                            );
          }
        }

  // Get Quality
      FitQuality* fitQuality = GiveFitQuality( pMB_SegmentAth,holeVec );

  // Tag the author
     Trk::Segment::Author SegmentAuthor = Trk::Segment::Muonboy;

  // The fitted local position and angles on the surface
      const Amg::Vector2D segLocPos(0.,0.);
      Trk::LocalDirection psegLocDir(ThealphaXZ,ThealphaYZ);

  // Create the local 4x4 error matrix
      AmgMatrix(4,4)* pWeightMatrix = new AmgMatrix(4,4);
//    (*pWeightMatrix)[Trk::locX][Trk::locX]       = pMB_SegmentAth->Wij(1,1);
//    (*pWeightMatrix)[Trk::alphaXZ][Trk::alphaXZ] = pMB_SegmentAth->Wij(2,2);
//    (*pWeightMatrix)[Trk::locX][Trk::alphaXZ]    = pMB_SegmentAth->Wij(1,2);
//    (*pWeightMatrix)[Trk::alphaXZ][Trk::locX]    = pMB_SegmentAth->Wij(2,1);
//    (*pWeightMatrix)[Trk::locY][Trk::locY]       = pMB_SegmentAth->Wij(3,3);
//    (*pWeightMatrix)[Trk::alphaYZ][Trk::alphaYZ] = pMB_SegmentAth->Wij(4,4);
//    (*pWeightMatrix)[Trk::locY][Trk::alphaYZ]    = pMB_SegmentAth->Wij(3,4);
//    (*pWeightMatrix)[Trk::alphaYZ][Trk::locY]    = pMB_SegmentAth->Wij(4,3);
      (*pWeightMatrix)(0,0) = pMB_SegmentAth->Wij(1,1); 
      (*pWeightMatrix)(1,1) = pMB_SegmentAth->Wij(2,2); 
      (*pWeightMatrix)(0,1) = pMB_SegmentAth->Wij(1,2); 
      (*pWeightMatrix)(1,0) = pMB_SegmentAth->Wij(2,1); 
      (*pWeightMatrix)(2,2) = pMB_SegmentAth->Wij(3,3); 
      (*pWeightMatrix)(3,3) = pMB_SegmentAth->Wij(4,4); 
      (*pWeightMatrix)(2,3) = pMB_SegmentAth->Wij(3,4); 
      (*pWeightMatrix)(3,2) = pMB_SegmentAth->Wij(4,3); 
      AmgMatrix(4,4) plocerr = (*pWeightMatrix);

  // Create the hep rotation
      Amg::RotationMatrix3D surfaceRot;
      surfaceRot.col(0) = dX;
      surfaceRot.col(1) = dY;
      surfaceRot.col(2) = dZ;

  // Create a new hep transformation
      Amg::Transform3D* surfaceTransform = new Amg::Transform3D(surfaceRot, pS);
  // Finally the surface ... put dimension in for displaying the surface
      double surfaceDimX = 5000. ;
      double surfaceDimY = 5000. ;
      Trk::PlaneSurface* psf = new Trk::PlaneSurface(surfaceTransform, surfaceDimX, surfaceDimY);

  // Conversion done
      Muon::MuonSegment* pMuonSegment = new Muon::MuonSegment(
                                                              segLocPos ,
                                                              psegLocDir,
                                                              plocerr,
                                                              psf,
                                                              pMeasurementBaseSet,
                                                              fitQuality,
                                                              SegmentAuthor
                                                             );
  // Set T0
      double FittedT0, eFittedT0;
      pMB_SegmentAth->GetFittedT0(FittedT0,eFittedT0 );
      float t0      = (float) FittedT0  ;
      float t0Error = (float) eFittedT0 ;
      if ( t0 < 9998. ) pMuonSegment->setT0Error( t0, t0Error); 
  //Alternative for CSC segments
      T0CSCSegment(pMuonSegment,t0,t0Error) ;
      pMuonSegment->setT0Error( t0, t0Error); 
                
      return pMuonSegment;

}
  
const Trk::RIO_OnTrack*
Trk::MuonboyToSegmentTool::GiveRots(const MB_SegmentAth* pMB_SegmentAth,int iDig){

  const RIO_OnTrack*          pRIO_OnTrack     = 0 ;
  const Trk::TrackParameters* pTrackParameters = 0 ;

  pRIO_OnTrack  = GiveROTforMbROT(pMB_SegmentAth,iDig,pTrackParameters);

  delete pTrackParameters;
    
  if ( pRIO_OnTrack == 0) {
    const MbIOObject* pMbIOObject = pMB_SegmentAth->LiIdGetMbIOObject(iDig);
    ATH_MSG_INFO( " No pRIO_OnTrack with iDig "<< iDig 
                    << " and GetType() "<< pMbIOObject->GetType() << " and GetTech() "<< pMbIOObject->GetTech() 
                    );
    return 0 ;
  }

  return pRIO_OnTrack;

}

const Trk::RIO_OnTrack*
Trk::MuonboyToSegmentTool::GiveROTforMbROT(const MB_SegmentAth* pMB_SegmentAth,int iDig,const TrackParameters*& pTrackParameters){

  const MbPOT*        pMbPOT   = pMB_SegmentAth->LiIdGetMbPOT(iDig);
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
    pRIO_OnTrack  = MdtROT(pMbPOT,pTrackParameters,pMB_SegmentAth->LiIdGetSurface(iDig),pMB_SegmentAth->LiIdGetMdtPrepData(iDig));
  }else{
    if ( pMbPOT->IsTechRPC () ) {
      pRIO_OnTrack  = RpcROT(pMbPOT,pTrackParameters,pMB_SegmentAth->LiIdGetSurface(iDig),pMB_SegmentAth->LiIdGetRpcPrepData(iDig));
    }else{
      if ( pMbPOT->IsTechTGC () ) {
        pRIO_OnTrack  = TgcROT(pMbPOT,pTrackParameters,pMB_SegmentAth->LiIdGetSurface(iDig),pMB_SegmentAth->LiIdGetTgcPrepData(iDig));
      }else{
        if ( pMbPOT->IsTechCSC () ) {
          pRIO_OnTrack  = CscROT(pMbPOT,pTrackParameters,pMB_SegmentAth->LiIdGetSurface(iDig),pMB_SegmentAth->LiIdGetCscStripPrepData(iDig));
        }else{
          if ( pMbPOT->IsTechCSCclu () ) {
            pRIO_OnTrack  = CscCluROT(pMbPOT,pTrackParameters,pMB_SegmentAth->LiIdGetSurface(iDig),pMB_SegmentAth->LiIdGetCscClusterPrepData(iDig));
          }else{
            ATH_MSG_INFO( " GiveROTforMbROT:  GetTech() "<< pMbPOT->GetTech() << " unknown " 
                            );
            return 0 ;
          }
        }
      }
    }
  }

  if ( m_DoDbg == 1 &&  !(pMbPOT->IsTechMDT()) && pTrackParameters ) CheckAdequation(pMB_SegmentAth->LiIdGetSurface(iDig),pTrackParameters->position() );

  return pRIO_OnTrack;

}
const Trk::RIO_OnTrack* 
Trk::MuonboyToSegmentTool::MdtROT(const MbPOT* pMbPOT ,const TrackParameters*& pTrackParameters,const Trk::Surface* pSurface,const Muon::MdtPrepData* pPrepData){

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
  errStrategy.setParameter(Muon::MuonDriftCircleErrorStrategy::Segment, true);
  errStrategy.setParameter(Muon::MuonDriftCircleErrorStrategy::ErrorAtPredictedPosition, false);

  const Muon::MdtDriftCircleOnTrack* pMdtDriftCircleOnTrack_const =
                                           p_IMdtDriftCircleOnTrackCreator->createRIO_OnTrack( 
                                                                                              (*pPrepData), 
                                                                                              (*pTrackParameters).position() , 
                                                                                             &(*pTrackParameters).momentum(), 
                                                                                             &errStrategy 
                                                                                             ) ;
  if (!pMdtDriftCircleOnTrack_const) return pMdtDriftCircleOnTrack_const;

  int m_DoSetError = 1 ;
  if ( m_DoSetError > 0 ){
    Muon::MdtDriftCircleOnTrack* pMdtDriftCircleOnTrack = const_cast<Muon::MdtDriftCircleOnTrack*>( pMdtDriftCircleOnTrack_const );
    Amg::MatrixX* pCovarianceMatrix = const_cast<Amg::MatrixX*>( &(pMdtDriftCircleOnTrack->localCovariance() ) );
    double OldValue = (*pCovarianceMatrix)(0,0) ;
    double NewValue = pMbROTMDT->GetReso() * pMbROTMDT->GetReso() ;
    (*pCovarianceMatrix)(0,0) = NewValue ;
    if ( m_DoSetError > 1 ){
      ATH_MSG_INFO( setiosflags(std::ios::fixed) );
      ATH_MSG_INFO( "Radius/RotC/Mboy/diff "  
 		<< std::setw(10)<<std::setprecision(1) 
        	<< std::setw(10)<<pMdtDriftCircleOnTrack->driftRadius()
                  << std::setw(10)<<std::setprecision(1) <<   sqrt(OldValue)                   * 1000.
                  << std::setw(10)<<std::setprecision(1) <<                   sqrt(NewValue)   * 1000.
                  << std::setw(10)<<std::setprecision(1) << ( sqrt(OldValue) -sqrt(NewValue) ) * 1000.
                  << " "<<m_idHelper->toString(pMdtDriftCircleOnTrack->identify())
                  );
    }
  }
  
  return pMdtDriftCircleOnTrack_const;
}

const Trk::RIO_OnTrack* 
Trk::MuonboyToSegmentTool::RpcROT(const MbPOT* pMbPOT ,const TrackParameters*& pTrackParameters,const Trk::Surface* pSurface,const Muon::RpcPrepData* pPrepData){

  pTrackParameters = 0 ;
  
  if ( pMbPOT    == 0 ) return 0 ;
  if ( pPrepData == 0 ) return 0 ;
  
  p_IMuonboyToTrackParametersTool->DoTheTrackParametersCorrected(m_DoCorrectPosition , m_DoDbg ,pMbPOT,pSurface,pTrackParameters);

  return p_IMuonClusterOnTrackCreatorRPC->createRIO_OnTrack( (*pPrepData), pTrackParameters->position() );

}

const Trk::RIO_OnTrack* 
Trk::MuonboyToSegmentTool::TgcROT(const MbPOT* pMbPOT ,const TrackParameters*& pTrackParameters,const Trk::Surface* pSurface,const Muon::TgcPrepData* pPrepData){

  pTrackParameters = 0 ;
  
  if ( pMbPOT    == 0 ) return 0 ;
  if ( pPrepData == 0 ) return 0 ;
  
  p_IMuonboyToTrackParametersTool->DoTheTrackParametersCorrected(m_DoCorrectPosition , m_DoDbg ,pMbPOT,pSurface,pTrackParameters);

  return p_IMuonClusterOnTrackCreatorTGC->createRIO_OnTrack( (*pPrepData), pTrackParameters->position() );

}

const Trk::RIO_OnTrack* 
Trk::MuonboyToSegmentTool::CscROT(const MbPOT* pMbPOT ,const TrackParameters*& pTrackParameters,const Trk::Surface* pSurface,const Muon::CscStripPrepData* pPrepData){

  pTrackParameters = 0 ;
  
  if ( pMbPOT    == 0 ) return 0 ;
  if ( pPrepData == 0 ) return 0 ;
  
  p_IMuonboyToTrackParametersTool->DoTheTrackParametersCorrected(m_DoCorrectPosition , m_DoDbg ,pMbPOT,pSurface,pTrackParameters);
  
  return p_IMuonClusterOnTrackCreatorCSCStrip->createRIO_OnTrack( (*pPrepData), pTrackParameters->position() );

}
const Trk::RIO_OnTrack* 
Trk::MuonboyToSegmentTool::CscCluROT(const MbPOT* pMbPOT ,const TrackParameters*& pTrackParameters,const Trk::Surface* pSurface,const Muon::CscPrepData* pPrepData){

  pTrackParameters = 0 ;
  
  if ( pMbPOT    == 0 ) return 0 ;
  if ( pPrepData == 0 ) return 0 ;
  
  p_IMuonboyToTrackParametersTool->DoTheTrackParametersCorrected(m_DoCorrectPosition , m_DoDbg ,pMbPOT,pSurface,pTrackParameters);
  
  return p_IMuonClusterOnTrackCreatorCSCCluster->createRIO_OnTrack( (*pPrepData), pTrackParameters->position() );

}

void Trk::MuonboyToSegmentTool::CheckAdequation( const Trk::Surface* /*pSurface*/, const Amg::Vector3D& /*GP*/ ) {
  
  // Amg::Vector3D lpos = pSurface->transform().inverse()*GP ;

}

Trk::FitQuality*                     
Trk::MuonboyToSegmentTool::GiveFitQuality(const MB_SegmentAth* pMB_SegmentAth, std::vector<Identifier>& holeVec){

  int NberOfDOF = 1 ;
  return  new Muon::MuonSegmentQuality(pMB_SegmentAth->GetQualityFactor(),NberOfDOF,holeVec, pMB_SegmentAth->isStrict() );

}

StatusCode Trk::MuonboyToSegmentTool::convertCollection(
                                const MB_SegmentStoreAth* pMB_SegmentStoreAth,
			        Trk::SegmentCollection* pSegmentCollection
                                 , int Clean 
){

  m_Clean = Clean ;

  int NberOfSegments = pMB_SegmentStoreAth->NberOfMB_Segment();
  for (int SegmentNber=0; SegmentNber<NberOfSegments; SegmentNber++){

    const MB_SegmentAth* pMB_SegmentAth = pMB_SegmentStoreAth->GetMB_SegmentAth(SegmentNber) ;

    Trk::Segment* pTrkSegment = convert(pMB_SegmentAth) ;

    if (!pTrkSegment)  ATH_MSG_DEBUG( " convert failed for seg  SegmentNber" << SegmentNber  );

    if ( pTrkSegment ) pSegmentCollection->push_back(pTrkSegment);

  }


  return StatusCode::SUCCESS;

}   
                      
std::vector<const Muon::MuonSegment*>* Trk::MuonboyToSegmentTool::convertCollection(
                                const MB_SegmentStoreAth* pMB_SegmentStoreAth
                                 , int Clean 
){

  m_Clean = Clean ;

  std::vector<const Muon::MuonSegment*>* pVectorOfMuonSegment = new std::vector<const Muon::MuonSegment*>() ;
  pVectorOfMuonSegment->clear();
  
  int NberOfSegments = pMB_SegmentStoreAth->NberOfMB_Segment();
  for (int SegmentNber=0; SegmentNber<NberOfSegments; SegmentNber++){

    const MB_SegmentAth* pMB_SegmentAth = pMB_SegmentStoreAth->GetMB_SegmentAth(SegmentNber) ;

    Muon::MuonSegment* pMuonSegment = convertToMuonSegment(pMB_SegmentAth) ;

    if (!pMuonSegment)  ATH_MSG_DEBUG( " convert failed for seg  SegmentNber" << SegmentNber  );

    if ( pMuonSegment ) pVectorOfMuonSegment->push_back(pMuonSegment);

  }

  return pVectorOfMuonSegment ;

}                         

int  Trk::MuonboyToSegmentTool::RecSeg_IsOk(const MB_SegmentAth* pMB_SegmentAth)
{

   if ( m_Clean == 0 )  return 1 ;

//If there is no error matrix, give up
  if ( pMB_SegmentAth->ErrorValidity() == 0) return 0 ;

  if( m_LetCSCGo==1 ){
    std::string StationName = pMB_SegmentAth->GetStationName() ;
    if ( StationName == "CSS" ) return 1;
    if ( StationName == "CSL" ) return 1;
    if ( StationName == "CSC" ) return 1;
  }

  if( m_ApplyQualityFactorCut==1 ){
    double TheQualityFactor = pMB_SegmentAth->GetQualityFactor() ;
    if (TheQualityFactor >= m_QualityFactorCut ) return 0;
  }

  if( m_ApplyNberofDigitsCut==1 ){
    int TheNberOfDig = pMB_SegmentAth->GetNberOfDigi();
    if (TheNberOfDig < m_NberofDigitsCut ) return 0;
  }

  if( m_ApplyNberofPrecisonDigitsCut==1 ){
    int TheNberOfPrecisonDig = pMB_SegmentAth->GetNberOfDigiMDT() 
    + pMB_SegmentAth->GetNberOfDigiCSCPrecise() ;
    if (TheNberOfPrecisonDig < m_NberofPrecisonDigitsCut ) return 0;
  }

  return 1;

}

void Trk::MuonboyToSegmentTool::SortElements(
                  const MB_SegmentAth* pMB_SegmentAth,
                  std::vector<int>&  LinearIndexSorted ,
                  std::vector<int>&  AreaTagSorted ,
                  double XrefStart,
                  double YrefStart,
                  double ZrefStart
){

  int TotalNberOfElements=pMB_SegmentAth->GetNberOfElements(); 

  std::vector<double> PointXdca ;
  std::vector<double> PointYdca ;
  std::vector<double> PointZdca ;
  std::vector<int>    PointInde ;
  
  for (int Index=0; Index<TotalNberOfElements; Index++){
  
    const MbPOT*        pMbPOT   = pMB_SegmentAth->LiIdGetMbPOT(Index);
    const Trk::Surface* pSurface = pMB_SegmentAth->LiIdGetSurface(Index);
    
    if ( pMbPOT )   {
      const Trk::TrackParameters*   pTrackParameters  = 0;
      p_IMuonboyToTrackParametersTool->DoTheTrackParametersCorrected(m_DoCorrectPosition , m_DoDbg ,pMbPOT,pSurface,pTrackParameters);
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
void Trk::MuonboyToSegmentTool::T0CSCSegment(const Muon::MuonSegment* pMuonSegment, float& t0, float& t0Error)
{

   t0      = pMuonSegment->time()      ;
   t0Error = pMuonSegment->errorTime() ;
   
   int   DoAlternative = 0 ;
   int   Alternative_ThereIsaCscTimeEarly  = 0 ;
   int   Alternative_ThereIsaCscTimeLate   = 0 ;
   float Alternative_LatestEarlyTime  = 0. ;
   float Alternative_EarliestLateTime = 0. ;
   float Alternative_Kounter = 0. ;
   float Alternative_t0      = 0. ;
   float Alternative_t0sqr   = 0. ;
   float Alternative_t0Error = 0. ;
   const std::vector<const Trk::RIO_OnTrack*> pRIOSet = pMuonSegment->containedROTs();
   std::vector<const Trk::RIO_OnTrack*>::const_iterator pRIO_OnTrackIter = pRIOSet.begin();
   for (; pRIO_OnTrackIter!=pRIOSet.end(); ++pRIO_OnTrackIter){
     if ( (*pRIO_OnTrackIter) ){
       const Muon::CscClusterOnTrack* pCscClusterOnTrack = dynamic_cast<const Muon::CscClusterOnTrack*>(*pRIO_OnTrackIter);
       if ( pCscClusterOnTrack ){
         DoAlternative = 1 ;
         double Time = pCscClusterOnTrack->time() ;
         Muon::CscTimeStatus TimeStatus = pCscClusterOnTrack->timeStatus() ;
//         
//Temporary: Take time/time status from prep as long as the seeting of CscClusterOnTrackCreator is not solved
       const Trk::PrepRawData* pPrepRawData =(*pRIO_OnTrackIter)->prepRawData () ;
       if ( pPrepRawData ){
         const Muon::CscPrepData* pPrepData = dynamic_cast<const Muon::CscPrepData*>(pPrepRawData);
         if ( pPrepData ){
           Time = pPrepData->time() ;
           TimeStatus = pPrepData->timeStatus() ;
         }
       }
//
         if (TimeStatus == Muon::CscTimeSuccess){
           Alternative_Kounter = Alternative_Kounter + 1. ;
           Alternative_t0    = Alternative_t0    + Time      ;
           Alternative_t0sqr = Alternative_t0sqr + Time*Time ;
         }
         if (TimeStatus == Muon::CscTimeEarly) {
           if ( Alternative_ThereIsaCscTimeEarly == 0 ) Alternative_LatestEarlyTime = Time ;
           if ( Time >= Alternative_LatestEarlyTime   ) Alternative_LatestEarlyTime = Time ; 
           Alternative_ThereIsaCscTimeEarly = 1 ;
         }
         if (TimeStatus == Muon::CscTimeLate ) {
           if ( Alternative_ThereIsaCscTimeLate == 0 ) Alternative_EarliestLateTime = Time ;
           if ( Time <= Alternative_EarliestLateTime ) Alternative_EarliestLateTime = Time ; 
           Alternative_ThereIsaCscTimeLate  = 1 ;
         }
       }
     }
   }
   
   if (DoAlternative==1){ 
     if (Alternative_Kounter){
       Alternative_t0    =  Alternative_t0    / Alternative_Kounter ;
       Alternative_t0sqr =  Alternative_t0sqr / Alternative_Kounter ;
       Alternative_t0Error =  Alternative_t0sqr - Alternative_t0*Alternative_t0 ;
       if (Alternative_t0Error < 0.){
         Alternative_t0Error = 0. ;
       }else{
         Alternative_t0Error = sqrt(Alternative_t0Error) ;
       }
     }else if (Alternative_ThereIsaCscTimeEarly == 1 && Alternative_ThereIsaCscTimeLate == 0){
       Alternative_t0      = Alternative_LatestEarlyTime       ;
       Alternative_t0Error = fabs(Alternative_LatestEarlyTime) ;
     }else if (Alternative_ThereIsaCscTimeEarly == 0 && Alternative_ThereIsaCscTimeLate == 1){
       Alternative_t0      = Alternative_EarliestLateTime ;
       Alternative_t0Error = Alternative_EarliestLateTime ;
     }else{
       Alternative_t0      = 99999. ;
       Alternative_t0Error = 99999. ;
     }
     t0      = Alternative_t0      ;
     t0Error = Alternative_t0Error ;
   }

}
