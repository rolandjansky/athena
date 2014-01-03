/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/errorcheck.h"

#include "TrigMuonCoinHierarchy/IMuonRoiChainSvc.h"
#include "TrigMuonCoinHierarchy/ITgcCoinHierarchySvc.h"

#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"
#include "AnalysisTriggerEvent/LVL1_ROI.h"

#include "MuonReadoutGeometry/TgcReadoutElement.h"

#include "TrigEffJpsiTools/ITrigEffJpsiTools.h"

#include "L1TgcCoinHierarchyFillerTool.h"



D3PD::L1TgcCoinHierarchyFillerTool::L1TgcCoinHierarchyFillerTool(const std::string& type,
                                                                 const std::string& name,
                                                                 const IInterface* parent)
  : BlockFillerTool<void>(type, name, parent),
    m_muonRoiChainSvc("Trigger::MuonRoiChainSvc", name),
    m_tgcCoinHierarchySvc("Trigger::TgcCoinHierarchySvc", name)
{
  declareProperty ("Lvl1_RoiName", m_l1RoiKey = "LVL1_ROI", "Level1 RoI container name.");

  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode
D3PD::L1TgcCoinHierarchyFillerTool::initialize()
{
  CHECK(m_muonRoiChainSvc.retrieve());
  CHECK(m_tgcCoinHierarchySvc.retrieve());
  return StatusCode::SUCCESS;
}


StatusCode
D3PD::L1TgcCoinHierarchyFillerTool::book()
{
  CHECK(addVariable("index", m_index));   // index of MuonROI
  CHECK(addVariable("dR_hiPt", m_deltaR_hiPt));
  CHECK(addVariable("dPhi_hiPt", m_deltaPhi_hiPt));
  CHECK(addVariable("dR_tracklet", m_deltaR_tracklet));
  CHECK(addVariable("dPhi_tracklet", m_deltaPhi_tracklet));
  CHECK(addVariable("isChamberBoundary", m_isInEndcapChamberBoundary));

  return StatusCode::SUCCESS;
}



StatusCode
D3PD::L1TgcCoinHierarchyFillerTool::fill()
{
  const LVL1_ROI* lvl1Roi = 0;
  CHECK(evtStore()->retrieve(lvl1Roi, m_l1RoiKey));
  const LVL1_ROI::muons_type muonRoi = lvl1Roi->getMuonROIs();

  int index = 0;
  typedef LVL1_ROI::muons_type::const_iterator cit_roi;
  for (cit_roi cit = muonRoi.begin(); cit != muonRoi.end(); cit++) {
    const Muon_ROI roi = *cit;
    const Muon::TgcCoinData* tgcHiptWire = 0;
    const Muon::TgcCoinData* tgcHiptStrip = 0;
    const Muon::TgcCoinData* tgcTrackletWire = 0;
    const Muon::TgcCoinData* tgcTrackletStrip = 0;
    int tgcIsInEndcapChamberBoundary = -1;
    
    const Trigger::MuonRoiChain* chain = 0;
    StatusCode sc = m_muonRoiChainSvc->getChain(&chain, &roi);
    if (sc.isSuccess() and chain) {
      tgcIsInEndcapChamberBoundary = static_cast<int>(chain->isInEndcapChamberBoundary());

      const Muon::TgcCoinData* tgcSL = chain->getTgcCoinData();
      if (tgcSL) {
        std::vector<const Trigger::TgcCoinHierarchy*> hierarchies;
        sc = m_tgcCoinHierarchySvc->getHierarchy(&hierarchies, tgcSL);
        if (sc.isSuccess() and hierarchies.size() == 1) {
          const Trigger::TgcCoinHierarchy* hierarchy = hierarchies.at(0);
          tgcHiptWire = hierarchy->getCoincidence(Muon::TgcCoinData::TYPE_HIPT, false);
          tgcHiptStrip = hierarchy->getCoincidence(Muon::TgcCoinData::TYPE_HIPT, true);
          tgcTrackletWire = hierarchy->getCoincidence(Muon::TgcCoinData::TYPE_TRACKLET, false);
          tgcTrackletStrip = hierarchy->getCoincidence(Muon::TgcCoinData::TYPE_TRACKLET, true);
        }
      }
    }

    m_index->push_back(index++);
    m_deltaR_hiPt->push_back(tgcHiptWire ? tgcHiptWire->delta() : -31);
    m_deltaPhi_hiPt->push_back(tgcHiptStrip ? tgcHiptStrip->delta() : -31);
    m_deltaR_tracklet->push_back(tgcTrackletWire ? tgcTrackletWire->delta() : -31);
    m_deltaPhi_tracklet->push_back(tgcTrackletStrip ? tgcTrackletStrip->delta() : -31);
    m_isInEndcapChamberBoundary->push_back(tgcIsInEndcapChamberBoundary);
  }


  return StatusCode::SUCCESS;
}
/* eof */
