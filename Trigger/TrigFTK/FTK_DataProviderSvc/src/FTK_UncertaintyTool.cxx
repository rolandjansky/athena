/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration 
*/

////////////////////////////////////////////////////////////////////////////////
// FTK_UncertaintyTool tool
// -------------------------------
// ATLAS Collaboration
//
// Store and return the parametrized uncertianties for the FTK tracks   
//
// 17.07.2013 Package created
//
// Author: John Alison, UChicago
// e-mail: John.Alison@cern.ch
//
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>
#include "GaudiKernel/MsgStream.h"

#include "FTK_DataProviderSvc/FTK_UncertaintyTool.h"

using std::map; using std::string;
using std::cout; using std::endl;

namespace {
  struct FastSimParameters
  {
    double amplCore;
    double meanCore;
    double aCore;
    double bCore;
    double amplTail;
    double meanTail;
    double aTail;
    double bTail;
  };

  // Parameters in 2 IBL bins (hit/no hit) and 5 eta bins (0, 0.5, 1.0, 1.5, 2.0, 2.5)
  using FastSimParameterSet = const FastSimParameters[2][5];

  inline int EtaBin(double eta)
  {
    const double abseta = abs(eta);

    if (abseta > 2.5)
      return 4;

    return static_cast<int>(abseta * 2);
  }

  inline int IBLBin(bool hasIBLHit)
  {
    return hasIBLHit ? 0 : 1;
  }

  inline double sigma(double a, double b, double invPt2)
  {
    return sqrt(a + b * (invPt2*invPt2));
  }

  double GetError(bool hasIBLHit, double eta, double invPt, FastSimParameterSet& parameters)
  {
    const int etabin = EtaBin(eta);
    const int iblbin = IBLBin(hasIBLHit);

    auto& params = parameters[iblbin][etabin];

    const double invPt2 = 0.5 * invPt;

    const double sigmaCore = sigma(params.aCore, params.bCore, invPt2);
    const double sigmaTail = sigma(params.aTail, params.bTail, invPt2);

    return (params.amplCore * sigmaCore + params.amplTail * sigmaTail) / (params.amplCore + params.amplTail);
  }

  const FastSimParameterSet FastSimParameters_d0 = {
    {
      {1.000000e+00, 0.000000e+00, 5.700200e-04, 5.437944e+04, 1.100000e-01, 0.000000e+00, 1.666591e-03, 3.586450e+05},
      {1.000000e+00, 0.000000e+00, 4.844049e-04, 7.070505e+04, 1.100000e-01, 0.000000e+00, 2.693387e-03, 4.568432e+05},
      {1.000000e+00, 0.000000e+00, 5.435634e-04, 1.064718e+05, 1.100000e-01, 0.000000e+00, 3.731382e-03, 6.174656e+05},
      {1.000000e+00, 0.000000e+00, 9.325689e-04, 1.803775e+05, 1.100000e-01, 0.000000e+00, 6.702443e-03, 1.330276e+06},
      {1.000000e+00, 0.000000e+00, 1.525671e-03, 3.459623e+05, 1.100000e-01, 0.000000e+00, 1.674857e-02, 3.133805e+06},
    },
    {
      {1.000000e+00, 0.000000e+00, 3.365111e-04, 1.846811e+04, 1.100000e-01, 0.000000e+00, 1.164449e-03, 5.983721e+04},
      {1.000000e+00, 0.000000e+00, 3.186037e-04, 2.331183e+04, 1.100000e-01, 0.000000e+00, 1.282122e-03, 7.966916e+04},
      {1.000000e+00, 0.000000e+00, 4.012198e-04, 3.172024e+04, 1.100000e-01, 0.000000e+00, 1.644470e-03, 1.205381e+05},
      {1.000000e+00, 0.000000e+00, 5.482587e-04, 4.827720e+04, 1.100000e-01, 0.000000e+00, 2.607949e-03, 1.921344e+05},
      {1.000000e+00, 0.000000e+00, 8.326011e-04, 7.892970e+04, 1.100000e-01, 0.000000e+00, 6.407767e-03, 3.508604e+05},
    },
  };

  const FastSimParameterSet FastSimParameters_z0 = {
    {
      {1.000000e+00, 0.000000e+00, 1.962677e-02, 9.086464e+04, 1.100000e-01, 0.000000e+00, 4.075973e-02, 1.726567e+06 },
      {1.000000e+00, 0.000000e+00, 1.562178e-02, 2.516894e+05, 1.100000e-01, 0.000000e+00, 4.164214e-02, 1.302287e+06 },
      {1.000000e+00, 0.000000e+00, 1.815425e-02, 5.192081e+05, 1.100000e-01, 0.000000e+00, 6.684842e-02, 2.826332e+06 },
      {1.000000e+00, 0.000000e+00, 4.166662e-02, 1.631462e+06, 1.100000e-01, 0.000000e+00, 2.141205e-01, 1.240381e+07 },
      {1.000000e+00, 0.000000e+00, 1.013725e-01, 8.650727e+06, 1.100000e-01, 0.000000e+00, 8.230914e-01, 8.169051e+07 },
    },
    {
      {1.000000e+00, 0.000000e+00, 6.253207e-03, 5.240011e+04, 1.100000e-01, 0.000000e+00, 7.939942e-03, 1.517791e+05 },
      {1.000000e+00, 0.000000e+00, 3.936087e-03, 7.066114e+04, 1.100000e-01, 0.000000e+00, 8.974887e-03, 2.283212e+05 },
      {1.000000e+00, 0.000000e+00, 8.602167e-03, 1.869958e+05, 1.100000e-01, 0.000000e+00, 2.367743e-02, 6.415471e+05 },
      {1.000000e+00, 0.000000e+00, 1.694396e-02, 5.427136e+05, 1.100000e-01, 0.000000e+00, 6.771001e-02, 1.933855e+06 },
      {1.000000e+00, 0.000000e+00, 5.482604e-02, 1.907271e+06, 1.100000e-01, 0.000000e+00, 2.969994e-01, 8.139293e+06 },
    },
  };

