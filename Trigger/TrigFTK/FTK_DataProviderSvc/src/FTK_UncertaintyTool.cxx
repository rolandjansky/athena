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


FTK_UncertaintyTool::FTK_UncertaintyTool(const std::string& t, 
					       const std::string& n,
					       const IInterface*  p ): 
  AthAlgTool(t,n,p),
  m_noIBL(false),
  m_ftkparversion("OCT2017_V1")
{
  declareInterface< IFTK_UncertaintyTool >( this );
  declareProperty( "NoIBL",  m_noIBL);
  declareProperty( "ftkparversion",  m_ftkparversion);
}

StatusCode FTK_UncertaintyTool::initialize() {

  StatusCode sc = AlgTool::initialize();
  MsgStream athenaLog(msgSvc(), name());

  //
  //   Load Constants
  //
  
  if(m_noIBL)
    LoadConstants_NoIBL();
  else
    if(m_ftkparversion == "LEGACY")
      LoadConstants();
    else if(m_ftkparversion == "OCT2017_V1")
      LoadConstants_OCT2017_V1();
    else 
      ATH_MSG_WARNING("m_ftkparversion not supported, reverting to default OCT2017_V1 parameters");
      LoadConstants_OCT2017_V1();

  athenaLog << MSG::INFO << "FTK_UncertaintyTool initialized "<< endmsg;
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
  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );
  athenaLog << MSG::VERBOSE << "In getParamCovMtx: id0: " << id0 << " id1: " << id1 << endmsg; 


  //
  // Use diagonal Maxtrix for now
  //
  if (id0!=id1) {
    return 0.;
  }

  if(outputLevel <= MSG::DEBUG)
    athenaLog << MSG::DEBUG << "FTK_UncertaintyTool:: has BL " << hasIBL << endmsg; 
  
  double trkIpt = trk.getInvPt();  
  double trkTheta = atan2(1.0,trk.getCotTh());
  double trkEta = -log(tan(trkTheta/2));

  if(outputLevel <= MSG::VERBOSE){
    athenaLog << MSG::VERBOSE << "FTK_UncertaintyTool:: trkIpt " << trkIpt << endmsg; 
    athenaLog << MSG::VERBOSE << "FTK_UncertaintyTool:: trkEta " << trkEta << endmsg; 
  }

  double sigmaTP = -1.0;

  //
  //  Uncertianties are stored using eta and 1/pt
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
    if(outputLevel <= MSG::DEBUG){
      athenaLog << MSG::DEBUG << "FTK_UncertaintyTool:: sigmaTP ("   
		<< sigmaTP <<") = sqrt("<<m_allConsts[hasIBL].par0(lookUpParam,trkEta) 
		<< "+" << m_allConsts[hasIBL].par1(lookUpParam,trkEta) << "*" << trkIpt << "**2)" <<  endmsg; 
      athenaLog << MSG::DEBUG << "FTK_UncertaintyTool:: (sqrt)cov "   << sigmaTP*sigmaTP << endmsg; 
    }
    
  //
  // linear model
  //
  }else{
    sigmaTP = m_allConsts[hasIBL].par0(lookUpParam,trkEta) + m_allConsts[hasIBL].par1(lookUpParam,trkEta)*fabs(trkIpt);
    if(outputLevel <= MSG::DEBUG){
      athenaLog << MSG::DEBUG << "FTK_UncertaintyTool:: sigmaTP ("   
		<< sigmaTP <<") = "<<m_allConsts[hasIBL].par0(lookUpParam,trkEta) 
		<< "+" << m_allConsts[hasIBL].par1(lookUpParam,trkEta) << "*" << fabs(trkIpt) <<  endmsg; 
      athenaLog << MSG::DEBUG << "FTK_UncertaintyTool:: (linear)cov "   << sigmaTP*sigmaTP << endmsg; 
    }
  }

  if(sigmaTP < 0)
    athenaLog << MSG::ERROR << " sigma TP" << sigmaTP << endmsg;

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


/**
 * 
 * 
 * 
 * 
 * 
 */ 

/** New Constants for FTK error parameterisation.
 *    Generated uses single muon samples
 *    For more information see the JIRA: FTKSIM-22
 */
