/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArWFParamTool.h" 
#include "CaloIdentifier/LArEM_ID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArCabling/LArOnOffIdMapping.h"

#include <math.h>
#ifndef __APPLE__
#include <values.h>
#else
#include <float.h>
#endif 
#include <cassert>

#ifndef LAR_MAX_DOUBLE
#ifndef __APPLE__
#define LAR_MAX_DOUBLE MAXDOUBLE
#else
#define LAR_MAX_DOUBLE __DBL_MAX__
#endif
#endif

const double LArWFParamTool::m_DEFAULT = -1;
const double LArWFParamTool::m_TINY=2.22045e-16; 
const double LArWFParamTool::m_SQRT_EPSILON=1.49012e-08;
const double LArWFParamTool::m_EPSILON=2.22045e-16;

//double fminbr(double a, double b, double (*f)(double), double tol);

LArWFParamTool::LArWFParamTool ( const std::string& type, const std::string& name,const IInterface* parent )
  : AthAlgTool(type,name,parent),
    m_NBaseline(0),
    m_ShiftToStart(false),
    m_SubtractBaseline(false),
    m_UseOmegaScanHelper(false)
{

  // Declare additional interface
  declareInterface<LArWFParamTool>(this);

  declareProperty("NBaseline",        m_NBaseline);
  declareProperty("ShiftToStart",     m_ShiftToStart);
  declareProperty("SubtractBaseline", m_SubtractBaseline);

  m_DeltaTtail.resize(3);
  int default_deltattail[3] = { 100, 50, 0 } ;
  for ( unsigned i=0; i<3; ++i )
    m_DeltaTtail[i] = default_deltattail[i] ;
  declareProperty("DeltaTtail", m_DeltaTtail);

  m_TtailMin.resize(4);
  m_TtailMax.resize(4); 
  int default_ttailmin[4] = { 0, 0, 0, 0 };
  int default_ttailmax[4] = { 0, 0, 0, 0 };
  for ( unsigned i=0; i<4; ++i ) {
    m_TtailMin[i] = default_ttailmin[i];
    m_TtailMax[i] = default_ttailmax[i];    
  }
  declareProperty("TtailMin", m_TtailMin);
  declareProperty("TtailMax", m_TtailMax);
  
  // flags to force cos resp scan:
  m_cosRespScan.resize(4) ;
  bool defaults[4] = { false , true , false , true } ;
  for ( unsigned i=0 ; i<4 ; ++i ) 
    m_cosRespScan[i] = defaults[i] ;
  declareProperty("CosRespScan", m_cosRespScan ) ;
 

  // omega0 intervals and granularities:
  //float    default_omegamin[4] = { 0.01 , 0.47 , 0.10 , 0.30 } ;
  //float    default_omegamax[4] = { 1.00 , 0.48 , 0.30 , 0.60 } ;
  //unsigned default_npoints[4]  = { 100 , 50 , 100 , 100 } ;
  float    default_omegamin[4] = { 0.01 , 0.10 , 0.10 , 0.10 };
  float    default_omegamax[4] = { 1.00 , 0.33 , 0.20 , 0.33 };
  unsigned default_npoints[4]  = {  100 ,  100 ,  100 ,  100 };
  m_omegamin.resize(4) ;
  m_omegamax.resize(4) ;
  m_npoints.resize(4) ;
  for ( unsigned i=0 ; i<4 ; ++i ) {
    m_omegamin[i] = default_omegamin[i] ;
    m_omegamax[i] = default_omegamax[i] ;
    m_npoints[i]  = default_npoints[i] ;
  }
  declareProperty("Omega0Min",  m_omegamin);
  declareProperty("Omega0Max",  m_omegamax);
  declareProperty("NpointScan", m_npoints );
  declareProperty("UseOmegaScanHelper", m_UseOmegaScanHelper );

  // flags to save residual oscillation
  m_storeResOscill.resize(4,false) ;
  declareProperty("StoreResOscill", m_storeResOscill) ;
  
  declareProperty("TShaper",    m_Tshaper=15.) ;
}

// destructor 
LArWFParamTool::~LArWFParamTool()
{
}

//const InterfaceID& LArWFParamTool::interfaceID( )
//{ return IID_ILArWFParamTool;  }


