/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     LArBadFebMaskingTool
PACKAGE:  offline/Calorimeter/LArCellRec

AUTHORS: G.Unal
CREATION:  23 april 2009

PURPOSE:

********************************************************************/

#include "LArBadFebMaskingTool.h"

#include "CaloEvent/CaloCellContainer.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArOnlineID.h" 
#include "LArRecConditions/LArBadFeb.h"
#include "LArRecEvent/LArEventBitInfo.h"
#include "GaudiKernel/ThreadLocalContext.h"

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
/////////////////////////////////////////////////////////////////////

StatusCode LArBadFebMaskingTool::initialize()
{
  m_evt=0;
  m_mask=0;

  m_errorToMask = 0;
  if (m_maskParity)         m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::Parity);
  if (m_maskSampleHeader)   m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::SampleHeader);
  if (m_maskEVTID)          m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::EVTID);
  if (m_maskScacStatus)     m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::ScacStatus);
  if (m_maskScaOutOfRange)  m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::ScaOutOfRange);
  if (m_maskGainMismatch)   m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::GainMismatch);
  if (m_maskTypeMismatch)   m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::TypeMismatch);
  if (m_maskNumOfSamples)   m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::NumOfSamples);
  if (m_maskEmptyDataBlock) m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::EmptyDataBlock);
  if (m_maskDspBlockSize)   m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::DspBlockSize);
  if (m_maskCheckSum)       m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::CheckSum);
  if (m_maskMissingHeader)  m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::MissingHeader);
  if (m_maskBadGain)        m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::BadGain);

  ATH_MSG_INFO (" bit mask for errors to mask " << m_errorToMask);

  // initialize read handle keys
  ATH_CHECK(m_larFebErrorSummaryKey.initialize());
  ATH_CHECK( m_badFebKey.initialize());
  ATH_CHECK( m_cablingKey.initialize());
  ATH_CHECK(m_eventInfoKey.initialize());

  // retrieve identifier helpers
  ATH_CHECK( detStore()->retrieve(m_onlineID, "LArOnlineID") );
  ATH_CHECK( detStore()->retrieve(m_calo_id,"CaloCell_ID") );

  return StatusCode::SUCCESS;

}

StatusCode LArBadFebMaskingTool::finalize()
{
   ATH_MSG_INFO ("  ---- Summary from LArBadFebMaskingTool ");
   ATH_MSG_INFO ("   Number of events processed    " << m_evt);
   ATH_MSG_INFO ("   Number of masked Feb total    " << m_mask);
   float ratio=0.;
   if (m_evt>0) ratio=((float)(m_mask))/((float)(m_evt));
   ATH_MSG_INFO ("   Number of masked Feb per event " << ratio);

   return StatusCode::SUCCESS;
}

StatusCode LArBadFebMaskingTool::process (CaloCellContainer* theCont,
                                          const EventContext& ctx) const
{
  m_evt++;


  ATH_MSG_DEBUG (" in LArBadFebMaskingTool::process ");
  SG::ReadHandle<LArFebErrorSummary>larFebErrorSummary(m_larFebErrorSummaryKey, ctx);
  if (!larFebErrorSummary.isValid()) {
    ATH_MSG_WARNING ("Cannot retrieve Feb error summary with key " << m_larFebErrorSummaryKey.key() <<". Skip LArBadFebMaskingTool::process ");
    return StatusCode::SUCCESS;
  }

  // retrieve map of Feb-errors
  const std::map<unsigned int,uint16_t>& febMap = larFebErrorSummary->get_all_febs();

  ATH_MSG_DEBUG (" Number of Febs " << febMap.size());

  // catch cases of empty LAR container  => severe problem in decoding => flag event as in ERROR
  unsigned int nLar = theCont->nCellsCalo(CaloCell_ID::LAREM)+theCont->nCellsCalo(CaloCell_ID::LARHEC)+theCont->nCellsCalo(CaloCell_ID::LARFCAL);
  if (nLar==0) {
     ATH_MSG_DEBUG (" empty lar cell container ");
     SG::ReadHandle<xAOD::EventInfo> eventInfo (m_eventInfoKey, ctx);
     ATH_MSG_DEBUG (" set error bit for LAr for this event ");
     if (!eventInfo->updateErrorState(xAOD::EventInfo::LAr,xAOD::EventInfo::Error)) {
        ATH_MSG_WARNING (" cannot set error state for LAr ");
     }
     if (!eventInfo->updateEventFlagBit(xAOD::EventInfo::LAr,LArEventBitInfo::DATACORRUPTED)) {
      ATH_MSG_WARNING (" cannot set event bit info for LAr ");
     }
  }


  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey,ctx};
  const LArOnOffIdMapping* cabling=*cablingHdl;

  SG::ReadCondHandle<LArBadFebCont> badFebHdl{m_badFebKey,ctx};
  const LArBadFebCont* badFebs=*badFebHdl;


// loop over all Febs

  for (HWIdentifier febId : m_onlineID->feb_range()) {
      bool toMask1 = false;   // mask because of bad error
      bool inError = false;   // mask because Feb listed in database as being to mask

// for debug
      unsigned int ifeb = febId.get_identifier32().get_compact();
      ATH_MSG_DEBUG (" process Feb: " << ifeb);

      std::map<unsigned int,uint16_t>::const_iterator it1 = febMap.find(ifeb);
      if (it1 != febMap.end()) {
        uint16_t ierror = (*it1).second;
        if (ierror & m_errorToMask) toMask1=true;
        ATH_MSG_DEBUG (" ierror,toMask " << ierror << " " << toMask1 << " ");
      }


      LArBadFeb febstatus = badFebs->status(febId);
      inError = febstatus.inError();

      if (toMask1 || inError) {
         m_mask++;
         for (int ch=0; ch<128; ++ch) {
           HWIdentifier hwid = m_onlineID->channel_Id(febId, ch);
           if (cabling->isOnlineConnected(hwid)) {
              Identifier id = cabling->cnvToIdentifier( hwid);
              IdentifierHash theCellHashID = m_calo_id->calo_cell_hash(id);
              int index = theCont->findIndex(theCellHashID);
              if (index<0) {
                 ATH_MSG_DEBUG (" cell " << hwid.get_compact() << " " << id.get_compact() << " is not in the container ");
                 continue;
              }
              CaloCell* aCell = theCont->at(index);

              if (aCell) {
                ATH_MSG_DEBUG ("  mask cell hwid= " << hwid.get_compact() << " offlineid = " << id.get_compact()
                               << " " << aCell->ID().get_compact());
                aCell->setEnergy(0.);
                aCell->setTime(0.);
                uint16_t qua=0;
                aCell->setQuality(qua);
                uint16_t provenance = (aCell->provenance() | 0x0800);
                aCell->setProvenance(provenance);
              }

           }  // isConnected
         }    // loop over channels in Feb
      }       // toMask

  }       // loop over Febs in error


  return StatusCode::SUCCESS;
}
