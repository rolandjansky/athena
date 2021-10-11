/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCALOREC_HLTCALOCELLCORRECTOR_H
#define TRIGCALOREC_HLTCALOCELLCORRECTOR_H

#include <AthenaBaseComps/AthReentrantAlgorithm.h>
#include <CaloEvent/CaloConstCellContainer.h>
#include <StoreGate/ReadHandleKey.h>
#include <StoreGate/WriteHandleKey.h>
#include <xAODHIEvent/HIEventShapeAuxContainer.h>
#include <xAODHIEvent/HIEventShapeContainer.h>

class HLTCaloCellCorrector : public AthReentrantAlgorithm {
  public:
    HLTCaloCellCorrector(std::string const& name, ISvcLocator* pSvcLocator);

    virtual StatusCode initialize() override;
    virtual StatusCode execute(EventContext const& context) const override;

  private:
    SG::ReadHandleKey<xAOD::HIEventShapeContainer> m_eventShapeCollectionKey {
      this, "EventShapeCollection", "HLTHIEventShape", "" };
    SG::ReadHandleKey<CaloConstCellContainer> m_inputCellContainerKey {
      this, "InputCellKey", "HLTCaloCellMakerRoI", "" };

    SG::WriteHandleKey<CaloConstCellContainer> m_outputCellContainerKey {
      this, "OutputCellKey", "HLTCorrectedCaloCellRoI", "" };
};

#endif /* TRIGCALOREC_HLTCALOCELLCORRECTOR_H */