  const FastSimParameterSet FastSimParameters_eta = {
    {
      {1.000000e+00, 0.000000e+00, 1.204840e-06, 4.668120e+01, 1.100000e-01, 0.000000e+00, 2.043563e-06, 2.295754e+02 },
      {1.000000e+00, 0.000000e+00, 6.419600e-07, 5.133359e+01, 1.100000e-01, 0.000000e+00, 1.496217e-06, 2.521133e+02 },
      {1.000000e+00, 0.000000e+00, 5.447665e-07, 6.166226e+01, 1.100000e-01, 0.000000e+00, 1.546516e-06, 2.755418e+02 },
      {1.000000e+00, 0.000000e+00, 5.665617e-07, 9.508760e+01, 1.100000e-01, 0.000000e+00, 3.171946e-06, 4.840149e+02 },
      {1.000000e+00, 0.000000e+00, 9.373726e-07, 1.919797e+02, 1.100000e-01, 0.000000e+00, 6.897586e-06, 1.026491e+03 },
    },
    {
      {1.000000e+00, 0.000000e+00, 7.496859e-07, 3.301107e+01, 1.100000e-01, 0.000000e+00, 1.642214e-06, 1.016009e+02 },
      {1.000000e+00, 0.000000e+00, 4.564739e-07, 3.304368e+01, 1.100000e-01, 0.000000e+00, 1.254484e-06, 9.852592e+01 },
      {1.000000e+00, 0.000000e+00, 4.366910e-07, 4.449627e+01, 1.100000e-01, 0.000000e+00, 1.937983e-06, 1.249774e+02 },
      {1.000000e+00, 0.000000e+00, 5.777798e-07, 5.663534e+01, 1.100000e-01, 0.000000e+00, 2.589830e-06, 1.762125e+02 },
      {1.000000e+00, 0.000000e+00, 9.363907e-07, 7.985674e+01, 1.100000e-01, 0.000000e+00, 5.036494e-06, 2.844372e+02 },
    },
  };

  const FastSimParameterSet FastSimParameters_phi = {
    {
      {1.000000e+00, 0.000000e+00, 8.130652e-08, 3.158970e+01, 1.100000e-01, 0.000000e+00, 5.350961e-07, 1.538612e+02 },
      {1.000000e+00, 0.000000e+00, 1.083964e-07, 4.038393e+01, 1.100000e-01, 0.000000e+00, 4.313475e-07, 2.150540e+02 },
      {1.000000e+00, 0.000000e+00, 1.285549e-07, 6.095933e+01, 1.100000e-01, 0.000000e+00, 7.242223e-07, 2.976441e+02 },
      {1.000000e+00, 0.000000e+00, 3.031775e-07, 1.054400e+02, 1.100000e-01, 0.000000e+00, 2.989829e-06, 6.108252e+02 },
      {1.000000e+00, 0.000000e+00, 5.462634e-07, 1.970348e+02, 1.100000e-01, 0.000000e+00, 8.746469e-06, 1.054684e+03 },
    },
    {
      {1.000000e+00, 0.000000e+00, 1.429275e-07, 1.833850e+01, 1.100000e-01, 0.000000e+00, 6.376208e-07, 5.786213e+01 },
      {1.000000e+00, 0.000000e+00, 1.498748e-07, 2.295547e+01, 1.100000e-01, 0.000000e+00, 7.434808e-07, 7.394416e+01 },
      {1.000000e+00, 0.000000e+00, 2.110908e-07, 3.174038e+01, 1.100000e-01, 0.000000e+00, 9.643213e-07, 1.106054e+02 },
      {1.000000e+00, 0.000000e+00, 3.251302e-07, 4.917401e+01, 1.100000e-01, 0.000000e+00, 2.177912e-06, 1.753651e+02 },
      {1.000000e+00, 0.000000e+00, 5.438376e-07, 7.807236e+01, 1.100000e-01, 0.000000e+00, 3.982290e-06, 3.344589e+02 },
    },
  };

  const FastSimParameterSet FastSimParameters_Ipt = {
    {
      {1.000000e+00, 0.000000e+00, 2.176247e-12, 3.372455e-04, 1.100000e-01, 0.000000e+00, 1.619578e-11, 1.297069e-03 },
      {1.000000e+00, 0.000000e+00, 2.515409e-12, 4.173315e-04, 1.100000e-01, 0.000000e+00, 1.640457e-11, 1.934070e-03 },
      {1.000000e+00, 0.000000e+00, 2.969128e-12, 8.230829e-04, 1.100000e-01, 0.000000e+00, 3.165478e-11, 3.211156e-03 },
      {1.000000e+00, 0.000000e+00, 1.241242e-11, 1.492929e-03, 1.100000e-01, 0.000000e+00, 1.667900e-10, 5.584656e-03 },
      {1.000000e+00, 0.000000e+00, 1.853669e-11, 2.362025e-03, 1.100000e-01, 0.000000e+00, 2.258558e-10, 9.794419e-03 },
    },
    {
      {1.000000e+00, 0.000000e+00, 1.826416e-12, 3.552206e-04, 1.100000e-01, 0.000000e+00, 8.161897e-12, 1.196878e-03 },
      {1.000000e+00, 0.000000e+00, 1.934396e-12, 4.709845e-04, 1.100000e-01, 0.000000e+00, 8.623748e-12, 1.524293e-03 },
      {1.000000e+00, 0.000000e+00, 3.033192e-12, 8.394077e-04, 1.100000e-01, 0.000000e+00, 1.796648e-11, 2.951569e-03 },
      {1.000000e+00, 0.000000e+00, 9.374058e-12, 1.578456e-03, 1.100000e-01, 0.000000e+00, 9.828657e-11, 4.168337e-03 },
      {1.000000e+00, 0.000000e+00, 1.328649e-11, 2.343957e-03, 1.100000e-01, 0.000000e+00, 1.007092e-10, 8.286561e-03 },
    },
  };
}


