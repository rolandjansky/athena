/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_EMECPRESAMPLERHVMODULE_H
#define LARHV_EMECPRESAMPLERHVMODULE_H

/**
 * @class EMECPresamplerHVModule
 *
 * @brief Describes one HV Module within the EMEc Presampler
 *
 */

class EMECPresamplerHVManager;

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
class LArHVIdMapping;
#endif

class EMECPresamplerHVModule
{
 public:
  EMECPresamplerHVModule(const EMECPresamplerHVManager *manager
			 , unsigned int iSide
			 , unsigned int iPhi);
  ~EMECPresamplerHVModule();

  double getEtaMin() const;
  double getEtaMax() const;

  double getPhiMin() const;
  double getPhiMax() const;

  unsigned int getPhiIndex() const;

  // Side Index (0=Negative, 1=Positive)
  unsigned int getSideIndex() const;

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
  int hvLineNo(int iGap, const LArHVIdMapping* hvIdMapping) const;
#endif
  
  const EMECPresamplerHVManager& getManager() const;

 private:
  // Illegal operations 
  EMECPresamplerHVModule(const EMECPresamplerHVModule& right);
  EMECPresamplerHVModule& operator=(const EMECPresamplerHVModule& right);

  class Clockwork;
  Clockwork *m_c;  
};

#endif 
