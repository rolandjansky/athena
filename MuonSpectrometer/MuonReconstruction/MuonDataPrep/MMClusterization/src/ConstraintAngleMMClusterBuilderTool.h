/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CONSTRIANTANGLEMMCLUSTERBUILDERTOOL_h
#define CONSTRIANTANGLEMMCLUSTERBUILDERTOOL_h

#include <string>
#include <vector>

#include "GaudiKernel/ToolHandle.h"
#include "MMClusterization/IMMClusterBuilderTool.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "AthenaBaseComps/AthAlgTool.h"


namespace Muon{
    class MuonIdHelperTool;
}

namespace Muon{
    class ConstraintAngleMMClusterBuilderTool: virtual public IMMClusterBuilderTool, public AthAlgTool {

        public:
            ConstraintAngleMMClusterBuilderTool(const std::string&, const std::string&, const IInterface*);

            /** Default destructor */
            virtual ~ConstraintAngleMMClusterBuilderTool() = default;

            /** standard initialize method */
            virtual StatusCode initialize() override;


            StatusCode getClusters(std::vector<Muon::MMPrepData>& MMprds, 
			   std::vector<Muon::MMPrepData*>& clustersVec) const;

            
        private:
            ToolHandle<Muon::MuonIdHelperTool> m_muonIdHelperTool;

            StatusCode sortHitsToLayer(const std::vector<Muon::MMPrepData>& MMprds, std::vector<std::vector<Muon::MMPrepData>>& prdsPerLayer) const ;
            StatusCode scanLayer(const std::vector<Muon::MMPrepData> &mmPrdsPerLayer,std::vector<std::vector<uint>> &idxClusters, std::vector<double> &clusterTheta)const ;
            StatusCode fitCluster(const std::vector<Muon::MMPrepData> &prdPerLayer,const std::vector<uint>& idxCluster,const double &clusterTheta,std::vector<Muon::MMPrepData*>& clustersVec) const;

            int m_nSigmaSelection;
            double m_sigmaTheta;
            double m_fitAngleCut;
    };  // class ConstraintAngleMMClusterBuilderTool

}  // namespace Muon

#endif