FTK_UncertaintyTool::FTK_UncertaintyTool(const std::string& t, 
					       const std::string& n,
					       const IInterface*  p ): 
  AthAlgTool(t,n,p),
  m_noIBL(false),
  m_ftkparversion("DEC2017_V1")
{
  declareInterface< IFTK_UncertaintyTool >( this );
  declareProperty( "NoIBL",  m_noIBL);
  declareProperty( "ftkparversion",  m_ftkparversion);
}

StatusCode FTK_UncertaintyTool::initialize() {

  StatusCode sc = AlgTool::initialize();

  //
  //   Load Constants
  //
  if(m_noIBL){
    LoadConstants_NoIBL();
  }
  else{
    if(m_ftkparversion == "LEGACY"){
      LoadConstants();
    }
    else if(m_ftkparversion == "DEC2017_V1"){
      LoadConstants_DEC2017_V1();
    }
    else{
      ATH_MSG_WARNING("m_ftkparversion not supported, reverting to default DEC2017_V1 parameters");
      LoadConstants_DEC2017_V1();
    }
  }


  ATH_MSG_INFO("FTK_UncertaintyTool initialized ");
  return sc;
}

StatusCode FTK_UncertaintyTool::finalize() {
  StatusCode sc = AlgTool::finalize(); 
  return sc;
}

//
// Covariance Matrix if there is a BLayer Hit
//
double FTK_UncertaintyTool::getParamCovMtx(const FTK_RawTrack &trk, bool hasIBL, int id0, int id1)
{
  ATH_MSG_VERBOSE("In getParamCovMtx: id0: " << id0 << " id1: " << id1); 


  //
  // Use diagonal Maxtrix for now
  //
  if (id0!=id1) {
    return 0.;
  }

  ATH_MSG_DEBUG("FTK_UncertaintyTool:: has BL " << hasIBL); 
  
  double trkIpt = trk.getInvPt();  
  double trkTheta = atan2(1.0,trk.getCotTh());
  double trkEta = -log(tan(trkTheta/2));

  ATH_MSG_VERBOSE("FTK_UncertaintyTool:: trkIpt " << trkIpt);
  ATH_MSG_VERBOSE("FTK_UncertaintyTool:: trkEta " << trkEta); 

  double sigmaTP = -1.0;

  //
  //  Uncertainties are stored using eta and 1/pt
  //
  int lookUpParam = id0;

  //   if theta is asked for, fetch eta and then convert later
  if(id0 == FTKTrackParam::theta){
    lookUpParam = FTKTrackParam::eta;
  }

  //   if q/P or pt are asked for, fetch Ipt and then convert later
  if(id0 == FTKTrackParam::qOp || id0 == FTKTrackParam::pt){
    lookUpParam = FTKTrackParam::Ipt;
  }


  //
  // square root model
  //
  if(m_allConsts[hasIBL].mode(lookUpParam,trkEta) == FTK_UncertaintyTool::sqroot){
    sigmaTP = sqrt(m_allConsts[hasIBL].par0(lookUpParam,trkEta)+m_allConsts[hasIBL].par1(lookUpParam,trkEta)*trkIpt*trkIpt);
    ATH_MSG_DEBUG("FTK_UncertaintyTool:: sigmaTP ("   
		  << sigmaTP <<") = sqrt("<<m_allConsts[hasIBL].par0(lookUpParam,trkEta) 
		  << "+" << m_allConsts[hasIBL].par1(lookUpParam,trkEta) << "*" << trkIpt << "**2)");
    ATH_MSG_DEBUG("FTK_UncertaintyTool:: (sqrt)cov "   << sigmaTP*sigmaTP); 
    
    
  //
  // linear model
  //
  }else{
    sigmaTP = m_allConsts[hasIBL].par0(lookUpParam,trkEta) + m_allConsts[hasIBL].par1(lookUpParam,trkEta)*fabs(trkIpt);
    ATH_MSG_DEBUG("FTK_UncertaintyTool:: sigmaTP ("   
		  << sigmaTP <<") = "<<m_allConsts[hasIBL].par0(lookUpParam,trkEta) 
		  << "+" << m_allConsts[hasIBL].par1(lookUpParam,trkEta) << "*" << fabs(trkIpt)); 
    ATH_MSG_DEBUG("FTK_UncertaintyTool:: (linear)cov "   << sigmaTP*sigmaTP); 
  }

  if(sigmaTP < 0)
    ATH_MSG_ERROR(" sigma TP" << sigmaTP);

  //
  // Convert eta to theta
  //
  if(id0 == FTKTrackParam::theta){
    double sigmaTheta = getSigmaTheta(trkEta, sigmaTP);
    sigmaTP    = sigmaTheta;
  }

  //
  // Convert 1/2pt  to qoverp
  //
  if(id0 == FTKTrackParam::qOp){
    double sigmaEta     = m_allConsts[hasIBL].par0(FTKTrackParam::eta,trkEta)+m_allConsts[hasIBL].par1(FTKTrackParam::eta,trkEta)*fabs(trkIpt);
    double sigmaQoverP  = getSigmaQoverP(trkIpt, sigmaTP, trkEta, sigmaEta );
    sigmaTP             = sigmaQoverP;
  }

  //
  // Convert 1/2pt  to pt
  //
  if(id0 == FTKTrackParam::pt){
    double sigmaPt      = getSigmaPt(trkIpt, sigmaTP);
    sigmaTP             = sigmaPt;
  }

  
  return sigmaTP*sigmaTP;

}



