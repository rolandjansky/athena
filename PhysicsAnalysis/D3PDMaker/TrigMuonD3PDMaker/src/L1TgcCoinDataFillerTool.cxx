/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/errorcheck.h"

#include "MuonTrigCoinData/TgcCoinDataCollection.h"
#include "MuonTrigCoinData/TgcCoinDataContainer.h"
#include "MuonTrigCoinData/TgcCoinData.h"
#include "MuonDigitContainer/TgcDigit.h"

#include "L1TgcCoinDataFillerTool.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"



D3PD::L1TgcCoinDataFillerTool::L1TgcCoinDataFillerTool(const std::string& type,
                                                       const std::string& name,
                                                       const IInterface* parent)
  : BlockFillerTool<void>(type, name, parent)
{
  declareProperty ("TgcCoinDataPrevious", m_tgcCoinDataPrv = "TrigT1CoinDataCollectionPriorBC", "SG key for TgcCoinData in previous bunch");
  declareProperty ("TgcCoinDataCurrent", m_tgcCoinDataCur = "TrigT1CoinDataCollection", "SG key for TgcCoinData in current bunch");
  declareProperty ("TgcCoinDataNext", m_tgcCoinDataNxt = "TrigT1CoinDataCollectionNextBC", "SG key for TgcCoinData in next bunch");

  book().ignore(); // Avoid coverity warnings.
}



StatusCode
D3PD::L1TgcCoinDataFillerTool::book()
{
  CHECK(addVariable("coin_x_In", m_coin_x_In));
  CHECK(addVariable("coin_y_In", m_coin_y_In));
  CHECK(addVariable("coin_z_In", m_coin_z_In));
  CHECK(addVariable("coin_x_Out", m_coin_x_Out));
  CHECK(addVariable("coin_y_Out", m_coin_y_Out));
  CHECK(addVariable("coin_z_Out", m_coin_z_Out));
  CHECK(addVariable("coin_width_In", m_coin_width_In));
  CHECK(addVariable("coin_width_Out", m_coin_width_Out));
  CHECK(addVariable("coin_width_R", m_coin_width_R));
  CHECK(addVariable("coin_width_Phi", m_coin_width_Phi));
  CHECK(addVariable("coin_isAside", m_coin_isAside));
  CHECK(addVariable("coin_isForward", m_coin_isForward));
  CHECK(addVariable("coin_isStrip", m_coin_isStrip));
  CHECK(addVariable("coin_isPositiveDeltaR", m_coin_isPositiveDeltaR));
  CHECK(addVariable("coin_type", m_coin_type));
  CHECK(addVariable("coin_trackletId", m_coin_trackletId));
  CHECK(addVariable("coin_trackletIdStrip", m_coin_trackletIdStrip));
  CHECK(addVariable("coin_phi", m_coin_phi));
  CHECK(addVariable("coin_roi", m_coin_roi));
  CHECK(addVariable("coin_pt", m_coin_pt));
  CHECK(addVariable("coin_delta", m_coin_delta));
  CHECK(addVariable("coin_sub", m_coin_sub));
  CHECK(addVariable("coin_bunch", m_coin_bunch));

  return StatusCode::SUCCESS;
}



StatusCode
D3PD::L1TgcCoinDataFillerTool::fill()
{
  CHECK(fillPreviousBc());
  CHECK(fillCurrentBc());
  CHECK(fillNextBc());

  return StatusCode::SUCCESS;
}



