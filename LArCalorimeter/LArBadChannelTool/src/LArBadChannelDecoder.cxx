/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArBadChannelTool/LArBadChannelDecoder.h"

#include "LArRecConditions/LArBadChannelParser.h"
#include "GaudiKernel/MsgStream.h"
#include "LArIdentifier/LArOnlineID.h"

std::vector<LArBadChannelDecoder::BadChanEntry>
LArBadChannelDecoder::readASCII( const std::string& fname, 
				 State::CoolChannelEnum coolChan) const
{
  std::vector<BadChanEntry> result;

  bool oldIdCheckStatus=m_onlineID->do_checks();
  m_onlineID->set_do_checks(true); //Check Id when reading from ASCII file
 
  // set up parsing with exactly 6 ints and >= 1 string, for reading channels
  LArBadChannelParser parser(fname, &m_log, 6, -1);
  if (!parser.fileStatusGood()) {
    m_log << MSG::ERROR << "Failed to open file " << fname
	<< " for COOL channel " << State::coolChannelName( coolChan) << endmsg;
    return result;
  }

  typedef std::string ParseType;
  typedef std::pair<std::vector<int>, std::vector<ParseType> > ParsedLine;
  std::vector<ParsedLine> parsed = parser.parseFile<ParseType>();
  m_log << MSG::INFO << "Parsed " << parsed.size() << " lines from file " << fname << endmsg;

  for (std::vector<ParsedLine>::const_iterator i=parsed.begin();
       i != parsed.end(); i++) {
    HWIdentifier hwid = constructChannelId( i->first, coolChan, m_log);
    if (hwid.is_valid()) {
      std::pair<bool,LArBadChannel> badCh = constructStatus( i->second, m_log);
      if (badCh.first) {
	result.push_back( BadChanEntry( hwid, badCh.second));
      }
    }
  }
  //Reset checking, to avoid downstream problems (see bug #33824)
  m_onlineID->set_do_checks(oldIdCheckStatus); 
  return result;
}

std::vector<LArBadChannelDecoder::BadFebEntry>
LArBadChannelDecoder::readFebASCII( const std::string& fname) const
{
  std::vector<BadFebEntry> result;

  bool oldIdCheckStatus=m_onlineID->do_checks();
  m_onlineID->set_do_checks(true); //Check Id when reading from ASCII file
 
 // set up a parser to read 4 ints (the 4th of which can be a wildcard) and >=1 strings
  LArBadChannelParser parser(fname, &m_log, 4, -1, 4);
  if (!parser.fileStatusGood()) {
    m_log << MSG::ERROR << "Failed to open missing FEBs file " << fname << endmsg;
    return result;
  }
  
  typedef std::string ParseType;
  typedef std::pair<std::vector<int>, std::vector<ParseType> > ParsedLine;
  std::vector<ParsedLine> parsed = parser.parseFile<ParseType>();
  for (std::vector<ParsedLine>::const_iterator i=parsed.begin();
       i != parsed.end(); i++) {
    std::vector<HWIdentifier> hwid = constructFebId( i->first, m_log);
    if (!hwid.empty()) {
      // BEFORE: result.insert( result.end(), hwid.begin(), hwid.end());
      std::pair<bool,LArBadFeb> badFeb = constructFebStatus( i->second, m_log);
      if (badFeb.first) {
	for (std::vector<HWIdentifier>::const_iterator i=hwid.begin(); i!=hwid.end(); ++i) {
	  result.push_back( BadFebEntry( *i, badFeb.second));
	}
      }
    }
  }
  //Reset checking, to avoid downstream problems (see bug #33824)
  m_onlineID->set_do_checks(oldIdCheckStatus); 
  return result;
}

HWIdentifier LArBadChannelDecoder::constructChannelId( const std::vector<int>& intVec, 
						       State::CoolChannelEnum coolChan,
						       MsgStream& log) const
{
  HWIdentifier invalid;
  if (intVec.size() < 5) { //  redundant error check
    log << MSG::WARNING << "Failed to produce a channel HWIdentifier for ";
    for (unsigned int i=0; i<intVec.size(); i++) log << intVec[i] << " ";
    log << "not enough identifiers" << endmsg;
    return invalid;
  }
  try {
    HWIdentifier hwid( m_onlineID->channel_Id( intVec[barrel_ec], intVec[pos_neg], intVec[feedthrough],
					       intVec[slot], intVec[channel]));
    if (!checkId( hwid, intVec[barrel_ec], intVec[pos_neg], coolChan)) {
      log << MSG::WARNING << "Channel "; insertExpandedID( intVec, log);
      log << " does not belong to COOL channel " << State::coolChannelName( coolChan) 
	  << ". Skipped" <<  endmsg;
      return invalid;
    }
    log << MSG::DEBUG << "Translating id ";  insertExpandedID( intVec, log);
    log << " to 0x" << MSG::hex << hwid << MSG::dec << endmsg;   
    return hwid;
  } 
  catch( LArOnlID_Exception& idException) {
    log << MSG::ERROR << "Failed to produce a HWIdentifier for ";  insertExpandedID( intVec, log) << endmsg;
  }
  return invalid;
}

