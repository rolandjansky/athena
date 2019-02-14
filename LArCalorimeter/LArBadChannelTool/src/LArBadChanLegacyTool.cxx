/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArBadChannelTool/LArBadChanLegacyTool.h"

#include "LArBadChannelTool/LArBadChannelDecoder.h"
#include "StoreGate/StoreGate.h"
#include "GaudiKernel/MsgStream.h"
#include "LArIdentifier/LArOnlineID.h"
#include "StoreGate/StoreGateSvc.h"
#include "LArRecConditions/LArBadChanBitPacking.h"
#include "LArRecConditions/LArBadFebBitPacking.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoolKernel/types.h"
#include "LArBadChannelTool/LArBadChannelDBTools.h"
#include "LArBadChannelTool/LArBadChanBlobUtils.h"

#include "combined_ordered_container.h"
#include "mergeDuplicateEntries.h"

#include <set>

#include <fstream>

struct BadFebEntryLess {
  bool operator()(const LArBadChanLegacyTool::BadFebEntry& a,
		  const LArBadChanLegacyTool::BadFebEntry& b) const {
    return a.first < b.first;
  }
};

struct BadFebEntryMerger {
  LArBadChanLegacyTool::BadFebEntry operator()( const LArBadChanLegacyTool::BadFebEntry& a, 
					  const LArBadChanLegacyTool::BadFebEntry& b) const {
    return LArBadChanLegacyTool::BadFebEntry( a.first, 
					LArBadFeb( a.second.packedData() | b.second.packedData()));
  }
};

LArBadChanLegacyTool::LArBadChanLegacyTool(const std::string& type, const std::string& name, 
			       const IInterface* parent) :
	AthAlgTool( type, name, parent), 
        m_cablingService("LArCablingLegacyService"),
	m_updatedFromDB( false),
	m_updatedFebsFromDB( false),
	m_ready( false),
	m_readFromASCII(false),
	m_ASCII_action(0),
	m_writeEmptyFolders(false),
	m_fileNames(State::NCoolChannels),
	m_DBFolder("/LAR/BadChannels/BadChannels"),
	m_TmpDBFolder(),                           // empty by default
	m_DBBadFebFolder("/LAR/BadChannels/MissingFEBs"),
	m_dumpCache(false),
	m_OfflineCacheInvalid(true)
{
  declareInterface<ILArBadChanTool>(this);
  declareInterface<ICaloBadChanTool>(this);

  declareProperty("ReadFromASCII", m_readFromASCII);
  declareProperty("ASCIIFileAction", m_ASCII_action);
  declareProperty("WriteEmptyFolders", m_writeEmptyFolders);
  declareProperty("CoolFolder", m_DBFolder);
  declareProperty("ComplementaryCoolFolder", m_TmpDBFolder);
  declareProperty("CoolMissingFEBsFolder", m_DBBadFebFolder);
  declareProperty("DumpCache", m_dumpCache);

  declareProperty("EMBAfile", m_fileNames[State::EMBA]);
  declareProperty("EMBCfile", m_fileNames[State::EMBC]);
  declareProperty("EMECAfile", m_fileNames[State::EMECA]);
  declareProperty("EMECCfile", m_fileNames[State::EMECC]);
  declareProperty("HECAfile", m_fileNames[State::HECA]);
  declareProperty("HECCfile", m_fileNames[State::HECC]);
  declareProperty("FCALAfile", m_fileNames[State::FCALA]);
  declareProperty("FCALCfile", m_fileNames[State::FCALC]);
  declareProperty("FEBfile", m_febFileName);

}

const InterfaceID& LArBadChanLegacyTool::interfaceID() 
{ 
  //return ILArBadChanLegacyTool::interfaceID(); 
  static const InterfaceID id("LArBadChanLegacyTool", 1 , 0);
  return id; 
}

StatusCode LArBadChanLegacyTool::queryInterface( const InterfaceID& riid, void** ppvIf )
{
   if ( riid == ILArBadChanTool::interfaceID() )  {
      *ppvIf = static_cast<ILArBadChanTool*>(this);
      addRef();
      return StatusCode::SUCCESS;
   }
   if ( riid == interfaceID() )  {
      *ppvIf = this;
      addRef();
      return StatusCode::SUCCESS;
   }
   /*
   if ( riid == ICaloBadChannelTool::interfaceID() )  {
      *ppvIf = static_cast<ICaloBadChannelTool*>(this);
      addRef();
      return StatusCode::SUCCESS;
   }
   */
   return AthAlgTool::queryInterface( riid, ppvIf );
}

