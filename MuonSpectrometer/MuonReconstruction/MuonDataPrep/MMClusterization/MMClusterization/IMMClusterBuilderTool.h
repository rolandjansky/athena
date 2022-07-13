/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef IMMClusterBuilderTool_h
#define IMMClusterBuilderTool_h

//
// Interface class for MM clustering
//
#include <vector>

#include "EventPrimitives/EventPrimitives.h"
#include "GaudiKernel/IAlgTool.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "NSWCalibTools/INSWCalibTool.h"

namespace Muon {
    class MMPrepData;
}



namespace Muon {

    class IMMClusterBuilderTool : virtual public IAlgTool {
    public:  // static methods
        static const InterfaceID& interfaceID() { 
            static const InterfaceID IID_IMMClusterBuilderTool("Muon::IMMClusterBuilderTool", 1, 0);
            return IID_IMMClusterBuilderTool; 
        }

        virtual StatusCode getClusters(std::vector<Muon::MMPrepData>& stripsVect,
                                       std::vector<std::unique_ptr<Muon::MMPrepData>>& clustersVect) const = 0;
  
        virtual StatusCode getCalibratedClusterPosition(const Muon::MMPrepData* cluster, std::vector<NSWCalib::CalibratedStrip>&,
                                                        const float thetaEstimate, Amg::Vector2D& clusterLocalPosition,
                                                        Amg::MatrixX& covMatrix) const = 0; 
    };   

}  // namespace Muon

#endif
