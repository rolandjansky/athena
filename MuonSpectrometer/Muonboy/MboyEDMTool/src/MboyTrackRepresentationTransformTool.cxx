/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "EventPrimitives/EventPrimitives.h"

/////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyTrackRepresentationTransformTool.h"

#include "TrkEventPrimitives/ParamDefs.h"
 
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkTrack/TrackInfo.h"

#include "MboyEDMToolInterfaces/IMboyDetectorLimitsTool.h"
//----------------------------------------------------------------//

namespace Muon {

MboyTrackRepresentationTransformTool::MboyTrackRepresentationTransformTool(const std::string& t, 
                                             const std::string& n,
                                             const IInterface*  p ):AthAlgTool(t,n,p)
, p_IMboyDetectorLimitsTool          ( "Muon::MboyDetectorLimitsTool/MboyDetectorLimitsTool" )
{

  declareInterface<IMboyTrackRepresentationTransformTool>(this);

//Declare the properties

  declareProperty("IMboyDetectorLimitsTool"    , p_IMboyDetectorLimitsTool ) ;

  declareProperty("ToolName"          , m_ToolName          = n                           ) ;

}

MboyTrackRepresentationTransformTool::~MboyTrackRepresentationTransformTool(){}

// Initialize
StatusCode MboyTrackRepresentationTransformTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL(  " AthAlgTool::initialize() failed" );
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO( "================================" );
  ATH_MSG_INFO( "=Proprieties are     " );
  ATH_MSG_INFO( "= ToolName                               " << m_ToolName                               ) ;
  ATH_MSG_INFO( "================================" );

//Retrieve p_IMboyDetectorLimitsTool
  if ( p_IMboyDetectorLimitsTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMboyDetectorLimitsTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMboyDetectorLimitsTool );

  ATH_MSG_INFO(  "Initialisation ended     " );

  return StatusCode::SUCCESS;

}

StatusCode MboyTrackRepresentationTransformTool::finalize(){return StatusCode::SUCCESS;}


const Trk::TrackParameters* MboyTrackRepresentationTransformTool::AtACOrAtAD2MeasuredPerigee(const Trk::TrackParameters* pTrackParameters)
{
 
  if (!pTrackParameters) return 0 ;
  
  const Trk::AtaCylinder* pAtaCylinder = dynamic_cast<const Trk::AtaCylinder*>(pTrackParameters);
  if ( pAtaCylinder ) {
    const Trk::AtaCylinder* pMeasuredAtaCylinder = dynamic_cast<const Trk::AtaCylinder*>(pAtaCylinder);
    if ( pMeasuredAtaCylinder ){
      return MeasuredAtaCylinder2MeasuredPerigee(pMeasuredAtaCylinder);
    }
  }
  
  const Trk::AtaDisc* pAtaDisc = dynamic_cast<const Trk::AtaDisc*>(pTrackParameters);
  if ( pAtaDisc ) {
    const Trk::AtaDisc* pMeasuredAtaDisc = dynamic_cast<const Trk::AtaDisc*>(pAtaDisc);
    if ( pMeasuredAtaDisc ){
      return MeasuredAtaDisc2MeasuredPerigee(pMeasuredAtaDisc) ;
    }
  }

  return 0;
  
}
const Trk::TrackParameters* MboyTrackRepresentationTransformTool::MeasuredPerigee2AtACOrAtAD(const Trk::TrackParameters* pTrackParameters)
{

  if (!pTrackParameters) return 0 ;

  const Trk::Perigee* pMeasuredPerigee = dynamic_cast<const Trk::Perigee*>(pTrackParameters);
  if (!pMeasuredPerigee) return 0 ;
  
  if (p_IMboyDetectorLimitsTool->IsOnCylinder(pMeasuredPerigee))
    return MeasuredPerigee2MeasuredAtaCylinder(pMeasuredPerigee);
   
  return MeasuredPerigee2MeasuredAtaDisc(pMeasuredPerigee);

}


