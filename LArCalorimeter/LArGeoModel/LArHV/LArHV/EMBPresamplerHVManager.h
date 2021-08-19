/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_EMBPRESAMPLERHVMANAGER_H
#define LARHV_EMBPRESAMPLERHVMANAGER_H

#include "LArHV/EMBPresamplerHVModule.h"
#include "Identifier/HWIdentifier.h"
#include <memory>
#include <functional>

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
class LArHVIdMapping;
#endif

class CondAttrListCollection;
class EMBPresamplerHVDescriptor;
class EMBPresamplerHVModule;
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
  class EMBPresamplerHVData
  {
  public:
    static constexpr double INVALID = -99999;
    class Payload;
    EMBPresamplerHVData();
    EMBPresamplerHVData (std::unique_ptr<Payload> payload);
    EMBPresamplerHVData& operator= (EMBPresamplerHVData&& other);
    ~EMBPresamplerHVData();
    bool hvOn (const EMBPresamplerHVModule& module, const int& iGap) const;
    double voltage (const EMBPresamplerHVModule& module, const int& iGap) const;
    double current (const EMBPresamplerHVModule& module, const int& iGap) const;
    int  hvLineNo  (const EMBPresamplerHVModule& module, const int& iGap) const;
  private:
    int index (const EMBPresamplerHVModule& module) const;
    std::unique_ptr<Payload> m_payload;
  };

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

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
  EMBPresamplerHVData getData (const LArHVIdMapping& hvIdMapping,
                               const std::vector<const CondAttrListCollection*>& attrLists) const;
  // Get hvLine for a module
  int hvLineNo(const EMBPresamplerHVModule& module
               , int gap
               , const LArHVIdMapping* hvIdMapping) const;
#endif

 private:
  using idfunc_t = std::function<std::vector<HWIdentifier>(HWIdentifier)>;
  EMBPresamplerHVData getData (idfunc_t idfunc,
                               const std::vector<const CondAttrListCollection*>& attrLists) const;

  // Illegal operations
  EMBPresamplerHVManager(const EMBPresamplerHVManager& right) = delete;
  EMBPresamplerHVManager& operator=(const EMBPresamplerHVManager& right) = delete;
  
  friend class ImaginaryFriend;
  class Clockwork;
  std::unique_ptr<const Clockwork> m_c;
};

#endif
