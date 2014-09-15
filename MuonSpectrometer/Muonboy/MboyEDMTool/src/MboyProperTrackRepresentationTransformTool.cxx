/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "TrkParameters/TrackParameters.h"

#include "MboyAthEvt/MB_TrackAth.h"

/////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyProperTrackRepresentationTransformTool.h"

#include "TrkEventPrimitives/ParamDefs.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
 
#include "TrkEventPrimitives/JacobianCotThetaPtToThetaP.h"

//----------------------------------------------------------------//

namespace Muon {

MboyProperTrackRepresentationTransformTool::MboyProperTrackRepresentationTransformTool(const std::string& t, 
                                             const std::string& n,
                                             const IInterface*  p ):AthAlgTool(t,n,p)
{

  declareInterface<IMboyProperTrackRepresentationTransformTool>(this);

//Declare the properties

  declareProperty("ToolName"          , m_ToolName          = n                           ) ;

}

MboyProperTrackRepresentationTransformTool::~MboyProperTrackRepresentationTransformTool(){}

// Initialize
StatusCode MboyProperTrackRepresentationTransformTool::initialize(){

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

  ATH_MSG_INFO(  "Initialisation ended     " );

  return StatusCode::SUCCESS;

}

StatusCode MboyProperTrackRepresentationTransformTool::finalize(){return StatusCode::SUCCESS;}

MB_Track_SpecEntr* 
MboyProperTrackRepresentationTransformTool::AtaCylinderOrAtaDiscToTrkParMboy(const Trk::TrackParameters* pTrackParameters)
{
  
  if (!pTrackParameters) {
    ATH_MSG_DEBUG( " AtaCylinderOrAtaDiscToTrkParMboy returns 0 since pTrackParameters is 0 ") ;
    return 0 ;
  }

  MB_Track_SpecEntr* pMB_Track_SpecEntr = 0 ;
   
  pMB_Track_SpecEntr = AtaCylinderToTrkParMboy(pTrackParameters);
  if (pMB_Track_SpecEntr) return pMB_Track_SpecEntr ; 

  pMB_Track_SpecEntr = AtaDiscToTrkParMboy(pTrackParameters);
  if (pMB_Track_SpecEntr) return pMB_Track_SpecEntr ; 

  ATH_MSG_DEBUG( " AtaCylinderOrAtaDiscToTrkParMboy returns 0 since both AtaCylinderToTrkParMboy and  AtaDiscToTrkParMboy return 0 ") ;
  return 0 ;
  
} 

const Trk::TrackParameters* 
MboyProperTrackRepresentationTransformTool::TrkParMboyToAtaCylinderOrAtaDisc(const MB_Track_MboyCyl* pMB_Track_MboyCyl)
{

  if (!pMB_Track_MboyCyl) {
    ATH_MSG_DEBUG( " TrkParMboyToAtaCylinderOrAtaDisc returns 0 since pMB_Track_MboyCyl is 0 ") ;
    return 0 ;
  }

  const Trk::TrackParameters* pTrackParameters = 0 ;

  pTrackParameters = TrkParMboyToAtaCylinder(pMB_Track_MboyCyl);
  if (pTrackParameters) return pTrackParameters ; 

  pTrackParameters = TrkParMboyToAtaDisc(pMB_Track_MboyCyl);
  if (pTrackParameters) return pTrackParameters ; 
  
  ATH_MSG_DEBUG( " TrkParMboyToAtaCylinderOrAtaDisc returns 0 since both TrkParMboyToAtaCylinder and  TrkParMboyToAtaDisc return 0 ") ;
  return 0 ;
  
}

const Trk::TrackParameters* 
MboyProperTrackRepresentationTransformTool::TrkParPerigee(const MB_Track_Perigee* pMB_Track_Perigee) {
  
  if (!pMB_Track_Perigee) {
    ATH_MSG_DEBUG( " TrkParPerigee returns 0 since pMB_Track_Perigee is 0 ") ;
    return 0 ;
  }

  double P[5];
  P[0] =-pMB_Track_Perigee->ImpactParameterAtPerigee()  ;
  P[1] = pMB_Track_Perigee->ZAtPerigee()		     ;
  P[2] = pMB_Track_Perigee->PhiAtPerigee() 	     ;
  P[3] = pMB_Track_Perigee->CotgAtPerigee()	     ;
  P[4] = pMB_Track_Perigee->InverseTransverseMomentum() ;  

  const float pi = 3.1415927, pi2=2.*pi; 
  if(P[2]>pi) P[2]-=pi2; else if(P[2]<-pi) P[2]+=pi2;

  //Go to new perigee representation!!
  double theta    = atan2(1.,P[3]); 
  double qp       = P[4] * sin(theta) ;  
  P[3] = theta ;
  P[4] = qp ;
  double cotTheta = pMB_Track_Perigee->CotgAtPerigee() ;
  double qpT      = pMB_Track_Perigee->InverseTransverseMomentum() ; 
  Trk::JacobianCotThetaPtToThetaP TheJac(cotTheta, qpT);
  AmgSymMatrix(5)* OldRepresentationMatrix = new AmgSymMatrix(5);
  (*OldRepresentationMatrix)(0,0) = pMB_Track_Perigee->Covij(1,1);
  (*OldRepresentationMatrix)(1,0) =-pMB_Track_Perigee->Covij(2,1);
  (*OldRepresentationMatrix)(1,1) = pMB_Track_Perigee->Covij(2,2);
  (*OldRepresentationMatrix)(2,0) =-pMB_Track_Perigee->Covij(3,1);
  (*OldRepresentationMatrix)(2,1) = pMB_Track_Perigee->Covij(3,2);
  (*OldRepresentationMatrix)(2,2) = pMB_Track_Perigee->Covij(3,3);
  (*OldRepresentationMatrix)(3,0) =-pMB_Track_Perigee->Covij(4,1);
  (*OldRepresentationMatrix)(3,1) = pMB_Track_Perigee->Covij(4,2);
  (*OldRepresentationMatrix)(3,2) = pMB_Track_Perigee->Covij(4,3);
  (*OldRepresentationMatrix)(3,3) = pMB_Track_Perigee->Covij(4,4);
  (*OldRepresentationMatrix)(4,0) =-pMB_Track_Perigee->Covij(5,1);
  (*OldRepresentationMatrix)(4,1) = pMB_Track_Perigee->Covij(5,2);
  (*OldRepresentationMatrix)(4,2) = pMB_Track_Perigee->Covij(5,3);
  (*OldRepresentationMatrix)(4,3) = pMB_Track_Perigee->Covij(5,4);
  (*OldRepresentationMatrix)(4,4) = pMB_Track_Perigee->Covij(5,5);

  AmgSymMatrix(5) OldRepresentationMatrixJaced = OldRepresentationMatrix->similarity(TheJac) ;
  
  double Thed0    = P[0] ;
  double Thez0    = P[1] ;
  double ThePhi0  = P[2] ;
  double TheTheta = P[3] ;
  double TheqOnp  = P[4] ; 

  if ( pMB_Track_Perigee->IsToBeReversed() == 1){
    FlipMeasuredPerigee(
                    Thed0      ,
                    Thez0      ,
                    ThePhi0    ,
                    TheTheta   ,
                    TheqOnp    ,
                    OldRepresentationMatrixJaced
                   );
  }

  AmgSymMatrix(5)* covMatrix = new AmgSymMatrix(5)( OldRepresentationMatrixJaced );
  const Trk::PerigeeSurface perigeeSurface;

  const Trk::Perigee* perigee   = new Trk::Perigee(
                                       Thed0      ,
                                       Thez0      ,
                                       ThePhi0    ,
                                       TheTheta   ,
                                       TheqOnp    ,
                                       perigeeSurface,
                                       covMatrix
                                       );

  delete OldRepresentationMatrix ;
  return perigee;

}

const Trk::TrackParameters* 
MboyProperTrackRepresentationTransformTool::TrkParMboyToAtaCylinder(
  const MB_Track_MboyCyl* pMB_Track_MboyCyl
) {

  if (!pMB_Track_MboyCyl) {
    ATH_MSG_DEBUG( " TrkParMboyToAtaCylinder returns 0 since pMB_Track_MboyCyl is 0 ") ;
    return 0 ;
  }

  if (IsOnTheCylinder(pMB_Track_MboyCyl) != 1) return 0 ;
  int IsToBeReversed = pMB_Track_MboyCyl->IsToBeReversed();

  double XCrossing = pMB_Track_MboyCyl->XCrossingPoint()   ;
  double YCrossing = pMB_Track_MboyCyl->YCrossingPoint()   ;
  double ZCrossing = pMB_Track_MboyCyl->ZCrossingPoint()   ;
  double RCrossing = sqrt ( XCrossing*XCrossing + YCrossing*YCrossing );

  double ThetaX =  pMB_Track_MboyCyl->ThetaOfCrossingPoint() ;
  double PhiX   =  pMB_Track_MboyCyl->PhiOfCrossingPoint()   ;
  double ThetaV =  pMB_Track_MboyCyl->ThetaOfUnitVector()    ;
  double PhiV   =  pMB_Track_MboyCyl->PhiOfUnitVector()      ;
  double qOnp   =  pMB_Track_MboyCyl->InverseMomentum()      ;

  const float pi = 3.1415927, pi2=2.*pi; 
  if(PhiX>pi) PhiX-=pi2; else if(PhiX<-pi) PhiX+=pi2;
  if(PhiV>pi) PhiV-=pi2; else if(PhiV<-pi) PhiV+=pi2;

  //Go to new cylinder representation!!
  double TheLocRPhi = RCrossing*PhiX; 
  double TheLocZ    = RCrossing/tan(ThetaX) ;
  double ThePhi     = PhiV ;
  double TheTheta   = ThetaV ;
  double TheqOnp    = qOnp ;

  AmgSymMatrix(5)* OldRepresentationMatrix = new AmgSymMatrix(5);
  for (int i=1 ; i <=5; i++){ for (int j=1 ; j<=i; j++){ (*OldRepresentationMatrix)(i-1,j-1) = pMB_Track_MboyCyl->Covij(i,j); } }

  AmgMatrix(5,5) TheJac;
  TheJac.setZero();
  
  Jacobian_MB_Track_MboyCyl_To_AtaCylinder( 
                         &TheJac  , 
                         RCrossing   ,
                         TheLocRPhi  , 
                         TheLocZ     , 
                         ThePhi      , 
                         TheTheta    , 
                         TheqOnp     ,
                         ThetaX      , 
                         PhiX        , 
                         ThetaV      , 
                         PhiV        , 
                         qOnp   
                        );
  AmgSymMatrix(5) OldRepresentationMatrixJaced = OldRepresentationMatrix->similarity(TheJac) ;
  
  if ( IsToBeReversed == 1){
    FlipMeasuredAtaCylinder(
                     TheLocRPhi ,
                     TheLocZ    ,
                     ThePhi     ,
                     TheTheta   ,
                     TheqOnp    ,
                     OldRepresentationMatrixJaced
                    );
  }
      
  AmgSymMatrix(5)* covMatrix = new AmgSymMatrix(5)( OldRepresentationMatrixJaced );

  double CylRadius     = RCrossing ;
  double CylHalfLength = 2. * fabs( ZCrossing );
  const Trk::CylinderSurface cylinder(CylRadius, CylHalfLength);

  const Trk::AtaCylinder* pMeasuredAtaCylinder   = new Trk::AtaCylinder(
                                                                   TheLocRPhi ,
                                                                   TheLocZ    ,
                                                                   ThePhi     ,
                                                                   TheTheta   ,
                                                                   TheqOnp    ,
                                                                   cylinder ,
                                                                   covMatrix
                                                                   );

  delete OldRepresentationMatrix ;
  return pMeasuredAtaCylinder;

}

const Trk::TrackParameters* 
MboyProperTrackRepresentationTransformTool::TrkParMboyToAtaDisc(
  const MB_Track_MboyCyl* pMB_Track_MboyCyl
) {

  if (!pMB_Track_MboyCyl) {
    ATH_MSG_DEBUG( " TrkParMboyToAtaDisc returns 0 since pMB_Track_MboyCyl is 0 ") ;
    return 0 ;
  }

  if (IsOnTheCylinder(pMB_Track_MboyCyl) == 1) return 0 ;
  int IsToBeReversed = pMB_Track_MboyCyl->IsToBeReversed();

  double XCrossing = pMB_Track_MboyCyl->XCrossingPoint()   ;
  double YCrossing = pMB_Track_MboyCyl->YCrossingPoint()   ;
  double ZCrossing = pMB_Track_MboyCyl->ZCrossingPoint()   ;
  double RCrossing = sqrt ( XCrossing*XCrossing + YCrossing*YCrossing );

  double ThetaX = pMB_Track_MboyCyl->ThetaOfCrossingPoint() ;
  double PhiX   = pMB_Track_MboyCyl->PhiOfCrossingPoint()   ;
  double ThetaV = pMB_Track_MboyCyl->ThetaOfUnitVector()    ;
  double PhiV   = pMB_Track_MboyCyl->PhiOfUnitVector()      ;
  double qOnp   = pMB_Track_MboyCyl->InverseMomentum()      ;

  const float pi = 3.1415927, pi2=2.*pi; 
  if(PhiX>pi) PhiX-=pi2; else if(PhiX<-pi) PhiX+=pi2;
  if(PhiV>pi) PhiV-=pi2; else if(PhiV<-pi) PhiV+=pi2;

  //Go to new cylinder representation!!
  double TheLocR   = ZCrossing *tan(ThetaX) ; 
  double TheLocPhi = PhiX ;
  double ThePhi    = PhiV ;
  double TheTheta  = ThetaV ;
  double TheqOnp   = qOnp ;

  AmgSymMatrix(5)* OldRepresentationMatrix = new AmgSymMatrix(5);
  for (int i=1 ; i <=5; i++){ for (int j=1 ; j<=i; j++){ (*OldRepresentationMatrix)(i-1,j-1) = pMB_Track_MboyCyl->Covij(i,j); } }

  AmgMatrix(5,5) TheJac;
  TheJac.setZero();
  
  Jacobian_MB_Track_MboyCyl_To_AtaDisc( 
                         &TheJac  , 
                         ZCrossing   ,
                         TheLocR     , 
                         TheLocPhi   , 
                         ThePhi      , 
                         TheTheta    , 
                         TheqOnp     ,
                         ThetaX      , 
                         PhiX        , 
                         ThetaV      , 
                         PhiV        , 
                         qOnp   
                        );

  AmgSymMatrix(5) OldRepresentationMatrixJaced = OldRepresentationMatrix->similarity(TheJac) ;
  
  if ( IsToBeReversed == 1){
    FlipMeasuredAtaDisc(
                 TheLocR   ,
                 TheLocPhi ,
                 ThePhi    ,
                 TheTheta  ,
                 TheqOnp   ,
                 OldRepresentationMatrixJaced
                );
  }
              
  AmgSymMatrix(5)* covMatrix = new AmgSymMatrix(5)( OldRepresentationMatrixJaced );

  Amg::Translation3D discPosition(0.,0.,ZCrossing);
  Amg::Transform3D* transform = new Amg::Transform3D(discPosition * Amg::RotationMatrix3D::Identity());
  double innerRadius      = 0.;
  double outerRadius      = 2. * RCrossing;
  Trk::DiscSurface disc(transform, innerRadius, outerRadius);

  const Trk::AtaDisc* pMeasuredAtaDisc   = new Trk::AtaDisc(
                                                                   TheLocR   ,
                                                                   TheLocPhi ,
                                                                   ThePhi    ,
                                                                   TheTheta  ,
                                                                   TheqOnp   ,
                                                                   disc ,
                                                                   covMatrix
                                                                   );

  delete OldRepresentationMatrix ;
  return pMeasuredAtaDisc;

}

MB_Track_SpecEntr* MboyProperTrackRepresentationTransformTool:: AtaDiscToTrkParMboy    (const Trk::TrackParameters* pTrackParameters)
{

  const Trk::AtaDisc* pMeasuredAtaDisc = dynamic_cast<const Trk::AtaDisc*>(pTrackParameters);
  if (!pMeasuredAtaDisc) return 0 ;
  
  double AtAD_TheLocR   = pMeasuredAtaDisc->parameters()[Trk::locR]    ;
  double AtAD_TheLocPhi = pMeasuredAtaDisc->parameters()[Trk::locPhi]  ;
  double AtAD_ThePhi    = pMeasuredAtaDisc->parameters()[Trk::phi]     ;
  double AtAD_TheTheta  = pMeasuredAtaDisc->parameters()[Trk::theta]   ;
  double AtAD_TheqOnp   = pMeasuredAtaDisc->parameters()[Trk::qOverP]  ;

  double XCrossing = (pMeasuredAtaDisc->position()).x()   ;
  double YCrossing = (pMeasuredAtaDisc->position()).y()   ;
  double ZCrossing = (pMeasuredAtaDisc->position()).z()   ;
  double RCrossing = sqrt ( XCrossing*XCrossing + YCrossing*YCrossing );

  Amg::Vector3D aGlobalMomentum = pMeasuredAtaDisc->momentum () ;
  double Momentum = aGlobalMomentum.mag() ;
  double XUnit = aGlobalMomentum.x()/Momentum   ;
  double YUnit = aGlobalMomentum.y()/Momentum   ;
  double ZUnit = aGlobalMomentum.z()/Momentum   ;

  double ThetaX = atan2(RCrossing,ZCrossing);
  double PhiX   = atan2(YCrossing,XCrossing)	 ;
  double ThetaV = AtAD_TheTheta	 ;
  double PhiV   = AtAD_ThePhi	 ;
  double qOnp   = AtAD_TheqOnp    ;  
  
  AmgMatrix(5,5) TheJac;
  TheJac.setZero();

  Jacobian_AtaDisc_To_MB_Track_MboyCyl( 
                         &TheJac  , 
                         ZCrossing   ,
                         AtAD_TheLocR     , 
                         AtAD_TheLocPhi   , 
                         AtAD_ThePhi      , 
                         AtAD_TheTheta    , 
                         AtAD_TheqOnp     ,
                         ThetaX      , 
                         PhiX        , 
                         ThetaV      , 
                         PhiV        , 
                         qOnp   
                        );

  AmgSymMatrix(5) TheCovarianceMatrix = pMeasuredAtaDisc->covariance()->similarity(TheJac);

//Create a MB_Track_SpecEntr
  MB_Track_SpecEntr*  pMB_Track_SpecEntr = new  MB_Track_SpecEntr() ;

  double Zhalfcylind = fabs( 2.*ZCrossing ) ;
  double Rcylind     = RCrossing            ;

  pMB_Track_SpecEntr->SetTrackSpecEntr(
     ThetaX ,
     PhiX   ,
     ThetaV ,
     PhiV   ,
     qOnp   ,
     TheCovarianceMatrix(0,0),
     TheCovarianceMatrix(1,0),TheCovarianceMatrix(1,1),
     TheCovarianceMatrix(2,0),TheCovarianceMatrix(2,1),TheCovarianceMatrix(2,2),
     TheCovarianceMatrix(3,0),TheCovarianceMatrix(3,1),TheCovarianceMatrix(3,2),TheCovarianceMatrix(3,3),
     TheCovarianceMatrix(4,0),TheCovarianceMatrix(4,1),TheCovarianceMatrix(4,2),TheCovarianceMatrix(4,3),TheCovarianceMatrix(4,4),
     XCrossing ,
     YCrossing ,
     ZCrossing ,
     XUnit ,
     YUnit ,
     ZUnit ,
     Momentum,
     Zhalfcylind,  Rcylind);

  return pMB_Track_SpecEntr ;
}


MB_Track_SpecEntr* MboyProperTrackRepresentationTransformTool::AtaCylinderToTrkParMboy(const Trk::TrackParameters* pTrackParameters)
{

  const Trk::AtaCylinder* pMeasuredAtaCylinder = dynamic_cast<const Trk::AtaCylinder*>(pTrackParameters);
  if (!pMeasuredAtaCylinder) return 0 ;

  double AtAC_TheLocRPhi = pMeasuredAtaCylinder->parameters()[Trk::locRPhi]  ;
  double AtAC_TheLocZ    = pMeasuredAtaCylinder->parameters()[Trk::locZ]     ;
  double AtAC_ThePhi     = pMeasuredAtaCylinder->parameters()[Trk::phi]      ;
  double AtAC_TheTheta   = pMeasuredAtaCylinder->parameters()[Trk::theta]    ;
  double AtAC_TheqOnp    = pMeasuredAtaCylinder->parameters()[Trk::qOverP]   ;
  
  double XCrossing = (pMeasuredAtaCylinder->position()).x()   ;
  double YCrossing = (pMeasuredAtaCylinder->position()).y()   ;
  double ZCrossing = (pMeasuredAtaCylinder->position()).z()   ;
  double RCrossing = sqrt ( XCrossing*XCrossing + YCrossing*YCrossing );

  Amg::Vector3D aGlobalMomentum = pMeasuredAtaCylinder->momentum () ;
  double Momentum = aGlobalMomentum.mag() ;
  double XUnit = aGlobalMomentum.x()/Momentum   ;
  double YUnit = aGlobalMomentum.y()/Momentum   ;
  double ZUnit = aGlobalMomentum.z()/Momentum   ;
  
  double ThetaX = atan2(RCrossing,ZCrossing);
  double PhiX   = atan2(YCrossing,XCrossing)	 ;
  double ThetaV = AtAC_TheTheta 	 ;
  double PhiV   = AtAC_ThePhi	 ;
  double qOnp   = AtAC_TheqOnp      ;  

  AmgMatrix(5,5) TheJac;
  TheJac.setZero();
  
  Jacobian_AtaCylinder_To_MB_Track_MboyCyl( 
                         &TheJac  , 
                         RCrossing   ,
                         AtAC_TheLocRPhi  , 
                         AtAC_TheLocZ     , 
                         AtAC_ThePhi      , 
                         AtAC_TheTheta    , 
                         AtAC_TheqOnp     ,
                         ThetaX      , 
                         PhiX        , 
                         ThetaV      , 
                         PhiV        , 
                         qOnp   
                        );

  AmgSymMatrix(5) TheCovarianceMatrix = pMeasuredAtaCylinder->covariance()->similarity(TheJac);

//Create a MB_Track_SpecEntr
  MB_Track_SpecEntr*  pMB_Track_SpecEntr = new  MB_Track_SpecEntr() ;

  double Zhalfcylind = fabs( 2.*ZCrossing ) ;
  double Rcylind     = RCrossing            ;

  pMB_Track_SpecEntr->SetTrackSpecEntr(
     ThetaX ,
     PhiX   ,
     ThetaV ,
     PhiV   ,
     qOnp   ,
     TheCovarianceMatrix(0,0),
     TheCovarianceMatrix(1,0),TheCovarianceMatrix(1,1),
     TheCovarianceMatrix(2,0),TheCovarianceMatrix(2,1),TheCovarianceMatrix(2,2),
     TheCovarianceMatrix(3,0),TheCovarianceMatrix(3,1),TheCovarianceMatrix(3,2),TheCovarianceMatrix(3,3),
     TheCovarianceMatrix(4,0),TheCovarianceMatrix(4,1),TheCovarianceMatrix(4,2),TheCovarianceMatrix(4,3),TheCovarianceMatrix(4,4),
     XCrossing ,
     YCrossing ,
     ZCrossing ,
     XUnit ,
     YUnit ,
     ZUnit ,
     Momentum,
     Zhalfcylind,  Rcylind);

  return pMB_Track_SpecEntr ;
}

int
MboyProperTrackRepresentationTransformTool::IsOnTheCylinder(const MB_Track_MboyCyl* pMB_Track_MboyCyl) {

  if (!pMB_Track_MboyCyl) {
    ATH_MSG_DEBUG( " IsOnTheCylinder returns 0 since pMB_Track_MboyCyl is 0 ") ;
    return 0 ;
  }

  double RCylinder = pMB_Track_MboyCyl->RadiusOfCylinder() ;
  double XCrossing = pMB_Track_MboyCyl->XCrossingPoint()   ;
  double YCrossing = pMB_Track_MboyCyl->YCrossingPoint()   ;
//double ZCrossing = pMB_Track_MboyCyl->ZCrossingPoint()   ;
  double RCrossing = sqrt ( XCrossing*XCrossing + YCrossing*YCrossing );
  double VachementPetit = 0.100 ;
  if ( fabs(RCrossing - RCylinder) > VachementPetit ) return 0 ;
  return 1 ;

}
void 
MboyProperTrackRepresentationTransformTool::Jacobian_MB_Track_MboyCyl_To_AtaCylinder(
                                                 AmgMatrix(5,5)* pTheJac  ,
                                                 double& RCrossing   ,
                                                 double& /* TheLocRPhi */ ,
                                                 double& /* TheLocZ    */ ,
                                                 double& /* TThePhi    */ ,
                                                 double& /* TheTheta   */ ,
                                                 double& /* TheqOnp    */ ,
                                                 double& ThetaX           ,
                                                 double& /* PhiX       */ ,
                                                 double& /* ThetaV     */ ,
                                                 double& /* PhiV       */ ,
                                                 double& /* qOnp       */
){

  (*pTheJac)(1,2) =  RCrossing ;
  (*pTheJac)(2,1) = -RCrossing/std::pow( sin(ThetaX) , 2 ) ;
  (*pTheJac)(3,4) = 1. ;
  (*pTheJac)(4,3) = 1. ;
  (*pTheJac)(5,5) = 1. ;

}

void 
MboyProperTrackRepresentationTransformTool::Jacobian_AtaCylinder_To_MB_Track_MboyCyl(
                                                 AmgMatrix(5,5)* pTheJac  ,
                                                 double& RCrossing   ,
                                                 double& /* TheLocRPhi */ ,
                                                 double& /* TheLocZ    */ ,
                                                 double& /* TThePhi    */ ,
                                                 double& /* TheTheta   */ ,
                                                 double& /* TheqOnp    */ ,
                                                 double& ThetaX           ,
                                                 double& /* PhiX       */ ,
                                                 double& /* ThetaV     */ ,
                                                 double& /* PhiV       */ ,
                                                 double& /* qOnp       */
){

  (*pTheJac)(1,2) = -std::pow( sin(ThetaX) , 2 )/RCrossing  ;
  (*pTheJac)(2,1) = 1./RCrossing ;
  (*pTheJac)(3,4) = 1. ;
  (*pTheJac)(4,3) = 1. ;
  (*pTheJac)(5,5) = 1. ;

}

void 
MboyProperTrackRepresentationTransformTool::Jacobian_MB_Track_MboyCyl_To_AtaDisc(
                                                 AmgMatrix(5,5)* pTheJac       ,
                                                 double& ZCrossing        ,
                                                 double& /* TheLocR    */ ,
                                                 double& /* TheLocPhi  */ ,
                                                 double& /* ThePhi     */ ,
                                                 double& /* TheTheta   */ ,
                                                 double& /* TheqOnp    */ ,
                                                 double& ThetaX           ,
                                                 double& /* PhiX       */ ,
                                                 double& /* ThetaV     */ ,
                                                 double& /* PhiV       */ ,
                                                 double& /* qOnp       */
){

  (*pTheJac)(1,1) = ZCrossing/std::pow( cos(ThetaX) , 2 );
  (*pTheJac)(2,2) = 1. ;
  (*pTheJac)(3,4) = 1. ;
  (*pTheJac)(4,3) = 1. ;
  (*pTheJac)(5,5) = 1. ;

}
void 
MboyProperTrackRepresentationTransformTool::Jacobian_AtaDisc_To_MB_Track_MboyCyl(
                                                 AmgMatrix(5,5)* pTheJac       ,
                                                 double& ZCrossing        ,
                                                 double& /* TheLocR    */ ,
                                                 double& /* TheLocPhi  */ ,
                                                 double& /* ThePhi     */ ,
                                                 double& /* TheTheta   */ ,
                                                 double& /* TheqOnp    */ ,
                                                 double& ThetaX           ,
                                                 double& /* PhiX       */ ,
                                                 double& /* ThetaV     */ ,
                                                 double& /* PhiV       */ ,
                                                 double& /* qOnp       */
){

  (*pTheJac)(1,1) = std::pow( cos(ThetaX) , 2 )/ZCrossing;
  (*pTheJac)(2,2) = 1. ;
  (*pTheJac)(3,4) = 1. ;
  (*pTheJac)(4,3) = 1. ;
  (*pTheJac)(5,5) = 1. ;

}
void MboyProperTrackRepresentationTransformTool::FlipMeasuredPerigee(
                        double&  Thed0    ,
                        double&  /*Thez0*/    ,
                        double&  ThePhi0  ,
                        double&  TheTheta ,
                        double&  TheqOnp  ,
                        AmgSymMatrix(5)& TheHepSymMatrix
){

  Trk::ParamDefs ParamDefsArray[5]={ Trk::d0 , Trk::z0 , Trk::phi0 , Trk::theta , Trk::qOverP };
  double         SignArray[5]     ={ -1.     ,  1.     ,  1.       , -1.        , -1.         };

  for (int Ind1=0; Ind1<5; Ind1++){    
    Trk::ParamDefs Par1 = ParamDefsArray[Ind1] ;
    for (int Ind2=Ind1; Ind2<5; Ind2++){
      Trk::ParamDefs Par2 = ParamDefsArray[Ind2] ;
      double NEWvalue = SignArray[Ind1] * SignArray[Ind2] * TheHepSymMatrix(Par1,Par2) ;
      TheHepSymMatrix(Par1,Par2) = NEWvalue ;
    }
  }
  
  Thed0    = -Thed0           ;
  ThePhi0  =  ThePhi0 + M_PI  ;
  TheTheta =  M_PI - TheTheta ;
  TheqOnp  = -TheqOnp         ;
  
  if(ThePhi0>M_PI) ThePhi0 -= 2.*M_PI; else if(ThePhi0<-M_PI) ThePhi0 +=2.*M_PI;

}

void MboyProperTrackRepresentationTransformTool::FlipMeasuredAtaDisc(
                        double&  /*TheLocR*/   ,
                        double&  /*TheLocPhi*/ ,
                        double&  ThePhi    ,
                        double&  TheTheta  ,
                        double&  TheqOnp   ,
                        AmgSymMatrix(5)& TheHepSymMatrix
){

  Trk::ParamDefs ParamDefsArray[5]={ Trk::locR , Trk::locPhi , Trk::phi , Trk::theta , Trk::qOverP };
  double         SignArray[5]     ={  1.       ,  1.         ,  1.      , -1.        , -1.         };

  for (int Ind1=0; Ind1<5; Ind1++){    
    Trk::ParamDefs Par1 = ParamDefsArray[Ind1] ;
    for (int Ind2=Ind1; Ind2<5; Ind2++){
      Trk::ParamDefs Par2 = ParamDefsArray[Ind2] ;
      double NEWvalue = SignArray[Ind1] * SignArray[Ind2] * TheHepSymMatrix(Par1,Par2) ;
      TheHepSymMatrix(Par1,Par2) = NEWvalue ;
    }
  }
  
  ThePhi    =  ThePhi + M_PI   ;
  TheTheta  =  M_PI - TheTheta ;
  TheqOnp   = -TheqOnp         ;
  
  if(ThePhi>M_PI) ThePhi -= 2.*M_PI; else if(ThePhi<-M_PI) ThePhi +=2.*M_PI;
  
}

void MboyProperTrackRepresentationTransformTool::FlipMeasuredAtaCylinder(
                        double&  /*TheLocRPhi*/ ,
                        double&  /*TheLocZ*/    ,
                        double&  ThePhi     ,
                        double&  TheTheta   ,
                        double&  TheqOnp    ,
                        AmgSymMatrix(5)& TheHepSymMatrix
){

  Trk::ParamDefs ParamDefsArray[5]={ Trk::locRPhi , Trk::locZ , Trk::phi , Trk::theta , Trk::qOverP };
  double         SignArray[5]     ={  1.          ,  1.       ,  1.      , -1.        , -1.         };

  for (int Ind1=0; Ind1<5; Ind1++){    
    Trk::ParamDefs Par1 = ParamDefsArray[Ind1] ;
    for (int Ind2=Ind1; Ind2<5; Ind2++){
      Trk::ParamDefs Par2 = ParamDefsArray[Ind2] ;
      double NEWvalue = SignArray[Ind1] * SignArray[Ind2] * TheHepSymMatrix(Par1,Par2) ;
      TheHepSymMatrix(Par1,Par2) = NEWvalue ;
    }
  }
  
  ThePhi     =  ThePhi + M_PI   ;
  TheTheta   =  M_PI - TheTheta ;
  TheqOnp    = -TheqOnp         ;
  
  if(ThePhi>M_PI) ThePhi -= 2.*M_PI; else if(ThePhi<-M_PI) ThePhi +=2.*M_PI;
  
}


}