StatusCode LArBadChanLegacyTool::initialize()
{
  ATH_MSG_DEBUG ("in initialize()" );
	
  ATH_CHECK( detStore()->retrieve(m_onlineID, "LArOnlineID") );
  if (!prepareFebHash()) {
    ATH_MSG_ERROR ( "Could not configure FEB hasher" );
    return StatusCode::FAILURE;
  }

  // we need to read from ASCII before the callback registration, because
  // if the tool is initialised e.g. during event processing the callback
  // will be triggered immediately
  if (m_readFromASCII) {
    if (!readASCII()) {
      ATH_MSG_ERROR ("Could not read ASCII file(s)" );
      return StatusCode::FAILURE;
    }
    // prepare the tool for use in case no DB update occurs, but keep m_ready false
    m_State = m_Updates;
    if (!m_BadFebUpdates.empty()) applyFebUpdates(); // from ASCII file. Does sorting and duplicate removal
    updateCache();
    updateFebCache();
  }

  if (!m_DBFolder.empty()) {
    ATH_CHECK( detStore()->regFcn( &ILArBadChanTool::updateFromDB, dynamic_cast<ILArBadChanTool*>(this),
                                   m_DBColl, m_DBFolder, true) );
  }

  if (!m_TmpDBFolder.empty()) {
    ATH_CHECK( detStore()->regFcn( &ILArBadChanTool::updateFromDB, dynamic_cast<ILArBadChanTool*>(this),
                                   m_TmpDBColl, m_TmpDBFolder, true) );
  }

  if (!m_DBBadFebFolder.empty()) {
    ATH_CHECK( detStore()->regFcn( &ILArBadChanTool::updateBadFebsFromDB, dynamic_cast<ILArBadChanTool*>(this),
                                   m_DBBadFebColl, m_DBBadFebFolder, true) );
  }

  ATH_CHECK( m_cablingService.retrieve() );
  ATH_MSG_DEBUG ( "CablingService retrieved" );
  
  return StatusCode::SUCCESS;
}

std::vector<HWIdentifier> LArBadChanLegacyTool::missingFEBs() const
{
  std::vector<HWIdentifier> result( m_BadFebs.size());
  for (unsigned int i=0; i<m_BadFebs.size(); ++i)
    result[i] = m_BadFebs[i].first;
  return result;
}

void LArBadChanLegacyTool::complain() const
{
  if (!m_updatedFromDB) {
    // if we are here then the bad channel DB update did not take place before first use
    ATH_MSG_WARNING ( "the LArBadChanLegacyTool is used without bad channel information from DB" );

    if (m_Updates.empty()) {
      ATH_MSG_WARNING ( "and it contains no bad channel information from ASCII file." );
      ATH_MSG_WARNING 
        ( "All channels will be considered good, but this may not be what you wanted" );
      BadChanVec empty;
      m_HwBadChan.set(empty); //need to populate the hash table with good channels
    }
    else {
      // No need to apply updates from ASCII file, that's done in initialize()
      ATH_MSG_WARNING ( "Only ASCII file bad channel information is available." );
    }
  }
  if (!m_updatedFebsFromDB) {
    // if we are here then the bad Feb DB update did not take place before first use
    ATH_MSG_WARNING ( "the LArBadChanLegacyTool is used without missing FEB information from DB" );

    if (m_BadFebUpdates.empty()) {
      ATH_MSG_WARNING ( "and it contains no missing FEB information from ASCII file." );
      ATH_MSG_WARNING 
        ( "All FEBs will be considered good, but this may not be what you wanted" );
      // the febs are good by default so we don't need to set them explicitly
    }
    else {
      // No need to apply updates from ASCII file, that's done in initialize()
      ATH_MSG_WARNING ( "Only ASCII file missing FEB information is available." );
    }
  }
  m_ready = true;
}

