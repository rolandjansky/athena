/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

//----------------------------------------------------------------//
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkParameters/TrackParameters.h"

#include "TrkGeometry/HomogeneousLayerMaterial.h"
//#include "TrkGeometry/SimplifiedMaterialProperties.h"

#include "GeoPrimitives/GeoPrimitivesHelpers.h"

//----------------------------------------------------------------//
#include "TrkGeometry/PlaneLayer.h"

/////////////////////////////////////////////////////////
#include "MboyEDMTool/MuonboyToLayer.h"

//----------------------------------------------------------------//
#include "MboyCoreEvt/MbMuSc.h"

MuonboyToLayer::MuonboyToLayer(const std::string& t, 
                                             const std::string& n,
                                             const IInterface*  p ):AthAlgTool(t,n,p)
{

  declareInterface<IMuonboyToLayer>(this);

}

MuonboyToLayer::~MuonboyToLayer(){}

// Initialize
StatusCode MuonboyToLayer::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL(  " AthAlgTool::initialize() failed" );
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO(  "Initialisation ended     " );

  return StatusCode::SUCCESS;

}

StatusCode MuonboyToLayer::finalize(){return StatusCode::SUCCESS;}

std::pair<const Trk::TrackParameters*,const Trk::Layer* > MuonboyToLayer::ToLayer(
                            const MbMuSc* pMbMuSc,
			    double sign
){

//   double X0MASC_V   = pMbMuSc->GetX0MASC()   ;
   double ALMASC_V   = pMbMuSc->GetALMASC()   ;
   double BLMASC_V   = pMbMuSc->GetBLMASC()   ;
   double XposMASC_V = pMbMuSc->GetXposMASC() ;
   double YposMASC_V = pMbMuSc->GetYposMASC() ;
   double ZposMASC_V = pMbMuSc->GetZposMASC() ;
   double XdirMASC_V = pMbMuSc->GetXdirMASC() ;
   double YdirMASC_V = pMbMuSc->GetYdirMASC() ;
   double ZdirMASC_V = pMbMuSc->GetZdirMASC() ;
   double PmomMASC_V = pMbMuSc->GetPmomMASC() ;
   //std::cout << "mom: " << PmomMASC_V << " 1/mom: " << 1./PmomMASC_V << std::endl;
   //std::cout << "eloss: " << ALMASC_V << " " << BLMASC_V << " " << ALMASC_V+BLMASC_V*PmomMASC_V;
   if (PmomMASC_V<10.*CLHEP::GeV){
     ALMASC_V*=PmomMASC_V/(10.*CLHEP::GeV);
     BLMASC_V*=PmomMASC_V/(10.*CLHEP::GeV);
   }
   //std::cout << " scaled: " << ALMASC_V+BLMASC_V*PmomMASC_V << std::endl;
   
//   Trk::SimplifiedMaterialProperties aSimplifiedMaterialProperties(
//     X0MASC_V,
//     ALMASC_V,
//     BLMASC_V,
//     PmomMASC_V
//   );
   Trk::HomogeneousLayerMaterial aHomogenousLayerMaterial;//(
//     aSimplifiedMaterialProperties
//   );

   ATH_MSG_ERROR(  "Problem in MuonboyToLayer::ToLayer --- no material put on layers!!!" );

   Amg::Vector3D aHep3Vector_dZ(
     XdirMASC_V, 
     YdirMASC_V, 
     ZdirMASC_V
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

   Amg::Vector3D aHep3Vector_dY = aHep3Vector_dZ.cross(aHep3Vector_MostPerp);
   aHep3Vector_dY.normalize();

   Amg::Vector3D aHep3Vector_dX = aHep3Vector_dY.cross(aHep3Vector_dZ);
   aHep3Vector_dX.normalize();
 
   Amg::RotationMatrix3D aHepRotation;
   aHepRotation.col(0) = aHep3Vector_dX;
   aHepRotation.col(1) = aHep3Vector_dY;
   aHepRotation.col(2) = aHep3Vector_dZ;

   Amg::Translation3D aHep3Vector( XposMASC_V, YposMASC_V , ZposMASC_V );

   Amg::Transform3D* pTransform3D = new Amg::Transform3D( aHepRotation, aHep3Vector );

//    double HalfLengthX = 5000. ;
//    double HalfLengthY = 5000. ;
//    Trk::RectangleBounds* pRectangleBounds = new Trk::RectangleBounds(
//      HalfLengthX,
//      HalfLengthY
//    );
   Trk::RectangleBounds* pRectangleBounds = 0 ;

   double thickness_Dummy = 0. ;
   Trk::OverlapDescriptor* pOverlapDescriptor_Dummy = 0;
   int TheLayerType=int(Trk::passive) ;
   
   Trk::PlaneLayer *aPlaneLayer=new Trk::PlaneLayer(
     pTransform3D,
     pRectangleBounds,
     aHomogenousLayerMaterial,
     thickness_Dummy ,
     pOverlapDescriptor_Dummy,
     TheLayerType
   );
   const Trk::PlaneSurface &surf=aPlaneLayer->surfaceRepresentation();
   
   Trk::AtaPlane *trackpar=new Trk::AtaPlane(Amg::Vector3D(XposMASC_V, YposMASC_V , ZposMASC_V),aHep3Vector_dZ.phi(),aHep3Vector_dZ.theta(),sign/PmomMASC_V,surf);

   return std::make_pair(trackpar,aPlaneLayer);

}
