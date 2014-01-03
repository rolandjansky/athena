/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/errorcheck.h"

#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepData.h"

#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

#include "L1TgcRpcPrepDataFillerTool.h"


using HepGeom::Point3D;


D3PD::L1TgcRpcPrepDataFillerTool::L1TgcRpcPrepDataFillerTool(const std::string& type,
                                                             const std::string& name,
                                                             const IInterface* parent)
  : BlockFillerTool<void>(type, name, parent),
    m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool")
{
  declareProperty ("RpcPrepData", m_sgKey = "RPC_Measurements", "SG key for RpcPrepData");
  declareProperty ("MuonIdHelperTool", m_idHelper, "Muon IdHelperTool instance.");

  book().ignore(); // Avoid coverity warnings.
}



/** 
 * @brief Standard Gaudi initialize method.
 */
StatusCode
D3PD::L1TgcRpcPrepDataFillerTool::initialize()
{
  CHECK( m_idHelper.retrieve() );
  return StatusCode::SUCCESS;
}


StatusCode
D3PD::L1TgcRpcPrepDataFillerTool::book()
{
  CHECK(addVariable("prd_x", m_prd_x));
  CHECK(addVariable("prd_y", m_prd_y));
  CHECK(addVariable("prd_z", m_prd_z));
  CHECK(addVariable("prd_time", m_prd_time));
  CHECK(addVariable("prd_triggerInfo", m_prd_triggerInfo));
  CHECK(addVariable("prd_ambiguityFlag", m_prd_ambiguityFlag));
  CHECK(addVariable("prd_measuresPhi", m_prd_measuresPhi));
  CHECK(addVariable("prd_inRibs", m_prd_inRibs));
  CHECK(addVariable("prd_station" , m_prd_station));
  CHECK(addVariable("prd_stationEta" , m_prd_stationEta));
  CHECK(addVariable("prd_stationPhi" , m_prd_stationPhi));
  CHECK(addVariable("prd_doubletR", m_prd_doubletR));
  CHECK(addVariable("prd_doubletZ", m_prd_doubletZ));
  CHECK(addVariable("prd_stripWidth", m_prd_stripWidth));
  CHECK(addVariable("prd_stripLength", m_prd_stripLength));
  CHECK(addVariable("prd_stripPitch", m_prd_stripPitch));

  return StatusCode::SUCCESS;
}



StatusCode
D3PD::L1TgcRpcPrepDataFillerTool::fill()
{
  const RpcIdHelper& rpcIdHelper = m_idHelper->rpcIdHelper();

  const Muon::RpcPrepDataContainer* container = 0;
  CHECK(evtStore()->retrieve(container, m_sgKey));

  Muon::RpcPrepDataContainer::const_iterator cit_cont;
  Muon::RpcPrepDataCollection::const_iterator cit_coll;
  for (cit_cont = container->begin(); cit_cont != container->end(); cit_cont++) {
    for (cit_coll = (*cit_cont)->begin(); cit_coll != (*cit_cont)->end(); cit_coll++) {
      const Muon::RpcPrepData* data = *cit_coll;
      const Amg::Vector3D& globalPos = data->globalPosition();

      m_prd_x->push_back(globalPos.x());
      m_prd_y->push_back(globalPos.y());
      m_prd_z->push_back(globalPos.z());
      m_prd_time->push_back(data->time());
      m_prd_triggerInfo->push_back(data->triggerInfo());
      m_prd_ambiguityFlag->push_back(data->ambiguityFlag());

      const MuonGM::RpcReadoutElement* element = data->detectorElement();
      const Identifier id = data->identify();
      int isPhi = rpcIdHelper.measuresPhi(id);

      m_prd_measuresPhi->push_back(isPhi);
      m_prd_inRibs->push_back(element->inTheRibs());
      m_prd_station->push_back(rpcIdHelper.stationName(id));
      m_prd_stationEta->push_back(element->getStationEta());
      m_prd_stationPhi->push_back(element->getStationPhi());
      m_prd_doubletR->push_back(element->getDoubletR());
      m_prd_doubletZ->push_back(element->getDoubletZ());
      m_prd_stripWidth->push_back(element->StripWidth(isPhi));
      m_prd_stripLength->push_back(element->StripLength(isPhi));
      m_prd_stripPitch->push_back(element->StripPitch(isPhi));
    }
  }

  return StatusCode::SUCCESS;
}
/* eof */