bool LArBadChanLegacyTool::readASCII() 
{
  LArBadChannelDecoder decoder(&(*m_onlineID), msg());

  int goodLines = 0;
  for (int i=0; i<State::NCoolChannels; i++) {
    if ( !m_fileNames.at(i).empty()) {
      State::CoolChannelEnum coolChan = static_cast<State::CoolChannelEnum>(i);
      ATH_MSG_INFO ( "Attempting to read file " << m_fileNames[i] 
                     << " for COOL channel " << State::coolChannelName( coolChan) );

      std::vector<State::BadChanEntry> res = decoder.readASCII( m_fileNames[i], coolChan);
      if ( !res.empty()) {
	ATH_MSG_INFO ( "Read " << res.size() << " lines from file " << m_fileNames[i]
                       << " into COOL channel " << State::coolChannelName( coolChan) );
	m_Updates.addUpdate( res, coolChan);
	goodLines += res.size();
      }
      else if (!m_writeEmptyFolders) {
	ATH_MSG_ERROR ( "Failed to read any lines from file " << m_fileNames[i] 
                        << " for COOL channel " << State::coolChannelName( coolChan) );
	return false;
      }
    }
  }
  if ( !m_febFileName.empty()) {
    std::vector<LArBadChannelDecoder::BadFebEntry> febs = decoder.readFebASCII( m_febFileName);
    if (febs.empty() && (!m_writeEmptyFolders) ) {
      ATH_MSG_ERROR ( "Failed to read any lines from FEB file " <<  m_febFileName );
      return false;
    }
    else {
      ATH_MSG_INFO ( "Decoded " << febs.size() << " lines from FEB file " <<  m_febFileName );
      m_BadFebUpdates = febs;
      goodLines += febs.size();
    }
  }

  if (goodLines == 0 && (!m_writeEmptyFolders) ) {
    ATH_MSG_ERROR ( "ASCII reading requested but no file names specified " );
    return false;
  }
  else {
    ATH_MSG_INFO ( "Read " << goodLines 
                   << " lines in total (for all COOL channels and missing FEBs together) " );
  }

  // at this stage we have all the contents of the ASCII files in m_Updates.
  // We need to
  //  1) resolve duplicate entries for same channel
  //  2) update the m_HwBadChan
  //  3) invalidate or update m_OfflineBadChan
  //  4) write the new status to the DB

  for (int i=0; i<State::NCoolChannels; i++) {
    if ( m_Updates.coolChannel(i).changed()) {
      // resolveDuplicateEntries( m_Updates.coolChannel(i));
      m_Updates.coolChannel(i).resolveDuplicateEntries();
    }
  }
  return true;
}

LArBadChannel LArBadChanLegacyTool::status( HWIdentifier id) const {
  check();
  try {
    return m_HwBadChan.status(id);
  }
  catch (const std::exception& ex) {
    warnInvalidFEB(id);
    return LArBadChannel();
  }
}

LArBadChannel LArBadChanLegacyTool::status( const HWIdentifier& FEBid, int chan) const {
  check();
  try {
    return m_HwBadChan.status( FEBid, chan);
  }
  catch (const std::exception& ex) {
    warnInvalidFEB( FEBid);
    return LArBadChannel();
  }
}

void LArBadChanLegacyTool::warnInvalidFEB( const HWIdentifier& id) const
{
  ATH_MSG_WARNING ( "status requested for unknown HWIdentifier " 
                    << id.get_identifier32().get_compact() );
}

LArBadChannel LArBadChanLegacyTool::offlineStatus( Identifier id) const
{
  check();

  if (m_OfflineCacheInvalid) fillOfflineInfo();

  return m_OfflineBadChan.status(id);
}

CaloBadChannel LArBadChanLegacyTool::caloStatus( Identifier id) const
{
  CaloBadChannel::BitWord res = 0;
  LArBadChannel lbc = offlineStatus( id);

  //Set 'noise' bit if appropriate
  if ( lbc.reallyNoisy() || lbc.sporadicBurstNoise()) {
    CaloBadChannel::setBit( CaloBadChannel::noisyBit, res);
  }
  //Set 'dead' bit (could be combined with noise bit)
  if (lbc.deadReadout() || lbc.deadPhys()) {
    CaloBadChannel::setBit( CaloBadChannel::deadBit, res);
  }
  //Set 'affected' bit (could be combined with noise bit, but not with dead bit
  else if ( ! lbc.good()) {
    CaloBadChannel::setBit( CaloBadChannel::affectedBit, res);
  }
  return CaloBadChannel(res);
}

