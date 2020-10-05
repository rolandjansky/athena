/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi/Athena include(s):
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

// Local include(s):
#include "TrigT1Result/MuCTPI_DataWord_Decoder.h"

/**
 * The constructor only has to give an initial value to MuCTPI_DataWord_Decoder::m_dataWord stored in the object
 *
 * @param dataword  The 32-bit MuCTPI data word that you want to decode
 */
MuCTPI_DataWord_Decoder::MuCTPI_DataWord_Decoder( uint32_t dataword )
  : m_dataWord( dataword ) {

}

/**
 * @return flag showing whether there were more than 2 candidates in this candidate's sector
 */
uint16_t MuCTPI_DataWord_Decoder::getSectorOverflow() const {
  return ((m_dataWord >> MuCTPI_RDO::CAND_OVERFLOW_SHIFT) &
	  MuCTPI_RDO::CAND_OVERFLOW_MASK);
}

/**
 * @return flag showing whether there was more than 1 candidate in this candidate's RoI
 */
uint16_t MuCTPI_DataWord_Decoder::getRoiOverflow() const {
  return ((m_dataWord >> MuCTPI_RDO::ROI_OVERFLOW_SHIFT) &
	  MuCTPI_RDO::ROI_OVERFLOW_MASK);
}

/**
 * @return RoI number
 */
uint16_t MuCTPI_DataWord_Decoder::getRoiNumber() const {

  uint16_t roi = 0;
  switch( getSectorLocation() ) {
  case MuCTPI_RDO::ENDCAP:
    roi = (m_dataWord >> MuCTPI_RDO::ROI_SHIFT) & MuCTPI_RDO::ENDCAP_ROI_MASK;
    break;
  case MuCTPI_RDO::FORWARD:
    roi = (m_dataWord >> MuCTPI_RDO::ROI_SHIFT) & MuCTPI_RDO::FORWARD_ROI_MASK;
    break;
  case MuCTPI_RDO::BARREL:
    roi = (m_dataWord >> MuCTPI_RDO::ROI_SHIFT) & MuCTPI_RDO::BARREL_ROI_MASK;
    break;
  default:
    roi = 99;
    break;
  }

  return roi;
}

/**
 * @return 2-bit overlap bits given to the candidate by the Sector Logic
 */
uint16_t MuCTPI_DataWord_Decoder::getOverlapBits() const {

  uint16_t ol = 0;
  switch( getSectorLocation() ) {
  case MuCTPI_RDO::ENDCAP:
    ol = (m_dataWord >> MuCTPI_RDO::ENDCAP_OL_SHIFT) & MuCTPI_RDO::ENDCAP_OL_MASK;
    break;
  case MuCTPI_RDO::FORWARD:
    ol = 0;
    break;
  case MuCTPI_RDO::BARREL:
    ol = (m_dataWord >> MuCTPI_RDO::BARREL_OL_SHIFT) & MuCTPI_RDO::BARREL_OL_MASK;
    break;
  default:
    ol = 99;
    break;
  }

  return ol;
}

/**
 * @return name of the system that detected the muon candidate
 */
MuCTPI_RDO::SectorLocation MuCTPI_DataWord_Decoder::getSectorLocation() const {

  if( getSectorAddress() & MuCTPI_RDO::ENDCAP_ADDRESS_MASK ) {
    return MuCTPI_RDO::ENDCAP;
  } else if( getSectorAddress() & MuCTPI_RDO::FORWARD_ADDRESS_MASK ) {
    return MuCTPI_RDO::FORWARD;
  } else {
    return MuCTPI_RDO::BARREL;
  }
}

/**
 * @return 8-bit address of the sector that detected the candidate
 */
uint16_t MuCTPI_DataWord_Decoder::getSectorAddress() const {
  return ((m_dataWord >> MuCTPI_RDO::CAND_SECTOR_ADDRESS_SHIFT) &
	  MuCTPI_RDO::CAND_SECTOR_ADDRESS_MASK);
}

/**
 * @return number of the sector that detected the candidate
 */
uint16_t MuCTPI_DataWord_Decoder::getSectorID(bool newScheme) const {

  if( getSectorLocation() == MuCTPI_RDO::ENDCAP ) {
    return ((getSectorAddress() >> 1) & MuCTPI_RDO::ENDCAP_SECTORID_MASK);
  } 
  else if( getSectorLocation() == MuCTPI_RDO::BARREL ) {
    if(newScheme)
      return ((getSectorAddress() >> 1) & MuCTPI_RDO::BARREL_SECTORID_MASK);
    else{
      uint16_t id = (getSectorAddress() >> 1) & MuCTPI_RDO::BARREL_SECTORID_MASK;
      if( getHemisphere() ) id += 32;
      return id;
    }
  }
  else if( getSectorLocation() == MuCTPI_RDO::FORWARD ) {
    return ((getSectorAddress() >> 1) & MuCTPI_RDO::FORWARD_SECTORID_MASK);
  }
  return 0;
}

/**
 * @return hemisphere in which the candidate was detected
 */
uint16_t MuCTPI_DataWord_Decoder::getHemisphere() const {
  return (getSectorAddress() & MuCTPI_RDO::SECTOR_HEMISPHERE_MASK);
}

