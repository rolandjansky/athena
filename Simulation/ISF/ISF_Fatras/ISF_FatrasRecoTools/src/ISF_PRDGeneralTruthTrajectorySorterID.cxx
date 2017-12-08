/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FatrasRecoTools/ISF_PRDGeneralTruthTrajectorySorterID.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "ISF_FatrasEvent/PlanarCluster.h"

iFatras::ISF_PRDGeneralTruthTrajectorySorterID::ISF_PRDGeneralTruthTrajectorySorterID(const std::string& t, const std::string& n, const IInterface* p) :
AthAlgTool(t,n,p)
{
declareInterface<Trk::IPRD_TruthTrajectoryManipulator>(this);

}

StatusCode iFatras::ISF_PRDGeneralTruthTrajectorySorterID::initialize() {
  ATH_MSG_VERBOSE("Initializing ...");
  return StatusCode::SUCCESS;
}

StatusCode iFatras::ISF_PRDGeneralTruthTrajectorySorterID::finalize() {
  ATH_MSG_VERBOSE("Finalizing ...");
  return StatusCode::SUCCESS;
}

class mysort{
public:
  bool operator()(const Trk::PrepRawData *prd1,const Trk::PrepRawData *prd2) {
    const Amg::Vector3D* globalPosition_prd1 = ((prd1->detectorElement())->surface(prd1->identify())).localToGlobal(prd1->localPosition());
    const Amg::Vector3D* globalPosition_prd2 = ((prd2->detectorElement())->surface(prd2->identify())).localToGlobal(prd2->localPosition());
    
    //for debugging
    //std::cout << "globalPosition_prd1->mag() = " << globalPosition_prd1->mag() << std::endl;
    //std::cout << "globalPosition_prd2->mag() = " << globalPosition_prd2->mag() << std::endl;
    //std::cout << "globalPosition_prd1->mag() < globalPosition_prd2->mag() = " << (globalPosition_prd1->mag() < globalPosition_prd2->mag()) << std::endl;
        
    return (globalPosition_prd1->mag() < globalPosition_prd2->mag());
  }
};

bool iFatras::ISF_PRDGeneralTruthTrajectorySorterID::manipulateTruthTrajectory( Trk::PRD_TruthTrajectory &prdvec ) const{
  std::stable_sort(prdvec.prds.begin(),prdvec.prds.end(),mysort());
  return true;
}
