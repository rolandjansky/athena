/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetBeamSpotVertex.h"
#include "GaudiKernel/ITHistSvc.h"

#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxContainer.h"

using namespace InDet;

namespace BeamSpot {
  const std::vector<  BeamSpot::VrtHolder > * vertexData;
  void myFCN_LLsolver( Int_t &, Double_t *, Double_t &, Double_t *, Int_t); // FCN for LL
  void myFCN_LLsolverNorm( Int_t &, Double_t *, Double_t &, Double_t *, Int_t); // FCN for LL
  double norm_xMin(-1e8), norm_xMax(1e8), norm_yMin(-1e8), norm_yMax(1e8) , norm_zMin(-1e8), norm_zMax(1e8);
  double pdfxy(double *x, double *p);
}

class SortDistToMedian: public std::binary_function <double, double, bool>
{
  double m_median;

public:
  SortDistToMedian(double median): m_median(median) {}

  bool operator()(double a , double b) const {
    return fabs(a - m_median) < fabs(b -m_median);
  }
};


InDetBeamSpotVertex::InDetBeamSpotVertex( const std::string& type, 
              const std::string& name, 
              const IInterface* parent):
  AthAlgTool(type,name,parent),
  m_x(4),m_cov(4,0),m_z(0.),m_zErr(0.),
  m_p(4), m_V(4,0),
  m_zSolved(0.), m_zErrSolved(0.),
  m_NPARS(10), m_pLL(m_NPARS,0),m_VLL(m_NPARS,0),
  m_vertexCount(0),m_getLLres(false),
  m_fitStatus(unsolved),m_minVtxProb{},m_nUsed(0)

{
  declareInterface<IInDetBeamSpotTool>(this);
  declareProperty("UseLikelihood",m_useLL = true);

  //Fit initial parameter setup
  declareProperty("InitParX",      m_init_x =0.0);
  declareProperty("InitParY",      m_init_y =0.0);
  declareProperty("InitParZ",      m_init_z =0.0);
  declareProperty("InitParAx",     m_init_ax=0.0);
  declareProperty("InitParAy",     m_init_ay=0.0);
  declareProperty("InitParSigmaX", m_init_sx=0.01);
  declareProperty("InitParSigmaY", m_init_sy=0.01);
  declareProperty("InitParSigmaZ", m_init_sz=56.0);
  declareProperty("InitParK",      m_init_k =1.0);
  declareProperty("InitParRhoXY",  m_init_rhoxy=0.0);
  declareProperty("MinuitMaxIterations",  m_minuit_maxIter=2000);
  declareProperty("InitParMinX",      m_init_min_x =0.0);
  declareProperty("InitParMinY",      m_init_min_y =0.0);
  declareProperty("InitParMinZ",      m_init_min_z =0.0);
  declareProperty("InitParMinAx",     m_init_min_ax=0.0);
  declareProperty("InitParMinAy",     m_init_min_ay=0.0);
  declareProperty("InitParMinSigmaX", m_init_min_sx=0.0001);
  declareProperty("InitParMinSigmaY", m_init_min_sy=0.0001);

  declareProperty("InitParMinSigmaZ", m_init_min_sz=0.0);
  declareProperty("InitParMinK",      m_init_min_k =0);
  declareProperty("InitParMinRhoXY",  m_init_min_rhoxy=-1.0);
 
  declareProperty("InitParMaxX",      m_init_max_x =0.0);
  declareProperty("InitParMaxY",      m_init_max_y =0.0);
  declareProperty("InitParMaxZ",      m_init_max_z =0.0);
  declareProperty("InitParMaxAx",     m_init_max_ax=0.0);
  declareProperty("InitParMaxAy",     m_init_max_ay=0.0);
  declareProperty("InitParMaxSigmaX", m_init_max_sx=0.7);
  declareProperty("InitParMaxSigmaY", m_init_max_sy=0.7);
  declareProperty("InitParMaxSigmaZ", m_init_max_sz=3000.);
  declareProperty("InitParMaxK",      m_init_max_k =10.);
  declareProperty("InitParMaxRhoXY",  m_init_max_rhoxy=1.0);

  declareProperty("DefaultX0",  m_def_x0=0.0);
  declareProperty("DefaultY0",  m_def_y0=0.0);
  declareProperty("DefaultZ",   m_def_z=0.0);
  declareProperty("DefaultTiltX",  m_def_ax=0.0);
  declareProperty("DefaultTiltY",  m_def_ay=0.0);
  //these are used when returning a chi2 value
  declareProperty("DefaultSigmaX",  m_def_sx=0.0);
  declareProperty("DefaultSigmaY",  m_def_sy=0.0);
  declareProperty("DefaultSigmaZ",  m_def_sz=0.0);

 
  // selections
  declareProperty("MaxSigmaTr", m_sigTr=100.); 
  declareProperty("MaxVtxErrTr", m_maxVtxErTr=100.); // in mm
  declareProperty("OutlierChi2Tr", m_outlierChi2Tr=50.);
  declareProperty("MaxOutlierLoops",m_maxOutlierLoops = 30);
  declareProperty("OutlierMaxRejection",m_singleIterationMax=30);
  declareProperty("OutlierWidthFail", m_widthFail=5.1e-3); // in mm
  declareProperty("OutlierRhoFail", m_rhoFail=0.8); 

  declareProperty("OutlierKFailMin", m_kMinFail = 0);
  declareProperty("OutlierKFailMax", m_kMaxFail = 9.9);
  
  declareProperty("DoFitSanityCheck", m_doFitSanityCheck=true); 
  declareProperty("DoChi2OnlyOutlierRemoval",m_doChi2OutlierRemoval=false);
  //  declareProperty( "MinVertexProb", m_minVtxProb=0.001); // min prob(chi2,ndf)
  
  declareProperty( "FixParK" ,   m_fixInputK = false);
  declareProperty( "UseLLNorm" , m_useLLNorm = false);
  declareProperty( "FixWidth",   m_fixWidth  = false); 

  declareProperty("TruncatedRMS", m_truncatedRMS = false); 
  declareProperty("RMSFraction", m_fractionRMS = 0.95); 
  declareProperty("SetInitialRMS", m_setInitialRMS = false);

}

