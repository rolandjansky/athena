/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloEvent/CaloCellLinkContainer.h"
#include "CaloUtils/CaloClusterStoreHelper.h"

#include "CellsInCone.h"
#include "DerivationFrameworkCalo/CellsInConeThinning.h"

DerivationFramework::CellsInConeThinning::CellsInConeThinning(
  const std::string& type,
  const std::string& name,
  const IInterface* parent)
  : ExpressionParserUser<AthAlgTool>(type, name, parent)
  , m_selectionString("")
  , m_dr(0.5)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("deltaR", m_dr = 0.5);
}

StatusCode
DerivationFramework::CellsInConeThinning::initialize()
{

  ATH_CHECK(m_SGKey.initialize());
  ATH_CHECK(m_InputCellsSGKey.initialize());
  ATH_CHECK(m_OutputClusterSGKey.initialize());
  ATH_CHECK(m_OutputCellLinkSGKey.initialize());
  ATH_CHECK(m_caloMgrKey.initialize());

  if (!m_selectionString.empty()) {
    ATH_CHECK(initializeParser(m_selectionString));
  }
  return StatusCode::SUCCESS;
}

StatusCode
DerivationFramework::CellsInConeThinning::finalize()
{
  ATH_CHECK(finalizeParser());
  return StatusCode::SUCCESS;
}

StatusCode
DerivationFramework::CellsInConeThinning::addBranches() const
{

  /// Make new container
  SG::WriteHandle<xAOD::CaloClusterContainer> dclHdl(m_OutputClusterSGKey);
  ATH_CHECK(CaloClusterStoreHelper::AddContainerWriteHandle(dclHdl));
 
  /// Input objects
  SG::ReadHandle<xAOD::EgammaContainer> egHdl(m_SGKey);
  const xAOD::EgammaContainer* egammas = egHdl.cptr();
  if (!egammas) {
    ATH_MSG_ERROR("Couldn't retrieve egamma container with key: " << m_SGKey);
    return StatusCode::FAILURE;
  }

  unsigned int nEgammas(egammas->size());
  if (nEgammas == 0) {
    return StatusCode::SUCCESS;
  }

  SG::ReadHandle<CaloCellContainer> cellHdl(m_InputCellsSGKey);
  const CaloCellContainer* cells = cellHdl.cptr();
  if (!cells) {
    ATH_MSG_ERROR(
      "Couldn't retrieve cell container with key: " << m_InputCellsSGKey);
    return StatusCode::FAILURE;
  }

  SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{ m_caloMgrKey };
  const CaloDetDescrManager* caloDDMgr = *caloMgrHandle;

  // We have a selection string
  if (!m_selectionString.empty()) {
    std::vector<int> entries = m_parser->evaluateAsVector();
    unsigned int nEntries = entries.size();
    // check the sizes are compatible
    if (nEgammas != nEntries) {
      ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string "
                    "used e-gamma objects");
      return StatusCode::FAILURE;
    }
    // Loop over the egammas, pick the selected ones and fill the cluster
    int index(0);
    for (const xAOD::Egamma* eg : *egammas) {
      if (static_cast<bool>(entries.at(index))) {
        xAOD::CaloCluster* dummy = CaloClusterStoreHelper::makeCluster(dclHdl.ptr(),cells);
        DerivationFramework::CellsInCone::egammaSelect(
          dummy, cells, caloDDMgr, eg, m_dr);
      }
      ++index;
    }
  }
  /// Fill clusters without selection string
  else {
    for (const xAOD::Egamma* eg : *egammas) {
      xAOD::CaloCluster* dummy = CaloClusterStoreHelper::makeCluster(dclHdl.ptr(),cells);
      DerivationFramework::CellsInCone::egammaSelect(
        dummy, cells, caloDDMgr, eg, m_dr);
    }
  }
  /// Finalize clusters
  SG::WriteHandle<CaloClusterCellLinkContainer> cellLinks(
    m_OutputCellLinkSGKey);
  ATH_CHECK(CaloClusterStoreHelper::finalizeClusters(cellLinks, dclHdl.ptr()));
  /// Return
  return StatusCode::SUCCESS;
}
////

