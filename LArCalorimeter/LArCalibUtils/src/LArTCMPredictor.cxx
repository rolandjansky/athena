/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArTCMPredictor.h" 
#include <cmath>
//#include "LArCalibUtils/TMinuit.h"


LArTCMPredictor::LArTCMPredictor ()
{
	m_caliStart=-999.;
	m_physShift=0.; //not used so far
	
	//Characteristic start values
	// need to declareproperty
	m_taud = 450.;
	m_f = 0.09;
	m_tauexp = 365.;
	m_taur=11.;
	m_w0 = 0.184;
	m_alpha=0.025;
}

// destructor 
LArTCMPredictor::~LArTCMPredictor()
{
}


// implement eval pour double x et interpolation

double LArTCMPredictor::evalLArCaliWave(int x) {
	return m_gCali.getSample(x); // use shifted LArCaliWave
}


double LArTCMPredictor::Integrand(int t, int tau) {
	return evalLArCaliWave(tau)*Gt3(t-tau);	//need to choose between model
}

double LArTCMPredictor::qfast(int a, int b) {
	double sum=0.;
	for(int k=a; k<=b; ++k) {  //basic computation of the convolution
		sum+=Integrand(b,k);	
	}
	return sum;
}

///////////////////////////////////////////////////////////////////////////////
LArPhysWave LArTCMPredictor::predictLArPhysWave(const LArCaliWave &larCaliWave)
{
	m_gCali = larCaliWave;
	
	LArWave gCali(m_gCali), gPhysPred, gPhysPredShifted;
	
	if (m_caliStart==-999.) { // if m_caliStart not initialized by LArTCMFitter 
		m_caliStart = (double)getStart(m_gCali)-2.; //
		if (m_caliStart<0.) m_caliStart=0.; // for low dac the beginning of the pulse is not always well defined
	}
	
	gCali = translate(gCali,(int)m_caliStart); // shift of gCali
	m_gCali = LArCaliWave(gCali.getWave(),m_gCali.getDt(),m_gCali.getDAC()); //create shifted LArCaliWave
	
	gPhysPred.setSize(larCaliWave.getSize());
	for(unsigned int i=0; i<m_gCali.getSize();++i) {
			gPhysPred.setSample(i,qfast(0,0+i)); //choose between method
	}
	
	//Translation of m_physShift
	gPhysPredShifted.setSize(larCaliWave.getSize());
	for(unsigned int i=0; i<gPhysPredShifted.getSize();++i) {
		if((double)i>m_physShift) {
			gPhysPredShifted.setSample(i,linearInterpolate(gPhysPred,(double)i-m_physShift));
		} else gPhysPredShifted.setSample(i,0.);
	}
	
	//temporary
	
	m_gPhysPred = LArPhysWave(gPhysPredShifted.getWave(),m_gCali.getDt());
	return m_gPhysPred;
}

// implement translate avec double nshift et interpolation


double LArTCMPredictor::linearInterpolate(const LArWave& larWave, double t)
{
	int bin = (int)t;
	double x = t - (double)bin;
	return (larWave.getSample(bin+1)-larWave.getSample(bin))*x+larWave.getSample(bin);
}	

LArWave LArTCMPredictor::translate(const LArWave& larWave, int nshift)
{
	const unsigned int N = larWave.getSize();
	std::vector<double> trWave;
	trWave.resize(N);
	for(unsigned int i=0; i<N; ++i) {
		if(i<(N-nshift)) trWave[i]=larWave.getSample(i+nshift);
		else trWave[i]=0.;
	}
	return LArWave(trWave,larWave.getDt());
	 	
}

unsigned int LArTCMPredictor::getindMax(const LArWave &larWave)
{
	const unsigned int N = larWave.getSize();
	unsigned int indmax=0;
	for (unsigned int i=0; i<N; ++i) {
		if(larWave.getSample(i)>larWave.getSample(indmax)) indmax=i;	
		//if(larWave[i]>larWave[indmax]) indmax=i; //do not work...
	}
	//std::cout<<"indmax"<<indmax<<std::endl;
	return indmax;	
}

/*
template <class T> unsigned int LArTCMPredictor::getMax(const T &PULSE)
{
	LArWave larWave(PULSE.getWave(),PULSE.getDt());
	return PULSE.getSample(getindMax(larWave));
}
*/

double LArTCMPredictor::getMax(const LArCaliWave &larCaliWave)
{
	LArWave larWave(larCaliWave.getWave(),larCaliWave.getDt());
	return larCaliWave.getSample(getindMax(larWave));
}

double LArTCMPredictor::getMax(const LArPhysWave &larPhysWave)
{
	LArWave larWave(larPhysWave.getWave(),larPhysWave.getDt());
	return larPhysWave.getSample(getindMax(larWave));
}


unsigned int LArTCMPredictor::getStart(const LArCaliWave &larCaliWave) 
{
	LArWave larWave(larCaliWave.getWave(),larCaliWave.getDt());
	return getStart(larWave);
}