InDetBeamSpotVertex::InDetBeamSpotVertex( const InDetBeamSpotVertex& rhs) : 
  IInDetBeamSpotTool(rhs), 
  AthAlgTool(rhs.type(), rhs.name(), rhs.parent()),
  m_x(4),m_cov(4,0),m_z(0.),m_zErr(0.),
  m_p(4), m_V(4,0),
  m_zSolved(0.), m_zErrSolved(0.),
  m_NPARS(10), m_pLL(m_NPARS,0),m_VLL(m_NPARS,0),
  m_vertexCount(0),m_getLLres(false),
  m_fitStatus(unsolved),m_nUsed(0)
{
  m_useLL = rhs.m_useLL;
  m_init_x = rhs.m_init_x;
  m_init_y = rhs.m_init_y;
  m_useLL = rhs.m_useLL;

  m_init_x = rhs.m_init_x; 
  m_init_y = rhs.m_init_y;
  m_init_z = rhs.m_init_z;
  m_init_sx = rhs.m_init_sx; 
  m_init_sy = rhs.m_init_sy;
  m_init_sz = rhs.m_init_sz;
  m_init_ax = rhs.m_init_ax;
  m_init_ay = rhs.m_init_ay;
  m_init_k = rhs.m_init_k;
  m_init_rhoxy = rhs.m_init_rhoxy;
  
  m_init_min_x = rhs.m_init_min_x;
  m_init_min_y = rhs.m_init_min_y;
  m_init_min_z = rhs.m_init_min_z;
  m_init_min_sx = rhs.m_init_min_sx;
  m_init_min_sy = rhs.m_init_min_sy;
  m_init_min_sz = rhs.m_init_min_sz;
  m_init_min_ax = rhs.m_init_min_ax;
  m_init_min_ay = rhs.m_init_min_ay;
  m_init_min_k = rhs.m_init_min_k;
  m_init_min_rhoxy = rhs.m_init_min_rhoxy;
  
  m_init_max_x = rhs.m_init_max_x;
  m_init_max_y = rhs.m_init_max_y;
  m_init_max_z = rhs.m_init_max_z;
  m_init_max_sx = rhs.m_init_max_sx;
  m_init_max_sy = rhs.m_init_max_sy;
  m_init_max_sz = rhs.m_init_max_sz;
  m_init_max_ax = rhs.m_init_max_ax;
  m_init_max_ay = rhs.m_init_max_ay;
  m_init_max_k = rhs.m_init_max_k;
  m_init_max_rhoxy = rhs.m_init_max_rhoxy;
  
  m_def_x0 = rhs.m_def_x0;
  m_def_y0 = rhs.m_def_y0;
  m_def_z = rhs.m_def_z;
  m_def_ax = rhs.m_def_ax;
  m_def_ay = rhs.m_def_ay;
  m_def_sx = rhs.m_def_sx;
  m_def_sy = rhs.m_def_sy;
  m_def_sz = rhs.m_def_sz;

  m_sigTr = rhs.m_sigTr;
  m_maxVtxErTr = rhs.m_maxVtxErTr;


  m_minuit_maxIter = rhs.m_minuit_maxIter;
  
  m_outlierChi2Tr       = rhs.m_outlierChi2Tr;
  m_maxOutlierLoops     = rhs.m_maxOutlierLoops;
  m_singleIterationMax  = rhs.m_singleIterationMax;
  m_widthFail           = rhs.m_widthFail;
  m_rhoFail             = rhs.m_rhoFail;
  m_kMinFail            = rhs.m_kMinFail;
  m_kMaxFail            = rhs.m_kMaxFail;

  m_doFitSanityCheck      = rhs.m_doFitSanityCheck;
  m_doChi2OutlierRemoval  = rhs.m_doChi2OutlierRemoval;

  m_fixInputK   = rhs.m_fixInputK;
  m_useLLNorm   = rhs.m_useLLNorm;
  m_fixWidth    = rhs.m_fixWidth;

  m_truncatedRMS = rhs.m_truncatedRMS;
  m_fractionRMS = rhs.m_fractionRMS;
  m_setInitialRMS = rhs.m_setInitialRMS;
  m_minVtxProb = rhs.m_minVtxProb;
}

StatusCode InDetBeamSpotVertex::initialize() {
  ATH_MSG_DEBUG( "In initialize" );
  return StatusCode::SUCCESS;
}

StatusCode InDetBeamSpotVertex::finalize() {
  ATH_MSG_DEBUG( "In finalize" );
  clear(); // clear the data;

  return StatusCode::SUCCESS;
}

void InDetBeamSpotVertex::clear() {
  //reset the data information;

  m_x *= 0.;
  m_p *= 0.;
  
  m_cov *= 0.;
  m_V *= 0.;
  m_z=0.;
  m_zErr =0.;
  m_zSolved =0.;
  m_zErrSolved =0.;

  m_vertexCount =0;
  m_fitStatus = unsolved;

  m_vertexData.clear();

  m_nUsed = 0;
}

InDetBeamSpotVertex::FitStatus InDetBeamSpotVertex::fit(std::vector< BeamSpot::VrtHolder > &vtxData){ 
  //get solution from current data;
  //Note, even if doing LL, use the chi2 information
  m_vertexData = vtxData;
  m_vertexCount = vtxData.size();

  
  bool chi2Result = solveChi2();
  m_getLLres = false;
  bool llResult = false;
  if (m_useLL) {
    applyOutlierRemoval(); // Apply a final selection
    m_getLLres =  true; //set system to use these results
    llResult = solveLL();
    if (!llResult) {
      ATH_MSG_WARNING( "Log-likelihood fit failed: Reverting to chi2 solution" );
      m_getLLres = false;
      chi2Result = solveChi2(); //unfortunately need to resolve, to set correct fit parameters
    }
  }

  if (m_useLL) {
    m_getLLres =  true; //set system to use these results
    llResult = solveLL();
    if (!llResult) {
      ATH_MSG_WARNING( "Log-likelihood fit failed: Reverting to chi2 solution" );
      m_getLLres = false;
      chi2Result = solveChi2(); //unfortunately need to resolve, to set correct fit parameters
    }
  } // if use LL
  
  

  if (m_getLLres) 
   m_fitStatus = (llResult ? successful : failed);
  else
    m_fitStatus = (chi2Result ? successful : failed);

  for ( std::vector< BeamSpot::VrtHolder >::iterator it =  
    m_vertexData.begin();
  it != m_vertexData.end(); ++it) {
    if ( it->valid ) m_nUsed++;
  }  

  if (msgLvl(MSG::INFO)) {
    //display results
    ATH_MSG_INFO( "Fit result was: " << ( m_fitStatus == successful ? "Successful" : "Failure") 
       << " using Vertex " << (m_getLLres ? "Log-likelihood":"Chi2") << " method" );
    ATH_MSG_INFO( "Number of vertices: " << m_vertexCount );
    ATH_MSG_INFO( "x(z=0) = " << getX(0.) << " +- " << getErrX(0) );
    ATH_MSG_INFO( "y(z=0) = " << getY(0.) << " +- " << getErrY(0) );
    ATH_MSG_INFO( "z = " << getZ() << " +- " << getErrZ() );
    double z = getZ();
    ATH_MSG_INFO( "x(z) = " << getX(z) << " +- " << getErrX(z) );
    ATH_MSG_INFO( "y(z) = " << getY(z) << " +- " << getErrY(z) );
    
    ATH_MSG_INFO( "TiltX = " << getTiltX() << " +- " << getErrTiltX() );
    ATH_MSG_INFO( "TiltY = " << getTiltY() << " +- " << getErrTiltY() );
    
    ATH_MSG_INFO( "SigmaX(z=0) = " << getSigmaX(0.) << " +- " << getErrSigmaX(0.) );
    ATH_MSG_INFO( "SigmaY(z=0) = " << getSigmaY(0.) << " +- " << getErrSigmaY(0.) );
    ATH_MSG_INFO( "SigmaZ      = " << getSigmaZ()   << " +- " << getErrSigmaZ() );
    
    ATH_MSG_INFO( "rhoXY = " << getRhoXY() << " +- " << getErrRhoXY() ); 
    ATH_MSG_INFO( "K = " << getK() << " +- " << getErrK() ); 
  }
  
  return m_fitStatus;
}


CLHEP::HepSymMatrix InDetBeamSpotVertex::getCov(double z) const { //x(z),y(z),tiltx,tilty
  CLHEP::HepSymMatrix c(4,0);

  if ( m_getLLres ) {
    //maximum likelihood
    c(1,1) = m_VLL(1,1) + 2*z*m_VLL(3,1) + z*z*m_VLL(3,3);
    c(2,2) = m_VLL(2,2) + 2*z*m_VLL(4,2) + z*z*m_VLL(4,4);
    c(3,3) = m_VLL(3,3);
    c(4,4) = m_VLL(4,4);

    c(2,1) = m_VLL(2,1) + z*m_VLL(2,3) + z*m_VLL(4,1)+z*z*m_VLL(4,3);
    c(3,1) = m_VLL(3,1) + z*m_VLL(3,3);
    c(4,1) = m_VLL(4,1) + z*m_VLL(3,4);

    c(3,2) = m_VLL(2,3) + z*m_VLL(4,3);
    c(4,2) = m_VLL(4,2) + z*m_VLL(4,4);
    
    c(4,3) = m_VLL(4,3);
    
  } else {
    //chi2
    c(1,1) = m_V(1,1) + 2*z*m_V(2,1) + z*z*m_V(2,2);
    c(2,2) = m_V(3,3) + 2*z*m_V(4,3) + z*z*m_V(4,4);
    c(3,3) = m_V(2,2);
    c(4,4) = m_V(4,4);

    c(2,1) = m_V(3,1) + z*m_V(3,2) + z*m_V(4,1)+z*z*m_V(4,2);
    c(3,1) = m_V(2,1) + z*m_V(2,2);
    c(4,1) = m_V(4,1) + z*m_V(4,2);

    c(3,2) = m_V(3,2) + z*m_V(4,2);
    c(4,2) = m_V(4,3) + z*m_V(4,4);
    
    c(4,3) = m_V(4,2);

  }

  return c;
}


