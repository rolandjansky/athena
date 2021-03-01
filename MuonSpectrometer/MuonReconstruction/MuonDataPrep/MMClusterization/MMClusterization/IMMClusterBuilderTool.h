/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef IMMClusterBuilderTool_h
#define IMMClusterBuilderTool_h

//
// Interface class for MM clustering
//
#include <vector>
#include "GaudiKernel/IAlgTool.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"
#include "NSWCalibTools/INSWCalibTool.h"

namespace Muon {
  class MMPrepData;
}

static const InterfaceID IID_IMMClusterBuilderTool("Muon::IMMClusterBuilderTool", 1, 0);

namespace Muon {

  class IMMClusterBuilderTool : virtual public IAlgTool {
    
  public:    // static methods
    
    static const InterfaceID& interfaceID()  { return IID_IMMClusterBuilderTool; } 
    
  public:    // interface methods
    
    virtual StatusCode getClusters(std::vector<Muon::MMPrepData>& stripsVect, 
				   std::vector<std::unique_ptr<Muon::MMPrepData>>& clustersVect)const =0;


    /// compute the cluster position based on its list of PrepRawData (strips)
    virtual StatusCode getClusterPosition(std::vector<Muon::MMPrepData>& strips, 
					     Amg::Vector2D& clusterLocalPosition, Amg::MatrixX* covMatrix) const;
    
    virtual StatusCode getCalibratedClusterPosition(const Muon::MMPrepData* cluster, std::vector<NSWCalib::CalibratedStrip>&,
						    Amg::Vector2D& clusterLocalPosition, Amg::MatrixX& covMatrix) const;
    
  };

  inline StatusCode IMMClusterBuilderTool::getClusterPosition(std::vector<Muon::MMPrepData>& strips,
							      Amg::Vector2D& clusterLocalPosition, Amg::MatrixX* covMatrix) const
  {
    strips.clear();
    (void)covMatrix;
    clusterLocalPosition = Amg::Vector2D(0.0,0.0);
    return StatusCode::SUCCESS;
  }
  
  inline StatusCode IMMClusterBuilderTool::getCalibratedClusterPosition(const Muon::MMPrepData* cluster, std::vector<NSWCalib::CalibratedStrip>& strips,
									Amg::Vector2D& clusterLocalPosition, Amg::MatrixX& covMatrix) const
  {
    (void)cluster;
    strips.clear();
    covMatrix = Amg::MatrixX(1,1);
    clusterLocalPosition = Amg::Vector2D(0.0,0.0);
    return StatusCode::SUCCESS;
  }




}

#endif