StatusCode LArWFParamTool::initialize()
{
  std::string layerName[4] = { "PS" , "Strips" , "Middle" , "Back" } ;
  
  ATH_MSG_INFO( "TShaper set to " << m_Tshaper << " ns" ) ;

  ATH_MSG_INFO( "Step response (Fstep)        -> Ttail = Tmin + " << m_DeltaTtail[0] << " steps" ) ;

  if (!m_UseOmegaScanHelper)
    ATH_MSG_INFO( "Cosine response (Omega0)     -> Ttail = Tmin + " << m_DeltaTtail[1] << " steps" ) ;
  else
    ATH_MSG_INFO( "Cosine response (Omega0)     -> Will use OmegaScanHelper() to dynamically select settings" );

  ATH_MSG_INFO( "Residual oscillations (Taur) -> Ttail = Tmin + " << m_DeltaTtail[2] << " steps" ) ;
  
  for ( unsigned layer=0 ; layer<4 ; ++layer ) {
    ATH_MSG_INFO( "options for Layer " << layer << " (" << layerName[layer] << ")" ) ;

    ATH_MSG_INFO( "      perform cos resp scan: " );
    if ( m_cosRespScan[layer] && !m_UseOmegaScanHelper ) 
      ATH_MSG_INFO( "YES (" << m_npoints[layer] << " points)" ) ;
    if ( !m_cosRespScan[layer] && !m_UseOmegaScanHelper ) 
      ATH_MSG_INFO( "NO" ) ;
    if ( m_UseOmegaScanHelper )
      ATH_MSG_INFO( "Will use OmegaScanHelper() to dynamically select settings" ) ;

    if (!m_UseOmegaScanHelper) {
      ATH_MSG_INFO( "      search interval: [ " << m_omegamin[layer] << " ; " << m_omegamax[layer] << " ]" ) ;
      if ( m_omegamin[layer] >= m_omegamax[layer] ) {
	ATH_MSG_ERROR( "Omega0Min >= Omega0Max in layer " << layer << " -- exit!" ) ;
	return StatusCode::FAILURE ;
      }
    }

    ATH_MSG_INFO( "      store residual oscillation wave:" );
    if ( m_storeResOscill[layer] ) {
      ATH_MSG_INFO( "YES" ) ;
    } else {
      ATH_MSG_INFO( "NO" ) ;
    }

  }

  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );
  
  ATH_CHECK( detStore()->retrieve(m_emId, "LArEM_ID") );

  return StatusCode::SUCCESS;
}

     
StatusCode LArWFParamTool::getLArWaveParams(const LArCaliWave& larCaliWave,
                                            const HWIdentifier chid, 
					    const CaloGain::CaloGain gain,
                                            LArWFParams& wfParams,
                                            const LArOnOffIdMapping *cabling,
					    LArCaliWave* omegaScanWave,
					    LArCaliWave* resOscill0,
					    LArCaliWave* resOscill1)  const
{


  wfParams.setTshaper(m_Tshaper);    
  LArCaliWave gCali=larCaliWave; //down-cast to LArWave + deep copy (we'll modify the wave)

  // find m_Tstart, m_Tpeak, m_Tcross, m_Tmin, m_Amplitude
  WaveTiming_t waveTiming=timeExtr(gCali);

  // find m_Tcal   
  if ( wfParams.tcal() == DoExtract ) {
    const double Tcal = expTail(gCali,waveTiming) ;
    if ( Tcal < 0 ) {
      ATH_MSG_WARNING( "Could not extract Tcal for ChID=" << chid.get_compact() 
			<< " gain=" << (int)gain ) ;
      wfParams.setTcal(FailExtract);
      return StatusCode::FAILURE;
    }
    else {
      wfParams.setTcal(Tcal);
    }
    ATH_MSG_VERBOSE( "set Tcal = " << wfParams.tcal()) ;
  }
    
  // find m_Fstep
  if ( wfParams.fstep() == DoExtract ) {
    StatusCode sc = GetFstep(gCali,wfParams,waveTiming);
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING( "Could not extract Fstep for ChID=" << chid.get_compact() 
			<< " gain=" << (int)gain );
      wfParams.setFstep(FailExtract);
      return sc ;
    }
  } else {
    ATH_MSG_VERBOSE( "set Fstep = " <<wfParams.fstep() );  
  }

  // find m_Omega0 using RTM
  if ( wfParams.omega0() == DoExtract ) {
    StatusCode sc = RTM_Omega0(gCali,chid,wfParams,waveTiming,cabling,omegaScanWave);
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING( "Could not extract Omega0 for ChID=" << chid.get_compact() 
			<< " gain=" << (int)gain ); 
      wfParams.setOmega0(FailExtract) ;
      return sc ;
    }
  } else {
    ATH_MSG_VERBOSE( "set Omega0 = " << wfParams.omega0());
  }


  const Identifier id = cabling->cnvToIdentifier(chid);
  const unsigned layer=m_emId->sampling(id);
  if ( m_storeResOscill[ layer ] && resOscill0) {
    LArWave injres0 = injRespRes(gCali,wfParams.omega0(),0);
    *resOscill0 = LArCaliWave(injres0.getWave(),gCali.getDt(),gCali.getDAC());
  }

  // find m_Taur using RTM
  if ( wfParams.taur() == DoExtract ) { 
    StatusCode sc = RTM_Taur(gCali, wfParams,waveTiming ) ;
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING( "Could not extract Taur for ChID=" << chid.get_compact() 
			<< " gain=" << (int)gain ) ;
      wfParams.setTaur(FailExtract);
      return sc ;
    }
    ATH_MSG_VERBOSE( "set Taur = " << wfParams.taur() );
  }

  wfParams.setFlag( 0 ) ;  // this should contain the method used to find parameters and the gain

  if ( m_storeResOscill[ layer ] && resOscill1) {
    LArWave injres1 = injRespRes(gCali,wfParams.omega0(),wfParams.taur());
    *resOscill1 = LArCaliWave(injres1.getWave(),gCali.getDt(),gCali.getDAC());  
  }

  return( StatusCode::SUCCESS );
}



