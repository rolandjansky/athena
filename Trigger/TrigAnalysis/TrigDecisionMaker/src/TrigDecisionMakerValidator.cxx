/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigDecisionMakerValidator.h"


TrigDec::TrigDecisionMakerValidator::TrigDecisionMakerValidator(const std::string &name, ISvcLocator *pSvcLocator)
    : ::AthReentrantAlgorithm(name, pSvcLocator)
{}

TrigDec::TrigDecisionMakerValidator::~TrigDecisionMakerValidator() {}

StatusCode
TrigDec::TrigDecisionMakerValidator::initialize() {
  ATH_CHECK( m_tdt.retrieve() );
  ATH_CHECK( m_navigationReadHandleKey.initialize(m_doHLT && m_edmVersion >= 3) );
  return StatusCode::SUCCESS;
}

StatusCode TrigDec::TrigDecisionMakerValidator::reportError(const std::string& item, const std::string& msg) const {
  if(m_errorOnFailure) {
    ATH_MSG_ERROR(item << ": " << msg);
    return StatusCode::FAILURE;
  }
  ATH_MSG_WARNING(item << ": " << msg);
  return StatusCode::SUCCESS;
}

StatusCode TrigDec::TrigDecisionMakerValidator::execute(const EventContext& context) const {

  if (context.eventID().event_number() % m_samplingFrequence != 0) {
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG("Validator is checking this event.");

  if (m_doL1) {

    std::vector<std::string> l1Items = m_tdt->getListOfTriggers("L1_.*");

    size_t passL1 = 0;
    for (const std::string& item : l1Items) {

      const bool passedPhysics = m_tdt->isPassed(item);
      const unsigned passBits = m_tdt->isPassedBits(item);
      const bool l1TBP = (passBits & TrigDefs::L1_isPassedBeforePrescale);
      const bool l1TAP = (passBits & TrigDefs::L1_isPassedAfterPrescale);
      const bool l1TAV = (passBits & TrigDefs::L1_isPassedAfterVeto);

      if (passedPhysics) {
        ++passL1;
        ATH_MSG_VERBOSE("Passing L1:" << item);
      }

      if (l1TAP && !l1TBP) {
        ATH_CHECK( reportError(item, "Cannot pass after L1 prescale if failing before L1 prescale") );
      }

      if (l1TAV && (!l1TBP || !l1TAP)) {
        ATH_CHECK( reportError(item, "Cannot pass after L1 veto if failing either before L1 prescale or after L1 prescale") );
      }

      if (l1TAV && !passedPhysics) {
        ATH_CHECK( reportError(item, "Inconsistency - L1 pass after veto is true but isPassed is false") );
      }

    }

    ATH_MSG_DEBUG("Passing L1 Items: " << passL1);
    if (!passL1) {
      ATH_CHECK( reportError("L1", "Zero L1 items passed this event.") );
    }

  }

  if (m_doHLT) {

    std::vector<std::string> hltChains = m_tdt->getListOfTriggers("HLT_.*");

    TrigCompositeUtils::DecisionIDContainer terminusIDs;
    if (m_edmVersion >= 3) {
      SG::ReadHandle<TrigCompositeUtils::DecisionContainer> navRH{m_navigationReadHandleKey, context};
      const TrigCompositeUtils::Decision* terminusNode = TrigCompositeUtils::getTerminusNode(*navRH);
      if (!terminusNode) {
        ATH_CHECK( reportError(m_navigationReadHandleKey.key(), "Terminus Node could not be located from the primary navigation collection.") );
      }
      TrigCompositeUtils::decisionIDs(terminusNode, terminusIDs);
    }

    size_t passHLT = 0;
    for (const std::string& chain : hltChains) {

      const bool passedPhysics = m_tdt->isPassed(chain);
      const unsigned passBits = m_tdt->isPassedBits(chain);
      const bool l1TBP = (passBits & TrigDefs::L1_isPassedBeforePrescale);
      const bool l1TAP = (passBits & TrigDefs::L1_isPassedAfterPrescale);
      const bool l1TAV = (passBits & TrigDefs::L1_isPassedAfterVeto);
      const bool isPrescaled = (passBits & TrigDefs::EF_prescaled);
      const bool isPassedRaw = (passBits & TrigDefs::EF_passedRaw);
      const bool inTerminusNode = (terminusIDs.count( HLT::Identifier(chain).numeric() ) == 1);

      if (passedPhysics) {
        ++passHLT;
        ATH_MSG_VERBOSE("Passing HLT:" << chain);
      }

      if (l1TAP && !l1TBP) {
        ATH_CHECK( reportError(chain, "Cannot pass after L1 prescale if failing before L1 prescale") );
      }

      if (l1TAV && (!l1TBP || !l1TAP)) {
        ATH_CHECK( reportError(chain, "Cannot pass after L1 veto if failing either before L1 prescale or after L1 prescale") );
      }

      if (passedPhysics && !isPassedRaw) {
        ATH_CHECK( reportError(chain, "Cannot pass for physics if failing at the HLT") );
      }

      if (passedPhysics && !l1TAV) {
        ATH_CHECK( reportError(chain, "Cannot pass for physics if failing at L1") );
      }

      // These check(s) are only valid for the R3 trigger 
      if (m_edmVersion >= 3) {
        if (isPassedRaw && isPrescaled) { // (possible due to passthrough & rerun in R1, R2)
          ATH_CHECK( reportError(chain, "Cannot pass if prescaled") );
        }

        if (passedPhysics && !inTerminusNode) {
          ATH_CHECK( reportError(chain, "Inconsistency - passing for physics, but not present in the navigation terminus node") );
        }

        if (inTerminusNode && !isPassedRaw) {
          ATH_CHECK( reportError(chain, "Inconsistency - in the navigation terminus node, but not passing raw") );
        }
      }

    }

    ATH_MSG_DEBUG("Passing HLT Chains: " << passHLT);
    if (!passHLT) {
      ATH_CHECK( reportError("HLT", "Zero HLT chains passed this event.") );
    }

  }

  return StatusCode::SUCCESS;
}
