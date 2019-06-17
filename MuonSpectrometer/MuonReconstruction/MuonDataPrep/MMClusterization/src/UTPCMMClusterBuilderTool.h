/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
    virtual ~UTPCMMClusterBuilderTool() = default;

    /** standard initialize method */
    virtual StatusCode initialize() override;
    
    StatusCode getClusters(std::vector<Muon::MMPrepData>& MMprds, 
			   std::vector<Muon::MMPrepData*>& clustersVec);

  private: 

    /// Muon Detector Descriptor
    const MuonGM::MuonDetectorManager* m_muonMgr;
    const MmIdHelper* m_mmIdHelper;


    // configurable parameters
    Gaudi::Property<float> m_alphaMin{this,"HoughAlphaMin",0,"Min angle for Hough transform"};
    Gaudi::Property<float> m_alphaMax{this,"HoughAlphaMax",90,"Max angle for Hough transform"};
    Gaudi::Property<float> m_alphaResolution{this,"HoughAlphaResolution",.5,"Angle resolution for Hough transform"};
    Gaudi::Property<float> m_dMin{this,"HoughDMin",0,"Hough transform DMin"};
    Gaudi::Property<float> m_dMax{this,"HoughDMax",0,"Hough transform DMax"};
    Gaudi::Property<float> m_dResolution{this,"HoughDResolution",1.0,"Hough transform DResolution"};
    Gaudi::Property<int> m_houghMinCounts{this,"HoughMinCounts",3,"Hough transform min counts"};
    Gaudi::Property<float> m_timeOffset{this,"uTPCTimeOffset",0,"Hough transform time offset"};
    Gaudi::Property<float> m_dHalf{this,"uTPCDHalf",2.5,"DHalf"};
    Gaudi::Property<float> m_vDrift{this,"vDrift",0.048,"Drift velocity"};

    float m_toRad=TMath::Pi()/180.;
    //float m_toRad;


    StatusCode runHoughTrafo(std::vector<int>& flag,std::vector<float>& xpos, std::vector<float>& time,std::vector<int>& idx_selected) const;
    StatusCode fillHoughTrafo(std::unique_ptr<TH2F>& cummulator,std::vector<int>& flag, 
			      std::vector<float>& xpos, std::vector<float>& time, float meanX) const;
    StatusCode houghInitCummulator(std::unique_ptr<TH2F>& cummulator,std::unique_ptr<TH1F>& fineCummulator,
				   float xmax,float xmin,float xmean) const;
    StatusCode doFineScan(std::unique_ptr<TH1F>& fineCummulator,std::vector<int>& flag, std::vector<float>& xpos, 
			  std::vector<float>& time,float amean,float meanX,float& dmean, float& dRMS) const;
    StatusCode transformParameters(float alpha, float d, float dRMS, float& slope,float& intercept, float& interceptRMS) const;
    StatusCode findMaxAlpha(std::unique_ptr<TH2F>& h_hough,float& amean) const;
    StatusCode selectPoints(std::vector<int>& flag,std::vector<float>& xpos, std::vector<float>& time, 
			    float& slope, float& intercept, float& interceptRMS,float& xmean, std::vector<int>& idxSelected ) const;
    StatusCode finalFit(std::vector<float>& xpos, std::vector<float>& time, std::vector<int>& idxSelected,float& x0) const;
};


}
#endif