bool InDetBeamSpotVertex::solveLL() {

  TMinuit * minuit = new TMinuit(m_NPARS);
  //setInitialPars( minuit);  
  setParsFromChi2(minuit);
  
  //Set Minuit output level
  if (msg().level() > MSG::DEBUG)
    minuit->SetPrintLevel(1);
  else if (msg().level() == MSG::DEBUG)
    minuit->SetPrintLevel(0);
  else minuit->SetPrintLevel(-1);
  
  //  minuit->SetPrintLevel(0);
  if (msgLvl(MSG::VERBOSE)) minuit->SetPrintLevel(1);
  else if (msgLvl(MSG::INFO)) minuit->SetPrintLevel(-1);
  else minuit->SetPrintLevel(0);

  minuit->SetErrorDef(0.5); // for Log-Likeihood: 0.5

  int errFlag =0;
  //SET STRategy for calculating second derivs.
  double arglist[1];
  arglist[0] = 2;
  minuit->mnexcm("SET STR",arglist,1,errFlag);


  // Insert the likelihood function
  // need to make the data accessible to it
  BeamSpot::vertexData = &m_vertexData;
  if ( !m_useLLNorm)
    minuit->SetFCN(BeamSpot::myFCN_LLsolver);
  else {
    minuit->SetFCN(BeamSpot::myFCN_LLsolverNorm);
  }
  
  std::pair<int, std::string>  status; // fit status
  //standard fit
  bool goodFit = false;

  //tried this approach - we want to succeed, so if fail, try another approach
  if (!goodFit) {
    doFit2(minuit);
    successfulFit(minuit, status);
    if ( status.first == 3 && ( status.second == "SUCCESSFUL" ||
        status.second == "CONVERGED " ||
        status.second == "CONVERGED") ){
      goodFit = true;
    } else {
      goodFit = false;
    }
  }

  if (goodFit){
    ATH_MSG_INFO( "Successful fit" );
    setOutput( minuit);
  } else {
    ATH_MSG_DEBUG( "No LL fit convergence" );
  }
  
  delete minuit;
  
  return goodFit;
}

bool InDetBeamSpotVertex::setOutput( TMinuit * minuit) {
  //set the LL fitted parameters to the output.
  //output
  double t;
  for (int i =0; i<m_NPARS; ++i) {
    minuit->GetParameter(i, m_pLL(i+1)  ,t);
  }//for

  //double res[nPar];
  //  double cov[m_NPARS * m_NPARS];
  double * cov = new double[m_NPARS*m_NPARS];
  minuit->mnemat(cov,m_NPARS);
  for (int i =0; i<m_NPARS; ++i) {
    for ( int j=i; j<m_NPARS; ++j) {
      m_VLL(i+1,j+1) = cov[ i*m_NPARS + j]; 
    }//for j
  }//for i
  delete[] cov;
  
  return true;
}



int InDetBeamSpotVertex::setParsFromChi2( TMinuit * minuit) {
  // use results of a chi2 fit to override some initial paramters
  int errFlag;
  minuit->mnparm(0,"X0",m_p(1) ,1e-5,m_init_min_x,m_init_max_x, errFlag);
  minuit->mnparm(1,"Y0",m_p(3) ,1e-5,m_init_min_y,m_init_max_y, errFlag);
  minuit->mnparm(2,"Ax",m_p(2) ,1e-6,m_init_min_ax,m_init_max_ax, errFlag);
  minuit->mnparm(3,"Ay",m_p(4) ,1e-6,m_init_min_ay,m_init_max_ay, errFlag);
  minuit->mnparm(4,"#sigma(x)",m_init_sx, 1e-5,m_init_min_sx,m_init_max_sx, errFlag);
  minuit->mnparm(5,"#sigma(y)",m_init_sy, 1e-5,m_init_min_sy,m_init_max_sy, errFlag);
  minuit->mnparm(6,"#rhoxy",m_init_rhoxy,1e-5,m_init_min_rhoxy,m_init_max_rhoxy, errFlag);
  minuit->mnparm(7,"k",m_init_k, 1e-5,m_init_min_k,m_init_max_k, errFlag);
  minuit->mnparm(8,"z0",m_zSolved, 1e-3,m_init_min_z,m_init_max_z, errFlag);
  minuit->mnparm(9,"#sigma(z)",m_init_sz, 1e-3,m_init_min_sz,m_init_max_sz, errFlag);
  
  minuit->SetMaxIterations(m_minuit_maxIter);

  return errFlag;
}

int InDetBeamSpotVertex::setInitialPars( TMinuit * minuit) {
  int errFlag;
  //Set initial Parameters
  minuit->mnparm(0,"X0",m_init_x ,1e-5,m_init_min_x,m_init_max_x, errFlag);
  minuit->mnparm(1,"Y0",m_init_y ,1e-5,m_init_min_y,m_init_max_y, errFlag);
  minuit->mnparm(2,"Ax",m_init_ax ,1e-6,m_init_min_ax,m_init_max_ax, errFlag);
  minuit->mnparm(3,"Ay",m_init_ay ,1e-6,m_init_min_ay,m_init_max_ay, errFlag);
  minuit->mnparm(4,"#sigma(x)",m_init_sx, 1e-5,m_init_min_sx,m_init_max_sx, errFlag);
  minuit->mnparm(5,"#sigma(y)",m_init_sy, 1e-5,m_init_min_sy,m_init_max_sy, errFlag);
  minuit->mnparm(6,"#rhoxy",m_init_rhoxy,1e-5,m_init_min_rhoxy,m_init_max_rhoxy, errFlag);
  minuit->mnparm(7,"k",m_init_k, 1e-5,m_init_min_k,m_init_max_k, errFlag);
  minuit->mnparm(8,"z0",m_init_z, 1e-3,m_init_min_z,m_init_max_z, errFlag);
  minuit->mnparm(9,"#sigma(z)",m_init_sz, 1e-3,m_init_min_sz,m_init_max_sz, errFlag);
  
  minuit->SetMaxIterations(m_minuit_maxIter);

  return errFlag;
}

bool InDetBeamSpotVertex::solveChi2() {
  ATH_MSG_DEBUG( "Attempting to solve " );
  
  //calulate a solution for the current set of data
  if (m_vertexCount == 0) {
    ATH_MSG_DEBUG( "No vertices stored for solve" );
    m_fitStatus = failed;
    return false; 
  }


  //invert matrix
  int fail;
  m_V = m_cov;
  ATH_MSG_DEBUG( "Matrix prior to inversion\n" << m_V);

  m_V.invert(fail); //replaces the matrix with inverse
  if (fail != 0) {
    return false;
  }
  ATH_MSG_DEBUG( "Matrix post inversion\n" << m_V);


  m_p = m_V*m_x;  // calculate parameter estimates
  m_zSolved = m_z / m_zErr; // weighted 
  m_zErrSolved = 1./sqrt(m_zErr);

  
  ATH_MSG_DEBUG( "Fit solved succesfully from " << m_vertexCount << " vertices");
  ATH_MSG_DEBUG( "Chi2 Solution:\n"<<m_p <<"\n" << "Covariance: \n" << m_V );
 
  return true;
}