void FTK_UncertaintyTool::LoadConstants_OCT2017_V1()
{
  //////////////////////////////////////////////////
  // constants for tracks with an Inner B Layer hit.

  TPConsts&  nomConsts =  m_allConsts[1];

  //////////////////////////////////////////////////
  // constants for tracks with NO Inner B Layer hit.

  TPConsts&  nBLConsts =  m_allConsts[0];
  


  //////////////////////////////////////////////////
  // constants for tracks with an Inner B Layer hit.
  //
  //////////////////////////////////////////////////
  //
  // Fit Mode : Linear
  //////////////////////////////////////////////////
  //
  // Track Parameter : d0
  //////////////////////////////////////////////////
  nomConsts.set(FTKTrackParam::d0, 0,linear ,0.0220148,  46.2681 );
  nomConsts.set(FTKTrackParam::d0, 1,linear ,0.0215074,  54.7555 );
  nomConsts.set(FTKTrackParam::d0, 2,linear ,0.0245233,  63.6461 );
  nomConsts.set(FTKTrackParam::d0, 3,linear ,0.0289256,  81.6173 );
  nomConsts.set(FTKTrackParam::d0, 4,linear ,0.0355705,  107.437 );

  //
  // Track Parameter : z0
  //////////////////////////////////////////////////
  nomConsts.set(FTKTrackParam::z0, 0,linear ,0.0843801,  49.9992 );
  nomConsts.set(FTKTrackParam::z0, 1,linear ,0.0768551,  59.5731 );
  nomConsts.set(FTKTrackParam::z0, 2,linear ,0.109679,  109.795 );
  nomConsts.set(FTKTrackParam::z0, 3,linear ,0.157026,  213.575 );
  nomConsts.set(FTKTrackParam::z0, 4,linear ,0.294999,  382.75 );

  //
  // Track Parameter : eta
  //////////////////////////////////////////////////
  nomConsts.set(FTKTrackParam::eta, 0,linear ,0.000985233,  1.74928 );
  nomConsts.set(FTKTrackParam::eta, 1,linear ,0.000794436,  1.86279 );
  nomConsts.set(FTKTrackParam::eta, 2,linear ,0.000814759,  2.24591 );
  nomConsts.set(FTKTrackParam::eta, 3,linear ,0.000928633,  2.62711 );
  nomConsts.set(FTKTrackParam::eta, 4,linear ,0.0012434,  2.86116 )  ;

  //
  // Track Parameter : phi
  //////////////////////////////////////////////////
  nomConsts.set(FTKTrackParam::phi, 0,linear ,0.000470877,  1.65773 );
  nomConsts.set(FTKTrackParam::phi, 1,linear ,0.000491551,  1.8313 );
  nomConsts.set(FTKTrackParam::phi, 2,linear ,0.000567083,  2.20669 );
  nomConsts.set(FTKTrackParam::phi, 3,linear ,0.000744999,  2.63184 );
  nomConsts.set(FTKTrackParam::phi, 4,linear ,0.000931243,  3.47986 );

  //
  // Track Parameter : inverse pt
  //////////////////////////////////////////////////
  nomConsts.set(FTKTrackParam::Ipt, 0,linear ,0.00340545,  15.5999 );
  nomConsts.set(FTKTrackParam::Ipt, 1,linear ,0.00341737,  18.7316 );
  nomConsts.set(FTKTrackParam::Ipt, 2,linear ,0.00423504,  24.7642 );
  nomConsts.set(FTKTrackParam::Ipt, 3,linear ,0.00900023,  27.4348 );
  nomConsts.set(FTKTrackParam::Ipt, 4,linear ,0.00973772,  37.3089 );

  //////////////////////////////////////////////////
  //
  // Fit Mode : Square Root
  //////////////////////////////////////////////////
  //
  // Track Parameter : d0
  //////////////////////////////////////////////////
  nomConsts.set(FTKTrackParam::d0, 0,sqroot ,0.000637139,  6001.77 );
  nomConsts.set(FTKTrackParam::d0, 1,sqroot ,0.00066471,  7508.6 );
  nomConsts.set(FTKTrackParam::d0, 2,sqroot ,0.000822638,  10604.5 );
  nomConsts.set(FTKTrackParam::d0, 3,sqroot ,0.00117164,  15415.9 );
  nomConsts.set(FTKTrackParam::d0, 4,sqroot ,0.00187295,  25707 );

  //
  // Track Parameter : z0
  //////////////////////////////////////////////////
  nomConsts.set(FTKTrackParam::z0, 0,sqroot ,0.00787881,  17682.2 );
  nomConsts.set(FTKTrackParam::z0, 1,sqroot ,0.00678476,  20436.2 );
  nomConsts.set(FTKTrackParam::z0, 2,sqroot ,0.0138578,  59498.7 );
  nomConsts.set(FTKTrackParam::z0, 3,sqroot ,0.0307752,  163152 );
  nomConsts.set(FTKTrackParam::z0, 4,sqroot ,0.104587,  573875 );

  //
  // Track Parameter : eta
  //////////////////////////////////////////////////
  nomConsts.set(FTKTrackParam::eta, 0,sqroot ,1.21166e-06,  10.0528 );
  nomConsts.set(FTKTrackParam::eta, 1,sqroot ,8.54596e-07,  9.59626 );
  nomConsts.set(FTKTrackParam::eta, 2,sqroot ,9.45997e-07,  12.7565 );
  nomConsts.set(FTKTrackParam::eta, 3,sqroot ,1.25597e-06,  16.3869 );
  nomConsts.set(FTKTrackParam::eta, 4,sqroot ,2.09273e-06,  22.9401 );

  //
  // Track Parameter : phi
  //////////////////////////////////////////////////
  nomConsts.set(FTKTrackParam::phi, 0,sqroot ,3.29631e-07,  5.74672 );
  nomConsts.set(FTKTrackParam::phi, 1,sqroot ,3.69142e-07,  7.05712 );
  nomConsts.set(FTKTrackParam::phi, 2,sqroot ,4.99178e-07,  10.1754 );
  nomConsts.set(FTKTrackParam::phi, 3,sqroot ,8.4178e-07,  14.6755 );
  nomConsts.set(FTKTrackParam::phi, 4,sqroot ,1.30762e-06,  24.8247 );

  //
  // Track Parameter : inverse pt
  //////////////////////////////////////////////////
  nomConsts.set(FTKTrackParam::Ipt, 0,sqroot ,1.69434e-05,  448.741 );
  nomConsts.set(FTKTrackParam::Ipt, 1,sqroot ,1.94485e-05,  587.337 );
  nomConsts.set(FTKTrackParam::Ipt, 2,sqroot ,3.04694e-05,  1055.24 );
  nomConsts.set(FTKTrackParam::Ipt, 3,sqroot ,0.000120377,  1720.64 );
  nomConsts.set(FTKTrackParam::Ipt, 4,sqroot ,0.000149814,  2797.65 );


  //////////////////////////////////////////////////
  // NEW SECTION 
  //////////////////////////////////////////////////

  // constants for tracks with NO Inner B Layer hit.
  //
  //////////////////////////////////////////////////
  //
  // Fit Mode : Linear
  //////////////////////////////////////////////////
  //
  // Track Parameter : d0
  //////////////////////////////////////////////////
  nBLConsts.set(FTKTrackParam::d0, 0,linear ,0.0303368,  86.0664 );
  nBLConsts.set(FTKTrackParam::d0, 1,linear ,0.0285923,  109.879 );
  nBLConsts.set(FTKTrackParam::d0, 2,linear ,0.0297772,  132.037 );
  nBLConsts.set(FTKTrackParam::d0, 3,linear ,0.0405575,  160.262 );
  nBLConsts.set(FTKTrackParam::d0, 4,linear ,0.0510576,  244.525 );

  //
  // Track Parameter : z0
  //////////////////////////////////////////////////
  nBLConsts.set(FTKTrackParam::z0, 0,linear ,0.150392,  111.119 );
  nBLConsts.set(FTKTrackParam::z0, 1,linear ,0.143484,  131.085 );
  nBLConsts.set(FTKTrackParam::z0, 2,linear ,0.159992,  214.075 );
  nBLConsts.set(FTKTrackParam::z0, 3,linear ,0.234851,  431.632 );
  nBLConsts.set(FTKTrackParam::z0, 4,linear ,0.433525,  862.367 );

  //
  // Track Parameter : eta
  //////////////////////////////////////////////////
  nBLConsts.set(FTKTrackParam::eta, 0,linear ,0.00117151,  2.23426 );
  nBLConsts.set(FTKTrackParam::eta, 1,linear ,0.000972738,  2.54413 );
  nBLConsts.set(FTKTrackParam::eta, 2,linear ,0.000881282,  2.8418 );
  nBLConsts.set(FTKTrackParam::eta, 3,linear ,0.000886967,  3.85905 );
  nBLConsts.set(FTKTrackParam::eta, 4,linear ,0.00127867,  4.97624 );

  //
  // Track Parameter : phi
  //////////////////////////////////////////////////
  nBLConsts.set(FTKTrackParam::phi, 0,linear ,0.000417072,  2.37975 );
  nBLConsts.set(FTKTrackParam::phi, 1,linear ,0.000444298,  2.77566 );
  nBLConsts.set(FTKTrackParam::phi, 2,linear ,0.000468003,  3.45385 );
  nBLConsts.set(FTKTrackParam::phi, 3,linear ,0.000742838,  4.20268 );
  nBLConsts.set(FTKTrackParam::phi, 4,linear ,0.00104741,  5.76834 );

  //
  // Track Parameter : inverse pt
  //////////////////////////////////////////////////
  nBLConsts.set(FTKTrackParam::Ipt, 0,linear ,0.00354561,  15.5388 );
  nBLConsts.set(FTKTrackParam::Ipt, 1,linear ,0.00385489,  17.9517 );
  nBLConsts.set(FTKTrackParam::Ipt, 2,linear ,0.004851,  23.0106 );
  nBLConsts.set(FTKTrackParam::Ipt, 3,linear ,0.00987171,  26.3157 );
  nBLConsts.set(FTKTrackParam::Ipt, 4,linear ,0.0106293,  38.0365 );

  //////////////////////////////////////////////////
  //
  // Fit Mode : Square Root
  //////////////////////////////////////////////////
  //
  // Track Parameter : d0
  //////////////////////////////////////////////////
  nBLConsts.set(FTKTrackParam::d0, 0,sqroot ,0.00126753,  16937 );
  nBLConsts.set(FTKTrackParam::d0, 1,sqroot ,0.00124176,  24184.4 );
  nBLConsts.set(FTKTrackParam::d0, 2,sqroot ,0.00150547,  33635.4 );
  nBLConsts.set(FTKTrackParam::d0, 3,sqroot ,0.00260565,  51653.5 );
  nBLConsts.set(FTKTrackParam::d0, 4,sqroot ,0.00437864,  106540 );

  //
  // Track Parameter : z0
  //////////////////////////////////////////////////
  nBLConsts.set(FTKTrackParam::z0, 0,sqroot ,0.0256128,  67263.8 );
  nBLConsts.set(FTKTrackParam::z0, 1,sqroot ,0.0238329,  79920.8 );
  nBLConsts.set(FTKTrackParam::z0, 2,sqroot ,0.0314672,  170293 );
  nBLConsts.set(FTKTrackParam::z0, 3,sqroot ,0.0707115,  571577 );
  nBLConsts.set(FTKTrackParam::z0, 4,sqroot ,0.242194,  2.20335e+06 );

  //
  // Track Parameter : eta
  //////////////////////////////////////////////////
  nBLConsts.set(FTKTrackParam::eta, 0,sqroot ,1.8004e-06,  14.4476 );
  nBLConsts.set(FTKTrackParam::eta, 1,sqroot ,1.34158e-06,  15.9376 );
  nBLConsts.set(FTKTrackParam::eta, 2,sqroot ,1.10103e-06,  18.6251 );
  nBLConsts.set(FTKTrackParam::eta, 3,sqroot ,1.30795e-06,  29.8202 );
  nBLConsts.set(FTKTrackParam::eta, 4,sqroot ,2.6078e-06,  50.8278 );

  //
  // Track Parameter : phi
  //////////////////////////////////////////////////
  nBLConsts.set(FTKTrackParam::phi, 0,sqroot ,2.96999e-07,  9.5144 );
  nBLConsts.set(FTKTrackParam::phi, 1,sqroot ,3.21576e-07,  12.9412 );
  nBLConsts.set(FTKTrackParam::phi, 2,sqroot ,4.12824e-07,  19.3168 );
  nBLConsts.set(FTKTrackParam::phi, 3,sqroot ,9.64825e-07,  30.6326 );
  nBLConsts.set(FTKTrackParam::phi, 4,sqroot ,1.99829e-06,  57.3082 );

  //
  // Track Parameter : inverse pt
  //////////////////////////////////////////////////
  nBLConsts.set(FTKTrackParam::Ipt, 0,sqroot ,1.68493e-05,  449.859 );
  nBLConsts.set(FTKTrackParam::Ipt, 1,sqroot ,2.0417e-05,  577.767 );
  nBLConsts.set(FTKTrackParam::Ipt, 2,sqroot ,3.99292e-05,  975.569 );
  nBLConsts.set(FTKTrackParam::Ipt, 3,sqroot ,0.000147852,  1702.44 );
  nBLConsts.set(FTKTrackParam::Ipt, 4,sqroot ,0.000193678,  2728.82 );

  //////////////////////////////////////////////////
  // END OF SECTION
  //////////////////////////////////////////////////

  return;
}
