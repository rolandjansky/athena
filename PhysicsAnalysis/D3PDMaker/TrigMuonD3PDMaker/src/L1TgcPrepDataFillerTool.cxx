/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/errorcheck.h"

#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonPrepRawData/TgcPrepData.h"
#include "MuonDigitContainer/TgcDigit.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"

#include "L1TgcPrepDataFillerTool.h"


using HepGeom::Point3D;


D3PD::L1TgcPrepDataFillerTool::L1TgcPrepDataFillerTool(const std::string& type,
                                                       const std::string& name,
                                                       const IInterface* parent)
  : BlockFillerTool<void>(type, name, parent),
    m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool")
{
  declareProperty ("TgcPrepDataPrevious", m_tgcPrepDataPrv = "TGC_MeasurementsPriorBC", "SG key for TgcPrepData in previous bunch");
  declareProperty ("TgcPrepDataCurrent", m_tgcPrepDataCur = "TGC_Measurements", "SG key for TgcPrepData in current bunch");
  declareProperty ("TgcPrepDataNext", m_tgcPrepDataNxt = "TGC_MeasurementsNextBC", "SG key for TgcPrepData in next bunch");
  declareProperty ("MuonIdHelperTool", m_idHelper, "Muon IdHelperTool instance.");

  book().ignore(); // Avoid coverity warnings.
}



/** 
 * @brief Standard Gaudi initialize method.
 */
StatusCode
D3PD::L1TgcPrepDataFillerTool::initialize()
{
  CHECK( m_idHelper.retrieve() );
  return StatusCode::SUCCESS;
}


StatusCode
D3PD::L1TgcPrepDataFillerTool::book()
{
  CHECK(addVariable("prd_x", m_prd_x));
  CHECK(addVariable("prd_y", m_prd_y));
  CHECK(addVariable("prd_z", m_prd_z));
  CHECK(addVariable("prd_shortWidth", m_prd_shortWidth));
  CHECK(addVariable("prd_longWidth", m_prd_longWidth));
  CHECK(addVariable("prd_length", m_prd_length));
  CHECK(addVariable("prd_isStrip", m_prd_isStrip));
  CHECK(addVariable("prd_gasGap", m_prd_gasGap));
  CHECK(addVariable("prd_channel", m_prd_channel));
  CHECK(addVariable("prd_eta", m_prd_eta));
  CHECK(addVariable("prd_phi", m_prd_phi));
  CHECK(addVariable("prd_station", m_prd_station));
  CHECK(addVariable("prd_bunch", m_prd_bunch));

  return StatusCode::SUCCESS;
}



StatusCode
D3PD::L1TgcPrepDataFillerTool::fill()
{
  CHECK(fillPreviousBc());
  CHECK(fillCurrentBc());
  CHECK(fillNextBc());

  return StatusCode::SUCCESS;
}



StatusCode
D3PD::L1TgcPrepDataFillerTool::fillTgcPrepData(const Muon::TgcPrepData* data,
                                               const int bunch)
{
  const TgcIdHelper& tgcIdHelper = m_idHelper->tgcIdHelper();

  const MuonGM::TgcReadoutElement* element = data->detectorElement();
  const Identifier id = data->identify();
  const int gasGap = tgcIdHelper.gasGap(id);
  const int channel = tgcIdHelper.channel(id);
  const bool isStrip = tgcIdHelper.isStrip(id);
  const Amg::Vector3D& pos = isStrip ? element->stripPos(gasGap, channel) : element->gangPos(gasGap, channel);

  m_prd_x->push_back(pos[0]);
  m_prd_y->push_back(pos[1]);
  m_prd_z->push_back(pos[2]);

  m_prd_shortWidth->push_back(isStrip ? element->stripShortWidth(gasGap, channel) : element->gangShortWidth(gasGap, channel));
  m_prd_longWidth->push_back(isStrip ? element->stripLongWidth(gasGap, channel) : element->gangLongWidth(gasGap, channel));
  m_prd_length->push_back(isStrip ? element->stripLength(gasGap, channel) : element->gangLength(gasGap, channel));

  m_prd_isStrip->push_back(tgcIdHelper.isStrip(id));
  m_prd_gasGap->push_back(tgcIdHelper.gasGap(id));
  m_prd_channel->push_back(tgcIdHelper.channel(id));
  m_prd_eta->push_back(tgcIdHelper.stationEta(id));
  m_prd_phi->push_back(tgcIdHelper.stationPhi(id));
  m_prd_station->push_back(tgcIdHelper.stationName(id));
  m_prd_bunch->push_back(bunch);

  return StatusCode::SUCCESS;
}



StatusCode
D3PD::L1TgcPrepDataFillerTool::fillPreviousBc()
{
  const Muon::TgcPrepDataContainer* container = 0;
  CHECK(evtStore()->retrieve(container, m_tgcPrepDataPrv));

  Muon::TgcPrepDataContainer::const_iterator tpdc_it;
  Muon::TgcPrepDataCollection::const_iterator cit;
  for (tpdc_it = container->begin(); tpdc_it != container->end(); tpdc_it++) {
    for (cit = (*tpdc_it)->begin(); cit != (*tpdc_it)->end(); cit++) {
      const Muon::TgcPrepData *data = *cit;
      CHECK(fillTgcPrepData(data, TgcDigit::BC_PREVIOUS));
    }
  }

  return StatusCode::SUCCESS;
}



StatusCode
D3PD::L1TgcPrepDataFillerTool::fillCurrentBc()
{
  const Muon::TgcPrepDataContainer* container = 0;
  CHECK(evtStore()->retrieve(container, m_tgcPrepDataCur));

  Muon::TgcPrepDataContainer::const_iterator tpdc_it;
  Muon::TgcPrepDataCollection::const_iterator cit;
  for (tpdc_it = container->begin(); tpdc_it != container->end(); tpdc_it++) {
    for (cit = (*tpdc_it)->begin(); cit != (*tpdc_it)->end(); cit++) {
      const Muon::TgcPrepData *data = *cit;
      CHECK(fillTgcPrepData(data, TgcDigit::BC_CURRENT));
    }
  }

  return StatusCode::SUCCESS;
}



StatusCode
D3PD::L1TgcPrepDataFillerTool::fillNextBc()
{
  const Muon::TgcPrepDataContainer* container = 0;
  CHECK(evtStore()->retrieve(container, m_tgcPrepDataNxt));

  Muon::TgcPrepDataContainer::const_iterator tpdc_it;
  Muon::TgcPrepDataCollection::const_iterator cit;
  for (tpdc_it = container->begin(); tpdc_it != container->end(); tpdc_it++) {
    for (cit = (*tpdc_it)->begin(); cit != (*tpdc_it)->end(); cit++) {
      const Muon::TgcPrepData *data = *cit;
      CHECK(fillTgcPrepData(data, TgcDigit::BC_NEXT));
    }
  }

  return StatusCode::SUCCESS;
}
/* eof */
