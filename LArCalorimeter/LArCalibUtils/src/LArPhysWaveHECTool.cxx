/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArPhysWaveHECTool.h" 

#include "gsl/gsl_integration.h"
#include <gsl/gsl_integration.h>
#include <gsl/gsl_errno.h>

#include  <iostream> 
#include <fstream>
//#include "TCanvas.h"
#include <vector>

#include <TGraphSmooth.h>

using std::cout;
using std::endl;

const int NMAX=1000;

static Double_t Tp(Double_t *t, Double_t *par);
static Double_t Tp_gsl(Double_t *t, Double_t *par);
static Double_t DTp(Double_t *t, Double_t *par);
static Double_t Tp4(Double_t t);
static Double_t Tp5_gsl(Double_t t);
static Double_t Tp4_gsl(Double_t t);
static Double_t Tp5(Double_t t);
static Double_t Tc(Double_t t);
Double_t normalize(TF1 *func, Double_t *rampl=NULL,
                   Double_t from=0., Double_t to = 0., Double_t step = 1.);

Double_t normalize_prof(TProfile *histo, Double_t *rampl=NULL);

static  Double_t tau0, taup, tauz, taus, rc, rz, rs, sp, s0, sz, ss, sc, fs,
                 tdr, tc, a, t0, al;
static Double_t x[NMAX], y[NMAX], STEP, *pref = NULL;

static Double_t f4_g(Double_t *x, Double_t *par);
static Double_t f5_g(Double_t *x, Double_t *par);

static double f4_gsl(double x, void *par);
static double f5_gsl(double x, void *par);

struct gsl_f_params { double a; };

struct usertim {Double_t t;};

static TSpline3 *splin = NULL;

void Smooth_new(Double_t *x, Double_t *y, Double_t *yref, Int_t ymax);
void SmootKern(Double_t *x, Double_t *y, Int_t ymax, Double_t band = 10.);

const int LArPhysWaveHECTool::DEFAULT=-1;
int LArWaveFlag=0, Tp4_gsl_err_counter=0, Tp5_gsl_err_counter=0;

LArPhysWaveHECTool::LArPhysWaveHECTool ( const std::string& type, const std::string& name,const IInterface* parent )
  : AthAlgTool(type,name,parent)
{

  // Declare additional interface
  declareInterface<LArPhysWaveHECTool>(this);

  declareProperty("NormalizeCali",    m_normalizeCali=false) ;//true?
  declareProperty("TimeOriginShift",  m_timeOriginShift=false) ;
  declareProperty("SubtractBaseline", m_subtractBaseline=true) ;
  declareProperty("TcalMin",          m_TcalMin=370) ;
  declareProperty("TcalMax",          m_TcalMax=490) ;
  declareProperty("TcalAverage",      m_TcalAverage=422.2) ;
  declareProperty("FstepMin",         m_FstepMin=0.02) ;
  declareProperty("FstepMax",         m_FstepMax=0.1) ;
  declareProperty("FstepAverage",     m_FstepAverage=0.068) ;
  declareProperty("MinAmplitude",     m_MinAmp=100.);  
  //declareProperty("NSteps",           m_NStep=24); //added by FT
}

LArPhysWaveHECTool::~LArPhysWaveHECTool() {}

StatusCode LArPhysWaveHECTool::initialize()
{    
  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );
  return StatusCode::SUCCESS;
}


