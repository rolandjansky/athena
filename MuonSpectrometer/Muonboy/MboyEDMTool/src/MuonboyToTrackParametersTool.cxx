/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

//----------------------------------------------------------------//
#include "TrkSurfaces/Surface.h"

//----------------------------------------------------------------//
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
 
#include "TrkParameters/TrackParameters.h"

#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"

/////////////////////////////////////////////////////////
#include "MboyEDMTool/MuonboyToTrackParametersTool.h"

//----------------------------------------------------------------//
#include "MboyEDMToolInterfaces/IMboyEDMHelper.h"
#include "MboyCoreEvt/MbPOT.h"
#include "MboyEDMToolInterfaces/IMuonboyToSurfaceTool.h"

//----------------------------------------------------------------//
#include "MboyCoreEvt/MbROT.h"

#include "MboyCoreEvt/MbROTMDT.h"
#include "MboyCoreEvt/MbROTRPC.h"
#include "MboyCoreEvt/MbROTTGC.h"
#include "MboyCoreEvt/MbROTCSC.h"
#include "MboyCoreEvt/MbROTCSCclu.h"

#include "MboyCoreEvt/MbHoleMDT.h"
#include "MboyCoreEvt/MbHoleRPC.h"
#include "MboyCoreEvt/MbHoleTGC.h"
#include "MboyCoreEvt/MbHoleCSC.h"
#include "MboyCoreEvt/MbHoleCSCclu.h"

#include "MboyCoreEvt/MbIDTagMDT.h"
#include "MboyCoreEvt/MbIDTagRPC.h"
#include "MboyCoreEvt/MbIDTagTGC.h"
#include "MboyCoreEvt/MbIDTagCSC.h"
#include "MboyCoreEvt/MbIDTagCSCclu.h"

Trk::MuonboyToTrackParametersTool::MuonboyToTrackParametersTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p) ,
 p_OwnEDMHelper                  ( "MboyEDMHelper" ), 
 p_IMuonboyToSurfaceTool         ( "Trk::MuonboyToSurfaceTool" ) 
{

  declareInterface<IMuonboyToTrackParametersTool>(this);

  declareProperty("MboyEDMHelper"                 , p_OwnEDMHelper          ) ;
  declareProperty("IMuonboyToSurfaceTool"         , p_IMuonboyToSurfaceTool ) ;

  declareProperty( "NullSurface" , m_NullSurface               = 0 ) ;

  declareProperty("ToolName"          , m_ToolName          = n                           ) ;

  m_DoCorrectPosition = 1 ;
 
  m_DoDbg = 0 ;

  m_DoFlip = 0 ;
 
}

Trk::MuonboyToTrackParametersTool::~MuonboyToTrackParametersTool(){}

// Initialize
StatusCode Trk::MuonboyToTrackParametersTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" );
    return( StatusCode::FAILURE );
  }

  ATH_MSG_INFO( "================================" );
  ATH_MSG_INFO( "=Proprieties are     " );
  ATH_MSG_INFO( "= ToolName                               " << m_ToolName                               ) ;
  ATH_MSG_INFO( "================================" );

//Retrieve p_OwnEDMHelper
  if ( p_OwnEDMHelper.retrieve().isFailure() ) {
    ATH_MSG_FATAL(  "Failed to retrieve tool " << p_OwnEDMHelper );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_OwnEDMHelper );

//Retrieve p_IMuonboyToSurfaceTool
  if ( p_IMuonboyToSurfaceTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL(  "Failed to retrieve tool " << p_IMuonboyToSurfaceTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMuonboyToSurfaceTool );


  ATH_MSG_INFO( "Initialisation ended     " );

  return StatusCode::SUCCESS;

}

StatusCode Trk::MuonboyToTrackParametersTool::finalize(){return StatusCode::SUCCESS;}

