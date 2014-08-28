//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


///////////////////////////////////////////////////////////////////
// PAUconvGeoTool.h
// author: Kerstin Tackmann, Thomas Koffas
///////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////

#ifndef PHOTONANALYSISUTILS_CONVGEOTOOL_H
#define PHOTONANALYSISUTILS_CONVGEOTOOL_H


// Gaudi
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"

#include "PhotonAnalysisUtils/IPAUconvGeoTool.h"

#include <string>

namespace InDet
{
  class VertexPointEstimator;
}

namespace Trk
{
  class ITrkDistanceFinder;
}
	
class PAUconvGeoTool : public AthAlgTool, virtual public IPAUconvGeoTool {
  
public:

  /** AlgTool like constructor */
  PAUconvGeoTool(const std::string&,const std::string&,const IInterface*);
  
  /**Virtual destructor*/
  virtual ~PAUconvGeoTool();
  
  /** AlgTool initialize method.*/
  StatusCode initialize();
  /** AlgTool finalize method */
  StatusCode finalize();

  const std::vector<float> convGeoVars(const PAU::egamma* g) const;  
  const std::vector<float> convGeoVars(const Analysis::Photon* g) const;  
  const std::vector<float> convGeoVars(const Trk::VxCandidate* vtx) const;  

private:

  //handle to distance of minimum approach tool
  ToolHandle<Trk::ITrkDistanceFinder> m_distanceTool; 

  //handle to vertex point estimator
  ToolHandle<InDet::VertexPointEstimator> m_vertexEstimator; 

  float dCotTheta(float theta1, float theta2) const;

//   void getStartingPoint(const Trk::MeasuredPerigee *per1, const Trk::MeasuredPerigee *per2, int sit, double& m_MDdr, double& m_ddphi) const;
//   double areaVar(double xc1, double yc1, double r1, double xc2, double yc2, double r2, double& h, double& hl, double &ddphi) const;
//   bool circleIntersection(double xc1, double yc1, double r1, double xc2, double yc2, double r2, double& xi1, double& yi1, double& xi2, double& yi2) const;
//   bool secondDegree(double a, double b, double c, double& y1, double& y2) const;
//   double areaTriangle(double a, double b, double d, double e, double g, double h) const;  




};

#endif // PHOTONANALYSISUTILS_CONVGEOTOOL_H