StatusCode LArPhysWaveHECTool::makeLArPhysWaveHEC(LArWFParams& wfParam, LArCaliWave &caliWave,
						  LArPhysWave & predLArPhysWave, const LArPhysWave & idealPhysWave,
						  float & MphysMcali, const HWIdentifier& chid, const int gain, 
						  int & LArPhysWaveFlag) {
  LArPhysWaveFlag=LArWave::predCali;
  Tp4_gsl_err_counter=0, Tp5_gsl_err_counter=0;
  // set input objects
  m_Omega0 = 0.;
  m_Taur   = 0.;
  m_gIdealPhys = &idealPhysWave;

//  if(*(std::max_element(larCaliWave.getWave().begin(), larCaliWave.getWave().end())) < m_MinAmp) {
//    return StatusCode::FAILURE;
//  }
  if(wfParam.fstep()<m_FstepMin || wfParam.fstep()>m_FstepMax){ 
    ATH_MSG_INFO (" Fstep="<< wfParam.fstep() << " out of accepted region ("<<m_FstepMin<< ","<<m_FstepMax<<") average used instead : "<<m_FstepAverage);
    wfParam.setFstep(m_FstepAverage);
  }
  if(wfParam.tcal()<m_TcalMin || wfParam.tcal()>m_TcalMax ){
    ATH_MSG_INFO (" Tcal="<< wfParam.tcal() << "out of accepted region ("<<m_TcalMin<< ","<<m_TcalMax<<") average used instead : "<<m_TcalAverage);
    wfParam.setTcal(m_TcalAverage);
  }
  //if(m_Fstep<0.01) m_Fstep=0.65; // 0.04-.09  -> property into JO
  //if(m_Tcal>600)   m_Tcal=417;   // 400-460   -> property into JO

  ATH_MSG_DEBUG (" Tdrift="<< wfParam.tdrift() <<" Tcal="<<wfParam.tcal()<<" Fstep="<< wfParam.fstep()<<" m_Omega0="<< m_Omega0<<" m_Taur="<<m_Taur<<" LArWaveFlag="
                 << LArWaveFlag <<" LArPhysWaveFlag="<< LArPhysWaveFlag );
  predict_phys_HEC(wfParam,caliWave,predLArPhysWave, MphysMcali,chid,gain);
  //LArPhysWaveFlag=LArWaveFlag;
//  std::cout<<"makeLArPhysWaveHEC: LArWaveFlag="<<LArWaveFlag<<" LArPhysWaveFlag="<< LArPhysWaveFlag <<std::endl;
  //predLArPhysWave = m_gPhys ;
  
  return StatusCode::SUCCESS; 
}

 void LArPhysWaveHECTool::predict_phys_HEC(LArWFParams& wfParam, LArCaliWave &caliWave,
					  LArPhysWave & predLArPhysWave, 
					  float & MphysMcali, const HWIdentifier& chid, const int gain) {

  // calib. signal at Mother Board :
  LArWave gCaliMB=caliWave ;//deep-copy and cast to base-class
  LArWaveHelper wHelper;

  //std::cout << "Step1:  CaliPeak =" <<  gCaliMB.getSample( wHelper.getMax(gCaliMB))  << " at index " << wHelper.getMax(gCaliMB) << std::endl;
  //std::cout << "Step1:  IdealPeak =" <<  m_gIdealPhys->getSample( wHelper.getMax(*m_gIdealPhys) ) << " at index " << wHelper.getMax(*m_gIdealPhys) << std::endl;

  // shift gCaliMB to start point and remove baseline

  m_Tstart = wHelper.getStart(gCaliMB) ;
  double baseline = wHelper.getBaseline(gCaliMB,m_Tstart) ;
  if ( m_subtractBaseline )   gCaliMB = gCaliMB+ (-baseline);
  if ( m_timeOriginShift )    gCaliMB = wHelper.translate(gCaliMB,-m_Tstart,baseline) ;
  
  // normalization of calibration pulse
  double peak_tmp=gCaliMB.getSample( wHelper.getMax(gCaliMB) ) ;
  if ( m_normalizeCali ) {
    double peak = gCaliMB.getSample( wHelper.getMax(gCaliMB) ) ;
    peak_tmp=peak;
    ATH_MSG_VERBOSE ( "*** Normalisation \t|-> YES (CaliWave peak = " << peak << ")" );
    if ( peak <=0 ) {
      ATH_MSG_WARNING ( "Peak value <=0 , cannot normalize!" );
    } else {
      gCaliMB = gCaliMB * (1./peak)  ;
    }
  } else {
    ATH_MSG_VERBOSE ( "*** Normalisation \t|-> NO" );
  }

  
  int FT      = m_onlineHelper->feedthrough(chid);
  int Slot    = m_onlineHelper->slot(chid);
  int Channel = m_onlineHelper->channel(chid);
  int adc = 128*(Slot-5)+ Channel+1;

  ATH_MSG_VERBOSE ( "*** Physics waveform\t|-> FT=" << FT   << " Slot=" << Slot << " Channel=" <<Channel<< " adc=" << adc << " gain=" << gain );
  ATH_MSG_VERBOSE ( "*** Physics waveform\t|-> m_Tdrift  = " << wfParam.tdrift() << " ns " );
  ATH_MSG_VERBOSE ( "*** Physics waveform\t|-> m_Fstep   = " << wfParam.fstep()  << " ns " );
  ATH_MSG_VERBOSE ( "*** Physics waveform\t|-> m_Tcal    = " << wfParam.tcal() << " ns " );
  


  ///////////////////////////////////////////////////////////////////////
  unsigned int CaliWaveBins = gCaliMB.getSize();
  double const BinWidth=25.0/24;
  double const HalfBinWidth=BinWidth/2.0;
  double par[3]={0.0,1.0,0.10};
  bool uset0=false, norm=false;
  //norm=m_normalizeCali;
 
  double const QUAL_REQ=peak_tmp*0.04; //80 ADC units is max allowed deviation of Ampl between two bins
  double const QUAL_REQ_AMPL=peak_tmp*0.3;
  //double const QUAL_REQ=400; //80 ADC units is max allowed deviation of Ampl between two bins
  //double const QUAL_REQ_AMPL=40;
  double DIFF=0,DIFF_AMPL=0;
  unsigned short int repro_count=0;
  int idx_bad_time=-1,idx_bad_time_ampl=-1;
  TF1 *PhysWaveFunc=0;
  const unsigned short int ITER_MAX = 10;
  double Ampl_problem=-1, Time_problem=-1, Ampl_problem_ampl=-1, Time_problem_ampl=-1;
  double CALIWAVE_SHIFT=0;
  TProfile pcal("pcal","Amplitude vs Time",CaliWaveBins,0-HalfBinWidth,CaliWaveBins*BinWidth-HalfBinWidth);
  while((  (DIFF>=QUAL_REQ || DIFF_AMPL>=QUAL_REQ_AMPL) && repro_count<ITER_MAX)
	   || repro_count==0){ 

    // if deviation is above a limit, move the CALIWAVE by 1.0 ADC counts
    CALIWAVE_SHIFT=1.0*repro_count;

    if(repro_count>0){
      ATH_MSG_INFO ("FT="<<FT<<" Slot="<<Slot<<" Ch="<<Channel<<" Gain="<<gain<<" adc="<<adc);
      ATH_MSG_INFO (repro_count<<". Iteration of INTEGRATION: CALIWAVE IS MOVED UP by "<<CALIWAVE_SHIFT<<" ADC units");
      if(DIFF_AMPL>=QUAL_REQ_AMPL)
	ATH_MSG_INFO ("Problematic bin="<<idx_bad_time_ampl<<" AmplPhysGSL="<<Ampl_problem_ampl<<" Time="<< Time_problem_ampl <<" Deviation="<<DIFF_AMPL<<" ADC units"<<" Peak="<<peak_tmp);
      if(DIFF>=QUAL_REQ)
	ATH_MSG_INFO ("Problematic bin="<<idx_bad_time<<" AmplPhysGSL="<<Ampl_problem<<" Time="<<Time_problem<<" Deviation="<<DIFF<<" ADC units"<< " Peak="<<peak_tmp);
    }

    pcal.Reset();
    for(unsigned tbin=1;tbin<=gCaliMB.getSize();++tbin){
      double AmplCalib = gCaliMB.getSample(tbin-1)+CALIWAVE_SHIFT;
      double Time      = gCaliMB.getTime(tbin-1);
      pcal.Fill(Time,AmplCalib,1);
    }
    bool gslflag=true;
    Double_t *xmax=0;
    //double parCL[2]={m_Tcal,m_Fstep};
    double parCL[2]={wfParam.tcal(),wfParam.fstep()};
    TF1 *deriv=0;
    PhysWaveFunc= (TF1*)CaliWave2PhysWaveHEC(&pcal, par, parCL, deriv, uset0, norm, adc, xmax,gslflag)->Clone();  
    //PhysWaveFunc=CaliWave2PhysWaveHEC(&pcal, par, parCL, deriv, uset0, norm, adc, xmax,gslflag);
    DIFF=0;DIFF_AMPL=0;Ampl_problem=-1; Time_problem=-1; Ampl_problem_ampl=-1; Time_problem_ampl=-1;
    if (deriv) delete deriv;

    for(int i=1;i<pcal.GetNbinsX(); ++i){
      double TimePhys1 = pcal.GetBinCenter(i+1);
      double TimePhys0 = TimePhys1-BinWidth;
      double DIFF_TMP = PhysWaveFunc->Eval(TimePhys1)-PhysWaveFunc->Eval(TimePhys0);
      if(TimePhys1>200 && DIFF<fabs(DIFF_TMP)){
	DIFF=fabs(DIFF_TMP);
	idx_bad_time=i;
	Ampl_problem=PhysWaveFunc->Eval(TimePhys1);
	Time_problem=TimePhys1;
      }
      else if(TimePhys1<=200 && DIFF_AMPL<fabs(DIFF_TMP)){
	DIFF_AMPL=fabs(DIFF_TMP);
	idx_bad_time_ampl=i;
	Ampl_problem_ampl=PhysWaveFunc->Eval(TimePhys1);
	Time_problem_ampl=TimePhys1;
      } }
    repro_count++;
  }

  if(repro_count>=ITER_MAX && (DIFF>=QUAL_REQ || DIFF_AMPL>=QUAL_REQ_AMPL)){
    ATH_MSG_WARNING ("FT="<<FT<<" Slot="<<Slot<<" Ch="<<Channel<<" Gain="<<gain<<" #iterations for CALIWAVE increasing reached the limit! LArWaveFlag set to -1" );
    LArWaveFlag=-1;
  }

  predLArPhysWave=LArPhysWave(gCaliMB.getSize(),gCaliMB.getDt()); //Assignment per value, but wave is empty at this point
  //predLArPhysWave.setSize(gCaliMB.getSize()); //FIXME: Doesnt' resize the vectors holding error and trigger! 
  //predLArPhysWave.setDt(gCaliMB.getDt());
  
  for(unsigned int i=0;i<gCaliMB.getSize(); ++i){
    const double Time  = gCaliMB.getTime(i);
    if(Time<=t0){
      predLArPhysWave.setSample(i,PhysWaveFunc->Eval(Time));
      //FT cout<<"PhysWaveFunc->Eval("<<Time<<")="<<PhysWaveFunc->Eval(Time)<<" PhysWaveVec="<<PhysWaveVec[i]<<" CALIWAVE_SHIFT - NOT USED"<<std::endl;
    }
    else{
      predLArPhysWave.setSample(i,PhysWaveFunc->Eval(Time)-CALIWAVE_SHIFT);
      //cout<<"PhysWaveFunc->Eval("<<Time<<")="<<PhysWaveFunc->Eval(Time)<<" PhysWaveVec="<<PhysWaveVec[i]<<" CALIWAVE_SHIFT="<<CALIWAVE_SHIFT<<std::endl;
    }
  }

  /////////////////////////////////////////////////////

  // compute Mphys/Mcal
  if ( m_normalizeCali ) {
     // caliwave is normalized to 1 => Mcali = 1
    MphysMcali = predLArPhysWave.getSample( wHelper.getMax(predLArPhysWave) ) ;
    std::cout<<"Normalized: MphysMcali="<<MphysMcali<<std::endl;
  } else {
    MphysMcali = predLArPhysWave.getSample( wHelper.getMax(predLArPhysWave) ) /
      gCaliMB.getSample( wHelper.getMax(gCaliMB) ) ;
  }  
  ATH_MSG_VERBOSE ( "*** Physics waveform\t|-> m_MphysMcali = " << MphysMcali << " adc=" <<adc);
  
  return ;			 
}

