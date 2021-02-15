///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// IsolationBuilder.cxx
// Implementation file for class IsolationBuilder
///////////////////////////////////////////////////////////////////

// Isolation includes
#include "IsolationBuilder.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/Photon.h"

IsolationBuilder::IsolationBuilder(const std::string& name,
                                   ISvcLocator* pSvcLocator)
  : ::AthReentrantAlgorithm(name, pSvcLocator)
{}

IsolationBuilder::~IsolationBuilder() {}

StatusCode
IsolationBuilder::initialize()
{
  ATH_MSG_INFO("Initializing " << name() << "...");

  std::set<xAOD::Iso::IsolationFlavour> runIsoType;

  ATH_MSG_DEBUG("Initializing central electrons");
  ATH_CHECK(initializeIso(runIsoType,
                          &m_elCaloIso,
                          &m_elTrackIso,
                          m_ElectronContainerName,
                          m_elisoInts,
                          m_elcorInts,
                          m_elcorIntsExtra,
                          m_customConfigEl));

  ATH_MSG_DEBUG("Initializing central photons");
  ATH_CHECK(initializeIso(runIsoType,
                          &m_phCaloIso,
                          &m_phTrackIso,
                          m_PhotonContainerName,
                          m_phisoInts,
                          m_phcorInts,
                          m_phcorIntsExtra,
                          m_customConfigPh));

  ATH_MSG_DEBUG("Initializing forward electrons");
  ATH_CHECK(initializeIso(runIsoType,
                          &m_feCaloIso,
                          nullptr,
                          m_FwdElectronContainerName,
                          m_feisoInts,
                          m_fecorInts,
                          m_fecorIntsExtra,
                          m_customConfigFwd));

  ATH_MSG_DEBUG("Initializing muons");
  ATH_CHECK(initializeIso(runIsoType,
                          &m_muCaloIso,
                          &m_muTrackIso,
                          m_MuonContainerName,
                          m_muisoInts,
                          m_mucorInts,
                          m_mucorIntsExtra,
                          m_customConfigMu));

  // declare the dependencies
  // (need to do this since the WriteDecorHandleKeys are not properties
  declareIso(m_elCaloIso);
  declareIso(m_phCaloIso);
  declareIso(m_feCaloIso);
  declareIso(m_muCaloIso);

  declareIso(m_elTrackIso);
  declareIso(m_phTrackIso);
  declareIso(m_muTrackIso);

  // Retrieve the tools (there three Calo ones are the same in fact)
  if (!m_cellIsolationTool.empty() &&
      runIsoType.find(xAOD::Iso::etcone) != runIsoType.end()) {
    ATH_CHECK(m_cellIsolationTool.retrieve());
  } else {
    m_cellIsolationTool.disable();
  }

  if (!m_topoIsolationTool.empty() &&
      runIsoType.find(xAOD::Iso::topoetcone) != runIsoType.end()) {
    ATH_CHECK(m_topoIsolationTool.retrieve());
  } else {
    m_topoIsolationTool.disable();
  }

  if (!m_pflowIsolationTool.empty() &&
      runIsoType.find(xAOD::Iso::neflowisol) != runIsoType.end()) {
    ATH_CHECK(m_pflowIsolationTool.retrieve());
  } else {
    m_pflowIsolationTool.disable();
  }

  if (!m_trackIsolationTool.empty() &&
      runIsoType.find(xAOD::Iso::IsolationFlavour::ptcone) !=
        runIsoType.end()) {
    ATH_CHECK(m_trackIsolationTool.retrieve());
  } else {
    m_trackIsolationTool.disable();
  }

  // initialise data handles
  ATH_CHECK(m_cellsKey.initialize(!m_cellIsolationTool.empty()));

  return StatusCode::SUCCESS;
}