void LArBadChanLegacyTool::fillOfflineInfo() const
{
  m_OfflineBadChan.clear(); 
  OfflineVec vec;
  std::vector<HWIdentifier> hwvec;
  for (BadChanVec::const_iterator i=m_HwBadChan.begin(); i!=m_HwBadChan.end(); i++) {
    Identifier id = m_cablingService->cnvToIdentifier( i->first);
    vec.push_back(OfflineVec::value_type(id, status(i->first)));
    hwvec.push_back(i->first);
  }
  std::sort( hwvec.begin(), hwvec.end());

  // fill the missing FEB channels
  for (std::vector<BadFebEntry>::const_iterator feb=m_BadFebs.begin(); feb!=m_BadFebs.end(); ++feb) {
    for (int ch=0; ch<128; ++ch) {
      HWIdentifier hwid = m_onlineID->channel_Id( feb->first, ch); 
      // check if the channel is bad to avoid duplicates
      if ( !std::binary_search( hwvec.begin(), hwvec.end(), hwid)) {
	// check if the ids we generate are connected
	if (m_cablingService->isOnlineConnected(hwid)) {
	  Identifier id = m_cablingService->cnvToIdentifier( hwid);
	  vec.push_back( OfflineVec::value_type( id, status(hwid,ch)));
	}
      }
    }
  }

  m_OfflineBadChan = OfflineInfo(vec);
  m_OfflineCacheInvalid = false;
}

void LArBadChanLegacyTool::updateCache()
{
  ATH_MSG_DEBUG ( " entering updateCache " );

  BadChanVec bcv;

  for (int cc=0; cc<State::NCoolChannels; cc++) {
    const State::CoolChannelData& coolChan = m_State.coolChannel(cc);
    bcv.insert( bcv.end(), coolChan.begin(), coolChan.end());
  }
  m_HwBadChan.set(bcv);

  // invalidate offline cache (will be filled at first access, if needed
  m_OfflineCacheInvalid = true;
  
  ATH_MSG_DEBUG ( " updateCache done " );
}

void LArBadChanLegacyTool::updateFebCache() 
{
  // just copy current FEB list to the cache
  m_HwBadChan.setBadFEBs( m_BadFebs);

  // invalidate offline cache (will be filled at first access, if needed
  m_OfflineCacheInvalid = true;
  
}

bool LArBadChanLegacyTool::readFromDB( const DataHandle<CondAttrListCollection> collection) 
{
  for ( CondAttrListCollection::const_iterator i=collection->begin(); 
	i != collection->end(); ++i) {
    State::CoolChannelEnum coolChan = static_cast<State::CoolChannelEnum>(i->first);
    ATH_MSG_DEBUG ( "Reading Cool Channel " << i->first 
                    << " corresponding to " << State::coolChannelName(coolChan) );
    /*
    const AthenaAttributeList& attrList( i->second);
    const coral::Blob& blob = attrList["Blob"].data<coral::Blob>();
    unsigned int chanSize = attrList["ChannelSize"].data<unsigned int>();
    unsigned int stateSize = attrList["StatusWordSize"].data<unsigned int>();
    unsigned int endian = attrList["Endianness"].data<unsigned int>();
    unsigned int version = attrList["Version"].data<unsigned int>();
    
    std::vector<State::BadChanEntry> stateVec = 
      LArBadChanBlobUtils::decodeBlob<LArBadChannel>( &blob, chanSize, stateSize, endian,
						      version, log);
    */
    /* 
    // Unfortunately we have to skip this test because we need empty channels to 
    // bypass Athena problem.
    if (stateVec.empty()) {
    log <<MSG::ERROR <<"Got empty channel list after Blob decoding of col channel " 
    << State::coolChannelName(coolChan) << endmsg;
    return false;
    }
    */
    
    m_State.addFromDB( LArBadChannelDBTools::readBadChan( i->second, msg()), coolChan);
  }
  return true;
}

