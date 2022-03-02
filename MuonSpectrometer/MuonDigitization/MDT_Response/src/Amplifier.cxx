/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MDT_Response/Amplifier.h"

#include <cmath>

Amplifier::Amplifier(): 
 m_integrationWindow(0),
 m_integral(0.),
 m_adcResponse(0.) 
{ 
  InitAmplifierParameters();
}

Amplifier::~Amplifier()
{
}

void  Amplifier::InitAmplifierParameters()
{
  m_threshold = 0.;
  m_responseMax = 0.;
  m_triggerElectron = 20.;
  m_adcOffset = 30.;
  m_adcFactor = 90.;
  m_adcFraction = 10.;
  m_binsize = 1.;
  //std::cout << "Amplifier Threshold " << m_triggerElectron << std::endl;
} 

double Amplifier::ResponseFunction(double time)
{
  /*
    amplifier f(t) = (t/tau)**3 exp(-t/tau) u(t)
    
    signal    I(t) = I0/(t+t0) u(t)
    
    u(t) step function 
    tau 15 ns, t0=12 nsec, I0=1

  */               
  //double TP=20.;
  double TP=15.;
  double T0=12./2.;
  /*
    FI1=1./3.
    FI2=2./3.
    signal = ( FI1 exp(t/t0)/t0 + FI2 exp(-t/(9*t0))/(9*t0))
    ion/electron fraction  electron delta function ion signal
  */ 
  double FI1=1./3.;
  double FI2=2./3.;
  double face=2.0; 

  /* third order amplifier

  f(t)= u(t) * t/tau**3* exp(-t/tau)/3!/tau
  I(t)= u(t) * exp(-t/t0)/ t0 

  */
  double TAU=TP/3.;
  double a=1./TAU;
  double b =1./T0;
  double a4 = a*a*a*a;
  double ab4 = a-b;
  ab4 = ab4*ab4*ab4*ab4;
  double dt = (a-b)*time;
  double dt2 = dt*dt;
  double dt3 = dt2*dt;
  // double dt4 = dt2*dt2; // not used

  double F3 = a4*time*time*time*exp(-a*time)/6.;
  double xnor = b*a4/ab4;
  double res31= xnor*(exp(-b*time) - exp(-a*time)  
		      -dt*exp(-a*time)
		      -dt2/2.*exp(-a*time)
		      -dt3/6.*exp(-a*time));

  b =1./T0/9.;
  ab4 = a-b;
  ab4 = ab4*ab4*ab4*ab4;
  dt = (a-b)*time;
  dt2 = dt*dt;
  dt3 = dt2*dt;
  // dt4 = dt2*dt2; // not used
  xnor = b*a4/ ab4;
  double res32= xnor*(exp(-b*time) - exp(-a*time)  
		      -dt*exp(-a*time)
		      -dt2/2.*exp(-a*time)
		      -dt3/6.*exp(-a*time));
  double resolution=(face*(res31*FI1+res32*FI2)+F3)/(1.+face);

  return resolution;
}

void Amplifier::InitResponse(unsigned int bins, double binsize)
{
  m_binsize = binsize;  
  m_integrationWindow =  (int)(20./binsize)-1 ;

  m_response.resize(bins);
  m_signal.resize(bins);
  //std::cout << "Amplifier Initialize: new response vector with " << m_response.size() << " bins" << std::endl;

  cluster_vec_it it = m_response.begin();
  double i(0.);
  double resp(0),max(0.);//,integral(0.);
  while( it != m_response.end() ){
    resp = m_binsize*ResponseFunction(m_binsize*i);
    if(resp>max) max = resp;
    //integral+=resp;
    *it = resp;
    ++it;++i;
  }  
  m_responseMax = max;
  m_threshold = m_triggerElectron*max;
  //std::cout << "max " << max << " integral " << integral << " threshold " << m_threshold <<std::endl;
  Reset();
}

void Amplifier::Reset()
{ 
  m_integral = -10000.;     
  std::vector<double>::iterator dit = m_signal.begin();
  for( ; dit != m_signal.end(); ++dit) 
    *dit = 0.;
  m_signal_th = m_signal.begin();
  m_signal_stop = m_signal.end();
}

bool Amplifier::AddClusters(const std::vector<double>& clusters)
{
  if(clusters.empty()){   // no clusters, no signal!
    return false;
  }

  // reset cluster iterators 
  m_cluster_begin = clusters.begin(); 
  m_cluster_stop = clusters.end();         // stop iterator can be changed in the AddCluster-routine
  cluster_vec_const_it cit = m_cluster_begin;  
  int bin(0);
  for( ; cit!=m_cluster_stop;++cit){       // iterate over all clusters
    if(*cit == 0.) continue;         // check if the cluster size in the current bin is not zero
    bin = cit - m_cluster_begin;           // calculate the corresponding bin number
    AddCluster(bin,*cit);            // add response of cluster to total detector response
  }
  Integral();                        // calculate the Q-integral
  DoAdcResponse();                   // get the ADC response

  return PassedThreshold();
}

void Amplifier::AddCluster(int startbin, double charge)
{
  // set start values for signal and response iterator
  cluster_vec_it response_it = m_response.begin();
  cluster_vec_it signal_it = m_signal.begin() + startbin;

  // fill signal vector until stop
  while(signal_it != m_signal_stop) {
    
    *signal_it += *response_it*charge;        // add response to signal
    if( *signal_it > m_threshold ) {   // check if signal above threshold
      
      // if new threshold found
      if( signal_it - m_signal_th < 0 || m_signal_th == m_signal.begin()){
	m_signal_th = signal_it;            // store new threshold

	/* if the bin (threshold + integration window) lies 
	   within the signal vector, point the stop iterator to it.
	   nessicary to make sure m_signal_stop never points beyond the m_signal.end() */

	if( m_signal.end()-signal_it > m_integrationWindow ){
	  m_signal_stop = m_signal_th + m_integrationWindow;
	  m_cluster_stop = m_cluster_begin + (m_signal_stop-m_signal.begin());
	}
      }
      ++signal_it;++response_it;             // go to next bin
      while(signal_it != m_signal_stop) {         // loop over the remaining bins
	*signal_it += *response_it*charge;
	++signal_it;++response_it;
      }
      break;                     // exit loop
    }
    ++signal_it;++response_it;    
  }
}

double Amplifier::Integral()
{
  cluster_vec_it s = m_signal_th;
  m_integral = 0.;
  while(s != m_signal_stop){         // sum charge within integration window
    m_integral += *s;
    ++s;
  }
  return m_integral;
} 

void Amplifier::DoAdcResponse()  
{
  if(m_integral > 0){
    m_adcResponse = m_adcOffset + m_adcFactor*(log(m_integral)/log(m_adcFraction)-1);
  }else{
    m_adcResponse = -1000;
  }
}

