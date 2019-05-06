/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_HECHVMODULE_H
#define LARHV_HECHVMODULE_H

class HECHVManager;
class HECHVSubgap;

/**
 * @class HECHVModule
 *
 * @brief Describes one HV Module within the HEC
 * 
 * It owns the pointers to HV Subgaps
 */


class HECHVModule
{
 public:
  HECHVModule(const HECHVManager *manager
	      , unsigned int iSide
	      , unsigned int iPhi
	      , unsigned int iSampling);
  ~HECHVModule();

  // Side Index (0=Negative, 1=Positive)
  unsigned int getSideIndex() const;

  unsigned int getSamplingIndex() const;
  unsigned int getPhiIndex() const;

  unsigned int getNumSubgaps() const;
  const HECHVSubgap& getSubgap(unsigned int iElectrode) const;
  
  double getPhiMin() const;
  double getPhiMax() const;

  const HECHVManager& getManager() const;

 private:
  HECHVModule(const HECHVModule& right);
  HECHVModule& operator=(const HECHVModule& right);

  class Clockwork;
  Clockwork *m_c;
};

#endif