StatusCode
D3PD::L1TgcCoinDataFillerTool::fillTgcCoinData(const Muon::TgcCoinData* data,
                                               const int bunch)
{
  //const Identifier id = data->identify();
  const int type = data->type();

  const Amg::Vector3D& posIn = data->globalposIn();
  m_coin_x_In->push_back(posIn[0]);
  m_coin_y_In->push_back(posIn[1]);
  m_coin_z_In->push_back(posIn[2]);

  const Amg::Vector3D& posOut = data->globalposOut();
  m_coin_x_Out->push_back(posOut[0]);
  m_coin_y_Out->push_back(posOut[1]);
  m_coin_z_Out->push_back(posOut[2]);

  m_coin_width_In->push_back(data->widthIn());
  m_coin_width_Out->push_back(data->widthOut());

  if (type == Muon::TgcCoinData::TYPE_SL) {
    const Amg::MatrixX& matrix = data->errMat();
    m_coin_width_R->push_back(Amg::error(matrix, Trk::locR));
    m_coin_width_Phi->push_back(Amg::error(matrix, Trk::locPhi));
  } else {
    m_coin_width_R->push_back(0.);
    m_coin_width_Phi->push_back(0.);
  }

  m_coin_isAside->push_back(data->isAside());
  m_coin_isForward->push_back(data->isForward());
  m_coin_isStrip->push_back(data->isStrip());
  m_coin_isPositiveDeltaR->push_back(data->isPositiveDeltaR());

  m_coin_type->push_back(type);
  m_coin_trackletId->push_back(data->trackletId());
  m_coin_trackletIdStrip->push_back(data->trackletIdStrip());
  m_coin_phi->push_back(data->phi());
  m_coin_roi->push_back(data->roi());
  m_coin_pt->push_back(data->pt());
  m_coin_delta->push_back(data->delta());
  m_coin_sub->push_back(data->sub());

  m_coin_bunch->push_back(bunch);

  return StatusCode::SUCCESS;
}



StatusCode
D3PD::L1TgcCoinDataFillerTool::fillPreviousBc()
{
  const Muon::TgcCoinDataContainer* container = 0;
  CHECK(evtStore()->retrieve(container, m_tgcCoinDataPrv));

  Muon::TgcCoinDataContainer::const_iterator tcdc_it;
  Muon::TgcCoinDataCollection::const_iterator cit;
  for (tcdc_it = container->begin(); tcdc_it != container->end(); tcdc_it++) {
    for (cit = (*tcdc_it)->begin(); cit != (*tcdc_it)->end(); cit++) {
      const Muon::TgcCoinData *data = *cit;
      CHECK(fillTgcCoinData(data, (int)TgcDigit::BC_PREVIOUS));
    }
  }

  return StatusCode::SUCCESS;
}



StatusCode
D3PD::L1TgcCoinDataFillerTool::fillCurrentBc()
{
  const Muon::TgcCoinDataContainer* container = 0;
  CHECK(evtStore()->retrieve(container, m_tgcCoinDataCur));

  Muon::TgcCoinDataContainer::const_iterator tcdc_it;
  Muon::TgcCoinDataCollection::const_iterator cit;
  for (tcdc_it = container->begin(); tcdc_it != container->end(); tcdc_it++) {
    for (cit = (*tcdc_it)->begin(); cit != (*tcdc_it)->end(); cit++) {
      const Muon::TgcCoinData *data = *cit;
      CHECK(fillTgcCoinData(data, (int)TgcDigit::BC_CURRENT));
    }
  }

  return StatusCode::SUCCESS;
}



StatusCode
D3PD::L1TgcCoinDataFillerTool::fillNextBc()
{
  const Muon::TgcCoinDataContainer* container = 0;
  CHECK(evtStore()->retrieve(container, m_tgcCoinDataNxt));

  Muon::TgcCoinDataContainer::const_iterator tcdc_it;
  Muon::TgcCoinDataCollection::const_iterator cit;
  for (tcdc_it = container->begin(); tcdc_it != container->end(); tcdc_it++) {
    for (cit = (*tcdc_it)->begin(); cit != (*tcdc_it)->end(); cit++) {
      const Muon::TgcCoinData *data = *cit;
      CHECK(fillTgcCoinData(data, (int)TgcDigit::BC_NEXT));
    }
  }

  return StatusCode::SUCCESS;
}
/* eof */