LArWFParamTool::WaveTiming_t LArWFParamTool::timeExtr(LArWave& gCali) const
{
  WaveTiming_t wt; //return object

  wt.Tstart = m_wHelper.getStart(gCali);
  
  const unsigned NBaseline= m_NBaseline==0 ? wt.Tstart :  m_NBaseline ;

  const double baseline = m_wHelper.getBaseline(gCali,NBaseline);
  
  if ( m_ShiftToStart ) // move the wave to the start
    gCali = m_wHelper.translate(gCali, -wt.Tstart, baseline); 
  
  if ( m_SubtractBaseline ) // remove the baseline
    gCali = gCali + (-baseline);
  
  wt.Tpeak = m_wHelper.getMax(gCali);

  wt.Amplitude = gCali.getSample(wt.Tpeak);
   
  wt.Tmin  = m_wHelper.getMin(gCali);

  wt.Tcross = m_wHelper.getZeroCross(gCali);

  return wt;
}

 
double LArWFParamTool::expTail(const LArWave& gCali, const WaveTiming_t& wt) const
{
  unsigned int t1; 
  unsigned int t2; 
  if ( m_TtailMin[0]!=0 && m_TtailMax[0]!=0 && m_TtailMin[0]<m_TtailMax[0] ) {
    t1 = m_TtailMin[0];
    t2 = m_TtailMax[0];
  } else {
    const unsigned nAfterMin  = 150;
    const unsigned nBeforeEnd = 125;
    t1 = wt.Tmin + nAfterMin;
    t2 = gCali.getSize() - nBeforeEnd ;  // last points rejected because sometimes are crazy!
  }
  ATH_MSG_VERBOSE( "exp-fitting between " << t1 << " and " << t2 ) ;
  
  double A, rho  ;
  std::vector<double> fitPars = m_wHelper.expfit(gCali,t1,t2,rho) ; // fit to B*exp(A*t) function
  if (fitPars.size() == 2 ) {
    A = fitPars[1];
    //B = fitPars[0];
  } else {
    return -1;
  }

  const double Tcal = -1./A ;
  if ( Tcal < 0 ) {
    ATH_MSG_WARNING( "Exponential fit yielded negative Tcal " << Tcal );
  }
  ATH_MSG_VERBOSE( "*** Exponential fit\t--> m_Tcal    = " << Tcal ) ;

  return Tcal;
}

StatusCode LArWFParamTool::GetFstep(const LArWave& gCali, LArWFParams& wfParams, const WaveTiming_t& wt) const
{
  // step response
  const unsigned Ttail = wt.Tmin + m_DeltaTtail[0]; 
  //m_Fstep = 0.; // starting point for iterations
  const double fstep1=0.0;
  const double Tcal=wfParams.tcal();
  wfParams.setFstep(fstepMin(gCali,fstep1,Tcal,Ttail) );
  //ATH_MSG_VERBOSE( "*** Step response\t--> m_Fstep   = " << m_Fstep ) ;
  return StatusCode::SUCCESS ;
}

double LArWFParamTool::fstepMin (const LArWave& gCali, double fstep, const double Tc, const unsigned Ttail) const {
  int niter = 0;
  //double dfstep;
  while (1) {
    niter++ ;
    const double dfstep = dFstep(gCali,fstep,Tc,Ttail) ;
    fstep += dfstep ;
    if ( niter > 50 || fstep < 0. ) {
      fstep = 0.;
      break;
    }
    if ( fabs( dfstep ) <= m_TINY ) break;
  }
  return fstep;
}

double LArWFParamTool::dFstep (const LArWave& gCali, const double fstep, const double Tc, const unsigned Ttail) const {
  LArWave stepResp1        = stepResp(gCali, fstep, Tc) ; 
  LArWave dstepRespDfstep1 = dstepRespDfstep(gCali,fstep,Tc)  ; 
  //LArWaveHelper wHelper;
  double a,b;
  if ( m_TtailMin[1]!=0 && m_TtailMax[1]!=0 && m_TtailMin[1]<m_TtailMax[1] ) {
    b = m_wHelper.getSumRegion( stepResp1 * dstepRespDfstep1,m_TtailMin[1],m_TtailMax[1]);
    a = m_wHelper.getSumSquareRegion( dstepRespDfstep1,m_TtailMin[1],m_TtailMax[1]);
  } else {
    b = m_wHelper.getSumTail( stepResp1 * dstepRespDfstep1,Ttail);
    a = m_wHelper.getSumSquareTail( dstepRespDfstep1,Ttail);
  }
  return -b/a ;
}

LArWave LArWFParamTool::stepResp (const LArWave& gCali, const double fstep, const double Tc) const {
  //return m_gCali + m_gCali % stepCorr() ;
  LArWave result = gCali % stepCorr(gCali, fstep, Tc);
  result+=gCali;
  return result;
}

LArWave LArWFParamTool::stepCorr(const LArWave& gCali, const double& fstep, const double& Tc) const {
  const unsigned N = gCali.getSize() ;
  const double dt = gCali.getDt() ;

  LArWave w(N,dt);
  const double A=(1.-fstep)/Tc;
  for ( unsigned i=0 ; i<N ; i++ ) {
    w.setSample(i,A*exp(-fstep*i*dt/Tc));
  }
  return w ;
}

