/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZdcByteStreamTool.cxx
 *
 *  Created on: May 7, 2009
 *      Author: leite
 *              (shameless based on the L1 Calo ByteStreamTool)
 *
 */

#include <numeric>
#include <utility>

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/MsgStream.h"

#include "ByteStreamCnvSvcBase/FullEventAssembler.h"


#include "ZdcEvent/ZdcDigits.h"
#include "ZdcEvent/ZdcDigitsCollection.h"
#include "ZdcByteStream/ZdcSrcIdMap.h"
#include "ZdcByteStream/ZdcSubBlock.h"
#include "ZdcByteStream/ZdcUserHeader.h"
//#include "ZdcByteStream/ZdcModifySlices.h"
#include "ZdcByteStream/ZdcPpmSubBlock.h"

#include "ZdcConditions/ZdcCablingService.h"

#include "ZdcByteStream/ZdcByteStreamTool.h"

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

//==================================================================================================
// Interface ID
static const InterfaceID IID_IZdcByteStreamTool("ZdcByteStreamTool", 1, 1);

const InterfaceID& ZdcByteStreamTool::interfaceID()
{
	return IID_IZdcByteStreamTool;
}
//==================================================================================================

ZdcByteStreamTool::ZdcByteStreamTool(const std::string& type,
										      const std::string& name,
										      const IInterface* parent) :
			AthAlgTool(type, name, parent),
			//m_version(1),
			//m_compVers(4),
			m_channels(64),
			m_crates(8),
			//m_modules(16),
			m_subDetector(eformat::FORWARD_ZDC),
			m_srcIdMap(0),
			//m_towerKey(0),
			//m_errorBlock(0),
			m_rodStatus(0)

{
	declareInterface<ZdcByteStreamTool> (this);

	declareProperty("PrintCompStats", m_printCompStats = 0, "Print compressed format statistics");

	// Properties for reading bytestream only
	declareProperty("ZeroSuppress", m_zeroSuppress = 0,
					"Only make trigger towers with non-zero EM or Had energy");
	declareProperty("ROBSourceIDs", m_sourceIDs, "ROB fragment source identifiers");
	declareProperty("PedestalValue", m_pedestal = 10,
					"Pedestal value - needed for compressed formats 0,1 only");

	// Properties for writing bytestream only
	declareProperty("DataFormat", m_dataFormat = 1, "Format identifier (0-3) in sub-block header");
	declareProperty("FADCBaseline", m_fadcBaseline = 0,
					"FADC baseline lower bound for compressed formats");
	declareProperty("FADCThreshold", m_fadcThreshold = 1,
					"FADC threshold for super-compressed format");
	declareProperty("SlinksPerCrate", m_slinks = 4, "The number of S-Links per crate");
	declareProperty("SimulSlicesLUT", m_dfltSlicesLut = 1,
					"The number of LUT slices in the simulation");
	declareProperty("SimulSlicesFADC", m_dfltSlicesFadc = 7,
					"The number of FADC slices in the simulation");
	declareProperty("ForceSlicesLUT", m_forceSlicesLut = 0,
					"If >0, the number of LUT slices in bytestream");
	declareProperty("ForceSlicesFADC", m_forceSlicesFadc = 0,
					"If >0, the number of FADC slices in bytestream");

}

ZdcByteStreamTool::~ZdcByteStreamTool()
{
}

