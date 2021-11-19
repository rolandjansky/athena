/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArPhysWaveTool.h" 

LArPhysWaveTool::LArPhysWaveTool ( const std::string& type, 
				   const std::string& name, 
				   const IInterface* parent )
  : AthAlgTool(type,name,parent)
{
  // Declare additional interface
  declareInterface<LArPhysWaveTool>(this);

  // Prepare defaults property values
  bool injPcorr[4] = { false , true , true , false } ;
  m_injPointCorrLayer.clear();
  for ( unsigned i=0;i<4;i++ ) m_injPointCorrLayer.push_back(injPcorr[i]) ;

  bool injPtaur[4] = { false , false , true , false } ;
  m_injPointUseTauR.clear();
  for ( unsigned i=0;i<4;i++ ) m_injPointUseTauR.push_back(injPtaur[i]) ;

  // Declare properties 
  declareProperty("InjPointCorr",     m_injPointCorr=false);
  declareProperty("InjPointCorrLayer",m_injPointCorrLayer);
  declareProperty("InjPointUseTauR",  m_injPointUseTauR);
  declareProperty("NormalizeCali",    m_normalizeCali=true);
  declareProperty("TimeOriginShift",  m_timeOriginShift=false);
  declareProperty("SubtractBaseline", m_subtractBaseline=true);
}

LArPhysWaveTool::~LArPhysWaveTool() {}



StatusCode LArPhysWaveTool::makeLArPhysWave(const LArWFParams& larWFParam, 
					    const LArCaliWave& larCaliWave,
					    int /*region*/, int layer,
					    LArPhysWave& predLArPhysWave,
					    float& MphysMcali) const {

  
  // calib. signal at Mother Board :
  LArWave gCaliMB(larCaliWave);
  LArWave gPhys;
  LArWaveHelper wHelper;

  // shift gCaliMB to start point and remove baseline

  unsigned tstart = wHelper.getStart(gCaliMB) ;
  double baseline = wHelper.getBaseline(gCaliMB,tstart) ;
  if ( m_subtractBaseline )   gCaliMB = gCaliMB + (-baseline) ;
  if ( m_timeOriginShift )    gCaliMB = wHelper.translate(gCaliMB,-tstart,baseline) ;
  
  // normalization of calibration pulse

  if ( m_normalizeCali ) {
    double peak = gCaliMB.getSample( wHelper.getMax(gCaliMB) ) ;
    ATH_MSG_VERBOSE ( "*** Normalisation \t|-> YES (CaliWave peak = " << peak << ")" );
    if ( peak <=0 ) {
      ATH_MSG_WARNING ( "Peak value <=0 , cannot normalize!" );
    } else {
      gCaliMB = gCaliMB * (1./peak)  ;
    }
  } else {
    ATH_MSG_VERBOSE ( "*** Normalisation \t|-> NO" );
  }

  // ionisation waveform prediction

  ATH_MSG_VERBOSE ( "*** Physics waveform\t|-> m_Tdrift  = " << larWFParam.tdrift() << " ns " );
  ATH_MSG_VERBOSE ( "*** Physics waveform\t|-> m_Fstep   = " << larWFParam.fstep()  << " ns " );
  ATH_MSG_VERBOSE ( "*** Physics waveform\t|-> m_Tcal    = " << larWFParam.tcal()   << " ns " );
  ATH_MSG_VERBOSE ( "*** Physics waveform\t|-> m_Omega0  = " << larWFParam.omega0() << " GHz" );
  ATH_MSG_VERBOSE ( "*** Physics waveform\t|-> m_Taur    = " << larWFParam.taur()   << " ns " );
  
  bool doInjPointCorr = ( ( ( layer>=0 && layer<4 && m_injPointCorrLayer[layer] ) || m_injPointCorr )
			  && larWFParam.omega0() != 0. ) ;

  const unsigned N = gCaliMB.getSize();   
  const double dt = gCaliMB.getDt() ;


  if ( ! doInjPointCorr ) {
    // perform only exp->triangle correction
    ATH_MSG_VERBOSE ( "*** Inj. Point Corr \t|-> NO" );
    gPhys = exp2Tri ( gCaliMB,N,dt, larWFParam) ;
  } else {
    // perform exp->triangle and then injection point correction
    ATH_MSG_VERBOSE ( "*** Inj. Point Corr \t|-> YES" );
    if ( !m_injPointUseTauR[layer] ) {
      //Copy LArWFParams and set Taur to 0
      LArWFParams paramsNoTaur=larWFParam;
      paramsNoTaur.setTaur(0);
      ATH_MSG_VERBOSE ( "*** Inj. Point TauR \t|-> NO" );
      gPhys = injResp ( exp2Tri ( gCaliMB,N,dt,paramsNoTaur),N,dt,paramsNoTaur);
    }
    else {
      gPhys = injResp ( exp2Tri ( gCaliMB,N,dt,larWFParam),N,dt,larWFParam);
    }
  }
  
  // compute Mphys/Mcal
  if ( m_normalizeCali ) {
     // caliwave is normalized to 1 => Mcali = 1
     MphysMcali = gPhys.getSample( wHelper.getMax(gPhys) ) ;
  } else {
     MphysMcali = gPhys.getSample( wHelper.getMax(gPhys) ) /
                  gCaliMB.getSample( wHelper.getMax(gCaliMB) ) ;
  }  
  ATH_MSG_VERBOSE ( "*** Physics waveform\t|-> m_MphysMcali = " << MphysMcali );
  
  predLArPhysWave = LArPhysWave( gPhys.getWave(), larCaliWave.getDt() );
			 
  return StatusCode::SUCCESS;			 
}

