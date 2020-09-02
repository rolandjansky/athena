/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef ProjectionMMClusterBuilderTool_h
#define ProjectionMMClusterBuilderTool_h

#include <numeric>
#include <string>
#include <vector>

#include "MMClusterization/IMMClusterBuilderTool.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

class MmIdHelper;
namespace MuonGM
{
  class MuonDetectorManager;
}

//
// fixed angle projection cluster builder tool for MicroMegas
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
    StatusCode getClusters(std::vector<Muon::MMPrepData>& MMprds, std::vector<std::unique_ptr<Muon::MMPrepData>>& clustersVec) const override;

  private: 

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    bool m_writeStripProperties;

    double m_tmin,m_tmax,m_tOffset;
    double m_p0, m_p1, m_p2;

    int m_t0;

    uint m_minClusterSize;


    StatusCode calculateCorrection(const std::vector<Muon::MMPrepData> &prdsOfLayer,std::vector<double>& v_posxc,std::vector<double>& v_cor) const ;
    StatusCode doFineScan(std::vector<int>& flag,const std::vector<double>& v_posxc, const std::vector<double>& v_cor, std::vector<int>& idx_selected) const ;   
    StatusCode doPositionCalculation(std::vector<double>& v_posxc, const std::vector<double>& v_cor, const std::vector<int>& idx_selected,double& xmean, double& xmeanErr, double &  qtot,const std::vector<Muon::MMPrepData>& prdsOfLayer) const;
    
    StatusCode writeNewPrd(std::vector<std::unique_ptr<Muon::MMPrepData>>& clustersVect,double xmean, double xerr,double qtot,const std::vector<int>& idx_selected,const std::vector<Muon::MMPrepData>& prdsOfLayer) const ;
    
    double getSigma(double correction)const;

};


}
#endif
