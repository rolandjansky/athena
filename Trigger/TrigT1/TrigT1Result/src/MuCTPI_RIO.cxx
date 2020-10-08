/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi/Athena include(s):
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "TrigT1Result/MuCTPI_RIO.h"

/**
 * There are so many parameters defined in the class, that no constructor that would actually set
 * any of them would make sense. So this one just initialises everything to zero, in the assumption
 * that everything will be set by the "setter" methods.
 */
MuCTPI_RIO::MuCTPI_RIO()
  : m_headerSourceId( 0 ), m_headerRunNumber( 0 ), m_headerLVL1ID( 0 ), m_headerBCID( 0 ),
    m_headerLVL1TriggerType( 0 ), m_headerDetEventType( 0 ), m_headerNDataWords( 0 ),
    m_headerNStatusWords( 0 ), m_bcId( 0 ), m_sum() {

}


/**
 * This function sets the muon candidate multiplicity sums in the 6 p<sub>T</sub> thresholds.
 *
 * @see MuCTPI_RIO::getSum()
 * @param sum  constant pointer to an array holding 6 uint16_t values
 */
void MuCTPI_RIO::setSum( const uint16_t* sum ) {
  for( int i = 0; i < N_SUM; ++i ) m_sum[ i ] = *( sum + i );
  return;
}

/**
 * Function returning the 3-bit number of muon candidates in threshold "index".
 * <strong>Be aware that index is defined from 0 to 5, not from 1 to 6 as the actual thresholds
 * are numbered!</strong>
 *
 * @param index   Index of the p<sub>T</sub> threshold to get the multiplicity sum for
 * @return        The 3-bit number of muon candidates at the given index
 */
uint16_t MuCTPI_RIO::getSum( const int index ) const {
  if( index >= N_SUM ) return 0;
  return m_sum[ index ];
}

/**
 * Function that <strong>adds</strong> one more muon candidate to the stored list.
 *
 * @param bcId             3-bit BCID from the muon candidate word
 * @param pTval            p<sub>T</sub> threshold value in GeV taken from the configuration
 * @param pTnum            p<sub>T</sub> threshold number from the muon candidate word
 * @param eta              &eta; coordinate of the muon candidate
 * @param phi              &phi; coordinate of the muon candidate
 * @param secID            8-bit sector ID from the muon candidate word
 * @param sysId            system ID ROI (Barrel=0, Endcap=1, Forward=2) 
 * @param hemisphere       Hemisphere (0: z < 0, 1: z > 0)
 * @param roiNum           RoI number from the muon candidate word
 * @param accepted         flag showing whether the candidate was sent to the RoIB
 * @param first            flag showing whether the candidate had the highest p<sub>T</sub> in it's sector
 * @param duplicatedRoI    flag showing whether there was more than 1 muon candidate in this candidate's RoI
 * @param duplicatedSector flag showing whether there were more than 2 muon candidates in this candidate's sector
 */
void MuCTPI_RIO::setRoI( const uint16_t bcId, const uint16_t pTval, const uint16_t pTnum, const double eta, const double phi,
                         const uint16_t secID, const uint16_t sysId, const uint16_t hemisphere, const uint16_t roiNum, 
			 const bool accepted, const bool first, const bool duplicatedRoI, const bool duplicatedSector) {

  MyRoI roI( bcId, pTval,pTnum, eta, phi, secID, sysId, hemisphere, roiNum, accepted, first, duplicatedRoI, duplicatedSector);
  m_roI.push_back( roI );

  return;
}

