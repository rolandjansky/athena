/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


// STL include(s):
#include <string>

// Gaudi/Athena include(s):
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "TrigT1Result/MuCTPIResult.h"
#include "TrigT1Result/MuCTPI_RDO.h"
#include "TrigT1Interfaces/TrigT1CTPDefs.h"

using namespace std;

namespace ROIB {

  /**
   * This constructor is used by the RoIB simulation to create the object from the class.
   *
   * @param head      Reference to the header to be used by the object
   * @param trail     Reference to the trailer to be used by the object
   * @param roIVector Vector of muon RoIs collected from the MuCTPI
   */
  MuCTPIResult::MuCTPIResult(  Header&& head, Trailer&& trail, vector< MuCTPIRoI >&& roIVector )
    : m_MuCTPIResultHeader( std::move(head) ), m_MuCTPIResultTrailer( std::move(trail) ),
      m_MuCTPIResultRoIVec( std::move(roIVector) ) {

  }

  /**
   * Empty constructor needed by POOL to be able to stream the objects.
   */
  MuCTPIResult::MuCTPIResult()
    : m_MuCTPIResultHeader(), m_MuCTPIResultTrailer(),
      m_MuCTPIResultRoIVec() {

  }

  /**
   * Simply returning a constant reference to the variable
   *
   * @return Constant reference to the header
   */
  const Header& MuCTPIResult::header() const {
    return m_MuCTPIResultHeader;
  }

  /**
   * Simply returning a constant reference to the variable
   *
   * @return Constant reference to the trailer
   */
  const Trailer& MuCTPIResult::trailer() const {
    return m_MuCTPIResultTrailer;
  }

  /**
   * Simply returning a constant reference to the variable
   *
   * @return Constant reference to the RoI vector
   */
  const vector< MuCTPIRoI >& MuCTPIResult::roIVec() const {
    return m_MuCTPIResultRoIVec;
  }

  const std::string MuCTPIResult::dump() const
  {
    std::ostringstream s;

    s << " [" << this->header().dump() << "] ";
    s << " [";
      for (std::vector<MuCTPIRoI>::size_type i(0); i < roIVec().size(); ++i) {
	s << LVL1CTP::convertToHex(roIVec()[i].roIWord());
      }
    s << "] ";
    s << " [" << this->trailer().dump() << "] ";
    
    return s.str();
  }

  const std::string MuCTPIResult::print(const bool longFormat) const
  {
    std::ostringstream s;

    s << "header [" << this->header().print(longFormat) << "] ";
    if (longFormat) s << std::endl;

    for(std::vector< MuCTPIRoI >::size_type i(0); i < roIVec().size(); ++i ) {

      std::string loc = "UNDEFINED";
      if( roIVec()[i].getSectorLocation() == MuCTPI_RDO::ENDCAP )
	loc = "ENDCAP";
      else if( roIVec()[i].getSectorLocation() == MuCTPI_RDO::FORWARD )
	loc = "FORWARD";
      else if( roIVec()[i].getSectorLocation() == MuCTPI_RDO::BARREL )
	loc = "BARREL";

      if (i == 0 || longFormat) s << "data";
      if (longFormat) s << "[" << std::setw(2) << i << "]";

      s << " [";
      if (longFormat) s << LVL1CTP::convertToHex(roIVec()[i].roIWord());
      if (longFormat) s << std::setw(12) << "\n Threshold: ";
      s << " " << roIVec()[i].pt();
      if (longFormat) s << std::setw(12) << "\n Sector location: ";
      s << " " << loc;
      if (longFormat) s << std::setw(12) << "\n Sector ID:";
      s << " " << roIVec()[i].getSectorID();
      if (longFormat) s << std::setw(12) << "\n Sector addr:";
      s << " " << LVL1CTP::convertToHex(roIVec()[i].getSectorID());
      if (longFormat) s << std::setw(12) << "\n Sector overflow:";
      s << " " << roIVec()[i].getSectorOverflow();
      if (longFormat) s << std::setw(12) << "\n RoI overflow:";
      s << " " << roIVec()[i].getRoiOverflow();
      if (longFormat) s << std::setw(12) << "\n RoI number:";
      s << " " << roIVec()[i].getRoiNumber();
      if (longFormat) s << std::setw(12) << "\n IsHighestPt:";
      s << " " << roIVec()[i].getCandidateIsHighestPt();
      if (longFormat) s << std::setw(12) << "\n Overlap:";
      s << " " << roIVec()[i].getOverlapBits();
      s << "] ";
    }
    s << "trailer [" << this->trailer().print(longFormat) << "] ";

    return s.str();
  }


  /**
   * This function is not responsible for actually printing the information, it just creates
   * a message stream to print the information to with MuCTPIResult::dumpData(MsgStream&) const.
   */
  void MuCTPIResult::dumpData() const {

    IMessageSvc*  msgSvc;
    ISvcLocator* svcLoc = Gaudi::svcLocator( );
    StatusCode sc = svcLoc->service( "MessageSvc", msgSvc );
    if( sc.isFailure() ) {
      return;
    }
    MsgStream log( msgSvc, "MuCTPIResult" );
    dumpData( log );

    return;
  }

  /**
   * Function printing the information stored in the object in an easy-to-read fashion.
   * It calls Header::dumpData(MsgStream&) const and Trailer::dumpData(MsgStream&) const
   * to print their information as well.
   *
   * @param log  A message stream to print the information to
   */
  void MuCTPIResult::dumpData( MsgStream& log ) const {

    log << MSG::DEBUG << "*BEGIN* MuCTPIResult" << endmsg;

    m_MuCTPIResultHeader.dumpData( log );
    int counter = 0;
    std::vector< MuCTPIRoI >::const_iterator it = m_MuCTPIResultRoIVec.begin();
    for( ; it != m_MuCTPIResultRoIVec.end(); ++it, ++counter ) {

      std::string loc = "UNDEFINED";
      if( it->getSectorLocation() == MuCTPI_RDO::ENDCAP )
	loc = "ENDCAP";
      else if( it->getSectorLocation() == MuCTPI_RDO::FORWARD )
	loc = "FORWARD";
      else if( it->getSectorLocation() == MuCTPI_RDO::BARREL )
	loc = "BARREL";

      log << MSG::DEBUG << "RoI word[" << counter << "]        : 0x"
	  << MSG::hex << it->roIWord() << MSG::dec << endmsg;
      log << MSG::DEBUG << "Threshold               :  pt" << it->pt() << endmsg;
      log << MSG::DEBUG << "Sector location         :  " << loc << endmsg;
      log << MSG::DEBUG << "Sector ID               :  " << it->getSectorID() << endmsg;
      log << MSG::DEBUG << "Sector addr             :  0x" << MSG::hex
	  << it->getSectorID() << MSG::dec << endmsg;
      log << MSG::DEBUG << "Sector overflow         :  " << it->getSectorOverflow() << endmsg;
      log << MSG::DEBUG << "RoI overflow            :  " << it->getRoiOverflow() << endmsg;
      log << MSG::DEBUG << "RoI number              :  " << it->getRoiNumber() << endmsg;
      log << MSG::DEBUG << "IsHighestPt             :  " << it->getCandidateIsHighestPt() << endmsg;
      log << MSG::DEBUG << "Overlap                 :  " << it->getOverlapBits() << endmsg;

    }
    m_MuCTPIResultTrailer.dumpData( log );
    log << MSG::DEBUG << "*END* MuCTPIResult" << endmsg;

    return;
  }

} // namespace ROIB
