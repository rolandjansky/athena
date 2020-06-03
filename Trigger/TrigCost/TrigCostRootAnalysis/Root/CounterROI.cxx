// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <iostream>

// Local include(s):
#include "../TrigCostRootAnalysis/CounterROI.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/Utility.h"

namespace TrigCostRootAnalysis {
  /**
   * Region of Interest counter object.
   * @param name Const ref to name of the counter. Will only have one (plus a dummy).
   * @param ID ID number of Read Out Buffer
   */
  CounterROI::CounterROI(const TrigCostData* costData, const std::string& name, Int_t ID, UInt_t detailLevel,
                         MonitorBase* parent)
    : CounterBase(costData, name, ID, detailLevel, parent) {
    m_dataStore.newVariable(kVarType).setSavePerCall("ROI Type;ROI Type;Calls");
    std::vector<std::string> roiTypes;
    roiTypes.push_back("None");
    roiTypes.push_back("#mu");
    roiTypes.push_back("EM/#tau");
    roiTypes.push_back("Jet");
    roiTypes.push_back("JetE_{T}");
    roiTypes.push_back("Energy");
    m_dataStore.setBinLabels(kVarType, kSavePerCall, roiTypes);

    m_dataStore.newVariable(kVarEventsActive).setSavePerEvent();
    m_dataStore.newVariable(kVarEta).setSavePerCall("ROI #eta Distribution;#eta;ROIs");
    m_dataStore.newVariable(kVarPhi).setSavePerCall("ROI #phi Distribution;#phi;ROIs");
    m_dataStore.newVariable(kVarEt).setSavePerCall(
      "ROI Energy Distribution;E_{T} (e,#tau,j,#mu) or H_{T} (energy RoI) [GeV];ROIs");
    m_dataStore.newVariable(kVarArea).setSavePerCall("Area (#eta#times#phi) Of ROIs;Area;ROIs");
    m_dataStore.newVariable(kVarL1Thresh).setSavePerCall("Number Of ROI L1 Thresholds;L1 Thresholds;ROIs");
    m_dataStore.newVariable(kVarCalls).setSavePerEvent("Number Of ROIs Per Event;N ROIs;Events");
  }

  /**
   * Counter destructor.
   */
  CounterROI::~CounterROI() {
  }

  /**
   * Reset per-event counter(s). Not used currently for ROI counter.
   */
  void CounterROI::startEvent() {
  }

  /**
   * Perform monitoring of a RoI.
   * @param e RoI to process.
   * @param f Unused
   * @param weight Event weight.
   */
  void CounterROI::processEventCounter(UInt_t e, UInt_t /*f*/, Float_t weight) {
    ++m_calls;

    if (m_costData->getIsRoINone(e) == kTRUE) m_dataStore.store(kVarType, 0., weight);
    else if (m_costData->getIsRoIMuon(e) == kTRUE) m_dataStore.store(kVarType, 1., weight);
    else if (m_costData->getIsRoIEmTau(e) == kTRUE) m_dataStore.store(kVarType, 2., weight);
    else if (m_costData->getIsRoIJet(e) == kTRUE) m_dataStore.store(kVarType, 3., weight);
    else if (m_costData->getIsRoIJetEt(e) == kTRUE) m_dataStore.store(kVarType, 4., weight);
    else if (m_costData->getIsRoIEnergy(e) == kTRUE) m_dataStore.store(kVarType, 5., weight);

    if (m_costData->getRoIEta(e) > -9000) { // Some ROIs do not have a location - these return -9999 for these vars
      m_dataStore.store(kVarEta, m_costData->getRoIEta(e), weight);
      m_dataStore.store(kVarPhi, m_costData->getRoIPhi(e), weight);
      m_dataStore.store(kVarArea, m_costData->getRoIArea(e), weight);
    }
    m_dataStore.store(kVarL1Thresh, m_costData->getRoINL1Thresh(e), weight);
    //m_dataStore.store(kVarEt, m_costData->getRoIEt(e), weight ); // disable for now until propagated everywhere
    m_dataStore.store(kVarCalls, 1., weight);


    if (Config::config().debug()) debug(e);
  }

  /**
   * Perform end-of-event monitoring on the DataStore.
   */
  void CounterROI::endEvent(Float_t weight) {
    m_dataStore.store(kVarEventsActive, 1., weight);
    m_dataStore.endEvent();
  }

  /**
   * When running with prescales applied. This function returns how the counter should be scaled for the current call.
   * Not used by ROIs - doesn't make sense. Object not mapped to chain
   * @return Multiplicative weighting factor
   */
  Double_t CounterROI::getPrescaleFactor(UInt_t /*e*/) {
    return 0.;
  }

  /**
   * Output debug information on this call to the console
   */
  void CounterROI::debug(UInt_t e) {
    Info("CounterROI::debug",
         "ROI %u %s: ID:%i Eta:%.2f Phi:%.2f Area:%.2f NL1T:%i ET:%f ETLarge:%.2f muC:%i iso:%i EX:%.2f EY:%.2f isNone:%i isMu:%i isEmTau:%i isTau:%i isJet:%i isJetEt:%i isE:%i",
         e,
         m_costData->getRoITypeString(e).c_str(),
         m_costData->getRoIID(e),
         m_costData->getRoIEta(e),
         m_costData->getRoIPhi(e),
         m_costData->getRoIArea(e),
         m_costData->getRoINL1Thresh(e),

         m_costData->getRoIEt(e),
         m_costData->getRoIEtLarge(e),
         m_costData->getRoIMuonCharge(e),
         m_costData->getRoIEmTauIsoBits(e),
         m_costData->getRoIVectorEX(e),
         m_costData->getRoIVectorEY(e),
         //m_costData->getRoIOverflowEX(e),
         //m_costData->getRoIOverflowEY(e),
         //m_costData->getRoIOverflowET(e),

         m_costData->getIsRoINone(e),
         m_costData->getIsRoIMuon(e),
         m_costData->getIsRoIEmTau(e),
         m_costData->getIsRoITau(e),
         m_costData->getIsRoIJet(e),
         m_costData->getIsRoIJetEt(e),
         m_costData->getIsRoIEnergy(e));
  }
} // namespace TrigCostRootAnalysis
