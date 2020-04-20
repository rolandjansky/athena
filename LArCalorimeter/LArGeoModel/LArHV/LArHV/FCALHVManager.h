/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_FCALHVMANAGER_H
#define LARHV_FCALHVMANAGER_H

#include "LArHV/FCALHVModule.h"

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
class LArHVIdMapping;
#endif

struct FCALHVPayload;

/**
 * @class FCALHVManager
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

class FCALHVManager
{
 public:
  FCALHVManager();
  ~FCALHVManager();

  // Begin/End side index (0=negative and 1= positive)
  unsigned int beginSideIndex() const;
  unsigned int endSideIndex() const;

  unsigned int beginSectorIndex(unsigned int iSampling) const;
  unsigned int endSectorIndex(unsigned int iSampling) const;

  unsigned int beginSamplingIndex() const;
  unsigned int endSamplingIndex() const;

  const FCALHVModule& getHVModule(unsigned int iSide
				  , unsigned int iSector
				  , unsigned int iSampling) const;

  // Refresh from the database if needed
  void update() const;
  
  // Make the data stale.  Force update of data.
  void reset() const;
  
  // Get the database payload
  FCALHVPayload *getPayload(const FCALHVLine &) const;

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
  // Get hvLine for a subgap
  int hvLineNo(const FCALHVLine& line
               , const LArHVIdMapping* hvIdMapping) const;
#endif

 private:
  FCALHVManager(const FCALHVManager& right);
  FCALHVManager& operator=(const FCALHVManager& right);

  class Clockwork;
  Clockwork *m_c;
};

#endif
