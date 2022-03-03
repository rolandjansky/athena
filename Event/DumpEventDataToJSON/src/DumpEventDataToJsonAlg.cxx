/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/Algorithm.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"

#include "DumpEventDataToJsonAlg.h"

#include <string>
#include <fstream>
#include <cmath>

DumpEventDataToJsonAlg::DumpEventDataToJsonAlg(const std::string &name,
                                               ISvcLocator *pSvcLocator)
    : AthAlgorithm(name, pSvcLocator)
{
}

StatusCode DumpEventDataToJsonAlg::initialize()
{
  ATH_CHECK(m_eventInfoKey.initialize());
  ATH_CHECK(m_trackParticleKeys.initialize());
  ATH_CHECK(m_jetKeys.initialize());
  ATH_CHECK(m_caloClustersKeys.initialize());
  ATH_CHECK(m_caloCellKey.initialize());
  ATH_CHECK(m_muonKeys.initialize());
  ATH_CHECK(m_trackCollectionKeys.initialize());

  ATH_CHECK(m_cscPrepRawDataKey.initialize());
  ATH_CHECK(m_mdtPrepRawDataKey.initialize());
  ATH_CHECK(m_rpcPrepRawDataKey.initialize());
  ATH_CHECK(m_tgcPrepRawDataKey.initialize());
  ATH_CHECK(m_pixelPrepRawDataKey.initialize());
  ATH_CHECK(m_sctPrepRawDataKey.initialize());
  ATH_CHECK(m_trtPrepRawDataKey.initialize());


  if (m_extrapolateTrackParticless)
  {
    ATH_CHECK(m_extrapolator.retrieve());
  }
  else
  {
    m_extrapolator.disable();
  }
  return StatusCode::SUCCESS;
}

StatusCode DumpEventDataToJsonAlg::execute()
{
  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey);
  if (!eventInfo.isValid())
  {
    ATH_MSG_WARNING("Did not find xAOD::EventInfo at " << m_eventInfoKey);
    return StatusCode::SUCCESS;
  }

  if (m_dumpTestEvent)
  {
    prependTestEvent();
    m_dumpTestEvent = false;
  }

  ATH_MSG_VERBOSE("Run num :" << eventInfo->runNumber() << " Event num: " << eventInfo->eventNumber());

  nlohmann::json j;
  j["event number"] = eventInfo->eventNumber();
  j["run number"] = eventInfo->runNumber();

  ATH_CHECK(getAndFillArrayOfContainers(j, m_jetKeys, "Jets"));
  ATH_CHECK(getAndFillArrayOfContainers(j, m_trackParticleKeys, "Tracks"));
  ATH_CHECK(getAndFillArrayOfContainers(j, m_muonKeys, "Muons"));
  ATH_CHECK(getAndFillArrayOfContainers(j, m_caloClustersKeys, "CaloClusters"));
  ATH_CHECK(getAndFillArrayOfContainers(j, m_caloCellKey, "CaloCells"));
  ATH_CHECK(getAndFillArrayOfContainers(j, m_trackCollectionKeys, "Tracks"));

  // hits
  ATH_CHECK(getAndFillContainer(j, m_cscPrepRawDataKey, "Hits"));
  ATH_CHECK(getAndFillContainer(j, m_mdtPrepRawDataKey, "Hits"));
  ATH_CHECK(getAndFillContainer(j, m_rpcPrepRawDataKey, "Hits"));
  ATH_CHECK(getAndFillContainer(j, m_tgcPrepRawDataKey, "Hits"));
  ATH_CHECK(getAndFillContainer(j, m_pixelPrepRawDataKey, "Hits"));
  ATH_CHECK(getAndFillContainer(j, m_sctPrepRawDataKey, "Hits"));
  // ATH_CHECK(getAndFillContainer(j, m_trtPrepRawDataKey, "Hits")); // Need specialisation. TODO.

  // For the moment the label is just the event/run number again, but can be manually overwritten in the output file
  std::string label = std::to_string(eventInfo->eventNumber()) + "/" + std::to_string(eventInfo->runNumber());
  m_eventData[label] = j;

  return StatusCode::SUCCESS;
}

