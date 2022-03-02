/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_Amp.h"

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

//STD includes
#include <cmath>
#include <fstream>

//#define SCT_DIG_DEBUG

// constructor
SCT_Amp::SCT_Amp(const std::string& type, const std::string& name, const IInterface* parent) 
  : base_class(type, name, parent)
{
}

//----------------------------------------------------------------------
// Initialize
//----------------------------------------------------------------------
StatusCode SCT_Amp::initialize() {
  
  StatusCode sc{AthAlgTool::initialize()};
  if (sc.isFailure()) {
    ATH_MSG_FATAL("SCT_Amp::initialize() failed");
    return sc;
  }
  ATH_MSG_DEBUG("SCT_Amp::initialize()");

  /** CHLEP Units */
  m_PeakTime.setValue(m_PeakTime.value() * CLHEP::ns);
  m_dt.setValue(m_dt.value() * CLHEP::ns);
  m_tmin.setValue(m_tmin.value() * CLHEP::ns);
  m_tmax.setValue(m_tmax.value() * CLHEP::ns);

  m_NormConstCentral = (exp(3.0)/27.0)*(1.0-m_CrossFactor2sides)*(1.0-m_CrossFactorBack);
  m_NormConstNeigh = exp(3.0-sqrt(3.0))/(6*(2.0*sqrt(3.0)-3.0));
  m_NormConstNeigh *= (m_CrossFactor2sides/2.0)*(1.0-m_CrossFactorBack);

#ifdef SCT_DIG_DEBUG
  ATH_MSG_INFO("\tAmp created, PeakTime = " << m_PeakTime);
  ATH_MSG_INFO("\tResponse will be CR-RC^3 with tp = " << m_PeakTime/3.0);
  ATH_MSG_INFO("\tCoupling to both neighbours = " << m_CrossFactor2sides);
  ATH_MSG_INFO("\tCoupling to backplane = " << m_CrossFactorBack);
  ATH_MSG_INFO("\tNormalization for central " << m_NormConstCentral);
  ATH_MSG_INFO("\tNormalization for neighbour " << m_NormConstNeigh);
#endif

  return sc;
}

//----------------------------------------------------------------------
// Finalize 
//----------------------------------------------------------------------
StatusCode SCT_Amp::finalize() {
  StatusCode sc{AthAlgTool::finalize()};
  if (sc.isFailure()) {
    ATH_MSG_FATAL("SCT_Amp::finalize() failed");
    return sc;
  }
  ATH_MSG_DEBUG("SCT_Amp::finalize()");
  return sc;
}

//----------------------------------------------------------------------
// Electronique response is now CR-RC^3 of the charge diode
//----------------------------------------------------------------------
float SCT_Amp::response(const list_t& Charges, const float timeOfThreshold) const {
  float resp{0.0};
  float tp{static_cast<float>(m_PeakTime/3.0)}; // for CR-RC^3
  for (const SiCharge& charge: Charges) {
    float ch{static_cast<float>(charge.charge())};
    float tC{static_cast<float>(timeOfThreshold - charge.time())};
    if (tC > 0.0) {
      tC/=tp; //to avoid doing it four times
      resp += ch*tC*tC*tC*std::exp(-tC); //faster than pow
    }
  }
  return resp*m_NormConstCentral;
}

void SCT_Amp::response(const list_t& Charges, const float timeOfThreshold, std::vector<float>& response) const {
  short bin_max{static_cast<short>(response.size())};
  std::fill(response.begin(), response.end(), 0.0);
  float tp{static_cast<float>(m_PeakTime/3.0)}; // for CR-RC^3
  for (const SiCharge& charge: Charges) {
    float ch{static_cast<float>(charge.charge())};
    float ch_time{static_cast<float>(charge.time())};
    short bin_end{static_cast<short>(bin_max-1)};
    for (short bin{-1}; bin<bin_end; ++bin) {
      float bin_timeOfThreshold{timeOfThreshold + bin*25};//25, fix me
      float tC{bin_timeOfThreshold - ch_time};
      if (tC > 0.0) {
        tC/=tp; //to avoid doing it four times
        response[bin+1] += ch*tC*tC*tC*std::exp(-tC); //faster than pow
      }
    }
  }
  for (short bin{0}; bin<bin_max; ++bin) response[bin] = response[bin]*m_NormConstCentral;
  return;
}

//----------------------------------------------------------------------
// differenciated and scaled pulse on the neighbour strip! 
//----------------------------------------------------------------------
float SCT_Amp::crosstalk(const list_t& Charges, const float timeOfThreshold) const {
  float resp{0};
  float tp{static_cast<float>(m_PeakTime/3.0)}; // for CR-RC^3
  for (const SiCharge& charge: Charges) {
    float ch{static_cast<float>(charge.charge())};
    float tC{static_cast<float>(timeOfThreshold - charge.time())};
    if (tC > 0.0) {
      tC/=tp; //to avoid doing it four times
      resp += ch*tC*tC*std::exp(-tC)*(3.0-tC); //faster than pow
    }
  }
  return resp*m_NormConstNeigh;
}

void SCT_Amp::crosstalk(const list_t& Charges, const float timeOfThreshold, std::vector<float>& response) const {
  short bin_max{static_cast<short>(response.size())};
  std::fill(response.begin(), response.end(), 0.0);
  float tp{static_cast<float>(m_PeakTime/3.0)}; // for CR-RC^3
  for (const SiCharge& charge: Charges) {
    float ch{static_cast<float>(charge.charge())};
    float ch_time{static_cast<float>(charge.time())};
    short bin_end{static_cast<short>(bin_max-1)};
    for (short bin{-1}; bin<bin_end; ++bin) {
      float bin_timeOfThreshold{timeOfThreshold + bin*25}; // 25, fix me
      float tC{bin_timeOfThreshold - ch_time};
      if (tC > 0.0) {
        tC/=tp; //to avoid doing it four times
        response[bin+1] += ch*tC*tC*std::exp(-tC)*(3.0-tC); //faster than pow
      }
    }
  }
  for (short bin{0}; bin<bin_max; ++bin) response[bin] = response[bin]*m_NormConstNeigh;
  return;
}
