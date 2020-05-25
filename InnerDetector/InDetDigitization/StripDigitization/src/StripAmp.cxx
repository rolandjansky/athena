/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "StripAmp.h"

//STD includes
#include <cmath>
#include <fstream>

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"


// constructor
StripAmp::StripAmp(const std::string& type, const std::string& name,const IInterface* parent )
  :base_class(type, name, parent),
   m_NormConstCentral(0),
   m_NormConstNeigh(0)
{
  declareInterface< ISCT_Amp >( this );
  declareProperty("CrossFactor2sides",m_CrossFactor2sides=0.1); //! <Loss of charge to neighbour strip constant
  declareProperty("CrossFactorBack",m_CrossFactorBack=0.07);    //! <Loss of charge to back plane constant
  declareProperty("PeakTime",m_PeakTime=21);                    //! <Front End Electronics peaking time
  declareProperty("deltaT",m_dt=1.0);
  declareProperty("Tmin",m_tmin=-25.0);
  declareProperty("Tmax",m_tmax=150.0);
  declareProperty("NbAverage",m_Navr=0);
}

//----------------------------------------------------------------------
// Initialize
//----------------------------------------------------------------------
StatusCode StripAmp::initialize() {

  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "StripAmp::initialize() failed" );
    return sc ;
  }
  ATH_MSG_DEBUG ( "StripAmp::initialize()" );

  /** CHLEP Units */
  m_PeakTime *= CLHEP::ns;
  m_dt *= CLHEP::ns ;
  m_tmin *= CLHEP::ns ;
  m_tmax *= CLHEP::ns ;

  m_NormConstCentral = (exp(3.0)/27.0)*(1.0-m_CrossFactor2sides)*(1.0-m_CrossFactorBack);
  m_NormConstNeigh = exp(3.0-sqrt(3.0))/(6*(2.0*sqrt(3.0)-3.0));
  m_NormConstNeigh *= (m_CrossFactor2sides/2.0)*(1.0-m_CrossFactorBack);

  int Narr = int((m_tmax-m_tmin)/m_dt)+1 ;
  m_InAvr.resize(Narr+1); m_OutAvr.resize(Narr+1); m_SideAvr.resize(Narr+1) ;
  m_InAvr.clear(); m_OutAvr.clear(); m_SideAvr.clear();

#ifdef STRIP_DIG_DEBUG
  ATH_MSG_INFO (  "\tAmp created, PeakTime = " << m_PeakTime );
  ATH_MSG_INFO (  "\tResponse will be CR-RC^3 with tp = " << m_PeakTime/3.0 );
  ATH_MSG_INFO (  "\tCoupling to both neighbours = " << m_CrossFactor2sides );
  ATH_MSG_INFO (  "\tCoupling to backplane = " << m_CrossFactorBack );
  ATH_MSG_INFO (  "\tNormalization for central " << m_NormConstCentral );
  ATH_MSG_INFO (  "\tNormalization for neighbour " << m_NormConstNeigh  );
  ATH_MSG_INFO (  "\tAverages for " << Narr << " times prepared" );
#endif

  return sc ;
}

//----------------------------------------------------------------------
// Finalize
//----------------------------------------------------------------------
StatusCode StripAmp::finalize() {
  //  PrintAverages(std::string("tape.avpul").c_str()) ;
  StatusCode sc = AthAlgTool::finalize();
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "StripAmp::finalize() failed" );
    return sc ;
  }
  ATH_MSG_DEBUG ( "StripAmp::finalize()" );
  return sc ;
}

//----------------------------------------------------------------------
// Electronique response is now CR-RC^3 of the charge diode
//----------------------------------------------------------------------
float StripAmp::response(const list_t & Charges,const float timeOfThreshold) const {
  float resp=0.0 ;
  float tp=m_PeakTime/3.0 ; // for CR-RC^3
  list_t::const_iterator p_charge = Charges.begin();
  list_t::const_iterator p_charge_end = Charges.end();
  for(; p_charge != p_charge_end; ++p_charge) {
    float ch=p_charge->charge() ;
    float tC= timeOfThreshold - p_charge->time() ;
    // if(tC > 0.0) resp += ch*std::pow(tC/tp,3)*exp(-tC/tp) ;
    if(tC > 0.0){
      tC/=tp; //to avoid doing it four times
      resp += ch*tC*tC*tC*exp(-tC) ; //faster than pow
    }
  }
  return resp*m_NormConstCentral ;
}

void StripAmp::response(const list_t & Charges,const float timeOfThreshold, std::vector<float> &response) const {
  short bin_max = response.size();
  std::fill(response.begin(), response.end(), 0.0);
  float tp=m_PeakTime/3.0 ; // for CR-RC^3
  list_t::const_iterator p_charge = Charges.begin();
  list_t::const_iterator p_charge_end = Charges.end();
  for(;p_charge != p_charge_end; ++p_charge) {
    float ch=p_charge->charge() ;
    float ch_time = p_charge->time();
    short bin_end = bin_max-1;
    for(short bin=-1; bin<bin_end; ++bin){
      float bin_timeOfThreshold = timeOfThreshold + bin*25;//25, fix me
      float tC= bin_timeOfThreshold - ch_time;
      //if(tC > 0.0) response[bin+1] += ch*std::pow(tC/tp,3)*exp(-tC/tp) ;
      if(tC > 0.0) {
          tC/=tp; //to avoid doing it four times
          response[bin+1] += ch*tC*tC*tC*exp(-tC) ; //faster than pow
      }
    }
  }
  for(short bin=0; bin<bin_max; ++bin) response[bin] = response[bin]*m_NormConstCentral;
  return;
}

