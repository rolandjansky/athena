/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef UTPCMMClusterBuilderTool_h
#define UTPCMMClusterBuilderTool_h

#include <tuple>
#include <vector>
#include <memory>
#include <string>

#include "MMClusterization/IMMClusterBuilderTool.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include "TH2D.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TLinearFitter.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TMatrixDSym.h"

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

    virtual
    StatusCode getClusters(std::vector<Muon::MMPrepData>& MMprds, 
	 		   std::vector<std::unique_ptr<Muon::MMPrepData>>& clustersVec)const override ;

  private:

    /// Muon Detector Descriptor
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    bool m_writeStripProperties;

    // params for the hough trafo
    double m_alphaMin,m_alphaMax,m_alphaResolution,m_selectionCut;
    double m_dMin,m_dMax,m_dResolution,m_driftRange;
    int m_houghMinCounts;

    double m_outerChargeRatioCut;
    int m_maxStripsCut;

    bool m_digiHasNegativeAngles;
    float m_scaleClusterError;

    StatusCode runHoughTrafo(const std::vector<Muon::MMPrepData> &mmPrd, std::vector<double>& xpos, std::vector<int>& flag, std::vector<int>& idx_selected)const;
    StatusCode fillHoughTrafo(const std::vector<Muon::MMPrepData> &mmPrd, std::vector<double>& xpos, std::vector<int>& flag,  std::unique_ptr<TH2D>& h_hough)const;
    StatusCode houghInitCummulator(std::unique_ptr<TH2D>& cummulator, double xmax, double xmin)const;

    StatusCode findAlphaMax(std::unique_ptr<TH2D>& h_hough, std::vector<std::tuple<double,double>> &maxPos)const;
    StatusCode selectTrack(const std::vector<Muon::MMPrepData> &mmPrd, std::vector<double>& xpos, std::vector<int>& flag, std::vector<std::tuple<double,double>> &tracks, std::vector<int> &idxGoodStrips)const;

    StatusCode transformParameters(double alpha, double d, double dRMS, double& slope, double& intercept, double& interceptRMS)const;
    StatusCode applyCrossTalkCut(std::vector<int> &idxSelected,const std::vector<MMPrepData> &MMPrdsOfLayer,std::vector<int> &flag,int &nStripsCut)const;
    StatusCode finalFit(const std::vector<Muon::MMPrepData> &mmPrd, std::vector<int>& idxSelected,double& x0, double &sigmaX0, double &fitAngle, double &chiSqProb)const;
  };


} //  namespace Muon
#endif
