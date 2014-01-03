/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/errorcheck.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonPrepRawData/TgcPrepData.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonDigitContainer/TgcDigit.h"

#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"

#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

#include "TrigEffJpsiTools/ITrigEffJpsiTools.h"

#include "L1TgcTrkExtrapolator.h"
#include "L1TgcTrkExtrapolationFillerTool.h"


using HepGeom::Point3D;


const double D3PD::L1TgcTrkExtrapolationFillerTool::M1_Z = 13605.0;
const double D3PD::L1TgcTrkExtrapolationFillerTool::M2_Z = 14860.0;
const double D3PD::L1TgcTrkExtrapolationFillerTool::M3_Z = 15280.0;
const double D3PD::L1TgcTrkExtrapolationFillerTool::EI_Z  = 7425.0;
const double D3PD::L1TgcTrkExtrapolationFillerTool::FI_Z  = 7030.0;
const double D3PD::L1TgcTrkExtrapolationFillerTool::APPROX_RPC2_R_LO = 7000.;
const double D3PD::L1TgcTrkExtrapolationFillerTool::APPROX_RPC2_R_HI = 9000.;

D3PD::L1TgcTrkExtrapolationFillerTool::L1TgcTrkExtrapolationFillerTool(const std::string& type,
                                                                       const std::string& name,
                                                                       const IInterface* parent)
  : BlockFillerTool<void>(type, name, parent),
    m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_trigEffJpsiTools("TrigEffJpsiTools"),
    m_l1TgcTrkExtrapolator("D3PD::L1TgcTrkExtrapolator")
{
  declareProperty ("TgcPrepDataPrevious", m_tgcPrepDataPrv = "TGC_MeasurementsPriorBC", "SG key for TgcPrepData in previous bunch");
  declareProperty ("TgcPrepDataCurrent", m_tgcPrepDataCur = "TGC_Measurements", "SG key for TgcPrepData in current bunch");
  declareProperty ("TgcPrepDataNext", m_tgcPrepDataNxt = "TGC_MeasurementsNextBC", "SG key for TgcPrepData in next bunch");
  declareProperty ("RpcPrepData", m_rpcPrepData = "RPC_Measurements", "SG key for RpcPrepData");
  declareProperty ("MuonContainer", m_muonKey = "StacoMuonCollection", "SG key for Muon");
  declareProperty ("MuonIdHelperTool", m_idHelper, "Muon IdHelperTool instance.");
  declareProperty ("ExtrapolationMethod", m_extrapolationMethod = "BIASED", "method of track extrapolation for alignmnet study.");
  declareProperty ("OnlyToTriggerPlane", m_onlyToTriggerPlane = false, "flag used for NTUP_TRIGMU to activate extrapolation only for detector plane.");

  book().ignore(); // Avoid coverity warnings.
}



/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode
D3PD::L1TgcTrkExtrapolationFillerTool::initialize()
{
  CHECK(m_idHelper.retrieve());
  CHECK(m_trigEffJpsiTools.retrieve());
  CHECK(m_l1TgcTrkExtrapolator.retrieve());
  return StatusCode::SUCCESS;
}


StatusCode
D3PD::L1TgcTrkExtrapolationFillerTool::book()
{

  CHECK(addVariable("type", m_type));
  CHECK(addVariable("index", m_index)); // index of Muon
  CHECK(addVariable("size", m_size));

  CHECK(addVariable("targetVec", m_targetVec));
  CHECK(addVariable("targetDistanceVec", m_targetDistanceVec));
  CHECK(addVariable("targetEtaVec", m_targetEtaVec));
  CHECK(addVariable("targetPhiVec", m_targetPhiVec));
  CHECK(addVariable("targetDeltaEtaVec", m_targetDeltaEtaVec));
  CHECK(addVariable("targetDeltaPhiVec", m_targetDeltaPhiVec));
  CHECK(addVariable("targetPxVec", m_targetPxVec));
  CHECK(addVariable("targetPyVec", m_targetPyVec));
  CHECK(addVariable("targetPzVec", m_targetPzVec));

  if (m_extrapolationMethod == "BIASED") {
    m_extrapolationType = BIASED;

  } else if (m_extrapolationMethod == "UNBIASED" ) {
    m_extrapolationType = UNBIASED;

  } else {
    ATH_MSG_ERROR("unknown method specified: " << m_extrapolationMethod);
    return StatusCode::FAILURE;
    
  }

  return StatusCode::SUCCESS;
}



StatusCode
D3PD::L1TgcTrkExtrapolationFillerTool::fill()
{
  Target target;
  CHECK(fillTargetDistanceTgc(m_tgcPrepDataPrv, target));
  CHECK(fillTargetDistanceTgc(m_tgcPrepDataCur, target));
  CHECK(fillTargetDistanceTgc(m_tgcPrepDataNxt, target));

  CHECK(fillTargetDistanceRpc(m_rpcPrepData, target));

  CHECK(fillExtrapolation(target));

  return StatusCode::SUCCESS;
}



