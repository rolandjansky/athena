/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimParametrization/FastCaloSimCaloExtrapolation.h"
#include "ISF_FastCaloSimParametrization/IFastCaloSimGeometryHelper.h"

#include "CaloTrackingGeometry/ICaloSurfaceHelper.h"
#include "CaloTrackingGeometry/ICaloSurfaceBuilder.h"
#include "CaloDetDescr/ICaloCoordinateTool.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "GaudiKernel/ListItem.h"

FastCaloSimCaloExtrapolation::FastCaloSimCaloExtrapolation(const std::string& t, const std::string& n, const IInterface* p) : AthAlgTool(t,n,p),m_CaloBoundaryR(1148.0),m_CaloBoundaryZ(3549.5),m_calomargin(100)
{
  declareInterface<IFastCaloSimCaloExtrapolation>(this);

  declareProperty("CaloBoundaryR", m_CaloBoundaryR);
  declareProperty("CaloBoundaryZ", m_CaloBoundaryZ);
  declareProperty("CaloMargin", m_calomargin);
  //######################

  declareProperty("Extrapolator",                   m_extrapolator );
  declareProperty("CaloEntrance",                   m_caloEntranceName );
  declareProperty("CaloSurfaceHelper",              m_caloSurfaceHelper );

  declareProperty("CaloGeometryHelper",             m_CaloGeometryHelper );
}

FastCaloSimCaloExtrapolation::~FastCaloSimCaloExtrapolation()
{
}

StatusCode FastCaloSimCaloExtrapolation::initialize() {
  ATH_MSG_INFO( "Initializing FastCaloSimCaloExtrapolation" );
  
  // Get CaloGeometryHelper
  if (m_CaloGeometryHelper.retrieve().isFailure()) {
    ATH_MSG_ERROR("CaloGeometryHelper not found ");
    return StatusCode::FAILURE;
  }

  // Get PDG table
  IPartPropSvc* p_PartPropSvc=0;
  if (service("PartPropSvc",p_PartPropSvc).isFailure() || p_PartPropSvc == 0)
  {
    ATH_MSG_ERROR("could not find PartPropService");
    return StatusCode::FAILURE;
  }
  
  m_particleDataTable = (HepPDT::ParticleDataTable*) p_PartPropSvc->PDT();
  if(m_particleDataTable == 0)
  {
	   ATH_MSG_ERROR("PDG table not found");
    return StatusCode::FAILURE;
  }
  //#########################
  
  // Get TimedExtrapolator 
  if (!m_extrapolator.empty() && m_extrapolator.retrieve().isFailure()) {
	   ATH_MSG_ERROR("Extrapolator retrieval failed");
    return StatusCode::FAILURE;
  } else ATH_MSG_DEBUG("Extrapolator retrieved "<< m_extrapolator);

  // Get CaloSurfaceHelper
  if (m_caloSurfaceHelper.retrieve().isFailure()) {
    ATH_MSG_ERROR("CaloSurfaceHelper not found ");
    return StatusCode::FAILURE;
  }   

  return StatusCode::SUCCESS;
}		 

StatusCode FastCaloSimCaloExtrapolation::finalize() {
  ATH_MSG_INFO( "doing finalize()" );

  return StatusCode::SUCCESS;
}

bool FastCaloSimCaloExtrapolation::isCaloBarrel(int sample) const
{
  return GetCaloGeometry()->isCaloBarrel(sample);
}

double FastCaloSimCaloExtrapolation::deta(int sample,double eta) const
{
  return GetCaloGeometry()->deta(sample,eta);
}

void FastCaloSimCaloExtrapolation::minmaxeta(int sample,double eta,double& mineta,double& maxeta) const 
{
  GetCaloGeometry()->minmaxeta(sample,eta,mineta,maxeta);
}

double FastCaloSimCaloExtrapolation::rmid(int sample,double eta) const 
{
  return GetCaloGeometry()->rmid(sample,eta);
}

double FastCaloSimCaloExtrapolation::zmid(int sample,double eta) const 
{
  return GetCaloGeometry()->zmid(sample,eta);
}

double FastCaloSimCaloExtrapolation::rzmid(int sample,double eta) const
{
  return GetCaloGeometry()->rzmid(sample,eta);
}

double FastCaloSimCaloExtrapolation::rent(int sample,double eta) const 
{
  return GetCaloGeometry()->rent(sample,eta);
}

double FastCaloSimCaloExtrapolation::zent(int sample,double eta) const 
{
  return GetCaloGeometry()->zent(sample,eta);
}

double FastCaloSimCaloExtrapolation::rzent(int sample,double eta) const
{
  return GetCaloGeometry()->rzent(sample,eta);
}

double FastCaloSimCaloExtrapolation::rext(int sample,double eta) const 
{
  return GetCaloGeometry()->rext(sample,eta);
}

double FastCaloSimCaloExtrapolation::zext(int sample,double eta) const 
{
  return GetCaloGeometry()->zext(sample,eta);
}

double FastCaloSimCaloExtrapolation::rzext(int sample,double eta) const
{
  return GetCaloGeometry()->rzext(sample,eta);
}

double FastCaloSimCaloExtrapolation::rpos(int sample,double eta,int subpos) const
{
  return GetCaloGeometry()->rpos(sample,eta,subpos);
}

double FastCaloSimCaloExtrapolation::zpos(int sample,double eta,int subpos) const
{
  return GetCaloGeometry()->zpos(sample,eta,subpos);
}

double FastCaloSimCaloExtrapolation::rzpos(int sample,double eta,int subpos) const
{
  return GetCaloGeometry()->rzpos(sample,eta,subpos);
}


