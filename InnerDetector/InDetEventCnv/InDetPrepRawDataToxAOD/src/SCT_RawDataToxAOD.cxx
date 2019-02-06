/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_RawDataToxAOD.cxx
//   Implementation file for class SCT_RawDataToxAOD
///////////////////////////////////////////////////////////////////

#include "SCT_RawDataToxAOD.h"

#include "InDetIdentifier/SCT_ID.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
// xAOD container type
#include "xAODTracking/SCTRawHitValidationAuxContainer.h"

SCT_RawDataToxAOD::SCT_RawDataToxAOD(const std::string &name,
                                     ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator),
    m_SCTHelper{nullptr}
{
}

StatusCode SCT_RawDataToxAOD::initialize() {
  ATH_CHECK(detStore()->retrieve(m_SCTHelper, "SCT_ID"));
  ATH_CHECK(m_rdoContainerName.initialize());
  ATH_CHECK(m_xAodRawHitContainerName.initialize());
  return StatusCode::SUCCESS;
}

// define accessors used by execute method; defining them once means fewer
// string comparisons.
static SG::AuxElement::Accessor<int> bec_acc("bec");
static SG::AuxElement::Accessor<int> layer_acc("layer");
static SG::AuxElement::Accessor<int> phi_module_acc("phi_module");
static SG::AuxElement::Accessor<int> eta_module_acc("eta_module");
static SG::AuxElement::Accessor<int> side_acc("side");

StatusCode SCT_RawDataToxAOD::execute(const EventContext& ctx) const {
  SG::ReadHandle<SCT_RDO_Container> rdoContainer(m_rdoContainerName, ctx);

  // Create the output xAOD container and its auxiliary store:
  SG::WriteHandle<xAOD::SCTRawHitValidationContainer> xaod(m_xAodRawHitContainerName, ctx);
  ATH_CHECK(xaod.record(std::make_unique<xAOD::SCTRawHitValidationContainer>(),
                        std::make_unique<xAOD::SCTRawHitValidationAuxContainer>()));

  /// loop over input RDOs
  for (const SCT_RDO_Collection* collection : *rdoContainer) {
    for (const SCT_RDORawData* rdo : *collection) {
      // create and add xAOD object
      xAOD::SCTRawHitValidation* xrdo = new xAOD::SCTRawHitValidation();
      xaod->push_back(xrdo);
      /// copy xrdo properties from input rdo
      const Identifier& id = rdo->identify();
      xrdo->setIdentifier(id.get_compact());
      xrdo->setWord(rdo->getWord());
      // setting additional decorations based on identifier
      bec_acc(*xrdo) = m_SCTHelper->barrel_ec(id);
      layer_acc(*xrdo) = m_SCTHelper->layer_disk(id);
      phi_module_acc(*xrdo) = m_SCTHelper->phi_module(id);
      eta_module_acc(*xrdo) = m_SCTHelper->eta_module(id);
      side_acc(*xrdo) = m_SCTHelper->side(id);
    }
  }
  ATH_MSG_DEBUG(" recorded SCT_RawData objects: size " << xaod->size());
  return StatusCode::SUCCESS;
}

StatusCode SCT_RawDataToxAOD::finalize() {
  return StatusCode::SUCCESS;
}
