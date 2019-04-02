/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef UTPCMMClusterBuilderTool_h
#define UTPCMMClusterBuilderTool_h

#include "GaudiKernel/ToolHandle.h"
#include "MMClusterization/IMMClusterBuilderTool.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include <numeric>

#include "TH2F.h"
#include "TMath.h"
#include "TF1.h"
#include "TGraphErrors.h"

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
  
  class UTPCMMClusterBuilderTool : virtual public IMMClusterBuilderTool, public AthAlgTool {

  public:
    /** Default constructor */
    UTPCMMClusterBuilderTool(const std::string&, const std::string&, const IInterface*);
     
    /** Default destructor */
    virtual ~UTPCMMClusterBuilderTool();

    /** standard initialize method */
    virtual StatusCode initialize();
    
    /** standard finalize method */
    virtual StatusCode finalize();

    StatusCode getClusters(std::vector<Muon::MMPrepData>& MMprds, 
			   std::vector<Muon::MMPrepData*>& clustersVec);

  private: 

    /// Muon Detector Descriptor
    const MuonGM::MuonDetectorManager* m_muonMgr;
    const MmIdHelper* m_mmIdHelper;


    // params for the hough trafo
    float m_alphaMin,m_alphaMax,m_alphaResolution;
    float m_dMin,m_dMax,m_dResolution;
    int m_houghMinCounts;

    float m_timeOffset,m_dHalf,m_vDrift;

    //    float m_toRad=TMath::Pi()/180.;
    float m_toRad;


    StatusCode runHoughTrafo(std::vector<float>& xpos, std::vector<float>& time,std::vector<int>& idx_selected);
    StatusCode fillHoughTrafo(TH2F* cummulator,std::vector<float>& xpos, std::vector<float>& time, float meanX);
    StatusCode houghInitCummulator(TH2F* cummulator,TH1F* fineCummulator,float xmax,float xmin,float xmean);
    StatusCode doFineScan(TH1F* fineCummulator,std::vector<float>& xpos, std::vector<float>& time,float amean,float meanX,float& dmean, float& dRMS);
    StatusCode transformParameters(float alpha, float d, float dRMS, float& slope,float& intercept, float& interceptRMS);
    StatusCode findMaxAlpha(TH2F* h_hough,float& amean);
    StatusCode selectPoints(std::vector<float>& xpos, std::vector<float>& time, float& slope, float& intercept, float& interceptRMS,float& xmean, std::vector<int>& idxSelected );
    StatusCode finalFit(std::vector<float>& xpos, std::vector<float>& time, std::vector<int>& idxSelected,float& x0);
};


}
#endif