//
// Get sigma(Q/P) from sigma(1/2pT) and sigma(eta) 
//    (NOTE: invpt = 1/2pT)
double FTK_UncertaintyTool::getSigmaQoverP(double invpt, double sigmaTwoInvPt, double eta,  double sigmaEta)
{
  //
  // (Might not be necessary, but lets to it right.)
  //  
  //   We have paramaterized the uncertianty of 1/2xPt and eta
  //    (NOTE: invpt = 1/2pT)
  //
  //
  // But we want sigma q/p := "f"
  //   define sigmaTwoInvPt := "g"
  //
  //   f = 2g/cosh(eta)
  //  
  // (After calculus) 
  //
  //  sigma(f) = 2/cosh(eta) x sqrt( sigma(g)^2 + (gxtanh(eta))^2 )
  //
  
  double secondTerm = invpt*tanh(eta)*sigmaEta;
  double coshEta    = cosh(eta); 
  double sigmaQoverP = coshEta ? 2./cosh(eta) * sqrt( sigmaTwoInvPt*sigmaTwoInvPt + secondTerm*secondTerm) : 10 ;
  return sigmaQoverP;
}

//
// Get sigma(Pt) from sigma(1/2pT) 
//    (NOTE: invpt = 1/2pT)
double FTK_UncertaintyTool::getSigmaPt(double invpt, double sigmaTwoInvPt)
{
  //
  // (Might not be necessary, but lets to it right.)
  //  
  //   We have paramaterized the uncertianty of 1/2xPt
  //    (NOTE: invpt = 1/2pT)
  //
  //  if,    f := 1./(2xpt), then pt = 1./(2xf)
  //  sigma(pT) = 1./(2xf^2) sigma(f) 
  
  double sigmaPt = 1./(2*invpt * invpt) * sigmaTwoInvPt;
  return sigmaPt;
}



double FTK_UncertaintyTool::getSigmaTheta(double eta, double sigmaEta)
{
  //
  //  sigma(theta) = d(theta)/d(eta) x sigma(eta)
  //
  //  and,   d(theta)/d(eta) = 1./cosh(eta)
  //
  double coshEta    = cosh(eta); 
  double sigmaTheta = coshEta ? sigmaEta/coshEta : 10;
  return sigmaTheta;
}


