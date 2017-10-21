/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ***************************************************************************
// Liquid Argon detector description package
// -----------------------------------------
//
//****************************************************************************

#include "TBRec/TBExtrapolTrackToCaloTool.h"

#include "GaudiKernel/Property.h"

#include <vector>

// Stuff needed for the extrapolation :
//#include "TrkEventPrimitives/GlobalPosition.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "xAODTracking/Vertex.h"
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"

// Calo specific stuff :
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/ICaloCoordinateTool.h"
#include "CaloTrackingGeometry/ICaloSurfaceBuilder.h"
#include "CaloDetDescr/CaloDepthTool.h"

#include <cmath>
#include <iostream>
#include <iomanip>



TBExtrapolTrackToCaloTool::TBExtrapolTrackToCaloTool(const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_calo_id(0),
  m_calo_dd(0),
  m_calo_tb_coord(0),
  m_calosurf("CaloSurfaceBuilder"),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator")


  //  m_extrapolatorName("Trk::Extrapolator"),   
  // m_extrapolatorInstanceName("Extrapolator")
{
  //declareInterface<IExtrapolateToCaloTool>( this );

 declareProperty ("CaloSurfaceBuilder",m_calosurf,"Extrapolation ToolHandle");
 declareProperty ("Extrapolator",m_extrapolator,"CaloSurfaceBuilder ToolHandle");

}

TBExtrapolTrackToCaloTool::~TBExtrapolTrackToCaloTool()
{}

StatusCode
TBExtrapolTrackToCaloTool::initialize()
{

  // at this point the joboption should have been read
  m_calo_dd = CaloDetDescrManager::instance();
  m_calo_id = m_calo_dd->getCaloCell_ID();
  
  
    
  ATH_MSG_DEBUG ( " getting started ");
 

    
  // Get TrkExtrapolator from ToolService
  StatusCode s=m_extrapolator.retrieve();
    
  if (s.isFailure())
    {
      ATH_MSG_FATAL ( "Could not find Tool " 
			<< m_extrapolator.name() << " with parent " << m_extrapolator.parentName() );
      return s;
    } else
    {
      ATH_MSG_INFO ( "Successfully retrieved tool  " << m_extrapolator.typeAndName()  << " with parent " << m_extrapolator.parentName() ) ;
    }


  IAlgTool* tool;    
  s = toolSvc()->retrieveTool("TBCaloCoordinate",tool);
  if (s.isFailure())
    { 
      ATH_MSG_FATAL ("Could not find Tool TBCaloCoordinate" );
      
      return s;
    }     
  m_calo_tb_coord = dynamic_cast<ICaloCoordinateTool*>(tool);


    
  // Get the CaloSurface tool and sets the depthtool it should use:
  s=m_calosurf.retrieve();
    
  if (s.isFailure())
    { 
	ATH_MSG_FATAL ("Could not find Tool CaloSurfaceBuilder" 
		       << m_calosurf.typeAndName());
	return s;
    } else {
    ATH_MSG_INFO ("Successfully retrieved Tool CaloSurfaceBuilder " 
		  << m_calosurf.typeAndName());

  }


  StatusCode sc = StatusCode::SUCCESS;
  return sc;
}

StatusCode
TBExtrapolTrackToCaloTool::finalize()
{
    StatusCode sc = StatusCode::SUCCESS;
    return sc;
}

void 
TBExtrapolTrackToCaloTool::CaloVariables (const Trk::TrackParameters* parm, 
			       double& etaCaloLocal, double& phiCaloLocal)
{


  Amg::Vector3D pt_ctb,pt_local;  
  etaCaloLocal = 0.;
  phiCaloLocal = 0.;

  if (!parm) {
    ATH_MSG_WARNING ("CaloVariables called with null Trk::TrackParameters*" ) ;
    return;
  }
  
  pt_ctb = parm->position();

  m_calo_tb_coord->ctb_to_local(pt_ctb, pt_local);

  if ( msgLvl(MSG::DEBUG) ){
    
    ATH_MSG_DEBUG ("Impact point in ctb coord : x,y,z= " 
	<< pt_ctb.x() << " " 
	<< pt_ctb.y() << " " << pt_ctb.z() << " R=" 
	<< std::sqrt( pt_ctb.x()*pt_ctb.x() + pt_ctb.y()*pt_ctb.y() 
		    + pt_ctb.z()*pt_ctb.z())
	 << " eta=" << pt_ctb.eta() << " phi=" << pt_ctb.phi() );


    
    ATH_MSG_DEBUG ("Impact point in local coord : x,y,z= " 
	<< pt_local.x() << " " 
	<< pt_local.y() << " " << pt_local.z() << " R=" 
	<< std::sqrt( pt_local.x()*pt_local.x() + pt_local.y()*pt_local.y() 
		    + pt_local.z()*pt_local.z())
			<< " eta=" << pt_local.eta() << " phi=" << pt_local.phi());
  }
  
  etaCaloLocal = pt_local.eta();
  phiCaloLocal = pt_local.phi();
  
}

void 
TBExtrapolTrackToCaloTool::CaloLocalPoint (const Trk::TrackParameters* parm,
                                           Amg::Vector3D* pt_ctb, 
                                           Amg::Vector3D* pt_local)
{

  if (!parm) {
    ATH_MSG_WARNING ("CaloLocalPoint TrkParam called with null Trk::TrackParameters*" );
    return;
  }

  if (!pt_local) {
    ATH_MSG_WARNING ("CaloLocalPoint TrkParam called with null Amg::Vector3D* pt_local" );
    return;
  }

  if (!pt_ctb) {
    ATH_MSG_WARNING ("CaloLocalPoint TrkParam called with null Amg::Vector3D* pt_ctb" );
    return;
  }
  

  *pt_ctb = parm->position();

  ATH_MSG_DEBUG ("Impact point in ctb coord : x,y,z= " 
      << pt_ctb->x() << " " 
      << pt_ctb->y() << " " << pt_ctb->z() << " R=" 
      << std::sqrt( pt_ctb->x()*pt_ctb->x() + pt_ctb->y()*pt_ctb->y() 
		    + pt_ctb->z()*pt_ctb->z())
		 << " eta=" << pt_ctb->eta() << " phi=" << pt_ctb->phi() );

  m_calo_tb_coord->ctb_to_local(*pt_ctb, *pt_local);
    
  ATH_MSG_DEBUG ("Impact point in local coord : x,y,z= " 
      << pt_local->x() << " " 
      << pt_local->y() << " " << pt_local->z() << " R=" 
      << std::sqrt( pt_local->x()*pt_local->x() 
		    + pt_local->y()*pt_local->y() 
		    + pt_local->z()*pt_local->z())
		 << " eta=" << pt_local->eta() << " phi=" << pt_local->phi());
}

bool  
TBExtrapolTrackToCaloTool::TrackSeenByCalo (const Trk::Track* trk, 
				const CaloCell_ID::CaloSample sample,
				const double offset, 
				Amg::Vector3D* pt_ctb,
                                Amg::Vector3D* pt_local)
{


  if (!trk) {
    ATH_MSG_WARNING ("TrackSeenByCalo Trk called with null Trk::TrackParameters*" );
    return false;
  }

  if (!pt_local) {
    ATH_MSG_WARNING ("TrackSeenByCalo Trk called with null Amg::Vector3D* pt_local");
    
    return false;
  }

  if (!pt_ctb) {
    ATH_MSG_WARNING ("TrackSeenByCalo Trk called with null Amg::Vector3D* pt_ctb");
    
    return false;
  }


  Trk::Surface* surf = 0;
  bool success = false;
  double trketa = 0.;

  // Take eta of the last measured hit as best guess and create surface :
  const DataVector <const Trk::TrackParameters>* paramvec = trk->trackParameters();
  if (paramvec) { 
    DataVector <const Trk::TrackParameters>::const_iterator it = paramvec->begin();
    DataVector <const Trk::TrackParameters>::const_iterator itEnd = paramvec->end();
    for (;it!=itEnd; it++) 
      trketa = (*it)->eta();
    surf = m_calosurf->CreateUserSurface (sample,offset,trketa);
  } 
  if (!surf) return success;

  //std::cout << " here we go : " << std::endl;
  const Trk::TrackParameters* param = extrapolate(trk,surf);


  
  if(param) {
    CaloLocalPoint (param,pt_ctb, pt_local);
    success = true;
    delete param;  
  }
  delete surf;

  return success;
} 

bool  
TBExtrapolTrackToCaloTool::TrackSeenByCalo (const Trk::TrackParameters* parm, 
				const CaloCell_ID::CaloSample sample,
				const double offset, 
				Amg::Vector3D* pt_ctb,
                                Amg::Vector3D* pt_local)
{
  bool success = false;


  if (!parm) {
    ATH_MSG_WARNING ("TrackSeenByCalo TrkParam called with null Trk::TrackParameters*" );
    return false;
  }

  if (!pt_local) {

    ATH_MSG_WARNING("TrackSeenByCalo TrkParam called with null Amg::Vector3D* pt_local" );
    return false;
  }

  if (!pt_ctb) {
    ATH_MSG_WARNING ("TrackSeenByCalo TrkParam called with null Amg::Vector3D* pt_ctb" );
    return false;
  }


  // Take eta as best guess of track direction and create surface :
  double trketa = parm->eta();
  Trk::Surface* surf = m_calosurf->CreateUserSurface (sample,offset,trketa);
  if (!surf) return success;

  const Trk::TrackParameters* resparam = extrapolate(parm,surf);


  
  if(resparam) {
    CaloLocalPoint (resparam,pt_ctb, pt_local);
    success = true;
    delete resparam;  
  }
  delete surf;

  return success;
} 

bool  
TBExtrapolTrackToCaloTool::TrackSeenByCalo (const Trk::Track* trk,  
					  const bool barrel,
					  const CaloCell_ID::SUBCALO subcalo,
					  const int sampling_or_module, 
					  const double offset, 
					  Amg::Vector3D* pt_ctb,
                                          Amg::Vector3D* pt_local)
{


  CaloCell_ID::CaloSample sample;
  m_calo_dd->build_sample ( subcalo, barrel ,sampling_or_module, sample);
  return TrackSeenByCalo(trk,sample,offset,pt_ctb,pt_local);
} 

bool  
TBExtrapolTrackToCaloTool::TrackSeenByCalo (const Trk::TrackParameters* parm,  
					  const bool barrel,
					  const CaloCell_ID::SUBCALO subcalo,
					  const int sampling_or_module, 
					  const double offset, 
					  Amg::Vector3D* pt_ctb,
                                          Amg::Vector3D* pt_local)
{
  CaloCell_ID::CaloSample sample;
  m_calo_dd->build_sample ( subcalo, barrel ,sampling_or_module, sample);
  return TrackSeenByCalo(parm,sample,offset,pt_ctb,pt_local);
} 

bool 
TBExtrapolTrackToCaloTool::TrackSeenByCalo (const Trk::Track* trk, 
					  const CaloCell_ID::CaloSample sample,
					  const double offset, 
					  Amg::Vector3D* pt_ctb,
                                          Amg::Vector3D* pt_local,
					  double& trketa_atcalo, double& trkphi_atcalo)
{


  if (!trk) {
    ATH_MSG_WARNING ("TrackSeenByCalo Trk called with null Trk::TrackParameters*" );
    return false;
  }

  if (!pt_local) {
    ATH_MSG_WARNING ("TrackSeenByCalo Trk called with null Amg::Vector3D* pt_local" ) ;
    return false;
  }

  if (!pt_ctb) {
    ATH_MSG_WARNING ("TrackSeenByCalo Trk called with null Amg::Vector3D* pt_ctb" );
    return false;
  }

  Trk::Surface* surf = 0;
  bool success = false;
  double trketa = 0.;
  
  // Take eta of the last measured hit as best guess and create surface :
  const DataVector <const Trk::TrackParameters>* paramvec = trk->trackParameters();
  if (paramvec) { 
    DataVector <const Trk::TrackParameters>::const_iterator it = paramvec->begin();
    DataVector <const Trk::TrackParameters>::const_iterator itEnd = paramvec->end();

    if(m_calo_dd -> lar_geometry() == "H8")
      trketa = m_calo_tb_coord -> beam_local_eta();
    else 
      for (;it!=itEnd; it++) 
	trketa = (*it)->eta();
    
    //std::cout<< " In Extrapol... : "<<trketa<<std::endl;

    surf = m_calosurf->CreateUserSurface (sample,offset,trketa);
  } 
  if (!surf) return success;
  
  //std::cout << " here we go : " << std::endl;
  const Trk::TrackParameters* param = extrapolate(trk,surf);
  

  
  if(param) {
    CaloLocalPoint (param,pt_ctb, pt_local);
    trketa_atcalo = param->eta();
    if ( sin(param->parameters()[Trk::phi]) > 0.)
      trkphi_atcalo = std::acos(cos(param->parameters()[Trk::phi]));
    else 
      trkphi_atcalo = -1. * std::acos(cos(param->parameters()[Trk::phi]));
    trkphi_atcalo = m_range.fix(trkphi_atcalo);
    success = true;
    delete param;  
  }
  else {
    trketa_atcalo = 999999.;
    trkphi_atcalo = 999999.;
  }
  delete surf;
  
  return success;
}

CaloDepthTool*
TBExtrapolTrackToCaloTool::getCaloDepth()
{
  if (m_calosurf==0) return nullptr;
  
  // getCaloDepth from the surface builder
  return m_calosurf->getCaloDepth();
}

const Trk::TrackParameters* 
TBExtrapolTrackToCaloTool::extrapolate(const Trk::TrackParameters* parm,
			   const Trk::Surface* surf)
{


  const Trk::TrackParameters* param=0;

  if (!parm) {
    ATH_MSG_WARNING ("extrapolate TrkParam called with null Trk::TrackParameters*" ) ;
    return param;
  }

  if (!surf) {
    ATH_MSG_WARNING ("extrapolate TrkParam called with null Trk::Surface*" << endmsg );
    return param;
  }




  ATH_MSG_DEBUG ( "Trying to propagate TrackParameters to Surface ... " 
      << (*surf));

  // for the moment use nonInteracting to avoid the navigation   
  param = m_extrapolator->extrapolate( *parm, *surf,
				       Trk::alongMomentum,
				       true, Trk::nonInteracting);
  
  if (param)
    ATH_MSG_DEBUG ("Propagation successful ");
  else
    ATH_MSG_DEBUG ("Propagation failed ");

  return param;
}

const Trk::TrackParameters* 
TBExtrapolTrackToCaloTool::extrapolate(const Trk::Track* trk,
			   const Trk::Surface* surf)
{

  const Trk::TrackParameters* param=0;
  if (!trk) {
    ATH_MSG_WARNING ("extrapolate Trk called with null Trk::Track*");
    return param;
  }

  if (!surf) {
    ATH_MSG_WARNING ("extrapolate Trk called with null Trk::Surface*" );
    return param;
  }




  ATH_MSG_DEBUG ("Trying to propagate to Surface ... " << (*surf) );
  
  param = m_extrapolator->extrapolate( *trk, *surf,
				       Trk::alongMomentum,
				       true, Trk::nonInteracting);
  
  // the other way to do it:
  //
  // const TrackParameters* extrapolate(const Trk::Propagator& prop,
  // const Trk::Track& trk,
  // const Trk::Surface& sf,
  // Trk::PropDirectiondir=anyDirection,Trk::BoundaryCheck bcheck = true) 
  //        const;

  if (param)
    ATH_MSG_DEBUG ("Propagation successful " );
  else
    ATH_MSG_DEBUG ("Propagation failed ");

  return param;
}


Amg::Vector3D TBExtrapolTrackToCaloTool::getMomentumAtVertex(const xAOD::Vertex& vertex, bool /*reuse*/) const
{
  Amg::Vector3D momentum(0., 0., 0.);  
  if (vertex.vxTrackAtVertexAvailable())
  {
    // Use the parameters at the vertex 
    // (the tracks should be parallel but we will do the sum anyway)
    for (const auto& trkAtVertex : vertex.vxTrackAtVertex())
    {
      const Trk::TrackParameters* paramAtVertex = trkAtVertex.perigeeAtVertex();
      if (!paramAtVertex)
        ATH_MSG_WARNING("VxTrackAtVertex does not have perigee at vertex");
      else
        momentum += paramAtVertex->momentum();
    }
  }
  else if (vertex.nTrackParticles() == 1)
  {
    // Use the first measurement
    const xAOD::TrackParticle *tp = vertex.trackParticle(0);
    unsigned int index(0);
    if (!tp || !tp->indexOfParameterAtPosition(index, xAOD::FirstMeasurement))
    {
      ATH_MSG_WARNING("No TrackParticle or no have first measurement");
    }
    else
      momentum += tp->curvilinearParameters(index).momentum();
    // OR last 3 values of trackParameters(index)
  }
  else
  {
    // Extrapolate track particles to vertex
    // (the tracks should be parallel but we will do the sum anyway)
    const Trk::PerigeeSurface *surface = new Trk::PerigeeSurface(vertex.position());
    for (unsigned int i = 0; i < vertex.nTrackParticles(); ++i)
    {
      const xAOD::TrackParticle* tp = vertex.trackParticle( i );
      if (!tp)
      {
        ATH_MSG_WARNING("NULL pointer to TrackParticle in vertex");
        continue;
      }
      const Trk::TrackParameters* params = m_extrapolator->extrapolate(*tp, *surface, Trk::alongMomentum);
      if (!params)
        ATH_MSG_DEBUG("Extrapolation to vertex (perigee) failed");
      else
      {
        momentum += params->momentum();
        delete params;
      }
    }
    delete surface;
  }

  return momentum;
}


#if 0
Trk::Intersection TBExtrapolTrackToCaloTool::getIntersectionInCalo(const Amg::Vector3D& position, const Amg::Vector3D& momentum, const CaloCell_ID::CaloSample sample) const
{
  Trk::Intersection result{Amg::Vector3D(0., 0., 0.), 0., false};

  // get the destination Surface
  const Trk::Surface* surface = m_calosurf->CreateUserSurface (sample, 0. /* offset */, momentum.eta());
  if (!surface)
    return result;
  
  // intersect with calorimeter surface
  result = surface->straightLineIntersection(position, momentum);     
  delete surface;
  return result;
}
#endif