const Trk::Perigee* MboyTrackRepresentationTransformTool::MeasuredAtaCylinder2MeasuredPerigee(const Trk::AtaCylinder* pMeasuredAtaCylinder)
{
 
  if (!pMeasuredAtaCylinder) return 0 ;
  
  double AtAC_TheLocRPhi = pMeasuredAtaCylinder->parameters()[Trk::locRPhi]  ;
  double AtAC_TheLocZ    = pMeasuredAtaCylinder->parameters()[Trk::locZ]     ;
  double AtAC_ThePhi     = pMeasuredAtaCylinder->parameters()[Trk::phi]      ;
  double AtAC_TheTheta   = pMeasuredAtaCylinder->parameters()[Trk::theta]    ;
  double AtAC_TheqOnp    = pMeasuredAtaCylinder->parameters()[Trk::qOverP]   ;
  
  double XCrossing = (pMeasuredAtaCylinder->position()).x()   ;
  double YCrossing = (pMeasuredAtaCylinder->position()).y()   ;
  double RCrossing = sqrt ( XCrossing*XCrossing + YCrossing*YCrossing );
  Amg::Vector3D TheVertex(XCrossing,YCrossing,0.);
  
  double Peri_Thed0    = 0.              ; 
  double Peri_Thez0    = AtAC_TheLocZ    ;
  double Peri_ThePhi0  = AtAC_ThePhi     ;
  double Peri_TheTheta = AtAC_TheTheta   ;
  double Peri_TheqOnp  = AtAC_TheqOnp    ;
  
  AmgMatrix(5,5) TheJac;
  TheJac.setZero();

  Jacobian_AtaCylinder_To_Perigee(
                                  &TheJac  ,
                                  RCrossing   ,
                                  Peri_Thed0       ,
                                  Peri_Thez0     ,
                                  Peri_ThePhi0   ,
                                  Peri_TheTheta  ,
                                  Peri_TheqOnp   ,
                                  AtAC_TheLocRPhi    ,
                                  AtAC_TheLocZ     ,
                                  AtAC_ThePhi        ,
                                  AtAC_TheTheta      ,
                                  AtAC_TheqOnp      
                                 );  
                                 
  AmgSymMatrix(5) TheCovarianceMatrix = pMeasuredAtaCylinder->covariance()->similarity(TheJac);
  AmgSymMatrix(5)* covMatrix = new AmgSymMatrix(5)( TheCovarianceMatrix );
  
  return new Trk::Perigee(
                        Peri_Thed0      ,
                        Peri_Thez0      ,
                        Peri_ThePhi0    ,
                        Peri_TheTheta   ,
                        Peri_TheqOnp    ,
                        TheVertex  ,
                        covMatrix
                       );
}
void 
MboyTrackRepresentationTransformTool::Jacobian_AtaCylinder_To_Perigee(
                                                 AmgMatrix(5,5)* pTheJac  ,
                                                 double& RCrossing   ,
                                                 double&    Peri_Thed0       ,
                                                 double& /* Peri_Thez0    */ ,
                                                 double& /* Peri_ThePhi0  */ ,
                                                 double& /* Peri_TheTheta */ ,
                                                 double& /* Peri_TheqOnp  */ ,
                                                 double&    AtAC_TheLocRPhi    ,
                                                 double& /* AtAC_TheLocZ    */ ,
                                                 double&    AtAC_ThePhi        ,
                                                 double&    AtAC_TheTheta      ,
                                                 double& /* AtAC_TheqOnp    */  
){

   double PhiX   = AtAC_TheLocRPhi / RCrossing ;
   double PhiV   = AtAC_ThePhi ;
   double ThetaV = AtAC_TheTheta ;
    
   (*pTheJac)(1,1) =  cos(PhiV-PhiX)                                 ;
   (*pTheJac)(2,1) = -sin(PhiV-PhiX)   /        tan(ThetaV)          ;
   (*pTheJac)(2,2) =  1.                                             ;
   (*pTheJac)(2,3) = -Peri_Thed0       /        tan(ThetaV)          ;
   (*pTheJac)(3,3) =  1.                                             ;
   (*pTheJac)(4,4) =  1.                                             ;
   (*pTheJac)(5,5) =  1.                                             ;

}

