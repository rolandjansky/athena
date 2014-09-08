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
   * @param _name Const ref to name of the counter. Will only have one (plus a dummy).
   * @param _ID ID number of Read Out Buffer
   */
  CounterROI::CounterROI( const TrigCostData* _costData, const std::string& _name, Int_t _ID, UInt_t _detailLevel ) : CounterBase(_costData, _name, _ID, _detailLevel) {
    
    m_dataStore.newVariable(kVarType).setSavePerCall("ROI Type;ROI Type;Calls");
    std::vector<std::string> _roiTypes;
    _roiTypes.push_back("None");
    _roiTypes.push_back("#mu");
    _roiTypes.push_back("EM/#tau");
    _roiTypes.push_back("Jet");
    _roiTypes.push_back("JetE_{T}");
    _roiTypes.push_back("Energy");
    m_dataStore.setBinLabels(kVarType, kSavePerCall, _roiTypes);

    m_dataStore.newVariable(kVarEta).setSavePerCall("ROI #eta Distribution;#eta;Calls");
    m_dataStore.newVariable(kVarPhi).setSavePerCall("ROI #phi Distribution;#phi;Calls");
    m_dataStore.newVariable(kVarArea).setSavePerCall("Area (#eta#times#phi) Of ROIs;Area;Calls");
    m_dataStore.newVariable(kVarL1Thresh).setSavePerCall("Number Of ROI L1 Thresholds;L1 Thresholds;Calls");
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
   * @param _e RoI to process.
   * @param _f Unused
   * @param _weight Event weight.
   */
  void CounterROI::processEventCounter(UInt_t _e, UInt_t _f, Float_t _weight) {
    ++m_calls;
    UNUSED( _f );

    if (m_costData->getIsRoINone(_e) == kTRUE) m_dataStore.store(kVarType, 0., _weight);
    else if (m_costData->getIsRoIMuon(_e) == kTRUE) m_dataStore.store(kVarType, 1., _weight);
    else if (m_costData->getIsRoIEmTau(_e) == kTRUE) m_dataStore.store(kVarType, 2., _weight);
    else if (m_costData->getIsRoIJet(_e) == kTRUE) m_dataStore.store(kVarType, 3., _weight);
    else if (m_costData->getIsRoIJetEt(_e) == kTRUE) m_dataStore.store(kVarType, 4., _weight);
    else if (m_costData->getIsRoIEnergy(_e) == kTRUE) m_dataStore.store(kVarType, 5., _weight);

    if (m_costData->getRoIEta(_e) > -9000) { // Some ROIs do not have a location - these return -9999 for these vars
      m_dataStore.store(kVarEta, m_costData->getRoIEta(_e), _weight );
      m_dataStore.store(kVarPhi, m_costData->getRoIPhi(_e), _weight );
      m_dataStore.store(kVarArea, m_costData->getRoIArea(_e), _weight );
    }
    m_dataStore.store(kVarL1Thresh, m_costData->getRoINL1Thresh(_e), _weight );
    m_dataStore.store(kVarCalls, 1., _weight );

    if ( Config::config().debug() ) debug(_e);
  }
  
  /**
   * Perform end-of-event monitoring on the DataStore.
   */
  void CounterROI::endEvent() {
    m_dataStore.endEvent();
  }
  
  /**
   * Output debug information on this call to the console
   */
  void CounterROI::debug(UInt_t _e) {

    Info("CounterROI::debug", "ROI %u: ID:%i Eta:%.2f Phi:%.2f Area:%.2f NL1T:%i isNone:%i isMu:%i isEmTau:%i isJet:%i isJetEt:%i isE:%i",
      _e,
      m_costData->getRoIID(_e),
      m_costData->getRoIEta(_e),
      m_costData->getRoIPhi(_e),
      m_costData->getRoIArea(_e),
      m_costData->getRoINL1Thresh(_e),
      m_costData->getIsRoINone(_e),
      m_costData->getIsRoIMuon(_e),
      m_costData->getIsRoIEmTau(_e),
      m_costData->getIsRoIJet(_e),
      m_costData->getIsRoIJetEt(_e), 
      m_costData->getIsRoIEnergy(_e) );
  }
  
} // namespace TrigCostRootAnalysis
