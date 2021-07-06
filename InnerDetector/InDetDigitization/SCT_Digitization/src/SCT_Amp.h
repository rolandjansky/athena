// -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 *  Header file for SCT_Amp Class  
 *  @brief A class to model an SCT amplifier and shaper. Gives a CRRC response to a 
 *  list of charges with times. Also calculates average input and output for 
 *  diagnostic purposes. Questions/comments to Szymon.Gadomski@cern.ch
 *  Name changed from SCTpreamp (misleading) on 09.05.01. 
 *  @author Szymon.Gadomski@cern.ch, Awatif.Belymam@cern.ch, Davide.Costanzo@cern.ch,
 *  tgcornel@nikhef.nl, Grant.Gorfine@cern.ch, Paul.Bell@cern.ch, Jorgen.Dalmau@cern.ch, 
 * 23/08/2007 - Kondo.Gnanvo@cern.ch 
 *            - Conversion of the SCT_Amp code AlgTool 
 */

#ifndef SCT_DIGITIZATION_SCTAMP_H
#define SCT_DIGITIZATION_SCTAMP_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "SiDigitization/IAmplifier.h"

#include "InDetSimEvent/SiCharge.h"

class SCT_Amp : public extends<AthAlgTool, IAmplifier> {
 public:

  /**  constructor */
  SCT_Amp(const std::string& type, const std::string& name, const IInterface* parent);
  /** Destructor */
  virtual ~SCT_Amp() = default;
  /** AlgTool initialize */
  virtual StatusCode initialize() override;
  /** AlgTool finalize */
  virtual StatusCode finalize() override;
   
  /** main purpose: CR-RC^3 response to a list of charges with times */
  virtual float response(const list_t& Charges, const float timeOverThreshold) const override;
  virtual void response(const list_t& Charges, const float timeOverThreshold, std::vector<float>& resp) const override;

  /** Neighbour strip cross talk response strip to a list of charges with times */
  virtual float crosstalk(const list_t& Charges, const float timeOverThreshold) const override;
  virtual void crosstalk(const list_t& Charges, const float timeOverThreshold, std::vector<float>& resp) const override;

private:

  /** signal peak time */   
  FloatProperty m_PeakTime{this, "PeakTime", 21., "Front End Electronics peaking time"};

  /** Cross factor 2 side */
  FloatProperty m_CrossFactor2sides{this, "CrossFactor2sides", 0.1, "Loss of charge to neighbour strip constant"};

  /** cross factor */
  FloatProperty m_CrossFactorBack{this, "CrossFactorBack", 0.07, "Loss of charge to back plane constant"};

  FloatProperty m_tmin{this, "Tmin", -25.0};
  FloatProperty m_tmax{this, "Tmax", 150.0};
  FloatProperty m_dt{this, "deltaT", 1.0};

  /** Normalisation factor for the signal response */
  float m_NormConstCentral{0.};

  /** Normalisation factor for the neighbour strip signal response */
  float m_NormConstNeigh{0.};
};

#endif // SCT_DIGITIZATION_SCTAMP_H
