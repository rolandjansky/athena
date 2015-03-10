/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBPLANETRACKINGALGO_H
#define TBPLANETRACKINGALGO_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"

#include "TBEvent/TBHitPlaneCont.h"

#include <utility>
#include <vector>

/** @class TBHitPlane TBHitPlane.h "TBEvent/TBHitPlane.h"
This algorithm is used to fit a collection of TBHitPlane (TBHitPlaneCont) 
and create a TBTrack data object.
*/

class TBPlaneTrackingAlgo : public AthAlgorithm {
 public:
  
  /////////////////////////////
  // Contructor & Destructor //
  /////////////////////////////
  TBPlaneTrackingAlgo(const std::string& name, ISvcLocator* pSvcLocator);
  ~TBPlaneTrackingAlgo(){};

  // public methods //
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  void FillRandomHit();
  StatusCode getnewcalib();

  
  // Private function members //

  ////////////////////////////////////////////////////////////////////////
  /// \brief Fit data to the function u = a1 + a2*w. and determines
  ///        intercept, slope, residual for each BPC, and chi2 on fit
  /////////////////////////////////////////////////////////////////////////
  bool fitPlane(const TBHitPlaneCont * hitPlaneCont, double &a1, double &a2,
		double &chi2, std::vector<double> &residual);

  ////////////////////////////////////////////////////////////////////////
  /// \brief Fit data to the function u = a1 + a2*w.
  /////////////////////////////////////////////////////////////////////////
  bool fitHits(const std::vector<double> & u, const std::vector<double> & w,
	       const std::vector<double> & eu, // const std::vector<double> & ew,
	       double &a1, double &a2);

  ////////////////////////////////////////////////////////////////////////
  /// \brief Calculates the residual-> r = (u_i - a1 - a2*w_i)
  /////////////////////////////////////////////////////////////////////////
  double getResidual(const double & u, const double & w,const double &a1,
		     const double &a2);

  ////////////////////////////////////////////////////////////////////////
  /// \brief Calculates the chi2 += ((u_i - a1 - a2*w_i)/eu)^2
  /////////////////////////////////////////////////////////////////////////
  double getChi2(const std::vector<double> &v_u, 
		 const std::vector<double> &v_w,
		 const std::vector<double> &v_eu,
//		 const std::vector<double> &v_ew,
		 const double &a1, const double &a2);

  ////////////////////////////////////////////////////////////////////////
  /// \brief Build HitPlaneCont from BPC
  /////////////////////////////////////////////////////////////////////////
  StatusCode buildHits();
    
  //Used to test this algo set to true if you want to debug
  bool m_testAlgo;
  TBHitPlaneCont m_hitPlaneCont_u;
  TBHitPlaneCont m_hitPlaneCont_v;

  // Positions of BPC needed to construct hits  
  std::vector<float> m_bpc_posZX,m_bpc_posZY,m_bpc_posX,m_bpc_posY;
  std::vector<float> m_bpc_errposZX,m_bpc_errposZY,m_bpc_errposX,m_bpc_errposY;
  std::vector<float> m_bpc_errmeasX,m_bpc_errmeasY;

  std::vector<std::string> m_bpc_names;

  // filename containing positions constants
  std::string m_calib_filename;
  unsigned int m_runnumber;

};

#endif	
