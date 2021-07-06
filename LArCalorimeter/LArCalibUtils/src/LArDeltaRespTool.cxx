/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArDeltaRespTool.h" 
#include "LArCalibUtils/LArPhysWaveTool.h"

const int LArDeltaRespTool::DEFAULT=-1;

// constructor
LArDeltaRespTool::LArDeltaRespTool ( const std::string& type, const std::string& name,const IInterface* parent )
  : AthAlgTool(type,name,parent),
    m_Tdrift(0),
    m_Fstep(0),
    m_Tcal(0),
    m_Omega0(0),
    m_Taur(0),
    m_Tstart(0)
{
  declareProperty("InjPointCorr",m_injPointCorr=true) ;
  declareProperty("NormalizeCali",m_normalizeCali=true) ;
  declareProperty("TimeOriginShift",m_timeOriginShift=false) ;
  declareProperty("SubtractBaseline",m_subtractBaseline=true) ;
}

// destructor 
LArDeltaRespTool::~LArDeltaRespTool() {}

LArCaliWave LArDeltaRespTool::makeLArDeltaResp( const LArWFParams &larWFParam, 
					        const LArCaliWave &larCaliWave )
{

  m_gCali  = larCaliWave;

  m_Tdrift = larWFParam.tdrift();
  m_Tcal   = larWFParam.tcal();
  m_Fstep  = larWFParam.fstep();
  m_Taur   = larWFParam.taur();
  m_Omega0 = larWFParam.omega0();

  compute_deltaresp(); 
  
  return m_gDelta; 
}


void LArDeltaRespTool::compute_deltaresp() 
{
  LArWaveHelper wHelper;

  // calib. signal at Mother Board :
  LArWave gCaliMB(m_gCali), gDelta;  

  // shift gCaliMB to start point and remove baseline
  m_Tstart = wHelper.getStart(gCaliMB) ;
  double baseline = wHelper.getBaseline(gCaliMB,m_Tstart) ;
  if ( m_subtractBaseline ) gCaliMB = gCaliMB + (-baseline) ;
  if ( m_timeOriginShift )  gCaliMB = wHelper.translate(gCaliMB,-m_Tstart,baseline) ;
  
  // normalization of calibration pulse
  if ( m_normalizeCali ) {
    ATH_MSG_VERBOSE ( "*** Normalisation \t|-> YES" );
    double peak = gCaliMB.getSample( wHelper.getMax(gCaliMB) ) ;
    ATH_MSG_VERBOSE  ( "peak = " << peak );
    if ( peak <=0 ) {
      ATH_MSG_WARNING ( "Peak value <=0 , cannot normalize!" );
    } else {
      gCaliMB = gCaliMB * (1./peak)  ;
    }
  } else {
    ATH_MSG_VERBOSE ( "*** Normalisation \t|-> NO" );
  }

  // delta responde waveform prediction
  ATH_MSG_VERBOSE ( "*** Delta response \t|-> m_Fstep   = " << m_Fstep  << " ns " );
  ATH_MSG_VERBOSE ( "*** Delta response \t|-> m_Tcal    = " << m_Tcal   << " ns " );
  ATH_MSG_VERBOSE ( "*** Delta response \t|-> m_Omega0  = " << m_Omega0 << " GHz" );
  ATH_MSG_VERBOSE ( "*** Delta response \t|-> m_Taur    = " << m_Taur   << " ns " );
  
  if ( ! m_injPointCorr ) {
    // perform only exp->triangle correction
    ATH_MSG_VERBOSE ( "*** Inj.Point Corr. \t|-> NO" );
    gDelta = deltaResp ( gCaliMB ) ;
  } else {
    // perform exp->triangle and then injection point correction
    ATH_MSG_VERBOSE ( "*** Inj.Point Corr. \t|-> YES" );
    //gDelta = deltaResp ( gCaliMB ) ;
    //gDelta = LArPhysWaveTool::injResp( gDelta ); // HOW TO MAKE THIS WORKING???
    gDelta = injResp( deltaResp ( gCaliMB ) ) ;
  }
  
  //int deltaRespDAC = m_gCali.getDAC() ;
  int deltaRespDAC = -3 ;
  
  m_gDelta = LArCaliWave( gDelta.getWave() ,
			  m_gCali.getDt() ,
	  	          deltaRespDAC ,
		          m_gCali.getFlag() );
			 
}


LArWave LArDeltaRespTool::deltaResp (const LArWave &w) const 
{
  double fstep = m_Fstep;
  double Tc    = m_Tcal;
  LArWaveHelper wHelper;
  return ( w % deltaCorr() ) + w*((1.-fstep)/Tc) + wHelper.derive_smooth(w);
}

LArWave LArDeltaRespTool::deltaCorr() const 
{
  unsigned N = m_gCali.getSize() ;
  double  dt = m_gCali.getDt() ;
  LArWave w(N,dt) ;
  for ( unsigned i=0 ; i<N ; i++ ) w.setSample(i,deltaCorr(i*dt)) ;
  return w ;
}

double LArDeltaRespTool::deltaCorr ( double t ) const 
{
  double fstep = m_Fstep ;
  double Tc    = m_Tcal ;
  return ((fstep*fstep-fstep)/(Tc*Tc)) * exp( -fstep*t/Tc );
}

LArWave LArDeltaRespTool::injResp (const LArWave& w) const {
  return  w % injCorr() ;
}

LArWave LArDeltaRespTool::injCorr() const {
  unsigned N = m_gCali.getSize() ;
  double dt = m_gCali.getDt() ;
  LArWave w(N,dt) ;
  for ( unsigned i=0 ; i<N ; i++ ) w.setSample(i,injCorr(i*dt)) ;
  return w ;
}

double LArDeltaRespTool::injCorr ( double t ) const {
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