LArWave LArWFParamTool::dstepRespDfstep (const LArWave& gCali, const double& fstep, const double& Tc) const {
  return gCali % dstepCorrDfstep(gCali,fstep,Tc);
}

double LArWFParamTool::dstepCorrDfstep (const double t, const double& fstep, const double& Tc ) const {
  return (-1./Tc)*(1.+((1.-fstep)/Tc)*t)*exp(-fstep*t/Tc);
}

LArWave LArWFParamTool::dstepCorrDfstep(const LArWave& gCali, const double& fstep, const double& Tcal ) const {
  const unsigned N = gCali.getSize() ;
  const double dt = gCali.getDt() ;
  LArWave w(N,dt);
  for ( unsigned i=0 ; i<N ; i++ ) w.setSample(i,dstepCorrDfstep(i*dt,fstep,Tcal)) ;
  return w ;
}

StatusCode LArWFParamTool::RTM_Omega0(const LArWave& gCali, const HWIdentifier chid, LArWFParams& wfParams, const WaveTiming_t& wt, const LArOnOffIdMapping *cabling, LArCaliWave* omegaScanWave) const
{
  /*  begin of the COSINE RESPONSE analysis */
   
  const Identifier id = cabling->cnvToIdentifier(chid);
  // Define the (raw) minimum research interval (if the Layer card is not set,
  // uses the wider interval, and default value for the first minimum scan)

  omegaScanParams_t OSParams;  

  // defaults (e.g. to be used in case cosRespScan=true)
  OSParams.cosRespScan  = true ;
  OSParams.omegamin   = 0.01 ;
  OSParams.omegamax   = 1.   ;
  OSParams.Npoint        = 100  ;
  OSParams.DeltaOmega = 0.01 ; // GHz
  OSParams.range.min=m_TtailMin[2];
  OSParams.range.max=m_TtailMax[2];

  if ( !m_UseOmegaScanHelper) {
    const unsigned layer=m_emId->sampling(id);    
    OSParams.cosRespScan = m_cosRespScan[ layer ] ;
    OSParams.omegamin    = m_omegamin[ layer ] ;
    OSParams.omegamax    = m_omegamax[ layer ] ;
    OSParams.Npoint      = m_npoints[ layer ] ;
    OSParams.DeltaOmega  = 0.02 ;
    //DeltaOmega  = 0.001 ;
  } else {
    OSParams=OmegaScanHelper(id,wt);
  }

  waveRange_t range=OSParams.range;
//if (m_TtailMin[2]==0 || m_TtailMax[2]==0 || m_TtailMin[2]>=m_TtailMax[2]) {
  if (range.min==0 || range.max==0 || range.min>=range.max) {
    range.min=wt.Tmin + m_DeltaTtail[1];
    range.max=gCali.getSize();
  }

  if ( OSParams.cosRespScan ){   
    // makes a raw scan of the cosresp chi2 logarithm
    const double dOmega = (OSParams.omegamax-OSParams.omegamin)/OSParams.Npoint;
    const unsigned vectSize = (unsigned) ceil ( OSParams.omegamax/dOmega );
    const unsigned indexmin = (unsigned) floor( OSParams.omegamin/dOmega );
    if (omegaScanWave) omegaScanWave->setSize(vectSize);
    double chi2min = LAR_MAX_DOUBLE, omega0raw = LAR_MAX_DOUBLE;  
    for (int i=0;i<OSParams.Npoint;i++) {
      const double omega = OSParams.omegamin+dOmega*i ;
      const double chi2 = logChi2CosRespShaper(omega, gCali, wfParams, range);
      if (omegaScanWave) omegaScanWave->setSample(i+indexmin,chi2);
      if ( chi2 < chi2min ) { 
	chi2min   = chi2;
	omega0raw = omega;
      }
    }

    OSParams.omegamin = omega0raw - OSParams.DeltaOmega ;
    OSParams.omegamax = omega0raw + OSParams.DeltaOmega ;
  }

  if ( OSParams.omegamin == OSParams.omegamax ) { // skip cosine response calculation
    wfParams.setOmega0(0.); // this will force no injection point correction in LArPhysWavePredictor
    ATH_MSG_VERBOSE( "*** Cosine response skipped\t--> m_Omega0  = " << wfParams.omega0() ) ;  
  } else { // numerical (Brent) log(chi2) minimization
    wfParams.setOmega0(omega0MinNum ( OSParams.omegamin, OSParams.omegamax, m_EPSILON, gCali, wfParams, range)  );
    ATH_MSG_VERBOSE( "*** Cosine response\t--> m_Omega0  = " <<wfParams.omega0() ) ;
  }

  /* end of the COSINE RESPONSE analysis */
  return StatusCode::SUCCESS ;
}


