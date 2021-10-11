/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_FCALHVMANAGER_H
#define LARHV_FCALHVMANAGER_H

#include "LArHV/FCALHVModule.h"
#include "Identifier/HWIdentifier.h"
#include <memory>
#include <functional>

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
class LArHVIdMapping;
#endif

class CondAttrListCollection;
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
  class FCALHVData
  {
  public:
    static constexpr double INVALID = -99999;
    class Payload;
    FCALHVData();
    FCALHVData (std::unique_ptr<Payload> payload);
    FCALHVData& operator= (FCALHVData&& other);
    ~FCALHVData();
    bool hvOn (const FCALHVLine& line) const;
    double voltage (const FCALHVLine& line) const;
    double current (const FCALHVLine& line) const;
    int  hvLineNo  (const FCALHVLine& line) const;
  private:
    int index (const FCALHVLine& line) const;
    std::unique_ptr<Payload> m_payload;
  };

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

  // Get the database payload --- for use by simulation only
  // (doesn't account for conditions changes)
  FCALHVData getDataSim() const;

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
  FCALHVData getData (const LArHVIdMapping& hvIdMapping,
                      const std::vector<const CondAttrListCollection*>& attrLists) const;
  // Get hvLine for an electrode
  int hvLineNo(const FCALHVLine& line
               , const LArHVIdMapping* hvIdMapping) const;
#endif

 private:
  using idfunc_t = std::function<std::vector<HWIdentifier>(HWIdentifier)>;
  FCALHVData getData (idfunc_t idfunc,
                      const std::vector<const CondAttrListCollection*>& attrLists) const;

  FCALHVManager(const FCALHVManager& right) = delete;
  FCALHVManager& operator=(const FCALHVManager& right) = delete;

  class Clockwork;
  std::unique_ptr<const Clockwork> m_c;
};

#endif