LArWave LArPhysWaveTool::exp2Tri (const LArWave &w,const unsigned N, const double dt, const LArWFParams& params) const {
  return w + ( w % caliPhysCorr(N,dt,params) ) ;
}


LArWave LArPhysWaveTool::caliPhysCorr(const unsigned N, const double dt, const LArWFParams& params) const {
  LArWave w(N,dt);
  for ( unsigned i=0 ; i<N ; i++ ) 
    w.setSample(i,caliPhysCorr(i*dt,params)) ;
  return w ;
}

/* =====================================================================
 * Function: Calibration to Ionisation correction function
 * ===================================================================== */
double LArPhysWaveTool::caliPhysCorr ( double t, const LArWFParams& params) const {
  const double fstep = params.fstep() ;
  const double Tc    = params.tcal() ;
  const double Td    = params.tdrift() ;
  if ( t<Td ) {
    if ( fstep==0. ) return ((1.-Tc/Td)-t/Td)/Tc ;
    return (1.-fstep)/Tc * exp (-fstep*t/Tc)
      +1./(fstep*Td) * ((1.-fstep)*exp(-fstep*t/Tc)-1.);
  }
  else {
    if ( fstep==0. ) return ((1.-Tc/Td)-t/Td)/Tc
                       + ((t-Td)/Td+Tc/Td)/Tc ;
    return (1.-fstep)/Tc * exp (-fstep*t/Tc)
      +1./(fstep*Td) * ( (1.-fstep)*exp(-fstep*t/Tc) - 1. )
      -1./(fstep*Td) * ( (1.-fstep)*exp(-fstep*(t-Td)/Tc) - 1. ) ;
  }
}
                                                                                
