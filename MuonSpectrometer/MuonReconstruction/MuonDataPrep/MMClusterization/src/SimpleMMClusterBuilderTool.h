/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SimpleMMClusterBuilderTool_h
#define SimpleMMClusterBuilderTool_h

#include "MMClusterization/IMMClusterBuilderTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonPrepRawData/MMPrepData.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include <vector>
#include <string>

//
// Simple clusterization tool for MicroMegas
//
namespace Muon
{
  
  class SimpleMMClusterBuilderTool : virtual public IMMClusterBuilderTool, public AthAlgTool {

  public:
    SimpleMMClusterBuilderTool(const std::string&, const std::string&, const IInterface*);
    virtual ~SimpleMMClusterBuilderTool()=default;
    virtual StatusCode initialize();

    StatusCode getClusters(std::vector<Muon::MMPrepData>& stripsVect, 
			   std::vector<std::unique_ptr<Muon::MMPrepData>>& clustersVect) const;

    StatusCode getClusterPosition(std::vector<Muon::MMPrepData>& stripsVect, 
				  Amg::Vector2D& clusterLocalPosition, Amg::MatrixX* covMatrix) const;
    
    StatusCode getCalibratedClusterPosition(const Muon::MMPrepData* cluster, std::vector<NSWCalib::CalibratedStrip>&, const float theta,
					    Amg::Vector2D& clusterLocalPosition, Amg::MatrixX& covMatrix) const;

  private: 

    /// avoid warning from overriding functions
    using IMMClusterBuilderTool::getClusterPosition;
    using IMMClusterBuilderTool::getCalibratedClusterPosition;

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    bool m_writeStripProperties;

    uint m_maxHoleSize;
    
};


}
#endif