unsigned int LArTCMPredictor::getStart(const LArWave &larWave) 
{
	const unsigned int N = larWave.getSize();
	double max=larWave.getSample(getindMax(larWave));
	double treshold = max*0.01;  //need to implement member/propertie for treshold
	unsigned int k=0;
	do k++; while((larWave.getSample(k)<treshold)&&(k<N));
	return k-1;	
}

////////////////////////////////////////
double LArTCMPredictor::Gt2(double t) const
// Function from Fares Djema - model without dirac nor rC
// Should not be used
{
double term1, termexp, termcos, termsin;
double w0_2 = m_w0*m_w0;

term1 = -1./(m_f*m_taud);
termexp = (-m_f*m_tauexp*w0_2+m_tauexp*w0_2-(m_w0*m_tauexp)*(m_w0*m_tauexp)/m_taud +(m_w0*m_tauexp)*(m_w0*m_tauexp)/(m_taud*m_f))*exp(-m_f*t/m_tauexp);
termcos = (m_f*m_tauexp*w0_2-m_tauexp*w0_2+(m_w0*m_tauexp)*(m_w0*m_tauexp)/m_taud +m_f/m_taud)*cos(m_w0*t);
termsin = (w0_2*m_w0*m_tauexp*m_tauexp+m_w0-m_f*m_w0*m_tauexp/m_taud+m_tauexp*m_w0/m_taud)*sin(m_w0*t);

return term1 + (termexp+termcos+termsin)/(m_f*m_f+w0_2*m_tauexp*m_tauexp);
//return -1;
}

////////////////////////////////////////
double LArTCMPredictor::Gt3(double t) const
// modele sans dirac avec rC
//
{

double term1, term2;

double w0_2 = m_w0*m_w0;
double taur_2 = m_taur*m_taur;
double tauexp_2 = m_tauexp*m_tauexp;
double f_2 = m_f*m_f;

double Sqd = sqrt(fabs(-4+taur_2*w0_2));
double f_t_tauexp=(m_f*t)/m_tauexp;
double tauexp_taur_w0_2 = m_tauexp*m_taur*w0_2;
double t_w0_Sqd = (t*m_w0*Sqd)/2.;
double t_taur_w0_2 = (t*m_taur*w0_2)/2.;
double exp_f_t_tauexp = exp(f_t_tauexp);

if ((-4+taur_2*w0_2)>0.) {
   term1 = -(exp(-(f_t_tauexp) - t_taur_w0_2)*
       (exp(t_taur_w0_2)*Sqd*
          ((-1 + m_f)*m_tauexp*(m_f*m_taud + m_tauexp)*w0_2 + 
            exp_f_t_tauexp*(f_2 + tauexp_2*w0_2 - m_f*tauexp_taur_w0_2)) + 
         exp_f_t_tauexp*m_f*Sqd*
          (m_tauexp*(m_taud - m_tauexp + m_taur)*w0_2 - m_f*(1 + m_taud*m_tauexp*w0_2))*
          cosh(t_w0_Sqd) + 
         exp_f_t_tauexp*m_f*m_w0*(m_tauexp*(-2 - tauexp_taur_w0_2 + taur_2*w0_2 + 
               m_taud*(-2*m_tauexp + m_taur)*w0_2) + m_f*(2*m_tauexp - m_taur + m_taud*(-2 + tauexp_taur_w0_2)))*
          sinh(t_w0_Sqd)));
    term2 = (m_f*m_taud*(f_2 + tauexp_2*w0_2 - m_f*tauexp_taur_w0_2)*Sqd);
	
}
else {
   term1 = -(exp(-(f_t_tauexp) - t_taur_w0_2)*
       (exp(t_taur_w0_2)*Sqd*
          ((-1 + m_f)*m_tauexp*(m_f*m_taud + m_tauexp)*w0_2 + 
            exp_f_t_tauexp*(f_2 + tauexp_2*w0_2 - m_f*tauexp_taur_w0_2)) + 
         exp_f_t_tauexp*m_f*Sqd*
          (m_tauexp*(m_taud - m_tauexp + m_taur)*w0_2 - m_f*(1 + m_taud*m_tauexp*w0_2))*
          cos(t_w0_Sqd) + 
         exp_f_t_tauexp*m_f*m_w0*(m_tauexp*(-2 - tauexp_taur_w0_2 + taur_2*w0_2 + 
               m_taud*(-2*m_tauexp + m_taur)*w0_2) + m_f*(2*m_tauexp - m_taur + m_taud*(-2 + tauexp_taur_w0_2)))*
          sin(t_w0_Sqd)));
    term2 = (m_f*m_taud*(f_2 + tauexp_2*w0_2 - m_f*tauexp_taur_w0_2)*Sqd);
	
}
 if (term2==0) {std::cout<<"Gt3::term2 zero divide"<<std::endl;term2=1e-9;}
return term1/term2;

}
