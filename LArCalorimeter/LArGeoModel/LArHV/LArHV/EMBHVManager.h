/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_EMBHVMANAGER_H
#define LARHV_EMBHVMANAGER_H

#include "LArHV/EMBHVModule.h"
#include "LArHV/EMBHVDescriptor.h"

#ifndef SIMULATIONBASE
class LArHVIdMapping;
#endif

struct EMBHVPayload;

/**
 * @class EMBHVManager
 *
 * @brief  This class provides direct access to information on the HV 
 * electrodes within the barrels.  The information may be accessed either
 * directly or iteratively.  Direct access is provided by the getHVModule()
 * method.  Iterative access
 * is by looping over valid side, eta, phi, and sector indices to
 * retrieve a HV module.  From the high voltage modules one
 * can obtain a list of electrodes (iteratively or directly).
 *
 * The manager owns the pointers to the HV Modules.
 */ 

class EMBHVManager
{
 public:
  EMBHVManager();
  ~EMBHVManager();

  // Gets the descriptor.  (Not generally for users but nothing to hide here).
  const EMBHVDescriptor& getDescriptor() const;

  unsigned int beginPhiIndex() const;
  unsigned int endPhiIndex() const;

  unsigned int beginEtaIndex() const;
  unsigned int endEtaIndex() const;

  unsigned int beginSectorIndex() const;
  unsigned int endSectorIndex() const;
  
  // Returns a high voltage module
  const EMBHVModule& getHVModule(unsigned int iSide
				 , unsigned int iEta
				 , unsigned int iPhi
				 , unsigned int iSector) const;
  
  // Begin/End side index (0=negative and 1= positive)
  unsigned int beginSideIndex() const;
  unsigned int endSideIndex() const;

  // Refresh from the database if needed
  void update() const;
  
  // Make the data stale.  Force update of data.
  void reset() const;
  
  // Get the database payload
  EMBHVPayload *getPayload(const EMBHVElectrode &) const;
  
#ifndef SIMULATIONBASE
  // Get hvLine for an electrode
  int hvLineNo(const EMBHVElectrode& electrode
	       , int gap
	       , const LArHVIdMapping* hvIdMapping) const;
#endif

 private:
  EMBHVManager(const EMBHVManager& right);
  EMBHVManager& operator=(const EMBHVManager& right);
  
  friend class ImaginaryFriend;
  class Clockwork;
  Clockwork *m_c;
};

#endif 
