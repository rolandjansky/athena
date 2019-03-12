/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandFlat.h"
#include "HepMC/GenParticle.h"

#include "InDetIdentifier/TRT_ID.h"
#include "InDetOverlay/TRTOverlay.h"
#include "InDetRawData/InDetRawData.h"
#include "InDetRawData/TRT_LoLumRawData.h"
#include "InDetRawData/TRT_RDORawData.h"
#include "InDetSimData/InDetSimDataCollection.h"

#include "IDC_OverlayBase/IDC_OverlayCommon.h"
#include "IDC_OverlayBase/IDC_OverlayHelpers.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"


namespace Overlay
{
  // Specialize copyCollection() for the TRT
  template<>
  std::unique_ptr<TRT_RDO_Collection> copyCollection(const IdentifierHash &hashId,
                                                     const TRT_RDO_Collection *collection)
  {
    auto outputCollection = std::make_unique<TRT_RDO_Collection>(hashId);
    outputCollection->setIdentifier(collection->identify());

    for (const TRT_RDORawData *existingDatum : *collection) {
      // Owned by the collection
      auto *datumCopy = new TRT_LoLumRawData(existingDatum->identify(), existingDatum->getWord());
      outputCollection->push_back(datumCopy);
    }

    return outputCollection;
  }
} // namespace Overlay


TRTOverlay::TRTOverlay(const std::string &name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
  declareProperty("TRT_HT_OccupancyCorrectionBarrel", m_HTOccupancyCorrectionB=0.110);
  declareProperty("TRT_HT_OccupancyCorrectionEndcap", m_HTOccupancyCorrectionEC=0.090);
  declareProperty("TRT_HT_OccupancyCorrectionBarrelNoE", m_HTOccupancyCorrectionB_noE=0.060);
  declareProperty("TRT_HT_OccupancyCorrectionEndcapNoE", m_HTOccupancyCorrectionEC_noE=0.050);
}


