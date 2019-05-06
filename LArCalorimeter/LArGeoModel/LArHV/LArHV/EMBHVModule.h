/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_EMBHVMODULE_H
#define LARHV_EMBHVMODULE_H

class EMBHVManager;
class EMBHVElectrode;

/**
 * @class EMBHVModule
 *
 * @brief Describes one HV Module within the EMB
 *
 * It owns the pointers to HV Electrodes
 */

class EMBHVModule
{
 public:
  // The ownership of the EMBHVManager pointer NOT transferred to EMBHVModule
  EMBHVModule(const EMBHVManager *manager
	      , unsigned int iSide
	      , unsigned int iEta
	      , unsigned int iPhi
	      , unsigned int iSector);
  ~EMBHVModule();  

  unsigned int getEtaIndex() const;
  unsigned int getPhiIndex() const;
  unsigned int getSectorIndex() const;
  
  unsigned int getNumElectrodes() const { return 64; }
  
  // Gets an electrode
  const EMBHVElectrode& getElectrode(unsigned int iElectrode) const;

  // Side Index (0=Negative, 1=Positive)
  unsigned int getSideIndex() const;
  
  double getEtaMin() const;
  double getEtaMax() const;
  double getPhiMin() const;
  double getPhiMax() const;
  
  const EMBHVManager& getManager() const;

 private:
  
  EMBHVModule& operator=(const EMBHVModule& right);
  EMBHVModule(const EMBHVModule& right);
  
  class Clockwork;
  Clockwork *m_c;
  
  friend class ImaginaryFriend;
};

#endif