//
//   Initialize the IBL constants
//
void FTK_UncertaintyTool::LoadConstants()
{
  //
  // has BLayer constants
  //
  TPConsts&  nomConsts =  m_allConsts[1];

  //
  // no BLayer constants
  //
  TPConsts&  nBLConsts =  m_allConsts[0];
  
  //
  //  d0
  //
  nomConsts.set(FTKTrackParam::d0,  0,  sqroot,     6.72e-04,   2.47e+04 );
  nomConsts.set(FTKTrackParam::d0,  1,  sqroot,     6.73e-04,   2.58e+04 );
  nomConsts.set(FTKTrackParam::d0,  2,  sqroot,     8.48e-04,   3.71e+04 );
  nomConsts.set(FTKTrackParam::d0,  3,  sqroot,     1.08e-03,   7.59e+04 );
  nomConsts.set(FTKTrackParam::d0,  4,  sqroot,     1.61e-03,   1.45e+05 );
 
  nBLConsts.set(FTKTrackParam::d0,  0,  sqroot,  3.14e-03,     2.11e5);
  nBLConsts.set(FTKTrackParam::d0,  1,  sqroot,  3.14e-03,     2.11e5);
  nBLConsts.set(FTKTrackParam::d0,  2,  sqroot,  3.14e-03,     2.11e5);
  nBLConsts.set(FTKTrackParam::d0,  3,  sqroot,  3.14e-03,     2.11e5);
  nBLConsts.set(FTKTrackParam::d0,  4,  sqroot,  3.14e-03,     2.11e5);

  //
  //  z0 
  //
  nomConsts.set(FTKTrackParam::z0,  0,  sqroot,      9.66e-03, 7.72e+04);
  nomConsts.set(FTKTrackParam::z0,  1,  sqroot,      7.39e-03, 7.35e+04);
  nomConsts.set(FTKTrackParam::z0,  2,  sqroot,      9.94e-03, 1.73e+05);
  nomConsts.set(FTKTrackParam::z0,  3,  sqroot,      2.61e-02, 5.31e+05);
  nomConsts.set(FTKTrackParam::z0,  4,  sqroot,      7.59e-02, 3.06e+06);

  nBLConsts.set(FTKTrackParam::z0,  0,  linear,  0.19,      558);
  nBLConsts.set(FTKTrackParam::z0,  1,  linear,  0.19,      558);
  nBLConsts.set(FTKTrackParam::z0,  2,  linear,  0.19,      558);
  nBLConsts.set(FTKTrackParam::z0,  3,  linear,  0.19,      558);
  nBLConsts.set(FTKTrackParam::z0,  4,  linear,  0.19,      558);

  //
  //  phi
  //
  nomConsts.set(FTKTrackParam::phi,  0,  linear,  0.00054,  4.3);
  nomConsts.set(FTKTrackParam::phi,  1,  linear,  0.00054,  4.3);
  nomConsts.set(FTKTrackParam::phi,  2,  linear,  0.00054,  4.3);
  nomConsts.set(FTKTrackParam::phi,  3,  linear,  0.00054,  4.3);
  nomConsts.set(FTKTrackParam::phi,  4,  linear,  0.00054,  4.3);

  nBLConsts.set(FTKTrackParam::phi,  0,  linear,  0.000614,   8.15);
  nBLConsts.set(FTKTrackParam::phi,  1,  linear,  0.000614,   8.15);
  nBLConsts.set(FTKTrackParam::phi,  2,  linear,  0.000614,   8.15);
  nBLConsts.set(FTKTrackParam::phi,  3,  linear,  0.000614,   8.15);
  nBLConsts.set(FTKTrackParam::phi,  4,  linear,  0.000614,   8.15);

  //
  //  eta
  //
  nomConsts.set(FTKTrackParam::eta,  0,  linear,  0.00081,  4.3);
  nomConsts.set(FTKTrackParam::eta,  1,  linear,  0.00081,  4.3);
  nomConsts.set(FTKTrackParam::eta,  2,  linear,  0.00081,  4.3);
  nomConsts.set(FTKTrackParam::eta,  3,  linear,  0.00081,  4.3);
  nomConsts.set(FTKTrackParam::eta,  4,  linear,  0.00081,  4.3);

  nBLConsts.set(FTKTrackParam::eta,  0,  linear,  0.00095,  6.7);
  nBLConsts.set(FTKTrackParam::eta,  1,  linear,  0.00095,  6.7);
  nBLConsts.set(FTKTrackParam::eta,  2,  linear,  0.00095,  6.7);
  nBLConsts.set(FTKTrackParam::eta,  3,  linear,  0.00095,  6.7);
  nBLConsts.set(FTKTrackParam::eta,  4,  linear,  0.00095,  6.7);

  //
  //  InvPt
  //
  nomConsts.set(FTKTrackParam::Ipt,  0,  linear,  2.7e-6,  0.023);
  nomConsts.set(FTKTrackParam::Ipt,  1,  linear,  2.7e-6,  0.023);
  nomConsts.set(FTKTrackParam::Ipt,  2,  linear,  2.7e-6,  0.023);
  nomConsts.set(FTKTrackParam::Ipt,  3,  linear,  2.7e-6,  0.023);
  nomConsts.set(FTKTrackParam::Ipt,  4,  linear,  2.7e-6,  0.023);

  nBLConsts.set(FTKTrackParam::Ipt,  0,  linear,  3.2e-6,  0.025);
  nBLConsts.set(FTKTrackParam::Ipt,  1,  linear,  3.2e-6,  0.025);
  nBLConsts.set(FTKTrackParam::Ipt,  2,  linear,  3.2e-6,  0.025);
  nBLConsts.set(FTKTrackParam::Ipt,  3,  linear,  3.2e-6,  0.025);
  nBLConsts.set(FTKTrackParam::Ipt,  4,  linear,  3.2e-6,  0.025);
  return;
}


