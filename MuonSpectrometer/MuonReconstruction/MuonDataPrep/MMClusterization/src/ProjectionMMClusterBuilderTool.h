/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef ProjectionMMClusterBuilderTool_h
#define ProjectionMMClusterBuilderTool_h

#include "GaudiKernel/ToolHandle.h"
#include "MMClusterization/IMMClusterBuilderTool.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include <numeric>

#include "TMath.h"



class MmIdHelper;
namespace MuonGM
{
  class MuonDetectorManager;
}

//
// Simple clusterization tool for MicroMegas
//
namespace Muon
{
  
  class ProjectionMMClusterBuilderTool : virtual public IMMClusterBuilderTool, public AthAlgTool {

  public:
    /** Default constructor */
    ProjectionMMClusterBuilderTool(const std::string&, const std::string&, const IInterface*);
     
    /** Default destructor */
    virtual ~ProjectionMMClusterBuilderTool()=default;

    /** standard initialize method */
    virtual StatusCode initialize() override;
    
    /**Interface fuction to IMMClusterBuilderTool; calling function manages the pointers inside clustersVec  */
    virtual
    StatusCode getClusters(std::vector<Muon::MMPrepData>& MMprds, std::vector<Muon::MMPrepData*>& clustersVec) override;

  private: 

    /// Muon Detector Descriptor
    const MuonGM::MuonDetectorManager* m_muonMgr;
    const MmIdHelper* m_mmIdHelper;

    double m_vDrift,m_tmin,m_tmax;
    double m_p0,m_p1,m_p2; //correction factors for charge dependence


    StatusCode calculateCorrection(const std::vector<Muon::MMPrepData> &prdsOfLayer,std::vector<double>& v_posxc,std::vector<double>& v_cor);
    StatusCode doFineScan(std::vector<int>& flag,const std::vector<double>& v_posxc, const std::vector<double>& v_cor, std::vector<int>& idx_selected);   
    StatusCode doPositionCalculation(std::vector<double>& v_posxc, const std::vector<double>& v_cor, const std::vector<int> idx_selected,double& xmean, double& xmeanErr, double &  qtot,const std::vector<Muon::MMPrepData>& prdsOfLayer);
    
    StatusCode writeNewPrd(std::vector<Muon::MMPrepData*>& clustersVect,double xmean, double xerr,double qtot,const std::vector<int>& idx_selected,const std::vector<Muon::MMPrepData>& prdsOfLayer);
    
    double getSigma(double correction);

};


}
#endif