void Trk::MuonboyToTrackParametersTool::DoTheTrackParametersCorrected(
                             int DoCorrectPosition ,
                             int DoDbg ,
                             const MbPOT* pMbPOT                 ,
                             const Trk::Surface*& pSurface,
                             const TrackParameters*& pTrackParameters 
){

  pTrackParameters = 0;
  
  if (!pMbPOT) {
    ATH_MSG_INFO( "DoTheTrackParametersCorrected called with null pMbPOT "
                   << " => No  TrackParameters computed "
                   );
    return ;
  }

  if ( m_NullSurface != 0 ) pSurface  = 0 ;

  if (!pSurface) {
    if ( m_NullSurface == 0 ) {
      const MbROT* pMbROT = dynamic_cast<const MbROT*> (pMbPOT);
      if (pMbROT) {
        ATH_MSG_INFO( "DoTheTrackParametersCorrected called with null pSurface for a pMbPOT which is a MbROT "
                       << " for GetType() "<< pMbPOT->GetType() 
                       << " and GetTech() "<< pMbPOT->GetTech()
                       << " => No  TrackParameters computed "
                       );
        return ;
      }
    }
    pSurface = p_IMuonboyToSurfaceTool->GetSurfaceForMbIOObject(pMbPOT);
  }
  
  if (!pSurface) {
    ATH_MSG_INFO( "DoTheTrackParametersCorrected get with null pSurface "
                   << " => No  TrackParameters computed "
                   );
    return ;
  }
  
  m_DoFlip            = pMbPOT->IsToBeReversed() ;
  
  m_DoCorrectPosition = DoCorrectPosition ;
  m_DoDbg             = DoDbg             ;

  double XdcaL     ;
  double YdcaL     ;
  double ZdcaL     ;
  double XdirL     ;
  double YdirL     ;
  double ZdirL     ;
  double TheMom    ;  
  double TheCharge ;
  GetParametersofTheTrack(
                          pMbPOT                 ,
                          XdcaL  ,
                          YdcaL  ,
                          ZdcaL  ,
                          XdirL  ,
                          YdirL  ,
                          ZdirL  ,
                          TheMom ,  
                          TheCharge
                         ); 

  const Trk::StraightLineSurface* pStraightLineSurface = 0 ;
  if (pSurface) pStraightLineSurface = dynamic_cast<const Trk::StraightLineSurface*> (pSurface);
  
  if (pStraightLineSurface){
    DoTheTrackParametersCorrectedLine(
                                      pSurface,
                                      XdcaL  ,
                                      YdcaL  ,
                                      ZdcaL  ,
                                      XdirL  ,
                                      YdirL  ,
                                      ZdirL  ,
                                      TheMom ,  
                                      TheCharge  ,
                                      pTrackParameters 
                                     );
  }else{
    DoTheTrackParametersCorrectedSurface(
                                         pSurface,
                                         XdcaL  ,
                                         YdcaL  ,
                                         ZdcaL  ,
                                         XdirL  ,
                                         YdirL  ,
                                         ZdirL  ,
                                         TheMom ,  
                                         TheCharge  ,
                                         pTrackParameters 
                                        );

  }

}
void Trk::MuonboyToTrackParametersTool::DoTheTrackParametersCorrectedSurface(
                             const Trk::Surface* pSurface,
                             double XdcaL  ,
                             double YdcaL  ,
                             double ZdcaL  ,
                             double XdirL  ,
                             double YdirL  ,
                             double ZdirL  ,
                             double TheMom ,  
                             double TheCharge  ,
                             const TrackParameters*& pTrackParameters 
){

  if (m_DoCorrectPosition==1)
  CorrectParametersofTheTrackSurface(
                              pSurface,
                              XdcaL  ,
                              YdcaL  ,
                              ZdcaL  ,
                              XdirL  ,
                              YdirL  ,
                              ZdirL  ,
                              TheMom ,  
                              TheCharge 
                             );

  DoTheTrackParametersBase(
                      pSurface,
                      XdcaL  ,
                      YdcaL  ,
                      ZdcaL  ,
                      XdirL  ,
                      YdirL  ,
                      ZdirL  ,
                      TheMom ,  
                      TheCharge ,
                      pTrackParameters
                     ); 

}
void Trk::MuonboyToTrackParametersTool::DoTheTrackParametersCorrectedLine(
                             const Trk::Surface* pSurface,
                             double XdcaL  ,
                             double YdcaL  ,
                             double ZdcaL  ,
                             double XdirL  ,
                             double YdirL  ,
                             double ZdirL  ,
                             double TheMom ,  
                             double TheCharge  ,
                             const TrackParameters*& pTrackParameters 
){

  if (m_DoCorrectPosition==1)
  CorrectParametersofTheTrackLine(
                              pSurface,
                              XdcaL  ,
                              YdcaL  ,
                              ZdcaL  ,
                              XdirL  ,
                              YdirL  ,
                              ZdirL  ,
                              TheMom ,  
                              TheCharge 
                             );

  DoTheTrackParametersBase(
                       pSurface,
                       XdcaL  ,
                       YdcaL  ,
                       ZdcaL  ,
                       XdirL  ,
                       YdirL  ,
                       ZdirL  ,
                       TheMom ,  
                       TheCharge ,
                       pTrackParameters
                      ); 

}

