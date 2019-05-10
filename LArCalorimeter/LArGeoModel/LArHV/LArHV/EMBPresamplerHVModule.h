/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_EMBPRESAMPLERHVMODULE_H
#define LARHV_EMBPRESAMPLERHVMODULE_H

class EMBPresamplerHVManager;

/**
 * @class EMBPresamplerHVModule
 *
 * @brief Describes one HV Module within the EMB Presampler
 *
 */

class EMBPresamplerHVModule
{
 public:
  EMBPresamplerHVModule(const EMBPresamplerHVManager *manager
			, unsigned int iSide
			, unsigned int iEta
			, unsigned int iPhi);
 ~EMBPresamplerHVModule();

  unsigned int getEtaIndex() const;
  unsigned int getPhiIndex() const;

  // Side Index (0=Negative, 1=Positive)
  unsigned int getSideIndex() const;

  double getEtaMin() const;
  double getEtaMax() const;
  double getPhiMin() const;
  double getPhiMax() const;

  // HV Status
  bool hvOn(int iGap) const;
  
  double voltage(int iGap) const;
  double current(int iGap) const;

  // Voltage and current at the same time...
  void voltage_current(int iGap, double& v, double& i) const;
  
  int hvLineNo(int iGap) const;

  const EMBPresamplerHVManager& getManager() const;
  
 private:
  // Illegal operations
  EMBPresamplerHVModule& operator=(const EMBPresamplerHVModule& right);
  EMBPresamplerHVModule(const EMBPresamplerHVModule& right);

  friend class ImaginaryFriend;  
  class Clockwork;
  Clockwork *m_c;
};

#endif 