//==================================================================================================
// Initialize  the tool
StatusCode ZdcByteStreamTool::initialize()
{
	msg(MSG::INFO) << "Initializing " << name() << " - package version " << PACKAGE_VERSION
			<< endmsg;

	/*
	 * This would be used for crate/module/channel to eta/phi/layer mappings
	StatusCode sc = m_zdcMaps.retrieve();
	if (sc.isFailure())
	{
		msg(MSG::ERROR) << "Failed to retrieve tool " << m_zdcMaps << endmsg;
		return sc;
	}
	else msg(MSG::INFO) << "Retrieved tool " << m_zdcMaps << endmsg;
    */
	m_srcIdMap = new ZdcSrcIdMap();
	m_errorBlock = new ZdcPpmSubBlock();
	m_rodStatus = new std::vector<uint32_t>(2);

	const ZdcID* zdcID = 0;
	if (detStore()->retrieve( zdcID ).isFailure() ) {
	  msg(MSG::ERROR) << "execute: Could not retrieve ZdcID object from the detector store" << endmsg;
	  return StatusCode::FAILURE;
	}
	else {
	  msg(MSG::DEBUG) << "execute: retrieved ZdcID" << endmsg;
	}
	m_zdcID = zdcID;

	return StatusCode::SUCCESS;
}
//==================================================================================================

//==================================================================================================
// Finalize the tool
StatusCode ZdcByteStreamTool::finalize()
{
	if (m_printCompStats && msgLvl(MSG::INFO))
	{
		msg(MSG::INFO);
		printCompStats();
	}

	delete m_rodStatus;
	delete m_errorBlock;
	//delete m_towerKey;
	delete m_srcIdMap;
	return StatusCode::SUCCESS;
}
//==================================================================================================

