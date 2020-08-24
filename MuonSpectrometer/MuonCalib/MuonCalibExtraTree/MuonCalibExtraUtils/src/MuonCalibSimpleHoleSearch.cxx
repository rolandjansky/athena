/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraUtils/MuonCalibSimpleHoleSearch.h"
#include "MuonCalibExtraUtils/MuonCalibSLPropagator.h"
#include "MuonCalibExtraUtils/MuonFixedIdPrinter.h"

#include <iostream>

namespace MuonCalib {

  MuonCalibSimpleGeometry MuonCalibSimpleHoleSearch::m_geometry; 
  MuonCalibSLPropagator   MuonCalibSimpleHoleSearch::m_propagator; 
  MuonFixedIdManipulator  MuonCalibSimpleHoleSearch::m_idManipulator; 
  MuonCalibSimpleHoleSearch::SurfaceIdVec MuonCalibSimpleHoleSearch::m_surfaceIdList; 


  void MuonCalibSimpleHoleSearch::init() {
    if( !m_surfaceIdList.empty() ) return;
    m_surfaceIdList.push_back( std::make_pair( &m_geometry.endcapInnerDiskASide, 
					       m_idManipulator.stationLayerIdentifier(MuonFixedIdManipulator::EIA) ) );
    m_surfaceIdList.push_back( std::make_pair( &m_geometry.endcapMiddleDiskASide, 
					       m_idManipulator.stationLayerIdentifier(MuonFixedIdManipulator::EMA) ) );
    m_surfaceIdList.push_back( std::make_pair( &m_geometry.endcapOuterDiskASide, 
					       m_idManipulator.stationLayerIdentifier(MuonFixedIdManipulator::EOA) ) );

    m_surfaceIdList.push_back( std::make_pair( &m_geometry.endcapInnerDiskCSide, 
					       m_idManipulator.stationLayerIdentifier(MuonFixedIdManipulator::EIC) ) );
    m_surfaceIdList.push_back( std::make_pair( &m_geometry.endcapMiddleDiskCSide, 
					       m_idManipulator.stationLayerIdentifier(MuonFixedIdManipulator::EMC) ) );
    m_surfaceIdList.push_back( std::make_pair( &m_geometry.endcapOuterDiskCSide, 
					       m_idManipulator.stationLayerIdentifier(MuonFixedIdManipulator::EOC) ) );

    m_surfaceIdList.push_back( std::make_pair( &m_geometry.barrelInnerCylinder, 
					       m_idManipulator.stationLayerIdentifier(MuonFixedIdManipulator::BI) ) );
    m_surfaceIdList.push_back( std::make_pair( &m_geometry.barrelMiddleCylinder, 
					       m_idManipulator.stationLayerIdentifier(MuonFixedIdManipulator::BM) ) );
    m_surfaceIdList.push_back( std::make_pair( &m_geometry.barrelOuterCylinder, 
					       m_idManipulator.stationLayerIdentifier(MuonFixedIdManipulator::BO) ) );
					       
  }

  MuonCalibSimpleHoleSearch::ResultVec MuonCalibSimpleHoleSearch::intersectsWithGeometry( const Amg::Vector3D& parPos, 
											  const Amg::Vector3D& parDir, double tolerance ) {
    ResultVec intersectedLayers;
    SurfaceIdIt it = m_surfaceIdList.begin();
    SurfaceIdIt it_end = m_surfaceIdList.end();
    for( ;it!=it_end;++it ){
      searchForSurfaceIntersect(parPos,parDir,*it->first,it->second, intersectedLayers, tolerance );
    }

    return intersectedLayers;
  }

  void MuonCalibSimpleHoleSearch::searchForSurfaceIntersect( const Amg::Vector3D& parPos, const Amg::Vector3D& parDir, 
							     const MuonCalibSurface& surf, const MuonFixedId& stLayerId,
							     ResultVec& intersects, double tolerance ) const {
    // propagate to layer
    Amg::Vector3D pos = m_propagator.propagate(parPos,parDir,surf);

    // check if position is within bounds, if so add intersect
    if( surf.inBounds(pos,tolerance) ) {
      
      bool add = true;
      if( m_idManipulator.isEndcap(stLayerId) ){
	// for the endcap only take intersects on the same side as the track
	double sideTrack =  parPos.z() < 0. ? -1. : 1.;
	double sideInter =  pos.z()    < 0. ? -1. : 1.;
	if(sideTrack!=sideInter) add = false;
      }
      if( add ) intersects.push_back( Result(stLayerId,pos) );
    }
  }
    
}
 