static Double_t P(Double_t *tt, Double_t *par)
{
   return par[1]*splin->Eval(*tt+par[0]);
}

static Double_t Rd(Double_t t)
{
 static Double_t res;

 if(t<0 || t>tdr) return 0;

 res = rc*((1+sc*al*tdr)*exp(-t*sc*al)-1) + rz*((1+sz*tdr)*exp(-t*sz)-1);
 res += rs*((1+ss*tdr)*exp(-t*ss)-1) - 1;
 res *= fs/tdr;

 return res;
}

static Double_t Ro(Double_t t)
{
 static Double_t res;

 if(t<tdr) return 0;
 
 res = rc*((1+sc*al*tdr)*exp(-t*sc*al) - exp((tdr-t)*sc*al));
 res += rz*((1+sz*tdr)*exp(-t*sz) - exp((tdr-t)*sz));
 res += rs*((1+ss*tdr)*exp(-t*ss) - exp((tdr-t)*ss));
 res *= fs/tdr;

 return res;
}

Double_t Tc(Double_t t)
{
 static Double_t res;

 if(t<t0) return 0;
 
 res = splin->Eval(t);

/*
 i=t/STEP;
 res = y[i] + (y[i+1] - y[i])/(x[i+1] - x[i])*(t - x[i]);
*/
 return res;
}


