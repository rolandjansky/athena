/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArPhysWaveTool.h" 

const int LArPhysWaveTool::DEFAULT=-1;

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
					    int region, int layer,
					    LArPhysWave& predLArPhysWave,
					    float& MphysMcali)
{
  // set input objects
  m_region = region ;
  m_layer  = layer ; 
  m_gCali  = larCaliWave;
  m_Tdrift = larWFParam.tdrift();
  m_Tcal   = larWFParam.tcal();
  m_Fstep  = larWFParam.fstep();
  m_Omega0 = larWFParam.omega0();
  m_Taur   = larWFParam.taur();
  
  // calculates m_gPhys from m_gCali, and m_MphysMcali
  predict_phys(); 
  
  // set output objects;
  predLArPhysWave = m_gPhys ;
  MphysMcali = m_MphysMcali ;
  
  return StatusCode::SUCCESS; 
}

void LArPhysWaveTool::predict_phys() {
  MsgStream log(msgSvc(), name());
  // calib. signal at Mother Board :
  LArWave gCaliMB(m_gCali) , gPhys ;
  LArWaveHelper wHelper;

  // shift gCaliMB to start point and remove baseline

  m_Tstart = wHelper.getStart(gCaliMB) ;
  double baseline = wHelper.getBaseline(gCaliMB,m_Tstart) ;
  if ( m_subtractBaseline )   gCaliMB = gCaliMB + (-baseline) ;
  if ( m_timeOriginShift )    gCaliMB = wHelper.translate(gCaliMB,-m_Tstart,baseline) ;
  
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

  ATH_MSG_VERBOSE ( "*** Physics waveform\t|-> m_Tdrift  = " << m_Tdrift << " ns " );
  ATH_MSG_VERBOSE ( "*** Physics waveform\t|-> m_Fstep   = " << m_Fstep  << " ns " );
  ATH_MSG_VERBOSE ( "*** Physics waveform\t|-> m_Tcal    = " << m_Tcal   << " ns " );
  ATH_MSG_VERBOSE ( "*** Physics waveform\t|-> m_Omega0  = " << m_Omega0 << " GHz" );
  ATH_MSG_VERBOSE ( "*** Physics waveform\t|-> m_Taur    = " << m_Taur   << " ns " );
  
  //  bool doInjPointCorr = ( ( ( m_region==0 && m_layer>=0 && m_layer<4 && m_injPointCorrLayer[m_layer] ) || m_injPointCorr )
  //			  && m_Omega0 != 0. ) ;

  bool doInjPointCorr = ( ( ( m_layer>=0 && m_layer<4 && m_injPointCorrLayer[m_layer] ) || m_injPointCorr )
			  && m_Omega0 != 0. ) ;
  
  if ( ! doInjPointCorr ) {
    // perform only exp->triangle correction
    ATH_MSG_VERBOSE ( "*** Inj. Point Corr \t|-> NO" );
    gPhys = exp2Tri ( gCaliMB ) ;
  } else {
    // perform exp->triangle and then injection point correction
    ATH_MSG_VERBOSE ( "*** Inj. Point Corr \t|-> YES" );
    if ( !m_injPointUseTauR[m_layer] ) {
      m_Taur = 0.;
      ATH_MSG_VERBOSE ( "*** Inj. Point TauR \t|-> NO" );
    }
    gPhys = injResp ( exp2Tri ( gCaliMB ) );
  }
  
  // compute Mphys/Mcal
  if ( m_normalizeCali ) {
     // caliwave is normalized to 1 => Mcali = 1
     m_MphysMcali = gPhys.getSample( wHelper.getMax(gPhys) ) ;
  } else {
     m_MphysMcali = gPhys.getSample( wHelper.getMax(gPhys) ) /
                    gCaliMB.getSample( wHelper.getMax(gCaliMB) ) ;
  }  
  ATH_MSG_VERBOSE ( "*** Physics waveform\t|-> m_MphysMcali = " << m_MphysMcali );
  
  m_gPhys = LArPhysWave( gPhys.getWave() ,
			 m_gCali.getDt() );
			 
  return ;			 
}

