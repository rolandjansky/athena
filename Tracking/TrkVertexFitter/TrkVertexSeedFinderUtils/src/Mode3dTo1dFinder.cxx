/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
          Mode3dTo1dFinder.cxx - Description in header file
*********************************************************************/

#include "TrkVertexSeedFinderUtils/IMode1dFinder.h"
#include "TrkVertexSeedFinderUtils/Mode3dTo1dFinder.h"
#include "TrkVertexSeedFinderUtils/SeedFinderParamDefs.h"
#include "TrkParameters/TrackParameters.h"
#include <math.h>

namespace Trk
{

  Mode3dTo1dFinder::Mode3dTo1dFinder(const std::string& t, const std::string& n, const IInterface*  p) : 
    AthAlgTool(t,n,p),
    m_mode1dfinder("Trk::FsmwMode1dFinder")
  {   
    declareProperty("Mode1dFinder", m_mode1dfinder);
    declareInterface<IMode3dFinder>(this);
  }

  Mode3dTo1dFinder::~Mode3dTo1dFinder() {}

  StatusCode Mode3dTo1dFinder::initialize() 
  { 
    StatusCode s = AlgTool::initialize();
    if (s.isFailure() )
    {
      msg(MSG::FATAL) << "AlgTool::initialize() initialize failed!" << endmsg;
      return StatusCode::FAILURE;
    }
    if ( m_mode1dfinder.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_mode1dfinder << endmsg;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_mode1dfinder << endmsg;
    }
    msg(MSG::INFO) << "Initialize successfull" << endmsg;
    return StatusCode::SUCCESS;
  }

  StatusCode Mode3dTo1dFinder::finalize() 
  {
    msg(MSG::INFO)  << "Finalize successfull" << endmsg;
    return StatusCode::SUCCESS;
  }

  const Amg::Vector3D Mode3dTo1dFinder::getMode(const std::vector<Trk::PositionAndWeight> & myVectorOfPoints) const {
    
    //create a vector of double values from a vector of "Point" objects
    
    std::vector<Trk::PositionAndWeight>::const_iterator begin = myVectorOfPoints.begin();
    std::vector<Trk::PositionAndWeight>::const_iterator end = myVectorOfPoints.end();
    
    std::vector<Trk::DoubleAndWeight> allx;
    std::vector<Trk::DoubleAndWeight> ally;
    std::vector<Trk::DoubleAndWeight> allz;
    
    for (std::vector<PositionAndWeight>::const_iterator i = begin; i!=end; i++) {
      allx.push_back(Trk::DoubleAndWeight(i->first.x(),i->second));
      ally.push_back(Trk::DoubleAndWeight(i->first.y(),i->second));
      allz.push_back(Trk::DoubleAndWeight(i->first.z(),i->second));
    }
    
    //  now find the mode separately for the distributions in x, y and z
    return Amg::Vector3D(m_mode1dfinder->getMode(allx),
			 m_mode1dfinder->getMode(ally),
			 m_mode1dfinder->getMode(allz));
  }


  //obtain the 3d-mode (position) from a list of positions (distribution in space) - NO WEIGHTS
  const Amg::Vector3D Mode3dTo1dFinder::getMode(const std::vector<Amg::Vector3D> & myVectorOfPoints) const
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


  void Mode3dTo1dFinder::setPriVtxPosition(double /* x */, double /* y */) {
    //implemented to satisfy inheritance
  }

  unsigned int Mode3dTo1dFinder::Modes1d(std::vector<float> &/* a */, std::vector<float> &/* b */, 
					 std::vector<float> &/* c */, std::vector<float> &/* d */){
    //implemented to satisfy inheritance
    return 0;
  }

  const std::vector<int> & Mode3dTo1dFinder::AcceptedCrossingPointsIndices() const{
    //implemented to satisfy inheritance
    return m_acceptedCrossingPoint;
  }

  void Mode3dTo1dFinder::getCorrelationDistance( double &/* cXY */, double &/* cZ */ ){
    //implemented to satisfy inheritance
  }

}


