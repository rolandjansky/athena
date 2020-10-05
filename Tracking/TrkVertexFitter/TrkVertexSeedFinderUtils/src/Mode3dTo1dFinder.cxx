/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
          Mode3dTo1dFinder.cxx - Description in header file
*********************************************************************/

#include "TrkVertexSeedFinderUtils/Mode3dTo1dFinder.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkVertexSeedFinderUtils/IMode1dFinder.h"
#include "TrkVertexSeedFinderUtils/SeedFinderParamDefs.h"
#include <cmath>

namespace Trk
{

  Mode3dTo1dFinder::Mode3dTo1dFinder(const std::string& t, const std::string& n, const IInterface*  p) : 
    base_class(t,n,p),
    m_mode1dfinder("Trk::FsmwMode1dFinder", this)
  {   
    declareProperty("Mode1dFinder", m_mode1dfinder);
  }


  StatusCode Mode3dTo1dFinder::initialize() 
  { 
    ATH_CHECK( AlgTool::initialize() );
    ATH_CHECK( m_mode1dfinder.retrieve() );
    ATH_MSG_INFO( "Initialize successfull"  );
    return StatusCode::SUCCESS;
  }


  StatusCode Mode3dTo1dFinder::finalize() 
  {
    ATH_MSG_INFO( "Finalize successfull"  );
    return StatusCode::SUCCESS;
  }


  Amg::Vector3D
  Mode3dTo1dFinder::getMode(const double /*vx*/,
                            const double /*vy*/,
                            const std::vector<Trk::PositionAndWeight> & myVectorOfPoints) const
  {
    //create a vector of double values from a vector of "Point" objects
    
    std::vector<Trk::PositionAndWeight>::const_iterator begin = myVectorOfPoints.begin();
    std::vector<Trk::PositionAndWeight>::const_iterator end = myVectorOfPoints.end();
    
    std::vector<Trk::DoubleAndWeight> allx;
    std::vector<Trk::DoubleAndWeight> ally;
    std::vector<Trk::DoubleAndWeight> allz;
    
    for (std::vector<PositionAndWeight>::const_iterator i = begin; i!=end; i++) {
      allx.emplace_back(i->first.x(),i->second);
      ally.emplace_back(i->first.y(),i->second);
      allz.emplace_back(i->first.z(),i->second);
    }
    
    //  now find the mode separately for the distributions in x, y and z
    return Amg::Vector3D(m_mode1dfinder->getMode(allx),
			 m_mode1dfinder->getMode(ally),
			 m_mode1dfinder->getMode(allz));
  }


  Amg::Vector3D
  Mode3dTo1dFinder::getMode(const double vx,
                            const double vy,
                            const std::vector<Trk::PositionAndWeight> & myVectorOfPoints,
                            std::unique_ptr<IMode3dInfo>& /*info*/) const
  {
    return getMode (vx, vy, myVectorOfPoints);
  }


  //obtain the 3d-mode (position) from a list of positions (distribution in space) - NO WEIGHTS
  Amg::Vector3D
  Mode3dTo1dFinder::getMode(const double /*vx*/,
                            const double /*vy*/,
                            const std::vector<Amg::Vector3D> & myVectorOfPoints) const
  {
    //create a vector of double values from a vector of "Point" objects
    
    std::vector<Amg::Vector3D>::const_iterator begin = myVectorOfPoints.begin();
    std::vector<Amg::Vector3D>::const_iterator end = myVectorOfPoints.end();
    
    std::vector<double> allx;
    std::vector<double> ally;
    std::vector<double> allz;
    
    for (std::vector<Amg::Vector3D>::const_iterator i = begin; i!=end; i++) {
      allx.push_back((*i).x());
      ally.push_back((*i).y());
      allz.push_back((*i).z());
    }
    
    //  now find the mode separately for the distributions in x, y and z
    return Amg::Vector3D(m_mode1dfinder->getMode(allx),
			 m_mode1dfinder->getMode(ally),
			 m_mode1dfinder->getMode(allz));
  }


  Amg::Vector3D
  Mode3dTo1dFinder::getMode(const double vx,
                            const double vy,
                            const std::vector<Amg::Vector3D> & myVectorOfPoints,
                            std::unique_ptr<IMode3dInfo>& /*info*/) const
                            
  {
    return getMode (vx, vy, myVectorOfPoints);
  }


}
