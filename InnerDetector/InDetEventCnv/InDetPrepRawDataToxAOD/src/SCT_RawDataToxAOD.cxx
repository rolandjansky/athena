/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_RawDataToxAOD.cxx
//   Implementation file for class SCT_RawDataToxAOD
///////////////////////////////////////////////////////////////////

#include "SCT_RawDataToxAOD.h"

#include "GaudiKernel/ServiceHandle.h"

#include "InDetIdentifier/SCT_ID.h"

// SCT ntuple container type
#include "InDetRawData/SCT_RDO_Container.h"

// xAOD container type
#include "xAODTracking/SCTRawHitValidationContainer.h"
#include "xAODTracking/SCTRawHitValidationAuxContainer.h"

SCT_RawDataToxAOD::SCT_RawDataToxAOD(const std::string &name,
                                     ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_SCTHelper(0)
{
  declareProperty("SiClusterContainer",  m_clustercontainer = "SCT_RawHits");
}

StatusCode SCT_RawDataToxAOD::initialize() {
  CHECK(detStore()->retrieve(m_SCTHelper, "SCT_ID"));
  return StatusCode::SUCCESS;
}

// define accessors used by execute method; defining them once means fewer
// string comparisons.
static SG::AuxElement::Accessor<int> bec_acc("bec");
static SG::AuxElement::Accessor<int> layer_acc("layer");
static SG::AuxElement::Accessor<int> phi_module_acc("phi_module");
static SG::AuxElement::Accessor<int> eta_module_acc("eta_module");
static SG::AuxElement::Accessor<int> side_acc("side");

StatusCode SCT_RawDataToxAOD::execute() {
  const SCT_RDO_Container* rdoContainer = 0;
  CHECK(evtStore()->retrieve(rdoContainer, "SCT_RDOs"));

  // Create the output xAOD container and its auxiliary store:
  xAOD::SCTRawHitValidationContainer* xaod = new xAOD::SCTRawHitValidationContainer();
  CHECK(evtStore()->record(xaod, m_clustercontainer));
  xAOD::SCTRawHitValidationAuxContainer* aux = new xAOD::SCTRawHitValidationAuxContainer();
  CHECK(evtStore()->record(aux, m_clustercontainer + "Aux."));
  xaod->setStore(aux);

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
      // error condensed hit not used anymore (duplicates information from ByteStream errors)
      //      SCT3_RawData* rdo3 = const_cast<SCT3_RawData*>(dynamic_cast<const SCT3_RawData*>(rdo));
      //      if (rdo3) xrdo->setErrorCondensedHit(rdo3->getErrorCondensedHit());
    }
  }
  ATH_MSG_DEBUG(" recorded SCT_RawData objects: size " << xaod->size());
  return StatusCode::SUCCESS;
}

StatusCode SCT_RawDataToxAOD::finalize() {
  return StatusCode::SUCCESS;
}
