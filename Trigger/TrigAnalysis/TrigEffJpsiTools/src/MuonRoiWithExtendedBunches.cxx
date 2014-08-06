/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @date      $Date: 2010-09-30 22:13:27 +0200 (Thu, 30 Sep 2010) $
 * @version   $Revision: 324593 $
 */

/** @todo */
/** @warnings */

/*--------------------------------------------------------------------*
 * headers
 *--------------------------------------------------------------------*/
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/StoreGate.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigT1Result/MuCTPI_RDO.h"
#include "TrigT1Result/MuCTPIResult.h"
#include "TrigT1Result/MuCTPIRoI.h"
#include "TrigT1Result/MuCTPI_DataWord_Decoder.h"
#include "TrigT1Result/MuCTPI_MultiplicityWord_Decoder.h"
#include "TrigT1Result/RoIBResult.h"

#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigT1Interfaces/RecMuonRoiSvc.h"

#include "TrigConfL1Data/TriggerThreshold.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"

#include "MuonRoiWithExtendedBunches.h"


namespace MuComm {

/*--------------------------------------------------------------------*
 * constructor & destructor
 *--------------------------------------------------------------------*/
MuonRoiWithExtendedBunches::MuonRoiWithExtendedBunches(const std::string& name,
                                                       ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_muctpiInput("MUCTPI_RDO"),  // StoreGate key for MuCTPI_RDO
    m_roibInput("RoIBResult"),  // StoreGate key for ROIB::RoIBResult
    m_extendedMuonRoi("ExtendedMuonRoi"), // StoreGate key for LVL1_ROI with extneded BCs
    m_allowedBcArray(), // a list of bunch-offset wrt to the current BCID
    m_allowedBcSet(),
    m_l1PtNames(),  // a list of level1 muon trigger item names
    m_l1PtValues(), // a list of level1 muon trigger item pt values in MeV
    m_recRpcRoiSvc(LVL1::ID_RecRpcRoiSvc, name),
    m_recTgcRoiSvc(LVL1::ID_RecTgcRoiSvc, name)
{
    declareProperty("MuCTPIInput", m_muctpiInput);
    declareProperty("RoIBInput", m_roibInput);
    declareProperty("Lvl1_RoiName", m_extendedMuonRoi);
    declareProperty("AllowedBCs", m_allowedBcArray);
    declareProperty("L1PtNames", m_l1PtNames);
    declareProperty("L1PtValues", m_l1PtValues);
}


/*--------------------------------------------------------------------*
 * public methods
 *--------------------------------------------------------------------*/
StatusCode
MuonRoiWithExtendedBunches::initialize()
{
  if ((m_l1PtNames.size() != NUM_THRESHOLDS) or
      (m_l1PtValues.size() != NUM_THRESHOLDS)) {
    ATH_MSG_ERROR("MuonRoiWithExtendedBunches::initialize(): "
                  "setup failure: L1PtNames or L1PtValues"
                  << m_l1PtNames << " : " << m_l1PtValues);
    return StatusCode::FAILURE;
  }

  m_allowedBcSet.clear();
  for (size_t ii = 0; ii < m_allowedBcArray.size(); ii++) {
    m_allowedBcSet.insert(m_allowedBcArray.at(ii));
    ATH_MSG_INFO("MuonRoiWithExtendedBunches::initialize(): "
                 "adding bc diff = " << m_allowedBcArray.at(ii));
  }

  // Get the RPC RecRoI service
  CHECK(m_recRpcRoiSvc.retrieve());
  ATH_MSG_VERBOSE(LVL1::ID_RecRpcRoiSvc << " retrieved");

  // Get the TGC RecRoI service
  CHECK(m_recTgcRoiSvc.retrieve());
  ATH_MSG_VERBOSE(LVL1::ID_RecTgcRoiSvc << " retrieved");

  return StatusCode::SUCCESS;
}


StatusCode
MuonRoiWithExtendedBunches::execute()
{
  ATH_MSG_DEBUG("MuonRoiWithExtendedBunches::execute()");

  ROIB::RoIBResult* roibResult = getExtendedRoIBResult();
  if (roibResult == 0) {
    ATH_MSG_FATAL("MuonRoiWithExtendedBunches: getExtendedRoiBResult() ");
    return StatusCode::FAILURE;
  }

  LVL1_ROI* rois = new LVL1_ROI();
  CHECK(evtStore()->record(rois, m_extendedMuonRoi));
  ATH_MSG_VERBOSE("recorded LVL1_ROI with key " << m_extendedMuonRoi);

  std::vector<TrigConf::TriggerThreshold*> dummy;
  const std::vector<ROIB::MuCTPIRoI>& muctpiRois = roibResult->muCTPIResult().roIVec();
  std::vector<ROIB::MuCTPIRoI>::const_iterator cit = muctpiRois.begin();
  std::vector<ROIB::MuCTPIRoI>::const_iterator end = muctpiRois.end();
  ATH_MSG_VERBOSE("found " << muctpiRois.size() << " muctpiRois");
  for (; cit != end; cit++) {
    int thresholdNumber = cit->pt();
    if (thresholdNumber < 1 or thresholdNumber > 6) {
      ATH_MSG_FATAL("MuonRoiWithExtendedBunches: execute: "
                    "unexpected threshold number: " << thresholdNumber);
      return StatusCode::FAILURE;
    }

    uint32_t roiWord = cit->roIWord();
    LVL1::RecMuonRoI recRoi(roiWord,
                            m_recRpcRoiSvc.operator->(),
                            m_recTgcRoiSvc.operator->(),
                            &dummy);

    Muon_ROI roi(roiWord, recRoi.eta(), recRoi.phi(),
                 m_l1PtNames.at(thresholdNumber-1),
                 m_l1PtValues.at(thresholdNumber-1));

    rois->addMuonROI(roi);
  }
  ATH_MSG_VERBOSE("found " << rois->getMuonROIs().size() << " muon rois");

  CHECK(evtStore()->setConst(rois));
  ATH_MSG_VERBOSE("set const for LVL1_ROI with key " << m_extendedMuonRoi);

  delete roibResult;

  return StatusCode::SUCCESS;
}


/*--------------------------------------------------------------------*
 * private methods
 *--------------------------------------------------------------------*/
ROIB::RoIBResult*
MuonRoiWithExtendedBunches::getExtendedRoIBResult()
{
  ATH_MSG_DEBUG("MuonRoiWithExtendedBunches::getExtendedRoIBResult()");

  const EventInfo* eventInfo = 0;
  if (StoreGate::pointer()->retrieve(eventInfo).isFailure()) {
    ATH_MSG_WARNING("MuonRoiWithExtendedBunches: couldn't retrieve EventInfo");
    return 0;
  }
  const EventID* eventId = eventInfo->event_ID();
  const unsigned int eventBcid = eventId->bunch_crossing_id();

  // Create the RoI vector from the RDO data:
  const MuCTPI_RDO* muctpi_rdo = 0;
  if (evtStore()->retrieve(muctpi_rdo, m_muctpiInput).isFailure()) {
    ATH_MSG_WARNING("MuonRoiWithExtendedBunches: couldn't retrieve "
                    << m_muctpiInput);
    return 0;
  }
  ATH_MSG_VERBOSE("Retrieved the MuCTPI_RDO object with key: "
                  << m_muctpiInput );

  ATH_MSG_VERBOSE("Now creating the RoIs in extended bunches...");
  std::vector<ROIB::MuCTPIRoI> roi_vector;
  MuCTPI_MultiplicityWord_Decoder mwDecoder(muctpi_rdo->candidateMultiplicity());
  uint32_t currentBcid = mwDecoder.getBCID();
  if (currentBcid != (eventBcid & 0x7)) {
    ATH_MSG_WARNING("MuonRoiWithExtendedBunches: BCID mis-match: " << eventBcid << ":" << currentBcid
                    << " at " << eventId->run_number() << " " << eventId->event_number() << " "
                    << eventId->lumi_block() << " " << eventId->time_stamp() << " " << eventId->time_stamp_ns_offset());
    currentBcid = (eventBcid & 0x7);
  }

  // Loop over all "data words" and select the ones in streatched time window
  ATH_MSG_VERBOSE("found " << muctpi_rdo->dataWord().size() << " muctpi_rdo dataWords");

  std::vector<uint32_t>::const_iterator cit = muctpi_rdo->dataWord().begin();
  std::vector<uint32_t>::const_iterator end = muctpi_rdo->dataWord().end();
  for(; cit != end; ++cit) {
    MuCTPI_DataWord_Decoder dwDecoder(*cit);

    uint32_t bcid = dwDecoder.getBCID();
    int bcDiff = bcid - currentBcid;
    switch (eventBcid) {
      case 0: // cannot distinguish 3563 and 3, 3562 and 2, 3561 and 1
        if (bcid == 3) {
          bcDiff = -1;
        } else if (bcid == 2) {
          bcDiff = -2;
        } else if (bcid == 1) {
          bcDiff = -3;
        }
        break;

      case 1: // cannot distinguish 3563 and 3, 3562 and 2
        if (bcid == 3) {
          bcDiff = -2;
        } else if (bcid == 2) {
          bcDiff = -3;
        }
        break;

      case 2: // cannot distinguish 3563 and 3
        if (bcid == 3) {
          bcDiff = -3;
        }
        break;

      default:
        break;
    }

    if (abs(bcDiff) > 4) {
      bcDiff = (8 - abs(bcDiff)) * ((bcDiff < 0) ? 1 : -1);
    }

    ATH_MSG_DEBUG(std::dec << bcid << " : " << currentBcid);
    ATH_MSG_DEBUG("bcid diff = " << std::dec << bcDiff <<
                  " loc = " << dwDecoder.getSectorLocation());

    if (dwDecoder.getSectorLocation() == MuCTPI_RDO::BARREL) {  // BARREL
      ATH_MSG_VERBOSE("bcid diff in BARREL");
    } else {  // ENDCAP
      ATH_MSG_VERBOSE("bcid diff in ENDCAP");
    }
    if (m_allowedBcSet.find(bcDiff) == m_allowedBcSet.end()) continue;

    ATH_MSG_DEBUG("accepted");
    roi_vector.push_back(ROIB::MuCTPIRoI(RDOtoRoI(*cit)));
    ATH_MSG_VERBOSE( "  - Processed data word: 0x" << std::hex << std::setw(8)
                          << std::setfill('0') << *cit );
  }


  // Create the new MuCTPI result:
  const ROIB::RoIBResult* roibResult = 0;
  if (evtStore()->retrieve(roibResult, m_roibInput).isFailure()) {
    ATH_MSG_WARNING("MuonRoiWithExtendedBunches: couldn't retrieve "
                    << m_roibInput);
    return 0;
  }
  ATH_MSG_VERBOSE("Retrieved the old RoIBResult object with key:"
                  << m_roibInput);

  ROIB::MuCTPIResult newResult(roibResult->muCTPIResult().header(),
                               roibResult->muCTPIResult().trailer(),
                               roi_vector);

  // Create the new RoIBResult object:
  return new ROIB::RoIBResult(newResult,
                              roibResult->cTPResult(),
                              roibResult->jetEnergyResult(),
                              roibResult->eMTauResult());
}


/**
 * @param data_word Data word from the MuCTPI_RDO object
 * @returns The RoI word created from the data word
 */
uint32_t
MuonRoiWithExtendedBunches::RDOtoRoI(uint32_t data_word)
{
  // MuCTPI_RDO to MuCTPIRoI
  // ref: Fig 3.6 and Fig 3.8 in https://edms.cern.ch/file/248757/1/mirod.pdf
  return (((data_word & 0x8000000) >> 4) | ((data_word & 0x3fe0000) >> 3) |
          (data_word & 0x3fff));
}


} // namespace MuComm