//
//   Initialize the IBL constants
//
void FTK_UncertaintyTool::LoadConstants_NoIBL()
{
  //
  // has BLayer constants
  //
  TPConsts&  nomConsts =  m_allConsts[1];

  //
  // has noBLayer constants
  //
  TPConsts&  nBLConsts =  m_allConsts[0];
  
  //
  //  d0
  //
  nomConsts.set(FTKTrackParam::d0,  0,  linear,  2.66e-02,   1.67e+02);
  nomConsts.set(FTKTrackParam::d0,  1,  linear,  2.63e-02,   2.00e+02);
  nomConsts.set(FTKTrackParam::d0,  2,  linear,  3.34e-02,   2.26e+02);
  nomConsts.set(FTKTrackParam::d0,  3,  linear,  3.84e-02,   3.14e+02);
  nomConsts.set(FTKTrackParam::d0,  4,  linear,  4.61e-02,   4.63e+02);
 
  nBLConsts.set(FTKTrackParam::d0,  0,  sqroot,  0.0093,     5.2e5);
  nBLConsts.set(FTKTrackParam::d0,  1,  sqroot,  0.0093,     5.2e5);
  nBLConsts.set(FTKTrackParam::d0,  2,  sqroot,  0.0093,     5.2e5);
  nBLConsts.set(FTKTrackParam::d0,  3,  sqroot,  0.0093,     5.2e5);
  nBLConsts.set(FTKTrackParam::d0,  4,  sqroot,  0.0093,     5.2e5);

  //
  //  z0 
  //
  nomConsts.set(FTKTrackParam::z0,  0,  linear,  1.52e-01,  2.68e+02);
  nomConsts.set(FTKTrackParam::z0,  1,  linear,  1.39e-01,  3.60e+02);
  nomConsts.set(FTKTrackParam::z0,  2,  linear,  2.01e-01,  5.25e+02);
  nomConsts.set(FTKTrackParam::z0,  3,  linear,  3.10e-01,  8.64e+02);
  nomConsts.set(FTKTrackParam::z0,  4,  linear,  5.58e-01,  1.44e+03);

  nBLConsts.set(FTKTrackParam::z0,  0,  linear,  0.35,      480);
  nBLConsts.set(FTKTrackParam::z0,  1,  linear,  0.35,      480);
  nBLConsts.set(FTKTrackParam::z0,  2,  linear,  0.35,      480);
  nBLConsts.set(FTKTrackParam::z0,  3,  linear,  0.35,      480);
  nBLConsts.set(FTKTrackParam::z0,  4,  linear,  0.35,      480);

  //
  //  phi
  //
  nomConsts.set(FTKTrackParam::phi,  0,  linear,  0.00052,  5.9);
  nomConsts.set(FTKTrackParam::phi,  1,  linear,  0.00052,  5.9);
  nomConsts.set(FTKTrackParam::phi,  2,  linear,  0.00052,  5.9);
  nomConsts.set(FTKTrackParam::phi,  3,  linear,  0.00052,  5.9);
  nomConsts.set(FTKTrackParam::phi,  4,  linear,  0.00052,  5.9);

  nBLConsts.set(FTKTrackParam::phi,  0,  linear,  0.0011,   9.0);
  nBLConsts.set(FTKTrackParam::phi,  1,  linear,  0.0011,   9.0);
  nBLConsts.set(FTKTrackParam::phi,  2,  linear,  0.0011,   9.0);
  nBLConsts.set(FTKTrackParam::phi,  3,  linear,  0.0011,   9.0);
  nBLConsts.set(FTKTrackParam::phi,  4,  linear,  0.0011,   9.0);

  //
  //  eta
  //
  nomConsts.set(FTKTrackParam::eta,  0,  linear,  0.0012,  5.6);
  nomConsts.set(FTKTrackParam::eta,  1,  linear,  0.0012,  5.6);
  nomConsts.set(FTKTrackParam::eta,  2,  linear,  0.0012,  5.6);
  nomConsts.set(FTKTrackParam::eta,  3,  linear,  0.0012,  5.6);
  nomConsts.set(FTKTrackParam::eta,  4,  linear,  0.0012,  5.6);

  nBLConsts.set(FTKTrackParam::eta,  0,  linear,  0.0017,  7.5);
  nBLConsts.set(FTKTrackParam::eta,  1,  linear,  0.0017,  7.5);
  nBLConsts.set(FTKTrackParam::eta,  2,  linear,  0.0017,  7.5);
  nBLConsts.set(FTKTrackParam::eta,  3,  linear,  0.0017,  7.5);
  nBLConsts.set(FTKTrackParam::eta,  4,  linear,  0.0017,  7.5);

  //
  //  InvPt
  //
  nomConsts.set(FTKTrackParam::Ipt,  0,  linear,  3.0e-6,  0.024);
  nomConsts.set(FTKTrackParam::Ipt,  1,  linear,  3.0e-6,  0.024);
  nomConsts.set(FTKTrackParam::Ipt,  2,  linear,  3.0e-6,  0.024);
  nomConsts.set(FTKTrackParam::Ipt,  3,  linear,  3.0e-6,  0.024);
  nomConsts.set(FTKTrackParam::Ipt,  4,  linear,  3.0e-6,  0.024);

  nBLConsts.set(FTKTrackParam::Ipt,  0,  linear,  3.3e-6,  0.028);
  nBLConsts.set(FTKTrackParam::Ipt,  1,  linear,  3.3e-6,  0.028);
  nBLConsts.set(FTKTrackParam::Ipt,  2,  linear,  3.3e-6,  0.028);
  nBLConsts.set(FTKTrackParam::Ipt,  3,  linear,  3.3e-6,  0.028);
  nBLConsts.set(FTKTrackParam::Ipt,  4,  linear,  3.3e-6,  0.028);
  return;
}


/** New Constants for FTK error parameterisation.
 *    Generated uses single muon samples
 *    For more information see the JIRA: FTKSIM-22
 */
