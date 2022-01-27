/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "HLTCaloCellCorrector.h"

#include <xAODHIEvent/HIEventShape.h>

#include <memory>

namespace {
  double get_average_energy(xAOD::HIEventShapeContainer const& eventShape,
                            int layer, double eta) {
    for (const auto es : eventShape) {
      if (es->layer() != layer || eta > es->etaMax() || eta <= es->etaMin())
        continue;

      double et = es->et();

      /* energy in MeV; essentially zero */
      if (std::abs(et) < 0.1)
        continue;

      return et / es->nCells();
    }

    return 0.;
  }
}

HLTCaloCellCorrector::HLTCaloCellCorrector(std::string const& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) {
}

StatusCode HLTCaloCellCorrector::initialize() {
  ATH_CHECK(m_eventShapeCollectionKey.initialize());
  ATH_CHECK(m_inputCellContainerKey.initialize());
  ATH_CHECK(m_outputCellContainerKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode HLTCaloCellCorrector::execute(EventContext const& context) const {
  ATH_MSG_DEBUG("HLTCaloCellCorrector::execute()");

  SG::ReadHandle<xAOD::HIEventShapeContainer> eventShapeHandle(m_eventShapeCollectionKey, context);
  SG::ReadHandle<CaloConstCellContainer> inputCellHandle(m_inputCellContainerKey, context);

  SG::WriteHandle<CaloConstCellContainer> outputCellHandle(m_outputCellContainerKey, context);

  auto eventShape = *eventShapeHandle;
  //auto outputCells = std::make_unique<CaloConstCellContainer>(SG::VIEW_ELEMENTS);
  auto outputCells = std::make_unique<CaloConstCellContainer>();

  for (auto const* cell : *inputCellHandle) {
    std::unique_ptr<CaloCell> copy = cell->clone();

    int layer = copy->caloDDE()->getSampling();

    double eavg = get_average_energy(eventShape, layer, copy->eta());
    double etotal = (copy->et() - eavg) / copy->sinTh();

    copy->setEnergy(etotal);
    outputCells->push_back(std::move(copy));
  }

  for (auto const id : { CaloCell_ID::LAREM, CaloCell_ID::LARHEC, CaloCell_ID::LARFCAL, CaloCell_ID::TILE, })
    if (inputCellHandle->hasCalo(id))
      outputCells->setHasCalo(CaloCell_ID::LAREM);

  outputCells->updateCaloIterators();

  ATH_CHECK(outputCellHandle.record(std::move(outputCells)));

  return StatusCode::SUCCESS;
}
