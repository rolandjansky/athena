/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_EMBHVMANAGER_H
#define LARHV_EMBHVMANAGER_H

#include "LArHV/EMBHVModule.h"
#include "LArHV/EMBHVDescriptor.h"
#include "Identifier/HWIdentifier.h"
#include <memory>
#include <functional>

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
class LArHVIdMapping;
#endif

class CondAttrListCollection;
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
  class EMBHVData
  {
  public:
    static constexpr double INVALID = -99999;
    class Payload;
    EMBHVData();
    EMBHVData (std::unique_ptr<Payload> payload);
    EMBHVData& operator= (EMBHVData&& other);
    ~EMBHVData();
    bool hvOn (const EMBHVElectrode& electrode, const int& iGap) const;
    double voltage (const EMBHVElectrode& electrode, const int& iGap) const;
    double current (const EMBHVElectrode& electrode, const int& iGap) const;
    int  hvLineNo  (const EMBHVElectrode& electrode, const int& iGap) const;
  private:
    int index (const EMBHVElectrode& electrode) const;
    std::unique_ptr<Payload> m_payload;
  };

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

  // Get the database payload
  EMBHVData getData() const;
  
#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
  EMBHVData getData (const LArHVIdMapping& hvIdMapping,
                     const std::vector<const CondAttrListCollection*>& attrLists) const;
  // Get hvLine for an electrode
  int hvLineNo(const EMBHVElectrode& electrode
	       , int gap
	       , const LArHVIdMapping* hvIdMapping) const;
#endif

 private:
  using idfunc_t = std::function<std::vector<HWIdentifier>(HWIdentifier)>;
  EMBHVData getData (idfunc_t idfunc,
                     const std::vector<const CondAttrListCollection*>& attrLists) const;

  EMBHVManager(const EMBHVManager& right) = delete;
  EMBHVManager& operator=(const EMBHVManager& right) = delete;
  
  friend class ImaginaryFriend;
  class Clockwork;
  std::unique_ptr<const Clockwork> m_c;
};

#endif 
