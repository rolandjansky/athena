/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloClusterROI_Builder.h"

#include "CaloTrackingGeometry/ICaloSurfaceBuilder.h"
#include "CaloDetDescr/CaloDepthTool.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEgamma/EgammaxAODHelpers.h"


  
//#include "TrkEventPrimitives/GlobalPosition.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkSurfaces/Surface.h"

#include "TrkCaloClusterROI/CaloClusterROI.h"


InDet::CaloClusterROI_Builder::CaloClusterROI_Builder(const std::string& t,
                                               const std::string& n,
                                               const IInterface*  p )
: AthAlgTool(t,n,p),
  m_calosurf("CaloSurfaceBuilder")
{
  declareInterface<ICaloClusterROI_Builder>(this);
  declareProperty( "CaloSurfaceBuilder",     m_calosurf      );
}

//--------------------------------------------------------------------------------------------
InDet::CaloClusterROI_Builder::~CaloClusterROI_Builder() {}
//--------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------
StatusCode InDet::CaloClusterROI_Builder::initialize() {
//--------------------------------------------------------------------------------------------
  
  ATH_MSG_INFO("Initializing CaloClusterROI_Builder");
 
  // Retrieve the updator CaloSurfaceBuilder
  if ( m_calosurf.retrieve().isFailure() ){
    ATH_MSG_FATAL ( "Unable to retrieve the instance " << m_calosurf.name() << "... Exiting!" );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------------------------
StatusCode InDet::CaloClusterROI_Builder::finalize(){ return StatusCode::SUCCESS; }
//--------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------
Trk::CaloClusterROI* InDet::CaloClusterROI_Builder::buildClusterROI( const xAOD::CaloCluster* cluster ) const
//--------------------------------------------------------------------------------------------
{

  ATH_MSG_DEBUG("Building Trk::CaloCluster_OnTrack");
   
  if(!cluster) return 0;

  const Trk::Surface* surface = getCaloSurface( cluster );
  
  if(!surface) return 0;
  
  const Trk::LocalParameters* lp = getClusterLocalParameters( cluster, surface );

  if (!lp){
    delete surface;
    return 0;
  }
  
  double energy    = cluster->e();
  double  widthPhi = 0.1;
  double  widthEta = 0.1;
  
  
  Trk::CaloClusterROI* ccROI = new  Trk::CaloClusterROI( lp, *surface, energy, widthPhi, widthEta );
  delete surface;
  
  if(ccROI) {
    ATH_MSG_DEBUG("Successful build of Trk::CaloClusterROI");
  }
  
  return ccROI;


}



//--------------------------------------------------------------------------------------------
const Trk::Surface*  InDet::CaloClusterROI_Builder::getCaloSurface( const xAOD::CaloCluster* cluster ) const
//--------------------------------------------------------------------------------------------
{
 
  const Trk::Surface* destinationSurface = 0;
  
  // Determine if we want to extrapolate to the barrel or endcap.  If in the crack choose the 
  // detector with largest amount of energy in the second sampling layer 
  if ( xAOD::EgammaHelpers::isBarrel( cluster ) )
  {
    destinationSurface = m_calosurf->CreateUserSurface (CaloCell_ID::EMB2, 0. , cluster->eta() );
  } else{ 
    destinationSurface = m_calosurf->CreateUserSurface (CaloCell_ID::EME2, 0. , cluster->eta() );
  }
  return destinationSurface;
}


//--------------------------------------------------------------------------------------------
const Trk::LocalParameters*  InDet::CaloClusterROI_Builder::getClusterLocalParameters( const xAOD::CaloCluster* cluster, 
                                                                                       const Trk::Surface* surf) const
//--------------------------------------------------------------------------------------------
{
 
  Amg::Vector3D surfRefPoint = surf->globalReferencePoint();
  //std::cout << "REFPOINT " << "[r,phi,z] = [ " << surfRefPoint.perp() << ", " << surfRefPoint.phi() << ", " << surfRefPoint.z() << " ]" <<std::endl; 
  
  double eta = cluster->eta();
  double theta = 2*atan(exp(-eta)); //  -log(tan(theta/2));
  double tantheta = tan(theta);
  double phi = cluster->phi();
  
  Trk::LocalParameters* newLocalParameters(0);
 
  if (  xAOD::EgammaHelpers::isBarrel( cluster ) ){
    //Two corindate in a cyclinder are 
    //Trk::locRPhi = 0 (ie phi)
    //Trk::locZ    = 1(ie z)
    double r = surfRefPoint.perp() ;
    double z = tantheta == 0 ? 0. : r/tantheta;      
    Trk::DefinedParameter locRPhi( r * phi  ,  Trk::locRPhi  ) ;
    Trk::DefinedParameter locZ   ( z        ,  Trk::locZ     ) ;
    std::vector<Trk::DefinedParameter> defPar ;
    defPar.push_back( locRPhi ) ;
    defPar.push_back( locZ ) ;
    newLocalParameters = new Trk::LocalParameters( defPar ) ;
  } else{ 
    //Local paramters of a disk are
    //Trk::locR   = 0
    //Trk::locPhi = 1
    double z = surfRefPoint.z();
    double r = z*tantheta; 
    Trk::DefinedParameter locR  ( r   ,  Trk::locR    ) ;
    Trk::DefinedParameter locPhi( phi ,  Trk::locPhi  ) ;
    std::vector<Trk::DefinedParameter> defPar ;
    defPar.push_back( locR ) ;
    defPar.push_back( locPhi ) ;
    newLocalParameters = new Trk::LocalParameters( defPar ) ;
  }
  
  return newLocalParameters;

}


