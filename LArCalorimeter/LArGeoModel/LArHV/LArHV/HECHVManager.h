/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_HECHVMANAGER_H
#define LARHV_HECHVMANAGER_H

#include "LArHV/HECHVDescriptor.h"
#include "LArHV/HECHVModule.h"

#ifndef SIMULATIONBASE
class LArHVIdMapping;
#endif

struct HECHVPayload;

/**
 * @class HECHVManager
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

class HECHVManager
{
 public:
  HECHVManager();
  ~HECHVManager();

  const HECHVDescriptor& getDescriptor() const;

  // Begin/End side index (0=negative and 1= positive)
  unsigned int beginSideIndex() const;
  unsigned int endSideIndex() const;

  unsigned int beginPhiIndex() const;
  unsigned int endPhiIndex() const;

  unsigned int beginSamplingIndex() const;
  unsigned int endSamplingIndex() const;

  const HECHVModule& getHVModule(unsigned int iSide
				 , unsigned int iPhi
				 , unsigned int iSampling) const;

  // Refresh from the database if needed
  void update() const;

  // Make the data stale.  Force update of data.
  void reset() const;

  // Get the database payload
  HECHVPayload *getPayload(const HECHVSubgap &) const;

#ifndef SIMULATIONBASE
  // Get hvLine for a subgap
  int hvLineNo(const HECHVSubgap& subgap
               , const LArHVIdMapping* hvIdMapping) const;
#endif

 private:
  HECHVManager(const HECHVManager& right);
  HECHVManager& operator=(const HECHVManager& right);

  class Clockwork;
  Clockwork *m_c;
};

#endif