bool InDetBeamSpotVertex::applyOutlierRemoval() {
  ATH_MSG_DEBUG( "In Outlier removal" );
  // apply selection using results of a chi2 fit (eg. displacement)
  if (!m_vertexData.size()) {
    ATH_MSG_INFO( "No vertices found" );
    return false;
  }
  static int rCount(0); // counter used for recussive mode

  
  // determine simple means
  double meanx(0.),meany(0.), meanz(0.);
  double meanxSqr(0.),meanySqr(0.), meanzSqr(0.);
  double rmsX(0.),rmsY(0.), rmsZ(0.);
  
  // for use in median determination
  std::vector<double> vx,vy,vz;

  int count(0);
  for ( std::vector< BeamSpot::VrtHolder >::iterator it =  
    m_vertexData.begin();
  it != m_vertexData.end(); ++it) {
    if (!it->valid) continue;

    if (!m_truncatedRMS) {
      meanx += it->x;    
      meanxSqr += (it->x)*(it->x);
      meany += it->y;    
      meanySqr += (it->y)*(it->y); 
      meanz += it->z;
      meanzSqr += (it->z)*(it->z);
      
      ++count;
    }

    vx.push_back(it->x);
    vy.push_back(it->y);
    vz.push_back(it->z);   
  }

  // sort the vectors
  std::sort(vx.begin(),vx.end());
  std::sort(vy.begin(),vy.end());
  std::sort(vz.begin(),vz.end());

  // get the median values -- don't worry about extrapolating betweeen even sized entries
  double medianx = (vx.size() > 1 ?  vx.at(vx.size()/2) : 0.);
  double mediany = (vy.size() > 1 ?  vy.at(vy.size()/2) : 0.);
  double medianz = (vz.size() > 1 ?  vz.at(vz.size()/2) : 0.);

  ATH_MSG_DEBUG(  "Median: x: " << medianx  
            <<  " y: " << mediany 
            <<  " z: " << medianz );

  if (m_truncatedRMS) {

    ATH_MSG_DEBUG( "Truncating RMS ... " );
    // Only use the fraction (95%) of tracks closest to the median position to determin the RMS
    int nvtx(vx.size());
    std::sort(vx.begin(), vx.end(), SortDistToMedian(medianx));
    vx.erase(vx.begin()+int(m_fractionRMS*nvtx), vx.end());
    
    std::sort(vy.begin(), vy.end(), SortDistToMedian(mediany));
    vy.erase(vy.begin()+int(m_fractionRMS*nvtx), vy.end());
    
    std::sort(vz.begin(), vz.end(), SortDistToMedian(medianz));
    vz.erase(vz.begin()+int(m_fractionRMS*nvtx), vz.end());
    
    ATH_MSG_DEBUG( "... using " << int(m_fractionRMS*nvtx) << " from " << nvtx << " vertices" );

    for (unsigned int ivtx(0); ivtx < vx.size(); ++ivtx) {
      double x = vx.at(ivtx);
      double y = vy.at(ivtx);
      double z = vz.at(ivtx);
      
      meanx += x;
      meanxSqr += x*x;
      meany += y;
      meanySqr += y*y;
      meanz += z;
      meanzSqr += z*z;
       
      ++count;
    }
  }

  if (count) {
    meanx /= count;
    meanxSqr /=count;
    rmsX = sqrt( fabs(meanxSqr - meanx*meanx));
    meany /= count;
    meanySqr /=count;
    rmsY = sqrt( fabs(meanySqr - meany*meany));
    meanz  /=count;
    meanzSqr /=count;
    rmsZ = sqrt( fabs(meanzSqr - meanz*meanz));
  }

  if(m_setInitialRMS){
    rmsX = m_init_sx;
    rmsY = m_init_sy;
    // should we include z here too??

    ATH_MSG_DEBUG( "Setting RMS in x to " << rmsX );
    ATH_MSG_DEBUG( "Setting RMS in y to " << rmsY );

  }

  ATH_MSG_DEBUG(  "mean, RMS; x: " << meanx << " " << rmsX
    << "   "  << ", y: " << meany << " " << rmsY <<  ", z: " << meanz << " " << rmsZ );


  // varivables to hold the new chi2 and weighted z-mean values
  CLHEP::HepVector chi2Pos(4);
  CLHEP::HepSymMatrix chi2Cov(4);
  double  zpos(0), zerr(0);
  
  
  // use simple mean and RMS to eliminate any vertices too far from centroid
  // also redetermine a new chi2 value, based on passed vertices
  long failCount(0);
  for ( std::vector< BeamSpot::VrtHolder >::iterator it =  
    m_vertexData.begin();
  it != m_vertexData.end(); ++it) {
    if (!it->valid) continue;
    int fail=0;
    //if (  fabs( meanx - it->x ) > m_sigTr * rmsX ) fail += 1;
    //if (  fabs( meany - it->y ) > m_sigTr * rmsY ) fail += 2;
    if ( fabs( medianx - it->x ) > m_sigTr *rmsX) fail += 4;
    if ( fabs( mediany - it->y ) > m_sigTr *rmsY) fail += 8;
    if ( fabs( medianz - it->z ) > 10*rmsZ) fail += 16;
    
    /*
    if (  (meanx - it->x)*(meanx-it->x)/rmsX/rmsX + (meany-it->y)*(meany-it->y)/rmsY/rmsY > m_sigTr*m_sigTr) {
      if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG( "Vertex info: extended past radial extent: sig." 
                << sqrt((meanx - it->x)*(meanx-it->x)/rmsX/rmsX + (meany-it->y)*(meany-it->y)/rmsY/rmsY) << " > "
                << sqrt( m_sigTr*m_sigTr ) << "." );
      // don't remove these vertices yet 
    }
    */
    if (  (medianx - it->x)*(medianx-it->x)/rmsX/rmsX + (mediany-it->y)*(mediany-it->y)/rmsY/rmsY > m_sigTr*m_sigTr) {
      ATH_MSG_DEBUG( "Vertex info: extended past radial extent: sig."
                     << sqrt((medianx - it->x)*(medianx-it->x)/rmsX/rmsX + (mediany-it->y)*(mediany-it->y)/rmsY/rmsY) << " > "
                     << sqrt( m_sigTr*m_sigTr ) << "." ); 
      fail += 128;
    }
    
    //and in Z?
    
    if (fail) { // TBD only allow failed vertices here to be removed on every nth iteration (to aid stability)
      ATH_MSG_DEBUG( "Vertex reject from simple mean; reason: " << fail << " : x,y,z: " 
        << it->x << "  " << it->y << "  " << it->z 
        << " , sigma(x,y,z): " << sqrt(it->vxx) << "  " << sqrt(it->vyy)
        << "  " << sqrt(it->vzz) 
        );
      
      it->valid = false;
      //      it->outlierRemoved = true;
      ++failCount;
    } else {
      // still valid, so add into chi2
      
      chi2Pos(1) += it->x * it->vxx          + it->y*it->vxy;
      chi2Pos(2) += it->x*it->vxx*it->z     + it->y*it->vxy*it->z;
      chi2Pos(3) += it->y*it->vyy            + it->x*it->vxy;
      chi2Pos(4) += it->y*it->vyy*it->z     + it->x*it->vxy*it->z;
      
      
      chi2Cov.fast(1,1) += it->vxx;
      chi2Cov.fast(2,1) += it->vxx*it->z;
      chi2Cov.fast(2,2) += it->vxx*it->z*it->z;
      chi2Cov.fast(3,1) += it->vxy;
      chi2Cov.fast(3,2) += it->vxy*it->z;
      chi2Cov.fast(3,3) += it->vyy;
      chi2Cov.fast(4,1) += it->vxy*it->z;
      chi2Cov.fast(4,2) += it->vxy*it->z*it->z;
      chi2Cov.fast(4,3) += it->vyy*it->z;
      chi2Cov.fast(4,4) += it->vyy*it->z*it->z;
      
      zpos += it->z/it->vzz; //weighted
      zerr += 1./it->vzz;
    }// end of valid vertex
    
  }// for 
  ATH_MSG_DEBUG( "Removed: " << failCount << " vertices from simple mean,RMS." );
  
  
  // update the new global chi2  - need to do this after the invert has been successful though !!!
  m_cov = chi2Cov;
  m_x   = chi2Pos;
  m_z   = zpos;
  m_zErr =zerr;
  // attempt to solve the new chi2 solution
  int  invFail(0);
  chi2Cov.invert(invFail);
  chi2Pos = chi2Cov*chi2Pos;
  zpos    = zpos / zerr;
  zerr    = 1./sqrt(zerr);
  
  
  
  
  // make comparisons of old chi2, new mean and new chi2 values
  if (msgLvl(MSG::DEBUG) ) {
    ATH_MSG_DEBUG( "Mean position: x,y,z " << meanx << "  " << meany << "  " << meanz );
    ATH_MSG_DEBUG( "     RMS:      x,y,z " << rmsX  << "  " << rmsY  << "  " << rmsZ  );
    
    ATH_MSG_DEBUG( "Original chi2:" << m_p << "\n" << m_V << "\n" << m_zSolved << "  " << m_zErrSolved );
    
    ATH_MSG_DEBUG( "New      chi2:" << chi2Pos << "\n" << chi2Cov << "\n" << zpos << "  " << zerr );

  } // debug statement
  
  m_V = chi2Cov;
  m_p = chi2Pos;
  m_zSolved = zpos;
  m_zErrSolved = zerr;
  
  
  /* TBD
  if (goodChi2) {
    if (msgLvl(MSG::DEBUG)) )  ATH_MSG_DEBUG( "Replacing original chi2 with new value" );
    
    
  }
  */
  
  //Dubious - !!! TBD: move this into the general LL solve area and do it properly!
  //m_init_sx = rmsX;
  //m_init_sy = rmsY;
  m_init_sz = rmsZ;
  
  // perform LL fit?
  bool llSolve(false);
  m_getLLres = false;
  if (m_useLL) {
    llSolve = solveLL();
    m_getLLres = llSolve; // allow the log-likelihood accessor values to returned, if sucessful
  }

  if ( llSolve and rCount > 0 ) {
    ATH_MSG_INFO( "Log-Likelihood fit converged in outlier removal. Exiting outlier removal." );
    return true;
  }
  
  CLHEP::HepSymMatrix bsCov(2); // for individual chi2 values
  double xbar = 0.;
  double ybar = 0.;
  double ax   = 0.;
  double ay   = 0.;
  
  
  
  if ( llSolve == false || getSigmaX(0.) < m_widthFail || getSigmaY(0.) < m_widthFail || fabs(getRhoXY()) > m_rhoFail ) {
    // ll solution not used, or not trusted
    // set a wide solution
    m_getLLres = false;
    ATH_MSG_INFO( ": removeOutliers: LL fit not use/converged/trusted - " <<
      "using chi2 for mean and simple RMS for width values " );
    ATH_MSG_DEBUG( llSolve << " " << getSigmaX(0.) << " " << getSigmaY(0.) << " " << getRhoXY() );
    xbar = getX(0.);
    ybar = getY(0.);
    ax  =  getTiltX();
    ay  =  getTiltY();

    bsCov(1,1) = rmsX*rmsX;
    bsCov(2,2) = rmsY*rmsY;
    bsCov(2,1) = 0.;
    
  } else { // ll fit succeeded
    xbar = getX(0.);
    ybar = getY(0.);
    ax  =  getTiltX();
    ay  =  getTiltY();
    
    
    bsCov(1,1) = getSigmaX(0.)*getSigmaX(0.);
    bsCov(2,2) = getSigmaY(0.)*getSigmaY(0.);
    bsCov(2,1) = getSigmaX(0.)*getSigmaY(0.) * getRhoXY();
    
  } // 
  
  // loop over all vertices and populate a map with the chi2 values of the vertices
  std::multimap<double, BeamSpot::VrtHolder*> chi2map;
  int fail(0);
  long fCount(0);
  for ( std::vector< BeamSpot::VrtHolder >::iterator it =  m_vertexData.begin();
       it != m_vertexData.end(); ++it) {
    if ( !it->valid) continue; // ignore vertices set to invalid
    fail = 0; // reset the fail value variable
    
    // selections
    if ( fabs(it->x - (xbar + it->z*ax))  > m_sigTr * rmsX) fail += 1;
    if ( fabs(it->y - (ybar + it->z*ay))  > m_sigTr * rmsY) fail += 2;
    
    if ( it->vxx >  m_maxVtxErTr*m_maxVtxErTr || it->vyy >  m_maxVtxErTr*m_maxVtxErTr) fail += 4;

    if (  fabs(it->z - meanz)  > m_sigTr * rmsZ) fail += 8;

    // add all other selections above here:
    double increaseChi2(0);
    if (fail) {
      increaseChi2 = fail * 1e5;
    }
    
    
    // chi2 value in transversve plane
    CLHEP::HepSymMatrix b(2);
    // TEST TBD !!!
    
    b(1,1) = it->vxx + bsCov(1,1);
    b(2,2) = it->vyy + bsCov(2,2);
    b(2,1) = it->vxy + bsCov(2,1);
    /*
    b(1,1) = it->vxx;
    b(2,2) = it->vyy;
    b(2,1) = it->vxy;
    */
    int failInv =0;
    b.invert(failInv);
    if (failInv) continue;
    double ch = (it->x - (xbar + it->z*ax)) * b(1,1) * (it->x - (xbar + it->z*ax))
    + (it->y - (ybar + it->z*ay)) * b(2,2) * (it->y - (ybar + it->z*ay))
    + 2*(it->x - (xbar + it->z*ax)) *b(2,1) * (it->y - (ybar + it->z*ay));
    
    //if (ch > m_outlierChi2Tr ) fail += 128; fail is never used after this

    // if vertex fails selection based on [1,2,4] add a large artificial chi2 value to make sure is removed.
    ch += increaseChi2;
    
    chi2map.insert(std::make_pair(ch, &(*it))); // add to list
    
  } // end for 
  
  // from the map remove the largest chi2 values, up to some maximum number of vertices
  fCount = 0;
  for (std::multimap<double, BeamSpot::VrtHolder*>::reverse_iterator vit =  chi2map.rbegin(); vit != chi2map.rend(); ++vit) {
    if ( !vit->second) continue; // no valid pointer ... 
    if ( !vit->second->valid) continue; // ignore vertices set to invalid - shouldn't happen here though
    if ( fCount >= m_singleIterationMax && m_singleIterationMax != -1) {
      ATH_MSG_DEBUG( " removeOutlier: Reached max number of vertex rejections for this iteration.\n" 
        << "\tNeed to recalculate mean positions." );
      break;
    } // if reached max iterations for this round

    ATH_MSG_VERBOSE( "Vertex chi2: " << vit->first  );


    
    if ( vit->first  < m_outlierChi2Tr ) {
      if (msgLvl(MSG::DEBUG)) {
        ATH_MSG_DEBUG( " No more 'bad' vertices found in this iteration."  );
        if (fCount == 0) {
          ATH_MSG_DEBUG( " No futher vertices removed - moving to final iteration" );
        } else {
          ATH_MSG_DEBUG( " Moving to next iteration of outlier removal." );
        }
      }
      break;
    } // if 
    
    // any vertex that is found here should be removed
    ++fCount;
    vit->second->valid = false;
    //    vit->second->outlierRemoved = true;
    ATH_MSG_DEBUG( "Vertex rejected; chi2: " << vit->first <<". pos(x,y,z): " 
      << vit->second->x << "  " << vit->second->y << "  " << vit->second->z 
      << " , sigma(x,y,z): " << sqrt(vit->second->vxx) << "  " << sqrt(vit->second->vyy)
      << "  " << sqrt(vit->second->vzz) 
      );
                   
  } // for
  
  // if no vertices removed and ll fit still fails, then we continue to have a problem ... 
  if (fCount == 0 && m_useLL && !llSolve && rCount !=0 ) { // if first iteration, we have another iteration later.
    //    if (msgLvl(MSG::WARNING)) ATH_MSG_WARNING( "In LL test MODE !!!!" );
    ATH_MSG_WARNING( "No vertices removed and fit still fails - most likely final result will fail"  );
    
    // this is our 'last-ditch approach'. Split the collection of vertices into two 'random' sets and solve for each.
    // if both successful, then compare (and be a little confused). If only one, compare to chi2 and take if ok.
    // if none, then really have to give up. No valid solution possible.

    //    m_vertexData // key holder.
    //
    std::vector< BeamSpot::VrtHolder > vertexTemp;
    // copy the original values
    std::copy(m_vertexData.begin(),m_vertexData.end(), back_inserter(vertexTemp)) ; 
    // muddle up the original
    std::random_shuffle(m_vertexData.begin(), m_vertexData.end());
    // divide the sample into two
    std::vector< BeamSpot::VrtHolder > vertex1,vertex2;
    std::copy(m_vertexData.begin() + m_vertexData.size()/2, m_vertexData.end(),    back_inserter(vertex2)) ; 
    std::copy(m_vertexData.begin(),  m_vertexData.begin() + m_vertexData.size()/2, back_inserter(vertex1)) ; 
    
    bool goodFit1(false), goodFit2(false);

    ATH_MSG_DEBUG( "Attempting fit with vextex half: 1");
    m_vertexData = vertex1;
    llSolve = solveLL();
    //m_getLLres = llSolve; // allow the log-likelihood accessor values to returned, if sucessful
    m_getLLres = true;
    ATH_MSG_WARNING( "Fit using \"vertex1\"  " << ( llSolve ? "Successful": "Failed") );
    if (llSolve) {
      goodFit1 = true;

      if (msgLvl(MSG::DEBUG)) {
        ATH_MSG_DEBUG( "x:  " << getX(0) << " +- " << getErrX(0) );
        ATH_MSG_DEBUG( "y:  " << getY(0) << " +- " << getErrY(0) );
        ATH_MSG_DEBUG( "z:  " << getZ() << " +- " << getErrZ() );
        ATH_MSG_DEBUG( "sx: " << getSigmaX(0) << " +- " << getErrSigmaX(0) );
        ATH_MSG_DEBUG( "sy: " << getSigmaY(0) << " +- " << getErrSigmaY(0) );
        ATH_MSG_DEBUG( "sz: " << getSigmaZ() << " +- " << getErrSigmaZ() );
        ATH_MSG_DEBUG( "ax: " << getTiltX() << " +- " << getErrTiltX() );
        ATH_MSG_DEBUG( "ay: " << getTiltY() << " +- " << getErrTiltY() );
        ATH_MSG_DEBUG( "sxy:" << getSigmaXY(0) << " +- " << getErrSigmaXY(0) );
        ATH_MSG_DEBUG( "rh: " << getRhoXY() << " +- " << getErrRhoXY() );
        ATH_MSG_DEBUG( "k:  " << getK() << " +- " << getErrK() );
      }// debug

    } // good fit

    ATH_MSG_DEBUG( "Attempting fit with vextex half: 2");
    m_vertexData = vertex2;
    llSolve = solveLL();
    //    m_getLLres = llSolve; // allow the log-likelihood accessor values to returned, if sucessful
    m_getLLres = true;
    ATH_MSG_WARNING( "Fit using \"vertex2\"  " << ( llSolve ? "Successful": "Failed") );
    if (llSolve) { 
      goodFit2 = true;
      if (msgLvl(MSG::DEBUG)) {
        ATH_MSG_DEBUG( "x:  " << getX(0) << " +- " << getErrX(0) );
        ATH_MSG_DEBUG( "y:  " << getY(0) << " +- " << getErrY(0) );
        ATH_MSG_DEBUG( "z:  " << getZ() << " +- " << getErrZ() );
        ATH_MSG_DEBUG( "sx: " << getSigmaX(0) << " +- " << getErrSigmaX(0) );
        ATH_MSG_DEBUG( "sy: " << getSigmaY(0) << " +- " << getErrSigmaY(0) );
        ATH_MSG_DEBUG( "sz: " << getSigmaZ() << " +- " << getErrSigmaZ() );
        ATH_MSG_DEBUG( "ax: " << getTiltX() << " +- " << getErrTiltX() );
        ATH_MSG_DEBUG( "ay: " << getTiltY() << " +- " << getErrTiltY() );
        ATH_MSG_DEBUG( "sxy:" << getSigmaXY(0) << " +- " << getErrSigmaXY(0) );
        ATH_MSG_DEBUG( "rh: " << getRhoXY() << " +- " << getErrRhoXY() );
        ATH_MSG_DEBUG( "k:  " << getK() << " +- " << getErrK() );
      }// debug

    }

    // what now ? ...
    ATH_MSG_WARNING( "Fit was " <<  ( goodFit2 || goodFit1 ? "Successful ": "Unsuccessful ") 
            <<"  using a subset of the available vertices" );
    if (( goodFit2 || goodFit1) )
      ATH_MSG_WARNING( "Using these subset data for final result!!! " );
    ATH_MSG_WARNING( "FIT HALFVERTX" );

    if (goodFit1) {
      m_vertexData = vertex1;
    } else if (goodFit2) {
      m_vertexData = vertex2;
    } else {
      m_vertexData = vertexTemp; // give up and go home...
    }
    /*
    // for test mode only 
    m_vertexData = vertexTemp;
    */
  } // last solution
  
  
  
  // recursive mode
  ATH_MSG_DEBUG( " Recursive debug: Loop: " << rCount << ". Number of failed vertices: " << fCount );
  
  ++rCount;
  if ( fCount > 0 || ( fCount == 0 && rCount == 1 && !llSolve)) { // if failed vertices or, no failed, first iteration, and no succesful fit
    if ( rCount > m_maxOutlierLoops) {
      ATH_MSG_WARNING( "OutlierRemoval: Reached maximum number of recursive loops: " << rCount 
        << ". No more iterations performed." );
    } else {
      ATH_MSG_DEBUG( "OutlierRemoval: Entering recursive loop: " << rCount );
      applyOutlierRemoval();
    } // if entering loop
  } // if fails > 0
  --rCount;
  return true;
} // outlier removal


