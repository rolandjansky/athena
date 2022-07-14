/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SimpleMMClusterBuilderTool_h
#define SimpleMMClusterBuilderTool_h

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MMClusterization/IMMClusterBuilderTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonPrepRawData/MMPrepData.h"

//
// Simple clusterization tool for MicroMegas
//
namespace Muon {

    class SimpleMMClusterBuilderTool : virtual public IMMClusterBuilderTool, public AthAlgTool {
    public:
        SimpleMMClusterBuilderTool(const std::string&, const std::string&, const IInterface*);
        virtual ~SimpleMMClusterBuilderTool() = default;
        
        StatusCode initialize() override;

        StatusCode getClusters(std::vector<Muon::MMPrepData>& stripsVect,
                               std::vector<std::unique_ptr<Muon::MMPrepData>>& clustersVect) const override;

        

        StatusCode getCalibratedClusterPosition(const Muon::MMPrepData* cluster, std::vector<NSWCalib::CalibratedStrip>&, const float theta,
                                                Amg::Vector2D& clusterLocalPosition, Amg::MatrixX& covMatrix) const override;

    private:
        void getClusterPosition(std::vector<Muon::MMPrepData>& stripsVect, Amg::Vector2D& clusterLocalPosition,
                                      Amg::MatrixX& covMatrix) const;
        

        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
        Gaudi::Property<bool> m_writeStripProperties{this, "writeStripProperties", true};
        Gaudi::Property<double> m_scaleClusterError{this, "scaleClusterError", 5.0};
        Gaudi::Property<uint> m_maxHoleSize{this, "maxHoleSize", 1};
    };

}  // namespace Muon
#endif