Double_t DTp(Double_t *tt, Double_t *par)
{
 static Double_t res,t;
 //   static Double_t time;
 
 static TF1 *ff=NULL;
 static TGraph *gg=NULL;
 static Double_t xx[21],yy[21];
 static Int_t j;

 if(*tt < t0) return 0.;
 t = *tt;

 if(ff==NULL) ff=new TF1("ff","pol2",0.,800.);
 if(gg==NULL) gg=new TGraph(11,xx,yy);


 ff->SetRange(*tt-5.,*tt+5.);
 for(j=-10; j<=10; ++j) {
           res = t + j/2.;
//	   cout<<"DTp: "<<t+j/2.<<" "<<Tp(&res,par)<<endl;
           gg->SetPoint(j+10,t+j/2.,Tp(&res,par));
          }
 if(gg->GetFunction("ff")) gg->GetFunction("ff")->Delete();
 gg->Fit("ff","QRI0");
 res = 2*t*ff->GetParameter(2) + ff->GetParameter(1);

 /*
 res = 0.;
 time = t-2.; res -= 2*Tp(&time,par)/10;
 time = t-1.; res -= Tp(&time,par)/10;
 time = t+1.; res += Tp(&time,par)/10;
 time = t+2.; res += 2*Tp(&time,par)/10;
 */
 return res; 
}

Double_t Tp(Double_t *tt, Double_t *par)
{
 static Double_t res;
 static Double_t t,t1,t4,t5;

 if(*tt < t0 + par[0]) return 0;
// if(par[0] < 0) return 0;

 t = *tt - par[0];
 taus = par[2];
// tc = par[2];
 
// Redefinition of poles

  sp = 1/taup; s0 = 1/tau0; sz = 1/tauz; ss = 1/taus; sc = 1/tc;
  //
// Coefficients

 rc = pow(al,3)*sc*sc + al*al*(-sc*sc-sc*s0-sc*sp);
 rc += al*(sc*s0+sc*sp+sp*s0) - sp*s0;
 rc /= -al*(al*al*sc*sc + al*(-sc*ss-sc*sz) + sz*ss);

 rz = sz*sz*(sc+s0+sp) - pow(sz,3) - sz*sc*(s0+sp) + sp*s0*(sc-sz);
 rz /= sz*(al*(-sc*sz+sc*ss) + sz*sz - sz*ss);

 rs = -ss*ss*(sc+s0+sp-ss) + sc*ss*(s0+sp) + ss*sp*s0 - sc*sp*s0;
 rs /= ss*(al*(-sc*sz+sc*ss) + sz*ss - ss*ss);

 fs = sz*ss / (s0*sp);

 // cout<<*tt<<" "<<t<<"       "<<t0<<" "<<a<<endl;
 t1 = fs*Tc(t);

 t4 = Tp4(t); t5 = Tp5(t);
 // printf("%6f %15.10g %15.10g    %15.10g %15.10g \n",t,t1, Tc(t), t4,t5);
 res = t1 + t4 + t5;

 // ++count;
 // cout<<"         "<<res<<endl;
 return par[1]*res;
}

Double_t Tp_gsl(Double_t *tt, Double_t *par)
{
 static Double_t res;
 static Double_t t,t1,t4,t5;

 if(*tt < t0 + par[0]) return 0;
// if(par[0] < 0) return 0;

 t = *tt - par[0];
 taus = par[2];
// tc = par[2];
 
// Redefinition of poles

  sp = 1/taup; s0 = 1/tau0; sz = 1/tauz; ss = 1/taus; sc = 1/tc;
  //
// Coefficients

 rc = pow(al,3)*sc*sc + al*al*(-sc*sc-sc*s0-sc*sp);
 rc += al*(sc*s0+sc*sp+sp*s0) - sp*s0;
 rc /= -al*(al*al*sc*sc + al*(-sc*ss-sc*sz) + sz*ss);

 rz = sz*sz*(sc+s0+sp) - pow(sz,3) - sz*sc*(s0+sp) + sp*s0*(sc-sz);
 rz /= sz*(al*(-sc*sz+sc*ss) + sz*sz - sz*ss);

 rs = -ss*ss*(sc+s0+sp-ss) + sc*ss*(s0+sp) + ss*sp*s0 - sc*sp*s0;
 rs /= ss*(al*(-sc*sz+sc*ss) + sz*ss - ss*ss);

 fs = sz*ss / (s0*sp);

// cout<<*tt<<" "<<t<<"       "<<t0<<" "<<a<<endl;
 t1 = fs*Tc(t);

 t4 = Tp4_gsl(t); t5 = Tp5_gsl(t);
 // printf("%6f %15.10g %15.10g    %15.10g %15.10g \n",t,t1, Tc(t), t4,t5);
 res = t1 + t4 + t5;

// ++count;
// cout<<"         "<<res<<endl;
 return par[1]*res;
}