//==================================================================================================
// Conversion bytestream to ZDCCollection
StatusCode ZdcByteStreamTool::convert( const IROBDataProviderSvc::VROBFRAG& robFrags,
                                              ZdcDigitsCollection* const ttCollection)
{

  const bool debug = msgLvl(MSG::DEBUG);
  //const bool debug = 1;
  const bool verbose = msgLvl(MSG::VERBOSE);


  if (debug) msg(MSG::DEBUG);
  if (verbose) msg(MSG::VERBOSE);

  // Clear trigger tower map

  typedef std::map<uint32_t,ZdcDigits*> hashmapType;
  hashmapType digits_map;

  // Loop over ROB fragments
  //msg(MSG::INFO) << "ZDC: Inside EVENT !" << endmsg;
  int robCount = 0;
  ROBIterator rob = robFrags.begin();
  ROBIterator robEnd = robFrags.end();
  for (; rob != robEnd; ++rob)
    {

      ++robCount;
      if (debug)
	{
	  msg() << "ZDC: Treating ROB fragment " << robCount << endmsg;
	}

      // Unpack ROD data (slinks)

      RODPointer payloadBeg;
      RODPointer payload;
      RODPointer payloadEnd;
      (*rob)->rod_data(payloadBeg);
      payloadEnd = payloadBeg + (*rob)->rod_ndata();
      payload = payloadBeg;
      if (payload == payloadEnd)
	{
	  if (debug) msg() << "ZDC: ROB fragment empty" << endmsg;
	  continue;
	}

      // Check identifier
      const uint32_t sourceID = (*rob)->rod_source_id();
      if (debug)
	{
	  if (m_srcIdMap->subDet(sourceID) != m_subDetector || m_srcIdMap->daqOrRoi(sourceID)
	      != 0)
	    {
	      msg() << "ZDC: Wrong source identifier in data: " << MSG::hex << sourceID << MSG::dec
		    << endmsg;
	    }
	}
      const int rodCrate = m_srcIdMap->crate(sourceID);
      if (debug)
	{
	  msg() << "ZDC: Treating crate " << rodCrate << " slink " << m_srcIdMap->slink(sourceID)
		<< "From SubDetectorID " << m_srcIdMap->subDet(sourceID) << endmsg;
	}

      /* Comment out - this needs adaptation as there are too much L1 dialect in here
       * for now I wnat ot compile/run the bytestream to esd convertion, as I know how
       * to unpack the ppm data
       *
       */
      // First word is User Header
      ZdcUserHeader userHeader(*payload);
      const int minorVersion = (*rob)->rod_version() & 0xffff;
      userHeader.setVersion(minorVersion);
      const int headerWords = userHeader.words();
      if (headerWords != 1 && debug)
	{
	  msg() << "ZDC: Unexpected number of user header words: " << headerWords << endmsg;
	}
      // Does this makes sense??? Look above headerWords == 1
      for (int i = 0; i < headerWords; ++i)
	++payload;
      // triggered slice offsets
      const int trigLut = userHeader.ppmLut();
      const int trigFadc = userHeader.ppmFadc();
      // FADC baseline lower bound
      m_fadcBaseline = userHeader.lowerBound();
      if (debug)
	{
	  msg() << "ZDC: Minor format version number: " << MSG::hex << minorVersion << MSG::dec
		<< endmsg << "ZDC: LUT triggered slice offset:  " << trigLut << endmsg
		<< "ZDC: FADC triggered slice offset: " << trigFadc << endmsg
		<< "ZDC: FADC baseline lower bound:   " << m_fadcBaseline << endmsg;
	}
      const int runNumber = (*rob)->rod_run_no() & 0xffffff;

      // Find the number of channels per sub-block
      int chanPerSubBlock = 0;
      if (payload != payloadEnd)
	{
	  if (ZdcSubBlock::wordType(*payload) != ZdcSubBlock::HEADER)
	    {
	      msg(MSG::ERROR) << "Missing Sub-block header" << endmsg;
	      return StatusCode::FAILURE;
	    }
	  ZdcPpmSubBlock testBlock;
	  //Fix coverity CID 19251
	  //payload = testBlock.read(payload, payloadEnd);
	  testBlock.read(payload, payloadEnd);
	  chanPerSubBlock = testBlock.channelsPerSubBlock();
	  if (chanPerSubBlock == 0)
	    {
	      msg(MSG::ERROR) << "Unsupported version/data format: " << testBlock.version()
			      << "/" << testBlock.format() << endmsg;
	      return StatusCode::FAILURE;
	    }
	  if (m_channels % chanPerSubBlock != 0)
	    {
	      msg(MSG::ERROR) << "Invalid channels per sub-block: " << chanPerSubBlock << endmsg;
	      return StatusCode::FAILURE;
	    }
	  if (debug) msg() << "Channels per sub-block: " << chanPerSubBlock << endmsg;
	}
      else
	{
	  if (debug) msg() << "ROB fragment contains user header only" << endmsg;
	  continue;
	}
      const int numSubBlocks = m_channels / chanPerSubBlock;

      // Loop over PPMs

      payload = payloadBeg;
      for (int i = 0; i < headerWords; ++i)
	++payload;
      while (payload != payloadEnd)
	{

	  // Get all sub-blocks for one PPM

	  int crate = 0;
	  int module = 0;
	  m_ppmBlocks.clear();
	  for (int block = 0; block < numSubBlocks; ++block)
	    {
	      if (ZdcSubBlock::wordType(*payload) != ZdcSubBlock::HEADER
		  || ZdcPpmSubBlock::errorBlock(*payload))
		{
		  msg(MSG::ERROR) << "Unexpected data sequence" << endmsg;
		  return StatusCode::FAILURE;
		}
	      if (chanPerSubBlock != m_channels && ZdcSubBlock::seqno(*payload) != block
		  * chanPerSubBlock)
		{
		  if (debug)
		    {
		      msg() << "Unexpected channel sequence number: "
			    << ZdcSubBlock::seqno(*payload) << " expected " << block
			* chanPerSubBlock << endmsg;
		    }
		  if (!m_ppmBlocks.empty()) break;
		  else
		    {
		      if (!debug)
			{
			  msg(MSG::ERROR) << "Unexpected channel sequence number" << endmsg;
			}
		      return StatusCode::FAILURE;
		    }
		}
	      ZdcPpmSubBlock* const subBlock = new ZdcPpmSubBlock();
	      m_ppmBlocks.push_back(subBlock);
	      payload = subBlock->read(payload, payloadEnd);
	      if (block == 0)
		{
		  crate = subBlock->crate();
		  module = subBlock->module();
		  if (debug)
		    {
		      msg() << "Module " << module << endmsg;
		      if (crate != rodCrate)
			{
			  msg() << "Inconsistent crate number in ROD source ID" << endmsg;
			}
		    }
		}
	      else
		{
		  if (subBlock->crate() != crate)
		    {
		      msg(MSG::ERROR) << "Inconsistent crate number in sub-blocks" << endmsg;
		      return StatusCode::FAILURE;
		    }
		  if (subBlock->module() != module)
		    {
		      msg(MSG::ERROR) << "Inconsistent module number in sub-blocks" << endmsg;
		      return StatusCode::FAILURE;
		    }
		}
	      if (payload == payloadEnd && block != numSubBlocks - 1)
		{
		  if (debug) msg() << "Premature end of data" << endmsg;
		  break;
		}
	    }

	  // Is there an error block?

	  //delete m_errorBlock;
	  //m_errorBlock = 0;
	  m_errorBlock->clear();
	  if (payload != payloadEnd)
	    {
	      if (ZdcSubBlock::wordType(*payload) == ZdcSubBlock::HEADER
		  && ZdcPpmSubBlock::errorBlock(*payload))
		{
		  if (debug) msg() << "Error block found" << endmsg;
		  //m_errorBlock = new ZdcPpmSubBlock();
		  payload = m_errorBlock->read(payload, payloadEnd);
		  if (m_errorBlock->crate() != crate)
		    {
		      msg(MSG::ERROR) << "Inconsistent crate number in error block" << endmsg;
		      return StatusCode::FAILURE;
		    }
		  if (m_errorBlock->module() != module)
		    {
		      msg(MSG::ERROR) << "Inconsistent module number in error block" << endmsg;
		      return StatusCode::FAILURE;
		    }
		  if (m_errorBlock->dataWords() && !m_errorBlock->unpack())
		    {
		      if (debug)
			{
			  std::string errMsg(m_errorBlock->unpackErrorMsg());
			  msg() << "Unpacking error block failed: " << errMsg << endmsg;
			}
		    }
		  //delete m_errorBlock;
		}
	    }

	  // Loop over sub-blocks and fill trigger towers

	  const int actualSubBlocks = m_ppmBlocks.size();
	  for (int block = 0; block < actualSubBlocks; ++block)
	    {
	      ZdcPpmSubBlock* const subBlock = m_ppmBlocks[block];
	      subBlock->setLutOffset(trigLut);
	      subBlock->setFadcOffset(trigFadc);
	      subBlock->setPedestal(m_pedestal);
	      subBlock->setFadcBaseline(m_fadcBaseline);
	      subBlock->setRunNumber(runNumber);
	      if (debug)
		{
		  msg() << "Unpacking sub-block version/format/seqno: " << subBlock->version()
			<< "/" << subBlock->format() << "/" << subBlock->seqno() << endmsg;
		}
	      if (subBlock->dataWords() && !subBlock->unpack())
		{
		  if (debug)
		    {
		      std::string errMsg(subBlock->unpackErrorMsg());
		      msg() << "Unpacking PPM sub-block failed: " << errMsg << endmsg;
		    }
		}
	      if (m_printCompStats) addCompStats(subBlock->compStats());
	      for (int chan = 0; chan < chanPerSubBlock; ++chan)
		{
		  const int channel = block * chanPerSubBlock + chan;
		  std::vector<int> lut;
		  std::vector<int> fadc;
		  std::vector<int> bcidLut;
		  std::vector<int> bcidFadc;
		  subBlock->ppmData(channel, lut, fadc, bcidLut, bcidFadc);
//		  int trigLutKeep = trigLut;
//		  int trigFadcKeep = trigFadc;
		  if (lut.size() < size_t(trigLut + 1))
		    {
		      if (debug)
			{
			  msg() << "Triggered LUT slice from header "
				<< "inconsistent with number of slices: " << trigLut << ", "
				<< lut.size() << ", reset to 0" << endmsg;
			}
//		      trigLutKeep = 0;
		    }
		  if (fadc.size() < size_t(trigFadc + 1))
		    {
		      if (debug)
			{
			  msg() << "Triggered FADC slice from header "
				<< "inconsistent with number of slices: " << trigFadc << ", "
				<< fadc.size() << ", reset to 0" << endmsg;
			}
//		      trigFadcKeep = 0;
		    }
		  /*
		    LVL1::DataError errorBits(0);
		    if (m_errorBlock)
		    {
		    errorBits.set(LVL1::DataError::PPMErrorWord,
		    m_errorBlock->ppmError(channel));
		    errorBits.set(LVL1::DataError::SubStatusWord, m_errorBlock->subStatus());
		    }
		    else
		    {
		    errorBits.set(LVL1::DataError::PPMErrorWord, subBlock->ppmError(channel));
		    const ZdcPpmSubBlock* const lastBlock = m_ppmBlocks[actualSubBlocks - 1];
		    errorBits.set(LVL1::DataError::SubStatusWord, lastBlock->subStatus());
		    }
		    // Wrong bit set for compressed formats 1.01 to 1.03
		    if (subBlock->format() > 1 && subBlock->seqno() < 4)
		    {
		    errorBits.set(LVL1::DataError::ModuleError, (errorBits.error()
		    >> (LVL1::DataError::ModuleError + 1)) & 0x1);
		    }
		    const int error = errorBits.error();
		  */
		  // Only save non-zero data

		  const bool any = std::accumulate(lut.begin(), lut.end(), 0)
		    || std::accumulate(fadc.begin(), fadc.end(), 0)
		    || std::accumulate(bcidLut.begin(), bcidLut.end(), 0)
		    || std::accumulate(bcidFadc.begin(), bcidFadc.end(), 0);


		  // Channel as unpacked is a SLink channel ordering, which is different
		  // from PPM channel ordering
		  // the sequence is :
		  // All channel A MCM
		  // All channel D MCM
		  // All channel B MCM
		  // All channel C MCM
		  Identifier chan_id;

		  int ppmChannel = subBlock->getPpmChannel(channel);

		  msg(MSG::DEBUG) << "--> ZCS: " << ZdcCablingService::getInstance() << endmsg;
		  chan_id = ZdcCablingService::getInstance()->h2s_channel_id(module, ppmChannel);

		  const uint32_t chan_hash = chan_id.get_identifier32().get_compact();
		  msg(MSG::DEBUG) << "chan_hash = " << chan_hash << endmsg;

		  //Change after uncommenting the above
		  //if (any || error)
		  if (any)
		    {
		      int gain  = ZdcCablingService::getInstance()->hwid2gain(module,ppmChannel);
		      int delay = ZdcCablingService::getInstance()->hwid2delay(module,ppmChannel);
		      if (debug)
			{
			  msg(MSG::DEBUG) << endmsg;
			  msg(MSG::DEBUG) <<" --------------------------------------" << endmsg;
			  msg(MSG::DEBUG) <<
			    "--> ZDC: [SubDet., Crate, Mod., Slink Chn., PPM Chan.] ";
			  msg(MSG::DEBUG) <<
			    "[" << m_subDetector << ", " << crate << ", " <<
			    module << ", " << channel << "," << ppmChannel << "]" << endmsg;

			  msg(MSG::DEBUG) << "--> ZDC: LUT:      ";
			  printVec(lut);
			  msg(MSG::DEBUG) << endmsg;

			  msg(MSG::DEBUG) << "--> ZDC: FADC:     ";
			  printVec(fadc);
			  msg(MSG::DEBUG) << endmsg;

			  msg(MSG::DEBUG) << "--> ZDC: bcidLUT:  ";
			  printVec(bcidLut);
			  msg(MSG::DEBUG) << endmsg;

			  msg(MSG::DEBUG) << "--> ZDC: bcidFADC: ";
			  printVec(bcidFadc);
			  msg(MSG::DEBUG) << endmsg;

			  msg(MSG::DEBUG) << "--> ZDC ID: " << chan_id.getString() << endmsg;
			  msg(MSG::DEBUG) << "--> ID side: " << m_zdcID->side(chan_id) << endmsg;
			  msg(MSG::DEBUG) << "--> ID module: " << m_zdcID->module(chan_id) << endmsg;
			  msg(MSG::DEBUG) << "--> ID type: " << m_zdcID->type(chan_id) << endmsg;
			  msg(MSG::DEBUG) << "--> ID channel: " << m_zdcID->channel(chan_id) << endmsg;

			  msg(MSG::DEBUG) << "gain=" << gain << " delay=" << delay << endmsg;

			  msg(MSG::DEBUG) <<" --------------------------------------" << endmsg;

			}

		      hashmapType::iterator iter = digits_map.find(chan_hash);
		      if (iter == digits_map.end())
			{
			  digits_map.insert(std::pair<uint32_t,ZdcDigits*>(chan_hash,new ZdcDigits(chan_id)));
			  iter = digits_map.find(chan_hash);
			}
		      if (iter != digits_map.end())
			{
			  if (gain==0&&delay==0) (*iter).second->set_digits_gain0_delay0(fadc);
			  if (gain==1&&delay==0) (*iter).second->set_digits_gain1_delay0(fadc);
			  if (gain==0&&delay==1) (*iter).second->set_digits_gain0_delay1(fadc);
			  if (gain==1&&delay==1) (*iter).second->set_digits_gain1_delay1(fadc);
			}

		    }
		}
	    }
	}
    }

  hashmapType::iterator iter = digits_map.begin();
  hashmapType::iterator iter_end = digits_map.end();

  while (iter != iter_end)
    {
      ttCollection->push_back((*iter).second);
      ++iter;
    }

  msg(MSG::DEBUG) << "-->ZDC: Collection has " << ttCollection->size() << " elements " << endmsg;

  return StatusCode::SUCCESS;
}
//==================================================================================================