void DumpEventDataToJsonAlg::prependTestEvent()
{
  ATH_MSG_VERBOSE("Prepending a test event.");
  nlohmann::json j;

  // FIXME - this
  auto writeEtaPhiLabel = [](float eta, float phi) { return std::to_string(eta) + "/" + std::to_string(phi); };

  j["event number"] = 999;
  j["run number"] = 999;

  // Here we want to draw some tracks at predefined positons
  unsigned int maxSteps = 3;
  Amg::Vector3D trackPos;
  float phi, eta;
  for (unsigned int nPhi = 0; nPhi < maxSteps; ++nPhi)
  {
    phi = static_cast<float>(nPhi) / static_cast<float>(maxSteps) * M_PI; // Want to range from 0 to M_PI
    for (unsigned int nEta = 0; nEta < maxSteps; ++nEta)
    {
      eta = static_cast<float>(nEta) / static_cast<float>(maxSteps) * 3.0; // Want to range from 0 to 3.0

      // Create a calo cluster at each value
      nlohmann::json cluster;
      cluster["phi"] = phi;
      cluster["eta"] = eta;
      cluster["energy"] = 999.9;
      cluster["label"] = writeEtaPhiLabel(eta, phi);

      j["CaloClusters"]["TestClusters"].push_back(cluster);

      // create a jet at each value
      nlohmann::json jet;
      jet["phi"] = phi;
      jet["eta"] = eta;
      jet["energy"] = 99999.9;
      jet["label"] = writeEtaPhiLabel(eta, phi);
      j["Jets"]["TestJets"].push_back(jet);

      nlohmann::json track;
      track["chi2"] = 0.0;
      track["dof"] = 0.0;

      double theta = 2 * std::atan(std::exp(-eta));
      // d0, z0, phi, theta, qOverP
      track["dparams"] = {0.0, 0.0, phi, theta, 0.0};
      // Add three positions (less than this might not count as a)
      for (unsigned int i = 0; i < 4; ++i)
      {
        Amg::setRThetaPhi(trackPos, i * 1000., theta, phi);
        track["pos"].push_back({trackPos.x(), trackPos.y(), trackPos.z()});
      }
      track["label"] = writeEtaPhiLabel(eta, phi);
      j["Tracks"]["TestTracks"].push_back(track);
    }
  }
  m_eventData["Test"] = j;
}

template <class TYPE>
StatusCode DumpEventDataToJsonAlg::getAndFillArrayOfContainers(nlohmann::json &event,
                                                               const SG::ReadHandleKeyArray<TYPE> &keys,
                                                               const std::string& jsonType)
{
  for (SG::ReadHandle<TYPE> handle : keys.makeHandles())
  {
    ATH_MSG_VERBOSE("Trying to load " << handle.key());
    ATH_CHECK(handle.isValid());
    ATH_MSG_VERBOSE("Got back " << handle->size());

    for (auto object : *handle)
    {
      nlohmann::json tmp = getData(*object);
      event[jsonType][handle.key()].push_back(tmp);
    }
  }
  return StatusCode::SUCCESS;
}

// Specialisation for Jets
template <>
nlohmann::json DumpEventDataToJsonAlg::getData(const xAOD::Jet &jet)
{
  nlohmann::json data;
  data["phi"] = jet.phi();
  data["eta"] = jet.eta();
  data["energy"] = jet.e();
  return data;
}

// Specialisation for CaloClusters
template <>
nlohmann::json DumpEventDataToJsonAlg::getData(const xAOD::CaloCluster &clust)
{
  nlohmann::json data;
  data["phi"] = clust.phi();
  data["eta"] = clust.eta();
  data["energy"] = clust.e();
  //data["etaSize"] = clust.getClusterEtaSize(); // empty
  //data["phiSize"] = clust.getClusterPhiSize(); // empty
  return data;
}

// Specialisation for CaloCells
template <>
nlohmann::json DumpEventDataToJsonAlg::getData(const CaloCell &cell)
{
  nlohmann::json data;
  data["phi"] = cell.phi();
  data["eta"] = cell.eta();
  data["energy"] = cell.e();
  //data["etaSize"] = clust.getClusterEtaSize(); // empty
  //data["phiSize"] = clust.getClusterPhiSize(); // empty
  return data;
}

// Specialisation for TracksParticles
template <>
nlohmann::json DumpEventDataToJsonAlg::getData(const xAOD::TrackParticle &tp)
{
  nlohmann::json data;
  data["chi2"] = tp.chiSquared();
  data["dof"] = tp.numberDoF();
  data["dparams"] = {tp.d0(), tp.z0(), tp.phi0(), tp.theta(), tp.qOverP()};

  if (!m_extrapolateTrackParticless)
  {
    data["pos"] = {{tp.perigeeParameters().position().x(), tp.perigeeParameters().position().y(), tp.perigeeParameters().position().z()}};
    for (unsigned int i = 0; i < tp.numberOfParameters(); ++i)
    {
      data["pos"].push_back({tp.parameterX(i), tp.parameterY(i), tp.parameterZ(i)});
    }
  }
  else
  {
    std::vector<Amg::Vector3D> positions;
    const Trk::Perigee &peri = tp.perigeeParameters();
    positions.push_back(Amg::Vector3D(peri.position().x(), peri.position().y(), peri.position().z()));

    Trk::CurvilinearParameters startParameters(peri.position(), peri.momentum(), peri.charge());
    Trk::ExtrapolationCell<Trk::TrackParameters> ecc(startParameters);
    ecc.addConfigurationMode(Trk::ExtrapolationMode::StopAtBoundary);
    ecc.addConfigurationMode(Trk::ExtrapolationMode::CollectPassive);
    ecc.addConfigurationMode(Trk::ExtrapolationMode::CollectSensitive);
    Trk::ExtrapolationCode eCode = m_extrapolator->extrapolate(ecc);
    if (eCode.isSuccess())
    {
      // loop over the collected information
      for (auto &es : ecc.extrapolationSteps)
      {

        // continue if we have parameters
        const Trk::TrackParameters *parameters = es.parameters;
        if (parameters)
        {
          Amg::Vector3D pos = parameters->position();
          positions.push_back(pos);
          delete parameters;
        }
      }
      positions.push_back(ecc.endParameters->position());

      // Now add the positions to the output
      for (auto pos : positions)
      {
        data["pos"].push_back({pos.x(), pos.y(), pos.z()});
      }
    }
    else
    {
      ATH_MSG_WARNING("Failure in extrapolation for Track with start parameters " << startParameters);
    }
  }
  return data;
}

