/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

//----------------------------------------------------------------//
//Can not be forwarded due to typedef
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/ParamDefs.h"

//----------------------------------------------------------------//
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"

#include "GeoPrimitives/GeoPrimitivesHelpers.h"

/////////////////////////////////////////////////////////
#include "MboyEDMTool/MbMuScToTSOSTool.h"

//----------------------------------------------------------------//
#include "MboyCoreEvt/MbPOT.h"
#include "MboyCoreEvt/MbMuSc.h"
#include "MboyEDMToolInterfaces/IMuonboyToMaterialEffectsOnTrack.h"

Trk::MbMuScToTSOSTool::MbMuScToTSOSTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p)
, p_IMuonboyToMaterialEffectsOnTrack ( "MuonboyToMaterialEffectsOnTrack" )
{

  declareInterface<IMbMuScToTSOSTool>(this);

//Declare the properties

  declareProperty("MuonboyToMaterialEffectsOnTrack"  , p_IMuonboyToMaterialEffectsOnTrack ) ;

}

Trk::MbMuScToTSOSTool::~MbMuScToTSOSTool(){}

// Initialize
StatusCode Trk::MbMuScToTSOSTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" );
    return( StatusCode::FAILURE );
  }

//Retrieve p_IMuonboyToMaterialEffectsOnTrack
  if ( p_IMuonboyToMaterialEffectsOnTrack.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMuonboyToMaterialEffectsOnTrack );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMuonboyToMaterialEffectsOnTrack );

  ATH_MSG_INFO( "Initialisation ended     " );

  return StatusCode::SUCCESS;

}

StatusCode Trk::MbMuScToTSOSTool::finalize(){return StatusCode::SUCCESS;}


const Trk::TrackStateOnSurface*
Trk::MbMuScToTSOSTool::GiveTSOSforMbMuSc(const MbPOT* pMbPOT){
  
  if ( pMbPOT == 0 ) {
    ATH_MSG_INFO( " GiveTSOSforMbMuSc called with a null pMbPOT  "
                    );
    return 0 ;
  }
  if ( !(pMbPOT->IsTypeMUSC ()) ) {
    ATH_MSG_INFO( " GiveTSOSforMbMuSc called for a nonMUSC pMbPOT with " 
                    << " GetType() "<< pMbPOT->GetType() << " GetTech() "<< pMbPOT->GetTech() 
                    );
    return 0 ;
  }
  const MbMuSc* pMbMuSc = dynamic_cast<const MbMuSc*> (pMbPOT);
  if (!pMbMuSc) {
    ATH_MSG_INFO( " GiveTSOSforMbMuSc pMbMuSc is null  " );
    return 0 ;
  }
  
  std::pair<const Trk::TrackParameters*,const Trk::MaterialEffectsOnTrack* > ThePair = p_IMuonboyToMaterialEffectsOnTrack->GetTrackParamAndMatEffect(pMbPOT);
  
  const Trk::TrackParameters*        pTrackParameters        = ThePair.first ;
  const Trk::MaterialEffectsOnTrack* pMaterialEffectsOnTrack = ThePair.second ;
  
  if ( pTrackParameters == 0) {
    ATH_MSG_INFO( " No pTrackParameters for a pMbPOT with " 
                    << " GetType() "<< pMbPOT->GetType() << " GetTech() "<< pMbPOT->GetTech() 
                    );
    return 0 ;
  }
  if ( pMaterialEffectsOnTrack == 0) {
    ATH_MSG_INFO( " No pMaterialEffectsOnTrack for a pMbPOT with " 
                    << " GetType() "<< pMbPOT->GetType() << " GetTech() "<< pMbPOT->GetTech() 
                    );
    return 0 ;
  }
    

  const Trk::AtaPlane* pAtaPlane = dynamic_cast<const Trk::AtaPlane*> (pTrackParameters);
  if (!pAtaPlane) {
    ATH_MSG_INFO( " GiveTSOSforMbMuSc pAtaPlane is null  " );
    return 0 ;
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
  aMbMuSc.SetPmomMASC ( pMbMuSc->GetPmomMASC () );
  aMbMuSc.SetPmom2MASC( pMbMuSc->GetPmom2MASC() );
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

  double XposMASC_V  = pMbMuScNG->GetXposMASC() ;
  double YposMASC_V  = pMbMuScNG->GetYposMASC() ;
  double ZposMASC_V  = pMbMuScNG->GetZposMASC() ;
  double Pmom2MASC_V = pMbMuScNG->GetPmom2MASC() ;
  double Xdir2MASC_V = pMbMuScNG->GetXdir2MASC() ;
  double Ydir2MASC_V = pMbMuScNG->GetYdir2MASC() ;
  double Zdir2MASC_V = pMbMuScNG->GetZdir2MASC() ;
  Amg::Vector3D aHep3Vector_dZ(
    Xdir2MASC_V, 
    Ydir2MASC_V, 
    Zdir2MASC_V
  );
  aHep3Vector_dZ.normalize();
  Amg::Vector3D aHep3Vector( XposMASC_V, YposMASC_V , ZposMASC_V );

  const Trk::TrackParameters* pTrackParameters2 
      = new Trk::AtaPlane(aHep3Vector,aHep3Vector_dZ.phi(),aHep3Vector_dZ.theta(),sign/Pmom2MASC_V, pAtaPlane->associatedSurface());
      
  delete pTrackParameters;
  pTrackParameters = pTrackParameters2 ;


  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> pattern;
  pattern.set(Trk::TrackStateOnSurface::Scatterer);

  const Trk::FitQualityOnSurface* pFitQualityOnSurface = 0;
  const Trk::MeasurementBase*     pMeasurementBase     = 0;
 
  return  ( new TrackStateOnSurface(pMeasurementBase, pTrackParameters, pFitQualityOnSurface,  pMaterialEffectsOnTrack   , pattern) ) ;

}