/**
 * Function that returns the properties of a given RoI in the variables given to it.
 *
 * @param index            index of the RoI in question in the stored list
 * @param bcId             3-bit BCID from the muon candidate word
 * @param pTval            p<sub>T</sub> threshold value in GeV taken from the configuration
 * @param pTnum            p<sub>T</sub> threshold number from the muon candidate word
 * @param eta              &eta; coordinate of the muon candidate
 * @param phi              &phi; coordinate of the muon candidate
 * @param secID            8-bit sector ID from the muon candidate word
 * @param sysId            system ID ROI (Barrel=0, Endcap=1, Forward=2) 
 * @param hemisphere       Hemisphere (0: z < 0, 1: z > 0)
 * @param roiNum           RoI number from the muon candidate word
 * @param accepted         flag showing whether the candidate was sent to the RoIB
 * @param first            flag showing whether the candidate had the highest p<sub>T</sub> in it's sector
 * @param duplicatedRoI    flag showing whether there was more than 1 muon candidate in this candidate's RoI
 * @param duplicatedSector flag showing whether there were more than 2 muon candidates in this candidate's sector
 * @return                 true in case the candidate was found, false otherwise
 */
bool MuCTPI_RIO::getRoI( const int index, uint16_t &bcId, uint16_t &pTval, uint16_t& pTnum, double &eta, double &phi, 
			 uint16_t& secID, uint16_t& sysId, uint16_t& hemisphere, uint16_t& roiNum, bool &accepted, 
			 bool &first, bool &duplicatedRoI, bool &duplicatedSector) const {

  if( index >= getNRoI() ) return false;

  const MyRoI &roi = m_roI.at( index );
  bcId = roi.m_bcId;
  pTval = roi.m_pTvalue;
  pTnum = roi.m_pTnumber;
  eta = roi.m_eta;
  phi = roi.m_phi;
  secID = roi.m_sectorID;
  sysId = roi.m_sysId;
  hemisphere = (roi.m_hemisphere ? 1 : 0);
  roiNum = roi.m_roINumber;
  accepted = roi.m_accepted;
  first = roi.m_first;
  duplicatedRoI = roi.m_duplicatedRoI;
  duplicatedSector = roi.m_duplicatedSector;
  

  return true;
}

/**
 * This function is not responsible for actually printing the information, it just creates
 * a message stream to print the information to with MuCTPI_RIO::dumpData(MsgStream&) const.
 */
void MuCTPI_RIO::dumpData() const {

  IMessageSvc* msgSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  StatusCode sc = svcLoc->service( "MessageSvc", msgSvc );
  if( sc.isFailure() ) {
    return;
  }
  MsgStream log( msgSvc, "MuCTPI_RIO" );
  dumpData( log );

  return;
}

/**
 * Function printing the information stored in the object in an easy-to-read fashion.
 *
 * @param log  A message stream to print the information to
 */
void MuCTPI_RIO::dumpData( MsgStream& log ) const {

  log << MSG::DEBUG << "=================================================" << endmsg;
  log << MSG::DEBUG << "MUCTPI ROD Header / Trailer data" << endmsg;
  log << MSG::DEBUG << "Source ID               :  0x" << MSG::hex << getHeaderSourceId() << MSG::dec << endmsg;
  log << MSG::DEBUG << "Run number              :  " << getHeaderRunNumber() << endmsg;
  log << MSG::DEBUG << "Ext. LVL1 ID            :  " << getHeaderLVL1ID() << endmsg;
  log << MSG::DEBUG << "BCID                    :  " << getHeaderBCID() << endmsg;
  log << MSG::DEBUG << "Trigger type            :  " << getHeaderLVL1TriggerType() << endmsg;
  log << MSG::DEBUG << "Det. event type         :  " << getHeaderDetectorEventType() << endmsg;
  log << MSG::DEBUG << "No data words           :  " << getHeaderNumberDataWords() << endmsg;
  log << MSG::DEBUG << "No status words         :  " << getHeaderNumberStatusWords() << endmsg;
  for( uint32_t i = 0 ; i < getHeaderNumberStatusWords(); ++i ) {
    log << MSG::DEBUG << "Status word " << i << "           :  0x" << MSG::hex << getHeaderStatusWords()[i]
        << MSG::dec << endmsg;
  }
  log << MSG::DEBUG << "=================================================" << endmsg;

  std::vector<MyRoI>::const_iterator it = m_roI.begin();
  for( ; it != m_roI.end(); ++it ) it->dumpData( log );

  return;
}