// Specialisation for Tracks
template <>
nlohmann::json DumpEventDataToJsonAlg::getData(const Trk::Track &track)
{
  nlohmann::json data;
  const Trk::FitQuality *quality = track.fitQuality();

  data["chi2"] = (quality ? quality->chiSquared() : 0.0);
  data["dof"] = (quality ? quality->doubleNumberDoF() : 0.0);

  const Trk::Perigee *peri = track.perigeeParameters();
  if (peri)
  {
    data["dparams"] = {peri->parameters()[Trk::d0], peri->parameters()[Trk::z0], peri->parameters()[Trk::phi0], peri->parameters()[Trk::theta], peri->parameters()[Trk::qOverP]};

    data["pos"] = {{peri->position().x(), peri->position().y(), peri->position().z()}};
  }
  else
  {
    data["pos"] = {{}};
  }

  const DataVector<const Trk::TrackParameters> *parameters = track.trackParameters();
  if (parameters)
  {
    for (const Trk::TrackParameters *param : *parameters)
    {
      data["pos"].push_back({param->position().x(), param->position().y(), param->position().z()});
    }
  }
  else
  {
    const DataVector<const Trk::MeasurementBase> *measurements = track.measurementsOnTrack();
    if (measurements)
    {
      for (const Trk::MeasurementBase *meas : *measurements)
      {
        data["pos"].push_back({meas->globalPosition().x(), meas->globalPosition().y(), meas->globalPosition().z()});
      }
    }
  }

  return data;
}

// Specialisation for Muons
template <>
nlohmann::json DumpEventDataToJsonAlg::getData(const xAOD::Muon &muon)
{
  nlohmann::json data;
  data["Phi"] = muon.phi();
  data["Eta"] = muon.eta();

  std::vector<std::string> quality = {"Tight", "Medium", "Loose", "VeryLoose"};
  data["Quality"] = quality[static_cast<unsigned int>(muon.quality())];
  std::vector<std::string> type = {"Combined", "Standalone", "SegmentTagged", "CaloTagged", "SiAssociatedForward"};
  data["Type"] = type[static_cast<unsigned int>(muon.muonType())];
  data["PassedHighPt"] = muon.passesHighPtCuts();

  addLink(muon.clusterLink(), data["LinkedClusters"]);
  addLink(muon.inDetTrackParticleLink(), data["LinkedTracks"]);
  addLink(muon.muonSpectrometerTrackParticleLink(), data["LinkedTracks"]);
  addLink(muon.extrapolatedMuonSpectrometerTrackParticleLink(), data["LinkedTracks"]);

  return data;
}

template <class TYPE>
void DumpEventDataToJsonAlg::addLink(const TYPE &link, nlohmann::json &data)
{
  if (link.isValid())
  {
    data.push_back(link.dataID() + ":" + std::to_string(link.index()));
  }
}

StatusCode DumpEventDataToJsonAlg::finalize()
{
  std::ofstream outputFile(m_outputJSON_Name);
  if (!outputFile.is_open())
  {
    ATH_MSG_WARNING("Unable to open " << m_outputJSON_Name << " for writing.");
    return StatusCode::FAILURE;
  }
  outputFile << m_eventData;
  return StatusCode::SUCCESS;
}

template <class TYPE>
StatusCode DumpEventDataToJsonAlg::getAndFillContainer(nlohmann::json &event,
                                                       const SG::ReadHandleKey<TYPE> &key,
                                                       const std::string& jsonType)
{
SG::ReadHandle<TYPE> handle(key);

ATH_MSG_VERBOSE("Trying to load " << handle.key());
ATH_CHECK(handle.isValid());
ATH_MSG_VERBOSE("Got back " << handle->size());

nlohmann::json tmp = getData(*handle);
event[jsonType][handle.key()]=tmp;

return StatusCode::SUCCESS;
}

// Generic PRD
template <class TYPE>
nlohmann::json DumpEventDataToJsonAlg::getData(const TYPE &container)
{

  nlohmann::json colldata;
  for (const auto& coll : container ) {
    for (const auto& prd : *coll ) {
      nlohmann::json data;
      data["pos"] = {prd->globalPosition().x(), prd->globalPosition().y(), prd->globalPosition().z()};
      Identifier id = prd->identify();
      data["id"] = id.get_compact();
      colldata.push_back(data);
    }
  }

  return colldata;
}

