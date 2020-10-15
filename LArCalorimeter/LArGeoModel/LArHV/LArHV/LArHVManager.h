/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_LARHVMANAGER_H
#define LARHV_LARHVMANAGER_H

#include "LArHV/EMBHVManager.h"
#include "LArHV/EMECHVManager.h"
#include "LArHV/HECHVManager.h"
#include "LArHV/FCALHVManager.h"
#include "LArHV/EMBPresamplerHVManager.h"
#include "LArHV/EMECPresamplerHVManager.h"
 
/**
 * @class LArHVManager
 *
 * @brief This class provides access to the High Voltage throughout the LAr. 
 * High voltage conditions can also be accessed through the readout
 * Geometry, if appropriate.
 */

class LArHVManager
{
 public:
  
  typedef EMECHVManager::IOType IOType;
  
  LArHVManager();
  ~LArHVManager();
    
  // Returns the EMBHVManager
  const EMBHVManager& getEMBHVManager() const;
  
  // Returns the Barrel Presampler Manager:
  const EMBPresamplerHVManager& getEMBPresamplerHVManager() const;
  
  // Returns the EMECHVManager
  const EMECHVManager& getEMECHVManager(IOType IO) const;
  
  // Returns the Endcap Presampler Manager:
  const EMECPresamplerHVManager& getEMECPresamplerHVManager() const;
  
  // Returns the HECHVManager
  const HECHVManager& getHECHVManager() const;
  
  // Returns the FCALHVManager
  const FCALHVManager& getFCALHVManager() const;
  
  
 protected:
  
  LArHVManager& operator=(const LArHVManager& right);
  
 private:

  LArHVManager(const LArHVManager& right);
  
  EMBHVManager            m_embHV;
  EMECHVManager           m_emecHVInner;
  EMECHVManager           m_emecHVOuter;
  HECHVManager            m_hecHV;
  FCALHVManager           m_fcalHV;
  EMBPresamplerHVManager  m_embPreHV;
  EMECPresamplerHVManager m_emecPreHV;
};

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(LArHVManager, 203332428, 1) 

#endif 
