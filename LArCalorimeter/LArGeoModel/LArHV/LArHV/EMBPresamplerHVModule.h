/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_EMBPRESAMPLERHVMODULE_H
#define LARHV_EMBPRESAMPLERHVMODULE_H

/**
 * @class EMBPresamplerHVModule
 *
 * @brief Describes one HV Module within the EMB Presampler
 *
 */

class EMBPresamplerHVManager;

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
class LArHVIdMapping;
#endif

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

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
  int hvLineNo(int iGap, const LArHVIdMapping* hvIdMapping) const;
#endif

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
