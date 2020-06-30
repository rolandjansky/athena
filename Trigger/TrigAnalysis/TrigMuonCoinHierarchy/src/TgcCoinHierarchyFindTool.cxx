/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonCoinHierarchy/TgcCoinHierarchyFindTool.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "TGCcablingInterface/ITGCcablingServerSvc.h"
#include "TrigMuonCoinHierarchy/DummyHitChannelToOnlineID.h"
#include "PathResolver/PathResolver.h"

using Muon::TgcCoinData;
using Muon::TgcPrepData;

namespace Trigger {
  TgcCoinHierarchyFindTool::TgcCoinHierarchyFindTool(const std::string& t, const std::string& n, const IInterface* p)
    : AthAlgTool(t, n, p), m_tgcCabling(0), m_bc(TgcCoinHierarchy::UNKNOWN)
  {
    // Flags 
    // true means running
    declareProperty("DummyHitChannel", m_dummyHitChannel=false);
    declareProperty("Mask2FileName", m_mask2FileName="TGC_mask2_channels_20101029.txt");
  }

  StatusCode TgcCoinHierarchyFindTool::queryInterface(const InterfaceID& riid, void** ppvIf) {
    ATH_MSG_DEBUG("queryInterface()");
    
    if(ITgcCoinHierarchyFindTool::interfaceID().versionMatch(riid)) {
      *ppvIf = dynamic_cast<ITgcCoinHierarchyFindTool*>(this); 
      addRef();
      ATH_MSG_DEBUG("InterfaceID successfully matched with ITgcCoinHierarchyFindTool one.");
      return StatusCode::SUCCESS;
    }

    return AthAlgTool::queryInterface(riid, ppvIf);
  }

  StatusCode TgcCoinHierarchyFindTool::initialize() {
    ATH_MSG_DEBUG("initialize()");

    ATH_CHECK(m_idHelperSvc.retrieve());
    
    // Try to configure the cabling service
    StatusCode sc = getCabling();
    if(sc.isFailure()) {
      ATH_MSG_INFO("TGCcablingServerSvc not yet configured; postone TGCcabling initialization at first event.");
    }

    if(m_dummyHitChannel){
      sc = readMask2File(); 
      if(sc.isFailure()) return sc; 
    }

    return StatusCode::SUCCESS;
  }
  
  StatusCode TgcCoinHierarchyFindTool::find(TgcCoinHierarchyTriggerSector* pTrigSectorComb, 
                                            std::vector<TgcCoinHierarchy*>* pHierarchies) {
    ATH_MSG_DEBUG("find()");

    // Find coincidences of TgcCoinHierarchy's from the TgcCoinHierarchyTriggerSector
    StatusCode sc = findCoincidence(pTrigSectorComb, pHierarchies);
    if(sc.isFailure()) {
      ATH_MSG_FATAL("findCoincidence() failed in find()");
      return sc;
    }

    // Find hits of TgcCoinHierarchy's from the TgcCoinHierarchyTriggerSector 
    sc = findHit(pTrigSectorComb, pHierarchies);
    if(sc.isFailure()) {
      ATH_MSG_FATAL("findHit() failed in find()");
      return sc;
    }

    return sc;
  }

  StatusCode TgcCoinHierarchyFindTool::findCoincidence(TgcCoinHierarchyTriggerSector* pTrigSectorComb,
                                                       std::vector<TgcCoinHierarchy*>* pHierarchies) {
    ATH_MSG_DEBUG("findCoincidence()");

    m_bc = pTrigSectorComb->getTiming();
    if(m_bc==TgcCoinHierarchy::UNKNOWN){
      ATH_MSG_FATAL("Timing Setup has problem. TIMING:" << m_bc << ":" << __FILE__<<":"<<__LINE__);
      return StatusCode::FAILURE;
    }

    // From Sector Logic
    unsigned int sizeSL = pTrigSectorComb->numCoincidences(TgcCoinHierarchy::SL);
    for(unsigned int iSL=0; iSL<sizeSL; iSL++) {
      // Get the corresponding TgcCoinData of SL from the TgcCoinHierarchyTriggerSector
      const TgcCoinData* sl = pTrigSectorComb->getCoincidence(iSL, TgcCoinHierarchy::SL);
      if(!sl) {
        ATH_MSG_FATAL("Returned pointer to SL is null. This should never happen.");
        return StatusCode::FAILURE;
      }

      // Create a new TgcCoinHierarchy  
      TgcCoinHierarchy* hierarchy = new TgcCoinHierarchy;
      if(!hierarchy) {
        ATH_MSG_FATAL("new TgcCoinHierarchy failed in SL. This should never happen.");
        return StatusCode::FAILURE;
      }
      pHierarchies->push_back(hierarchy);

      // Set the coincidence for the new TgcCoinHierarchy
      bool isOK = hierarchy->setCoincidence(sl, TgcCoinHierarchy::SL, m_bc);
      if(!isOK) showWARNINGforSetCoincidence(sl, TgcCoinHierarchy::SL);
      // Set the associated flag
      pTrigSectorComb->setCoincidenceAssociated(iSL, TgcCoinHierarchy::SL);

      // Find HiPt's and Tracklet's of TgcCoinHierarchy's from the SL with the TgcCoinHierarchyTriggerSector 
      StatusCode sc = findCoincidenceFromSL(pTrigSectorComb, hierarchy);
      if(sc.isFailure()) {
        ATH_MSG_FATAL("findCoincidenceFromSL failed in findCoincidence");
        return StatusCode::FAILURE;
      }
    }

    // From HiPt (Wire -> Strip) which is not associated with SL 
    for(int iTYPE=0; iTYPE<TgcCoinHierarchy::NTYPES; iTYPE++) { // wire or strip
      TgcCoinHierarchy::COINCIDENCE coincidence = (iTYPE==TgcCoinHierarchy::WIRE ? 
						   TgcCoinHierarchy::HIPTWIRE : TgcCoinHierarchy::HIPTSTRIP);
      unsigned int sizeHP = pTrigSectorComb->numCoincidences(coincidence);
      for(unsigned int iHP=0; iHP<sizeHP; iHP++) {
        // Check that the HiPt is already associated
        if(pTrigSectorComb->isCoincidenceAssociated(iHP, coincidence)) continue; 
      
        // Get the corresponding TgcCoinData of HiPt from the TgcCoinHierarchyTriggerSector
        const TgcCoinData*hp = pTrigSectorComb->getCoincidence(iHP, coincidence);
        if(!hp) {
          ATH_MSG_FATAL("Returned pointer to HIPT is null. This should never happen.");
          return StatusCode::FAILURE;
        }

        // Create a new TgcCoinHierarchy
        TgcCoinHierarchy* hierarchy = new TgcCoinHierarchy;
        if(!hierarchy) {
          ATH_MSG_FATAL("new TgcCoinHierarchy failed in HiPt. This should never happen.");
          return StatusCode::FAILURE;
        }
        pHierarchies->push_back(hierarchy);

        // Set the coincidence for the new TgcCoinHierarchy
        bool isOK = hierarchy->setCoincidence(hp, coincidence, m_bc);
        if(!isOK) showWARNINGforSetCoincidence(hp, coincidence);
        // Set the associated flag
        pTrigSectorComb->setCoincidenceAssociated(iHP, coincidence);

        // Find a Tracklet of TgcCoinHierarchy's from the HiPt with the TgcCoinHierarchyTriggerSector. 
        StatusCode sc = findCoincidenceFromHiPt(pTrigSectorComb, hierarchy, (iTYPE==TgcCoinHierarchy::STRIP));
        if(sc.isFailure()) {
          ATH_MSG_FATAL("findCoincidenceFromHiPt failed in findCoincidence");
          return StatusCode::FAILURE;
        }
      }
    }

    // From Tracklet (Wire -> Strip) which is not associated with SL and Tracklet 
    for(int iTYPE=0; iTYPE<TgcCoinHierarchy::NTYPES; iTYPE++) { // wire or strip
      TgcCoinHierarchy::COINCIDENCE coincidence = (iTYPE==TgcCoinHierarchy::WIRE ? 
                                                   TgcCoinHierarchy::TRACKLETWIRE : TgcCoinHierarchy::TRACKLETSTRIP);
      unsigned int sizeTR = pTrigSectorComb->numCoincidences(coincidence);
      for(unsigned int iTR=0; iTR<sizeTR; iTR++) {
        // Check that the Tracklet is already associated
        if(pTrigSectorComb->isCoincidenceAssociated(iTR, coincidence)) continue; 
       
        // Get the corresponding TgcCoinData of Tracklet from the TgcCoinHierarchyTriggerSector
        const TgcCoinData*tr = pTrigSectorComb->getCoincidence(iTR, coincidence);
        if(!tr) {
          ATH_MSG_FATAL("Returned pointer to TRACKLET is null. This should never happen.");
          return StatusCode::FAILURE;
        }

        // Create a new TgcCoinHierarchy
        TgcCoinHierarchy* hierarchy = new TgcCoinHierarchy;
        if(!hierarchy) {
          ATH_MSG_FATAL("new TgcCoinHierarchy failed in Tracklet. This should never happen.");
          return StatusCode::FAILURE;
        }
        pHierarchies->push_back(hierarchy);

        // Set the coincidence for the new TgcCoinHierarchy
        bool isOK = hierarchy->setCoincidence(tr, coincidence, m_bc);
        if(!isOK) showWARNINGforSetCoincidence(tr, coincidence);
        // Set the associated flag
        pTrigSectorComb->setCoincidenceAssociated(iTR, coincidence);
      }
    } 
    return StatusCode::SUCCESS;
  }