bool InDetBeamSpotVertex::successfulFit( TMinuit * minuit, 
               std::pair<int, std::string> & status)  {
  if (!minuit) return false;
  //This should be called directly after the fit
  std::string sRes = minuit->fCstatu.Data();

  Double_t fmin, fedm, errdef;
  Int_t npari,nparx,istat;
  minuit->mnstat(fmin, fedm, errdef,npari,nparx,istat);
  
  ATH_MSG_DEBUG( "Fit reports status: " << istat << " and " << sRes );

  status.first = istat;
  status.second = sRes;

  bool sanityPassed(true);
  if ( m_doFitSanityCheck) {
    double x(0), ex(0);
    minuit->GetParameter(6,x,ex); // rhoxy
    if ( fabs(x) > m_rhoFail){
      sanityPassed = false;
      ATH_MSG_DEBUG( "Fit Failed with rhoxy: " << x << " > " << m_rhoFail );
    }
    minuit->GetParameter(4,x,ex); // sigma x
    if ( fabs(x) < m_widthFail ){
      sanityPassed = false;
      ATH_MSG_DEBUG( "Fit Failed with sigmaX:" << x << " > " << m_widthFail );
    }
    minuit->GetParameter(5,x,ex); // sigma y
    if ( fabs(x) < m_widthFail ){
      sanityPassed = false;
      ATH_MSG_DEBUG( "Fit Failed with sigmaY: " << x << " > " <<m_widthFail  );
    }
    
    minuit->GetParameter(7,x,ex); // k
    if ( fabs(x) < m_kMinFail || fabs(x) > m_kMaxFail ){
      sanityPassed = false;
      ATH_MSG_DEBUG( "Fit Failed with k: " << x << " > " << m_kMaxFail 
                 << ", or " << x << " < " << m_kMinFail  );
    }


    
  } // sanity check
  if (!sanityPassed) {
    status.first = 99;
    status.second = "FAILED BEAMSPOT SANITY CHECK";
  }
  ATH_MSG_DEBUG( "Fit " << ( sanityPassed ? "Passed": "Failed") << " sanity check: " );

  if ( istat != 3) return false;


  return true;
}


