/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CONSTRIANTANGLEMMCLUSTERBUILDERTOOL_h
#define CONSTRIANTANGLEMMCLUSTERBUILDERTOOL_h

#include <string>
#include <vector>

#include "MMClusterization/IMMClusterBuilderTool.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"


namespace Muon{
    class ConstraintAngleMMClusterBuilderTool: virtual public IMMClusterBuilderTool, public AthAlgTool {

        public:
            ConstraintAngleMMClusterBuilderTool(const std::string&, const std::string&, const IInterface*);

            /** Default destructor */
            virtual ~ConstraintAngleMMClusterBuilderTool() = default;

            /** standard initialize method */
            virtual StatusCode initialize() override;


            virtual
            StatusCode getClusters(std::vector<Muon::MMPrepData>& MMprds, 
			   std::vector<std::unique_ptr<Muon::MMPrepData>>& clustersVec) const override;

            
        private:
            ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
            bool m_writeStripProperties;
            StatusCode sortHitsToLayer(const std::vector<Muon::MMPrepData>& MMprds, std::vector<std::vector<Muon::MMPrepData>>& prdsPerLayer ) const; 
            StatusCode scanLayer(const std::vector<Muon::MMPrepData> &mmPrdsPerLayer,std::vector<std::vector<uint>> &idxClusters, std::vector<double> &clusterTheta)const ;
            StatusCode fitCluster(const std::vector<Muon::MMPrepData> &prdPerLayer,const std::vector<uint>& idxCluster,const double &clusterTheta,std::vector<std::unique_ptr<Muon::MMPrepData>>& clustersVec) const;

            int m_nSigmaSelection;
            double m_sigmaTheta;
            double m_fitAngleCut;
    };  // class ConstraintAngleMMClusterBuilderTool

}  // namespace Muon

#endif