std::vector<HWIdentifier> LArBadChannelDecoder::constructFebId( const std::vector<int>& intVec, 
								MsgStream& log) const
{
  const int maxFebPerFT = 15; // hard-wired number of slots per feed-through

  std::vector<HWIdentifier> result;
  if (intVec.size() != 4) { // redundant error check
    log << MSG::WARNING << "Failed to produce a FEB HWIdentifier for ";
    for (unsigned int i=0; i<intVec.size(); i++) log << intVec[i] << " ";
    log << "not enough identifiers" << endmsg;
    return result;
  }
  
  // expand wildcard
  if (intVec[slot] == -1) {
    std::vector<int> vec( intVec); // modifiable copy

    // FEB slot counting starts at 1
    for (vec[slot]=1; vec[slot] <= maxFebPerFT; vec[slot]++) {

      HWIdentifier hwid = constructSingleFebId( vec, log);
      if (hwid.is_valid()) result.push_back( hwid);
    }
  }
  else{
    HWIdentifier hwid = constructSingleFebId( intVec, log);
    if (hwid.is_valid()) result.push_back( hwid);
  }
  return result;
}

HWIdentifier LArBadChannelDecoder::constructSingleFebId( const std::vector<int>& v, 
							 MsgStream& log) const
{
  HWIdentifier invalid;
  if (v.size() != 4) return invalid;
  try {
    HWIdentifier hwid( m_onlineID->feb_Id( v[barrel_ec], v[pos_neg], v[feedthrough], v[slot]));
    log << MSG::DEBUG << "Translating FEB id ";  insertExpandedID( v, log);
    log << " to 0x" << MSG::hex << hwid << MSG::dec << endmsg;   
    return hwid;
  } 
  catch( LArOnlID_Exception& idException) {
    log << MSG::ERROR << "Failed to produce a FEB HWIdentifier for ";  insertExpandedID( v, log);
    log << endmsg;
  }
  return invalid;
}

std::pair<bool,LArBadChannel> 
LArBadChannelDecoder::constructStatus( const std::vector<std::string>& vec, MsgStream& log) const
{
  LArBadChannel result;
  for(std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); ++it) {
    bool ok = m_packing.setBit( *it, result);
    if (!ok) { 
      log << MSG::WARNING << "LArBadChannelDecoder REJECTED line with " 
	  << ":\t unrecognized problem status: " << *it << endmsg;
       return std::pair<bool,LArBadChannel>(false,result); // exit on error
    }
  }
  return std::pair<bool,LArBadChannel>(true,result);
}

std::pair<bool,LArBadFeb> 
LArBadChannelDecoder::constructFebStatus( const std::vector<std::string>& vec,
					  MsgStream& log) const
{
  LArBadFeb result;
  for(std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); ++it) {
    bool ok = m_febPacking.setBit( *it, result);
    if (!ok) { 
      log << MSG::WARNING << "LArBadChannelDecoder REJECTED line with " 
	  << ":\t unrecognized problem status: " << *it << endmsg;
       return std::pair<bool,LArBadFeb>(false,result); // exit on error
    }
  }
  return std::pair<bool,LArBadFeb>(true,result);
}


MsgStream& LArBadChannelDecoder::insertExpandedID( const std::vector<int>& intVec, MsgStream& log) const 
{
  log << " b/e " << intVec[barrel_ec] 
      << " p/n " << intVec[pos_neg] 
      << " ft " <<  intVec[feedthrough] 
      << " slot " << intVec[slot];
  if (intVec.size() >= 5) log << " ch " <<  intVec[channel]; // no endmsg, 
  return log;
}

bool LArBadChannelDecoder::checkId( const HWIdentifier& id, int barrel_ec, int pos_neg,
			      State::CoolChannelEnum coolCh) const
{
  if (barrel_ec != State::barrelEndcap(coolCh)) return false; // first check barrel/ec (faster)
  if (pos_neg != State::posNeg(coolCh)) return false;         // then check side (faster)
  if (m_onlineID->isEMBchannel(id) && State::caloPart(coolCh)==State::EMB) return true;
  if (m_onlineID->isEMECchannel(id) && State::caloPart(coolCh)==State::EMEC) return true;
  if (m_onlineID->isHECchannel(id) && State::caloPart(coolCh)==State::HEC) return true;
  if (m_onlineID->isFCALchannel(id) && State::caloPart(coolCh)==State::FCAL) return true;
  return false;
}