void BeamSpot::myFCN_LLsolver( Int_t &, Double_t *, Double_t &f, Double_t *par, Int_t) {
//void Beamspot::myFCN_LLsolver( Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag) {
  const static double Pi = TMath::Pi();
  //par[*]
  //0,  1,  2,   3,  4,  5,    6,       7,   8    9
  //X0, Y0, Ax.  Ay, sx, sy, rhoxy,   k     z0    sigma(z)
  
  f = 0;
  
  typedef  std::vector<  BeamSpot::VrtHolder >  Vertices;
  Vertices::const_iterator vit = BeamSpot::vertexData->begin();
  
  double temp =0;
  double x=0,y=0,z=0;
  double vxx,vyy, vxy;
  double covXX,covYY,covXY;
  double det,k2;
  
  // ln L = Sum[ ln(F) ]
  for ( ; vit != vertexData->end(); ++vit) {
    if (!vit->valid) continue; // don't use non-valid vertices
    temp =0;
    x = vit->x;
    y = vit->y;
    z = vit->z;
    vxx = vit->vxx;
    vxy = vit->vxy;
    vyy = vit->vyy;
    
    
    k2 = par[7]*par[7];
    
    covXX = k2 *vxx +  par[4]*par[4];
    covYY = k2 *vyy +  par[5]*par[5];
    covXY = k2 *vxy +  par[6] *par[4]* par[5];
    
    det = covXX * covYY - covXY*covXY;
    double recDet = 1./det;

    //temp =  TMath::Log(2*Pi * sqrt(fabs(det)));
    temp  =  2*TMath::Log(2*Pi);
    temp  += TMath::Log(det);
    
    covXY = -covXY * recDet;
    double t = covXX *recDet;
    covXX = covYY *recDet;
    covYY = t;
    
    temp += ( 
              ( x - par[0] - par[2]*z) * covXX *  ( x - par[0] - par[2]*z) 
              +   ( y - par[1] - par[3]*z) * covYY *  ( y - par[1] - par[3]*z) 
              + 2*( x - par[0] - par[2]*z) * covXY *  ( y - par[1] - par[3]*z)
              );
    
    temp +=  TMath::Log( 2*Pi * par[9]*par[9] )  +  ( z - par[8]) * (z-par[8]) / (par[9] * par[9] );
    f+= 0.5*temp;
  }//for
  
  
}//myFCN

