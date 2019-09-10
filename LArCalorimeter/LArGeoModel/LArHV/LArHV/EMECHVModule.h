/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_EMECHVMODULE_H
#define LARHV_EMECHVMODULE_H

class EMECHVManager;
class EMECHVElectrode;

/**
 * @class EMBHVModule
 *
 * @brief Describes one HV Module within the EMEC
 *
 * It owns the pointers to HV Electrodes
 */

class EMECHVModule
{
 public:
  enum IOType { INNER = 0,  OUTER = 1};

  // The ownership of the EMECHVManager pointer NOT transferred to EMECHVModule
  EMECHVModule(const EMECHVManager *manager
	       , IOType iWheel
	       , unsigned int iSide
	       , unsigned int iEta
	       , unsigned int iPhi
	       , unsigned int iSector);

  ~EMECHVModule();

  double getEtaMin() const;
  double getEtaMax() const;

  double getPhiMin() const;
  double getPhiMax() const;

  unsigned int getEtaIndex() const;
  unsigned int getPhiIndex() const;

  unsigned int getNumElectrodes() const;
  const EMECHVElectrode& getElectrode(unsigned int iElectrode) const;

  // Side Index (0=Negative, 1=Positive)
  unsigned int getSideIndex() const;

  // Gets the Wheel, 0 for the Outer Wheel HV Manager and 1 for the inner Wheel HV Manager
  EMECHVModule::IOType getWheelIndex() const;

  unsigned int getSectorIndex() const;

  const EMECHVManager& getManager() const;
  
 private:
  EMECHVModule(const EMECHVModule& right);
  EMECHVModule& operator=(const EMECHVModule& right);
  
  class Clockwork;
  Clockwork *m_c;
};

#endif
