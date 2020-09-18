/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 *  Header file for StripAmp Class
 *  @brief A class to model an SCT amplifier and shaper. Gives a CRRC response to a
 *  list of charges with times.
 */

#ifndef STRIP_DIGITIZATION_STRIPAMP_H
#define STRIP_DIGITIZATION_STRIPAMP_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_Digitization/ISCT_Amp.h"

#include "InDetSimEvent/SiCharge.h"

class StripAmp : public extends<AthAlgTool,  ISCT_Amp>{
public:

  /**  constructor */
  StripAmp(const std::string& type, const std::string& name, const IInterface* parent ) ;
  /** Destructor */
  ~StripAmp() override = default;
  /** AlgTool initialize */
  StatusCode initialize() override;
  /** AlgTool finalize */
  StatusCode finalize() override;

  /** main purpose: CR-RC^3 response to a list of charges with times */
  float response(const list_t &Charges, const float timeOverThreshold) const;
  void response(const list_t &Charges, const float timeOverThreshold, std::vector<float> &resp) const;

  /** Neighbour strip cross talk response strip to a list of charges with times */
  float crosstalk(const list_t &Charges, const float timeOverThreshold) const;
  void crosstalk(const list_t &Charges, const float timeOverThreshold, std::vector<float> &resp) const;

  /** diagnostics */
  void AccumulateAverages(const list_t &Charges);
  void PrintAverages() const;
  void PrintAverages(const char *fname) const;

private:

  /** signal peak time */
  float m_PeakTime;

  /** Cross factor 2 side */
  float m_CrossFactor2sides;

  /** cross factor */
  float m_CrossFactorBack;

  /** Normalisation factor for the signal response */
  float m_NormConstCentral ;

  /** Normalisation factor for the neighbour strip signal response */
  float m_NormConstNeigh ;

  /** diagnostics average */
  int m_Navr;
  std::vector<float> m_InAvr,m_OutAvr,m_SideAvr;

  float m_tmin, m_tmax, m_dt ;
};

#endif
