/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_EMBPRESAMPLERHVMANAGER_H
#define LARHV_EMBPRESAMPLERHVMANAGER_H

#include "LArHV/EMBPresamplerHVModule.h"

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
class LArHVIdMapping;
#endif

class EMBPresamplerHVDescriptor;
struct EMBPresamplerHVPayload;

/**
 * @class EMBPresamplerHVManager
 *
 * @brief This class provides direct access to information on the HV 
 * electrodes within the barrels.  The information may be accessed either
 * directly or iteratively.  Direct access is provided by the getHVModule()
 * method.  Iterative access
 * is by looping over valid side, eta, phi, and sector indices to
 * retrieve a HV module.  From the high voltage modules one
 * can obtain a list of electrodes (iteratively or directly).
 *
 * The manager owns the pointers to the HV Modules.
 */

class EMBPresamplerHVManager
{
 public:
  EMBPresamplerHVManager();
  ~EMBPresamplerHVManager();

  const EMBPresamplerHVDescriptor *getDescriptor() const;

  unsigned int beginPhiIndex() const;
  unsigned int endPhiIndex() const;

  unsigned int beginEtaIndex() const;
  unsigned int endEtaIndex() const;

  // Returns a high voltage module
  const EMBPresamplerHVModule& getHVModule(unsigned int iSide
					   , unsigned int iEta
					   , unsigned int iPhi) const;

  // Begin/end side index (0=negative and 1= positive)
  unsigned int beginSideIndex() const;
  unsigned int endSideIndex() const;

  // Refresh from the database if needed
  void update() const;

  // Make the data stale.  Force update of data.
  void reset() const;

  // Get the database payload
  EMBPresamplerHVPayload *getPayload(const EMBPresamplerHVModule &) const;

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
  // Get hvLine for a module
  int hvLineNo(const EMBPresamplerHVModule& module
               , int gap
               , const LArHVIdMapping* hvIdMapping) const;
#endif

 private:
  // Illegal operations
  EMBPresamplerHVManager(const EMBPresamplerHVManager& right);
  EMBPresamplerHVManager& operator=(const EMBPresamplerHVManager& right);
  
  friend class ImaginaryFriend;
  class Clockwork;
  Clockwork *m_c;
};

#endif