LArWFParamTool::omegaScanParams_t LArWFParamTool::OmegaScanHelper(const Identifier id, const  WaveTiming_t& wt) const {

  omegaScanParams_t ret;

  // this is a skeleton of the final function: use at your own risk! M.D. & C.G.
  // EMEC part modified by L. March - 7 April 2011

  const unsigned layer=m_emId->sampling(id);
  
  ret.range.min   = m_TtailMin[2];
  ret.range.max   = m_TtailMax[2];
  ret.cosRespScan = m_cosRespScan[ (unsigned)layer ] ;
  ret.omegamin    = m_omegamin[ (unsigned)layer ] ;
  ret.omegamax    = m_omegamax[ (unsigned)layer ] ;
  ret.Npoint      = m_npoints[ (unsigned)layer ] ;
  ret.DeltaOmega  = 0.02 ;
  //DeltaOmega  = 0.001 ;

  //if ( m_BarrelEC==0 ) { // Barrel
  if (m_emId->is_em_barrel(id)) {

    if ( layer==0 ) { // PS
      ret.range.min   = wt.Tstart;
      ret.range.max   = wt.Tcross;
      ret.cosRespScan = m_cosRespScan[ (unsigned)layer ];
      ret.omegamin    = 0.240;
      ret.omegamax    = 0.330;
      ret.Npoint      = 900;
    }

    if ( layer==1 ) { // Strips
      ret.range.min = wt.Tstart;
      ret.range.max = wt.Tcross;
      ret.cosRespScan = true;
      // Strips interval 1
      //ret.omegamin    = 0.150;
      //ret.omegamax    = 0.220;
      //ret.Npoint      = 700;
      // Strips interval 2
      ret.omegamin    = 0.220;
      ret.omegamax    = 0.310;
      ret.Npoint      = 900;
      // Strips interval 3
      //omegamin    = 0.310;
      //omegamax    = 0.390;
      //Npoint      = 1400;
    }

    if ( layer==2 ) { // Middle
      ret.range.min = wt.Tstart;
      ret.range.max = wt.Tmin + m_DeltaTtail[1];
      ret.cosRespScan = false;
      ret.omegamin    = 0.060;
      ret.omegamax    = 0.215;
      //Npoint      = 315;
    }

    if ( layer==3 ) { // Back     
      ret.range.min   = wt.Tstart;
      ret.range.max   = wt.Tcross;
      ret.cosRespScan = false;
      ret.omegamin    = 0.100;
      ret.omegamax    = 0.700;  
      ret.Npoint      = 120;
    }

  } 
  else {
    //if ( m_BarrelEC==1 ) { // EndCap

    // use pulse peak to perform cosine response tranformation minimization
    //ret.range.min = m_Tstart;
    //ret.range.max = m_Tcross;
    ret.range.min = 0.;
    ret.range.max = 0.;

    ret.DeltaOmega  = 0.001 ;

    //(*m_log) << MSG::WARNING << " m_Tstart= " << m_Tstart << " m_Tcross=" << m_Tcross ) ;

    const bool IsEMECInnerWheel = m_emId->is_em_endcap_inner(id);//m_onlineHelper->isEMECinHECchannel(m_chID);
    const int eta=m_emId->eta(id); 
    if ( IsEMECInnerWheel ) { // EMEC Inner Wheel
      
      if ( layer==1 ) {
	//omegamin    = 0.10 ;
	//omegamax    = 0.20 ;
	ret.omegamin    = 0.12 ;
	if(eta>=0 && eta<=3) 
	  {
	    ret.omegamin    = 0.12 ;
	    //omegamax = 0.165 ;
	    ret.omegamax = 0.16 ;
	    ret.Npoint=100;
	  }
	if(eta>=4 && eta<=6) 
	  {
	    ret.omegamin    = 0.14 ;
	    ret.omegamax = 0.18 ;
	    ret.Npoint=100;
	  }
      } else
      if ( layer == 2 ) { // don't extract Omega0   
	/*
	cosRespScan = false;
	omegamin    = 0.;
	omegamax    = 0.;
	Npoint      = 0;
	DeltaOmega  = 0.; 
	*/
	ret.cosRespScan = true;
	ret.omegamin    = 0.12;
	ret.omegamax    = 0.35;
	ret.Npoint      = 100;
      } else { // don't extract Omega0   
	ret.cosRespScan = false;
	ret.omegamin    = 0.;
	ret.omegamax    = 0.;
	ret.Npoint      = 0;
	ret.DeltaOmega  = 0.; 
      }

    } else { // EMEC Outer Whell

      if ( layer==1 ) { // don't extract Omega0
	ret.cosRespScan = false;
	ret.omegamin    = 0.;
	ret.omegamax    = 0.;
	ret.Npoint      = 0;
	ret.DeltaOmega  = 0.;
      } else
      if ( layer == 2 ) { 
        ret.cosRespScan = true;
	ret.Npoint  =100;
	ret.omegamin=0.1;
	if(eta==0) {
	  ret.omegamax    = 0.140 ;
	} 
	if(eta==1) {
	  ret.omegamax    = 0.135 ;
	}
	if(eta==2) {
	  ret.omegamax    = 0.15 ;
	} 
	if(eta==3) {
	  ret.omegamin    = 0.10;
          ret.omegamax    = 0.16;
        }
	if(eta>=4 &&  eta<11 ) {
	  ret.omegamin    = 0.12 ;
	  ret.omegamax    = 0.20 ;
	} 
	if(eta==11) {
	  ret.omegamin    = 0.12 ;
	  ret.omegamax    = 0.22 ;
	} 
	if(eta>=12 && eta<28) {
	  ret.omegamin    = 0.12 ;
	  ret.omegamax    = 0.24 ;
	} 
	if(eta>=28 && eta<39) {
	  ret.omegamin    = 0.12 ;
	  ret.omegamax    = 0.245 ;
	} 
	if(eta>=39 && eta<43) {
	  //ret.omegamin    = 0.12 ;
          ret.omegamin    = 0.24 ; 
	  ret.omegamax    = 0.32 ;
	}
      } else { // don't extract Omega0
	ret.cosRespScan = false;
	ret.omegamin    = 0.;
	ret.omegamax    = 0.;
	ret.Npoint      = 0;
	ret.DeltaOmega  = 0.;
      }

    } // end of EMEC Outer Wheel

  } // end of EMEC

  return ret;
}


