/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_EMECHVMANAGER_H
#define LARHV_EMECHVMANAGER_H

#include "LArHV/EMECHVModule.h"
#include "LArHV/EMECHVDescriptor.h"
#include "Identifier/HWIdentifier.h"
#include <memory>
#include <functional>

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
class LArHVIdMapping;
#endif 

struct EMECHVPayload;
class CondAttrListCollection;

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

  class EMECHVData
  {
  public:
    static constexpr double INVALID = -99999;
    class Payload;
    EMECHVData();
    EMECHVData (std::unique_ptr<Payload> payload);
    EMECHVData& operator= (EMECHVData&& other);
    ~EMECHVData();
    bool hvOn (const EMECHVElectrode& electrode, const int& iGap) const;
    double voltage (const EMECHVElectrode& electrode, const int& iGap) const;
    double current (const EMECHVElectrode& electrode, const int& iGap) const;
    int  hvLineNo  (const EMECHVElectrode& electrode, const int& iGap) const;
  private:
    int index (const EMECHVElectrode& electrode) const;
    std::unique_ptr<Payload> m_payload;
  };

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

  // Get the database payload --- for use by simulation only
  // (doesn't account for conditions changes)
  EMECHVData getDataSim() const;

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
  EMECHVData getData (const LArHVIdMapping& hvIdMapping,
                      const std::vector<const CondAttrListCollection*>& attrLists) const;
  // Get hvLine for an electrode
  int hvLineNo(const EMECHVElectrode& electrode
	       , int gap
	       , const LArHVIdMapping* hvIdMapping) const;
#endif

 private:
  using idfunc_t = std::function<std::vector<HWIdentifier>(HWIdentifier)>;
  EMECHVData getData (idfunc_t idfunc,
                      const std::vector<const CondAttrListCollection*>& attrLists) const;

  EMECHVManager& operator=(const EMECHVManager& right) = delete;
  EMECHVManager(const EMECHVManager& right) = delete;

  class Clockwork;
  std::unique_ptr<const Clockwork> m_c;
};

#endif 