const Trk::AtaCylinder* MboyTrackRepresentationTransformTool::MeasuredPerigee2MeasuredAtaCylinder(const Trk::Perigee* pMeasuredPerigee)
{
 
  if (!pMeasuredPerigee) return 0 ;
  
  double Peri_Thed0    = pMeasuredPerigee->parameters()[Trk::d0]       ;
  double Peri_Thez0    = pMeasuredPerigee->parameters()[Trk::z0]       ;
  double Peri_ThePhi0  = pMeasuredPerigee->parameters()[Trk::phi]      ;
  double Peri_TheTheta = pMeasuredPerigee->parameters()[Trk::theta]    ;
  double Peri_TheqOnp  = pMeasuredPerigee->parameters()[Trk::qOverP]   ;

  double XCrossing = (pMeasuredPerigee->position()).x()   ;
  double YCrossing = (pMeasuredPerigee->position()).y()   ;
  double ZCrossing = (pMeasuredPerigee->position()).z()   ;
  double RCrossing = sqrt ( XCrossing*XCrossing + YCrossing*YCrossing );

  double AtAC_TheLocRPhi = atan2(YCrossing,XCrossing)  * RCrossing ;
  double AtAC_TheLocZ    = Peri_Thez0     ;
  double AtAC_ThePhi     = Peri_ThePhi0      ;
  double AtAC_TheTheta   = Peri_TheTheta    ;
  double AtAC_TheqOnp    = Peri_TheqOnp   ;
  
  AmgMatrix(5,5) TheJac;
  TheJac.setZero();

  Jacobian_Perigee_To_AtaCylinder(
                                  &TheJac  ,
                                  RCrossing   ,
                                  Peri_Thed0       ,
                                  Peri_Thez0     ,
                                  Peri_ThePhi0   ,
                                  Peri_TheTheta  ,
                                  Peri_TheqOnp   ,
                                  AtAC_TheLocRPhi    ,
                                  AtAC_TheLocZ     ,
                                  AtAC_ThePhi        ,
                                  AtAC_TheTheta      ,
                                  AtAC_TheqOnp      
                                 );  
                                 
  AmgSymMatrix(5) TheCovarianceMatrix = pMeasuredPerigee->covariance()->similarity(TheJac);
  AmgSymMatrix(5)* covMatrix = new AmgSymMatrix(5)( TheCovarianceMatrix );
  
  double CylRadius     = RCrossing ;
  double CylHalfLength = 2. * fabs( ZCrossing );
  Trk::CylinderSurface cylinder(new Amg::Transform3D, CylRadius, CylHalfLength);

  return new Trk::AtaCylinder(
                                      AtAC_TheLocRPhi ,
                                      AtAC_TheLocZ    ,
                                      AtAC_ThePhi     ,
                                      AtAC_TheTheta   ,
                                      AtAC_TheqOnp    ,
                                      cylinder ,
                                      covMatrix
                                     );

}

void 
MboyTrackRepresentationTransformTool::Jacobian_Perigee_To_AtaCylinder(
                                                 AmgMatrix(5,5)* pTheJac  ,
                                                 double& RCrossing   ,
                                                 double&    Peri_Thed0       ,
                                                 double& /* Peri_Thez0    */ ,
                                                 double& /* Peri_ThePhi0  */ ,
                                                 double& /* Peri_TheTheta */ ,
                                                 double& /* Peri_TheqOnp  */ ,
                                                 double&    AtAC_TheLocRPhi    ,
                                                 double& /* AtAC_TheLocZ    */ ,
                                                 double&    AtAC_ThePhi        ,
                                                 double&    AtAC_TheTheta      ,
                                                 double& /* AtAC_TheqOnp    */  
){

   double PhiX   = AtAC_TheLocRPhi / RCrossing ;
   double PhiV   = AtAC_ThePhi ;
   double ThetaV = AtAC_TheTheta ;
    
   (*pTheJac)(1,1) =  1./cos(PhiV-PhiX)                              ;
   (*pTheJac)(2,1) =  tan(PhiV-PhiX)   /        tan(ThetaV)          ;
   (*pTheJac)(2,2) =  1.                                             ;
   (*pTheJac)(2,3) =  Peri_Thed0       /        tan(ThetaV)          ;
   (*pTheJac)(3,3) =  1.                                             ;
   (*pTheJac)(4,4) =  1.                                             ;
   (*pTheJac)(5,5) =  1.                                             ;

}

