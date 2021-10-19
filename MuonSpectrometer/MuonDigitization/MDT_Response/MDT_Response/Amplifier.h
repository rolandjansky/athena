/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDT_RESPONSE_AMPLIFIER_H
#define MDT_RESPONSE_AMPLIFIER_H

#include <vector>
#include <iostream>

class Amplifier {
 private:
  // define internal data format
  typedef std::vector<double>                 cluster_vec;
  typedef std::vector<double>::iterator       cluster_vec_it; 
  typedef std::vector<double>::const_iterator cluster_vec_const_it; 

 public:
  Amplifier();
  ~Amplifier();

  // returne t_threshold
  double TThreshold() const;

  // returns time first electron arrived
  double TFirst() const;

  // returns amplifier response if the amplifier has signal else -1. 
  double AdcResponse() const; 

  // returns integrated charge before wilkinson adc if the amplifier has signal
  double Charge() const;

  bool   PassedThreshold() const;          // check if threshold was passed
  void   Reset();                          // reset amplifier signal

  // Set routines
  void SetTriggerElectron(double el);      // set threshold in fractions of triggerelectrons
  void SetIntegrationWindow(double win);   // set size integration windos
  
  // initialize response and signal, needs to be done before using amplifier!
  void   InitResponse(unsigned int bins, double binsize); 

  bool   AddClusters(const cluster_vec& clusters);  // add up response functions for a set of clusters

 private:
  void   InitAmplifierParameters();                       // default initialization of amplifier settings
  double ResponseFunction(double time);

  void   DoAdcResponse();                    // calculate adc response
  double Integral();                         // charge integral
  void   AddCluster(int bin,double charge);  

  double m_binsize = 0.0;                          // binsize in ns
  double m_triggerElectron = 0.0;                  // trigger electron 
  double m_threshold = 0.0;                        // threshold 
  int    m_integrationWindow;                // integration window in bins

  // maximum of the single electron response
  double m_responseMax = 0.0;

  cluster_vec             m_response;
  cluster_vec             m_signal;
  double   m_integral;     
  double   m_adcResponse;

  // iterators
  cluster_vec_it          m_signal_th;       // iterator pointing to the first bin over threshold
  cluster_vec_it          m_signal_stop;     
  cluster_vec_const_it    m_cluster_stop;    
  cluster_vec_const_it    m_cluster_begin;

  /* non linear response: 
     R = m_adcOffset + m_adcFactor*(log(Q)/log(m_adcFraction)-1) */
  double m_adcOffset = 0.0;
  double m_adcFactor = 0.0;
  double m_adcFraction = 0.0;
  
};

inline double Amplifier::TThreshold() const { 
  return m_binsize*(m_signal_th-m_signal.begin()); 
}
inline double Amplifier::TFirst()     const { 
  cluster_vec_const_it cit = m_signal.begin();
  while( *cit == 0 )
    ++cit;
  return m_binsize*(cit-m_signal.begin()); ;
}
inline double Amplifier::AdcResponse() const { 
  return m_adcResponse; 
}
inline double Amplifier::Charge() const {
  return m_integral; 
}

inline void  Amplifier::SetTriggerElectron(double el) {
  m_triggerElectron = el;
  m_threshold = el*m_responseMax; 
  //std::cout << "Amplifier: Changing threshold: now triggering on " << el << "th electron "	    << "new threshold " << m_threshold << std::endl;
}
inline void  Amplifier::SetIntegrationWindow(double win) { 
  m_integrationWindow = (int)(win/m_binsize)-1; 
}
inline bool  Amplifier::PassedThreshold() const { 
  return m_signal.begin() != m_signal_th; 
}

#endif
 