double BeamSpot::pdfxy(double *, double *) {
  /*
  double x = xx[0];
  double y = yy[0];
  
  double mux =p[0];
  double muy =p[1];
  double ax  =p[2];
  double ay  =p[3];
  double det = p[4];
  double covXX =p[5];
  double covYY =p[6];
  double covXY =p[7];


  double pxy = 1/(2*Pi*det)*exp( -0.5* (  ( x - mux -ax*z) * covXX *  ( x - mux - ax*z) 
            +   ( y - muy - ay*z) * covYY *  ( y - muy - ay*z) 
            + 2*( x - mux - *z) * covXY *  ( y - muy - ay*z)
            )
         );
  return pxy;
  */
  return 0; // TBD dlete
}



void BeamSpot::myFCN_LLsolverNorm( Int_t &, Double_t *, Double_t & /*f*/, Double_t * /*par*/, Int_t) {

}
/*
//void Beamspot::myFCN_LLsolver( Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag) {
  const static double Pi = TMath::Pi();
  //par[*]
  //0,  1,  2,   3,  4,  5,    6,       7,   8    9
  //X0, Y0, Ax.  Ay, sx, sy, rhoxy,   k     z0    sigma(z)
  
  f = 0;
  
  typedef  std::vector<  BeamSpot::VrtHolder >  Vertices;
  Vertices::const_iterator vit = BeamSpot::vertexData->begin();
  
  double temp =0;
  double x=0,y=0,z=0;
  double vxx,vyy, vxy;
  double covXX,covYY,covXY;
  double det,k2;
  
  // ln L = Sum[ ln(F) ]
  for ( ; vit != vertexData->end(); ++vit) {
    if (!vit->valid) continue; // don't use non-valid vertices
    temp =0;
    x = vit->x;
    y = vit->y;
    z = vit->z;
    vxx = vit->vxx;
    vxy = vit->vxy;
    vyy = vit->vyy;
    
    
    k2 = par[7]*par[7];
    
    covXX = k2 *vxx +  par[4]*par[4];
    covYY = k2 *vyy +  par[5]*par[5];
    covXY = k2 *vxy +  par[6] *par[4]* par[5];
    
    det = covXX * covYY - covXY*covXY;
    double recDet = 1./det;
    
    //temp =  TMath::Log(2*Pi * sqrt(fabs(det)));
    temp  =  2*TMath::Log(2*Pi);
    temp  += TMath::Log(det);
    
    covXY = -covXY * recDet;
    double t = covXX *recDet;
    covXX = covYY *recDet;
    covYY = t;
   

  }//for
  
  f=-f;
}//myFCN
*/
 /*
 temp += ( 
              ( x - par[0] - par[2]*z) * covXX *  ( x - par[0] - par[2]*z) 
              +   ( y - par[1] - par[3]*z) * covYY *  ( y - par[1] - par[3]*z) 
              + 2*( x - par[0] - par[2]*z) * covXY *  ( y - par[1] - par[3]*z)
              );
        
        temp +=  TMath::Log( 2*Pi * par[9]*par[9] )  +  ( z - par[8]) * (z-par[8]) / (par[9] * par[9] );
    */
    
    // apply normalisations

    /*
    double val[]  = {x,y};
    double pval[] = { par[0],par[2],par[1],par[3],det, covXX, covYY, covXY };

    Beamspot::transXY->SetParameters(pval);
    double pxy = Beamspot::transXY(
    double normxy = TMath::Integral( 

    double pz  = 1/sqrt(2*Pi*par[9]) * exp( -0.5*( ( z - par[8]) * (z-par[8]) / (par[9] * par[9] )));
    double normz  = 0.5*TMath::Erf(  BeamSpot::norm_zMin / (sqrt(2) * par[9] )); 
    */