const Trk::Perigee* MboyTrackRepresentationTransformTool::MeasuredAtaDisc2MeasuredPerigee(const Trk::AtaDisc* pMeasuredAtaDisc)
{
 
  if (!pMeasuredAtaDisc) return 0 ;
  
  double AtAD_TheLocR   = pMeasuredAtaDisc->parameters()[Trk::locR]    ;
  double AtAD_TheLocPhi = pMeasuredAtaDisc->parameters()[Trk::locPhi]  ;
  double AtAD_ThePhi    = pMeasuredAtaDisc->parameters()[Trk::phi]     ;
  double AtAD_TheTheta  = pMeasuredAtaDisc->parameters()[Trk::theta]   ;
  double AtAD_TheqOnp   = pMeasuredAtaDisc->parameters()[Trk::qOverP]  ;
  
  double XCrossing = (pMeasuredAtaDisc->position()).x()   ;
  double YCrossing = (pMeasuredAtaDisc->position()).y()   ;
  double ZCrossing = (pMeasuredAtaDisc->position()).z()   ;
  Amg::Vector3D TheVertex(XCrossing,YCrossing,0.);

  double Peri_Thed0    = 0.         ; 
  double Peri_Thez0    = ZCrossing    ;
  double Peri_ThePhi0  = AtAD_ThePhi     ;
  double Peri_TheTheta = AtAD_TheTheta   ;
  double Peri_TheqOnp  = AtAD_TheqOnp    ;

  AmgMatrix(5,5) TheJac;
  TheJac.setZero();

  Jacobian_AtaDisc_To_Perigee(
                              &TheJac  ,
                              ZCrossing ,
                              Peri_Thed0       ,
                              Peri_Thez0     ,
                              Peri_ThePhi0   ,
                              Peri_TheTheta  ,
                              Peri_TheqOnp   ,
                              AtAD_TheLocR       ,
                              AtAD_TheLocPhi     ,
                              AtAD_ThePhi        ,
                              AtAD_TheTheta      ,
                              AtAD_TheqOnp     
                                 );  

  AmgSymMatrix(5) TheCovarianceMatrix = pMeasuredAtaDisc->covariance()->similarity(TheJac);
  AmgSymMatrix(5)* covMatrix = new AmgSymMatrix(5)( TheCovarianceMatrix );

  return new Trk::Perigee(
                        Peri_Thed0      ,
                        Peri_Thez0      ,
                        Peri_ThePhi0    ,
                        Peri_TheTheta   ,
                        Peri_TheqOnp    ,
                        TheVertex  ,
                        covMatrix
                       );

}
void 
MboyTrackRepresentationTransformTool::Jacobian_AtaDisc_To_Perigee(
                                                 AmgMatrix(5,5)* pTheJac  ,
                                                 double& /* ZCrossing */,
                                                 double&    Peri_Thed0       ,
                                                 double& /* Peri_Thez0    */ ,
                                                 double& /* Peri_ThePhi0  */ ,
                                                 double& /* Peri_TheTheta */ ,
                                                 double& /* Peri_TheqOnp  */ ,
                                                 double&    AtAD_TheLocR       ,
                                                 double&    AtAD_TheLocPhi     ,
                                                 double&    AtAD_ThePhi        ,
                                                 double&    AtAD_TheTheta      ,
                                                 double& /* AtAD_TheqOnp    */ 
){
   double PhiX   = AtAD_TheLocPhi;
   double PhiV   = AtAD_ThePhi ;
   double ThetaV = AtAD_TheTheta ;
//Nb: TheLocR   = ZCrossing *tan(ThetaX)

   (*pTheJac)(1,1) =                 sin(PhiX-PhiV)               ;
   (*pTheJac)(1,2) =  AtAD_TheLocR * cos(PhiX-PhiV)               ;
   (*pTheJac)(2,1) = -               cos(PhiX-PhiV) / tan(ThetaV) ;
   (*pTheJac)(2,2) =  AtAD_TheLocR * sin(PhiX-PhiV) / tan(ThetaV) ;
   (*pTheJac)(2,3) = -Peri_Thed0                    / tan(ThetaV) ;
   (*pTheJac)(3,3) =  1.                                          ;
   (*pTheJac)(4,4) =  1.                                          ;
   (*pTheJac)(5,5) =  1.                                          ;

}
const Trk::AtaDisc* MboyTrackRepresentationTransformTool::MeasuredPerigee2MeasuredAtaDisc(const Trk::Perigee* pMeasuredPerigee)
{
 
  if (!pMeasuredPerigee) return 0 ;
  
  double Peri_Thed0    = pMeasuredPerigee->parameters()[Trk::d0]       ;
  double Peri_Thez0    = pMeasuredPerigee->parameters()[Trk::z0]       ;
  double Peri_ThePhi0  = pMeasuredPerigee->parameters()[Trk::phi]      ;
  double Peri_TheTheta = pMeasuredPerigee->parameters()[Trk::theta]    ;
  double Peri_TheqOnp  = pMeasuredPerigee->parameters()[Trk::qOverP]   ;

  double XCrossing = (pMeasuredPerigee->position()).x()   ;
  double YCrossing = (pMeasuredPerigee->position()).y()   ;
  double ZCrossing = (pMeasuredPerigee->position()).z()   ;
  double RCrossing = sqrt ( XCrossing*XCrossing + YCrossing*YCrossing );

  double AtAD_TheLocR   = RCrossing    ;
  double AtAD_TheLocPhi = atan2(YCrossing,XCrossing) ;
  double AtAD_ThePhi    = Peri_ThePhi0   ;
  double AtAD_TheTheta  = Peri_TheTheta  ;
  double AtAD_TheqOnp   = Peri_TheqOnp   ;
  
  AmgMatrix(5,5) TheJac;
  TheJac.setZero();

  Jacobian_Perigee_To_AtaDisc(
                              &TheJac  ,
                              ZCrossing ,
                              Peri_Thed0       ,
                              Peri_Thez0     ,
                              Peri_ThePhi0   ,
                              Peri_TheTheta  ,
                              Peri_TheqOnp   ,
                              AtAD_TheLocR       ,
                              AtAD_TheLocPhi     ,
                              AtAD_ThePhi        ,
                              AtAD_TheTheta      ,
                              AtAD_TheqOnp     
                                 );  

  Amg::Translation3D discPosition(0.,0.,ZCrossing);
  Amg::Transform3D* transform = new Amg::Transform3D(discPosition * Amg::RotationMatrix3D::Identity());
  double innerRadius      = 0.;
  double outerRadius      = 2. * RCrossing;
  Trk::DiscSurface disc(transform, innerRadius, outerRadius);

  AmgSymMatrix(5) TheCovarianceMatrix;
  TheCovarianceMatrix = pMeasuredPerigee->covariance()->similarity(TheJac);
  AmgSymMatrix(5)* covMatrix = new AmgSymMatrix(5)( TheCovarianceMatrix );

  return new Trk::AtaDisc(
                                  AtAD_TheLocR   ,
                                  AtAD_TheLocPhi ,
                                  AtAD_ThePhi    ,
                                  AtAD_TheTheta  ,
                                  AtAD_TheqOnp   ,
                                  disc ,
                                  covMatrix
                                 );

}
void 
MboyTrackRepresentationTransformTool::Jacobian_Perigee_To_AtaDisc(
                                                 AmgMatrix(5,5)* pTheJac  ,
                                                 double& /* ZCrossing */,
                                                 double&    Peri_Thed0       ,
                                                 double& /* Peri_Thez0    */ ,
                                                 double& /* Peri_ThePhi0  */ ,
                                                 double& /* Peri_TheTheta */ ,
                                                 double& /* Peri_TheqOnp  */ ,
                                                 double&    AtAD_TheLocR       ,
                                                 double&    AtAD_TheLocPhi     ,
                                                 double&    AtAD_ThePhi        ,
                                                 double&    AtAD_TheTheta      ,
                                                 double& /* AtAD_TheqOnp    */  
){
   double PhiX   = AtAD_TheLocPhi;
   double PhiV   = AtAD_ThePhi ;
   double ThetaV = AtAD_TheTheta ;
//Nb: TheLocR   = ZCrossing *tan(ThetaX)

   (*pTheJac)(1,1) =                            sin(PhiX-PhiV)                 ;
   (*pTheJac)(1,2) = -          tan(ThetaV)   * cos(PhiX-PhiV)                 ;
   (*pTheJac)(1,3) = -Peri_Thed0              * cos(PhiX-PhiV)                 ;
   (*pTheJac)(2,1) =                            cos(PhiX-PhiV) / AtAD_TheLocR  ;
   (*pTheJac)(2,2) =            tan(ThetaV)   * sin(PhiX-PhiV) / AtAD_TheLocR  ;
   (*pTheJac)(2,3) =  Peri_Thed0              * sin(PhiX-PhiV) / AtAD_TheLocR  ;
   (*pTheJac)(3,3) =  1.                                                       ;
   (*pTheJac)(4,4) =  1.                                                       ;
   (*pTheJac)(5,5) =  1.                                                       ;

}

}