StatusCode LArWFParamTool::RTM_Taur(const LArWave& gCali, LArWFParams& wf, const WaveTiming_t& wt) const
{
  /* begin of the INJECTION POINT CORRECTION RESIDUAL OSCILLATION analysis */
    
  // m_Taur minimum search interval... 
  double taurmin = 0.; // ns
  //double taurmax = 4.; // ns
  double taurmax = 100.; // ns

  if ( wf.omega0()==0) {
    wf.setTaur(0);
    ATH_MSG_VERBOSE( "*** Injection point skipped\t--> m_Taur    = " << wf.taur() ) ;
  } else {
    // numerical (Brent) log(chi2) minimization
    waveRange_t range;
    if ( m_TtailMin[3]!=0 && m_TtailMax[3]!=0 && m_TtailMin[3]<m_TtailMax[3]) {
      range.min=m_TtailMin[3];
      range.max=m_TtailMax[3];
    }
    else {
      range.min= wt.Tmin + m_DeltaTtail[2];
      range.max=gCali.getSize();
    }

    //std::cout << "WL2: min= " << range.min << ", max= " <<  range.max << std::endl;      
    wf.setTaur(taurMinNum ( taurmin, taurmax, m_EPSILON, gCali, wf, range));
    ATH_MSG_VERBOSE( "*** Injection point\t--> m_Taur    = " << wf.taur()) ;
  }

  /* end of the INJECTION POINT CORRECTION RESIDUAL OSCILLATION analysis */

  return StatusCode::SUCCESS ;
}

double LArWFParamTool::logChi2CosRespShaper (const double omega, const LArWave& gCali, const LArWFParams& wfParams, const waveRange_t& range) const {
  return log(m_wHelper.getSumSquareRegion(cosRespShaper(gCali,wfParams.fstep(),wfParams.tcal(),omega,wfParams.tshaper()),range.min,range.max)) ; 
}

LArWave LArWFParamTool::cosRespShaper (const LArWave& gCali, const double& fstep, const double& tcal, const double& w, const double& tsh ) const {
  return cosResp(gCali,fstep,tcal, w) * ( pow((1.+ w*w*tsh*tsh),1.5)/(w*tsh));
}

/******************************************************************
 * Cosine response
 *****************************************************************/
LArWave LArWFParamTool::cosResp (const LArWave& gCali, const double& fstep, const double& tcal, const double& omega) const {
  
  LArWave result=gCali % cosCorr(gCali.getSize(),gCali.getDt(),fstep, tcal, omega);
  result+=gCali;
  return result;
}

/* =================================================================
 * Function: Cosine response correction function
 * ============================================================== */
LArWave LArWFParamTool::cosCorr(const unsigned N, const double dt, const double fstep, const double Tc, const double omega) const {
  LArWave w(N,dt);
  const double C1=(fstep*fstep-fstep*fstep*fstep)/Tc;
  const double C2=(fstep+omega*omega*Tc*Tc);
  const double C3=(fstep-1.)*omega*Tc;
  const double C4=( fstep*fstep+omega*omega*Tc*Tc );
  for ( unsigned i=0 ; i<N ; i++ ) {
    const double t=i*dt;
    //const double p=((fstep*fstep-fstep*fstep*fstep)/Tc*exp(-fstep*t/Tc) - omega*((fstep+omega*omega*Tc*Tc)*sin(omega*t)+(fstep-1.)*omega*Tc*cos(omega*t)) ) / ( fstep*fstep+omega*omega*Tc*Tc ) ;
    const double p=(C1*exp(-fstep*t/Tc) - omega*(C2*sin(omega*t) + C3*cos(omega*t))) / C4 ;
    w.setSample(i,p);
  }
  return w ;
}


/* ================================================================
 * log of the chi2 built on the tail of the injection point correction
 * residual oscillation, to be used in numerical minimisation.
 * ============================================================== */
double LArWFParamTool::logChi2InjRespRes (const double taur, const LArWave& gCali, const LArWFParams& wf, const waveRange_t& range) const {
  //if (m_chID==957367040) std::cout << "WL3: " << std::setprecision(8) << taur << std::endl;
  return m_wHelper.getSumSquareRegion(injRespRes(gCali,wf.omega0(),taur), range.min, range.max);
}

LArWave LArWFParamTool::injRespRes (const LArWave& w, const double omega0, const double taur) const {
  return w - ( w % injCorr(w.getSize(),w.getDt(),omega0,taur) );
}