// version for GSL

Double_t Tp4_gsl(Double_t t)
{
  static Double_t a,b;
  
  //  static Double_t a,b, par[1];
  //static const Double_t epsrel = 0.001;

  //BEG: variables for QAWO
  static double L;
  static const double omega=0.0;
  static const size_t n=100;
  static const size_t limit=1001; // "n" must be < limit
  static const double epsabs=0.0;
  static double epsrel=1e-10;
  double abserr;
  static gsl_integration_workspace  *w=0;
  static gsl_integration_qawo_table *wf=0;
  //END: variables for QAWO

  struct gsl_f_params params = {t};
  gsl_function gsl_func;
  
  a = t - tdr;
  if(a < t0) a = t0;
  b = t;
  
  //for gsl_integration_qawo:
  L=b-a;
  if(!w) w = gsl_integration_workspace_alloc(limit);
  if(!wf) wf=gsl_integration_qawo_table_alloc(omega,L,GSL_INTEG_COSINE,n); 

  gsl_integration_qawo_table_set_length(wf, L);
  //end of gsl_integration_qawo

  gsl_func.function = &f4_gsl;
  gsl_func.params   = &params;
  double result;
  //gsl_integration_qawo_table_set(wf,omega,L,GSL_INTEG_COSINE);

  // set off automatic GSL error handling
  gsl_set_error_handler_off();
  // set integration precision 
  if(b>t0+130) epsrel=1e-8;
  else         epsrel=1e-10;

  unsigned short int INTEGRAL_ITER=0;
  const unsigned short int INTEGRAL_ITER_MAX=10;
  int status=-1;

    // compute integral with the starting high precision
  while((status!=0 || INTEGRAL_ITER==0) && INTEGRAL_ITER<INTEGRAL_ITER_MAX){
    status = gsl_integration_qawo (&gsl_func,a, epsabs, epsrel,limit,w,wf,&result,&abserr);

    // if failes, descrease the precision
    if(status == GSL_EROUND)      epsrel*=10;
    // if different error from GSL_EROUND, annonce integration ERROR
    else if(status!=0){
      if(Tp4_gsl_err_counter==0)
	std::cout<< "WARNING Tp4_gsl:QAWO ERROR (not roundoff): status="<<gsl_strerror(status)<<" ERRcode="<<status << " LArWaveFlag set to -1 !!"<< std::endl ;
      status=0;
      LArWaveFlag=-1;
      Tp4_gsl_err_counter++;
    }
    INTEGRAL_ITER++;
  }
  //if iteration of integration reaches the limit (=> bad precision) set LArWaveFlag to -1 
  if(INTEGRAL_ITER==INTEGRAL_ITER_MAX && status!=0){
    std::cout<<"WARNING  Integration of Tp4_gsl FAILED !!!! #iterations reached the MAXIMUM!!! status="<<gsl_strerror(status)<<" ERRcode="<<status << " LArWaveFlag set to -1 !!"<<std::endl ;
    LArWaveFlag=-1;
  }

//  gsl_integration_qawo_table_free(wf);
//  gsl_integration_workspace_free(w);
  return result;

}
// Integrand for the Tp4 function
// version for GSL
static double f4_gsl(Double_t x, void *par)
{
  struct gsl_f_params * params = (struct gsl_f_params *)par;
  static Double_t t;
  
  t = params -> a;
  //cout<<"params="<<params->a<<endl;
  //t = par[0];

  return  Tc(x) * Rd(t - x);
}

// version for GSL

Double_t Tp5_gsl(Double_t t)
{
   if(t<t0 + tdr) return 0.;
  Double_t b;
  
  //  static Double_t a,b, par[1];
  //static const Double_t epsrel = 0.001;

  //BEG: variables for QAWO
  static double L;
  static const double omega=0.0;
  static const size_t n=100;
  static const size_t limit=1001; // "n" must be < limit
  static const double epsabs=0.0;
  static double epsrel = 1e-10;
  double abserr;
  static gsl_integration_workspace  *w=0;
  static gsl_integration_qawo_table *wf=0;
  //END: variables for QAWO

  struct gsl_f_params params = {t};
  gsl_function gsl_func;
  double a = 0.;
  b = t - tdr;
  
  //for gsl_integration_qawo:
  L=b-a;
  if(!w) w = gsl_integration_workspace_alloc(limit);
  if(!wf) wf=gsl_integration_qawo_table_alloc(omega,L,GSL_INTEG_COSINE,n); 

  gsl_integration_qawo_table_set_length(wf, L);
  //end of gsl_integration_qawo

  gsl_func.function = &f5_gsl;
  gsl_func.params   = &params;

  
  //gsl_integration_qawo_table_set(wf,omega,L,GSL_INTEG_COSINE);

  // set off automatic GSL error handling
  gsl_set_error_handler_off();
  // set integration precision 
  if(b>t0+60) epsrel=1e-8;    // behinde the peak = tale region
  else         epsrel=1e-10;  // peak/amplitude region

  double result;
  unsigned short int INTEGRAL_ITER=0;
  const unsigned short int INTEGRAL_ITER_MAX=10;
  int status=-1;

  while((status!=0 || INTEGRAL_ITER==0) && INTEGRAL_ITER<INTEGRAL_ITER_MAX){
    status = gsl_integration_qawo (&gsl_func,a, epsabs, epsrel,limit,w,wf,&result,&abserr);
    //FT std::cout<<"Tp5_gsl integration: result="<<result<<" epsrel="<<epsrel<<" a="<<a<<" b="<<b<<" status="<<gsl_strerror(status)<<" ERRcode="<<status<<" limit="<<limit<<std::endl;
    // if failes, descrease the precision
    if(status == GSL_EROUND)      epsrel*=10;
    // if different error from GSL_EROUND, annonce integration ERROR
    else if(status!=0){
      if(Tp5_gsl_err_counter==0)
	std::cout <<"WARNING: Tp5_gsl:QAWO ERROR (not roundoff): status="<<gsl_strerror(status)<<" ERRcode="<<status << " LArWaveFlag set to -1 !!"<< std::endl ;
      status=0;
      LArWaveFlag=-1;
      Tp5_gsl_err_counter++;
    }
    INTEGRAL_ITER++;
  }
  //if iteration of integration reaches the limit (=> bad precision) set LArWaveFlag to -1 
  if(INTEGRAL_ITER==INTEGRAL_ITER_MAX && status!=0){
    std::cout<<"WARNING: Integration of Tp5_gsl FAILED: #iterations reached the MAXIMUM!!!! status="<<gsl_strerror(status)<<" ERRcode="<<status << " LArWaveFlag set to -1 !!"<<std::endl ;
    LArWaveFlag=-1;
  }

//  gsl_integration_qawo_table_free(wf);
//  gsl_integration_workspace_free(w);

  return result;

}