LArWave LArPhysWaveTool::exp2Tri (const LArWave &w) const {
  return w + ( w % caliPhysCorr() ) ;
}
LArWave LArPhysWaveTool::caliPhysCorr() const {
  unsigned N = m_gCali.getSize() ;
  double dt = m_gCali.getDt() ;
  LArWave w(N,dt);
  for ( unsigned i=0 ; i<N ; i++ ) 
    w.setSample(i,caliPhysCorr(i*dt)) ;
  return w ;
}

/* =====================================================================
 * Function: Calibration to Ionisation correction function
 * ===================================================================== */
double LArPhysWaveTool::caliPhysCorr ( double t ) const {
  double fstep = m_Fstep ;
  double Tc    = m_Tcal ;
  double Td    = m_Tdrift ;
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
LArWave LArPhysWaveTool::injResp (const LArWave& w) const {
  return  w % injCorr() ;
}
                                                                                
LArWave LArPhysWaveTool::stepResp () const {
  return m_gCali + m_gCali % stepCorr() ;
}
LArWave LArPhysWaveTool::step2Tri (const LArWave& w) const {
  return  w + w % stepPhysCorr() ;
}
/* =================================================================
 * Function: Step response to Ionisation correction function
 * =============================================================== */
double LArPhysWaveTool::stepPhysCorr ( double t ) const {
  double Td = m_Tdrift ;
  if ( t<0. || t>=Td ) return 0. ;
  else return -1./Td ;
}
LArWave LArPhysWaveTool::stepPhysCorr() const {
  unsigned N = m_gCali.getSize() ;
  double dt = m_gCali.getDt() ;
  LArWave w(N,dt) ;
  for ( unsigned i=0 ; i<N ; i++ ) w.setSample(i,stepPhysCorr(i*dt)) ;
  return w ;
}


LArWave LArPhysWaveTool::stepCorr() const {
  unsigned N = m_gCali.getSize() ;
  double dt = m_gCali.getDt() ;
  LArWave w(N,dt) ;
  for ( unsigned i=0 ; i<N ; i++ ) w.setSample(i,stepCorr(i*dt)) ;
  return w ;
}
double LArPhysWaveTool::stepCorr ( double t ) const {
  double fstep = m_Fstep ;
  double Tc    = m_Tcal ;
  return (1.-fstep)/Tc * exp( -fstep*t/Tc );
}


LArWave LArPhysWaveTool::injCorr() const {
  unsigned N = m_gCali.getSize() ;
  double dt = m_gCali.getDt() ;
  LArWave w(N,dt) ;
  for ( unsigned i=0 ; i<N ; i++ ) w.setSample(i,injCorr(i*dt)) ;
  return w ;
}
/* =================================================================
 * Function: injection point correction function
 * =============================================================== */
double LArPhysWaveTool::injCorr ( double t ) const {
  double tau0 = 1./m_Omega0;
  double taur = m_Taur;
  double Delta = pow(taur,2.) - pow(2*tau0,2.) ;
  if ( Delta > 0 ) {
    double sqrtDelta = sqrt(Delta) ;
    double taup = 0.5*( taur + sqrtDelta ) ;
    double taum = 0.5*( taur - sqrtDelta ) ;
    return ( exp(-t/taup) - exp(-t/taum) ) / ( taup - taum ) ;
  } else if ( Delta < 0 ) {
    double T = sqrt(-Delta) ;
    double A = 2 * taur / ( pow(taur,2.) - Delta ) ;
    double B = 2 * T / ( pow(taur,2.) - Delta ) ;
    return 2 * exp(-A*t) * sin(B*t) / T ;
  } else {
    double tau = 0.5 * taur ;
    return exp(-t/tau) * t / pow(tau,2.) ;
  }
#if 0
  double taur2 = taur*taur, tau02 = tau0*tau0 ;
  double taua = sqrt( 4.*tau02 - taur2 );
  return (2./taua)*exp(-t*taur/(2.*tau02))*sin(t*taua/(2.*tau02));
#endif
}