bool LArBadChanLegacyTool::readBadFebsFromDB() 
{

  ATH_CHECK( detStore()->retrieve( m_DBBadFebColl, m_DBBadFebFolder), false );
  ATH_MSG_INFO ( "Retrieved folder " << m_DBBadFebFolder );

  if (!m_DBBadFebColl.isValid()){
    ATH_MSG_ERROR ( " DB handle for missing FEBs is invalid" );
    return false;
  }
  /*
  const AthenaAttributeList& attrList( *m_DBBadFebColl);
  const coral::Blob& blob = attrList["Blob"].data<coral::Blob>();
  unsigned int chanSize = attrList["ChannelSize"].data<unsigned int>();
  unsigned int stateSize = attrList["StatusWordSize"].data<unsigned int>();
  unsigned int endian = attrList["Endianness"].data<unsigned int>();
  unsigned int version = attrList["Version"].data<unsigned int>();
    
  // overwrite list of bad febs
  if (version == 1) {
    std::vector<HWIdentifier> idvec = 
      LArBadChanBlobUtils::decodeFebBlobV1( &blob, chanSize, endian, version, log);
    m_BadFebs.resize(idvec.size());
    LArBadFeb missingState;
    LArBadFebBitPacking febPacking;
    febPacking.setBit( LArBadFeb::deadAllBit, missingState);
    for (unsigned int i=0; i<idvec.size(); ++i) {
      m_BadFebs[i] = BadFebEntry( idvec[i], missingState);
    }
  }
  else {
    m_BadFebs = LArBadChanBlobUtils::decodeBlob<LArBadFeb>( &blob, chanSize, stateSize,
							    endian, version, log);
  }
  */

  m_BadFebs = LArBadChannelDBTools::readBadFeb( *m_DBBadFebColl, msg());
  return true;
}


StatusCode LArBadChanLegacyTool::updateFromDB( int&, std::list<std::string>& keylist)
{
  m_State.reset();

  ATH_MSG_INFO (  "DB callback called with " << keylist.size() << " keys " );
  for (std::list<std::string>::const_iterator ikey = keylist.begin(); ikey != keylist.end(); ikey++) {
    ATH_MSG_INFO (  "DB callback called with key " << *ikey );
  }

  // Always read both folders if they are valid, even if only one folder triggered the callback
  if (m_DBColl.isValid()) {
    if (!readFromDB( m_DBColl)) return StatusCode::FAILURE;
  }
  if (m_TmpDBColl.isValid()) {
    if (!readFromDB( m_TmpDBColl)) return StatusCode::FAILURE;
  }

  m_updatedFromDB = true;

  ATH_MSG_INFO ( "Callback updateFromDB finished reading from DB" );

  if (!m_Updates.empty()) applyUpdates(); // from ASCII file

  updateCache();
  m_ready = m_updatedFebsFromDB; // m_ready is true if both channels and febs are OK from DB

  if(m_dumpCache) dumpAscii("dump.txt");

  ATH_MSG_INFO ( "Have " << m_HwBadChan.size() << " entries in total." );

  return StatusCode::SUCCESS;
}

StatusCode LArBadChanLegacyTool::updateBadFebsFromDB(IOVSVC_CALLBACK_ARGS)
{
  if (!readBadFebsFromDB()) return StatusCode::FAILURE;
  m_updatedFebsFromDB = true;

  ATH_MSG_INFO ( "Callback updateBadFebsFromDB finished reading from DB" );

  if (!m_BadFebUpdates.empty()) applyFebUpdates(); // from ASCII file

  updateFebCache();
  m_ready = m_updatedFromDB; // m_ready is true if both channels and febs are OK from DB

  if(m_dumpCache) dumpFEBsAscii("dumpFEBs.txt");

  ATH_MSG_INFO ( "Have " << m_BadFebs.size() << " missing FEBs in total." );
  return StatusCode::SUCCESS;
}

void LArBadChanLegacyTool::dumpHWCache() const 
{
  LArBadChanBitPacking packing;
  ATH_MSG_INFO ( "Begin of dump of online Id cache" );
  for (BadChanInfo::const_iterator i=m_HwBadChan.begin(); i!=m_HwBadChan.end(); ++i) {
    ATH_MSG_INFO ( "HWId " << i->first.get_identifier32().get_compact() << " status " 
                   << packing.stringStatus(i->second) );
  }
  ATH_MSG_INFO ( "End of dump of online Id cache" );
}

void LArBadChanLegacyTool::applyUpdates()
{
  typedef std::vector<State::BadChanEntry>::const_iterator   Iter;

  for (int cc=0; cc<State::NCoolChannels; cc++) {
    State::CoolChannelData& coolChan = m_State.coolChannel(cc);
    State::CoolChannelData& updChan = m_Updates.coolChannel(cc);

    if (coolChan.empty()) coolChan = updChan;
    else {
      for (Iter iupd = updChan.begin(); iupd != updChan.end(); iupd++) {
	applyUpdate( coolChan, *iupd);
      }
    }
  }
}