// Integrand for the Tp5 function
// version for the GSL

static double f5_gsl(Double_t x, void *par)
{
  struct gsl_f_params * params = (struct gsl_f_params *)par;
  static Double_t t;
  
  t = params -> a;
  //t = par[0];

  return  Tc(x) * Ro(t - x);
}


/////////////BEG///GAUS:ROOT INT////////////////////
//version for the Gauss
Double_t Tp4(Double_t t)
{
  static Double_t a,b;//, par[1];
  static const Double_t epsrel = 0.001;
  static TF1 *fun4 = NULL;

  if(fun4 == NULL) fun4 = new TF1("fun4",&f4_g,0.,NMAX,1);

  a = t - tdr;
  b = t;


  fun4->SetParameter(0,t);
  return fun4->Integral(a,b,epsrel);

}
// Integrand for the Tp4 function
// version for the Gauss
static Double_t f4_g(Double_t *x, Double_t *par)
{
  static Double_t t;
 
  t = par[0];

  return  Tc(*x) * Rd(t - *x);
}

// version for the Gauss
Double_t Tp5(Double_t t)
{
  static Double_t b;//, par[1];
  static const Double_t a = 0., epsrel = 0.001;
  static TF1 *fun5 = NULL;

  if(fun5 == NULL) fun5 = new TF1("fun5",&f5_g,0.,NMAX,1);

  b = t - tdr;
  fun5->SetParameter(0,t);
  return fun5->Integral(a,b,epsrel);
}

// Integrand for the Tp5 function
// version for the Gauss

static Double_t f5_g(Double_t *x, Double_t *par)
{
  static Double_t t;
 
  t = par[0];

  return  Tc(*x) * Ro(t - *x);
}

/////////////END///GAUS:ROOT INT////////////////////