  StatusCode TgcCoinHierarchyFindTool::findCoincidenceFromSL(TgcCoinHierarchyTriggerSector* pTrigSectorComb,
                                                             TgcCoinHierarchy* pHierarchy) {
    ATH_MSG_DEBUG("findCoincidenceFromSL()");

    // Get the corresponding TgcCoinData of SL from the TgcCoinHierarchyTriggerSector
    const TgcCoinData* sl = pHierarchy->getCoincidence(TgcCoinHierarchy::SL);
    if(!sl) {
      ATH_MSG_FATAL("Returned pointer to SL is null. This should never happen.");
      return StatusCode::FAILURE;
    }

    const int trackletId = sl->trackletId();
    const int trackletIdStrip = sl->trackletIdStrip();
    const int sub = getSubFromRoiNumber(sl->roi(), false/* Wire */, sl->isForward()); 
    const int subStrip = getSubFromRoiNumber(sl->roi(), true/* Strip */, sl->isForward()); 

    // Loop from HiPt Wire -> HiPt Strip -> Tracklet Wire -> Tracklet Strip
    for(int iCoin=TgcCoinHierarchy::HIPTWIRE; iCoin>=TgcCoinHierarchy::TRACKLETSTRIP; iCoin--) {
      TgcCoinHierarchy::COINCIDENCE coincidence = m_TCH.intToCOINCIDENCE(iCoin);
      const unsigned int sizeC = pTrigSectorComb->numCoincidences(coincidence);
      for(unsigned int iC=0; iC<sizeC; iC++) {
        const TgcCoinData* coinData = pTrigSectorComb->getCoincidence(iC, coincidence);
        if(!coinData) {
          ATH_MSG_FATAL("Returned pointer to HiPt or Tracklet is null. This should never happen.");
          return StatusCode::FAILURE;
        }

        // Compare trackletId and sub 
        if(coincidence==TgcCoinHierarchy::HIPTWIRE || 
           coincidence==TgcCoinHierarchy::TRACKLETWIRE) {
          if(trackletId!=coinData->trackletId()) continue;
          if(coincidence==TgcCoinHierarchy::HIPTWIRE && sub!=coinData->sub()) continue;
        } else if(coincidence==TgcCoinHierarchy::HIPTSTRIP || 
                  coincidence==TgcCoinHierarchy::TRACKLETSTRIP) {
          if(trackletIdStrip!=coinData->trackletId()) continue;
          if(coincidence==TgcCoinHierarchy::HIPTSTRIP && subStrip!=coinData->sub()) continue;
        } else {
          ATH_MSG_FATAL("Coincidence is not HiPt nor Tracklet. This should never happen.");
          return StatusCode::FAILURE;
        }
        
        // Set the coincidence for the TgcCoinHierarchy
        bool isOK = pHierarchy->setCoincidence(coinData, coincidence, m_bc);
        if(!isOK) showWARNINGforSetCoincidence(coinData, coincidence);
        // Set the associated flag
        pTrigSectorComb->setCoincidenceAssociated(iC, coincidence);
        // Coincidence can be duplicated (multiple coincidences have the same trackletId)
        // To check the duplication, loop over all coincidences
        //        break; 
      }
    }

    // Endcap Strip HiPt has a bug
    // In case that the HiPt Strip associated to SL is found but any associated Tracklet Strip is not found. 
    if(!sl->isForward() &&
       !pHierarchy->hasCoincidence(TgcCoinHierarchy::TRACKLETSTRIP)) { 
      const unsigned int sizeTR = pTrigSectorComb->numCoincidences(TgcCoinHierarchy::TRACKLETSTRIP);
      for(unsigned int iTR=0; iTR<sizeTR; iTR++) {
        const TgcCoinData* tr = pTrigSectorComb->getCoincidence(iTR, TgcCoinHierarchy::TRACKLETSTRIP);
        if(!tr) {
          ATH_MSG_FATAL("Returned pointer to TRACKLET STRIP is null. This should never happen.");
          return StatusCode::FAILURE;
        }

        // Check possible relation between HiPt Strip and Tracklet Strip due to the HiPt board bug 
        if(isPossibleTrackletIdStripAssociation(trackletIdStrip, tr->trackletId())) {
          // The first parameter should be trackletIdStrip not trackletId. 
          // Set the coincidence for the TgcCoinHierarchy
          bool isOK = pHierarchy->setCoincidence(tr, TgcCoinHierarchy::TRACKLETSTRIP, m_bc); 
          if(!isOK) showWARNINGforSetCoincidence(tr, TgcCoinHierarchy::TRACKLETSTRIP);
          // Set the associated flag
          pTrigSectorComb->setCoincidenceAssociated(iTR, TgcCoinHierarchy::TRACKLETSTRIP);
          // Coincidence can be duplicated (multiple coincidences have the same trackletId)
          // To check the duplication, loop over all coincidences
          //          break;
        }
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode TgcCoinHierarchyFindTool::findCoincidenceFromHiPt(TgcCoinHierarchyTriggerSector* pTrigSectorComb,
                                                               TgcCoinHierarchy* pHierarchy, 
                                                               const bool isStrip) {
    ATH_MSG_DEBUG("findCoincidenceFromHiPt()");

    // Get the corresponding TgcCoinData of HiPt from the TgcCoinHierarchyTriggerSector
    const TgcCoinData* hp = pHierarchy->getCoincidence(isStrip ? 
                                                       TgcCoinHierarchy::HIPTSTRIP : TgcCoinHierarchy::HIPTWIRE
						       ); 
    if(!hp) {
      ATH_MSG_FATAL("Returned pointer to HIPT is null. This should never happen.");
      return StatusCode::FAILURE;
    }

    const int trackletId = hp->trackletId();
    // HiPt -> Tracklet
    TgcCoinHierarchy::COINCIDENCE TRACKLET 
      = isStrip ? TgcCoinHierarchy::TRACKLETSTRIP : TgcCoinHierarchy::TRACKLETWIRE;
    const unsigned int sizeTR = pTrigSectorComb->numCoincidences(TRACKLET);
    for(unsigned int iTR=0; iTR<sizeTR; iTR++) {
      const TgcCoinData* tr = pTrigSectorComb->getCoincidence(iTR, TRACKLET);
      if(!tr) {
        ATH_MSG_FATAL("Returned pointer to TRACKLET is null. This should never happen.");
        return StatusCode::FAILURE;
      }
      // Compare trackletId 
      if(trackletId!=tr->trackletId()) continue;

      // Set the coincidence for the TgcCoinHierarchy
      bool isOK = pHierarchy->setCoincidence(tr, TRACKLET, m_bc);
      if(!isOK) showWARNINGforSetCoincidence(tr, TRACKLET);
      // Set the associated flag
      pTrigSectorComb->setCoincidenceAssociated(iTR, TRACKLET);
      // Coincidence can be duplicated (multiple coincidences have the same trackletId)
      // To check the duplication, loop over all coincidences
    }

    // Endcap Strip HiPt has a bug
    // In case that any associated Tracklet Strip is not found for HiPt Strip.
    if(isStrip && !hp->isForward() && !pHierarchy->hasCoincidence(TgcCoinHierarchy::TRACKLETSTRIP)) { 
      for(unsigned int iTR=0; iTR<sizeTR; iTR++) {
        const TgcCoinData* tr = pTrigSectorComb->getCoincidence(iTR, TgcCoinHierarchy::TRACKLETSTRIP);
        if(!tr) {
          ATH_MSG_FATAL("Returned pointer to TRACKLET STRIP is null. This should never happen.");
          return StatusCode::FAILURE;
        }

        // Check possible relation between HiPt Strip and Tracklet Strip due to the HiPt board bug
        if(isPossibleTrackletIdStripAssociation(trackletId, tr->trackletId())) {
          // Set the coincidence for the TgcCoinHierarchy
          bool isOK = pHierarchy->setCoincidence(tr, TgcCoinHierarchy::TRACKLETSTRIP, m_bc);
          if(!isOK) showWARNINGforSetCoincidence(tr, TgcCoinHierarchy::TRACKLETSTRIP);
          // Set the associated flag
          pTrigSectorComb->setCoincidenceAssociated(iTR, TgcCoinHierarchy::TRACKLETSTRIP);
          // Coincidence can be duplicated (multiple coincidences have the same trackletId)
          // To check the duplication, loop over all coincidences
        }
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode TgcCoinHierarchyFindTool::findHit(TgcCoinHierarchyTriggerSector* pTrigSectorComb,
                                               std::vector<TgcCoinHierarchy*>* pHierarchies) {
    ATH_MSG_DEBUG("findHit()");

    m_bc = pTrigSectorComb->getTiming();
    if(m_bc==TgcCoinHierarchy::UNKNOWN){
      ATH_MSG_FATAL("Timing Setup has problem. TIMING:" << m_bc << ":" << __FILE__<<":"<<__LINE__);
      return StatusCode::FAILURE;
    }


    std::vector<TgcCoinHierarchy*>::iterator it   = pHierarchies->begin(); 
    std::vector<TgcCoinHierarchy*>::iterator it_e = pHierarchies->end(); 
    for(; it!=it_e; it++) {

      /* timing match */
      if(m_bc!=(*it)->getTimingCoin()){
        //std::cout << "dbg:" << __FILE__ << ":" << __LINE__ << std::endl;
        continue;
      }
      /* end of timing match */

      if((*it)->hasCoincidence(TgcCoinHierarchy::HIPTWIRE)) {
        // Find TGC1 hits of TgcCoinHierarchy's from a HiPt Wire with the TgcCoinHierarchyTriggerSector
        StatusCode sc = findTGC1HitFromHiPt(pTrigSectorComb, *it, false);
        if(sc.isFailure()) {
          ATH_MSG_FATAL("findTGC1HitFromHiPt for wire failed. This should never happen"); 
          return sc;
        }
      }
      if((*it)->hasCoincidence(TgcCoinHierarchy::HIPTSTRIP)) {
        // Find TGC1 hits of TgcCoinHierarchy's from a HiPt Strip with the TgcCoinHierarchyTriggerSector
        StatusCode sc = findTGC1HitFromHiPt(pTrigSectorComb, *it, true);
        if(sc.isFailure()) {
          ATH_MSG_FATAL("findTGC1HitFromHiPt for strip failed. This should never happen"); 
          return sc;
        }
      }

      if((*it)->hasCoincidence(TgcCoinHierarchy::TRACKLETWIRE)) {
        // Find TGC1 hits of TgcCoinHierarchy's from a Tracklet Wire with the TgcCoinHierarchyTriggerSector
        StatusCode sc = findTGC23HitFromTracklet(pTrigSectorComb, *it, false);
        if(sc.isFailure()) {
          ATH_MSG_FATAL("findTGC23HitFromTracklet for wire failed. This should never happen"); 
          return sc;
        }
      }
      if((*it)->hasCoincidence(TgcCoinHierarchy::TRACKLETSTRIP)) {
        // Find TGC1 hits of TgcCoinHierarchy's from a Tracklet Strip with the TgcCoinHierarchyTriggerSector
        StatusCode sc = findTGC23HitFromTracklet(pTrigSectorComb, *it, true);
        if(sc.isFailure()) {
          ATH_MSG_FATAL("findTGC23HitFromTracklet for strip failed. This should never happen"); 
	  return sc;
        }
      }
    }
 
    return StatusCode::SUCCESS;
  }

  StatusCode TgcCoinHierarchyFindTool::findTGC1HitFromHiPt(TgcCoinHierarchyTriggerSector* pTrigSectorComb,
                                                           TgcCoinHierarchy* pHierarchy,
                                                           const bool isStrip) {
    ATH_MSG_DEBUG("findTGC1HitFromHiPt()");
    
    if(!m_tgcCabling) {
      StatusCode status = getCabling();
      if(!status.isSuccess()) return status; 
    }

    // Get the corresponding TgcCoinData of HiPt from the TgcCoinHierarchyTriggerSector
    if(!pHierarchy->hasCoincidence(TgcCoinData::TYPE_HIPT, isStrip)) {
      ATH_MSG_WARNING("There is no HIPT " << (isStrip ? "STRIP" : "WIRE") << " in findTGC1HitFromHiPt.");
      return StatusCode::SUCCESS;
    } 
    const TgcCoinData* hiPt = pHierarchy->getCoincidence(TgcCoinData::TYPE_HIPT, isStrip);
    if(!hiPt) {
      ATH_MSG_FATAL("Returned pointer to HiPt is null. This should never happen.");
      return StatusCode::FAILURE;
    }
    
    const int hiptDelta = hiPt ->delta();
    const int DeltaBeforeConvert = getDeltaBeforeConvert(isStrip, hiPt->delta());
    
    const bool hasTracklet = pHierarchy->hasCoincidence(TgcCoinData::TYPE_TRACKLET, isStrip);
    int trackletPos = -9999; // pos is channel on SLB, position of TgcRawData. pos is equal to or more than 0.  
    int trackletChannel = -9999;
    if(hasTracklet) {
      const TgcCoinData* tracklet = pHierarchy->getCoincidence(TgcCoinData::TYPE_TRACKLET, isStrip);
      const Identifier trackletChannelId = tracklet->channelIdOut();
      trackletChannel = m_idHelperSvc->tgcIdHelper().channel(trackletChannelId);  
      int trackletSubDetectorID = -1;
      int trackletRodID = -1;
      int trackletSswID = -1;
      int trackletSbLoc = -1; // https://twiki.cern.ch/twiki/pub/Main/TgcDocument/sbloc-070701.xls, slbId of TgcRawData
      int trackletBlock = -1; // block is 0 or 1, subMatrix of TgcRawData
      m_tgcCabling->getLowPtCoincidenceIDfromOfflineID(trackletChannelId,
                                                       trackletSubDetectorID,
                                                       trackletRodID,
                                                       trackletSswID,
                                                       trackletSbLoc,
                                                       trackletBlock,
                                                       trackletPos,
                                                       false); // TGC3 (not middle)
      trackletPos += BIT_POS_NUM_ASD*BIT_POS_ASD_SIZE*trackletBlock; 
      if(!isStrip) { // Wire 
        if(tracklet->isForward()) { // Forward
          trackletPos += WD_MAP_SIZE*(trackletSbLoc%8); // sbLoc=0-3 for phi0 and 8-11 for phi2 
        } else { // Endcap 
          trackletPos += WD_MAP_SIZE*trackletSbLoc + BIT_POS_NUM_ASD*BIT_POS_ASD_SIZE; // sbLoc=0-9 and 32 is offset_dt 
        }
      }
      trackletPos += DeltaBeforeConvert;
    }    
    
    const Identifier hiptChannelIdOut = hiPt->channelIdOut();
    int hiptChannel = m_idHelperSvc->tgcIdHelper().channel(hiptChannelIdOut);  
    int hiptStationPhi = m_idHelperSvc->tgcIdHelper().stationPhi(hiptChannelIdOut);  
    int hiptSubDetectorID = -1;
    int hiptRodID = -1;
    int hiptSswID_out = -1;
    int hiptSbLoc_out = -1;
    int hiptSlbID_out = -1;
    bool found = getHiPtIds(hiptChannelIdOut, hiptSubDetectorID, hiptRodID, hiptSswID_out, hiptSbLoc_out, hiptSlbID_out);
    if(!found) {
      return StatusCode::SUCCESS;
    }

    int hiptSector = -1;
    bool hiptisStrip_out = false;
    bool hiptisForward_out = false;
    int hiptChip_out = -1;
    int hiptHitID_out = -1;
    int hiptHsub_out = -1;
    int dummy_int[3] = {0, 0, 0};
    found = m_tgcCabling->getHighPtIDfromOfflineID(hiptChannelIdOut, dummy_int[0], dummy_int[1], hiptSector, hiptisStrip_out,
                                                   hiptisForward_out, dummy_int[2], hiptChip_out, hiptHitID_out, hiptHsub_out);
    if(!found) {
      return StatusCode::SUCCESS;
    }

    int slbSubMatrix_out = -1;
    int bitpos_out[2] = {0, 0};
    // TGC3 
    if(!isStrip) { // wire
      getBitPosOutWire(hiptHitID_out, hiptHsub_out, hiptChip_out, hiptisForward_out, slbSubMatrix_out, bitpos_out);
    } else { // strip
      getBitPosOutStrip(hiptHitID_out, hiptHsub_out, slbSubMatrix_out, bitpos_out);
    }

    int bitpos_in[2] = {0, 0}; //these are bit positions projected from TGC3 to TGC1
    int hiptSlbChannel[2] = {0, 0};
    // TGC1 
    if(!isStrip) { // wire
      int hiptSbLoc_in_wire[2] = {0, 0};
      getBitPosInWire(hiptisForward_out, bitpos_out, hiptSlbID_out, hiptSector, 
                      DeltaBeforeConvert, hiptSbLoc_in_wire, bitpos_in, hiptSlbChannel);
    } else { // strip
      int hiptSbLoc_in_strip = -1;
      getBitPosInStrip(hiptSector, hiptChip_out, bitpos_out, DeltaBeforeConvert, 
                       hiptSbLoc_in_strip, bitpos_in, hiptSlbChannel);
    }

    const int hiPtStationEtaOut = m_idHelperSvc->tgcIdHelper().stationEta(hiptChannelIdOut);

    TgcCoinHierarchy::TYPE type = (isStrip ? TgcCoinHierarchy::STRIP : TgcCoinHierarchy::WIRE);
    
    // Loop over all TGC1 hits in the TgcCoinHierarchyTriggerSector
    unsigned int numHitsTGC1 = pTrigSectorComb->numHits(type, TgcCoinHierarchy::TGC1);
    for(unsigned int iHitTGC1=0; iHitTGC1<numHitsTGC1; iHitTGC1++) {
      const TgcPrepData* hit = pTrigSectorComb->getHit(iHitTGC1, type, TgcCoinHierarchy::TGC1);
      if(!hit) {
        ATH_MSG_FATAL("Returned pointer to TGC1 HIT is null. This should never happen.");
        return StatusCode::FAILURE;
      }

      const int hitStationEta = m_idHelperSvc->tgcIdHelper().stationEta(hit->identify());

      // HiPt Endcap Strip looses the information to distinguish A,B-inputs and C,D-inputs.
      // The minimum combinations of TGC3 (HiPt) and TGC1 (Hit) are checked.
      if(!hiptisForward_out && isStrip && !isHiPtEndcapStripCombination(hiPtStationEtaOut, hitStationEta)) continue;

      int hitSubDetectorID = -1;
      int hitRodID = -1;
      int hitSswID = -1;
      int hitSbLoc = -1;
      int hitBitpos = -1;
      bool foundReadoutIDfromOfflineIDHit = m_tgcCabling->getReadoutIDfromOfflineID(hit->identify(),
                                                                                    hitSubDetectorID,
                                                                                    hitRodID,
                                                                                    hitSswID,
                                                                                    hitSbLoc,
                                                                                    hitBitpos,
                                                                                    false);
      if(!foundReadoutIDfromOfflineIDHit){
        ATH_MSG_WARNING("getReadoutIDfromOfflineID failed for  Hit with channelId=" << hit->identify()); 
        return StatusCode::SUCCESS; 
      } 

      int tmp_int[2] = {0, 0};
      bool tmp_bool[2] = {false, false};
      int hitSlbID = -1;
      bool foundgetSLBIDfromReadoutIDHit = m_tgcCabling->getSLBIDfromReadoutID(tmp_int[0],
                                                                               tmp_bool[0],
                                                                               tmp_bool[1],
                                                                               tmp_int[1],
                                                                               hitSlbID,
                                                                               hitSubDetectorID,
                                                                               hitRodID,
                                                                               hitSswID,
                                                                               hitSbLoc);
      if(!foundgetSLBIDfromReadoutIDHit) {
        ATH_MSG_WARNING("getSLBIDfromReadoutIDHit failed for Hit with channelId=" << hit->identify()); 
        return StatusCode::SUCCESS;  
      }

      int hitSlbChannel = -1;
      if(!isStrip) { // wire
        int tmpslbchannel = getchannel(hitBitpos, TgcRawData::SLB_TYPE_TRIPLET_WIRE);
        hitSlbChannel = WT_MAP_SIZE * hitSlbID + tmpslbchannel;
        //Endcap RdoChannel is from 78 to 665
        //Forward RdoChannel is from 0 to 312
      } else { // strip
        int tmpslbchannel = getchannelfromHitStrip(hitBitpos, TgcRawData::SLB_TYPE_TRIPLET_STRIP);
        hitSlbChannel = tmpslbchannel;
      }
      
      if((hiptSlbChannel[0]-2<=hitSlbChannel) && (hiptSlbChannel[1]+2>=hitSlbChannel)) {
        bool isMatched = false;
        if(hasTracklet) { 
          int deltaTGC1 = trackletPos-hitSlbChannel;
          if(isStrip) { // Strip 
            if(-7==hiptDelta || hiptDelta==7) {
              isMatched = (-1<=deltaTGC1) && (deltaTGC1<=+1);
            } else if((-6<=hiptDelta) && (hiptDelta<=+6)) { 
              isMatched = ( 0<=deltaTGC1) && (deltaTGC1<=+1); 
            } else {
              isMatched = false;
            }
            // Fix based on run159179, 2010/10/26 
            if(!isMatched) { 
              if((deltaTGC1==-2 && +7==hiptDelta                 ) || 
                 (deltaTGC1==-1 && +4<=hiptDelta && hiptDelta<=+6)) {
                if(hiptisForward_out) { // For Forward  
                  if(hiPtStationEtaOut==+1) { // Side A
                    if(hiptChannel== 1 && (hiptDelta-deltaTGC1)-trackletChannel== 2) { 
                      isMatched = true; 
                    }
                  } else { // Side C
                    // Do nothing 
                  }
                } else { // For Endcap 
                  if(hiPtStationEtaOut>0) { // Side A
                    if((hiptChannel== 1 && (hiptDelta-deltaTGC1)-trackletChannel== 2 && hiptStationPhi%2==0) ||
                       (hiptChannel==25 && (hiptDelta-deltaTGC1)+trackletChannel==35 && hiptStationPhi%2==1)) { 
                      isMatched = true; 
                    }
                  } else { // Side C
                    if((hiptChannel== 8 && (hiptDelta-deltaTGC1)-trackletChannel== 2 && hiptStationPhi%2==0) ||
                       (hiptChannel==32 && (hiptDelta-deltaTGC1)+trackletChannel==35 && hiptStationPhi%2==1)) { 
                      isMatched = true; 
                    }
                  }
                }
              }
            }
          } else { // Wire
            if((-15<=hiptDelta) && (hiptDelta<=+15)) {
              isMatched = (-1<=deltaTGC1) && (deltaTGC1<=+2);
            } else {
              isMatched = false;
            }
          }
        } else { 
          // If there is no associated Traclet wire, TGC1 hits are associated without check of delta value.  
          isMatched = true; 
        }
        
        if(isMatched) {
          // Set the hit for the TgcCoinHierarchy
          bool isOK = pHierarchy->setHit(hit, isStrip, TgcCoinHierarchy::TGC1, m_bc);
          if(!isOK) showWARNINGforSetHit(hit, isStrip, TgcCoinHierarchy::TGC1);
          // Set the associated flag
          pTrigSectorComb->setHitAssociated(iHitTGC1, type, TgcCoinHierarchy::TGC1);
	  if(m_dummyHitChannel) continue; //////////////// 2011.05.01 Susumu Oda ??? 
        }
      }       

      if(m_dummyHitChannel) {
	StatusCode status = findDummyTGC1HitFromHiPt();
	if(status.isFailure()) return status;
      }
    }
      
    return StatusCode::SUCCESS;
  }

  StatusCode TgcCoinHierarchyFindTool::findTGC23HitFromTracklet(TgcCoinHierarchyTriggerSector* pTrigSectorComb,
                                                                TgcCoinHierarchy* pHierarchy,
                                                                const bool isStrip) {
    ATH_MSG_DEBUG("findTGC23HitFromTracklet()");

    if(!m_tgcCabling) {
      StatusCode status = getCabling();
      if(!status.isSuccess()) return status; 
    }

    // Get the corresponding TgcCoinData of Tracklet from the TgcCoinHierarchyTriggerSector
    if(!pHierarchy->hasCoincidence(TgcCoinData::TYPE_TRACKLET, isStrip)) {
      ATH_MSG_WARNING("There is no TRACKLET " << (isStrip ? "STRIP" : "WIRE") << " in findTGC23HitFromTracklet.");
      return StatusCode::SUCCESS;
    } 
    const TgcCoinData* tracklet = pHierarchy->getCoincidence(TgcCoinData::TYPE_TRACKLET, isStrip);
    if(!tracklet) {
      ATH_MSG_FATAL("Returned pointer to Tracklet is null. This should never happen.");
      return StatusCode::FAILURE;
    }

    TgcCoinHierarchy::TYPE type = (isStrip ? TgcCoinHierarchy::STRIP : TgcCoinHierarchy::WIRE);

    for(int iStation=TgcCoinHierarchy::TGC2; iStation<=TgcCoinHierarchy::TGC3; iStation++) {
      TgcCoinHierarchy::STATION station = (iStation==TgcCoinHierarchy::TGC2 ?
                                           TgcCoinHierarchy::TGC2 : TgcCoinHierarchy::TGC3);
      bool middle = (iStation==TgcCoinHierarchy::TGC2); // middle is TGC2, not middle is TGC3 (pivot) 

      // Get the LowPtCoincidenceID of Tracklet 
      Identifier trackletChannelId = (middle ? tracklet->channelIdIn() : tracklet->channelIdOut());
      int trackletSubDetectorID = -1;
      int trackletRodID = -1;
      int trackletSswID = -1;
      int trackletSbLoc = -1; // https://twiki.cern.ch/twiki/pub/Main/TgcDocument/sbloc-070701.xls, slbId of TgcRawData 
      int trackletBlock = -1; // block is 0 or 1, subMatrix of TgcRawData
      int trackletPos = -1; // pos is from 0 to 31, channel on SLB, position of TgcRawData
      bool foundgetLowPtCoincidenceIDfromOfflineIDTracklet 
        = m_tgcCabling->getLowPtCoincidenceIDfromOfflineID(trackletChannelId, 
                                                           trackletSubDetectorID, 
                                                           trackletRodID, 
                                                           trackletSswID, 
                                                           trackletSbLoc, 
                                                           trackletBlock, 
                                                           trackletPos, 
                                                           middle); 
      if(!foundgetLowPtCoincidenceIDfromOfflineIDTracklet) { 
        ATH_MSG_WARNING("getLowPtCoincidenceIDfromOfflineID failed for Tracklet with channelId=" << trackletChannelId); 
        return StatusCode::SUCCESS; 
      } 
      ATH_MSG_DEBUG("getLowPtCoincidenceIDfromOfflineID succeeded for Tracklet with channelId=" << trackletChannelId); 
      
      // Special case of hitPos==trackletPos+1 and trackletPos==31
      bool isEdge = false;
      int additionalTrackletSbLoc = -1;
      int additionalTrackletBlock = -1;
      int additionalTrackletPos = -1;
      if(trackletPos==31) { // Edge channel of SLBOut
        if(trackletBlock==0) { // block 0
          isEdge = true;
          additionalTrackletSbLoc = trackletSbLoc;
          additionalTrackletBlock = 1;
          additionalTrackletPos = 0;
        } else { // block 1
          if(!isStrip) { // Wire channels are consecutive  
            if(((tracklet->isForward() && // Forward (not the innermost SLB)
                 ((trackletSbLoc>=0 && trackletSbLoc<=2) || (trackletSbLoc>=8 && trackletSbLoc<=10))) || 
                (!tracklet->isForward() && // Endcap (not the innermost SLB)
                 (trackletSbLoc>=0 && trackletSbLoc<=8)))) { 
              isEdge = true;
              additionalTrackletSbLoc = trackletSbLoc+1;
              additionalTrackletBlock = 0;
              additionalTrackletPos = 0;
            }
          }
        } 
      }
    
      // Loop over all hits in the TgcCoinHierarchyTriggerSector
      unsigned int numHits = pTrigSectorComb->numHits(type, station);
      for(unsigned int iHit=0; iHit<numHits; iHit++) {
        const TgcPrepData* hit = pTrigSectorComb->getHit(iHit, type, station);
        if(!hit) {
          ATH_MSG_FATAL("Returned pointer to " << (middle ? "TGC2" : "TGC3") << 
                        " HIT is null. This should never happen.");
          return StatusCode::FAILURE;
        }
        
        // Get the LowPtCoincidenceID of Hit at TGC2
        int hitSubDetectorID = -1;
        int hitRodID = -1;
        int hitSswID = -1;
        int hitSbLoc = -1;
        int hitBlock = -1;
        int hitPos = -1;
        bool foundgetLowPtCoincidenceIDfromOfflineIDHit
          = m_tgcCabling->getLowPtCoincidenceIDfromOfflineID(hit->identify(),
                                                             hitSubDetectorID,
                                                             hitRodID,
                                                             hitSswID,
                                                             hitSbLoc,
                                                             hitBlock,
                                                             hitPos,
                                                             middle);
          
	if(m_dummyHitChannel) {
	  StatusCode status = findDummyTGC23HitFromTracklet();
	  if(status.isFailure()) return status;
	}
         
        if(!foundgetLowPtCoincidenceIDfromOfflineIDHit || 
           hitPos<0 || 
           hitSubDetectorID!=trackletSubDetectorID ||  
           hitRodID!=trackletRodID || 
           hitSswID!=trackletSswID || 
           !((hitSbLoc==trackletSbLoc && 
              hitBlock==trackletBlock && // Coincidence condition : a tracklet is created by two or one hit(s) 
              ((hitPos==trackletPos) || (hitPos==trackletPos+1))) || // in one station and has smaller pos.
             (isEdge && // Special case of hitPos==trackletPos+1 and trackletPos==31 
              hitSbLoc==additionalTrackletSbLoc && 
              hitBlock==additionalTrackletBlock && 
              hitPos==additionalTrackletPos))) { 
          continue;
        } 
        
        // Set the hit for the TgcCoinHierarchy
        bool isOK = pHierarchy->setHit(hit, isStrip, station, m_bc);
        if(!isOK) showWARNINGforSetHit(hit, isStrip, station);
        // Set the associated flag
        pTrigSectorComb->setHitAssociated(iHit, type, station);
      } 
    }

    return StatusCode::SUCCESS;
  }

  bool TgcCoinHierarchyFindTool::isPossibleTrackletIdStripAssociation(int trackletIdFromHiPtStrip, 
                                                                      int trackletIdFromTrackletStrip) {
    // Check possible relation between HiPt Strip and Tracklet Strip due to the HiPt board bug 
    // In chip=1, hitId=5, 6 become hitId=3, 4 due to the bug
    // If hitId=3, 4, an object with hitId=5, 6 is also created
    return ((trackletIdFromHiPtStrip==38 && trackletIdFromTrackletStrip==40) ||
            (trackletIdFromHiPtStrip==39 && trackletIdFromTrackletStrip==41) ||
            (trackletIdFromHiPtStrip==40 && trackletIdFromTrackletStrip==38) ||
            (trackletIdFromHiPtStrip==41 && trackletIdFromTrackletStrip==39));
  }

  void TgcCoinHierarchyFindTool::showWARNINGforSetCoincidence(const TgcCoinData* coinData, 
                                                              TgcCoinHierarchy::COINCIDENCE coincidence) {
    ATH_MSG_WARNING("setCoincidence failed. const TgcCoinData* " << coinData <<
                    " and/or const TgcCoinHierarchy::COINCIDENCE " << coincidence <<
                    " are wrong."); 
    
  }

  void TgcCoinHierarchyFindTool::showWARNINGforSetHit(const TgcPrepData* hitData, 
                                                      const bool isStrip,
                                                      TgcCoinHierarchy::STATION station) {
    ATH_MSG_WARNING("setHit failed. const TgcPrepData* " << hitData <<
                    " and/or const bool isStrip " << isStrip <<
                    " and/or TgcCoinHierarchy::STATION " << station << 
                    " are wrong."); 
    
  }

  bool TgcCoinHierarchyFindTool::getHiPtIds(const Identifier hiptChannelId, int &hiptSubDetectorID,
                                            int &hiptRodID, int &hiptSswID,
                                            int &hiptSbLoc, int &hiptSlbID) {
    int tmpChannelId = -1;
    bool found = m_tgcCabling->getReadoutIDfromOfflineID(hiptChannelId,
                                                         hiptSubDetectorID,
                                                         hiptRodID,
                                                         hiptSswID,
                                                         hiptSbLoc,
                                                         tmpChannelId);

    if(!found) { 
      ATH_MSG_WARNING("getReadoutIDfromOfflineID failed "); 
      return false; 
    }  

    int dummy_int[2] = {0, 0};
    bool dummy_bool[2] = {false, false};
    found = m_tgcCabling->getSLBIDfromReadoutID(dummy_int[0],
                                                dummy_bool[0],
                                                dummy_bool[1],
                                                dummy_int[1],
                                                hiptSlbID,
                                                hiptSubDetectorID,
                                                hiptRodID,
                                                hiptSswID,
                                                hiptSbLoc);

    if(!found) {
      ATH_MSG_WARNING("getSLBIDfromReadoutID failed. "); 
      return false;  
    }
                 
    return true;
  }

  int TgcCoinHierarchyFindTool::getDeltaBeforeConvert(const bool isStrip, const int delta) const
  {
    int DeltaBeforeConvert = 0;

    if(isStrip) {// strip
      switch(delta) {
      case  5: DeltaBeforeConvert =   6; break;
      case  6: DeltaBeforeConvert =   8; break;
      case  7: DeltaBeforeConvert =  10; break;
      case -4: DeltaBeforeConvert =  -5; break;
      case -5: DeltaBeforeConvert =  -7; break;
      case -6: DeltaBeforeConvert =  -9; break;
      case -7: DeltaBeforeConvert = -12; break;
      default: DeltaBeforeConvert = delta; break;
      }
    } else {// wire
      switch (delta) {
      case  11: DeltaBeforeConvert =  12; break;
      case  12: DeltaBeforeConvert =  14; break;
      case  13: DeltaBeforeConvert =  16; break;
      case  14: DeltaBeforeConvert =  18; break;
      case  15: DeltaBeforeConvert =  20; break;
      case -12: DeltaBeforeConvert = -13; break;
      case -13: DeltaBeforeConvert = -15; break;
      case -14: DeltaBeforeConvert = -17; break;
      case -15: DeltaBeforeConvert = -19; break;
      default:  DeltaBeforeConvert = delta; break;
      }
    }

    return DeltaBeforeConvert;
  }

  void TgcCoinHierarchyFindTool::getBitPosOutWire(int &hitId, int &hsub, int &/*chip*/, bool &/*isForward*/,
                                                  int &TGC3slbSubMatrix, int *bitpos_out) {
    if((hitId%2)==1) { // 1,3,5
      TGC3slbSubMatrix = 1;
      if(hsub==0) {
        bitpos_out[0] = BIT_POS_B_INPUT_SMALL_R_CH15; //  94
        bitpos_out[1] = BIT_POS_A_INPUT_SMALL_R_CH08; //  65
      } else if(hsub==1) {
        bitpos_out[0] = BIT_POS_B_INPUT_SMALL_R_CH07; // 102
        bitpos_out[1] = BIT_POS_A_INPUT_SMALL_R_CH00; //  73
      }
    } else { // 2,4,6
      TGC3slbSubMatrix = 0;
      if(hsub==0) {
        bitpos_out[0] = BIT_POS_B_INPUT_LARGE_R_CH15; //  78
        bitpos_out[1] = BIT_POS_A_INPUT_LARGE_R_CH08; //  49
      } else if(hsub==1) {
        bitpos_out[0] = BIT_POS_B_INPUT_LARGE_R_CH07; //  86
        bitpos_out[1] = BIT_POS_A_INPUT_LARGE_R_CH00; //  57
      }
    }
  }

  void TgcCoinHierarchyFindTool::getBitPosOutStrip(int &hitId, int &hsub,
                                                   int &TGC3slbSubMatrix, int *bitpos_out) {
    // This method is used by decodeHiPt
    if(hitId%2==1) { // 1,3,5::hitId:1-6 for EC, 2-3 for Fw
      TGC3slbSubMatrix = 0;
      if(hsub==0) {
        bitpos_out[0] = BIT_POS_B_INPUT_SMALL_PHI_FOR_A_FWD_C_BWD_CH15; //  94
        bitpos_out[1] = BIT_POS_A_INPUT_SMALL_PHI_FOR_A_FWD_C_BWD_CH08; //  65
      } else if(hsub==1) {
        bitpos_out[0] = BIT_POS_B_INPUT_SMALL_PHI_FOR_A_FWD_C_BWD_CH07; // 102
        bitpos_out[1] = BIT_POS_A_INPUT_SMALL_PHI_FOR_A_FWD_C_BWD_CH00; //  73
      }
    } else { // 2,4,6
      TGC3slbSubMatrix = 1;
      if(hsub==0) {
        bitpos_out[0] = BIT_POS_B_INPUT_LARGE_PHI_FOR_A_FWD_C_BWD_CH15; //  78 
        bitpos_out[1] = BIT_POS_A_INPUT_LARGE_PHI_FOR_A_FWD_C_BWD_CH08; //  49
      } else if(hsub==1) {
        bitpos_out[0] = BIT_POS_B_INPUT_LARGE_PHI_FOR_A_FWD_C_BWD_CH07; //  86
        bitpos_out[1] = BIT_POS_A_INPUT_LARGE_PHI_FOR_A_FWD_C_BWD_CH00; //  57
      }
    }
  }

  void TgcCoinHierarchyFindTool::getBitPosInWire(bool &isForward, const int *bitpos_o, const int slbId_o, const int &sector, 
                                                 const int DeltaBeforeConvert, int *sbLoc_in, int *bitpos_i, int *rdochannel)
  {
    // This method is used by decodeHiPt 
    const int NUM_SLBID_SBLOC_OFFSET_WT = 8; 
    const int WD_MAP_SIZE = 64;

    int slbId_in[2] = {0, 0};
    int slbchannel_in[2] = {0, 0};

    int rdochIn_max = 0;
    int rdochIn_min = 0;
    int offset_dt = 0;
    if(!isForward) { // EWT
      rdochIn_max = 665;
      rdochIn_min = 78;
      offset_dt = 32;
    } else { // FWT
      rdochIn_max = 312;
      rdochIn_min = 0;
      offset_dt = 0;
    }

    int slbchannel_o[2] = {0, 0}; 
    int tmp_rdochannel_i = 0;

    for(int i=0; i<2; i++) {
      slbchannel_o[i] = getchannel(bitpos_o[i], TgcRawData::SLB_TYPE_DOUBLET_WIRE);
      tmp_rdochannel_i = WD_MAP_SIZE*slbId_o + slbchannel_o[i] + DeltaBeforeConvert + offset_dt; 
 
      if(tmp_rdochannel_i>rdochIn_max) {
        tmp_rdochannel_i = rdochIn_max;
      } else if(tmp_rdochannel_i<rdochIn_min) {
        tmp_rdochannel_i = rdochIn_min;
      }
      slbId_in[i]   = tmp_rdochannel_i /WT_MAP_SIZE;

      sbLoc_in[i]   = slbId_in[i];
      if(sector%2==1) { 
        sbLoc_in[i]   += NUM_SLBID_SBLOC_OFFSET_WT;
      }

      slbchannel_in[i]   = tmp_rdochannel_i %WT_MAP_SIZE;
 
      bitpos_i[i]  = getbitpos(slbchannel_in[i], TgcRawData::SLB_TYPE_TRIPLET_WIRE);

      rdochannel[i] = tmp_rdochannel_i;
    }

  }

  void TgcCoinHierarchyFindTool::getBitPosInStrip(int &sector, int &chip, const int *bitpos_o, 
                                                  const int DeltaBeforeConvert, int &sbLoc_i, int *bitpos_i, int *slbchannel)
  {
    // This method is used by decodeHiPt 
    //*** get bitpos for TGC1 Station ***//
    // ST
    int rdochIn_max = 64-1;
    int rdochIn_min = 0;
 
    if((sector%2)==0) { 
      if(chip==0) {
        sbLoc_i = 16; // EST0 (phi0 and phi2) or FST0 (phi0)
      } else if(chip==1) {
        sbLoc_i = 17; // EST1 (phi0 and phi2)
      }
    } else {
      if(chip==0) {
        sbLoc_i = 24; // EST0 (phi1 and phi3) or FST0 (phi2)
      } else if(chip==1) {
        sbLoc_i = 25; // EST1 (phi1 and phi3
      }
    }

    int slbchannel_o[2] = {0, 0};
    int slbchannel_i[2] = {0, 0};
 
    for(int i=0; i<2; i++) {
      slbchannel_o[i] = getchannel(bitpos_o[i], TgcRawData::SLB_TYPE_DOUBLET_STRIP);
      slbchannel_i[i] = slbchannel_o[i] + DeltaBeforeConvert;

      if(slbchannel_i[i]>rdochIn_max) {
        slbchannel_i[i] = rdochIn_max;
      } else if(slbchannel_i[i]<rdochIn_min) {
        slbchannel_i[i] = rdochIn_min;
      }

      bitpos_i[i] = getbitpos(slbchannel_i[i], TgcRawData::SLB_TYPE_TRIPLET_STRIP);
      slbchannel[i] =  slbchannel_i[i];
    }

  }

  int TgcCoinHierarchyFindTool::getchannel(int bitpos, TgcRawData::SlbType slbType)
  {
    int input = -1;
    if(     (bitpos<=BIT_POS_A_INPUT_ORIGIN) && (bitpos>BIT_POS_A_INPUT_ORIGIN-BIT_POS_INPUT_SIZE)) input = 2; // A-Input
    else if((bitpos<=BIT_POS_B_INPUT_ORIGIN) && (bitpos>BIT_POS_B_INPUT_ORIGIN-BIT_POS_INPUT_SIZE)) input = 1; // B-Input
    else if((bitpos<=BIT_POS_C_INPUT_ORIGIN) && (bitpos>BIT_POS_C_INPUT_ORIGIN-BIT_POS_INPUT_SIZE)) input = 0; // C-Input
    // D-Input is not implemented yet. 
    else return -1; // Invalid bitpos
    if(input==0 && slbType!=TgcRawData::SLB_TYPE_TRIPLET_WIRE) return -1; // Only Wire Triplet has C-input.
   
    int channel = 1-BIT_POS_INPUT_SIZE;
    if(slbType==TgcRawData::SLB_TYPE_TRIPLET_WIRE) {
      if(     input==2) channel += BIT_POS_A_INPUT_ORIGIN; // A-input
      else if(input==1) channel += BIT_POS_B_INPUT_ORIGIN; // B-input
      else              channel += BIT_POS_C_INPUT_ORIGIN; // C-input
      channel = 3*(bitpos - channel) + input;
      return channel; // C(0)->B(1)->A(2)->C(3)->B(4)->A(5)-> ... ->C(96-3)->B(96-2)->A(96-1)
    } else if(slbType==TgcRawData::SLB_TYPE_TRIPLET_STRIP ||
              slbType==TgcRawData::SLB_TYPE_DOUBLET_WIRE  ||
              slbType==TgcRawData::SLB_TYPE_DOUBLET_STRIP) {
      if(input==2) channel += BIT_POS_A_INPUT_ORIGIN; // A-input
      else         channel += BIT_POS_B_INPUT_ORIGIN; // B-input
      channel = 2*(bitpos - channel) + input-1;
      return channel; // B(0)->A(1)->B(2)->A(3)-> ... ->B(64-2)->A(64-1)
    } else {
      return -1;
    }
  }

  int TgcCoinHierarchyFindTool::getchannelfromHitStrip(int bitpos, TgcRawData::SlbType slbType) {
    int input = -1;
    if(     (bitpos<=BIT_POS_A_INPUT_ORIGIN) && (bitpos>BIT_POS_A_INPUT_ORIGIN-BIT_POS_INPUT_SIZE)) input = 3; // A-Input
    else if((bitpos<=BIT_POS_B_INPUT_ORIGIN) && (bitpos>BIT_POS_B_INPUT_ORIGIN-BIT_POS_INPUT_SIZE)) input = 2; // B-Input
    else if((bitpos<=BIT_POS_C_INPUT_ORIGIN) && (bitpos>BIT_POS_C_INPUT_ORIGIN-BIT_POS_INPUT_SIZE)) input = 1; // C-Input
    else if((bitpos<=BIT_POS_D_INPUT_ORIGIN) && (bitpos>BIT_POS_D_INPUT_ORIGIN-BIT_POS_INPUT_SIZE)) input = 0; // D-Input
    else return -1; // Invalid bitpos
    if(slbType!=TgcRawData::SLB_TYPE_TRIPLET_STRIP) return -1; // Only Wire Triplet has C-input.
   
    int channel = 1-BIT_POS_INPUT_SIZE;
    if(input==3) {
      channel += BIT_POS_A_INPUT_ORIGIN; // A-input
      channel = 2*(bitpos - channel) + input-2; // +1
    } else if(input==2) {
      channel += BIT_POS_B_INPUT_ORIGIN; // B-input
      channel = 2*(bitpos - channel) + input-2; // +0
    } else if(input==1) {
      channel += BIT_POS_A_INPUT_ORIGIN;  //C-input = A-input
      channel = 2*(bitpos - (BIT_POS_C_INPUT_ORIGIN - BIT_POS_A_INPUT_ORIGIN) - channel) + input;
    } else {
      channel += BIT_POS_B_INPUT_ORIGIN; //D-input = B-input
      channel = 2*(bitpos - (BIT_POS_D_INPUT_ORIGIN - BIT_POS_B_INPUT_ORIGIN) - channel) + input;
    }
    return channel; // B(0)->A(1)->B(2)->A(3)-> ... ->B(64-2)->A(64-1)
  }

  int TgcCoinHierarchyFindTool::getbitpos(int channel, TgcRawData::SlbType slbType) {
    int bitpos = -BIT_POS_INPUT_SIZE +1;
    if(slbType==TgcRawData::SLB_TYPE_TRIPLET_WIRE) {
      if(channel<0 || channel>=WT_MAP_SIZE) return -1; // Invalid channel
 
      if(     channel%3==0) bitpos += BIT_POS_C_INPUT_ORIGIN; // C-Input
      else if(channel%3==1) bitpos += BIT_POS_B_INPUT_ORIGIN; // B-Input
      else                  bitpos += BIT_POS_A_INPUT_ORIGIN; // A-Input
      return bitpos + channel/3;
    } else if(slbType==TgcRawData::SLB_TYPE_TRIPLET_STRIP || 
              slbType==TgcRawData::SLB_TYPE_DOUBLET_STRIP) {
      if(channel<0 || channel>=ST_MAP_SIZE) return -1; // Invalid channel, SD_MAP_SIZE is equal to ST_MAP_SIZE. 
 
      if(channel%2==0) bitpos += BIT_POS_B_INPUT_ORIGIN; // B-Input
      else             bitpos += BIT_POS_A_INPUT_ORIGIN; // A-Input
      return bitpos + channel/2;
    } else if(slbType==TgcRawData::SLB_TYPE_DOUBLET_WIRE) {
      if(channel<0 || channel>=WD_MAP_SIZE) return -1; // Invalid channel
 
      if(channel%2==0) bitpos += BIT_POS_B_INPUT_ORIGIN; // B-Input
      else             bitpos += BIT_POS_A_INPUT_ORIGIN; // A-Input
      return bitpos + channel/2;
    } else {
      return -1;
    }
  }

  bool TgcCoinHierarchyFindTool::isHiPtEndcapStripCombination(int hiPtStationEtaOut, int tgc1HitStationEta) const 
  {
    // Valid |stationEta| for TGC3 is between 1 and 5
    hiPtStationEtaOut = abs(hiPtStationEtaOut);
    if(hiPtStationEtaOut==0 || hiPtStationEtaOut>NUM_TGC3_ENDCAP) return false; 
    
    // Valid |stationEta| for TGC1 is between 1 and 4
    tgc1HitStationEta = abs(tgc1HitStationEta);
    if(tgc1HitStationEta==0 || tgc1HitStationEta>NUM_TGC1_ENDCAP) return false;
    
    // http://hepg.sdu.edu.cn/~atlas/TGC/doc/MuonEndcap_rev01.pdf
    // http://www.icepp.s.u-tokyo.ac.jp/~kubota/public/HPT20091013/endcap_S_data_flow_vol2.GIF 

    static const bool combination[NUM_TGC3_ENDCAP][NUM_TGC1_ENDCAP] = {
      //  EST-1         EST-0 
      //17,16  15,14, 9,8,   7,6 <-- LVDS
      // 1      2      3      4  <-- TGC1 |stationEta|
      {true , true , false, false}, // TGC3 |stationEta|=1, ESD1-4, chip1 
      //      ^^^^ the second true is to cope with the HPT ES board chip1 bug 
      {true , true , false, false}, // TGC3 |stationEta|=2, ESD1-3, chip1
      {false, true , true , false}, // TGC3 |stationEta|=3, ESD1-2, chip0
      {false, true , true , true }, // TGC3 |stationEta|=4, ESD0-1, chip0 (middle matrix)
      {false, false, true , true }  // TGC3 |stationEta|=5, ESD0-0, chip0 
    }; 

    return combination[hiPtStationEtaOut-1][tgc1HitStationEta-1];
  }

  int TgcCoinHierarchyFindTool::getSubFromRoiNumber(const int roi, const bool isStrip, const bool isForward) const 
  {
    int sub = -1;
    // Information from Takashi Hayakawa (Kobe University)  
    if(isStrip) { // Strip 
      // sub :  0  1  0  1
      // ------------------
      // roi :  0  1  2  3
      //        4  5  6  7
      //        8  9 10 11
      //       12 13 14 15
      //       ... ...
      sub = roi%2; 
    } else { // Wire 
      if(isForward) { // Forward
        // roi           sub
        // ------------------
        //  0  1  2  3 : 0
        //  4  5  6  7 : 1
        //  8  9 10 11 : 0
        // 12 13 14 15 : 1
        // ... ...
        sub = (roi/4)%2; 
      } else { // Endcap 
        // roi           sub
        // ------------------
        //  0  1  2  3 : 1
        //  4  5  6  7 : 0
        //  8  9 10 11 : 1
        // 12 13 14 15 : 0
        // ... ...
        sub = (roi/4+1)%2;
      }
    }
    return sub;
  }

  StatusCode TgcCoinHierarchyFindTool::getCabling() {
    // get TGC cablingSvc
    const ITGCcablingServerSvc* TgcCabGet = 0;
    StatusCode sc = service("TGCcablingServerSvc", TgcCabGet, true);
    if(!sc.isSuccess()) {
      msg(sc.isFailure() ? MSG::FATAL : MSG::ERROR) << "Could not get TGCcablingServerSvc !" << endmsg;
      return sc;
    }
    
    sc = TgcCabGet->giveCabling(m_tgcCabling);
    if(!sc.isSuccess()) {
      msg(sc.isFailure() ? MSG::FATAL : MSG::ERROR) << "Could not get ITGCcablingSvc from Server!" << endmsg;
      m_tgcCabling = 0;
      return sc;
    } else {
      ATH_MSG_DEBUG("Found the ITGCcablingSvc.");
    }
    
    return sc;
  }

  StatusCode TgcCoinHierarchyFindTool::readMask2File() {
    // Open and read a TGC dummyhit channel file
    std::string dummyfile = PathResolver::find_file(m_mask2FileName, "DATAPATH");
    if(dummyfile.size()==0){
      ATH_MSG_FATAL("mask file is not found");
      return StatusCode::FAILURE;
    }
    m_dummyhit_file.open(dummyfile.c_str());
    if(m_dummyhit_file.fail()){
      ATH_MSG_FATAL("Could not open mask file");
      return StatusCode::FAILURE;
    }
    char ss[256];
    int nB=0;
    while(m_dummyhit_file.getline(ss,256)){
      std::stringstream st;
      st<<ss;
      if(ss[0]=='B'){
	nB++;
	if(nB==2){
	  ATH_MSG_INFO("Done to read mask file");
	  break;
	}
      }
      else{
	std::string buf_isAside;
	int buf_sector;
	int buf_triggerSectorId;
	std::string buf_isForward;
	std::string buf_isStrip;
	int buf_layer;
	int buf_chamber;
	int buf_channel;
	st >> buf_isAside >> buf_sector >> buf_triggerSectorId >> buf_isForward
	   >> buf_isStrip >> buf_layer >> buf_chamber >> buf_channel;
	DummyHitChannelToOnlineID dummyhit( buf_isAside,
					    buf_sector,
					    buf_triggerSectorId,
					    buf_isForward,
					    buf_isStrip,
					    buf_layer,
					    buf_chamber,
					    buf_channel         );
	m_dummyhit_subsystemNumber.push_back(dummyhit.GetSubsystemNumber());
	m_dummyhit_octantNumber.push_back(dummyhit.GetOctantNumber());
	m_dummyhit_moduleNumber.push_back(dummyhit.GetModuleNumber());
	m_dummyhit_layerNumber.push_back(dummyhit.GetLayerNumber());
	m_dummyhit_rNumber.push_back(dummyhit.GetRNumber());
	m_dummyhit_wireOrStrip.push_back(dummyhit.GetWireOrStrip());
	m_dummyhit_channelNumber.push_back(dummyhit.GetChannelNumber());
      }
    }
    ATH_MSG_INFO("# of dummy hit channels is : " << m_dummyhit_subsystemNumber.size());
    m_dummyhit_file.close();

    return StatusCode::SUCCESS;
  }

  StatusCode TgcCoinHierarchyFindTool::findDummyTGC1HitFromHiPt() {
    if(!m_tgcCabling) {
      StatusCode status = getCabling();
      if(!status.isSuccess()) return status;
    }

    /*There are dummyhit channels in TGC1*/
    /*Compare with dummyhit channel*/
    /*
    for(unsigned int n_dumChan=0; n_dumChan<m_dummyhit_layerNumber.size(); n_dumChan++) {
      if(m_dummyhit_layerNumber.at(n_dumChan)==0 ||
	 m_dummyhit_layerNumber.at(n_dumChan)==1 ||
	 m_dummyhit_layerNumber.at(n_dumChan)==2) {
	int dummyhitSubDetectorID = -1;
	int dummyhitRodID = -1;
	int dummyhitSswID = -1;
	int dummyhitSbLoc = -1;
	int dummyhitBitpos = -1;
	bool foundReadoutIDfromOnlineIDDummyHit = m_tgcCabling->getReadoutIDfromOnlineID(dummyhitSubDetectorID,
											 dummyhitRodID,
											 dummyhitSswID,
											 dummyhitSbLoc,
											 dummyhitBitpos,
											 m_dummyhit_subsystemNumber.at(n_dumChan),
											 m_dummyhit_octantNumber.at(n_dumChan),
											 m_dummyhit_moduleNumber.at(n_dumChan),
											 m_dummyhit_layerNumber.at(n_dumChan),
											 m_dummyhit_rNumber.at(n_dumChan),
											 m_dummyhit_wireOrStrip.at(n_dumChan),
											 m_dummyhit_channelNumber.at(n_dumChan),
											 false);
	if(!foundReadoutIDfromOnlineIDDummyHit) {
	  ATH_MSG_WARNING("getReadoutIDfromOnlineID failed for Dummy Hit ");
	  return StatusCode::SUCCESS; 
	} 
              
	int tmp_dummyhit_int[2] = {0, 0};
	bool tmp_dummyhit_bool[2] = {false, false};
	int dummyhitSlbID = -1;
	bool foundgetSLBIDfromReadoutIDDummyHit = m_tgcCabling->getSLBIDfromReadoutID(tmp_dummyhit_int[0],
										      tmp_dummyhit_bool[0],
										      tmp_dummyhit_bool[1],
										      tmp_dummyhit_int[1],
										      dummyhitSlbID,
										      dummyhitSubDetectorID,
										      dummyhitRodID,
										      dummyhitSswID,
										      dummyhitSbLoc);
	
	if(!foundgetSLBIDfromReadoutIDDummyHit) {
	  ATH_MSG_WARNING("getSLBIDfromReadoutIDHit failed for Dummy Hit"); 
	  return StatusCode::SUCCESS;  
	}
	
	int dummyhitSlbChannel = -1;
	if(!isStrip) { // wire
	  int tmp_dummyhitslbchannel = getchannel(dummyhitBitpos, TgcRawData::SLB_TYPE_TRIPLET_WIRE);
	  dummyhitSlbChannel = WT_MAP_SIZE * dummyhitSlbID + tmp_dummyhitslbchannel;
	  //Endcap RdoChannel is from 78 to 665
	  //Forward RdoChannel is from 0 to 312
	} 
	else { // strip
	  int tmp_dummyhitslbchannel = getchannelfromHitStrip(dummyhitBitpos, TgcRawData::SLB_TYPE_TRIPLET_STRIP);
	  dummyhitSlbChannel = tmp_dummyhitslbchannel;
	}
	if((hiptSlbChannel[0]-2<=dummyhitSlbChannel) && (hiptSlbChannel[1]+2>=dummyhitSlbChannel)) {
	  bool isDummyHitMatched = false;
	  if(hasTracklet) { 
	    int deltaTGC1 = trackletPos-dummyhitSlbChannel;
	    if(isStrip) { // Strip 
	      if(-7==hiptDelta || hiptDelta==7) {
		isDummyHitMatched = (-1<=deltaTGC1) && (deltaTGC1<=+1);
	      } else if((-6<=hiptDelta) && (hiptDelta<=+6)) { 
		isDummyHitMatched = ( 0<=deltaTGC1) && (deltaTGC1<=+1); 
	      } else {
		isDummyHitMatched = false;
	      }
	    } // Strip 
	    else { // Wire
	      if((-15<=hiptDelta) && (hiptDelta<=+15)) {
		isDummyHitMatched = (-1<=deltaTGC1) && (deltaTGC1<=+2);
	      }
	      else {
		isDummyHitMatched = false;
	      }// Wire
	    }
	  } else { 
	    // If there is no associated Traclet wire, TGC1 hits are associated without check of delta value.  
	    isDummyHitMatched = true; 
	  }
	  if(isDummyHitMatched) {
	    // Set the hit for the TgcCoinHierarchy
	    bool isOK = pHierarchy->setHit(hit, isStrip, TgcCoinHierarchy::TGC1);
	    if(!isOK) showWARNINGforSetHit(hit, isStrip, TgcCoinHierarchy::TGC1);
	    // Set the associated flag
	    pTrigSectorComb->setHitAssociated(iHitTGC1, type, TgcCoinHierarchy::TGC1);
	    break;
	  }
	}
      }
    }
    */
    
    return StatusCode::SUCCESS;
  }

  StatusCode TgcCoinHierarchyFindTool::findDummyTGC23HitFromTracklet() {
    if(!m_tgcCabling) {
      StatusCode status = getCabling();
      if(!status.isSuccess()) return status;
    }

    /*There are dummyhit channels in TGC2 and TGC3*/
    /*Compare with dummyhit channel*/
    /*
    int subsystemNumber=0;
    int octantNumber=-1;
    int moduleNumber=-1;
    int layerNumber=-1;
    int rNumber=-1;
    int wireOrStrip=-1;
    int channelNumber=-1;

    bool foundOnlineIDfromOfflineID = m_tgcCabling->getOnlineIDfromOfflineID(trackletChannelId,
									     subsystemNumber,
									     octantNumber,
									     moduleNumber,
									     layerNumber,
									     rNumber,
									     wireOrStrip,
									     channelNumber);
    bool dummy_flag = false;
    if(foundOnlineIDfromOfflineID) {
      for(unsigned int n_dumChan=0; n_dumChan<m_dummyhit_channelNumber.size(); n_dumChan++){
	if(   channelNumber==m_dummyhit_channelNumber.at(n_dumChan)
	   && moduleNumber==m_dummyhit_moduleNumber.at(n_dumChan)
	   && octantNumber==m_dummyhit_octantNumber.at(n_dumChan)
	   && layerNumber==m_dummyhit_layerNumber.at(n_dumChan)
	   && rNumber==m_dummyhit_rNumber.at(n_dumChan)
	   && wireOrStrip==m_dummyhit_wireOrStrip.at(n_dumChan)
	   && subsystemNumber==m_dummyhit_subsystemNumber.at(n_dumChan)) {
	  dummy_flag = true;
	  break;
	} 
      }
      if(dummy_flag){ // Dummyhit channel cause bad hit
	// Set the hit for the TgcCoinHierarchy
	bool isOK = pHierarchy->setHit(hit, isStrip, station);
	if(!isOK) showWARNINGforSetHit(hit, isStrip, station);
	// Set the associated flag
	pTrigSectorComb->setHitAssociated(iHit, type, station);
	continue;
      }
    }
    */

    return StatusCode::SUCCESS;
  }
  
} // end of namespace Trigger