LArWave LArWFParamTool::injCorr(const unsigned N, const double dt,const double omega0, const double taur) const {

  LArWave w(N,dt);

  const double tau0 = 1./omega0;
  const double Delta = taur*taur - 4*tau0*tau0;
  if ( Delta > 0 ) {
    const double sqrtDelta = sqrt(Delta) ;
    const double taup = 0.5*( taur + sqrtDelta ) ;
    const double taum = 0.5*( taur - sqrtDelta ) ;
    for ( unsigned i=0 ; i<N ; i++ ) {
      const double t=i*dt;
      w.setSample(i, (exp(-t/taup) - exp(-t/taum) ) / (taup - taum));
    }
    //return ( exp(-t/taup) - exp(-t/taum) ) / ( taup - taum ) ;
  } else if ( Delta < 0 ) {
    const double T = sqrt(-Delta) ;
    const double A = 2 * taur / ( (taur*taur) - Delta ) ;
    const double B = 2 * T / ( (taur*taur) - Delta ); 
    for ( unsigned i=0 ; i<N ; i++ ) {
      const double t=i*dt;
      w.setSample(i, 2 * exp(-A*t) * sin(B*t) / T);
    }
    //return 2 * exp(-A*t) * sin(B*t) / T ;
  } else { //Delta==0
    const double tau = 0.5 * taur ;
    //    return exp(-t/tau) * t / pow(tau,2) ;
    for ( unsigned i=0 ; i<N ; i++ ) {
      const double t=i*dt;
      w.setSample(i,exp(-t/tau) * t / (tau*tau)) ;
    }
    //return exp(-t/tau) * t / (tau*tau) ;
  } //end else
  return w;
}

double LArWFParamTool::omega0MinNum (double omegaMin, double omegaMax, const double tol, const LArWave& gCali, const LArWFParams& wf, const waveRange_t& range) const {
  return fminbr ( omegaMin, omegaMax, tol, &LArWFParamTool::logChi2CosRespShaper, gCali, wf, range);
}






double LArWFParamTool::taurMinNum (const double taurmin, const double taurmax, const double tol, const LArWave& gCali, const LArWFParams& wf, const waveRange_t& range ) const {
  return fminbr ( taurmin, taurmax, tol, &LArWFParamTool::logChi2InjRespRes, gCali, wf, range );
}

/*
 ************************************************************************
 *	    		    C math library
 * function FMINBR - one-dimensional search for a function minimum
 *			  over the given range
 *
 * Input
 *	double fminbr(a,b,f,tol)
 *	double a; 			Minimum will be seeked for over
 *	double b;  			a range [a,b], a being < b.
 *	double (*f)(double x);		Name of the function whose minimum
 *					will be seeked for
 *	double tol;			Acceptable tolerance for the minimum
 *					location. It have to be positive
 *					(e.g. may be specified as m_EPSILON)
 *
 * Output
 *	Fminbr returns an estimate for the minimum location with accuracy
 *	3*m_SQRT_EPSILON*abs(x) + tol.
 *	The function always obtains a local minimum which coincides with
 *	the global one only if a function under investigation being
 *	unimodular.
 *	If a function being examined possesses no local minimum within
 *	the given range, Fminbr returns 'a' (if f(a) < f(b)), otherwise
 *	it returns the right range boundary value b.
 *
 * Algorithm
 *	G.Forsythe, M.Malcolm, C.Moler, Computer methods for mathematical
 *	computations. M., Mir, 1980, p.202 of the Russian edition
 *
 *	The function makes use of the "gold section" procedure combined with
 *	the parabolic interpolation.
 *	At every step program operates three abscissae - x,v, and w.
 *	x - the last and the best approximation to the minimum location,
 *	    i.e. f(x) <= f(a) or/and f(x) <= f(b)
 *	    (if the function f has a local minimum in (a,b), then the both
 *	    conditions are fulfiled after one or two steps).
 *	v,w are previous approximations to the minimum location. They may
 *	coincide with a, b, or x (although the algorithm tries to make all
 *	u, v, and w distinct). Points x, v, and w are used to construct
 *	interpolating parabola whose minimum will be treated as a new
 *	approximation to the minimum location if the former falls within
 *	[a,b] and reduces the range enveloping minimum more efficient than
 *	the gold section procedure. 
 *	When f(x) has a second derivative positive at the minimum location
 *	(not coinciding with a or b) the procedure converges superlinearly
 *	at a rate order about 1.324
 *
 ************************************************************************
 */

//#include "fminbr.h"
//#include "small_nums.h"

double LArWFParamTool::fminbr(double a, double b, const double tol, double (LArWFParamTool::*f)(double, const LArWave&, const LArWFParams&, const waveRange_t&) const, 
			      const LArWave& gCali, const LArWFParams& wf, const waveRange_t& waverange) const 
/* fminbr(a,b,f,tol)	An estimate to the min location
   a;			Left border  | of the range
   b;			Right border | the min is seeked
   (*f)(double x)	Function under investigation
   tol;			Acceptable tolerance		*/

