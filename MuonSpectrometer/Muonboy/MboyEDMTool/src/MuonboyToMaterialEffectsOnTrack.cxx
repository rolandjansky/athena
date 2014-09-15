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

#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"

//----------------------------------------------------------------//
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkExInterfaces/IEnergyLossUpdator.h"
#include "TrkExInterfaces/IMultipleScatteringUpdator.h"

//----------------------------------------------------------------//
#include "TrkGeometry/PlaneLayer.h"


/////////////////////////////////////////////////////////
#include "MboyEDMTool/MuonboyToMaterialEffectsOnTrack.h"

//----------------------------------------------------------------//
#include "MboyCoreEvt/MbPOT.h"

#include "MboyEDMToolInterfaces/IMuonboyToLayer.h"

//----------------------------------------------------------------//
#include "MboyCoreEvt/MbMuSc.h"


MuonboyToMaterialEffectsOnTrack::MuonboyToMaterialEffectsOnTrack(const std::string& t, 
                                             const std::string& n,
                                             const IInterface*  p ):AthAlgTool(t,n,p),                                             
 p_IEnergyLossUpdator("Trk::EnergyLossUpdator/AtlasEnergyLossUpdator"),
 p_IMultipleScatteringUpdator("Trk::MultipleScatteringUpdator/AtlasMultipleScatteringUpdator"),
 p_IMuonboyToLayer  ( "MuonboyToLayer" ) 
{

  declareInterface<IMuonboyToMaterialEffectsOnTrack>(this);

  declareProperty("EnergyLossUpdator",p_IEnergyLossUpdator);
  declareProperty("MultipleScatteringUpdator",p_IMultipleScatteringUpdator);
  declareProperty("MuonboyToLayer"  , p_IMuonboyToLayer ) ;

}

MuonboyToMaterialEffectsOnTrack::~MuonboyToMaterialEffectsOnTrack(){}

// Initialize
StatusCode MuonboyToMaterialEffectsOnTrack::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL(  " AthAlgTool::initialize() failed" ) ;
    return StatusCode::FAILURE;
  }

