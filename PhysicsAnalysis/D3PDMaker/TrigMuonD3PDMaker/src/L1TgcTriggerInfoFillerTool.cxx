/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/errorcheck.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigDecisionTool/ChainGroup.h"

#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"

#include "L1TgcTriggerInfoFillerTool.h"



D3PD::L1TgcTriggerInfoFillerTool::L1TgcTriggerInfoFillerTool(const std::string& type,
                                                             const std::string& name,
                                                             const IInterface* parent)
  : BlockFillerTool<void>(type, name, parent),
    m_tdt("Trig::TrigDecisionTool/TrigDecisionTool")
{
  declareProperty("TriggerPattern", m_pattern= "(EF_mu[0-9]+i?((_loose)|(_medium)|(_tight))?) | "
                                               "(EF_mu[0-9]+_MG((_loose)|(_medium)|(_tight))?) | "
                                               "(EF_mu[0-9]+_Trk_Jpsi((_loose)|(_medium)|(_tight))?)",
                                    "regular expression for triggers");
  // TODO: handle EF_mu[0-9]+_Trk_Jpsi((_loose)|(_medium)|(_tight))?) correctly

  book().ignore(); // Avoid coverity warnings.
}



/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode
D3PD::L1TgcTriggerInfoFillerTool::initialize()
{
  CHECK(m_tdt.retrieve());
  return StatusCode::SUCCESS;
}


StatusCode
D3PD::L1TgcTriggerInfoFillerTool::book()
{
  CHECK(addVariable("chain", m_chain));
  CHECK(addVariable("isPassed", m_isPassed));
  CHECK(addVariable("nTracks", m_nTracks));
  CHECK(addVariable("typeVec", m_typeVec));
  CHECK(addVariable("ptVec", m_ptVec));
  CHECK(addVariable("etaVec", m_etaVec));
  CHECK(addVariable("phiVec", m_phiVec));
  CHECK(addVariable("chargeVec", m_qVec));
  CHECK(addVariable("l1RoiWordVec", m_l1RoiWordVec));

  return StatusCode::SUCCESS;
}



StatusCode
D3PD::L1TgcTriggerInfoFillerTool::fill()
{
  const Trig::ChainGroup* cg = m_tdt->getChainGroup(m_pattern);

  std::vector<std::string> chains = cg->getListOfTriggers();

  for (size_t ii = 0; ii < chains.size(); ii++) {
    const std::string chain = chains.at(ii);
    ATH_MSG_DEBUG(chain);

    size_t nTracks = 0;
    std::vector<int> typeVec, qVec, l1RoiWordVec;
    std::vector<float> ptVec, etaVec, phiVec;

    const Trig::FeatureContainer fc = m_tdt->features(chain);
    std::vector<Trig::Combination>::const_iterator cit_combination;
    ATH_MSG_DEBUG("combinations = " << fc.getCombinations().size());
    for (cit_combination = fc.getCombinations().begin(); cit_combination != fc.getCombinations().end(); cit_combination++) {
      const Trig::Combination& combination = *cit_combination;
      if (not combination.active()) continue;
//      for (std::vector<const HLT::TriggerElement*>::const_iterator tes = combination.tes().begin(); tes != combination.tes().end(); tes++) {
//        ATH_MSG_WARNING("name = " << Trig::getTEName(**tes) << " " << (*tes)->getActiveState());
//      }


      std::vector<Trig::Feature<TrigMuonEFInfo> > infos = combination.get<TrigMuonEFInfo>();
      std::vector<Trig::Feature<TrigMuonEFInfo> >::const_iterator cit;
      for (cit = infos.begin(); cit != infos.end(); cit++) {
        const TrigMuonEFInfo* muonEF = cit->cptr();
        const TrigMuonEFInfoTrackContainer* tracks = muonEF->TrackContainer();
        Trig::Feature<Muon_ROI> l1roi = m_tdt->ancestor<Muon_ROI>(*cit);
        const Muon_ROI* l1muon = l1roi.cptr();
        ATH_MSG_DEBUG("chain = " << chain << " name = " << Trig::getTEName(*cit->te()) << ":"
                      << (cit->te())->getActiveState() << " tracks = " << tracks->size());
        TrigMuonEFInfoTrackContainer::const_iterator trk;
        for (trk = tracks->begin(); trk != tracks->end(); trk++) {
          const TrigMuonEFInfoTrack* efinfo = *trk;

          int type = 0;
          TrigMuonEFTrack* tp = 0;
          if (chain.find("MSonly") != std::string::npos) {
            if (efinfo->hasSpectrometerTrack()) {
              tp = efinfo->SpectrometerTrack();
              type = efinfo->MuonType() + STANDALONE;
            }

          } else  {
            if (efinfo->hasCombinedTrack()) {
                tp = efinfo->CombinedTrack();
                type = efinfo->MuonType() + COMBINED;

            } else if (efinfo->hasSpectrometerTrack()) {
              tp = efinfo->SpectrometerTrack();
              type = efinfo->MuonType() + STANDALONE;

            } else if (efinfo->hasExtrapolatedTrack()) {
              tp = efinfo->ExtrapolatedTrack();
              type = efinfo->MuonType() + EXTRAPOLATED;
            }
          }

          if (tp == 0) continue;
          ATH_MSG_DEBUG("filing");

          nTracks++;
          typeVec.push_back(type);
          ptVec.push_back(tp->pt());
          etaVec.push_back(tp->eta());
          phiVec.push_back(tp->phi());
          qVec.push_back((tp->charge() < 0.) ? -1 : 1);
          l1RoiWordVec.push_back(l1muon ? l1muon->getROIWord() : -1);
        }
      }
    }

    m_chain->push_back(chain);
    m_isPassed->push_back(m_tdt->isPassed(chain));
    m_nTracks->push_back(nTracks);
    m_typeVec->push_back(typeVec);
    m_ptVec->push_back(ptVec);
    m_etaVec->push_back(etaVec);
    m_phiVec->push_back(phiVec);
    m_qVec->push_back(qVec);
    m_l1RoiWordVec->push_back(l1RoiWordVec);
  }

  return StatusCode::SUCCESS;
}
/* eof */