StatusCode TRTOverlay::initialize()
{
  ATH_MSG_DEBUG("Initializing...");

  if (!m_includeBkg) {
    ATH_MSG_DEBUG("Disabling use of background RDOs...");
    ATH_CHECK( m_bkgInputKey.assign("") );
  }

  // Check and initialize keys
  ATH_CHECK( m_bkgInputKey.initialize(!m_bkgInputKey.key().empty()) );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_bkgInputKey);
  ATH_CHECK( m_signalInputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_signalInputKey);
  ATH_CHECK( m_outputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputKey);
  ATH_CHECK( m_signalInputSDOKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey for SDO: " << m_signalInputSDOKey);

  // Retrieve TRT ID helper
  if (!detStore()->retrieve(m_trtId, "TRT_ID").isSuccess() || !m_trtId) {
    ATH_MSG_FATAL("Cannot retrieve TRT ID helper");
    return StatusCode::FAILURE;
  }

  // Initialize random number generator
  CHECK(m_rndmSvc.retrieve());

  // Retrieve TRT local occupancy tool
  CHECK(m_TRT_LocalOccupancyTool.retrieve());

  if (!m_TRTStrawSummarySvc.empty()) {
    if (m_TRTStrawSummarySvc.retrieve().isFailure() ) {
      ATH_MSG_ERROR ("Failed to retrieve StrawStatus Summary " << m_TRTStrawSummarySvc);
      ATH_MSG_ERROR ("configure as 'None' to avoid its loading.");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG( "Retrieved tool " << m_TRTStrawSummarySvc );
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode TRTOverlay::execute()
{
  ATH_MSG_DEBUG("execute() begin");

  // Reading the input RDOs
  const TRT_RDO_Container *bkgContainerPtr = nullptr;
  if (m_includeBkg) {
    SG::ReadHandle<TRT_RDO_Container> bkgContainer(m_bkgInputKey);
    if (!bkgContainer.isValid()) {
      ATH_MSG_ERROR("Could not get background TRT RDO container " << bkgContainer.name() << " from store " << bkgContainer.store());
      return StatusCode::FAILURE;
    }
    bkgContainerPtr = bkgContainer.cptr();

    ATH_MSG_DEBUG("Found background TRT RDO container " << bkgContainer.name() << " in store " << bkgContainer.store());
    ATH_MSG_DEBUG("TRT Background = " << Overlay::debugPrint(bkgContainer.cptr()));
  }

  SG::ReadHandle<TRT_RDO_Container> signalContainer(m_signalInputKey);
  if (!signalContainer.isValid()) {
    ATH_MSG_ERROR("Could not get signal TRT RDO container " << signalContainer.name() << " from store " << signalContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found signal TRT RDO container " << signalContainer.name() << " in store " << signalContainer.store());
  ATH_MSG_DEBUG("TRT Signal     = " << Overlay::debugPrint(signalContainer.cptr()));

  SG::ReadHandle<InDetSimDataCollection> signalSDOContainer(m_signalInputSDOKey);
  if (!signalSDOContainer.isValid()) {
    ATH_MSG_ERROR("Could not get signal TRT SDO map container " << signalSDOContainer.name() << " from store " << signalSDOContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found signal TRT SDO map container " << signalSDOContainer.name() << " in store " << signalSDOContainer.store());

  // Creating output RDO container
  SG::WriteHandle<TRT_RDO_Container> outputContainer(m_outputKey);
  ATH_CHECK(outputContainer.record(std::make_unique<TRT_RDO_Container>(signalContainer->size())));
  if (!outputContainer.isValid()) {
    ATH_MSG_ERROR("Could not record output TRT RDO container " << outputContainer.name() << " to store " << outputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output TRT RDO container " << outputContainer.name() << " in store " << outputContainer.store());

  ATH_CHECK(overlayContainer(bkgContainerPtr, signalContainer.cptr(), outputContainer.ptr(), signalSDOContainer.cptr()));
  ATH_MSG_DEBUG("TRT Result   = " << Overlay::debugPrint(outputContainer.ptr()));

  ATH_MSG_DEBUG("execute() end");
  return StatusCode::SUCCESS;
}


StatusCode TRTOverlay::overlayContainer(const TRT_RDO_Container *bkgContainer,
                                        const TRT_RDO_Container *signalContainer,
                                        TRT_RDO_Container *outputContainer,
                                        const InDetSimDataCollection *signalSDOCollection)
{
  // Get all the hashes for the signal container
  const std::vector<IdentifierHash> signalHashes = signalContainer->GetAllCurrentHashes();

  // There are some use cases where background is empty
  if (!bkgContainer) {
    // Only loop through the signal collections and copy them over
    for (const IdentifierHash &hashId : signalHashes) {
      // Copy the signal collection
      std::unique_ptr<TRT_RDO_Collection> signalCollection = Overlay::copyCollection(hashId, signalContainer->indexFindPtr(hashId));

      if (outputContainer->addCollection(signalCollection.get(), hashId).isFailure()) {
        ATH_MSG_ERROR("Adding signal Collection with hashId " << hashId << " failed");
        return StatusCode::FAILURE;
      } else {
        signalCollection.release();
      }
    }

    return StatusCode::SUCCESS;
  }

  // Setup the random engine
  ATHRNG::RNGWrapper* rngWrapper = m_rndmSvc->getEngine(this);
  rngWrapper->setSeed( name(), Gaudi::Hive::currentContext() );
  CLHEP::HepRandomEngine *rndmEngine(*rngWrapper);

  // Retrieve the occupancy map
  std::map<int, double> occupancyMap = m_TRT_LocalOccupancyTool->getDetectorOccupancy(bkgContainer);

  // Get all the hashes for the background container
  const std::vector<IdentifierHash> bkgHashes = bkgContainer->GetAllCurrentHashes();

  // The MC signal container should typically be smaller than bkgContainer,
  // because the latter contains all the noise, minimum bias and pile up.
  // Thus we firstly iterate over signal hashes and store them in a map.
  std::map<IdentifierHash, bool> overlapMap;
  for (const IdentifierHash &hashId : signalHashes) {
    overlapMap.emplace(hashId, false);
  }

  // Now loop through the background hashes and copy unique ones over
  for (const IdentifierHash &hashId : bkgHashes) {
    auto search = overlapMap.find(hashId);
    if (search == overlapMap.end()) {
      // Copy the background collection
      std::unique_ptr<TRT_RDO_Collection> bkgCollection = Overlay::copyCollection(hashId, bkgContainer->indexFindPtr(hashId));

      if (outputContainer->addCollection(bkgCollection.get(), hashId).isFailure()) {
        ATH_MSG_ERROR("Adding background Collection with hashId " << hashId << " failed");
        return StatusCode::FAILURE;
      } else {
        bkgCollection.release();
      }
    } else {
      // Flip the overlap flag
      search->second = true;
    }
  }

  // Finally loop through the map and process the signal and overlay if
  // necessary
  for (const auto &[hashId, overlap] : overlapMap) {
    // Copy the signal collection
    std::unique_ptr<TRT_RDO_Collection> signalCollection = Overlay::copyCollection(hashId, signalContainer->indexFindPtr(hashId));

    if (overlap) { // Do overlay
      // Create the output collection, only works for Inner Detector
      auto outputCollection = std::make_unique<TRT_RDO_Collection>(hashId);
      outputCollection->setIdentifier(signalCollection->identify());
      // Copy the background collection
      std::unique_ptr<TRT_RDO_Collection> bkgCollection = Overlay::copyCollection(hashId, bkgContainer->indexFindPtr(hashId));

      // Merge collections
      int det = m_trtId->barrel_ec(signalCollection->identify());
      mergeCollections(bkgCollection.get(), signalCollection.get(), outputCollection.get(), occupancyMap[det], signalSDOCollection, rndmEngine);

      if (outputContainer->addCollection(outputCollection.get(), hashId).isFailure()) {
        ATH_MSG_ERROR("Adding overlaid Collection with hashId " << hashId << " failed");
        return StatusCode::FAILURE;
      } else {
        outputCollection.release();
      }
    } else { // Only write signal out
      if (outputContainer->addCollection(signalCollection.get(), hashId).isFailure()) {
        ATH_MSG_ERROR("Adding signal Collection with hashId " << hashId << " failed");
        return StatusCode::FAILURE;
      } else {
        signalCollection.release();
      }
    }
  }

  return StatusCode::SUCCESS;
}


void TRTOverlay::mergeCollections(TRT_RDO_Collection *bkgCollection,
                                  TRT_RDO_Collection *signalCollection,
                                  TRT_RDO_Collection *outputCollection,
                                  double occupancy,
                                  const InDetSimDataCollection *signalSDOCollection,
                                  CLHEP::HepRandomEngine *rndmEngine)
{
  if (bkgCollection->identify() != signalCollection->identify()) {
    throw std::runtime_error("mergeCollections(): collection Id mismatch");
  }

  // Merge by copying ptrs from background and signal to output collection
  TRT_RDO_Collection::size_type ibkg = 0, isig = 0;
  while ((ibkg < bkgCollection->size()) || (isig < signalCollection->size())) {
    // The RDO that goes to the output at the end of this step.
    TRT_RDORawData *tmp{};

    if (isig == signalCollection->size()) {
      // just copy the remaining background digits
      bkgCollection->swapElement(ibkg++, nullptr, tmp);
    } else if (ibkg == bkgCollection->size()) {
      // just copy the remaining signal digits
      signalCollection->swapElement(isig++, nullptr, tmp);
    } else {
      // Need to decide which one goes first.
      // See comments in TRTDigitization.cxx about the assumption that id1<id2 <=> hash1<hash2
      if (signalCollection->at(isig)->identify() < bkgCollection->at(ibkg)->identify()) {
        signalCollection->swapElement(isig++, nullptr, tmp);
      } else if (bkgCollection->at(ibkg)->identify() < signalCollection->at(isig)->identify()) {
        bkgCollection->swapElement(ibkg++, nullptr, tmp);
      } else {
        // The hits are on the same channel.
        TRT_RDORawData *tmpBkg{};
        bkgCollection->swapElement(ibkg++, nullptr, tmpBkg);
        signalCollection->swapElement(isig++, nullptr, tmp);

        TRT_LoLumRawData *sigRdo = dynamic_cast<TRT_LoLumRawData *>(tmp);
        const TRT_LoLumRawData *bkgRdo = dynamic_cast<const TRT_LoLumRawData *>(tmpBkg);

        if (sigRdo && bkgRdo) {
          // the actual merging
          sigRdo->merge(*bkgRdo);

          // If the hit is not already a high level hit
          if (!(sigRdo->getWord() & 0x04020100)) {
            // Determine if the hit is from an electron or not
            bool isElectron = false;
            Identifier rdoId = sigRdo->identify();
            InDetSimDataCollection::const_iterator sdoIter = signalSDOCollection->find(rdoId);
            if (sdoIter != signalSDOCollection->end()) {
              const std::vector<InDetSimData::Deposit> &deposits = sdoIter->second.getdeposits();
              for (const InDetSimData::Deposit &deposit : deposits) {
                const HepMcParticleLink &particleLink = deposit.first;
                if (particleLink.isValid()) {
                  if (std::abs(particleLink->pdg_id()) == 11) {
                    isElectron = true;
                    break;
                  }
                }
              }
            }

            // Determine what type of straw was hit
            bool isXenonStraw = false;
            if (!m_TRTStrawSummarySvc.empty()) {
              if (m_TRTStrawSummarySvc->getStatusHT(rdoId) == TRTCond::StrawStatus::Good) {
                isXenonStraw = true;
              }
            }

            // Get random number
            int det = m_trtId->barrel_ec(rdoId);
            float HTOccupancyCorrection = 0;
            if (isElectron) {
              HTOccupancyCorrection = std::abs(det) > 1 ? m_HTOccupancyCorrectionEC : m_HTOccupancyCorrectionB;
            } else {
              HTOccupancyCorrection = std::abs(det) > 1 ? m_HTOccupancyCorrectionEC_noE : m_HTOccupancyCorrectionB_noE;
            }

            unsigned int newWord = 0;
            if (isXenonStraw && occupancy * HTOccupancyCorrection > CLHEP::RandFlat::shoot(rndmEngine, 0, 1)) {
              newWord += 1 << (26-9);
            }
  
            TRT_LoLumRawData newRdo(rdoId, newWord);
            sigRdo->merge(newRdo);
          }
        } else {
          ATH_MSG_WARNING("TRT RDO is the wrong format");
        }

        delete tmpBkg;
      }
    }

    outputCollection->push_back(tmp);
  } // <= while
}