void FTK_UncertaintyTool::LoadConstants_DEC2017_V1()
{
  //////////////////////////////////////////////////
  // constants for tracks with an Inner B Layer hit.

  TPConsts&  nomConsts =  m_allConsts[1];

  //////////////////////////////////////////////////
  // constants for tracks with NO Inner B Layer hit.

  TPConsts&  nBLConsts =  m_allConsts[0];

  // linear fit
  // constants for tracks with an Inner B Layer hit. 

  // d0
  nomConsts.set(FTKTrackParam::d0 , 0, linear, 2.165000e-02,  9.746521e+01 );
  nomConsts.set(FTKTrackParam::d0 , 1, linear, 2.101431e-02,  1.160074e+02 );
  nomConsts.set(FTKTrackParam::d0 , 2, linear, 2.394603e-02,  1.361484e+02 );
  nomConsts.set(FTKTrackParam::d0 , 3, linear, 2.809658e-02,  1.742900e+02 );
  nomConsts.set(FTKTrackParam::d0 , 4, linear, 3.455036e-02,  2.320326e+02 );

  // z0
  nomConsts.set(FTKTrackParam::z0 , 0, linear, 8.291734e-02,  1.155988e+02 );
  nomConsts.set(FTKTrackParam::z0 , 1, linear, 7.341500e-02,  1.515614e+02 );
  nomConsts.set(FTKTrackParam::z0 , 2, linear, 1.054898e-01,  2.652691e+02 );
  nomConsts.set(FTKTrackParam::z0 , 3, linear, 1.516307e-01,  4.907506e+02 );
  nomConsts.set(FTKTrackParam::z0 , 4, linear, 2.813610e-01,  9.213986e+02 );

  // eta
  nomConsts.set(FTKTrackParam::eta, 0, linear, 9.444928e-04,  3.986158e+00 );
  nomConsts.set(FTKTrackParam::eta, 1, linear, 7.551402e-04,  4.239754e+00 );
  nomConsts.set(FTKTrackParam::eta, 2, linear, 7.807419e-04,  4.982420e+00 );
  nomConsts.set(FTKTrackParam::eta, 3, linear, 8.810903e-04,  5.835897e+00 );
  nomConsts.set(FTKTrackParam::eta, 4, linear, 1.167114e-03,  6.637692e+00 );

  // phi
  nomConsts.set(FTKTrackParam::phi, 0, linear, 4.638598e-04,  3.417675e+00 );
  nomConsts.set(FTKTrackParam::phi, 1, linear, 4.750186e-04,  3.876165e+00 );
  nomConsts.set(FTKTrackParam::phi, 2, linear, 5.519338e-04,  4.640639e+00 );
  nomConsts.set(FTKTrackParam::phi, 3, linear, 7.017347e-04,  5.796550e+00 );
  nomConsts.set(FTKTrackParam::phi, 4, linear, 8.980032e-04,  7.489104e+00 );

  // invpt NOTE: invpt = 1/2pt 
  nomConsts.set(FTKTrackParam::Ipt, 0, linear, 1.903324e-06,  1.633974e-02 );
  nomConsts.set(FTKTrackParam::Ipt, 1, linear, 1.861172e-06,  1.947333e-02 );
  nomConsts.set(FTKTrackParam::Ipt, 2, linear, 2.418678e-06,  2.525643e-02 );
  nomConsts.set(FTKTrackParam::Ipt, 3, linear, 4.411741e-06,  3.037951e-02 );
  nomConsts.set(FTKTrackParam::Ipt, 4, linear, 4.854594e-06,  3.993603e-02 );
  
  // constants for tracks with NO Inner B Layer hit. 
  
  // d0
  nBLConsts.set(FTKTrackParam::d0 , 0, linear, 2.912790e-02,  1.924919e+02 );
  nBLConsts.set(FTKTrackParam::d0 , 1, linear, 2.860355e-02,  2.299675e+02 );
  nBLConsts.set(FTKTrackParam::d0 , 2, linear, 2.798439e-02,  2.922039e+02 );
  nBLConsts.set(FTKTrackParam::d0 , 3, linear, 3.997899e-02,  3.538939e+02 );
  nBLConsts.set(FTKTrackParam::d0 , 4, linear, 5.354769e-02,  4.717690e+02 );
  // z0
  nBLConsts.set(FTKTrackParam::z0 , 0, linear, 1.470882e-01,  2.459152e+02 );
  nBLConsts.set(FTKTrackParam::z0 , 1, linear, 1.400019e-01,  3.123315e+02 );
  nBLConsts.set(FTKTrackParam::z0 , 2, linear, 1.559520e-01,  4.842909e+02 );
  nBLConsts.set(FTKTrackParam::z0 , 3, linear, 2.353992e-01,  9.313151e+02 );
  nBLConsts.set(FTKTrackParam::z0 , 4, linear, 4.052754e-01,  2.007789e+03 );
  // eta
  nBLConsts.set(FTKTrackParam::eta, 0, linear, 1.161208e-03,  4.964795e+00 );
  nBLConsts.set(FTKTrackParam::eta, 1, linear, 9.397004e-04,  5.581838e+00 );
  nBLConsts.set(FTKTrackParam::eta, 2, linear, 8.315023e-04,  6.417038e+00 );
  nBLConsts.set(FTKTrackParam::eta, 3, linear, 8.612335e-04,  8.373559e+00 );
  nBLConsts.set(FTKTrackParam::eta, 4, linear, 1.165942e-03,  1.116492e+01 );
  // phi
  nBLConsts.set(FTKTrackParam::phi, 0, linear, 3.968176e-04,  5.126463e+00 );
  nBLConsts.set(FTKTrackParam::phi, 1, linear, 4.198626e-04,  5.995350e+00 );
  nBLConsts.set(FTKTrackParam::phi, 2, linear, 4.472895e-04,  7.388593e+00 );
  nBLConsts.set(FTKTrackParam::phi, 3, linear, 7.468634e-04,  8.957967e+00 );
  nBLConsts.set(FTKTrackParam::phi, 4, linear, 1.013463e-03,  1.195214e+01 );
  // invpt NOTE: invpt = 1/2pt
  nBLConsts.set(FTKTrackParam::Ipt, 0, linear, 2.114881e-06,  1.591428e-02 );
  nBLConsts.set(FTKTrackParam::Ipt, 1, linear, 2.225571e-06,  1.814322e-02 );
  nBLConsts.set(FTKTrackParam::Ipt, 2, linear, 2.695112e-06,  2.459482e-02 );
  nBLConsts.set(FTKTrackParam::Ipt, 3, linear, 5.146374e-06,  2.830717e-02 );
  nBLConsts.set(FTKTrackParam::Ipt, 4, linear, 5.777676e-06,  3.808874e-02 );
  
  // square root fit
  // constants for tracks with an Inner B Layer hit. 

  // d0
  nomConsts.set(FTKTrackParam::d0 , 0, sqroot, 6.833476e-04,  2.053167e+04 );
  nomConsts.set(FTKTrackParam::d0 , 1, sqroot, 7.110604e-04,  2.629804e+04 );
  nomConsts.set(FTKTrackParam::d0 , 2, sqroot, 8.887155e-04,  3.635433e+04 );
  nomConsts.set(FTKTrackParam::d0 , 3, sqroot, 1.240019e-03,  5.512366e+04 );
  nomConsts.set(FTKTrackParam::d0 , 4, sqroot, 2.010290e-03,  9.366573e+04 );

  // z0
  nomConsts.set(FTKTrackParam::z0 , 0, sqroot, 8.182191e-03,  5.613992e+04 );
  nomConsts.set(FTKTrackParam::z0 , 1, sqroot, 6.926827e-03,  7.541361e+04 );
  nomConsts.set(FTKTrackParam::z0 , 2, sqroot, 1.450779e-02,  2.038478e+05 );
  nomConsts.set(FTKTrackParam::z0 , 3, sqroot, 3.203846e-02,  5.949688e+05 );
  nomConsts.set(FTKTrackParam::z0 , 4, sqroot, 1.098017e-01,  2.102301e+06 );

  // eta
  nomConsts.set(FTKTrackParam::eta, 0, sqroot, 1.285953e-06,  3.543054e+01 );
  nomConsts.set(FTKTrackParam::eta, 1, sqroot, 8.962818e-07,  3.544991e+01 );
  nomConsts.set(FTKTrackParam::eta, 2, sqroot, 1.005838e-06,  4.664460e+01 );
  nomConsts.set(FTKTrackParam::eta, 3, sqroot, 1.311960e-06,  6.090104e+01 );
  nomConsts.set(FTKTrackParam::eta, 4, sqroot, 2.210244e-06,  8.523981e+01 );

  // phi
  nomConsts.set(FTKTrackParam::phi, 0, sqroot, 3.532779e-07,  2.043296e+01 );
  nomConsts.set(FTKTrackParam::phi, 1, sqroot, 3.952656e-07,  2.575445e+01 );
  nomConsts.set(FTKTrackParam::phi, 2, sqroot, 5.448243e-07,  3.618809e+01 );
  nomConsts.set(FTKTrackParam::phi, 3, sqroot, 8.780914e-07,  5.498637e+01 );
  nomConsts.set(FTKTrackParam::phi, 4, sqroot, 1.403608e-06,  9.138036e+01 );

  // invpt NOTE: invpt = 1/2pt
  nomConsts.set(FTKTrackParam::Ipt, 0, sqroot, 1.836182e-11,  9.804247e-04 );
  nomConsts.set(FTKTrackParam::Ipt, 1, sqroot, 6.989922e-12,  5.492308e-04 );
  nomConsts.set(FTKTrackParam::Ipt, 2, sqroot, 1.038204e-11,  9.896642e-04 );
  nomConsts.set(FTKTrackParam::Ipt, 3, sqroot, 3.414800e-11,  1.627811e-03 );
  nomConsts.set(FTKTrackParam::Ipt, 4, sqroot, 4.302132e-11,  2.582682e-03 );
  
  // constants for tracks with NO Inner B Layer hit. 

  // d0
  nBLConsts.set(FTKTrackParam::d0 , 0, sqroot, 1.285215e-03,  6.789728e+04 );
  nBLConsts.set(FTKTrackParam::d0 , 1, sqroot, 1.318903e-03,  9.059932e+04 );
  nBLConsts.set(FTKTrackParam::d0 , 2, sqroot, 1.592501e-03,  1.304213e+05 );
  nBLConsts.set(FTKTrackParam::d0 , 3, sqroot, 2.723310e-03,  2.131978e+05 );
  nBLConsts.set(FTKTrackParam::d0 , 4, sqroot, 5.110093e-03,  3.781747e+05 );

  // z0
  nBLConsts.set(FTKTrackParam::z0 , 0, sqroot, 2.683949e-02,  2.209501e+05 );
  nBLConsts.set(FTKTrackParam::z0 , 1, sqroot, 2.498995e-02,  2.968105e+05 );
  nBLConsts.set(FTKTrackParam::z0 , 2, sqroot, 3.306697e-02,  5.964677e+05 );
  nBLConsts.set(FTKTrackParam::z0 , 3, sqroot, 7.886040e-02,  2.009275e+06 );
  nBLConsts.set(FTKTrackParam::z0 , 4, sqroot, 2.583879e-01,  8.139110e+06 );

  // eta
  nBLConsts.set(FTKTrackParam::eta, 0, sqroot, 1.915813e-06,  5.393240e+01 );
  nBLConsts.set(FTKTrackParam::eta, 1, sqroot, 1.411604e-06,  5.869697e+01 );
  nBLConsts.set(FTKTrackParam::eta, 2, sqroot, 1.164994e-06,  7.193975e+01 );
  nBLConsts.set(FTKTrackParam::eta, 3, sqroot, 1.383405e-06,  1.143532e+02 );
  nBLConsts.set(FTKTrackParam::eta, 4, sqroot, 2.680622e-06,  2.015519e+02 );

  // phi
  nBLConsts.set(FTKTrackParam::phi, 0, sqroot, 3.019947e-07,  3.838097e+01 );
  nBLConsts.set(FTKTrackParam::phi, 1, sqroot, 3.352412e-07,  5.019127e+01 );
  nBLConsts.set(FTKTrackParam::phi, 2, sqroot, 4.389225e-07,  7.557339e+01 );
  nBLConsts.set(FTKTrackParam::phi, 3, sqroot, 1.050043e-06,  1.213369e+02 );
  nBLConsts.set(FTKTrackParam::phi, 4, sqroot, 2.187977e-06,  2.114925e+02 );

  // invpt NOTE: invpt = 1/2pt
  nBLConsts.set(FTKTrackParam::Ipt, 0, sqroot, 7.004789e-12,  4.130296e-04 );
  nBLConsts.set(FTKTrackParam::Ipt, 1, sqroot, 7.521822e-12,  5.390953e-04 );
  nBLConsts.set(FTKTrackParam::Ipt, 2, sqroot, 1.274270e-11,  9.718143e-04 );
  nBLConsts.set(FTKTrackParam::Ipt, 3, sqroot, 4.524908e-11,  1.547622e-03 );
  nBLConsts.set(FTKTrackParam::Ipt, 4, sqroot, 5.795779e-11,  2.592702e-03 );
  
  
  return;
}