/**
 * @return 3-bit p<sub>T</sub> threshold of the candidate
 */
uint16_t MuCTPI_DataWord_Decoder::getPt() const {
  return ((m_dataWord >> MuCTPI_RDO::CAND_PT_SHIFT) &
	  MuCTPI_RDO::CAND_PT_MASK);
}

/**
 * @return 3-bit BCID fragment saved with the candidate
 */
uint16_t MuCTPI_DataWord_Decoder::getBCID() const {
  return ((m_dataWord >> MuCTPI_RDO::CAND_BCID_SHIFT) &
	  MuCTPI_RDO::CAND_BCID_MASK);
}

/**
 * @return flag showing whether the candidate had the highest p<sub>T</sub> in its sector
 */
uint16_t MuCTPI_DataWord_Decoder::getCandidateIsHighestPt() const {
  return ((m_dataWord >> MuCTPI_RDO::CAND_HIGHEST_PT_SHIFT) &
	  MuCTPI_RDO::CAND_HIGHEST_PT_MASK);
}

/**
 * @return flag showing whether the candidate was sent to the RoIB
 */
uint16_t MuCTPI_DataWord_Decoder::getSentRoi() const {
  return ((m_dataWord >> MuCTPI_RDO::CAND_SENT_ROI_SHIFT) &
	  MuCTPI_RDO::CAND_SENT_ROI_MASK);
}

/**
 * @returns The candidate's sign (0=negative, 1=positive, 100=undefined)
 */
uint16_t MuCTPI_DataWord_Decoder::getSign() const {

   // This bit is not defined for barrel candidates:
   if( getSectorLocation() == MuCTPI_RDO::BARREL ) {
      return 100;
   }

   return ( ( m_dataWord >> MuCTPI_RDO::CAND_TGC_CHARGE_SIGN_SHIFT ) &
            0x1 );
}

/**
 * @returns Flag showing if the candidate was vetoed in the multiplicity sum
 */
uint16_t MuCTPI_DataWord_Decoder::getVetoed() const {

   return ( ( m_dataWord >> MuCTPI_RDO::CAND_VETO_SHIFT )
            & 0x1 );
}

/**
 * This function is not responsible for actually printing the information, it just creates
 * a message stream to print the information to with MuCTPI_DataWord_Decoder::dumpData(MsgStream&) const.
 */
void MuCTPI_DataWord_Decoder::dumpData() const {

  IMessageSvc* msgSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service( "MessageSvc", msgSvc );
  if( sc.isFailure() ) {
    return;
  }
  MsgStream log( msgSvc, "MuCTPI_DataWord_Decoder" );
  dumpData( log );

  return;
}

/**
 * Function printing the decodable information of the MuCTPI data word in an easy-to-read fashion.
 *
 * @param log  A message stream to print the information to
 */
void MuCTPI_DataWord_Decoder::dumpData( MsgStream& log ) const {

  uint16_t sectorLocation = getSectorLocation();
  std::string locString = "BARREL";
  if( sectorLocation == MuCTPI_RDO::ENDCAP ) locString = "ENDCAP";
  else if( sectorLocation == MuCTPI_RDO::FORWARD ) locString = "FORWARD";
  uint16_t candHighestPt = getCandidateIsHighestPt();
  std::string isHigh = "NO";
  if( candHighestPt ) isHigh = "YES";
  uint16_t candSentRoi = getSentRoi();
  std::string sentRoi = "NO";
  if( candSentRoi ) sentRoi = "YES";
  uint16_t secOf = getSectorOverflow();
  std::string ofString = "NO";
  if( secOf ) ofString = "YES";
  uint16_t roiOf = getRoiOverflow();
  std::string roiString = "NO";
  if( roiOf ) roiString = "YES";
  uint16_t ol = getOverlapBits();
  std::string olString = "NO";
  if( ol ) olString = "YES";

  log << MSG::DEBUG << "Data word         :   0x" << MSG::hex << m_dataWord << MSG::dec << endmsg;
  log << MSG::DEBUG << "BCID              :   " << getBCID() << endmsg;
  log << MSG::DEBUG << "Pt                :   " << getPt() << endmsg;
  log << MSG::DEBUG << "Is highest Pt     :   " << isHigh << endmsg;
  log << MSG::DEBUG << "RoI ID            :   " << getRoiNumber() << endmsg;
  log << MSG::DEBUG << "Sent RoI          :   " << sentRoi << endmsg;
  log << MSG::DEBUG << "Sector address    :   0x" << MSG::hex << getSectorAddress() << MSG::dec << endmsg;
  log << MSG::DEBUG << "Sector ID         :   " << getSectorID() << endmsg;
  log << MSG::DEBUG << "Sector location   :   " << locString << endmsg;
  log << MSG::DEBUG << "Sector overflow   :   " << ofString << endmsg;
  log << MSG::DEBUG << "RoI overflow      :   " << roiString << endmsg;
  log << MSG::DEBUG << "Overlap found     :   " << olString << endmsg;
  log << MSG::DEBUG << "Hemisphere        :   " << getHemisphere() << endmsg;
  log << MSG::DEBUG << "=================================================" << endmsg;

  return;
}