void Trk::MuonboyToTrackParametersTool::GetParametersofTheTrack(
                             const   MbPOT* pMbPOT                 ,
                             double& XdcaL  ,
                             double& YdcaL  ,
                             double& ZdcaL  ,
                             double& XdirL  ,
                             double& YdirL  ,
                             double& ZdirL  ,
                             double& TheMom ,  
                             double& TheCharge 
){

  XdcaL = pMbPOT->GetXdca() ;
  YdcaL = pMbPOT->GetYdca() ;
  ZdcaL = pMbPOT->GetZdca() ;
  XdirL = pMbPOT->GetXdir() ;
  YdirL = pMbPOT->GetYdir() ;
  ZdirL = pMbPOT->GetZdir() ;
  TheMom= fabs(pMbPOT->GetEner()) ;  
  TheCharge = 1. ;
  if ( pMbPOT->GetEner() < 0 ) TheCharge = -1. ;
  
}

void Trk::MuonboyToTrackParametersTool::CorrectParametersofTheTrackSurface(
                             const Trk::Surface* pSurface,
                             double& XdcaL  ,
                             double& YdcaL  ,
                             double& ZdcaL  ,
                             double  XdirL  ,
                             double  YdirL  ,
                             double  ZdirL  ,
                             double& /*TheMom*/ ,  
                             double& /*TheCharge*/ 
){

  if (!pSurface) return ;
  
  Amg::Vector3D GPA( 
                          XdcaL ,
                          YdcaL ,
                          ZdcaL
                         );
  Amg::Vector3D lposA = pSurface->transform().inverse()*GPA ;
  double lposAz = lposA.z() ;

  Amg::Vector3D GPB( 
                          XdcaL + XdirL ,
                          YdcaL + YdirL ,
                          ZdcaL + ZdirL 
                         );
  Amg::Vector3D lposB = pSurface->transform().inverse()*GPB ;
  double lposBz = lposB.z() ;
  
  double lamda = lposAz /( lposAz - lposBz ) ;
  
  XdcaL = XdcaL + lamda * XdirL ;
  YdcaL = YdcaL + lamda * YdirL ;
  ZdcaL = ZdcaL + lamda * ZdirL ;

  if ( m_DoDbg == 1 ) {
    ATH_MSG_INFO( " CorrectParametersofTheTrackLine " 
                     << " lamda " << lamda 
                     );
  }

}
void Trk::MuonboyToTrackParametersTool::CorrectParametersofTheTrackLine(
                             const Trk::Surface* pSurface,
                             double& XdcaL  ,
                             double& YdcaL  ,
                             double& ZdcaL  ,
                             double  XdirL  ,
                             double  YdirL  ,
                             double  ZdirL  ,
                             double& /*TheMom*/ ,  
                             double& /*TheCharge*/ 
){

  if (!pSurface) return ;
  
  const Trk::StraightLineSurface* pStraightLineSurface = dynamic_cast<const Trk::StraightLineSurface*> (pSurface);
  if (!pStraightLineSurface) {
    ATH_MSG_INFO( " CorrectParametersofTheTrackLine called with pSurface not castable into StraightLineSurface" 
                      );
    return;
  }
  
  Amg::Vector3D  Wpt = pStraightLineSurface->center();
  Amg::Vector3D  wdi = (pStraightLineSurface->transform()).rotation().col(2) ;
  wdi.normalize();

  Amg::Vector3D  Tpt(
                  XdcaL ,
                  YdcaL ,
                  ZdcaL
                 );
  Amg::Vector3D tdi (
                   XdirL ,
                   YdirL ,
                   ZdirL
                  );
  tdi.normalize();
  
  Amg::Vector3D TW (
                   Wpt.x() - Tpt.x() ,
                   Wpt.y() - Tpt.y() ,
                   Wpt.z() - Tpt.z() 
                  );

  double lamda = TW.dot(tdi) - wdi.dot(tdi) * TW.dot(wdi);
  lamda = lamda/(1.-  wdi.dot(tdi) *  wdi.dot(tdi) );
  
  
  XdcaL = XdcaL + lamda * tdi.x() ;
  YdcaL = YdcaL + lamda * tdi.y() ;
  ZdcaL = ZdcaL + lamda * tdi.z() ;

  if ( m_DoDbg == 1 ) {
    ATH_MSG_INFO( " CorrectParametersofTheTrackLine " 
                     << " lamda " << lamda 
                     );
  }
  
}