TF1 * LArPhysWaveHECTool::CaliWave2PhysWaveHEC(TProfile *pcal, Double_t *par, double *parCL, TF1 *& deriv, 
					       Bool_t uset0, Bool_t norm, Int_t /*adc*/,  Double_t */*xmax*/, bool gsl_flag){

  //std::cout<<"1.ADC in CaliWave2PhysWaveHEC = "<<adc<<std::endl;
 TF1 *fitfun, *fu; 
 Int_t i,nbin,nbino,i5bin,count ;
 Double_t /*ptim,*/ pampl, t5, mcut, rshift, rmult;
 //Double_t vref;
 //Double_t tref;

 // to be computed
//  t0 = 84.;
//  t0 = 54.;
// Cables amplitude
  a = 0.974;
// Cables poles
  tau0 = 1.5; 
// New value according to findings on Sept. 2001 run
  taup = 19.5; 
//  taup = 21.8; 
  tauz = 18.;
// Should be smaller
//  taus = 2.5;
  taus = 0.1;
// Step amplitude
//  al = 0.0697;
//  al = 0.063;
  al=parCL[1]; // FT: can be exchanged with m_Fstep
//  al = 0.0727;
// Calibration exponenta
//  tc = 330.0;
//    tc = 356.;
//tc = 370.0;
  tc=parCL[0];  // FT: can be exchanged with m_Tcal
// Leonids value:
//    tc = 354.;
// Drift time
//  tdr = 440.;
// New value:
   tdr = 449.7;
   
 nbino = nbin = pcal->GetNbinsX();
 if(nbin>NMAX) nbin = NMAX;
 if(nbino>NMAX) nbino = NMAX;
// pcal->Print();
 
 if(norm) {
   /*ptim =*/ normalize_prof(pcal,&pampl);
   mcut = 0.05;
 } else {
   mcut = 0.05*pcal->GetMaximum();
 } 

 i5bin = 0;
 for(i=1; i<=nbin; ++i) {
   x[i-1] = pcal->GetBinCenter(i);
   y[i-1] = pcal->GetBinContent(i);
   //FT std::cout<<"calib2phys: CALIWAVE:  x[i-1]Time="<<x[i-1]<<" y[i-1]AmplCalib="<<y[i-1]<<std::endl;
   if(i5bin == 0 && y[i-1] > mcut) i5bin = i; 
  };
 //std::cout<<std::endl;
 t5 = (mcut-y[i5bin-2])*(x[i5bin-1]-x[i5bin-2])/(y[i5bin-1]-y[i5bin-2]);
 t5 += x[i5bin-2];
//
// t0 is app. 16.8 ns less than t5 (from reference shapes 2002)
//
 t0 = t5 - 16.8;

 //FT cout<<"i5bin: "<<i5bin<<", t0 founded: "<<t0<<endl;

 if(fabs((x[1] - x[0]) - (x[3] - x[2])) > 0.000001 ) {
   cout<< x[0] << " " << x[1] << " " << x[2] << " " << x[3] << endl;
   printf("\n Nonuniform step !\n");
   return NULL;
   //return ;
 }
 
 STEP = x[1] - x[0];


 if(pref) delete [] pref;
 pref = new Double_t[nbin];

 count = 0; 
 for(i=0; i<nbin; ++i) {
   if(x[i] < t0) {pref[i] = 0.; continue;}
   double AmplPhys      = m_gIdealPhys->getSample(count);
   //double Time          = m_gIdealPhys->getTime(count);
   pref[i]         = AmplPhys;
//   std::cout<<"1..LArPhysWaveHECTOOL from POOL:  AmplPhys["<<i<<"]="<<AmplPhys<< "  Time="<<Time <<std::endl;
   ++count;
   //   if(count==799) break;
   if(count==(int)m_gIdealPhys->getSize()) break;
 } 
 if(i<nbin) nbin = i+1;

// Try to find the shift and mult. for reference signal
 if(splin) delete splin;
 splin = new TSpline3("sp",x,pref,nbin,"b1",0.);
 fu = new TF1("fu",P,x[15],x[nbin-15],2);
 fu->SetParName(0,"t0 shift");
 fu->SetParLimits(0,-15.,15.);
 fu->SetParName(1,"mult");
 fu->SetParLimits(1,0.,10000.);
 fu->SetParameters(5.,5.);
// pcal->Draw();
// fu->Draw("same");
// splin->Draw("same");
// canv->Update();
// cin>>c;
 if( pcal->Fit("fu","RQ0") == 0) {
    rshift = pcal->GetFunction("fu")->GetParameter(0);
    rmult = pcal->GetFunction("fu")->GetParameter(1);
 } else {
    rshift = 0; rmult= pcal->GetMaximum()/ (* std::max_element(m_gIdealPhys->getWave().begin(), m_gIdealPhys->getWave().end()));
 }
// cout<<rshift<<" "<<rmult<<endl;
// fu->Draw("same");
// canv->Update();
// cin>>c;
 delete fu;
// Read the reference again, shifting and multiplying:
 count = 0; 
 for(i=0; i<nbin; ++i) {
   if(x[i] < t0-rshift) {pref[i] = 0.; continue;}
   double AmplPhys      = rmult*m_gIdealPhys->getSample(count);
   //double Time          = m_gIdealPhys->getTime(count);
   pref[i]         = AmplPhys;
//   std::cout<<"1..LArPhysWaveHECTOOL from POOL:  AmplPhys["<<i<<"]="<<AmplPhys<< "  Time="<<Time <<std::endl;
   ++count;
   //   if(count==799) break;
   if(count==(int)m_gIdealPhys->getSize()) break;
 } 
 if(i<nbin) nbin = i+1;

 if(uset0){ 
   for(i=1; i<=nbin; ++i)  x[i-1] -= t0;
   t0 = 0.;
  }
 else {
   for(i=1; i<=nbin; ++i)  x[i-1] += par[0];
   t0 += par[0];
  }
 par[0] = 0;
 Smooth_new(x, y, pref, nbin);
 if(splin) delete splin;
 // splin = new TSpline3("sp",x,y,nbin,"b1",0.);
 splin = new TSpline3("sp",x,pref,nbin,"b1",0.);

 // gaus or GSL ?

 if(!gsl_flag)
   fitfun = new TF1("fitfun",&Tp,x[0],x[nbin-1],3);
 else
   fitfun = new TF1("fitfun",&Tp_gsl,x[0],x[nbin-1],3);

 fitfun->SetParName(0,"t0 shift");
 fitfun->SetParLimits(0,0.,100.);
 fitfun->SetParName(1,"mult");
 fitfun->SetParLimits(1,0.,100.);
 fitfun->SetParName(2,"Taus");
 fitfun->SetParLimits(2,0.1,10.);
 fitfun->SetNpx(nbin+1);
 fitfun->SetParameters(par[0],par[1],par[2]);
 //std::cout<<"What happens before normalize?"<<std::endl;
 if(norm) {
   if(uset0)
     /*ptim     =*/ normalize(fitfun,    &pampl,     0., 100., STEP);
   else 
     /*ptim     =*/ normalize(fitfun,     &pampl,     t0, t0+100., STEP);
 }
 // std::cout<<"What happens after normalize?"<<std::endl;
// Return also the derivative
 deriv = new TF1("derivfun",&DTp,x[0],x[nbin-1],3);
 //std::cout<<"What happens after deriv?"<<std::endl; 
 deriv->SetParName(0,"t0 shift");
 deriv->SetParLimits(0,0.,100.);
 deriv->SetParName(1,"mult");
 deriv->SetParLimits(1,0.,100.);
 deriv->SetParName(2,"Taus");
 deriv->SetParLimits(2,0.1,10.);
 deriv->SetNpx(nbin+1);
 deriv->SetParameters(par[0],par[1],par[2]);

 //std::cout<<"What happens after total deriv?"<<std::endl; 
 if(norm) {
   /*ptim =*/ normalize(deriv, &pampl, t0+par[0], t0+par[0]+100., STEP);
 }
 //std::cout<<"What happens after deriv normalize? x["<<nbin-1<<"]="<<x[nbin-1]<<std::endl; 
 //if(xmax != NULL) *xmax = x[nbin-1];
 //std::cout<<"What happens after pointer assigment?"<<std::endl; 

 //for(unsigned int i=1;i<=nbin; ++i)
 //std::cout<<"xTime="<<x[i-1]<<" yAmplCalib="<<y[i-1]<<" AmplPhys="<<fitfun->Eval(x[i-1])<<std::endl;

 return fitfun;
}