StatusCode
IsolationBuilder::finalize()
{
  ATH_MSG_INFO("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode
IsolationBuilder::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("Executing " << name() << "...");

  // For etcone, needs the cells

  const CaloCellContainer* cellColl = nullptr;
  if (!m_cellIsolationTool.empty()) {
    SG::ReadHandle<CaloCellContainer> cellcoll(m_cellsKey, ctx);
    // check is only used for serial running; remove when MT scheduler used
    if (!cellcoll.isValid()) {
      ATH_MSG_FATAL("Failed to retrieve cell container: " << m_cellsKey.key());
      return StatusCode::FAILURE;
    }
    cellColl = cellcoll.cptr();
  }
  // Compute isolations

  ATH_MSG_DEBUG("About to execute Electron calo iso");
  ATH_CHECK(executeCaloIso(m_elCaloIso, cellColl));
  ATH_MSG_DEBUG("About to execute Photon calo iso");
  ATH_CHECK(executeCaloIso(m_phCaloIso, cellColl));
  ATH_MSG_DEBUG("About to execute Forwerd Electron calo iso");
  ATH_CHECK(executeCaloIso(m_feCaloIso, cellColl));
  ATH_MSG_DEBUG("About to execute muon calo iso");
  ATH_CHECK(executeCaloIso(m_muCaloIso, cellColl));

  ATH_MSG_DEBUG("About to execute Electron track iso");
  ATH_CHECK(executeTrackIso(m_elTrackIso));
  ATH_MSG_DEBUG("About to execute Photon track iso");
  ATH_CHECK(executeTrackIso(m_phTrackIso));
  ATH_MSG_DEBUG("About to execute Muon track iso");
  ATH_CHECK(executeTrackIso(m_muTrackIso));

  return StatusCode::SUCCESS;
}

// constructor
IsolationBuilder::CaloIsoHelpKey::CaloIsoHelpKey(IDataHandleHolder* owningAlg)
{
  isoDeco.setOwner(owningAlg);
  corrBitsetDeco.setOwner(owningAlg);
}

// declare dependencies
void
IsolationBuilder::CaloIsoHelpKey::declare(IDataHandleHolder* owningAlg)
{
  isoDeco.declare(owningAlg);
  owningAlg->declare(corrBitsetDeco);

  for (auto& coreCor : coreCorDeco) {
    owningAlg->declare(coreCor.second);
  }

  for (auto& noncoreCor : noncoreCorDeco) {
    noncoreCor.second.declare(owningAlg);
  }
}

// constructor
IsolationBuilder::TrackIsoHelpKey::TrackIsoHelpKey(IDataHandleHolder* owningAlg)
{
  isoDeco.setOwner(owningAlg);
  isoDecoV.setOwner(owningAlg);
  corrBitsetDeco.setOwner(owningAlg);
}

// declare dependencies
void
IsolationBuilder::TrackIsoHelpKey::declare(IDataHandleHolder* owningAlg)
{
  isoDeco.declare(owningAlg);
  isoDecoV.declare(owningAlg);
  owningAlg->declare(corrBitsetDeco);

  for (auto& coreCor : coreCorDeco) {
    owningAlg->declare(coreCor.second);
  }
}

// constructor
IsolationBuilder::CaloIsoHelpHandles::CaloIsoHelpHandles(
  const IsolationBuilder::CaloIsoHelpKey& keys)
  : corrBitsetDeco(keys.corrBitsetDeco)
{
  for (const auto& key : keys.isoDeco) {
    isoDeco.emplace_back(key);
  }
  for (const auto& coreCor : keys.coreCorDeco) {
    coreCorDeco.emplace(coreCor);
  }
  for (const auto& noncoreCor : keys.noncoreCorDeco) {
    noncoreCorDeco.emplace(
      noncoreCor.first,
      std::vector<SG::WriteDecorHandle<xAOD::IParticleContainer, float>>{
        std::begin(noncoreCor.second), std::end(noncoreCor.second) });
  }
}

IsolationBuilder::TrackIsoHelpHandles::TrackIsoHelpHandles(
  const IsolationBuilder::TrackIsoHelpKey& keys)
  : corrBitsetDeco(keys.corrBitsetDeco)
{
  for (const auto& key : keys.isoDeco) {
    isoDeco.emplace_back(key);
  }
  for (const auto& key : keys.isoDecoV) {
    isoDecoV.emplace_back(key);
  }
  for (const auto& coreCor : keys.coreCorDeco) {
    coreCorDeco.emplace(coreCor);
  }
}

bool
IsolationBuilder::isCoreCor(xAOD::Iso::IsolationCaloCorrection cor)
{
  return (cor == xAOD::Iso::coreCone || cor == xAOD::Iso::coreConeSC ||
          cor == xAOD::Iso::coreMuon || cor == xAOD::Iso::core57cells);
}

StatusCode
IsolationBuilder::initializeIso(
  std::set<xAOD::Iso::IsolationFlavour>& runIsoType, // out
  std::vector<std::pair<xAOD::Iso::IsolationFlavour, CaloIsoHelpKey>>*
    caloIsoMap, // out
  std::vector<std::pair<xAOD::Iso::IsolationFlavour, TrackIsoHelpKey>>*
    trackIsoMap, // out
  const std::string& containerName,
  const std::vector<std::vector<int>>& isoInts,
  const std::vector<std::vector<int>>& corInts,
  const std::vector<std::vector<int>>& corIntsExtra,
  const std::string& customConfig)
{

  std::string prefix = containerName + ".";

  for (size_t flavor = 0; flavor < isoInts.size(); flavor++) {
    // iterate over the flavor (cell, topo, eflow, track
    //   Note: it is a configuration error if different types
    //         are included in one inner vector

    CaloIsoHelpKey cisoH(this);
    TrackIsoHelpKey tisoH(this);

    // std::vector<SG::AuxElement::Decorator<float>*> Deco;
    xAOD::Iso::IsolationFlavour isoFlav = xAOD::Iso::numIsolationFlavours;
    xAOD::Iso::IsolationFlavour oldIsoFlav = xAOD::Iso::numIsolationFlavours;

    for (size_t type = 0; type < isoInts[flavor].size(); type++) {
      // iterate over the cone sizes for a given flavor.
      // (also check that the cone sizes really are of the same flavor;
      // otherwise an error)

      xAOD::Iso::IsolationType isoType =
        static_cast<xAOD::Iso::IsolationType>(isoInts[flavor][type]);
      isoFlav = xAOD::Iso::isolationFlavour(isoType);
      ATH_MSG_DEBUG("Saw isoType " << isoType << " and isoFlav " << isoFlav);
      if (oldIsoFlav != xAOD::Iso::numIsolationFlavours &&
          oldIsoFlav != isoFlav) {
        ATH_MSG_FATAL("Configuration error:  can only have one type of "
                      "isolation in inner vector");
        return StatusCode::FAILURE;
      }
      oldIsoFlav = isoFlav;
      std::string isoName = prefix + xAOD::Iso::toString(isoType);
      if (!customConfig.empty()) {
        isoName += "_" + customConfig;
      }
      if (isoFlav == xAOD::Iso::etcone || isoFlav == xAOD::Iso::topoetcone ||
          isoFlav == xAOD::Iso::neflowisol) {
        cisoH.isoTypes.push_back(isoType);
        cisoH.isoDeco.emplace_back(isoName);
      } else if (isoFlav == xAOD::Iso::ptcone) {
        tisoH.isoTypes.push_back(isoType);
        tisoH.isoDeco.emplace_back(isoName);
        auto coneSize =
          static_cast<int>(round(100 * xAOD::Iso::coneSize(isoType)));
        std::string isoNameV = prefix + "ptvarcone" + std::to_string(coneSize);
        if (!customConfig.empty()) {
          isoNameV += "_" + customConfig;
        }
        tisoH.isoDecoV.emplace_back(isoNameV);
      } else {
        ATH_MSG_FATAL("Configuration error: Isolation flavor "
                      << isoFlav << " not supported.");
        return StatusCode::FAILURE;
      }
    }

    // check that there were isolations configured
    if (isoFlav == xAOD::Iso::numIsolationFlavours) {
      ATH_MSG_WARNING("The configuration was malformed: an empty inner vector "
                      "was added; ignoring");
      continue;
    }

    ///////////////////////////////
    // Now that the isolations to calculate are determined,
    // initialize the isolation decorations
    // and then determine the corrections to apply,
    // and finally add it to the IsoMap.
    ///////////////////////////////

    if (isoFlav == xAOD::Iso::etcone || isoFlav == xAOD::Iso::topoetcone ||
        isoFlav == xAOD::Iso::neflowisol) {

      // let's initialize the decos
      ATH_MSG_DEBUG("Initializing cisoH.isoDeco");
      ATH_CHECK(cisoH.isoDeco.initialize());

      ATH_CHECK(addCaloIsoCorrections(
        flavor, isoFlav, cisoH, corInts, false, prefix, customConfig));
      ATH_CHECK(addCaloIsoCorrections(
        flavor, isoFlav, cisoH, corIntsExtra, true, prefix, customConfig));

      if (caloIsoMap) {
        caloIsoMap->push_back(std::make_pair(isoFlav, cisoH));
      } else {
        ATH_MSG_FATAL(
          "caloIsoMap was nullptr but the configuration attempted to use it");
        return StatusCode::FAILURE;
      }
    } else if (isoFlav == xAOD::Iso::ptcone) {

      // let's initialize the decos
      ATH_MSG_DEBUG("Initializing tisoH.isoDeco");
      ATH_CHECK(tisoH.isoDeco.initialize());
      ATH_MSG_DEBUG("Initializing tisoH.isoDecoV");
      ATH_CHECK(tisoH.isoDecoV.initialize());

      ATH_CHECK(addTrackIsoCorrections(
        flavor, isoFlav, tisoH, corInts, false, prefix, customConfig));
      ATH_CHECK(addTrackIsoCorrections(
        flavor, isoFlav, tisoH, corIntsExtra, true, prefix, customConfig));

      if (trackIsoMap) {
        trackIsoMap->push_back(std::make_pair(isoFlav, tisoH));
      } else {
        ATH_MSG_FATAL(
          "trackIsoMap was nullptr but the configuration attempted to use it");
        return StatusCode::FAILURE;
      }
    } else {
      ATH_MSG_WARNING("Isolation flavour "
                      << xAOD::Iso::toCString(isoFlav)
                      << " does not exist ! Check your inputs");
    }
    runIsoType.insert(isoFlav);
  }
  return StatusCode::SUCCESS;
}

StatusCode
IsolationBuilder::addCaloIsoCorrections(
  size_t flavor,
  xAOD::Iso::IsolationFlavour isoFlav,
  CaloIsoHelpKey& cisoH,
  const std::vector<std::vector<int>>& corInts,
  bool corrsAreExtra,
  const std::string& prefix,
  const std::string& customConfig)
{

  if (!corrsAreExtra) {
    std::string bitsetName =
      prefix + xAOD::Iso::toString(isoFlav) + "CorrBitset";
    if (!customConfig.empty()) {
      bitsetName += "_" + customConfig;
    }

    cisoH.corrBitsetDeco = bitsetName;
    ATH_MSG_DEBUG("Initializing " << cisoH.corrBitsetDeco.key());
    ATH_CHECK(cisoH.corrBitsetDeco.initialize());
  }

  for (size_t corrType = 0; corrType < corInts[flavor].size(); corrType++) {

    // iterate over the calo isolation corrections
    const auto cor = static_cast<unsigned int>(corInts[flavor][corrType]);
    if (!corrsAreExtra)
      cisoH.CorrList.calobitset.set(cor);
    const xAOD::Iso::IsolationCaloCorrection isoCor =
      static_cast<xAOD::Iso::IsolationCaloCorrection>(cor);

    if (isCoreCor(isoCor)) {
      std::string isoCorName = prefix;

      if (isoCor != xAOD::Iso::core57cells) {
        isoCorName += xAOD::Iso::toString(
          isoFlav); // since this doesn't depend on the flavor, just have one
      }

      // a core correction; only store core energy, not the core area
      isoCorName += xAOD::Iso::toString(isoCor) +
                    xAOD::Iso::toString(xAOD::Iso::coreEnergy) + "Correction";
      if (!customConfig.empty()) {
        isoCorName += "_" + customConfig;
      }
      cisoH.coreCorDeco.emplace(isoCor, isoCorName);
      cisoH.coreCorDeco[isoCor].setOwner(this);
      ATH_MSG_DEBUG("initializing " << cisoH.coreCorDeco[isoCor].key());
      ATH_CHECK(cisoH.coreCorDeco[isoCor].initialize());
    } else if (isoCor == xAOD::Iso::pileupCorrection) {
      // do not store pileup corrections as they are rho * pi * (R**2 -
      // areaCore) and rho is stored...
      continue;
    } else {
      // noncore correction
      cisoH.noncoreCorDeco.emplace(
        isoCor, SG::WriteDecorHandleKeyArray<xAOD::IParticleContainer>());
      auto& vec = cisoH.noncoreCorDeco[isoCor];
      vec.setOwner(this);
      for (auto type : cisoH.isoTypes) {
        std::string corName = prefix + xAOD::Iso::toString(type) +
                              xAOD::Iso::toString(isoCor) + "Correction";
        if (!customConfig.empty()) {
          corName += "_" + customConfig;
        }
        vec.emplace_back(corName);
      }
      ATH_MSG_DEBUG("Initializing " << xAOD::Iso::toString(isoCor)
                                    << " Corrections");
      ATH_CHECK(vec.initialize());
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode
IsolationBuilder::addTrackIsoCorrections(
  size_t flavor,
  xAOD::Iso::IsolationFlavour isoFlav,
  TrackIsoHelpKey& tisoH,
  const std::vector<std::vector<int>>& corInts,
  bool corrsAreExtra,
  const std::string& prefix,
  const std::string& customConfig)
{

  if (!corrsAreExtra) {
    std::string bitsetName =
      prefix + xAOD::Iso::toString(isoFlav) + "CorrBitset";
    if (!customConfig.empty()) {
      bitsetName += "_" + customConfig;
    }

    tisoH.corrBitsetDeco = bitsetName;
    ATH_MSG_DEBUG("Initializing " << tisoH.corrBitsetDeco.key());
    ATH_CHECK(tisoH.corrBitsetDeco.initialize());
  }

  for (size_t corrType = 0; corrType < corInts[flavor].size(); corrType++) {
    const auto cor = static_cast<unsigned int>(corInts[flavor][corrType]);
    if (!corrsAreExtra)
      tisoH.CorrList.trackbitset.set(cor);
    const xAOD::Iso::IsolationTrackCorrection isoCor =
      static_cast<xAOD::Iso::IsolationTrackCorrection>(cor);

    // all pt corrections are core type
    std::string isoCorName = prefix + xAOD::Iso::toString(isoFlav) +
                             xAOD::Iso::toString(isoCor) + "Correction";

    if (!customConfig.empty()) {
      isoCorName += "_" + customConfig;
    }
    tisoH.coreCorDeco.emplace(isoCor, isoCorName);
    tisoH.coreCorDeco[isoCor].setOwner(this);
    ATH_MSG_DEBUG("initializing " << tisoH.coreCorDeco[isoCor].key());
    ATH_CHECK(tisoH.coreCorDeco[isoCor].initialize());
  }
  return StatusCode::SUCCESS;
}

StatusCode
IsolationBuilder::executeCaloIso(
  const std::vector<std::pair<xAOD::Iso::IsolationFlavour, CaloIsoHelpKey>>&
    caloIsoMap,
  const CaloCellContainer* cellColl) const
{
  for (const auto& pr : caloIsoMap) {

    const xAOD::Iso::IsolationFlavour flav = pr.first;
    const auto& keys = pr.second;
    CaloIsoHelpHandles handles(keys);

    ATH_MSG_DEBUG("Executing calo iso flavor: " << xAOD::Iso::toString(flav));

    if (handles.isoDeco.empty()) {
      ATH_MSG_FATAL("Have a CaloIsoHelpHandles with no actual isolations; "
                    "something wrong happened");
      return StatusCode::FAILURE;
    }
    auto& readHandle =
      handles.isoDeco[0]; // can treat the writeDecorHandle as a read handle;
    if (!readHandle.isValid()) {
      ATH_MSG_FATAL("Could not retrieve read handle for "
                    << keys.isoDeco[0].key());
      return StatusCode::FAILURE;
    }

    for (const auto *part : *readHandle) {
      xAOD::CaloIsolation CaloIsoResult;
      bool successfulCalc = false;
      if (flav == xAOD::Iso::IsolationFlavour::etcone && cellColl) {
        successfulCalc = m_cellIsolationTool->caloCellIsolation(
          CaloIsoResult, *part, keys.isoTypes, keys.CorrList, cellColl);
      } else if (flav == xAOD::Iso::IsolationFlavour::topoetcone) {
        successfulCalc = m_topoIsolationTool->caloTopoClusterIsolation(
          CaloIsoResult, *part, keys.isoTypes, keys.CorrList);
      } else if (flav == xAOD::Iso::IsolationFlavour::neflowisol) {
        successfulCalc = m_pflowIsolationTool->neutralEflowIsolation(
          CaloIsoResult, *part, keys.isoTypes, keys.CorrList);
      }

      if (successfulCalc) {
        for (unsigned int i = 0; i < keys.isoTypes.size(); i++) {
          float iso = CaloIsoResult.etcones[i];
          ATH_MSG_DEBUG("custom Iso " << xAOD::Iso::toCString(keys.isoTypes[i])
                                      << " = " << iso / 1e3);
          (handles.isoDeco[i])(*part) = iso;
        }
        // corrections
        (handles.corrBitsetDeco)(*part) = keys.CorrList.calobitset.to_ulong();

        // let's do the core corrections

        // iterate over the values we want to store
        for (auto& coreCorDecoPr : handles.coreCorDeco) {
          // find the matching result
          auto corIter =
            CaloIsoResult.coreCorrections.find(coreCorDecoPr.first);
          if (corIter == CaloIsoResult.coreCorrections.end()) {
            ATH_MSG_FATAL("Could not find core correction of required type: "
                          << xAOD::Iso::toCString(coreCorDecoPr.first));
            ATH_MSG_FATAL("Check configuration");
            return StatusCode::FAILURE;
          }
          // now that we have the match, let's find the energy
          std::map<xAOD::Iso::IsolationCorrectionParameter,
                   float>::const_iterator it =
            corIter->second.find(xAOD::Iso::coreEnergy);
          if (it == corIter->second.end()) {
            ATH_MSG_FATAL("Could not find coreEnergy correction for: "
                          << xAOD::Iso::toCString(coreCorDecoPr.first));
            ATH_MSG_FATAL("Check configuration");
            return StatusCode::FAILURE;
          }
          ATH_MSG_DEBUG("About to write core correction: "
                        << xAOD::Iso::toCString(coreCorDecoPr.first));
          (coreCorDecoPr.second)(*part) = it->second;
        }

        // let's do the noncore corrections
        for (auto& noncoreCorDecoPr : handles.noncoreCorDeco) {
          // find the matching result
          auto corIter =
            CaloIsoResult.noncoreCorrections.find(noncoreCorDecoPr.first);
          if (corIter == CaloIsoResult.noncoreCorrections.end()) {
            ATH_MSG_FATAL("Could not find noncore correction of required type: "
                          << xAOD::Iso::toCString(noncoreCorDecoPr.first));
            ATH_MSG_FATAL("Check configuration");
            return StatusCode::FAILURE;
          }

          ATH_MSG_DEBUG("About to write noncore correction: "
                        << xAOD::Iso::toCString(noncoreCorDecoPr.first));
          auto& vecHandles = noncoreCorDecoPr.second;
          for (size_t i = 0; i < vecHandles.size(); i++) {
            (vecHandles[i])(*part) = corIter->second[i];
          }
        }
      } else {
        ATH_MSG_FATAL("Call to CaloIsolationTool failed for flavor "
                      << xAOD::Iso::toCString(flav));
        return StatusCode::FAILURE;
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode
IsolationBuilder::executeTrackIso(
  const std::vector<std::pair<xAOD::Iso::IsolationFlavour, TrackIsoHelpKey>>&
    trackIsoMap) const
{
  for (const auto& pr : trackIsoMap) {
    const xAOD::Iso::IsolationFlavour flav = pr.first;
    const auto& keys = pr.second;
    TrackIsoHelpHandles handles(keys);

    ATH_MSG_DEBUG("Executing track iso flavor: " << xAOD::Iso::toString(flav));

    if (handles.isoDeco.empty()) {
      ATH_MSG_FATAL("Have a TrackIsoHelpHandles with no actual isolations; "
                    "something wrong happened");
      return StatusCode::FAILURE;
    }
    auto& readHandle =
      handles.isoDeco[0]; // can treat the writeDecorHandle as a read handle;
    if (!readHandle.isValid()) {
      ATH_MSG_FATAL("Could not retrieve read handle for "
                    << keys.isoDeco[0].key());
      return StatusCode::FAILURE;
    }

    for (const auto *part : *readHandle) {
      xAOD::TrackIsolation TrackIsoResult;
      bool successfulCalc = false;
      // check to see if we are dealing with an electron
      const auto * eg = dynamic_cast<const xAOD::Egamma*>(part);
      if (eg) {
        ATH_MSG_DEBUG("Doing track isolation on an egamma particle");
        std::set<const xAOD::TrackParticle*> tracksToExclude;
        if (xAOD::EgammaHelpers::isElectron(eg)) {
          tracksToExclude =
            xAOD::EgammaHelpers::getTrackParticles(eg, m_useBremAssoc);
        } else {
          if (m_allTrackRemoval) { // New (from ??/??/16) : now this gives all
                                   // tracks
            tracksToExclude =
              xAOD::EgammaHelpers::getTrackParticles(eg, m_useBremAssoc);
          } else { // this is just to be able to have the 2015+2016 default case
                   // (only tracks from first vertex)
            const auto * gam = dynamic_cast<const xAOD::Photon*>(eg);
            if (gam && gam->nVertices() > 0) {
              const xAOD::Vertex* phvtx = gam->vertex(0);
              for (unsigned int itk = 0; itk < phvtx->nTrackParticles(); itk++)
                tracksToExclude.insert(
                  m_useBremAssoc
                    ? xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(
                        phvtx->trackParticle(itk))
                    : phvtx->trackParticle(itk));
            }
          }
        }
        successfulCalc = m_trackIsolationTool->trackIsolation(TrackIsoResult,
                                                              *part,
                                                              keys.isoTypes,
                                                              keys.CorrList,
                                                              nullptr,
                                                              &tracksToExclude);
      } else {
        ATH_MSG_DEBUG("Not doing track isolation on an egamma particle");
        successfulCalc = m_trackIsolationTool->trackIsolation(
          TrackIsoResult, *part, keys.isoTypes, keys.CorrList);
      }

      if (successfulCalc) {
        for (unsigned int i = 0; i < keys.isoTypes.size(); i++) {
          float iso = TrackIsoResult.ptcones[i];
          float isoV = TrackIsoResult.ptvarcones_10GeVDivPt[i];
          ATH_MSG_DEBUG("custom Iso " << xAOD::Iso::toCString(keys.isoTypes[i])
                                      << " = " << iso / 1e3
                                      << ", var cone = " << isoV / 1e3);
          (handles.isoDeco[i])(*part) = iso;
          (handles.isoDecoV[i])(*part) = isoV;
        }

        // corrections
        (handles.corrBitsetDeco)(*part) = keys.CorrList.trackbitset.to_ulong();
        // let's do the core corrections
        // iterate over the values we want to store
        for (auto& coreCorDecoPr : handles.coreCorDeco) {
          // find the matching result
          auto corIter =
            TrackIsoResult.coreCorrections.find(coreCorDecoPr.first);
          if (corIter == TrackIsoResult.coreCorrections.end()) {
            ATH_MSG_FATAL("Could not find core correction of required type: "
                          << xAOD::Iso::toCString(coreCorDecoPr.first));
            ATH_MSG_FATAL("Check configuration");
            return StatusCode::FAILURE;
          }
          ATH_MSG_DEBUG("About to write tracking core correction: "
                        << xAOD::Iso::toCString(coreCorDecoPr.first));
          (coreCorDecoPr.second)(*part) = corIter->second;
        }

      } else {
        ATH_MSG_FATAL("Call to TrackIsolationTool failed for flavor "
                      << xAOD::Iso::toCString(flav));
        return StatusCode::FAILURE;
      }
    }
  }
  return StatusCode::SUCCESS;
}

void
IsolationBuilder::declareIso(
  std::vector<std::pair<xAOD::Iso::IsolationFlavour, CaloIsoHelpKey>>& caloIso)
{
  for (auto& iso : caloIso) {
    iso.second.declare(this);
  }
}

void
IsolationBuilder::declareIso(
  std::vector<std::pair<xAOD::Iso::IsolationFlavour, TrackIsoHelpKey>>&
    trackIso)
{
  for (auto& iso : trackIso) {
    iso.second.declare(this);
  }
}