//----------------------------------------------------------------------
// differenciated and scaled pulse on the neighbour strip!
//----------------------------------------------------------------------
float StripAmp::crosstalk(const list_t & Charges,const float timeOfThreshold) const {
  float resp=0 ;
  float tp=m_PeakTime/3.0 ; // for CR-RC^3
  list_t::const_iterator p_charge = Charges.begin();
  list_t::const_iterator p_charge_end = Charges.end();
  for(; p_charge != p_charge_end; ++p_charge) {
    float ch=p_charge->charge() ;
    float tC= timeOfThreshold - p_charge->time() ;
    //if(tC > 0.0) resp += ch*std::pow(tC/tp,2)*exp(-tC/tp)*(3.0-tC/tp) ;
    if(tC > 0.0){
      tC/=tp; //to avoid doing it four times
      resp += ch*tC*tC*exp(-tC)*(3.0-tC) ; //faster than pow
    }
  }
  return resp*m_NormConstNeigh ;
}

void StripAmp::crosstalk(const list_t & Charges,const float timeOfThreshold, std::vector<float> &response) const {
  short bin_max = response.size();
  std::fill(response.begin(), response.end(), 0.0);
  float tp=m_PeakTime/3.0 ; // for CR-RC^3
  list_t::const_iterator p_charge = Charges.begin();
  list_t::const_iterator p_charge_end = Charges.end();
  for(; p_charge != p_charge_end; ++p_charge) {
    float ch=p_charge->charge();
    float ch_time = p_charge->time();
    short bin_end = bin_max-1;
    for(short bin=-1; bin<bin_end; ++bin){
      float bin_timeOfThreshold = timeOfThreshold + bin*25;// 25, fix me
      float tC= bin_timeOfThreshold - ch_time;
      //if(tC > 0.0) response[bin+1] += ch*std::pow(tC/tp,2)*exp(-tC/tp)*(3.0-tC/tp);
      if(tC > 0.0)
        {
          tC/=tp; //to avoid doing it four times
          response[bin+1] += ch*tC*tC*exp(-tC)*(3.0-tC) ; //faster than pow
        }
    }
  }
  for(short bin=0; bin<bin_max; ++bin) response[bin] = response[bin]*m_NormConstNeigh;
  return;
}
//----------------------------------------------------------------------
// diagnostics
//----------------------------------------------------------------------
void StripAmp::AccumulateAverages(const list_t & Charges) {

  if(m_Navr>=0) {
    int Narr = int((m_tmax-m_tmin)/m_dt)+1 ;
    // input charge integrated
    list_t::const_iterator p_charge = Charges.begin();
    list_t::const_iterator p_charge_end = Charges.end();
    for(; p_charge != p_charge_end; ++p_charge) {
      float ch=p_charge->charge() ;
      float tC = p_charge->time() ;
      int indx = int((tC - m_tmin)/m_dt)+1 ;
      for(int i=indx; i<=Narr; ++i) {
        m_InAvr[i] += ch ;
      }
    }
    // output and side signals
    for(float tr=m_tmin; tr<=m_tmax; tr+=m_dt) {
      int i = int((tr - m_tmin)/m_dt)+1 ;
      m_OutAvr[i] += this->response(Charges,tr) ;
      m_SideAvr[i] += this->crosstalk(Charges,tr) ;
    }
    m_Navr++ ;
  }
}

//----------------------------------------------------------------------
// Printing on the screen
//----------------------------------------------------------------------
void StripAmp::PrintAverages() const {
  ATH_MSG_INFO (  "Averages after " << m_Navr );
  if(m_Navr>0) {
    int Narr = int((m_tmax-m_tmin)/m_dt)+1 ;
    for(int i=0;i<=Narr;++i) {
      ATH_MSG_INFO (  m_tmin+m_dt*i << " " );
      ATH_MSG_INFO (  m_InAvr[i]/m_Navr << " " );
      ATH_MSG_INFO (  m_OutAvr[i]/m_Navr << " " );
      ATH_MSG_INFO (  m_SideAvr[i]/m_Navr  ) ;
    }
  }
}

//----------------------------------------------------------------------
// Printing and saving in a file
//----------------------------------------------------------------------
void StripAmp::PrintAverages(const char *fname) const {
  ATH_MSG_INFO (  "Averages after " << m_Navr <<  " to " << fname );
  std::ofstream avrfile(fname) ;
  if(m_Navr>0) {
    int Narr = int((m_tmax-m_tmin)/m_dt)+1 ;
    for(int i=0;i<=Narr;++i) {
      avrfile << m_tmin+m_dt*i << " " ;
      avrfile << m_InAvr[i]/m_Navr << " " ;
      avrfile << m_OutAvr[i]/m_Navr << " " ;
      avrfile << m_SideAvr[i]/m_Navr << std::endl ;
    }
  }
  avrfile.close() ;
}