Double_t normalize(TF1 *func, Double_t *rampl,
                   Double_t from, Double_t to, Double_t step)
{
 static Double_t amax, tmax,t, ampl;
 static Double_t t1,t2,tt1,tt2;
 static Int_t i;

 if((from == to) || (from > to)) 
     func->GetRange(t1,t2);
 else {
    func->GetRange(tt1,tt2);
    if(from>tt1) t1 = from; else t1 = tt1;
    if(to<tt2) t2 = to; else t2 = tt2;
  }

 amax = 0; tmax = t1;
// cout << "Normalize: "<<t0 << " " << ampl << " " << t1 << " " << t2 << " "<<step<<endl;
 for(t=t1; t<=t2; t+=step)
  {
   if(func->Eval(t) > amax) {
     amax = func->Eval(t); tmax = t; 
//     cout<<t<<" "<<amax<<endl;
    }
   }
 Double_t x[20], y[20];
 i=0;
 for(t=tmax-5; t<tmax+5; ++t) {
    x[i] = t;
    y[i] = func->Eval(t);
    ++i;
   }
 TGraph *g = new TGraph(10,x,y);
 g->Fit("pol2","QIE0");
 tmax = - g->GetFunction("pol2")->GetParameter(1) / (2 *g->GetFunction("pol2")->GetParameter(2));
 amax = g->GetFunction("pol2")->Eval(tmax);
 
 ampl = func->GetParameter(1);
 if(amax != 0) func->SetParameter(1,ampl/amax);
   else tmax = 0;
 cout << "Normalize: tmax="<<tmax << " ampl=" << ampl<<" amax="<<amax << endl;
 if(rampl != NULL) *rampl = amax;
 g->Delete();
 return tmax;
}

Double_t normalize_prof(TProfile *histo, Double_t *rampl)
{
 Double_t amax, tmax, x, y, err;
 Int_t nbins,i, j, nent;
 amax = histo->GetBinContent(0);
 tmax = histo->GetBinCenter(0);
 nbins = histo->GetNbinsX();
 for(i=1; i<nbins; i++)
  {
   if(amax < histo->GetBinContent(i))
     {
      amax = histo->GetBinContent(i);
      tmax =  histo->GetBinCenter(i);
     }
  }

 histo->Fit("pol2","0QI","",tmax-10., tmax+10.);
 tmax = - histo->GetFunction("pol2")->GetParameter(1) / (2 * histo->GetFunction("pol2")->GetParameter(2));
 amax = histo->GetFunction("pol2")->Eval(tmax);
 TProfile *htmp = new TProfile(*histo);
 histo->Reset();
// cout<<"Norm_prof amax= "<<amax<<endl;
 for(i=0; i<=nbins; i++)
  {
   if(htmp->GetBinEntries(i) == 0) continue;
   x = htmp->GetBinCenter(i);
   y = htmp->GetBinContent(i);
   nent=int(htmp->GetBinEntries(i));
   err = htmp->GetBinError(i);
//   cout<<x<<" "<<y<<" "<<err<<endl;
// Hack for stupid root v.3, which do not like the one entries in profile histo
//   histo->SetBinContent(i,0.);
   for(j=0; j<7; ++j) {
      if(nent != 0) {
           histo->Fill(x,y/amax+sqrt(2)*err/nent);
           histo->Fill(x,y/amax-sqrt(2)*err/nent);
      } else {
           histo->Fill(x,y/amax+sqrt(2)*err);
           histo->Fill(x,y/amax-sqrt(2)*err);
      }
    }
  }

 if(rampl != NULL) *rampl = amax;
// htmp->GetFunction("pol2")->Delete();
 
 return tmax;
}
void SmootKern(Double_t *x, Double_t *y, Int_t ymax, Double_t band)
{
   Double_t *xx, *yy;
   TGraph *g=new TGraph(ymax,x,y);
   TGraphSmooth *gs = new TGraphSmooth();
   TGraph *g1 = gs->SmoothKern(g,"normal",band);
   xx = g1->GetX(); yy = g1->GetY();
   for(Int_t i=0; i<g->GetN(); ++i) {
      x[i] = xx[i]; y[i] = yy[i];
   }
   delete g; delete gs;
}

void Smooth_new(Double_t *x, Double_t *y, Double_t *yref, Int_t ymax)
{
// Smoothing the difference between the yref and y, 
// then y = y + smoothed differnce
// repeating few times
  static Double_t *dif = NULL;
  static Int_t i,j;
  
  if(dif) delete[] dif;
  dif = new Double_t[ymax];
  for(j=0; j<4; ++j) {
    for(i=0; i<ymax; ++i) dif[i] = y[i] - yref[i];
    SmootKern(x,dif,ymax, 10.);
    for(i=0; i<ymax; ++i) yref[i] += dif[i];
  }
}