void LArBadChanLegacyTool::applyUpdate( State::CoolChannelData& coolChan,
				  const State::BadChanEntry& entry)
{
  typedef std::vector<State::BadChanEntry>::iterator   Iter;

  // we assume that coolChan does not contain duplicate entries
  // because at some point it came from ASCII, so resolveDuplicateEntries was called then.
  for (Iter i = coolChan.begin(); i != coolChan.end(); i++) {
    if (i->first == entry.first) {  // Try to match the update with an existing bad channel
      State::mergeFlags( *i, entry);       // If found, combine the problems
      return;
    }
  }    // The cell from the ASCII update was not already present in the database
  coolChan.add(entry); // So, add the new bad channel
}

// should not be called if m_BadFebUpdates.empty()
void LArBadChanLegacyTool::applyFebUpdates()
{
  using namespace LArBadChanImpl;
  typedef combined_ordered_container< BadFebVec, BadFebEntryLess >   CNT;

  std::sort( m_BadFebUpdates.begin(), m_BadFebUpdates.end(), BadFebEntryLess());

  if (m_BadFebs.empty()) {
    m_BadFebs = mergeDuplicateEntries( m_BadFebUpdates, BadFebEntryLess(), BadFebEntryMerger());
  }
  else {
    std::sort( m_BadFebs.begin(), m_BadFebs.end(), BadFebEntryLess()); // in case it's not sorted
    CNT cnt( m_BadFebs, m_BadFebUpdates); // a combined sorted view of the two containers
    m_BadFebs = mergeDuplicateEntries( cnt, BadFebEntryLess(), BadFebEntryMerger());
  }
}

void LArBadChanLegacyTool::dumpAscii( const std::string& fileName) const
{
  std::ofstream out(fileName.c_str());
  LArBadChanBitPacking packing;

  for (BadChanInfo::const_iterator i=m_HwBadChan.begin(); i!=m_HwBadChan.end(); ++i) {
    int be = m_onlineID->barrel_ec(i->first);
    int pn = m_onlineID->pos_neg(i->first);
    int ft = m_onlineID->feedthrough(i->first);
    int slot = m_onlineID->slot(i->first);
    int ch = m_onlineID->channel(i->first);
    out << be << " " << pn << " " << ft << " " << slot << " " << ch << " " << 0 << " "
	<< packing.stringStatus(i->second) << std::endl;
  }
}

void LArBadChanLegacyTool::dumpFEBsAscii( const std::string& fileName) const
{
  std::ofstream out(fileName.c_str());
  LArBadFebBitPacking packing;

  for (BadFebVec::const_iterator i=m_BadFebs.begin(); i!=m_BadFebs.end(); ++i) {
    int be = m_onlineID->barrel_ec(i->first);
    int pn = m_onlineID->pos_neg(i->first);
    int ft = m_onlineID->feedthrough(i->first);
    int slot = m_onlineID->slot(i->first);
    out << be << " " << pn << " " << ft << " " << slot << " " 
	<< packing.stringStatus(i->second) << std::endl;
  }
}

bool LArBadChanLegacyTool::prepareFebHash() 
{
  int firstFEB = m_onlineID->feb_begin()->get_identifier32().get_compact();
  int prevFEB = firstFEB;
  int minDelta = 999999999;
  int nFEBs = 0;
  for (std::vector<HWIdentifier>::const_iterator i = ++(m_onlineID->feb_begin());
       i != m_onlineID->feb_end(); ++i) {
    int iFEB = i->get_identifier32().get_compact();
    minDelta = std::min( minDelta, iFEB - prevFEB);
    nFEBs++;
    prevFEB = iFEB;
  }
  int lastFEB = prevFEB; // == iFEB
  if (minDelta < 1) {
    // FEB ids not sorted
    return false;
  }
  
  unsigned int febShift = 0;
  for (int i = 1; i < 31; i++) {
    if ( 1 << i == minDelta) febShift = i; 
    // check also that minDelta is a power of 2!!!
  }
  if ( febShift == 0) return false;

  int FEBRange = (lastFEB - firstFEB) / minDelta;
  if (FEBRange > 2500) {
    // too much memory
    return false;
  }

  m_HwBadChan.setHasher( LArBadChanFEBHasher( firstFEB, lastFEB, febShift, 
					      &(*m_onlineID)));
  return true;
}
