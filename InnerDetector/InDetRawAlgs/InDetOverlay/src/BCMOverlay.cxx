/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetOverlay/BCMOverlay.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"


BCMOverlay::BCMOverlay(const std::string &name, ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode BCMOverlay::initialize()
{
  ATH_MSG_DEBUG("Initializing...");

  // Check and initialize keys
  ATH_CHECK( m_bkgInputKey.initialize(!m_bkgInputKey.empty()) );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_bkgInputKey);
  ATH_CHECK( m_signalInputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_signalInputKey);
  ATH_CHECK( m_outputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputKey);

  return StatusCode::SUCCESS;
}

StatusCode BCMOverlay::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("execute() begin");

  // Reading the input RDOs
  ATH_MSG_VERBOSE("Retrieving input RDO containers");

  const BCM_RDO_Container *bkgContainerPtr = nullptr;
  if (!m_bkgInputKey.empty()) {
    SG::ReadHandle<BCM_RDO_Container> bkgContainer(m_bkgInputKey, ctx);
    if (!bkgContainer.isValid()) {
      ATH_MSG_ERROR("Could not get background BCM RDO container " << bkgContainer.name() << " from store " << bkgContainer.store());
      return StatusCode::FAILURE;
    }
    bkgContainerPtr = bkgContainer.cptr();

    ATH_MSG_DEBUG("Found background BCM RDO container " << bkgContainer.name() << " in store " << bkgContainer.store());
  }

  SG::ReadHandle<BCM_RDO_Container> signalContainer(m_signalInputKey, ctx);
  if (!signalContainer.isValid()) {
    ATH_MSG_ERROR("Could not get signal BCM RDO container " << signalContainer.name() << " from store " << signalContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found signal BCM RDO container " << signalContainer.name() << " in store " << signalContainer.store());

  // Creating output RDO container
  SG::WriteHandle<BCM_RDO_Container> outputContainer(m_outputKey, ctx);
  ATH_CHECK(outputContainer.record(std::make_unique<BCM_RDO_Container>()));
  if (!outputContainer.isValid()) {
    ATH_MSG_ERROR("Could not record output BCM RDO container " << outputContainer.name() << " to store " << outputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output BCM RDO container " << outputContainer.name() << " in store " << outputContainer.store());

  ATH_CHECK(overlayContainer(ctx, bkgContainerPtr, signalContainer.cptr(), outputContainer.ptr()));

  ATH_MSG_DEBUG("execute() end");
  return StatusCode::SUCCESS;
}

StatusCode BCMOverlay::overlayContainer(const EventContext& ctx,
                                        const BCM_RDO_Container *bkgContainer, 
                                        const BCM_RDO_Container *signalContainer, 
                                        BCM_RDO_Container *outputContainer) const
{

  if (!bkgContainer) {
    for (const BCM_RDO_Collection *coll : *signalContainer) {

      std::unique_ptr<BCM_RDO_Collection> outputColl = std::make_unique<BCM_RDO_Collection>();
      
      for (const BCM_RawData *rdo : *coll) {
        outputColl->push_back(new BCM_RawData(rdo->getWord1(), rdo->getWord2()));
      }
      outputContainer->push_back(outputColl.release());
    }

    return StatusCode::SUCCESS;
  }

  std::unordered_map<unsigned int, const BCM_RDO_Collection *> bkgChannelMap;
  for (const BCM_RDO_Collection *bkgColl : *bkgContainer) {
    bkgChannelMap.emplace(bkgColl->getChannel(), bkgColl);
  }

  for (const BCM_RDO_Collection *sigColl : *signalContainer) {

    auto it = bkgChannelMap.find(sigColl->getChannel());
    if (it == bkgChannelMap.end()) {
      ATH_MSG_ERROR ("No BCM background collection with channel " << sigColl->getChannel());
      return StatusCode::FAILURE;
    }

    const BCM_RDO_Collection *bkgColl = it->second;
    bkgChannelMap.erase(it);
    std::unique_ptr<BCM_RDO_Collection> outputColl = std::make_unique<BCM_RDO_Collection>();
    outputColl->setChannel(sigColl->getChannel());

    constexpr size_t mcSize{1}; // MC always has exactly one RDO
    if (m_dataOverlay.value()) {
      // Data has RDOs from more BCIDs, check MC size
      if (sigColl->size() != mcSize) {
        ATH_MSG_ERROR ("BCM signal collection size mismatch");
        return StatusCode::FAILURE;
      }
    } else {
      // In case of MC+MC overlay collection sizes are the same
      size_t collectionSize = bkgColl->size();
      if (collectionSize != sigColl->size() && collectionSize != mcSize) {
        ATH_MSG_ERROR ("BCM signal and background collection size mismatch");
        return StatusCode::FAILURE;
      }
    }

    int currentBCID = ctx.eventID().bunch_crossing_id();
    for (const BCM_RawData *bkgRDO : *bkgColl) {
      if (m_dataOverlay.value() && bkgRDO->getBCID() != currentBCID) {
        if (m_storeAllBCID.value()) {
          outputColl->push_back(new BCM_RawData(bkgRDO->getWord1(), bkgRDO->getWord2()));
        }
        continue;
      }

      const BCM_RawData *sigRDO = sigColl->front();
      if (bkgRDO->getChannel() == sigRDO->getChannel()) {
        std::unique_ptr<BCM_RawData> mergedRDO = mergeChannel(bkgRDO, sigRDO);
        if (mergedRDO != nullptr) {
          outputColl->push_back(mergedRDO.release());
        } else {
          ATH_MSG_ERROR ("BCM channel merging failed");
          return StatusCode::FAILURE;
        }
      } else {
        ATH_MSG_ERROR ("BCM signal and background channel mismatch");
        return StatusCode::FAILURE;
      }

    }
    outputContainer->push_back(outputColl.release());
  }

  return StatusCode::SUCCESS;
}

std::unique_ptr<BCM_RawData> BCMOverlay::mergeChannel(const BCM_RawData *bkgRDO, 
                                                      const BCM_RawData *signalRDO) const
{

  if (bkgRDO->getPulse1Width()==0) {
    return std::make_unique<BCM_RawData>(signalRDO->getWord1(), signalRDO->getWord2());
  } else if (signalRDO->getPulse1Width()==0) {
    return std::make_unique<BCM_RawData>(bkgRDO->getWord1(), bkgRDO->getWord2());
  }

  unsigned int bkg_p1 = bkgRDO->getPulse1Position();
  unsigned int bkg_w1 = bkgRDO->getPulse1Width();
  unsigned int bkg_p2 = bkgRDO->getPulse2Position();
  unsigned int bkg_w2 = bkgRDO->getPulse2Width();
  unsigned int sig_p1 = signalRDO->getPulse1Position();
  unsigned int sig_w1 = signalRDO->getPulse1Width();
  unsigned int sig_p2 = signalRDO->getPulse2Position();
  unsigned int sig_w2 = signalRDO->getPulse2Width();

  std::vector<std::unique_ptr<BCM_Pulse>> merged_pulses;

  if (bkg_w1 > 0) merged_pulses.push_back(std::make_unique<BCM_Pulse>(bkg_p1,bkg_w1));
  if (bkg_w2 > 0) merged_pulses.push_back(std::make_unique<BCM_Pulse>(bkg_p2,bkg_w2));
  if (sig_w1 > 0) merged_pulses.push_back(std::make_unique<BCM_Pulse>(sig_p1,sig_w1));
  if (sig_w2 > 0) merged_pulses.push_back(std::make_unique<BCM_Pulse>(sig_p2,sig_w2));

  overlayPulses(merged_pulses);
  std::sort(merged_pulses.begin(), merged_pulses.end(), compare);

  // Check whether some of the pulses merged
  for (size_t i = 0; i < merged_pulses.size()-1; i++) {

    BCM_Pulse *early = merged_pulses.at(i).get();
    BCM_Pulse *later = merged_pulses.at(i+1).get();

    if ( (early->p + early->w - 1) >= later->p ) {
      early->w = later->p - early->p + later->w;      // Enlarge early pulse
      merged_pulses.erase(merged_pulses.begin()+i+1, 
                          merged_pulses.begin()+i+2); // Omit later pulse
      i--;
    }
  }

  unsigned int merged_p1;
  unsigned int merged_w1;
  unsigned int merged_p2;
  unsigned int merged_w2;

  if (merged_pulses.size() > 0) {
    merged_p1 = merged_pulses.at(0)->p;
    merged_w1 = merged_pulses.at(0)->w;
  } else {
    merged_p1 = 0;
    merged_w1 = 0;
  }

  if (merged_pulses.size() > 1) {
    merged_p2 = merged_pulses.at(1)->p;
    merged_w2 = merged_pulses.at(1)->w;
  } else {
    merged_p2 = 0;
    merged_w2 = 0;
  }

  // Record two earliest pulses into the output RDO
  return std::make_unique<BCM_RawData>(bkgRDO->getChannel(),
                                       merged_p1, merged_w1,
                                       merged_p2, merged_w2,
                                       bkgRDO->getLVL1A(),
                                       bkgRDO->getBCID(),
                                       bkgRDO->getLVL1ID());
}

void BCMOverlay::overlayPulses(std::vector<std::unique_ptr<BCM_Pulse>>& merged_pulses) const
{

  constexpr double fullPulseWidth{15.};      // Analogue pulse width
                                             // before application of
                                             // time over threshold
  constexpr double slopeUpFraction{1./3.};   // Pulse rise time,
                                             // expressed in the fraction
                                             // of full pulse width
  constexpr double slopeDownFraction{2./3.}; // Pulse fall time,
                                             // expressed in the fraction
                                             // of full pulse width

  for (size_t i = 0; i < merged_pulses.size(); i++) {
    BCM_Pulse* pulse_1 = merged_pulses.at(i).get();

    for (size_t j = 0; j < i; j++) {
      BCM_Pulse* pulse_2 = merged_pulses.at(j).get();
      auto[early,later] = timeOrder(pulse_1, pulse_2);

      double slope_up = 1./slopeUpFraction/(fullPulseWidth - later->w);
      double slope_down = 1./slopeDownFraction/(fullPulseWidth - early->w);
      int bin_min = early->p + early->w;
      int bin_max = later->p;

      // Widen the pulses, if they lie close enough to each other
      for (int bin_iter=bin_min; bin_iter < bin_max; bin_iter++) {
        if (slope_up*(bin_iter - bin_max) - slope_down*(bin_iter - bin_min) > -1) {
          early->w++;
        }
        else break;
      }
      for (int bin_iter=bin_max-1; bin_iter >= bin_min; bin_iter--) {
        if (slope_up*(bin_iter - bin_max) - slope_down*(bin_iter - bin_min) > -1) {
          later->p = bin_iter;
          later->w++;
        }
        else break;
      }

    }
  }

}

std::pair<BCM_Pulse*, BCM_Pulse*> BCMOverlay::timeOrder(BCM_Pulse* pulse1, 
                                                        BCM_Pulse* pulse2) const
{

  if (pulse2->p > pulse1->p) return std::pair(pulse1,pulse2);
  else return std::pair(pulse2,pulse1);

}

bool BCMOverlay::compare(const std::unique_ptr<BCM_Pulse>& a, 
                         const std::unique_ptr<BCM_Pulse>& b)
{

  return a->p < b->p;
  
}