//Retrieve p_IMultipleScatteringUpdator
  if ( p_IMultipleScatteringUpdator.retrieve().isFailure() ) {
    ATH_MSG_FATAL(   "Failed to retrieve tool " << p_IMultipleScatteringUpdator ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO(  "Retrieved tool " << p_IMultipleScatteringUpdator ) ;

//Retrieve p_IEnergyLossUpdator
  if ( p_IEnergyLossUpdator.retrieve().isFailure() ) {
    ATH_MSG_FATAL(   "Failed to retrieve tool " << p_IEnergyLossUpdator ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO(  "Retrieved tool " << p_IEnergyLossUpdator ) ;

  ATH_MSG_INFO(  "Initialisation ended     " ) ;

//Retrieve p_IMuonboyToLayer
  if ( p_IMuonboyToLayer.retrieve().isFailure() ) {
    ATH_MSG_FATAL(   "Failed to retrieve tool " << p_IMuonboyToLayer ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO(  "Retrieved tool " << p_IMuonboyToLayer ) ;

  ATH_MSG_INFO(  "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}

StatusCode MuonboyToMaterialEffectsOnTrack::finalize(){return StatusCode::SUCCESS;}

std::pair<const Trk::TrackParameters*,const Trk::MaterialEffectsOnTrack* > MuonboyToMaterialEffectsOnTrack::GetTrackParamAndMatEffect(
                             const MbPOT* pMbPOT
){

  const MbMuSc* pMbMuSc = dynamic_cast<const MbMuSc*> (pMbPOT);

  if (!pMbMuSc) {
    ATH_MSG_INFO(  " GetTrackParamAndMatEffect pMbMuSc is null  " ) ;
    const Trk::TrackParameters*        pTrackParameters_Empty        = 0 ;
    const Trk::MaterialEffectsOnTrack* pMaterialEffectsOnTrack_Empty = 0 ;
    return std::make_pair(pTrackParameters_Empty,pMaterialEffectsOnTrack_Empty);
  }

  MbMuSc aMbMuSc;
  aMbMuSc.SetRRMASC( pMbMuSc->GetRRMASC() );
  aMbMuSc.SetLLMASC( pMbMuSc->GetLLMASC() );
  aMbMuSc.SetX0MASC( pMbMuSc->GetX0MASC() );
  aMbMuSc.SetS1MASC( pMbMuSc->GetS1MASC() );
  aMbMuSc.SetS2MASC( pMbMuSc->GetS2MASC() );
  aMbMuSc.SetTHMASC( pMbMuSc->GetTHMASC() );
  aMbMuSc.SetALMASC( pMbMuSc->GetALMASC() );
  aMbMuSc.SetBLMASC( pMbMuSc->GetBLMASC() );
  aMbMuSc.SetPosDirMASC( 
                        pMbMuSc->GetXposMASC()  , 
                        pMbMuSc->GetYposMASC()  ,
                        pMbMuSc->GetZposMASC()  ,
                        pMbMuSc->GetXdirMASC()  ,
                        pMbMuSc->GetYdirMASC()  ,
                        pMbMuSc->GetZdirMASC() 
                       );
  aMbMuSc.SetDir2MASC( 
                      pMbMuSc->GetXdir2MASC()  ,
                      pMbMuSc->GetYdir2MASC()  ,
                      pMbMuSc->GetZdir2MASC() 
                     );
  aMbMuSc.SetPmomMASC( pMbMuSc->GetPmomMASC() );
  aMbMuSc.SetChgeMASC( pMbMuSc->GetChgeMASC() );
  

  MbMuSc* pMbMuScNG = &aMbMuSc;

  if ( pMbMuSc->IsToBeReversed() == 1 ){
    double TheCharge = -pMbMuScNG->GetChgeMASC() ;
    double Xdir      = -pMbMuScNG->GetXdirMASC() ;
    double Ydir      = -pMbMuScNG->GetYdirMASC() ;
    double Zdir      = -pMbMuScNG->GetZdirMASC() ;
    double Xdir2     = -pMbMuScNG->GetXdir2MASC() ;
    double Ydir2     = -pMbMuScNG->GetYdir2MASC() ;
    double Zdir2     = -pMbMuScNG->GetZdir2MASC() ;
    double Xpos      =  pMbMuScNG->GetXposMASC() ;
    double Ypos      =  pMbMuScNG->GetYposMASC() ;
    double Zpos      =  pMbMuScNG->GetZposMASC() ;
    pMbMuScNG->SetPosDirMASC( 
                          Xpos  , 
                          Ypos  ,
                          Zpos  ,
                          Xdir  ,
                          Ydir  ,
                          Zdir 
                         );
    pMbMuScNG->SetDir2MASC( 
                           Xdir2  ,
                           Ydir2  ,
                           Zdir2 
                          );
    pMbMuScNG->SetChgeMASC( TheCharge );
  }


  double sign = pMbMuScNG->GetChgeMASC() ;
    
  std::pair<const Trk::TrackParameters*,const Trk::Layer* > aPairTrackParametersLayer = p_IMuonboyToLayer->ToLayer(pMbMuScNG,sign);

  Trk::ParticleHypothesis aParticleHypothesis = Trk::muon ;

  const Trk::Layer*           pLayer           = aPairTrackParametersLayer.second;
  const Trk::TrackParameters* pTrackParameters = aPairTrackParametersLayer.first;
  
  const  Trk::Surface& rSurface = pLayer->surfaceRepresentation();
  
  double currentqoverp=pTrackParameters->parameters()[Trk::qOverP];
  
  const  Amg::Vector2D* pLocalPosition = rSurface.globalToLocal( pTrackParameters->position());
  const  Amg::Vector3D* layerNormal = rSurface.normal(*pLocalPosition);
  double costracksurf = 1.;    
  if(layerNormal){
    costracksurf=fabs((*layerNormal).dot(Amg::Vector3D(cos(pTrackParameters->parameters()[Trk::phi])*sin(pTrackParameters->parameters()[Trk::theta]),
                                                       sin(pTrackParameters->parameters()[Trk::phi])*sin(pTrackParameters->parameters()[Trk::theta]),
                                                       cos(pTrackParameters->parameters()[Trk::theta]))));
  }
  else ATH_MSG_WARNING( "No normal on surface found!" ) ;
  delete layerNormal;
  delete pLocalPosition;
  
  const  Trk::MaterialProperties* pMaterialProperties=pLayer->fullUpdateMaterialProperties(*pTrackParameters);
  double X0=pMaterialProperties->thicknessInX0();    
  double actualx0=X0/costracksurf;
  
  int LLMASC = pMbMuScNG->GetLLMASC();

  Trk::EnergyLoss* pEnergyLoss = 0 ;
  if ( LLMASC == 1 
  ||   LLMASC == 5 
  ||   LLMASC == 7) 
  pEnergyLoss = p_IEnergyLossUpdator->energyLoss(*pMaterialProperties, fabs(1./currentqoverp) ,1./costracksurf,Trk::alongMomentum,aParticleHypothesis);

  Trk::ScatteringAngles* pScatteringAngles = 0 ;
  if ( LLMASC != 1 ){
    double XdirMASC_V = pMbMuScNG->GetXdirMASC() ;
    double YdirMASC_V = pMbMuScNG->GetYdirMASC() ;
    double ZdirMASC_V = pMbMuScNG->GetZdirMASC() ;
    Amg::Vector3D aHep3Vector_dir(
      XdirMASC_V, 
      YdirMASC_V, 
      ZdirMASC_V
    );
    aHep3Vector_dir.normalize();
    double Xdir2MASC_V = pMbMuScNG->GetXdir2MASC() ;
    double Ydir2MASC_V = pMbMuScNG->GetYdir2MASC() ;
    double Zdir2MASC_V = pMbMuScNG->GetZdir2MASC() ;
    Amg::Vector3D aHep3Vector_dir2(
      Xdir2MASC_V, 
      Ydir2MASC_V, 
      Zdir2MASC_V
    );
    aHep3Vector_dir2.normalize();
    double DeltaTheta = aHep3Vector_dir.theta() - aHep3Vector_dir2.theta() ;
    double DeltaPhi   = aHep3Vector_dir.phi()   - aHep3Vector_dir2.phi()   ;
    if(DeltaPhi>M_PI) DeltaPhi -= 2.*M_PI; else if(DeltaPhi<-M_PI) DeltaPhi +=2.*M_PI;
    double sigmascat=sqrt(p_IMultipleScatteringUpdator->sigmaSquare(*pMaterialProperties, fabs(1./currentqoverp) ,1./costracksurf,aParticleHypothesis));
    pScatteringAngles = new Trk::ScatteringAngles(DeltaPhi,DeltaTheta,sigmascat/sin(pTrackParameters->parameters()[Trk::theta]),sigmascat);
  }

  Trk::MaterialEffectsOnTrack* pMaterialEffectsOnTrack = 0 ;
  
  if (pScatteringAngles){
    if (pEnergyLoss){
      pMaterialEffectsOnTrack = new Trk::MaterialEffectsOnTrack(actualx0,pScatteringAngles,pEnergyLoss,rSurface );
    }else{
      pMaterialEffectsOnTrack = new Trk::MaterialEffectsOnTrack(actualx0,pScatteringAngles,rSurface );
    }
  }else{
    if (pEnergyLoss){
      pMaterialEffectsOnTrack = new Trk::MaterialEffectsOnTrack(actualx0,pEnergyLoss,rSurface );
    }else{
      pMaterialEffectsOnTrack = new Trk::MaterialEffectsOnTrack(actualx0,rSurface );
    }
  }

  delete pLayer;
  
  return std::make_pair(pTrackParameters,pMaterialEffectsOnTrack);
    
}                                  