//==================================================================================================
// Conversion of ZDCCollection to bytestream
StatusCode ZdcByteStreamTool::convert( const ZdcDigitsCollection* const /*ttCollection*/,
				       RawEventWrite* const /*re*/)
{
        //if (re) ttCollection->size();
	//See PpmByteStreamTool for a implementation
	return StatusCode::SUCCESS;
}
//==================================================================================================

//==================================================================================================
// Add compression stats to totals
void ZdcByteStreamTool::addCompStats(const std::vector<uint32_t>& stats)
{
	if (stats.empty()) return;
	const int n = stats.size();
	if (m_compStats.empty()) m_compStats.resize(n);
	for (int i = 0; i < n; ++i)
		m_compStats[i] += stats[i];
}
//==================================================================================================

//==================================================================================================
// Print compression stats
void ZdcByteStreamTool::printCompStats() const
{
	msg() << "Compression stats format/count: ";
	const int n = m_compStats.size();
	for (int i = 0; i < n; ++i)
	{
		msg() << " " << i << "/" << m_compStats[i];
	}
	msg() << endmsg;
}
//==================================================================================================

//==================================================================================================
// Return reference to vector with all possible Source Identifiers
const std::vector<uint32_t>& ZdcByteStreamTool::sourceIDs()
{
  if (m_sourceIDs.empty()) {
    const int maxlinks = m_srcIdMap->maxSlinks();
    for (int crate = 0; crate < m_crates; ++crate) {
      for (int slink = 0; slink < maxlinks; ++slink) {
        const int daqOrRoi = 0;
        const uint32_t rodId = m_srcIdMap->getRodID(crate, slink, daqOrRoi,
                                                            m_subDetector);
        const uint32_t robId = m_srcIdMap->getRobID(rodId);
        m_sourceIDs.push_back(robId);
      }
    }
  }
  return m_sourceIDs;
}
//==================================================================================================

//==================================================================================================
// Print a vector
void ZdcByteStreamTool::printVec(const std::vector<int>& vec) const
{
	std::vector<int>::const_iterator pos;
	//msg() << "-----> " ;
	for (pos = vec.begin(); pos != vec.end(); ++pos)
	{
		if (pos != vec.begin()) msg() << ",";
		msg() << *pos;
	}
	//msg() << endmsg;
}
//==================================================================================================