void Trk::MuonboyToTrackParametersTool::DoTheTrackParametersBase(
                             const Trk::Surface* pSurface,
                             double XdcaL  ,
                             double YdcaL  ,
                             double ZdcaL  ,
                             double XTRAdir  ,
                             double YTRAdir  ,
                             double ZTRAdir  ,
                             double TheMom ,  
                             double TheCharge  ,
                             const  TrackParameters*& pTrackParameters 
){

//Do the GlobalPosition
    Amg::Translation3D DCApoint(
                         XdcaL ,
                         YdcaL ,
                         ZdcaL
                        );

    Amg::Vector3D aGlobalPosition( 
                                        DCApoint.x(),
                                        DCApoint.y(),
                                        DCApoint.z() 
                                       );
                                       
//Do the GlobalMomentum
    Amg::Vector3D TRAdir (
                        XTRAdir ,
                        YTRAdir ,
                        ZTRAdir
                       );
    TRAdir.normalize();
    Amg::setMag(TRAdir,TheMom);

    Amg::Vector3D aGlobalMomentum(
                                        TRAdir.x(),
                                        TRAdir.y(),
                                        TRAdir.z() 
                                       );

//Do the basic cases

  FlipAChargedMomentum(aGlobalMomentum,TheCharge);
  
  if (pSurface){
    const Trk::PlaneSurface* pPlaneSurface 
     = dynamic_cast<const Trk::PlaneSurface*> (pSurface);
    if (pPlaneSurface){
      pTrackParameters = p_OwnEDMHelper->GetSecuredAtaPlane (aGlobalPosition, aGlobalMomentum , TheCharge , *pPlaneSurface);
      return;
    }
    
    const Trk::StraightLineSurface* pStraightLineSurface 
     = dynamic_cast<const Trk::StraightLineSurface*> (pSurface);
    if (pStraightLineSurface){
      pTrackParameters =  new Trk::AtaStraightLine (aGlobalPosition, aGlobalMomentum , TheCharge , *pStraightLineSurface);
     return;
    }
    
    const Trk::CylinderSurface* pCylinderSurface 
     = dynamic_cast<const Trk::CylinderSurface*> (pSurface);
    if (pCylinderSurface){
      pTrackParameters =  new Trk::AtaCylinder (aGlobalPosition, aGlobalMomentum , TheCharge , *pCylinderSurface);
     return;
    }
    
    const Trk::DiscSurface* pDiscSurface 
     = dynamic_cast<const Trk::DiscSurface*> (pSurface);
    if (pDiscSurface){
      pTrackParameters =  new Trk::AtaDisc (aGlobalPosition, aGlobalMomentum , TheCharge , *pDiscSurface);
     return;
    }
  }

//If all failled: do the PlaneSurface
    Amg::Vector3D aHep3Vector_dZ(
                  TRAdir.x(), 
                  TRAdir.y(), 
                  TRAdir.z() 
                 );
    aHep3Vector_dZ.normalize();  
    
    double xCoord_MostPerp = 1. ;
    double yCoord_MostPerp = 0. ;
    double zCoord_MostPerp = 0. ;
    double Vmin = fabs(aHep3Vector_dZ.x()) ;
    if ( fabs(aHep3Vector_dZ.y()) < Vmin ){
      xCoord_MostPerp = 0. ;
      yCoord_MostPerp = 1. ;
      zCoord_MostPerp = 0. ;
      Vmin = fabs(aHep3Vector_dZ.y()) ;
    }
    if ( fabs(aHep3Vector_dZ.z()) < Vmin ){
      xCoord_MostPerp = 0. ;
      yCoord_MostPerp = 0. ;
      zCoord_MostPerp = 1. ;
      Vmin = fabs(aHep3Vector_dZ.z()) ;
    }
    Amg::Vector3D aHep3Vector_MostPerp(
      xCoord_MostPerp, 
      yCoord_MostPerp, 
      zCoord_MostPerp
    );

    Amg::Vector3D aHep3Vector_dX = aHep3Vector_dZ.cross(aHep3Vector_MostPerp);
    aHep3Vector_dX.normalize();

    Amg::Vector3D aHep3Vector_dY = aHep3Vector_dZ.cross(aHep3Vector_dX);
    aHep3Vector_dY.normalize();
    
    Amg::RotationMatrix3D aHepRotation;
    aHepRotation.col(0) = aHep3Vector_dX;
    aHepRotation.col(1) = aHep3Vector_dY;
    aHepRotation.col(2) = aHep3Vector_dZ;
    
    Amg::Transform3D* pTransform3D = new Amg::Transform3D(aHepRotation, DCApoint);
    
    double surfaceDimX = 5000. ;
    double surfaceDimY = 5000. ;
    Trk::PlaneSurface aPlaneSurface(pTransform3D, surfaceDimX, surfaceDimY);
    
//Do the AtaPlane
    pTrackParameters = p_OwnEDMHelper->GetSecuredAtaPlane (aGlobalPosition, aGlobalMomentum , TheCharge , aPlaneSurface);

}

void Trk::MuonboyToTrackParametersTool::FlipAChargedMomentum(Amg::Vector3D& aGlobalMomentum, double& TheCharge){
  if ( m_DoFlip != 1 ) return ;
  TheCharge = -TheCharge;
  double NEWx = -aGlobalMomentum.x();
  double NEWy = -aGlobalMomentum.y();
  double NEWz = -aGlobalMomentum.z();
  aGlobalMomentum.x() = NEWx ;
  aGlobalMomentum.y() = NEWy ;
  aGlobalMomentum.z() = NEWz ;
}