{
  double x,v,w;				/* Abscissae, descr. see above	*/
  double fx;				/* f(x)				*/
  double fv;				/* f(v)				*/
  double fw;				/* f(w)				*/
  const double r = (3.-sqrt(5.0))/2;	/* Gold section ratio		*/

  assert( tol > 0 && b > a );

  v = a + r*(b-a);  fv = (this->*f)(v,gCali,wf,waverange);       /* First step - always gold section*/
  x = v;  w = v;
  fx=fv;  fw=fv;

  for(;;)		/* Main iteration loop	*/
  {
    double range = b-a;			/* Range over which the minimum */
					/* is seeked for		*/
    double middle_range = (a+b)/2;
    double tol_act =			/* Actual tolerance		*/
		m_SQRT_EPSILON*fabs(x) + tol/3;
    double new_step;      		/* Step at this iteration       */

    if( fabs(x-middle_range) + range/2 <= 2*tol_act )
      return x;				/* Acceptable approx. is found	*/

					/* Obtain the gold section step	*/
    new_step = r * ( x<middle_range ? b-x : a-x );


    			/* Decide if the interpolation can be tried	*/
    if( fabs(x-w) >= tol_act  )		/* If x and w are distinct      */
    {					/* interpolatiom may be tried	*/
        double p;    		        /* Interpolation step is calcula-*/
	double q;                       /* ted as p/q; division operation*/
                                        /* is delayed until last moment	*/
	double t;

	t = (x-w) * (fx-fv);
	q = (x-v) * (fx-fw);
	p = (x-v)*q - (x-w)*t;
	q = 2*(q-t);

	if( q>(double)0 )		/* q was calculated with the op-*/
	  p = -p;			/* posite sign; make q positive	*/
	else				/* and assign possible minus to	*/
	  q = -q;			/* p				*/

	if( fabs(p) < fabs(new_step*q) &&	/* If x+p/q falls in [a,b]*/
	    p > q*(a-x+2*tol_act) &&		/* not too close to a and */
	    p < q*(b-x-2*tol_act)  )            /* b, and isn't too large */
	  new_step = p/q;			/* it is accepted         */
					/* If p/q is too large then the	*/
					/* gold section procedure can 	*/
					/* reduce [a,b] range to more	*/
					/* extent			*/
    }

    if( fabs(new_step) < tol_act ) {	/* Adjust the step to be not less*/
      if( new_step > (double)0 )	/* than tolerance		*/
	new_step = tol_act;
      else
	new_step = -tol_act;
    }

				/* Obtain the next approximation to min	*/
    {				/* and reduce the enveloping range	*/
      double t = x + new_step;	/* Tentative point for the min	*/
      double ft = (this->*f)(t,gCali,wf,waverange);
      if( ft <= fx )
      {                                 /* t is a better approximation	*/
	if( t < x )			/* Reduce the range so that	*/
	  b = x;                        /* t would fall within it	*/
	else
	  a = x;
      
	v = w;  w = x;  x = t;		/* Assign the best approx to x	*/
	fv=fw;  fw=fx;  fx=ft;
      }
      else                              /* x remains the better approx  */
      {        		             
	if( t < x )			/* Reduce the range enclosing x	*/
	  a = t;                   
	else
	  b = t;
      
        if( ft <= fw || w==x )
        {
           v = w;  w = t;
	   fv=fw;  fw=ft;
        }
        else if( ft<=fv || v==x || v==w )
        {
           v = t;
	   fv=ft;
        }
      }
      
    }			/* ----- end-of-block ----- */
  }		/* ===== End of loop ===== */   
}




unsigned LArWFParamTool::checkStatus(const LArCaliWave &larCaliWave) const  
{
  // this is supposed to be a quick test on cali waves, to flags those 
  // clearly problematic. but cuts must be tuned, so for the time being 
  // don't call this method!

  //MsgStream log(msgSvc(), name());

  unsigned length ;
  if ( (length=larCaliWave.getSize()) < 750 ) {
   ATH_MSG_WARNING( "Calibration wave is too short!" ) ;
    return TooShort ;
  }

  const unsigned range = 2 ;
  unsigned nMax = 0 ;
  double tremble = 0 ;
  double maxSample = -9.E+10 ;
  for ( unsigned i=0 ; i<length ; i++ ) {
    if ( larCaliWave.getError(i) < 0 || larCaliWave.getTrigger(i) < 0 ) {
      ATH_MSG_WARNING( "Calibration wave probably not completely filled" ) ;
      return NotFilled ;
    }
    if ( larCaliWave.getTrigger(i) < 50 ) {
      ATH_MSG_WARNING( "Calibration wave with low statistics" ) ;
      return LowStat ;
    }
    double thisSample = larCaliWave.getTrigger(i) ;
    if ( thisSample > maxSample ) maxSample = thisSample ;
    if ( i>=range && i<length-range ) {
      bool max = true ;
      double loc_tremble = 0 ;
      for ( unsigned k=i-range ; k<=i+range ; k++ ) {
	double delta = thisSample - larCaliWave.getSample(k) ;
	loc_tremble += delta*delta;//  pow(delta,2) ;
	if ( delta < 0 ) max = false ;
      }
      if ( max ) nMax ++ ;
      tremble += loc_tremble / (2*range) ; 
    }
  }
  if ( sqrt(tremble/length)/maxSample > 0.1 ) {
    ATH_MSG_WARNING( "Calibration wave is noisy " << sqrt(tremble/length)/maxSample ) ;
    return Noisy ;
  }
  if ( nMax > 1 ) {
    ATH_MSG_WARNING( "Calibration wave oscillates" ) ;
    return Oscillating ;
  }

  return OK ;
}
