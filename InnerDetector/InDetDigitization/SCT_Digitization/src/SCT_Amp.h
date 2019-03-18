/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "SCT_Digitization/ISCT_Amp.h"

#include "InDetSimEvent/SiCharge.h"

class SCT_Amp : public extends<AthAlgTool, ISCT_Amp> {
 public:

  /**  constructor */
  SCT_Amp(const std::string& type, const std::string& name, const IInterface* parent);
  /** Destructor */
  virtual ~SCT_Amp() = default;
  /** AlgTool initialize */
  virtual StatusCode initialize();
  /** AlgTool finalize */
  virtual StatusCode finalize();
   
  /** main purpose: CR-RC^3 response to a list of charges with times */
  float response(const list_t& Charges, const float timeOverThreshold) const;
  void response(const list_t& Charges, const float timeOverThreshold, std::vector<float>& resp) const;

  /** Neighbour strip cross talk response strip to a list of charges with times */
  float crosstalk(const list_t& Charges, const float timeOverThreshold) const;
  void crosstalk(const list_t& Charges, const float timeOverThreshold, std::vector<float>& resp) const;

private:

  /** signal peak time */   
  float m_PeakTime;

  /** Cross factor 2 side */
  float m_CrossFactor2sides;

  /** cross factor */
  float m_CrossFactorBack;

  /** Normalisation factor for the signal response */
  float m_NormConstCentral;

  /** Normalisation factor for the neighbour strip signal response */
  float m_NormConstNeigh;

  float m_tmin;
  float m_tmax;
  float m_dt;

};

#endif // SCT_DIGITIZATION_SCTAMP_H

