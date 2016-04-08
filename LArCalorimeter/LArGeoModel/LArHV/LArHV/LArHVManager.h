/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHVMANAGER_H_HEADER_INCLUDED_E73E3A24
#define LARHVMANAGER_H_HEADER_INCLUDED_E73E3A24
class EMBHVManager;
class EMECHVManager;
class HECHVManager;
class FCALHVManager;
class EMBPresamplerHVManager;
class EMECPresamplerHVManager;

#include "LArHV/EMECHVManager.h"

#include "IOVSvc/IOVSvc.h"
#include "StoreGate/DataHandle.h"
 
// This class provides access to the High Voltage throughout the LAr. 
// High voltage conditions can also be accessed through the readout
// Geometry, if appropriate.


class LArHVManager
{
  public:

  
    typedef EMECHVManager::IOType IOType;
  
    // Constructor
    // 
    LArHVManager(const EMBHVManager  *embHv, const EMECHVManager *emecHvInner, const EMECHVManager *emecHvOuter, const HECHVManager *hecHv, const FCALHVManager *fcalHv, const EMBPresamplerHVManager *embPreManager, const EMECPresamplerHVManager *emecPreManager);
    
    //  Destructor
    virtual ~LArHVManager();
    
    // Returns the EMBHVManager
    const EMBHVManager *getEMBHVManager() const;

    // Returns the Barrel Presampler Manager:
    const EMBPresamplerHVManager *getEMBPresamplerHVManager() const;
  
    // Returns the EMECHVManager
    const EMECHVManager *getEMECHVManager(IOType IO) const;
  
    // Returns the Endcap Presampler Manager:
    const EMECPresamplerHVManager *getEMECPresamplerHVManager() const;
  
    // Returns the HECHVManager
    const HECHVManager *getHECHVManager() const;
  
    // Returns the FCALHVManager
    const FCALHVManager *getFCALHVManager() const;
    
    void reset() const;
    
 protected:

    LArHVManager& operator=(const LArHVManager& right);
    
 private:

  LArHVManager(const LArHVManager& right);
  

  const EMBHVManager            *m_embHV;
  const EMECHVManager           *m_emecHVInner;
  const EMECHVManager           *m_emecHVOuter;
  const HECHVManager            *m_hecHV;
  const FCALHVManager           *m_fcalHV;
  const EMBPresamplerHVManager  *m_embPreHV;
  const EMECPresamplerHVManager *m_emecPreHV;

};

#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(LArHVManager, 203332428, 1) 

#endif /* LARHVMANAGER_H_HEADER_INCLUDED_E73E3A24 */