/**
 * Constructor specifying all the variables which are stored in the class
 *
 * @param bcId             3-bit BCID from the muon candidate word
 * @param pTVal            p<sub>T</sub> threshold value in GeV taken from the configuration
 * @param pTnumber         p<sub>T</sub> threshold number from the muon candidate word
 * @param eta              &eta; coordinate of the muon candidate
 * @param phi              &phi; coordinate of the muon candidate
 * @param secID            8-bit sector ID from the muon candidate word
 * @param sysId            system ID (Barrel=0, Endcap=1, Forward=2) 
 * @param hemisphere       Hemisphere (0: z < 0, 1: z > 0)
 * @param roiNum           RoI number from the muon candidate word
 * @param accepted         flag showing whether the candidate was sent to the RoIB
 * @param first            flag showing whether the candidate had the highest p<sub>T</sub> in it's sector
 * @param duplicatedRoI    flag showing whether there was more than 1 muon candidate in this candidate's RoI
 * @param duplicatedSector flag showing whether there were more than 2 muon candidates in this candidate's sector
 */
MuCTPI_RIO::MyRoI::MyRoI( const uint16_t bcId, const uint16_t pTVal,const uint16_t pTnumber, const double eta, const double phi,const uint16_t secID, const uint16_t sysId , const uint16_t hemisphere, const uint16_t roiNum, const bool accepted, const bool first, const bool duplicatedRoI, const bool duplicatedSector)
  : m_bcId( bcId ), m_pTvalue( pTVal ), m_pTnumber( pTnumber ), m_eta( eta ), m_phi( phi ),
    m_sectorID( secID ), m_sysId( sysId ), m_hemisphere( hemisphere ? true : false ), m_roINumber( roiNum ), m_accepted( accepted ), 
    m_first( first ), m_duplicatedRoI( duplicatedRoI ), m_duplicatedSector( duplicatedSector ) {}

/**
 * This function is not responsible for actually printing the information, it just creates
 * a message stream to print the information to with MuCTPI_RIO::MyRoI::dumpData(MsgStream&) const.
 */
void MuCTPI_RIO::MyRoI::dumpData() const {

  IMessageSvc* msgSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  StatusCode sc = svcLoc->service( "MessageSvc", msgSvc );
  if( sc.isFailure() ) {
    return;
  }
  MsgStream log( msgSvc, "MuCTPI_RIO::RoI" );
  dumpData( log );

  return;
}

/**
 * Function printing the information stored in the object in an easy-to-read fashion.
 *
 * @param log  A message stream to print the information to
 */
void MuCTPI_RIO::MyRoI::dumpData( MsgStream& log ) const {

  log << MSG::DEBUG << "=================================================" << endmsg;
  log << MSG::DEBUG << "MUCTPI RoI data" << endmsg;
  log << MSG::DEBUG << "BCID                    :  " << m_bcId << endmsg;
  log << MSG::DEBUG << "Pt value                :  " << m_pTvalue << endmsg;
  log << MSG::DEBUG << "Pt threshold number     :  " << m_pTnumber << endmsg;
  log << MSG::DEBUG << "Eta                     :  " << m_eta << endmsg;
  log << MSG::DEBUG << "Phi                     :  " << m_phi << endmsg;
  log << MSG::DEBUG << "RoI number              :  " << m_roINumber << endmsg;
  log << MSG::DEBUG << "Sector ID               :  " << m_sectorID << endmsg;
  log << MSG::DEBUG << "System ID               :  " << m_sysId << endmsg;
  log << MSG::DEBUG << "Hemisphere              :  " << m_hemisphere << endmsg;
  log << MSG::DEBUG << "Flags " << endmsg;
  log << MSG::DEBUG << "Accepted                :  " << m_accepted << endmsg;
  log << MSG::DEBUG << "First                   :  " << m_first << endmsg;
  log << MSG::DEBUG << "Duplicated sector       :  " << m_duplicatedSector << endmsg;
  log << MSG::DEBUG << "Duplicated RoI          :  " << m_duplicatedRoI << endmsg;
  log << MSG::DEBUG << "=================================================" << endmsg;

  return;
}
