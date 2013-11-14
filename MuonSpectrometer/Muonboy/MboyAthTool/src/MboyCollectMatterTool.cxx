/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "TrkSurfaces/RectangleBounds.h"
#include "TrkParameters/TrackParameters.h"

#include "TrkGeometry/HomogeneousLayerMaterial.h"

#include "MboyAthToolInterfaces/IMboyAthToolHelper.h"

/////////////////////////////////////////////////////////
#include "MboyAthTool/MboyCollectMatterTool.h"

#include "MboyEDMToolInterfaces/IMuonboyToLayer.h"

#include "MboyCoreEvt/MbMuSc.h"

MboyCollectMatterTool::MboyCollectMatterTool(const std::string& t, 
                                             const std::string& n,
                                             const IInterface*  p ):AthAlgTool(t,n,p),
 p_OwnEDMHelper                  ( "MboyAthToolHelper" ),
 p_IMuonboyToLayer  ( "MuonboyToLayer" )
{

  declareInterface<MboyCollectMatterTool>(this);

  declareProperty("MboyAthToolHelper"                 , p_OwnEDMHelper          ) ;
  declareProperty("MuonboyToLayer"  , p_IMuonboyToLayer ) ;

  declareProperty("AtCosm" , m_AtCosm   = 0   ) ;
  
  declareProperty("MuSpectro_Radius" , m_MuSpectro_Radius   = 4250.    ) ;
  declareProperty("MuSpectro_Zcyli"  , m_MuSpectro_Zcyli    = 6820.    ) ;
  declareProperty("RadiusProtection" , m_RadiusProtection   =    0.001 ) ;

}

MboyCollectMatterTool::~MboyCollectMatterTool(){}

// Initialize
StatusCode MboyCollectMatterTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING( " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }

  ATH_MSG_INFO( "================================" ) ;
  ATH_MSG_INFO( "=Proprieties are     " ) ;
  ATH_MSG_INFO( "= AtCosm                           " << m_AtCosm ) ;
  ATH_MSG_INFO( "= MuSpectro_Radius                 " << m_MuSpectro_Radius ) ;
  ATH_MSG_INFO( "= MuSpectro_Zcyli                  " << m_MuSpectro_Zcyli ) ;
  ATH_MSG_INFO( "= RadiusProtection                 " << m_RadiusProtection       ) ;
  ATH_MSG_INFO( "================================" ) ;


//Retrieve p_OwnEDMHelper
  if ( p_OwnEDMHelper.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_OwnEDMHelper ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_OwnEDMHelper ) ;

//Retrieve p_IMuonboyToLayer
  if ( p_IMuonboyToLayer.retrieve().isFailure() ) {
    ATH_MSG_FATAL(  "Failed to retrieve tool " << p_IMuonboyToLayer ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMuonboyToLayer ) ;

  ATH_MSG_INFO( "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}

StatusCode MboyCollectMatterTool::finalize(){return StatusCode::SUCCESS;}

void MboyCollectMatterTool::GiveSurfaceParameters(
                              double& MuSpectro_Radius ,
                              double& MuSpectro_Zcyli  ,
                              double& RadiusProtection 
){

  MuSpectro_Radius = m_MuSpectro_Radius ;
  MuSpectro_Zcyli  = m_MuSpectro_Zcyli  ;
  RadiusProtection = m_RadiusProtection ;

}

StatusCode MboyCollectMatterTool::DoCollectMatter(
                                       const Trk::TrackParameters* pTrackParameters,
                                       std::vector<std::pair<const Trk::TrackParameters*,const Trk::Layer* > > *VectorOfPlaneLayer
){

  StatusCode sc = StatusCode::SUCCESS;

  std::vector<MbMuSc*>* pVectorOfMbMuSc = new std::vector<MbMuSc*>();
  sc = p_OwnEDMHelper->DoCollectMatter( 
                                  m_AtCosm ,
  				  pTrackParameters,
    				  pVectorOfMbMuSc 
    				 ) ;
   
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " DoCollectMatter failed " ) ;
    delete pVectorOfMbMuSc;
    return( StatusCode::FAILURE);
  }
 
  double qoverp=pTrackParameters->parameters()[Trk::qOverP];
  double sign = (qoverp>=0) ? 1 : -1;
  int SizeOfVectorOfMbMuSc = pVectorOfMbMuSc->size();
  for (int Index= 0 ; Index <SizeOfVectorOfMbMuSc ; Index++){
    MbMuSc* pMbMuSc =  (*(pVectorOfMbMuSc))[Index] ;
    std::pair<const Trk::TrackParameters*,const Trk::Layer* > aPlaneLayer = DoConvert(pMbMuSc,sign);
    VectorOfPlaneLayer->push_back(aPlaneLayer) ;
    delete pMbMuSc;
  }
  delete pVectorOfMbMuSc;

  return StatusCode::SUCCESS;

}



std::pair<const Trk::TrackParameters*,const Trk::Layer* > MboyCollectMatterTool::DoConvert(
                            const MbMuSc* pMbMuSc,
			    double sign
){
   return p_IMuonboyToLayer->ToLayer(pMbMuSc,sign);
}
