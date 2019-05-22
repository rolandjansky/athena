/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_EMECPRESAMPLERHVMANAGER_H
#define LARHV_EMECPRESAMPLERHVMANAGER_H

#include "LArHV/EMECPresamplerHVModule.h"

#ifndef SIMULATIONBASE
class LArHVIdMapping;
#endif

struct EMECPresamplerHVPayload;
class CellBinning;

/**
 * @class EMECPresamplerHVManager
 *
 * @brief This class provides direct access to information on the HV 
 * electrodes within the EMEC.  The information may be accessed either
 * directly or iteratively.  Direct access is provided by the getHVModule()
 * method.  Iterative access
 * is by looping over valid side, eta, phi, and sector indices to
 * retrieve a HV module.  From the high voltage modules one
 * can obtain a list of electrodes (iteratively or directly).
 *
 * The manager owns the pointers to the HV Modules.
 */

class EMECPresamplerHVManager
{
 public:
  EMECPresamplerHVManager();
  ~EMECPresamplerHVManager();
    
  const CellBinning *getPhiBinning() const;

  unsigned int beginPhiIndex() const;
  unsigned int endPhiIndex() const;

  // Begin/end side index (0=negative and 1= positive)
  unsigned int beginSideIndex() const;
  unsigned int endSideIndex() const;

  // Get a link to the HV module:
  const EMECPresamplerHVModule& getHVModule(unsigned int iSide, unsigned int iPhi) const;

  // Refresh from the database if needed
  void update() const;

  // Make the data stale.  Force update of data.
  void reset() const;

  // Get the database payload
  EMECPresamplerHVPayload *getPayload(const EMECPresamplerHVModule &) const;

#ifndef SIMULATIONBASE
  // Get hvLine for a module
  int hvLineNo(const EMECPresamplerHVModule& module
               , const LArHVIdMapping* hvIdMapping) const;
#endif

 private:
  // Illegal operations
  EMECPresamplerHVManager& operator=(const EMECPresamplerHVManager& right);
  EMECPresamplerHVManager(const EMECPresamplerHVManager& right);

  class Clockwork;
  Clockwork *m_c;
};


#endif