// /******************************************************************
//  * Ionisation waveform prediction
//  * mode = 1 --> direct exponential to triangular, using wave W
//  * mode = 2 --> exponential to stepresp to triangular, using gCali
//  ******************************************************************/
// LArPhysWave LArPhysWaveTool::physPred ( int mode , LArPhysWave W ) const {
//   if ( mode<1 || mode>2 ) mode=1;
//   if ( mode==1 ) return injResp ( exp2Tri ( W ) );
//   else return injResp ( step2Tri ( stepResp() ) ) ;
// }
// /*******************************************************************
//  * Ionisation waveform prediction (default)
//  ******************************************************************/
// LArPhysWave LArPhysWaveTool::physPred ( LArPhysWave W ) const {
//   return physPred ( 1 , W ) ;
// }
/*******************************************************************
 * Injection point correction
 *******************************************************************/
LArWave LArPhysWaveTool::injResp (const LArWave& w, unsigned N, double dt, const LArWFParams& params) const {
  return  w % injCorr(N,dt,params);
}
                                                                                
LArWave LArPhysWaveTool::stepResp (const LArCaliWave& gCali, const LArWFParams& params) const {
  const unsigned N=gCali.getSize();
  const double dt=gCali.getDt();

  return gCali + gCali % stepCorr(N,dt,params) ;
}
LArWave LArPhysWaveTool::step2Tri (const LArWave& w, unsigned N, double dt, const LArWFParams& params) const {
  return  w + w % stepPhysCorr(N,dt,params.tdrift()) ;
}
/* =================================================================
 * Function: Step response to Ionisation correction function
 * =============================================================== */
double LArPhysWaveTool::stepPhysCorr ( double t, const double Td) const {
  if ( t<0. || t>=Td ) return 0. ;
  else return -1./Td ;
}
LArWave LArPhysWaveTool::stepPhysCorr(unsigned N, double dt, const double Td) const {
  LArWave w(N,dt) ;
  for ( unsigned i=0 ; i<N ; i++ ) w.setSample(i,stepPhysCorr(i*dt,Td)) ;
  return w ;
}


LArWave LArPhysWaveTool::stepCorr(unsigned N, double dt, const LArWFParams& params) const {
  LArWave w(N,dt) ;
  for ( unsigned i=0 ; i<N ; i++ ) w.setSample(i,stepCorr(i*dt,params)) ;
  return w ;
}
double LArPhysWaveTool::stepCorr (double t, const LArWFParams& params) const {
  const double fstep = params.fstep();
  const double Tc    = params.tcal();
  return (1.-fstep)/Tc * exp( -fstep*t/Tc );
}


LArWave LArPhysWaveTool::injCorr(unsigned N, double dt, const LArWFParams& params) const {
  LArWave w(N,dt) ;
  for ( unsigned i=0 ; i<N ; i++ ) w.setSample(i,injCorr(i*dt,params)) ;
  return w ;
}
/* =================================================================
 * Function: injection point correction function
 * =============================================================== */
double LArPhysWaveTool::injCorr ( double t, const LArWFParams& params) const {
  const double tau0 = 1./params.omega0();
  const double taur = params.taur();
  const double Delta = std::pow(taur,2) - std::pow(2*tau0,2) ;
  if ( Delta > 0 ) {
    double sqrtDelta = std::sqrt(Delta) ;
    double taup = 0.5*( taur + sqrtDelta ) ;
    double taum = 0.5*( taur - sqrtDelta ) ;
    return ( exp(-t/taup) - exp(-t/taum) ) / ( taup - taum ) ;
  } else if ( Delta < 0 ) {
    double T = std::sqrt(-Delta) ;
    double A = 2 * taur / ( std::pow(taur,2) - Delta ) ;
    double B = 2 * T / ( std::pow(taur,2) - Delta ) ;
    return 2 * exp(-A*t) * sin(B*t) / T ;
  } else {
    double tau = 0.5 * taur ;
    return exp(-t/tau) * t / std::pow(tau,2) ;
  }
#if 0
  double taur2 = taur*taur, tau02 = tau0*tau0 ;
  double taua = sqrt( 4.*tau02 - taur2 );
  return (2./taua)*exp(-t*taur/(2.*tau02))*sin(t*taua/(2.*tau02));
#endif
}