void InDetBeamSpotVertex::doFit2( TMinuit * minuit) {
  //second attempt to fit in a controlled way.
  //reset initial values
  setParsFromChi2(minuit);
  
  //fix k, and rho,
  minuit->FixParameter(6);
  minuit->FixParameter(7);
  
  //fix x0,y0,ax,ay
  minuit->FixParameter(0);
  minuit->FixParameter(1);
  minuit->FixParameter(2);
  minuit->FixParameter(3);

  if(m_fixWidth){
    minuit->FixParameter(4);
    minuit->FixParameter(5);
  }

  minuit->Migrad();
  minuit->Migrad();
  //release k,rho
  if ( !m_fixInputK){
    minuit->Release(7);
    minuit->Migrad();
  }

  if(!m_fixWidth){
    minuit->Release(6);
    minuit->Migrad();
  }

  minuit->Release(0);
  minuit->Release(1);
  minuit->Release(2);
  minuit->Release(3);
  minuit->Migrad();
  minuit->SetPrintLevel(0);
  minuit->Migrad();
  //look at fit status from calling function
}


std::map<std::string,double> InDetBeamSpotVertex::getCovMap() const {

  //Note: all the off-diagonal elements are errors calculated at (0,0,0).
  //While the diagonal elements are calculated at the z centroid
  //In practice, this should make little difference, but it is important to note for now.

  std::map<std::string,double> covMap;
  std::vector<double> covVector;
  covVector.resize(55);

  //This is the method that was used before to put the covariance matrix in the required order
  //We don't want to mess with this, because no one knows the original order

  //static const int map[] = {0,0,6,-1,-1,-1,-1,2,-2,-2};
  int map[] = {1,2,9,3,4,5,6,10,7,8};
  if(m_fixInputK){
    //            1 2 3  4  5  6  7 8  9 10 
    //int map2[] = {0,0,5,-1,-1,-1,-1,0,-2,-2};
    int map2[] = {1,2,8,3,4,5,6,9,7,10};
    for(int i=0; i < 10; ++i){
      map[i] = map2[i];
    }
  } else if (m_fixWidth) {
    //int map2[] = {0,0,3,-1,-1,-5,-6,-1, -8,-5};
    int map2[] = {1,2,6,3,4,8,9,7,10,5};
    for(int i=0; i < 10; ++i){
      map[i] = map2[i];
    }
  }
 
  int temp = 0;
  for (int i=0;i<10;++i) {
    for (int j=i;j<10;++j) {
      if( m_fixInputK && (i == 9 || j ==9 )){ 
        covVector[temp++] = 0; 
      } else if ( m_fixWidth && ( i == 5 || i == 6 || i == 8 || j == 5 || j == 6 || j == 8  ) ){
        covVector[temp++] = 0;
      }else{  
        covVector[temp++] = m_VLL( map[i], map[j] );
      }
    }
  }

  //This array is in the order required from the original ntuple format

  const std::string keyArr[] = {"posXErr","covXY","covXZ","covXTiltX","covXTiltY","covXSx","covXSy","covXSz","covXRhoXY","covXk",
                                "posYErr","covYZ","covYTiltX","covYTiltY","covYSx","covYSy","covYSz","covYRhoXY","covYk",
                                "posZErr","covZTiltX","covZTiltY","covZSx","covZSy","covZSz","covZRhoXY","covZk",
                                "tiltXErr","covTiltXTiltY","covTiltXSx","covTiltXSy","covTiltXSz","covTiltXRhoXY","covTiltXk",
                                "tiltYErr","covTiltYSx","covTiltYSy","covTiltYSz","covTiltYRhoXY","covTiltYk",
                                "sigmaXErr","covSxSy","covSxSz","covSxRhoXY","covSxk",
                                "sigmaYErr","covSySz","covSyRhoXY","covSyk",
                                "sigmaZErr","covSzRhoXY","covSzk",
                                "rhoXYErr","covRhoXYk",
                                "kErr"};

  /*  const std::string keyArr2[] = {"posXErr","covYX","covZX","covTiltXX","covTiltYX","covSxX","covSyX","covSzX",
         "covRhoXYX","covkX","posYErr","covZY","covTiltXY","covTiltYY","covSxY","covSyY",
         "covSzY","covRhoXYY","covkY","posZErr","covTiltXZ","covTiltYZ","covSxZ","covSyZ","covSzZ",
         "covRhoXYZ","covkZ","tiltXErr","covTiltYTiltX","covSxTiltX","covSyTiltX","covSzTiltX","covRhoXYTiltX",
         "covkTiltX","tiltYErr","covSxTiltY","covSyTiltY","covSzTiltY","covRhoXYTiltY","covkTiltY","sigmaXErr",
         "covSySx","covSzSx","covRhoXYSx","covkSx","sigmaYErr","covSzSy","covRhoXYSy","covkSy","sigmaZErr","covRhoXYSz",
         "covkSz","rhoXYErr","covkRhoXY","kErr"};
  */
  //Now that everything should be in the right order, it's simple to set the covariance matrix map correctly:
  
  
  for(int i = 0; i < 55; i++){
    covMap[keyArr[i]] = covVector[i];
    //std::cout << keyArr[i] << "  " <<  covVector[i] <<  std::endl;
    //covMap[keyArr2[i]]= covVector[i];
  }

  covMap[ keyArr[0] ]  = sqrt(covVector[0]);
  covMap[ keyArr[10] ] = sqrt(covVector[10]);
  covMap[ keyArr[19] ] = sqrt(covVector[19]);
  covMap[ keyArr[27] ] = sqrt(covVector[27]);
  covMap[ keyArr[34] ] = sqrt(covVector[34]);
  covMap[ keyArr[40] ] = sqrt(covVector[40]);
  covMap[ keyArr[45] ] = sqrt(covVector[45]);
  covMap[ keyArr[49] ] = sqrt(covVector[49]);
  covMap[ keyArr[52] ] = sqrt(covVector[52]);
  covMap[ keyArr[54] ] = sqrt(covVector[54]);
  
  
  //The errors on these 5 parameters were calculated at (0,0,0). This is how we convert them to be
  //at the centroid

  double z = getZ();
  CLHEP::HepSymMatrix covc = getCov(z);

  covMap["posXErr"]    = sqrt( covc(1,1) ); //xcxc
  covMap["posYErr"]    = sqrt( covc(2,2) ); //ycyc
  covMap["tiltXErr"]   = sqrt( covc(3,3) ); //axcaxc
  covMap["tiltYErr"]   = sqrt( covc(4,4) ); //aycayc
  covMap["sigmaXErr"]  = sqrt( getErrSigmaX(z)*getErrSigmaX(z) ); //sxcsxc
  covMap["sigmaYErr"]  = sqrt( getErrSigmaY(z)*getErrSigmaY(z) ); //sycsyc
    
  return covMap;

  
}
std::map<std::string,double> InDetBeamSpotVertex::getParamMap() const {
  double z = getZ();

  std::map<std::string,double> paramMap;
  paramMap["tiltX"]  = (m_getLLres ? m_pLL(3) : m_p(2));
  paramMap["tiltY"]  = (m_getLLres ? m_pLL(4) : m_p(4));
  paramMap["k"]      = (m_getLLres ? m_pLL(8) : 0.);
  paramMap["posX"]   = (m_getLLres ? m_pLL(1) + m_pLL(3)*z  : m_p(1) + m_p(2)*z);
  paramMap["posY"]   = (m_getLLres ? m_pLL(2) + m_pLL(4)*z  : m_p(3) + m_p(4)*z);
  paramMap["posZ"]   = (m_getLLres ? m_pLL(9) : m_zSolved);
  paramMap["sigmaX"] = (m_getLLres ? m_pLL(5) : m_def_sx);
  paramMap["sigmaY"] = (m_getLLres ? m_pLL(6) : m_def_sy);
  paramMap["sigmaZ"] = (m_getLLres ? m_pLL(10) : m_def_sz);
  paramMap["rhoXY"]  = (m_getLLres ? m_pLL(7) : 0.);
  paramMap["nUsed"] = m_nUsed;

  return paramMap;
}
