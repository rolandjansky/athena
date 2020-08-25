/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// CaloSurfacaHelper.cxx, (c) ATLAS detector software
//****************************************************************************

#include "CaloTrackingGeometry/CaloSurfaceHelper.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "CaloTrackingGeometry/ICaloSurfaceBuilder.h"
#include <vector>

// Amg
#include "GeoPrimitives/GeoPrimitives.h"

#include "TrkDetDescrInterfaces/ITrackingVolumeBuilder.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"

CaloSurfaceHelper::CaloSurfaceHelper(const std::string& type,
				       const std::string& name,
				       const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_surfBuilder("CaloSurfaceBuilder")
  //m_mbtsSurfs()
{
  declareInterface<ICaloSurfaceHelper>( this );

  declareProperty ("CaloSurfaceBuilder",         m_surfBuilder);

  //m_mbtsSurfs=std::pair<const Trk::Surface*,const Trk::Surface*> (0,0); 
}

CaloSurfaceHelper::~CaloSurfaceHelper()
{
  //delete m_mbtsSurfs.first; 
  //delete m_mbtsSurfs.second; 
}

StatusCode
CaloSurfaceHelper::initialize()
{
  
  if(m_surfBuilder.retrieve().isFailure()) {
    ATH_MSG_FATAL(  "Failed to retrieve tool " << m_surfBuilder );
    return StatusCode::FAILURE;
  } else ATH_MSG_INFO( "Retrieved tool " << m_surfBuilder );

  // load surfaces
  m_entrySurfs = m_surfBuilder->entrySurfaces();
  m_exitSurfs  = m_surfBuilder->exitSurfaces();

  get_flat_surfaces();

  return StatusCode::SUCCESS;
}

StatusCode
CaloSurfaceHelper::finalize()
{
  for (unsigned int i=0; i<m_flatEntrySurfs.size(); i++) {
    delete m_flatEntrySurfs[i].first;
    delete m_flatEntrySurfs[i].second;
  }

  return StatusCode::SUCCESS;
}

const Trk::Surface&  CaloSurfaceHelper::GetFlatEntrySurface (const CaloCell_ID::CaloSample sample, int side) {

  const Trk::Surface* surf = side>0 ? m_flatEntrySurfs[sample].first : m_flatEntrySurfs[sample].second;

  if (!surf) ATH_MSG_FATAL(" failed to retrieve flat entry layer for sample:"<<sample<<" on side:"<<side<<" crashing....");
  return *surf;
}

const Trk::Surface& CaloSurfaceHelper::GetEntrySurface(const CaloCell_ID::CaloSample sample, int side) {

  const Trk::Surface* surf = side>0 ? m_entrySurfs[sample].first : m_entrySurfs[sample].second;

  if (!surf) ATH_MSG_FATAL(" failed to retrieve entry layer for sample:"<<sample<<" on side:"<<side<<" crashing....");
  return *surf;
}

const Trk::Surface& CaloSurfaceHelper::GetExitSurface (const CaloCell_ID::CaloSample sample, int side) {

  const Trk::Surface* surf = side>0 ? m_exitSurfs[sample].first : m_exitSurfs[sample].second;

  if (!surf) ATH_MSG_FATAL(" failed to retrieve exit layer for sample:"<<sample<<" on side:"<<side<<" crashing....");
  return *surf;
}

#if 0
const Trk::Surface& CaloSurfaceHelper::GetMBTSSurface(int side) {

  const Trk::Surface* surf = side>0 ? m_mbtsSurfs.first : m_mbtsSurfs.second;

  if (!surf) ATH_MSG_FATAL(" failed to retrieve MBTS surface on side:"<<side<<" crashing....");
  return *surf;

}
#endif

void CaloSurfaceHelper::get_flat_surfaces() {

  // copy to get rid of sliding surfaces

  for (unsigned int i=0; i < m_entrySurfs.size(); i++) {
    // cloning turns sliding surface into "standard" one
    Trk::Surface* surf1=m_entrySurfs[i].first ? m_entrySurfs[i].first->clone() : 0;        
    Trk::Surface* surf2=m_entrySurfs[i].second ? m_entrySurfs[i].second->clone() : 0;   
    m_flatEntrySurfs.push_back(std::pair<const Trk::Surface*,const Trk::Surface*>(surf1,surf2));

  }

}