StatusCode
D3PD::L1TgcTrkExtrapolationFillerTool::fillTargetDistanceTgc(const std::string& key,
                                                             Target& target)
{

  //T.Kanno: for NTUP_TRIGMU. Only add trigger plane (3rd TGC) and return.
  if( m_onlyToTriggerPlane ){
    if( key==m_tgcPrepDataCur ){
      Amg::Vector3D m3a(0, 0, M3_Z);
      target.insert(Target::value_type(ITrigEffJpsiTools::TGC, m3a));
    }
    return StatusCode::SUCCESS;
  }
  //T.K

  const Muon::TgcPrepDataContainer* container = 0;
  CHECK(evtStore()->retrieve(container, key));

  const TgcIdHelper& tgcIdHelper = m_idHelper->tgcIdHelper();
  Muon::TgcPrepDataContainer::const_iterator tpdc_it;
  Muon::TgcPrepDataCollection::const_iterator cit;
  for (tpdc_it = container->begin(); tpdc_it != container->end(); tpdc_it++) {
    for (cit = (*tpdc_it)->begin(); cit != (*tpdc_it)->end(); cit++) {
      const Muon::TgcPrepData *data = *cit;
      const MuonGM::TgcReadoutElement* element = data->detectorElement();
      const Identifier id = data->identify();
      const int gasGap = tgcIdHelper.gasGap(id);
      const int channel = tgcIdHelper.channel(id);
      const bool isStrip = tgcIdHelper.isStrip(id);
      Amg::Vector3D pos = isStrip ? element->stripPos(gasGap, channel) : element->gangPos(gasGap, channel);
      target.insert(Target::value_type(ITrigEffJpsiTools::TGC, pos));
    }
  }

  // constant target
  Amg::Vector3D m1a(0, 0, M1_Z);
  target.insert(Target::value_type(ITrigEffJpsiTools::TGC, m1a));
  Amg::Vector3D m2a(0, 0, M2_Z);
  target.insert(Target::value_type(ITrigEffJpsiTools::TGC, m2a));
  Amg::Vector3D m3a(0, 0, M3_Z);
  target.insert(Target::value_type(ITrigEffJpsiTools::TGC, m3a));
  Amg::Vector3D eia(0, 0, EI_Z);
  target.insert(Target::value_type(ITrigEffJpsiTools::TGC, eia));
  Amg::Vector3D fia(0, 0, FI_Z);
  target.insert(Target::value_type(ITrigEffJpsiTools::TGC, fia));
  Amg::Vector3D m1c(0, 0, -M1_Z);
  target.insert(Target::value_type(ITrigEffJpsiTools::TGC, m1c));
  Amg::Vector3D m2c(0, 0, -M2_Z);
  target.insert(Target::value_type(ITrigEffJpsiTools::TGC, m2c));
  Amg::Vector3D m3c(0, 0, -M3_Z);
  target.insert(Target::value_type(ITrigEffJpsiTools::TGC, m3c));
  Amg::Vector3D eic(0, 0, -EI_Z);
  target.insert(Target::value_type(ITrigEffJpsiTools::TGC, eic));
  Amg::Vector3D fic(0, 0, -FI_Z);
  target.insert(Target::value_type(ITrigEffJpsiTools::TGC, fic));

  return StatusCode::SUCCESS;
}



StatusCode
D3PD::L1TgcTrkExtrapolationFillerTool::fillTargetDistanceRpc(const std::string& key,
                                                             Target& target)
{
  const Muon::RpcPrepDataContainer* container = 0;
  CHECK(evtStore()->retrieve(container, key));

  Muon::RpcPrepDataContainer::const_iterator tpdc_it;
  Muon::RpcPrepDataCollection::const_iterator cit;
  for (tpdc_it = container->begin(); tpdc_it != container->end(); tpdc_it++) {
    for (cit = (*tpdc_it)->begin(); cit != (*tpdc_it)->end(); cit++) {
      const Muon::RpcPrepData *data = *cit;
      Amg::Vector3D pos = data->globalPosition();
      //T.Kanno: for NTUP_TRIGMU. Only add prc hits which are in approx. at Middle layer
      if( m_onlyToTriggerPlane ){
        double x = pos.x();
        double y = pos.y();
        double distance = sqrt(x*x + y*y);
        if( distance <= APPROX_RPC2_R_LO || APPROX_RPC2_R_HI <= distance ){
          continue;
        }
      }
      //T.K
      target.insert(Target::value_type(ITrigEffJpsiTools::RPC, pos));
    }
  }

  return StatusCode::SUCCESS;
}



