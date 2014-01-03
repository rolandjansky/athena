/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TgcMdtPrepDataFillerTool.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/MdtPrepDataContainer.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "AthenaKernel/errorcheck.h"




D3PD::L1TgcMdtPrepDataFillerTool::L1TgcMdtPrepDataFillerTool(const std::string& type,
                                                             const std::string& name,
                                                             const IInterface* parent)
  : BlockFillerTool<void>(type, name, parent)
{
  declareProperty ("MdtPrepData", m_sgKey = "MDT_DriftCircles", "SG key for MdtPrepData");

  book().ignore(); // Avoid coverity warnings.
}



StatusCode
D3PD::L1TgcMdtPrepDataFillerTool::book()
{

  CHECK(addVariable("prd_x", m_prd_x));
  CHECK(addVariable("prd_y", m_prd_y));
  CHECK(addVariable("prd_z", m_prd_z));
  CHECK(addVariable("prd_adc", m_prd_adc));
  CHECK(addVariable("prd_tdc", m_prd_tdc));
  CHECK(addVariable("prd_status", m_prd_status));
  CHECK(addVariable("prd_drift_radius", m_prd_drift_radius));
  CHECK(addVariable("prd_drift_radius_error", m_prd_drift_radius_error));

  return StatusCode::SUCCESS;
}



StatusCode
D3PD::L1TgcMdtPrepDataFillerTool::fill()
{
  const Muon::MdtPrepDataContainer* container = 0;
  CHECK(evtStore()->retrieve(container, m_sgKey));

  for (Muon::MdtPrepDataContainer::const_iterator cit_cont = container->begin();
       cit_cont != container->end(); cit_cont++) {
    for (Muon::MdtPrepDataCollection::const_iterator cit_coll = (*cit_cont)->begin();
         cit_coll != (*cit_cont)->end(); cit_coll++) {

      const Muon::MdtPrepData *data = *cit_coll;
      const Amg::Vector3D& tubePos = data->globalPosition();

      m_prd_x->push_back(tubePos[0]);
      m_prd_y->push_back(tubePos[1]);
      m_prd_z->push_back(tubePos[2]);
      m_prd_adc->push_back(data->adc());
      m_prd_tdc->push_back(data->tdc());
      m_prd_status->push_back(data->status());
      m_prd_drift_radius->push_back(data->localPosition()[Trk::driftRadius]);
      m_prd_drift_radius_error->push_back(Amg::error(data->localCovariance(),Trk::driftRadius));
    }
  }

  return StatusCode::SUCCESS;
}
/* eof */
