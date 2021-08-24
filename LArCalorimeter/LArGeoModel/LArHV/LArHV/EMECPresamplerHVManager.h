/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_EMECPRESAMPLERHVMANAGER_H
#define LARHV_EMECPRESAMPLERHVMANAGER_H

#include "LArHV/EMECPresamplerHVModule.h"
#include "Identifier/HWIdentifier.h"
#include <memory>
#include <functional>

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
class LArHVIdMapping;
#endif

class CondAttrListCollection;
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
  class EMECPresamplerHVData
  {
  public:
    static constexpr double INVALID = -99999;
    class Payload;
    EMECPresamplerHVData();
    EMECPresamplerHVData (std::unique_ptr<Payload> payload);
    EMECPresamplerHVData& operator= (EMECPresamplerHVData&& other);
    ~EMECPresamplerHVData();
    bool hvOn (const EMECPresamplerHVModule& module, const int& iGap) const;
    double voltage (const EMECPresamplerHVModule& module, const int& iGap) const;
    double current (const EMECPresamplerHVModule& module, const int& iGap) const;
    int  hvLineNo  (const EMECPresamplerHVModule& module, const int& iGap) const;
  private:
    int index (const EMECPresamplerHVModule& module) const;
    std::unique_ptr<Payload> m_payload;
  };

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

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
  EMECPresamplerHVData getData (const LArHVIdMapping& hvIdMapping,
                                const std::vector<const CondAttrListCollection*>& attrLists) const;
  // Get hvLine for a module
  int hvLineNo(const EMECPresamplerHVModule& module
               , const LArHVIdMapping* hvIdMapping) const;
#endif

 private:
  using idfunc_t = std::function<std::vector<HWIdentifier>(HWIdentifier)>;
  EMECPresamplerHVData getData (idfunc_t idfunc,
                                const std::vector<const CondAttrListCollection*>& attrLists) const;

  // Illegal operations
  EMECPresamplerHVManager& operator=(const EMECPresamplerHVManager& right) = delete;
  EMECPresamplerHVManager(const EMECPresamplerHVManager& right) = delete;

  class Clockwork;
  std::unique_ptr<const Clockwork> m_c;
};


#endif
