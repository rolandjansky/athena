/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_EMECHVMANAGER_H
#define LARHV_EMECHVMANAGER_H

#include "LArHV/EMECHVModule.h"
#include "LArHV/EMECHVDescriptor.h"

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
class LArHVIdMapping;
#endif 

struct EMECHVPayload;

/**
 * @class EMECHVManager
 *
 * @brief  This class provides direct access to information on the HV 
 * electrodes within the EMEC.  The information may be accessed either
 * directly or iteratively.  Direct access is provided by the getHVModule()
 * method.  Iterative access
 * is by looping over valid side, eta, phi, and sector indices to
 * retrieve a HV module.  From the high voltage modules one
 * can obtain a list of electrodes (iteratively or directly).
 *
 * The manager owns the pointers to the HV Modules.
 */ 

class EMECHVManager
{
 public:
  typedef EMECHVModule::IOType IOType;

  EMECHVManager(IOType wheel);
  ~EMECHVManager();

  const EMECHVDescriptor& getDescriptor() const;

  unsigned int beginPhiIndex() const;
  unsigned int endPhiIndex() const;

  unsigned int beginEtaIndex() const;
  unsigned int endEtaIndex() const;

  const EMECHVModule& getHVModule(unsigned int iSide
				  , unsigned int iEta
				  , unsigned int iPhi
				  , unsigned int iSector) const;

  // Begin/End side index (0=negative and 1= positive)
  unsigned int beginSideIndex() const;
  unsigned int endSideIndex() const;

  unsigned int beginSectorIndex() const;
  unsigned int endSectorIndex() const;

  // Gets the Wheel, 0 for the Outer Wheel HV Manager and 1 for the inner Wheel HV Manager
  EMECHVManager::IOType getWheelIndex() const;

  // Refresh from the database if needed
  void update() const;
  
  // Make the data stale.  Force update of data.
  void reset() const;

  // Get the database payload
  EMECHVPayload *getPayload(const EMECHVElectrode &) const;

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
  // Get hvLine for an electrode
  int hvLineNo(const EMECHVElectrode& electrode
	       , int gap
	       , const LArHVIdMapping* hvIdMapping) const;
#endif

 private:
  EMECHVManager& operator=(const EMECHVManager& right);
  EMECHVManager(const EMECHVManager& right);

  class Clockwork;
  Clockwork *m_c;
};

#endif 
