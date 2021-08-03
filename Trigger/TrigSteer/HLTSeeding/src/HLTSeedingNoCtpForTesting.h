/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTSeeding_HLTSeedingNoCtpForTesting_h
#define HLTSeeding_HLTSeedingNoCtpForTesting_h


#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "TrigT1Result/RoIBResult.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

/*
  @brief an algorithm used to unpack the RoIB result and provide RoIs for test algorithms.
  It is different from the proper @see HLTSeeding algorithm because it has no menu dependencey and realy does not care about the CTP decisions.
  It only provides RoIs so that HLT reconstruction algorithms have seeds to run on.
  For EMTAU RoIs the generated decisions are flagged with a specimen chain (see source).
  @warning - this algorithm is only for testing, never to be used in production.
 */

class HLTSeedingNoCtpForTesting : public AthReentrantAlgorithm {
public:
  HLTSeedingNoCtpForTesting(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize() override;
  virtual StatusCode execute (const EventContext& ctx) const override;
  //  virtual StatusCode finalize() override;

private:

  ///@{ @name Properties
  SG::ReadHandleKey<ROIB::RoIBResult> m_RoIBResultKey{
    this, "RoIBResult", "RoIBResult",
    "Name of RoIBResult"};

  SG::WriteHandleKey<TrigRoiDescriptorCollection> m_trigEMRoIsKey{
    this, "EMRoIs", "HLT_TestEMRoIs",
    "Name of the EM RoIs object produced by the unpacker"};

  SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer> m_EMDecisionsKey{
    this, "EMDecisions", "HLTNav_TestL1EM", "Decisions for each EM RoI"};


  SG::WriteHandleKey<TrigRoiDescriptorCollection> m_trigFSRoIKey{
    this, "OutputTrigFSRoI", "HLT_TestFSRoI",
    "Name of the Full Scan RoIs object produced by the unpacker"};

  float m_roIWidth = 0.2;
};

#endif