StatusCode
D3PD::L1TgcTrkExtrapolationFillerTool::fillExtrapolation(const Target& target)
{

  const Analysis::MuonContainer* container = 0;
  CHECK(evtStore()->retrieve(container, m_muonKey));

  int index = 0;
  Analysis::MuonContainer::const_iterator cit;
  for (cit = container->begin(); cit != container->end(); cit++) {
    const Analysis::Muon *muon = *cit;

    const Rec::TrackParticle* tp = 0;
    int type = -100;
    if (muon->hasCombinedMuonTrackParticle()) {
      type = COMBINED;
      if (m_extrapolationType == BIASED) {
        tp = muon->combinedMuonTrackParticle();

      } else if (m_extrapolationType == UNBIASED) {
        tp = muon->inDetTrackParticle();
      }

    } else if (muon->hasInnerExtrapolatedTrackParticle()) {
      tp = muon->innerExtrapolatedTrackParticle();
      type = INNER_EXTRAPOLATED;

    } else if (muon->hasInDetTrackParticle()) {
      tp = muon->inDetTrackParticle();
      type = INDET;
    }

    std::vector<int> vDetector;
    std::vector<float> vDistance, vEta, vPhi, vDeltaEta, vDeltaPhi, vPx, vPy, vPz;
    size_t nn = 0;
    if (tp) {
      typedef std::map<std::string, double> DistanceMap;
      DistanceMap distanceZ;
      DistanceMap distanceR;
      double distance = -1.e30;

      Target::const_iterator cit_target;
      for (cit_target = target.begin(); cit_target != target.end(); cit_target++) {
        std::stringstream ss;
        ss.setf(std::ios::scientific);
        std::pair<DistanceMap::iterator, bool> rc;

        bool extrapolate = false;
        int detector = -1;

        if (cit_target->first == ITrigEffJpsiTools::TGC) {
          extrapolate = ((cit_target->second.z() * tp->eta()) > 0.);
          detector = ITrigEffJpsiTools::TGC;
          distance = cit_target->second.z();
          ss << distance;
          rc = distanceZ.insert(DistanceMap::value_type(ss.str(), distance));

        } else if (cit_target->first == ITrigEffJpsiTools::RPC) {
          extrapolate = (fabs(tp->eta()) < 1.5);
          detector = ITrigEffJpsiTools::RPC;
          double x = cit_target->second.x();
          double y = cit_target->second.y();
          distance = sqrt(x*x + y*y);
          ss << distance;
          rc = distanceR.insert(DistanceMap::value_type(ss.str(), distance));
        }

        if (rc.second == false) {
          ATH_MSG_DEBUG("det = " << detector << " distance = " << distance << " skipped");
          continue;
        }

        //T.Kanno to enable further skimming,  move this two lines later.
        //vDetector.push_back(detector);
        //vDistance.push_back(distance);

        Amg::Vector2D etaDeta(-1.e30, -1.e30);
        Amg::Vector2D phiDphi(-1.e30, -1.e30);
        Amg::Vector3D mom(-1.e30, -1.e30, -1.e30);
        if (extrapolate) {
          if (m_extrapolationType == BIASED) {
            m_l1TgcTrkExtrapolator->extrapolateFromMDT(tp, cit_target->second, detector,
                                                       etaDeta, phiDphi, mom);

          } else if (m_extrapolationType == UNBIASED) {
            m_trigEffJpsiTools->extrapolateToPoint(tp, &cit_target->second, detector,
                                                   &etaDeta, &phiDphi, &mom);
          }
        } 

        //T.Kanno do further skimming.
        if( m_onlyToTriggerPlane ){
          if( etaDeta.x() == -1.e30 ) continue;
          if( phiDphi.x() == -1.e30 ) continue;
        }
        // T.K

        //T.Kanno detector, distance are now stored here
        vDetector.push_back(detector);
        vDistance.push_back(distance);

        vEta.push_back(etaDeta.x());
        vPhi.push_back(phiDphi.x());
        vDeltaEta.push_back(etaDeta.y());
        vDeltaPhi.push_back(phiDphi.y());
        vPx.push_back(mom.x());
        vPy.push_back(mom.y());
        vPz.push_back(mom.z());

        nn++;
      }
    }

    m_type->push_back(type);
    m_index->push_back(index++);

    m_size->push_back(nn);
    m_targetVec->push_back(vDetector);
    m_targetDistanceVec->push_back(vDistance);
    m_targetEtaVec->push_back(vEta);
    m_targetPhiVec->push_back(vPhi);
    m_targetDeltaEtaVec->push_back(vDeltaEta);
    m_targetDeltaPhiVec->push_back(vDeltaPhi);
    m_targetPxVec->push_back(vPx);
    m_targetPyVec->push_back(vPy);
    m_targetPzVec->push_back(vPz);
  }

  return StatusCode::SUCCESS;
}
/* eof */
