/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelRodDecoder.h"

#include "PixelCabling/IPixelCablingSvc.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "PixelConditionsServices/IPixelByteStreamErrorsSvc.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"  
#include "ExtractCondensedIBLhits.h"
#include "PixelByteStreamModuleMask.h"
#include "eformat/SourceIdentifier.h"

#include "GaudiKernel/IssueSeverity.h"

#include <iostream>
#include <string>
#include <fstream>



//#define PIXEL_DEBUG ;
//#define PLOTS ; // this flag is used only for debuggin purposes, to create plots for internal validation
//--------------------------------------------------------------------------- constructor
PixelRodDecoder::PixelRodDecoder
( const std::string& type, const std::string& name,const IInterface* parent )
  :  AthAlgTool(type,name,parent),
     m_pixelCabling("PixelCablingSvc",name),
     m_condsummary("PixelConditionsSummarySvc",name),
     m_errors("PixelByteStreamErrorsSvc",name)
{
  declareInterface< IPixelRodDecoder  >( this ); 
  declareProperty ("ErrorsSvc",m_errors);
  m_is_ibl_present = false;
  m_is_ibl_module = false;
}
	     
//--------------------------------------------------------------------------- destructor
//destructor  
PixelRodDecoder::~PixelRodDecoder() {

}

//--------------------------------------------------------------------------- initialize


StatusCode PixelRodDecoder::initialize() {
  msg(MSG::INFO) << " initialize " <<endreq; 
  StatusCode sc;
  sc = AthAlgTool::initialize(); 
  if (sc.isFailure()) return sc;


  // Retrieve id mapping 
  if (m_pixelCabling.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve service " << m_pixelCabling << endreq;
    return StatusCode::FAILURE;
  } else 
    msg(MSG::INFO) << "Retrieved service " << m_pixelCabling << endreq;

  if (detStore()->retrieve(m_pixel_id, "PixelID").isFailure()) {
    msg(MSG::FATAL) << "Could not get Pixel ID helper" << endreq;
    return StatusCode::FAILURE;
  }

  const InDetDD::PixelDetectorManager* pixelManager;
  
  if (detStore()->retrieve(pixelManager, "Pixel").isFailure()) {
    msg(MSG::FATAL) << "failed to get Pixel Manager" << endreq; 
    return StatusCode::FAILURE; 
  }

  // check if the ibl layer is present or not (this is necessary for backward compatibility with non-IBL geometries)  
  m_is_ibl_present = false;  
  const InDetDD::SiNumerology& pixSiNum = pixelManager->numerology(); 
  m_is_ibl_present = (pixSiNum.numLayers() == 4);
  msg(MSG::INFO) << "m_is_ibl_present = " << m_is_ibl_present << endreq;

  // Retrieve Pixel Conditions Summary 
  if (m_condsummary.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_condsummary << endreq;
    return StatusCode::FAILURE;
  } else 
    msg(MSG::INFO) << "Retrieved tool " << m_condsummary << endreq;

  // Retrieve Pixel Errors Service 
  if (m_errors.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve ByteStream Errors tool " << m_errors << endreq;
    return StatusCode::FAILURE;
  } else 
    msg(MSG::INFO) << "Retrieved ByteStream Errors tool " << m_errors << endreq;

  masked_errors = 0;
  
  return StatusCode::SUCCESS;
}


//--------------------------------------------------------------------------- finalize
StatusCode PixelRodDecoder::finalize() {

#ifdef PIXEL_DEBUG
  msg(MSG::VERBOSE) << "in PixelRodDecoder::finalize" << endreq;
  msg(MSG::DEBUG) << masked_errors << " times BCID and LVL1ID error masked" << endreq;
#endif
  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------------------------------- fillCollection
StatusCode PixelRodDecoder::fillCollection( const ROBFragment *robFrag, PixelRDO_Container* rdoIdc, 
					    std::vector<IdentifierHash>* vecHash)
{
#ifdef PIXEL_DEBUG
  msg(MSG::VERBOSE) << "-------------------------------------------------------------------------------------------------------------" << endreq;  
  msg(MSG::DEBUG) << "Entering PixelRodDecoder" << endreq;
#endif

  Identifier invalidPixelId = Identifier(); // used by Cabling for an invalid entry
  Identifier pixelId;
  uint64_t onlineId(0);
    
  m_is_ibl_module = false;
  m_is_dbm_module = false;

  uint32_t rodId = robFrag->rod_source_id(); // get source ID => method of ROBFragment, Returns the source identifier of the ROD fragment
  uint32_t robId = robFrag->rob_source_id(); // get source ID => returns the Identifier of the ROB fragment. More correct to use w.r.t. rodId.
  if (m_pixelCabling->isIBL(robId) && (robId != rodId)) {
      ATH_MSG_WARNING("Discrepancy in IBL SourceId: ROBID 0x" << std::hex << robId << " unequal to RODID 0x" << rodId);
  }


#ifdef PIXEL_DEBUG
  msg(MSG::DEBUG) << "robId: 0x" << std::hex << robId << std::dec << endreq;
#endif
  // check the ROD status for truncation
  if (robFrag->nstatus() != 0) { 
    const uint32_t* rob_status;
    robFrag->status(rob_status);
    if ((*rob_status) != 0) {
      addRODError(robId,*rob_status);

      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ROB status word for robid 0x"<< std::hex << robId << " is non-zero 0x"
					      << (*rob_status) << std::dec<< endreq;

      if (((*rob_status) >> 27) & 0x1) {
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"ROB status word for robid 0x"<< std::hex << robId << std::dec <<" indicates data truncation."<< endreq;
	return StatusCode::RECOVERABLE;
      }

      if (((*rob_status) >> 31) & 0x1) {
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ROB status word for robid 0x"<< std::hex << robId<< std::dec <<" indicates resource was masked off."<< endreq;
	return StatusCode::RECOVERABLE;
      }
    } // end of the "    if ((*rob_status) != 0)"
  }   // end of the "  if (robFrag->nstatus() != 0) "

  unsigned int errorcode = 0;
  // m_errors->reset(); // reset the collection of errors

  StatusCode sc = StatusCode::SUCCESS;


  // Fill the Collections for this ROB.
  unsigned int mBCID           = 0;   // create module BCID variable 
  unsigned int mLVL1ID         = 0;   // create module LVL1ID variable
  //unsigned int mLVL1IDskip     = 0;   // create module LVL1ID variable   -- temporarily removed
  unsigned int mLink           = 0;   // create module Link variable
  //unsigned int mFeI4Bflag      = 0; // create IBL FeI4B flag bit variable -- temporarily removed
  int mLVL1A                   = 0;  // create LVL1A variable
  //  unsigned int mLinkNumTrailer = 0; // create the variable Link Number present in the IBL Trailer word
  //unsigned int mSkippedTrigTrailer = 0; // create IBL skipped trigger counter variable (in IBL trailer)   -- temporarily removed
  //  unsigned int mTimeOutErrorBit = 0; // create IBL timeout error bit variable (IBL trailer)
  //unsigned int mCondensedModeBit = 0; // create IBL condensed mode bit variable (in IBL trailer)  -- temporarily removed
  //  unsigned int mBitFlips       = 0; // create module bitflip variable

  // take the Header "nnnnn" bits as a reference for the other words (hit, feFlag, trailer words)
  uint32_t linkNum_IBLheader = 0; // create variable containing 5 bits, corresponding to the "nnnnn" bits in the header IBL/DBM word
  //  uint32_t linkNum_IBLword = 0; // create variable containing 5 bits, corresponding to the "nnnnn" bits in the hit/feFlag/trailer IBL/DBM word // At least temporarily not used, because IBL data format is not clear (Franconi, 17.06.2014)
  uint32_t fe_IBLheader = 0; // create fe variable, corresponding to the IBL FE number w.r.t. the sLink (bits 24-26 "nnn" in Header)
  //  uint32_t fe_IBLword = 0; // create fe variable, corresponding to the IBL FE number w.r.t. the sLink (bits 24-26 "nnn" in Hit/trailer words, bits 25-27 in FeFlag word) // At least temporarily not used, because IBL data format is not clear (Franconi, 17.06.2014)

  uint32_t sLinkSourceId = 0; // create sLink variable, corresponding to the number of the sLink of IBL/DBM, runs from 1 to 4. Retrieved from the robId
  uint32_t sLinkHeader = 0; // create sLink variable, retrieved from the header words
  //  uint32_t sLinkWord = 0; // create sLink variable, retrieved from the header/hit/feFlag/trailer words


  bool link_start = false;   // link (module) header found flag - reset by link (module) trailer
  // why unused FIXME      bool link_first = true;   // first link (module) flag
  //bool normal = true;   // BCID normal flag
  IdentifierHash offlineIdHash; // offlineIdHash
  IdentifierHash previous_offlineIdHash;   // previous module offline identifier
  int mBCID_offset = -1;   // BCID offset to calculate L1A
  unsigned int mBCID_max = 256;   // cycle of BCID values
  
  int countHitCondensedWords = 0; // counter to the "condensed hits" of the IBL
  uint32_t condensedWords[nCondensedWords] = {}; // this vector saves the 4 condensed hit words that are found 
  bool are_4condensed_words = false;
  
  
  uint32_t dataword_it_end = robFrag->rod_ndata();   // number of data words
  uint32_t rawDataWord;
  
  IdentifierHash skipHash = 0xffffffff, lastHash = 0xffffffff; // used for decoding to not have to search the vector all the time
  
  PixelRawCollection* m_coll = NULL;
  PixelRDO_Container::const_iterator m_cont_it;
  
  // get the data from the word
  OFFLINE_FRAGMENTS_NAMESPACE::PointerType vint;
  robFrag->rod_data(vint);
  
  
  
  
  // ============== ============== ============== ============== ============== ============== ============== ============== //
  // loop over the data in the fragment
  for (uint32_t dataword_it = 0; dataword_it < dataword_it_end; ++dataword_it) {   // loop over ROD
    rawDataWord = vint[dataword_it];   // set the dataword to investigate
    uint32_t word_type = getDataType(rawDataWord,link_start);   // get type of data word
    unsigned int headererror; 
    int trailererror;
    unsigned int MCCFlags; 
    unsigned int FEFlags;
    
    unsigned int serviceCode;
    unsigned int serviceCodeCounter;


   
    switch (word_type) { // depending on type of word call the right decoding method
      
      //-------------------------------------------------------------------------------------------- HEADER WORD FOUND
    case PRB_LINKHEADER:   // link (module) header found

#ifdef PIXEL_DEBUG
      msg(MSG::DEBUG) << "Header word found" << endreq;
      msg(MSG::VERBOSE) << "link_start = " << link_start << endreq;
#endif
      
      if (link_start) {    // if header found before last header was closed by a trailer -> error
	msg(MSG::WARNING) << "unexpected link header found: 0x" << std::hex << rawDataWord << ", data corruption - ROBID: 0x" << std::hex << robId << std::dec << endreq; // << " link: " << mLink << endreq;
	m_errors->addDecodingError(); 
	// do NOT add a "continue;" here, because it is more probable that this header is then related to the next hit/trailer words, so it is worth decoding it
      }
      else {
#ifdef PIXEL_DEBUG
	msg(MSG::DEBUG) << "Header decoding starts" << endreq;
#endif
      }
      link_start = true;   // setting link (module) header found flag
      are_4condensed_words = false;
      m_is_ibl_module = false;
      m_is_dbm_module = false;
      countHitCondensedWords = 0;
#ifdef PIXEL_DEBUG
      msg(MSG::DEBUG) << "********************* robId: 0x" << std::hex << robId << std::dec << endreq;
#endif
      if (m_is_ibl_present) {
	m_is_ibl_module = m_pixelCabling->isIBL(robId);
	m_is_dbm_module = m_pixelCabling->isDBM(robId);
      }
#ifdef PIXEL_DEBUG
      msg(MSG::DEBUG) << "m_is_ibl_module = " << m_is_ibl_module << "  m_is_dbm_module = " << m_is_dbm_module << endreq;
#endif
      errorcode = 0; // reset errorcode      

      if (m_is_ibl_module || m_is_dbm_module) { // decode IBL/DBM header word. 
#ifdef PIXEL_DEBUG
	msg(MSG::VERBOSE) << "Decoding the IBL/DBM header word: 0x" << std::hex << rawDataWord << std::dec << endreq;
#endif
#ifdef PLOTS
	std::cout << "[Plot1]:0x " << std::hex << rawDataWord << std::dec << std::endl;
	std::cout << "[Plot1]:(dec) " << rawDataWord << std::endl;
#endif
	// IBL header data format: 001nnnnnFLLLLLLLLLLLLLBBBBBBBBBB
	mBCID = decodeBCID_IBL(rawDataWord);   // decode IBL BCID: B
	mLVL1ID = decodeL1ID_IBL(rawDataWord);   // decode IBL LVL1ID: L
	//mFeI4Bflag = decodeFeI4Bflag_IBL(rawDataWord); // decode FeI4B flag bit: F    -- temporarily removed
	linkNum_IBLheader = decodeModule_IBL(rawDataWord);   // decode IBL FE number on the S-Link (range [0,7], as 8 FEs are connected to 1 S-Link in IBL) and the S-Link number itself: n 
	eformat::helper::SourceIdentifier sid_rob(robId);
	unsigned int moduleLink = sid_rob.module_id(); // it contains the 0xRRRL, where R indicates the RODid and L indicates the S-Link number
	sLinkSourceId = moduleLink & 0x000F; // retrieve S-Link number from the source Identifier
	if (sLinkSourceId  > 0x3) { // Check if SLink number for the IBL is correct!
	  msg(MSG::WARNING) << "SLink for the IBL/DBM, retrieved from the SourceIdentifier, is not in the correct range (0-3): SLink = " << sLinkSourceId << endreq;
	}
	fe_IBLheader = extractFefromLinkNum(linkNum_IBLheader);
	mLink = fe_IBLheader; // this is used to retrieve the onlineId. It contains only the 3 LSBs of the nnnnn, indicating the number of FE w.r.t. the SLink 
	sLinkHeader = extractSLinkfromLinkNum(linkNum_IBLheader); // this is used to check that the (redundant) info is correctly transmitted 
#ifdef PIXEL_DEBUG
	msg(MSG::VERBOSE) << "linkNum_IBL (from header word) = nnnnn: 0x" << std::hex << linkNum_IBLheader << ", sLinkWord (2 MSB): 0x" << sLinkHeader << ", fe (3 LSB): 0x" << fe_IBLheader << std::dec << endreq;
#endif
	if (sLinkHeader != sLinkSourceId) {
	  msg(MSG::WARNING) << "SLink retrieved from the source identifier differs from the SLink retrieved in the IBL/DBM header word: SLink(from SourceId) = 0x" << std::hex << sLinkSourceId << ", SLink (headerWord): 0x" << sLinkHeader << std::dec << endreq;
	}
      }


      else { // decode Pixel header word. Data format: 001PtlbxdnnnnnnnMMMMLLLLBBBBBBBB
#ifdef PIXEL_DEBUG
	msg(MSG::VERBOSE) << "Decoding Pixel header word: 0x" << std::hex << rawDataWord << std::dec << endreq;
#endif
#ifdef PLOTS
	std::cout << "[PlotA]:0x " << std::hex << rawDataWord << std::dec << std::endl;
       	std::cout << "[PlotA]:(dec) " << rawDataWord << std::endl;
#endif
	mBCID = decodeBCID(rawDataWord);   // decode Pixel BCID: B
	mLVL1ID = decodeL1ID(rawDataWord);   // decode Pixel LVL1ID: L
	//mLVL1IDskip = decodeL1IDskip(rawDataWord);   // decode Pixel skipped LVL1ID: M    -- temporarily removed
	mLink = decodeModule(rawDataWord);   // decode Pixel link (module): n         
	
	headererror = decodeHeaderErrors(rawDataWord);   // get link (module) header errors
	if (headererror != 0) { // only treatment for header errors now, FIXME
	  sc = StatusCode::RECOVERABLE;
	  errorcode = errorcode | (headererror << 20); //encode error as HHHHMMMMMMMMFFFFFFFFTTTT for header, flagword, trailer errors
	  if (headererror & (1 << 3))
	    m_errors->addPreambleError();
	  if (headererror & (1 << 2))
	    m_errors->addTimeOutError();
	  if (headererror & (1 << 1))
	    m_errors->addLVL1IDError();
	  if (headererror & (1 << 0))
	    m_errors->addBCIDError();
	}
      }

      onlineId = m_pixelCabling->getOnlineIdFromRobId(robId, mLink);
      if (onlineId == 0) msg(MSG::WARNING) << "Got invalid onlineId (= 0), from robID = 0x" << std::hex << robId << ", link = 0x" << mLink << endreq;

#ifdef PIXEL_DEBUG
      msg(MSG::VERBOSE) << "In decoder: got onlineId 0x" << std::hex << onlineId << endreq;
#endif

      offlineIdHash = m_pixelCabling->getOfflineIdHash(onlineId);

#ifdef PIXEL_DEBUG
      msg(MSG::VERBOSE) << "OfflineIdHash: " << offlineIdHash << endreq; 
#endif

      if (offlineIdHash != previous_offlineIdHash) {
	m_errors->addRead(offlineIdHash);
	mBCID_offset = mBCID;   // set BCID offset if this is first LVL1A
      }
      mLVL1A = mBCID - mBCID_offset;   // calculate the LVL1A
      if (mLVL1A < 0) {
	mLVL1A = mLVL1A + mBCID_max;
      }
#ifdef PIXEL_DEBUG
      msg(MSG::VERBOSE) << "link header with BCID: " << mBCID << " LVL1ID: " << mLVL1ID << " LVL1A: " << mLVL1A << " link: " << mLink << " found" << endreq;
      msg(MSG::VERBOSE) << "got OfflineIDHash: 0x" << std::hex << offlineIdHash << " from OnlineID: 0x" <<  onlineId << std::dec << " link: " << mLink << endreq;
#endif
      
      if (offlineIdHash == 0xffffffff) {   // if link (module) online identifier (ROBID and link number) not found by mapping
	msg(MSG::WARNING) << "unknown OnlineId identifier - not found by mapping - ROBID: 0x" << std::hex << robId << " link: 0x" << mLink << std::dec << endreq;   // link (module) online identifier error message 
	m_errors->addDecodingError();
	link_start = false;   // resetting link (module) header found flag
	continue;   // skip this word and process next one
      }
      break;
      
      //-------------------------------------------------------------------------------------------- HIT DATA WORD FOUND
    case PRB_DATAWORD: // hit data found
      if (link_start) {   // if header found before hit -> expected
#ifdef PIXEL_DEBUG
	msg(MSG::DEBUG) << "Hit word found" << endreq;
#endif
	unsigned int mFE = 0;
	unsigned int mRow = 0;
	unsigned int mColumn = 0;
	int mToT = 0;
	uint32_t row[nHits], col[nHits];
	int tot[nHits];
      
	if (m_is_ibl_module || m_is_dbm_module) { // check all the different types of IBL/DBM hit words (condensed and not condensed)
#ifdef PIXEL_DEBUG
	  msg(MSG::DEBUG) << "Decoding IBL/DBM hit word: 0x" << std::hex << rawDataWord << std::dec << endreq;
#endif
#ifdef PLOTS
	  std::cout << "[Plot1]:0x " << std::hex << rawDataWord << std::dec << std::endl;
	  std::cout << "[Plot1]:(dec) " << rawDataWord << std::endl;
#endif
	  if (((rawDataWord & PRB_HITDATAMASK) == PRB_FIRSTHITCONDENSEDWORD) && (countHitCondensedWords == 0)) { // it is the first of the 4 hit condensed words (IBL/DBM)
	    // Here, if the IBL and DBM words are mixed for any mistake in the byte stream, there's no way to check it. :-(
	    countHitCondensedWords = 1;
	    // msg(MSG::VERBOSE) << countHitCondensedWords << " Condensed" << endreq;
	    are_4condensed_words = false; // All 4 words have to be found before turning it on
	    condensedWords[0] = rawDataWord;
	    continue;
	  }
	  else if (countHitCondensedWords == 1) { // it is the second of the 4 hit condensed words (IBL)
	    ++countHitCondensedWords;
	    // msg(MSG::VERBOSE) << countHitCondensedWords << " Condensed" << endreq;
	    condensedWords[1] = rawDataWord;
	    continue;
	  }
	  else if (countHitCondensedWords == 2) { // it is the third of the 4 hit condensed words (IBL)
	    ++countHitCondensedWords; // this variable is = 3
	    // msg(MSG::VERBOSE) << countHitCondensedWords << " Condensed" << endreq;
	    condensedWords[2] = rawDataWord;
	    continue;
	  }
	  else if (((rawDataWord & PRB_HITDATAMASK) == PRB_LASTHITCONDENSEDWORD) && (countHitCondensedWords == 3)) { // it is the fourth of the 4 hit condensed words (IBL)
	    //	    ++countHitCondensedWords; // this is not necessary, comment it out when commenting the verbose message below
	    //	    msg(MSG::VERBOSE) << countHitCondensedWords << " Condensed" << endreq;
	    are_4condensed_words = true;
	    condensedWords[3] = rawDataWord;
	    uint32_t hitwords[nHits];
	  
	    //Condensed words
	    //  101RRRRRTTTTTTTTCCCCCCCRRRRRRRRR 
	    //  1CCCRRRRRRRRRTTTTTTTTCCCCCCCRRRR
	    //  1TTTCCCCCCCRRRRRRRRRTTTTTTTTCCCC
	    //  111TTTTTTTTCCCCCCCRRRRRRRRRTTTTT
	    // They become 5 hitwords with format : TTTTTTTTCCCCCCCRRRRRRRRR
	  
	    // first hitword: simple mask of the first condensed word
	    hitwords[0] = (condensedWords[0] & mask24);
	  
	    // second hitword: takes the "RRRRR" from the first condensed word + the "TTTTTTTTCCCCCCCRRRR" from the second (shifted left 5 bits to accomodate the "RRRRR" as LSB)
	    hitwords[1] = ((condensedWords[0] >> (skipRow + skipCol+ skipTOT) ) & mask5) + ((condensedWords[1] & mask19) << skip5);
	  
	    // third hitword: takes remaining "CCCRRRRRRRRR" from the second condensed word + the "TTTTTTTTCCCC" from the third (shifted left 12 bits to accomodate the LSB coming from the second condensed word
	    hitwords[2] = ((condensedWords[1] >> (skip4 + skipCol + skipTOT)) & mask12) + ((condensedWords[2] & mask12) << (skipRow + skip3));
	  
	    // fourth hitword: takes remaning "TTTCCCCCCCRRRRRRRRR" from the third condensed word +  the "TTTTT" from the fourth (shifted left 19 bits to accomodate the LSB coming from the third condensed word
	    hitwords[3] = ((condensedWords[2] >> (skipTOT + skip4)) & mask19) + ((condensedWords[3] & mask5) << (skip3 + skipRow + skipCol));
	  
	    // fifth hitword: simply shift 5 right to eliminate the "TTTTT" and mask24
	    hitwords[4] = (condensedWords[3] >> skip5) & mask24;
#ifdef PIXEL_DEBUG
	    msg(MSG::VERBOSE) << "4 consecutive IBL hit words found. Condensed hits are being decoded" << endreq;	    
	    // NOW all the hitwords have the shape (8T)(7C)(9R)
	    msg(MSG::VERBOSE) << "The 5 IBL hits (with shape (8ToT)(7Col)(9Row)) contain the following info: row, col and ToT (ToT is still 8 bits):" << endreq;
#endif
	    for (unsigned int i(0); i < nHits; ++i) {
	      row[i] = divideHits (hitwords[i], 0, 8); 
	      col[i] = divideHits (hitwords[i], 9, 15); 
	      tot[i] = divideHits (hitwords[i], 16, 23);
          if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "hitword[" << i << "] = 0x" << std::hex << hitwords[i] << ",  row: 0x" << row[i] << ",  col: 0x" << col[i] << ",  8-bit ToT: 0x" << tot[i] << std::dec << endreq;
	    }
	    countHitCondensedWords = 0;
	    //	    linkNum_IBLword = linkNum_IBLheader;
	    //	    fe_IBLword = fe_IBLheader;
	    //	    sLinkWord = sLinkHeader;
	  } // end of the condensed hit words (IBL)
	
	
	  else if ((rawDataWord & PRB_HIT_IBL_MASK) == PRB_HIT_NOTCONDENSED) { // it's a IBL not-condensed hit word
	    if (countHitCondensedWords != 0) { // I received some IBL words, but less than 4, there was an error in the rod transmission
	      msg(MSG::WARNING) << "Unexpected interruption of flow of IBL condensed words - data corruption - hit(s) ignored" << endreq;   // unexpected hit error message
	      m_errors->addDecodingError();
	      countHitCondensedWords = 0;
	    }
	    // IBL non-condensed Hit Words decoding:  
	    are_4condensed_words = false;
	    mRow = decodeRow(rawDataWord);
	    mColumn = decodeColumn(rawDataWord);
	    mToT = decodeToT(rawDataWord); // this needs, later in the code, to be divided into Tot(0) and Tot(1), because the IBL TOT information can give information of the ToTs from up to two adjacent pixels
	    //	    linkNum_IBLword = decodeLinkNumHit_IBL(rawDataWord); // this corresponds to the "nnnnn" bits of the bytestream convention. 
         	                                                 //They represent: 3 LSB = number of the FE linked by the S-Link sending the block (range [0,7]); 2 MSB = S-Link number (range [1,4]) 
	    //	    fe_IBLword = extractFefromLinkNum(linkNum_IBLword); 
	    //	    sLinkWord = extractSLinkfromLinkNum(linkNum_IBLword);
	    //	    	    if ((fe_IBLword != fe_IBLheader) || (sLinkWord != sLinkHeader)) {
        //	      msg(MSG::WARNING) << "The IBL header and non-condensed words do not contain the same link number information (bits 24-28)" << endreq;
	    //	    }
	  
#ifdef PIXEL_DEBUG
	    msg(MSG::DEBUG) << "hit dataword found for module offlineIDHash: " << offlineIdHash << " Row: 0x" << std::hex << mRow << " Col: 0x" <<  mColumn  << " Tot: 0x" << mToT << std::dec << endreq;   // hit found debug message
#endif
	  }
	  else { // it is an IBL/DBM hit word, but it hasn't been recognised
	    //FIXME Add error!!!!
	    msg(MSG::WARNING) << "An IBL/DBM hit word was sent, but was not recognised" << endreq;
	    m_errors->addDecodingError();
	  }
	
	  // computing the FE number on the silicon wafer for IBL ( mFE is = 0 for IBL 3D and the lower-eta FE on the planar sensor, 
	  // while is = 1 for the higher-eta FE on the planar sensor)
	  mFE = m_pixelCabling->getLocalFEI4(fe_IBLheader, onlineId);
	} // end of the if (m_is_ibl_module || m_is_dbm_module)
      
	else { // Pixel Hit Case
	  if (((rawDataWord & PRB_HIT_PIXEL_MASK) == PRB_HIT_NOTCONDENSED) &&  ((rawDataWord & PRB_DATAWORDEMPTY) == 0 ))// && (countHitCondensedWords == 0)){ // it is a Pixel Hit word
	    {
#ifdef PIXEL_DEBUG
	      msg(MSG::VERBOSE) << "Decoding Pixel hit word: 0x" << std::hex << rawDataWord << std::dec << endreq;
#endif
#ifdef PLOTS
	      std::cout << "[PlotA]:0x " << std::hex << rawDataWord << std::dec << std::endl;
	      std::cout << "[PlotA]:(dec) " << rawDataWord << std::endl;
#endif
	      if (countHitCondensedWords != 0)
		{ // I received some IBL words, but less than 4, there was an error in the rod transmission
		  msg(MSG::WARNING) << "Unexpected interruption of flow of IBL condensed words - data corruption - hit(s) ignored" << endreq;   // unexpected hit error message
		  m_errors->addDecodingError();
		  countHitCondensedWords = 0;
		}
	      //Pixel Hit Words decoding: 
	      are_4condensed_words = false;
	      mFE = decodeFE(rawDataWord);
	      mRow = decodeRow(rawDataWord);
	      mColumn = decodeColumn(rawDataWord);
	      mToT = decodeToT(rawDataWord);
#ifdef PIXEL_DEBUG
	      msg(MSG::DEBUG) << "hit dataword found for module offlineIDHash: " << offlineIdHash << " FE: " << mFE << " Row: " << mRow << " Col: " <<  mColumn << endreq;   // hit found debug message
#endif
	    } // end if "it is a pixel word"
	} // end of the condition "!m_is_ibl_module"
      
	// &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      
	if (vecHash != NULL) { // important for HLT
#ifdef PIXEL_DEBUG
	  msg(MSG::DEBUG) << "vecHash != NULL" << endreq;
#endif
	  if (offlineIdHash == skipHash) {// is the vector to be skipped (known from before)?
#ifdef PIXEL_DEBUG
	    msg(MSG::DEBUG) << "the offlineIdHash is equal to skipHash." << endreq;
#endif
	    continue;
	  }
	  else if (offlineIdHash != lastHash){
	    lastHash = offlineIdHash;
	    // 	    std::vector<IdentifierHash>::iterator p = find(vecHash->begin(),vecHash->end(),offlineIdHash);
	    // #ifdef PIXEL_DEBUG
	    // 	    msg(MSG::DEBUG) << "(m_condsummary->isActive(offlineIdHash)) = " << (m_condsummary->isActive(offlineIdHash)) << endreq;
	    // #endif
	    // 	    if (p == vecHash->end() || !(m_condsummary->isActive(offlineIdHash))){ // is the Hash to be skipped (not in the request list)? or is the module not active?
	    // 	      skipHash = offlineIdHash;
	    // #ifdef PIXEL_DEBUG
	    // 	      msg(MSG::DEBUG) << "setting skipHash = offlineIdHash" << endreq;
	    // #endif
	    //	      continue;
	    //	    }
	    //	    else{ // the Hash is to be filled, the cont iterator has to be set
	    // search for the collection (if it's not the old one)
	    m_cont_it = rdoIdc->indexFind(offlineIdHash);
	    // check if collection already exists and do a nasty trick
	    if (m_cont_it != rdoIdc->end()) {
	      m_coll = const_cast<PixelRawCollection*>(&**m_cont_it);
#ifdef PIXEL_DEBUG
	      msg(MSG::DEBUG) << "Collection ID = " << offlineIdHash << " already exists" << endreq;
#endif
	    }
	    else { 	   //if the Collection does not exist, create it
#ifdef PIXEL_DEBUG
	      msg(MSG::DEBUG) << " Collection ID = " << offlineIdHash	<< " does not exist, create it " << endreq;
#endif
	      m_coll = new PixelRawCollection (offlineIdHash);
	      // get identifier from the hash, this is not nice
	      Identifier ident = m_pixel_id->wafer_id(offlineIdHash);
	      // set the Identifier to be nice to downstream clients
	      m_coll->setIdentifier(ident);
	      // add collection into IDC
	      StatusCode sc = rdoIdc->addCollection(m_coll, offlineIdHash);
	      if (sc.isFailure()){
		msg(MSG::ERROR) << "failed to add Pixel RDO collection to container" << endreq;
		return sc;
	      }
	    }
	    //	    }
	  } // if (offlineIdHash == lastHash) nothing is to be done and the collection is filled
	} // end of if (vechHash != NULL)
      
      
	else {
#ifdef PIXEL_DEBUG
	  msg(MSG::DEBUG) << "vecHash == NULL" << endreq;
	  //	  msg(MSG::DEBUG) << "(m_condsummary->isActive(offlineIdHash)) = " << (m_condsummary->isActive(offlineIdHash)) << endreq;
#endif
	  //	  msg(MSG::DEBUG) << "(m_condsummary->isActive(offlineIdHash)) = " << (m_condsummary->isActive(offlineIdHash)) << endreq;
	  //	  if (!(m_condsummary->isActive(offlineIdHash))) // is the module active?
	  // reactivated // DE 28.03.2013 removed isActive() check temporarily pending update IBL conditions
	  //	    continue;
#ifdef PIXEL_DEBUG
	  msg(MSG::VERBOSE) << "(offlineIdHash != lastHash) = " << (offlineIdHash != lastHash) << endreq;
#endif
	  if (offlineIdHash != lastHash) { // vecHash == NULL: offline case; is the hash new?
	    lastHash = offlineIdHash;
	    // search for the collection (if it's not the old one)
	    m_cont_it = rdoIdc->indexFind(offlineIdHash);
	    // check if collection already exists and do a nasty trick
#ifdef PIXEL_DEBUG
	    msg(MSG::VERBOSE) << " Condition (m_cont_it != rdoIdc->end()) = " << (m_cont_it != rdoIdc->end()) << endreq;
#endif
	    if (m_cont_it != rdoIdc->end()) {
	      m_coll = const_cast<PixelRawCollection*>(&**m_cont_it);
#ifdef PIXEL_DEBUG
	      msg(MSG::DEBUG) << "Collection ID = " << offlineIdHash << " already exists" << endreq;
#endif
	    }
	    else { 	   //if the Collection does not exist, create it
#ifdef PIXEL_DEBUG
	      msg(MSG::DEBUG) << "Collection ID = " << offlineIdHash << " does not exist, create it " << endreq;
#endif
	      m_coll = new PixelRawCollection (offlineIdHash);
	      // get identifier from the hash, this is not nice
	      Identifier ident = m_pixel_id->wafer_id(offlineIdHash);
	      // set the Identifier to be nice to downstream clients
	      m_coll->setIdentifier(ident);
	      // add collection into IDC
	      StatusCode sc = rdoIdc->addCollection(m_coll, offlineIdHash);
	      if (sc.isFailure()) {
		msg(MSG::ERROR) << "failed to add Pixel RDO collection to container" << endreq;
		return sc;
	      }
	    }
	  }
	} // end of the else (what happens when (vecHash == NULL) )
      
	// &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      
	if (m_is_ibl_module || m_is_dbm_module) {
	  unsigned int num_cycles_toFillCollection(0);
	  if (are_4condensed_words) {
	    num_cycles_toFillCollection = nHits;
	  }
	  else {
	    num_cycles_toFillCollection = 1;
	    row[0] = mRow; // These 3 instructions are required to have be able to use the "for" cycle below
	    col[0] = mColumn;
	    tot[0] = mToT;
	  }
	  int IBLtot[2];
	  for (unsigned int i(0); i < num_cycles_toFillCollection; ++i) {
#ifdef PIXEL_DEBUG
	    msg(MSG::VERBOSE) << "ROW[" << i << "] = 0x"  << std::hex << row[i] << std::dec << ",  COL[" << i << "] = 0x" << std::hex << col[i] << std::dec << ",  8-bit TOT[" << i << "] = 0x" << std::hex << tot[i] << std::dec << endreq;
#endif
        if ((tot[i] & 0xF0) == 0x00) {  // This must not happen. If the first 4-bits of tot[i] == 1111, there was no hit, so the word shouldn't have been sent.
	      // Same happens if IBLtot[1] != 1111 (hit) and IBLtot[0]==1111 (no hit), but this second condition is included in the first.
	      msg(MSG::WARNING) << "Illegal IBL TOT code: ToT1 = 0x" << std::hex << (tot[i] >> 4) << " - ROB = 0x" << robId
          << ", hit word 0x" << rawDataWord << " decodes to row = " << std::dec << row[i] << " col = " << col[i] 
          << "ToT1 = 0x" << std::hex << IBLtot[0] << "ToT2 = 0x" << IBLtot[1] << endreq;

	      continue;
	    }
	    else {
          if (((row[i] != 0) && (col[i] != 0) && (row[i] <= 336) && (col[i] <= 80)) || m_is_dbm_module) { // FIXME : Hardcoded numbers. Should be ok as they are features of the FE-I4, but worth checking!
		pixelId = m_pixelCabling ->getPixelIdfromHash (offlineIdHash, mFE, row[i], col[i]);

		// this is to run a script to compare the log files from encoder and decoder
#ifdef PLOTS
		std::cout << "[VAL] " << std::hex << pixelId << " 0x" << robId << " 0x" << onlineId //<< " " << offlineIdHash
			  << std::dec << " " << m_pixel_id->eta_module(pixelId) << " " << m_pixel_id->phi_module(pixelId)
			  << " " << m_pixel_id->eta_index(pixelId) << " " << m_pixel_id->phi_index(pixelId) << std::endl;
#endif
#ifdef PIXEL_DEBUG
		msg(MSG::VERBOSE) << "PixelId: " << pixelId << endreq;
		int eta_i = m_pixel_id->eta_index(pixelId);
		int phi_i = m_pixel_id->phi_index(pixelId);
		int eta_m = m_pixel_id->eta_module(pixelId);
		int phi_m = m_pixel_id->phi_module(pixelId);
		msg(MSG::VERBOSE) << "          eta_i: " << eta_i << ", phi_i: " << phi_i << ",  eta_m: " <<  eta_m << ", phi_m: " << phi_m << endreq;
#endif
		if (pixelId == invalidPixelId) {
		  msg(MSG::WARNING) << "Illegal pixelId" << endreq;
		  m_errors->addInvalidIdentifier();
		  continue;
		} // end if "if (pixelId == invalidPixelId)"
		IBLtot[0] = divideHits(tot[i], 4, 7); // TOT_1 -> is the IBL ToT information coming from the IBL that corresponds to the pixel (col, row)
		IBLtot[1] = divideHits(tot[i], 0, 3); // TOT_2 -> is the IBL ToT information coming from the IBL that corresponds to the pixel (col, row+1)
#ifdef PIXEL_DEBUG
		msg(MSG::VERBOSE) << "Starting from tot = 0x" << std::hex << tot[i] << " IBLtot[0] = 0x" << std::hex << IBLtot[0] << "   IBLtot[1] = 0x" << IBLtot[1] << std::dec << endreq;
#endif
#ifdef PLOTS
		std::cout << "[Plot2]: " << robId << " " << eta_i << " " << phi_i << " " << IBLtot[0] << std::endl; 
		std::cout << "[Plot3]: " << robId << " " << col[i] << " " << row[i] << " " << IBLtot[0] << std::endl; 
#endif
		m_coll->push_back(new RDO(pixelId, IBLtot[0], mBCID,mLVL1ID,mLVL1A)); // insert the first part of the ToT info in the Collection
#ifdef PIXEL_DEBUG
		msg(MSG::VERBOSE) << "Collection filled with pixelId: " << pixelId << " TOT = 0x" << std::hex << IBLtot[0] << std::dec << " mBCID = " << mBCID << " mLVL1ID = " << mLVL1ID << "  mLVL1A = " << mLVL1A << endreq;
#endif
        if ((IBLtot[1] != 0x0) && (IBLtot[1] != 0xF)) {  // Consider both 0x0 and 0xF to indicate no hit
		  if ((row[i] + 1) > 336) { // FIXME: hardcoded number - but it should still be ok, because it's a feature of the FE-I4!
            // this should never happen. If row[i] == 336, (row[i]+1) == 337. This row does not exist, so the TOT(337) should always be 0 (== no hit)
            msg(MSG::WARNING) << "Illegal IBL row number for second ToT field: ROB = 0x" << std::hex << robId  
            << ", hit word 0x"<< rawDataWord << " decodes to row = " << std::dec << row[i]+1 << " col = " << col[i] 
            << " (ToT1 = 0x" << std::hex << IBLtot[0] << " ToT2 = 0x" << IBLtot[1] << ")" << endreq;
		    m_errors->addInvalidIdentifier();
		    continue;
		  }
		  else {
		    pixelId = m_pixelCabling ->getPixelIdfromHash (offlineIdHash, mFE, row[i] + 1, col[i]);
#ifdef PLOTS
		    std::cout << "[VAL] " << std::hex << pixelId << " 0x" << robId << " 0x" << onlineId //<< " " << offlineIdHash
			      << std::dec << " " << m_pixel_id->eta_module(pixelId) << " " << m_pixel_id->phi_module(pixelId)
			      << " " << m_pixel_id->eta_index(pixelId) << " " << m_pixel_id->phi_index(pixelId) << std::endl;
#endif
#ifdef PIXEL_DEBUG		    
		    int eta_i = m_pixel_id->eta_index(pixelId);
		    int phi_i = m_pixel_id->phi_index(pixelId);
		    int eta_m = m_pixel_id->eta_module(pixelId);
		    int phi_m = m_pixel_id->phi_module(pixelId);
		    int phi_i_max = m_pixel_id->phi_index_max(pixelId);
		    int eta_i_max = m_pixel_id->eta_index_max(pixelId);

		    msg(MSG::VERBOSE) << "PixelId = " << pixelId << endreq;
		    msg(MSG::VERBOSE) << "          eta_i: " << eta_i << ", phi_i: " << phi_i << ",  eta_m: " <<  eta_m << ", phi_m: " << phi_m  << ", eta_i_max: " << eta_i_max << ", phi_i_max: " << phi_i_max << endreq;
#endif
		    if (pixelId == invalidPixelId) {
		      msg(MSG::WARNING) << "Illegal pixelId" << endreq;
		      m_errors->addInvalidIdentifier();
		      continue;
		    } // end if "if (pixelId == invalidPixelId)" for the second pixel included in the TOT information
#ifdef PLOTS
		    std::cout << "[Plot2]: " << robId << " " << eta_i << " " << phi_i << " " << IBLtot[1] << std::endl; 
		    std::cout << "[Plot3]: " << robId << " " << col[i] << " " << row[i]+1 << " " << IBLtot[1] << std::endl; 
#endif
		    m_coll->push_back(new RDO(pixelId, IBLtot[1], mBCID, mLVL1ID, mLVL1A)); 
#ifdef PIXEL_DEBUG
		    msg(MSG::VERBOSE) << "Collection filled with pixelId: " << pixelId << " TOT = 0x" << std::hex << IBLtot[1] << std::dec << " mBCID = " << mBCID << " mLVL1ID = " << mLVL1ID << "  mLVL1A = " << mLVL1A << endreq;
#endif
		  }
		}
	      } // end of the if that checks that Row and Column of the IBL have a value > 0 and smaller than the maximum of the FE-I4.
	      else {
		msg(MSG::WARNING) << "Illegal IBL/DBM row and/or column number associated with a hit" << endreq;
		m_errors->addInvalidIdentifier();
		continue;
	      }
	    } // end of the else from the condition "(tot[i] & 0xF0) == 0xF", corresponding to an invalid tot sent by the ROD 
	  } // end of the loop over the hit information coming from the IBL words (condensed or not)
	} // end IBL/DBM words pushed into Collection
      
	else { // it is a Pixel hit word to be saved into the Collection
	  pixelId = m_pixelCabling->getPixelIdfromHash(offlineIdHash, mFE, mRow, mColumn);
#ifdef PLOTS
	  std::cout << "[VAL] " << std::hex << pixelId << " 0x" << robId << " 0x" << onlineId //<< " " << offlineIdHash
		    << std::dec << " " << m_pixel_id->eta_module(pixelId) << " " << m_pixel_id->phi_module(pixelId)
		    << " " << m_pixel_id->eta_index(pixelId) << " " << m_pixel_id->phi_index(pixelId) << std::endl;
#endif
	  if (pixelId == invalidPixelId)
	    {
	      msg(MSG::WARNING) << "Illegal pixelId" << endreq;
	      m_errors->addInvalidIdentifier();
	      continue;
	    }
	  // Now the Collection is there for sure. Create RDO and push it into Collection.
#ifdef PLOTS
	  int eta_i = m_pixel_id->eta_index(pixelId);
	  int phi_i = m_pixel_id->phi_index(pixelId);
	  std::cout << "[PlotB]: " << robId << " " << eta_i << " " << phi_i << " " << mToT << std::endl; 
	  std::cout << "[PlotC]: " << robId << " " << mColumn << " " << mRow << " " << mToT << std::endl; 
#endif 
	  m_coll->push_back(new RDO(pixelId, mToT, mBCID,mLVL1ID,mLVL1A));
#ifdef PIXEL_DEBUG
	  msg(MSG::VERBOSE) << "Collection filled with pixelId: " << pixelId << " TOT = 0x" << std::hex << mToT << std::dec << " mBCID = " << mBCID << " mLVL1ID = " << mLVL1ID << "  mLVL1A = " << mLVL1A << endreq;
#endif
	} // end Pixel words pushed into Collection
      } // end of the if (link_start)	   
      
      else { // no header found before hit -> error

	msg(MSG::WARNING) << "unexpected hit dataword found - data corruption - hit ignored" << endreq;   // unexpected hit error message
	m_errors->addDecodingError();
	continue;
      }
      break;





      //-------------------------------------------------------------------------------------------- TRAILER WORD FOUND     
    case PRB_LINKTRAILER:  // link (module) trailer found
      if (link_start) {   // if header found before trailer -> expected
#ifdef PIXEL_DEBUG
	msg(MSG::DEBUG) << "link trailer found" << endreq;   // trailer found debug message
#endif
      }
      else {   // no header found before trailer -> error
	msg(MSG::WARNING) << "unexpected trailer found: 0x" << std::hex << rawDataWord << ", data corruption - ROBID: 0x" << std::hex << robId << std::dec << " link: " << mLink << endreq;
	m_errors->addDecodingError();
	continue;
      }
      previous_offlineIdHash = offlineIdHash;  // save offlineId for next header;
      link_start = false;   // resetting link (module) header found flag
      are_4condensed_words = false;
      
      // Trailer decoding and error handling
      if (m_is_ibl_module || m_is_dbm_module) { // decode IBL/DBM Trailer word: 010nnnnnECPplbzhvMMMMMMMMMMxxxxx
#ifdef PIXEL_DEBUG
	msg(MSG::VERBOSE) << "Decoding IBL/DBM trailer word: 0x" << std::hex << rawDataWord << std::dec << endreq;
#endif
#ifdef PLOTS
	std::cout << "[Plot1]:0x " << std::hex << rawDataWord << std::dec << std::endl;
	std::cout << "[Plot1]:(dec) " << rawDataWord << std::endl;
#endif
	//mSkippedTrigTrailer = decodeSkippedTrigTrailer_IBL(rawDataWord); // decode skipped trigger counter bits => M  -- temporarily removed 
	trailererror = decodeTrailerErrors_IBL(rawDataWord); // => EWPplbzhv // taking all errors together.

	// TO BE CHECKED, as Data format is not clear yet (Franconi, 08.07.2014)
	if (trailererror & (1 << 8)) // time out error bit => E
	  m_errors->addTimeOutError();
	//	if (trailererror & (1 << 7)) // condensed mode bit => W
	//	if (trailererror & (1 << 6)) // link masked by PPC => P
	if (trailererror & (1 << 5)) // preamble error bit => p
	  m_errors->addPreambleError();
	if (trailererror & (1 << 4)) // LVL1 error bit => l
	  m_errors->addLVL1IDError();
	if (trailererror & (1 << 3)) // BCID error bit => b
	  m_errors->addBCIDError();
	if (trailererror & (1 << 2)) // trailer error bit => z
	  m_errors->addTrailerError();
	//	if (trailererror & (1 << 1)) // header/trailer limit error=> h
	//	if (trailererror & (1 << 0)) // data overflow error=> v



	//At least temporarily removed because the data format is not clear (Franconi, 17.06.2014)
	//	linkNum_IBLword = decodeLinkNumTrailer_IBL(rawDataWord); // => n 
	//	mBitFlips = decodeTrailerBitflips_IBL(rawDataWord);
	//	if (mBitFlips != 0)
	//	  msg(MSG::WARNING) << "The IBL trailer word has bitflips" << endreq;
	//	if (linkNum_IBLword != linkNum_IBLheader) 
	//	  msg(MSG::WARNING) << "The IBL header and trailer words do not contain the same link number information (bits 24-28)" << endreq;

	//FIXME: errors needed to be used in a clever way also for IBL!!!

      }

      else { // decode Pixel trailer word
#ifdef PIXEL_DEBUG
	msg(MSG::VERBOSE) << "Decoding Pixel trailer word: 0x" << std::hex << rawDataWord << std::dec << endreq;
#endif
#ifdef PLOTS
	std::cout << "[PlotA]:0x " << std::hex << rawDataWord << std::dec << std::endl;
	std::cout << "[PlotA]:(dec) " << rawDataWord << std::endl;
#endif
	trailererror = decodeTrailerErrors(rawDataWord);   // creating link (module) trailer error variable
	//mBitFlips = decodeTrailerBitflips(rawDataWord);   -- temporarily removed
	
	if (trailererror != 0) {
	  sc = StatusCode ::RECOVERABLE;
	  errorcode = errorcode | trailererror; //encode error as HHHHMMMMMMMMFFFFFFFFTTTT for header, flagword, trailer errors
	  //for now just sum all trailer errors
	  if (trailererror & (1 << 3))
	    m_errors->addTrailerError();
	  if (trailererror & (1 << 2))
	    m_errors->addTrailerError();
	  if (trailererror & (1 << 1))
	    m_errors->addTrailerError();
	  if (trailererror & (1 << 0))
	    m_errors->addTrailerError();
	}
      }
      if ( offlineIdHash != 0xffffffff ) // now write the error word to the service
	m_errors->setModuleErrors(offlineIdHash, errorcode);

      break;
      //-------------------------------------------------------------------------------------------- FE ERROR TYPE 2 WORD FOUND
    case PRB_FEERROR2 :    // type 2 flag found
      are_4condensed_words = false;
      if (m_is_ibl_module || m_is_dbm_module)
	{ // IBL flag found
#ifdef PIXEL_DEBUG
	  msg(MSG::VERBOSE) << "Decoding IBL/DBM FEflag word: 0x" << std::hex << rawDataWord << std::dec << endreq;
#endif

	  // Temporarily removed, as the data format is not clear yet (Franconi, 08.07.2014)
	  // linkNum_IBLword = decodeLinkNumFEFlag_IBL(rawDataWord);
	  // fe_IBLword = extractFefromLinkNum(linkNum_IBLword);
	  // sLinkWord = extractSLinkfromLinkNum(linkNum_IBLword); 
	  // if ((sLinkWord != sLinkHeader) || (fe_IBLword != fe_IBLheader))
	  //   msg(MSG::WARNING) << "The IBL header and FE flag words do not contain the same link number information" << endreq;

	  serviceCodeCounter = decodeServiceCodeCounter_IBL(rawDataWord); // frequency of the serviceCode (with the exceptions of serviceCode = 14,15 or 16)
	  serviceCode = decodeServiceCode_IBL(rawDataWord); // is a code. the corresponding meaning is listed in the table in the FE-I4 manual, pag. 105

	  // Temporarily removed, as the data format is not clear yet (Franconi, 08.07.2014)
	  //	  mBitFlips = decodeFEFlagBitFlips_IBL(rawDataWord);
	  //	  if (mBitFlips != 0)
	  //	    msg(MSG::WARNING) << "IBL Fe Flag error word has some bitflips: the word is 0x" << std::hex << rawDataWord << std::dec  << endreq;

	  // Treatment of the FE flag serviceCode and serviceCodeCounter
	  treatmentFEFlagInfo(serviceCode, serviceCodeCounter);
	}
      else
	{ // Pixel type2 flag found 
#ifdef PIXEL_DEBUG
	  msg(MSG::VERBOSE) << "Decoding Pixel FEflag word: 0x" << std::hex << rawDataWord << std::dec << endreq;      
#endif
	  FEFlags = decodeFEFlags2(rawDataWord);   // get FE flags
	  MCCFlags = decodeMCCFlags(rawDataWord);   // get MCC flags
	
	  FEFlags = FEFlags & 0xF3; // mask out the parity bits, they don't work
	  if ((MCCFlags | FEFlags) != 0) {
	    sc = StatusCode::RECOVERABLE;
	    errorcode = errorcode | (MCCFlags << 12) | (FEFlags << 4); //encode error as HHHHMMMMMMMMFFFFFFFFTTTT for header, flagword, trailer errors
	    //for now just sum all flagged errors
	    if (MCCFlags & (1 << 7))
	      m_errors->addFlaggedError();
	    if (MCCFlags & (1 << 6))
	      m_errors->addFlaggedError();
	    if (MCCFlags & (1 << 5))
	      m_errors->addFlaggedError();
	    if (MCCFlags & (1 << 4))
	      m_errors->addFlaggedError();
	    if (MCCFlags & (1 << 3))
	      m_errors->addFlaggedError();
	    if (MCCFlags & (1 << 2))
	      m_errors->addFlaggedError();
	    if (MCCFlags & (1 << 1))
	      m_errors->addFlaggedError();
	    if (MCCFlags & (1 << 0))
	      m_errors->addFlaggedError();
	    if (FEFlags & (1 << 7))
	      m_errors->addFlaggedError();
	    if (FEFlags & (1 << 6))
	      m_errors->addFlaggedError();
	    if (FEFlags & (1 << 5))
	      m_errors->addFlaggedError();
	    if (FEFlags & (1 << 4))
	      m_errors->addFlaggedError();
	    if (FEFlags & (1 << 3))
	      m_errors->addFlaggedError();
	    if (FEFlags & (1 << 2))
	      m_errors->addFlaggedError();
	    if (FEFlags & (1 << 1))
	      m_errors->addFlaggedError();
	    if (FEFlags & (1 << 0))
	      m_errors->addFlaggedError();
	  
	  } else {
	    m_errors->addDisabledFEError();
	    m_errors->addBadFE(offlineIdHash,(rawDataWord & 0x0F000000) >> 24);
#ifdef PIXEL_DEBUG
	    msg(MSG::DEBUG) << "Disabled Pixel chip " << ((rawDataWord & 0x0F000000) >> 24) << endreq; 
#endif
	  }
	}
      break;

      //-------------------------------------------------------------------------------------------- RAWDATA WORD FOUND
    case PRB_RAWDATA : // add treatment for raw data word here!
      are_4condensed_words = false;
      
      break;
      //-------------------------------------------------------------------------------------------- UNKNOWN WORD
    default:
      msg(MSG::WARNING) << "unknown word type found, " << std::hex << rawDataWord << std::dec << " ignoring" << endreq;
      m_errors->addDecodingError();
    } // end of switch
  }   // end of loop over ROD
   
  if (sc == StatusCode::RECOVERABLE) {
    
    if (errorcode == (3 << 20) ){  // Fix for M8, this error always occurs, masked out REMOVE FIXME !!
      masked_errors++;
      return StatusCode::SUCCESS;
    }
    
#ifdef PIXEL_DEBUG
    msg(MSG::DEBUG) << "Recoverable errors found in PixelRodDecoder, errorcode: " << errorcode << endreq;
#endif
    char error[100];
    if (errorcode == (1 << 20) ){  // only BCID error found
      sprintf(error,"BCID mismatch found, errorcode: %x",errorcode);
      sc = STATUSCODE(StatusCode::RECOVERABLE,IssueSeverity::DEBUG,error);
    }
    else{
      sprintf(error,"ErrorCode: %x, Pixel data likely corrupt",errorcode);
      sc = STATUSCODE(StatusCode::RECOVERABLE,IssueSeverity::DEBUG,error);
    }
  }
  return sc;
}



// ****************************************************************************************************** DECODING FUNCTIONS

////////////////////////
// extract the FE (w.r.t. SLink) from the Link Number (nnnnn), present in the IBL header, long hit, fe flag error and trailer words
////////////////////////
uint32_t PixelRodDecoder::extractFefromLinkNum(const uint32_t linkNum)
{
  return linkNum & 0x7; 
}

////////////////////////
// extract the FE (w.r.t. SLink) from the Link Number (nnnnn), present in the IBL header, long hit, fe flag error and trailer words
////////////////////////
uint32_t PixelRodDecoder::extractSLinkfromLinkNum(const uint32_t linkNum)
{
  return (linkNum >> 3) & 0x3; 
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ HEADER BITS

////////////////////////
// decode IBL BCID (bits 0-9 of IBL module header word)
////////////////////////
//template <class ROBData> 
uint32_t PixelRodDecoder::decodeBCID_IBL(const uint32_t word)
{
  return ((word >> PRB_BCIDskip_IBL) & PRB_BCIDmask_IBL);
}

////////////////////////
// decode IBL LVL1ID (bits 10-22 of IBL module header word)
////////////////////////
//template <class ROBData> 
uint32_t PixelRodDecoder::decodeL1ID_IBL(const uint32_t word)
{
  return ((word >> PRB_L1IDskip_IBL) & PRB_L1IDmask_IBL);
}

////////////////////////
// decode IBL FeI4B flag bit (bit 23 of IBL module header word)
////////////////////////
//template <class ROBData> 
uint32_t PixelRodDecoder::decodeFeI4Bflag_IBL(const uint32_t word)
{
  return ((word >> PRB_FeI4BFLAGskip_IBL) & PRB_FeI4BFLAGmask_IBL);
}

////////////////////////
// decode IBL module link number  (bits 24-28 of IBL module header word)
////////////////////////
//template <class ROBData> 
uint32_t PixelRodDecoder::decodeModule_IBL(const uint32_t word)
{
  return ((word >> PRB_MODULEskip_IBL) & PRB_MODULEmask_IBL);
}

////////////////////////
// decode BCID (bits 0-7 of module Pixel header word)
////////////////////////
//template <class ROBData> 
uint32_t PixelRodDecoder::decodeBCID(const uint32_t word)
{
  return ((word >> PRB_BCIDskip) & PRB_BCIDmask);
}

////////////////////////
// decode LVL1ID (bits 8-11 of module Pixel header word)
////////////////////////
//template <class ROBData> 
uint32_t PixelRodDecoder::decodeL1ID(const uint32_t word)
{
  return ((word >> PRB_L1IDskip) & PRB_L1IDmask);
}

////////////////////////
// decode LVL1ID skipped (bits 12-15 of module Pixel header word)
////////////////////////
//template <class ROBData>
uint32_t PixelRodDecoder::decodeL1IDskip(const uint32_t word)
{
  return ((word >> PRB_L1IDSKIPskip) & PRB_L1IDSKIPmask);
}

////////////////////////
// decode module link number (bits 16-22 of module Pixel header word)
////////////////////////
//template <class ROBData> 
uint32_t PixelRodDecoder::decodeModule(const uint32_t word)
{
  return ((word >> PRB_MODULEskip) & PRB_MODULEmask);
}

////////////////////////
// decode header errors (bits 25-28 of module Pixel header word)
////////////////////////
//template <class ROBData> 
uint32_t PixelRodDecoder::decodeHeaderErrors(const uint32_t word)
{
  return ((word >> PRB_HEADERERRORSskip) & PRB_HEADERERRORSmask);
}

/*// Not used at the moment
////////////////////////
// look for bitflips in header (bits 23-24 of module Pixel  header word)
////////////////////////
//template <class ROBData> 
uint32_t PixelRodDecoder::decodeHeaderBitflips(const uint32_t word)
{
uint32_t flipword = ((word >> PRB_HEADERBITFLIPskip) & PRB_HEADERBITFLIPmask);
uint32_t errorcount =0;
for (int i=0; i<=1; i++){    // this calculates the checksum of the flipword = the number of bitflips
if (flipword & 1)
errorcount++;
flipword >>= 1;
}
return errorcount;     // returns the number of errors
}
*/



// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ HIT WORD BITS

////////////////////////
// decode hit row number (bits 0-7 of Pixel hit word, bits 0-8 of IBL hit word)
////////////////////////
//template <class ROBData> 
uint32_t PixelRodDecoder::decodeRow(const uint32_t word)
{
  if(m_is_ibl_module || m_is_dbm_module) {
    return ((word >> PRB_ROWskip_IBL) & PRB_ROWmask_IBL);
  }
  else {
    return ((word >> PRB_ROWskip) & PRB_ROWmask);
  }
}

////////////////////////
// decode hit column number (bits 8-12 of Pixel hit word, bits 9-15 of IBL hit word)
////////////////////////
//template <class ROBData> 
uint32_t PixelRodDecoder::decodeColumn(const uint32_t word)
{
  if(m_is_ibl_module || m_is_dbm_module) {
    return ((word >> PRB_COLUMNskip_IBL) & PRB_COLUMNmask_IBL);
  }
  else {
    return ((word >> PRB_COLUMNskip) & PRB_COLUMNmask);
  }
}

////////////////////////
// decode hit TimeOverThreshold value (bits 16-23 of Pixel hit word, bits 16-23 of IBL hit word)
////////////////////////
//template <class ROBData> 
uint32_t PixelRodDecoder::decodeToT(const uint32_t word)
{
  if (m_is_ibl_module || m_is_dbm_module)
    { // IBL Tot => 8 bits to be divided into 2 Tot info, each of which is 4 bits
      uint32_t Tot8Bits = ((word >> PRB_TOTskip_IBL) & PRB_TOTmask_IBL); // this Tot information returns a 8-bit word. In the case of the IBL, it will be splitted into Tot(1) and Tot(2), corresponding to pixels (col,row) and (col, row+1)
      return Tot8Bits;
    }
  else 
    { // Pixel Tot
      return ((word >> PRB_TOTskip) & PRB_TOTmask);
    }
}
/* // At least temporarily not used, because IBL data format is not clear (Franconi, 17.06.2014)
////////////////////////
// decode Link number in the IBL not-condensed hit word (bits 24-26 of IBL hit word)
////////////////////////
//template <class ROBData> 
uint32_t PixelRodDecoder::decodeLinkNumHit_IBL(const uint32_t word)
{
  return ((word >> PRB_LINKNUMHITskip_IBL) & PRB_LINKNUMHITmask_IBL);
}
*/
////////////////////////
// decode module FE number (bits 24-27 of Pixel hit word) 
////////////////////////
//template <class ROBData> 
uint32_t PixelRodDecoder::decodeFE(const uint32_t word)
{
  return ((word >> PRB_FEskip) & PRB_FEmask);
}

/*
// not used in the code
////////////////////////
// look for bitflips in hitword (bits 13-15 and 28 of Pixel module hit word)
////////////////////////
//template <class ROBData> 
uint32_t PixelRodDecoder::decodeHitBitflips(const uint32_t word)
{
uint32_t flipword = ((word >> PRB_HITBITFLIPskip) & PRB_HITBITFLIPmask);
uint32_t errorcount = 0;
for (int i=0; i<=2; i++){    // this calculates the checksum of the flipword = the number of bitflips
if (flipword & 1)
errorcount++;
flipword >>= 1;}
flipword >>= 12;
if (flipword & 1)
errorcount++;
return errorcount;     // returns the number of errors
}
*/



// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ TRAILER BITS

////////////////////////
// decode IBL "M" in the trailer (bits 5-14 of IBL module trailer word)
////////////////////////
//template <class ROBData> 
uint32_t PixelRodDecoder::decodeSkippedTrigTrailer_IBL(const uint32_t word)
{
  return ((word >> PRB_SKIPPEDTRIGGERTRAILERskip_IBL) & PRB_SKIPPEDTRIGGERTRAILERmask_IBL);
}

////////////////////////
// decode IBL trailer errors (bits 15-23 of IBL module trailer word)
////////////////////////
//template <class ROBData> 
uint32_t PixelRodDecoder::decodeTrailerErrors_IBL(const uint32_t word)
{
  return ((word >> PRB_TRAILERERRORSskip_IBL) & PRB_TRAILERERRORSmask_IBL);
}


/* // At least temporarily not used, because IBL data format is not clear (Franconi, 17.06.2014)
////////////////////////
// decode Link Number in the IBL Trailer (bits 24-26 of IBL module trailer word)
////////////////////////
//template <class ROBData>
uint32_t PixelRodDecoder::decodeLinkNumTrailer_IBL(const uint32_t word)
{
  return ((word >> PRB_LINKNUMTRAILERskip_IBL) & PRB_LINKNUMTRAILERmask_IBL);
}
*/



/*
////////////////////////
// decode Time Out Error Bit in the IBL trailer (bit 23 of IBL module trailer word)
////////////////////////
//template <class ROBData>
uint32_t PixelRodDecoder::decodeTimeOutErrorBit_IBL(const uint32_t word)
{
return ((word >> PRB_TIMEOUTERRORskip_IBL) & PRB_TIMEOUTERRORmask_IBL);
}
*/


////////////////////////
// decode Condensed Mode Bit in the IBL trailer (bit 22 of IBL module trailer word)
////////////////////////
//template <class ROBData>
uint32_t PixelRodDecoder::decodeCondensedModeBit_IBL(const uint32_t word)
{
return ((word >> PRB_CONDENSEDMODEskip_IBL) & PRB_CONDENSEDMODEmask_IBL);
}

/*
////////////////////////
// decode LVL1ID skipped IBL (link masked by the PPC) (bit 21 of IBL module trailer word)
////////////////////////
//template <class ROBData>
uint32_t PixelRodDecoder::decodeL1IDskipTrailer_IBL(const uint32_t word)
{
return ((word >> PRB_L1IDSKIPTRAILERskip_IBL) & PRB_L1IDSKIPTRAILERmask_IBL);
}
*/

////////////////////////
// decode trailer errors (bits 26-28 of Pixel module trailer word)
////////////////////////
//template <class ROBData> 
uint32_t PixelRodDecoder::decodeTrailerErrors(const uint32_t word)
{
  return ((word >> PRB_TRAILERERRORSskip) & PRB_TRAILERERRORSmask);
}

////////////////////////
// look for bitflips in trailer word (bits 0-25 of Pixel module trailer word)
////////////////////////
//template <class ROBData> 
uint32_t PixelRodDecoder::decodeTrailerBitflips(const uint32_t word)
{
  uint32_t flipword = ((word >> PRB_TRAILERBITFLIPskip) & PRB_TRAILERBITFLIPmask);
  uint32_t errorcount = 0;
  if (flipword != 0) {
    for (int i=0; i<=25; i++){    // this calculates the checksum of the flipword = the number of bitflips
      if (flipword & 1)
	errorcount++;
      flipword >>= 1;
    }
  }
  return errorcount;     // returns the number of errors
}



/* // At least temporarily not used, because IBL data format is not clear (Franconi, 17.06.2014)
////////////////////////
// look for bitflips in IBL trailer word (bits 0-4 of IBL module trailer word)
////////////////////////
//template <class ROBData> 
uint32_t PixelRodDecoder::decodeTrailerBitflips_IBL(const uint32_t word)
{
  uint32_t flipword = ((word >> PRB_BITFLIPSTRAILERskip_IBL) & PRB_BITFLIPSTRAILERmask_IBL);
  uint32_t errorcount = 0;
  for (int i = 0; i <= 5; i++) {    // this calculates the checksum of the flipword = the number of bitflips
    if (flipword & 1){
      errorcount++;}
    flipword >>= 1;
  }
  return errorcount;     // returns the number of errors
}
*/


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ FLAG WORDS BITS

////////////////////////
// decodeServiceCodeCounter (bits 0-9 of IBL flag word)
////////////////////////
uint32_t PixelRodDecoder::decodeServiceCodeCounter_IBL(const uint32_t word)
{
  return((word >> PRB_SERVICECODECOUNTERskip_IBL) & PRB_SERVICECODECOUNTERmask_IBL);
}

////////////////////////
// decodeServiceCode (bits 17-22 of IBL flag word)
////////////////////////
uint32_t PixelRodDecoder::decodeServiceCode_IBL(const uint32_t word)
{
  return((word >> PRB_SERVICECODEskip_IBL) & PRB_SERVICECODEmask_IBL);
}



/* // At least temporarily not used, because IBL data format is not clear (Franconi, 17.06.2014)
////////////////////////
// decodeLinkNumFEFlag (bits 25-27 of IBL flag word)
////////////////////////
uint32_t PixelRodDecoder::decodeLinkNumFEFlag_IBL(const uint32_t word)
{
  return((word >> PRB_LINKNUMBERFEFLAGskip_IBL) & PRB_LINKNUMBERFEFLAGmask_IBL);
}
*/

/* // At least temporarily not used, because IBL data format is not clear (Franconi, 17.06.2014)
////////////////////////
// decodeFEFlagBitFlips (bits 10-16 and 23 of IBL flag word)
////////////////////////
uint32_t PixelRodDecoder::decodeFEFlagBitFlips_IBL(const uint32_t word)
{
  uint32_t flipword = ((word >> PRB_BITFLIPSFEFLAGskip_IBL) & PRB_BITFLIPSFEFLAGmask_IBL);
  uint32_t errorcount = 0;
  if (flipword != 0x0) {
    for (int i = 0; i <= 14; i++) {    // this calculates the checksum of the flipword = the number of bitflips
      if (flipword & 1)
	errorcount++;
      flipword >>= 1;
    }
  }
  return errorcount;     // returns the number of errors
}
*/

////////////////////////
// decode FE flag errors (bits 0-7 of Pixel flag type 2 word)
////////////////////////
//template <class ROBData>
uint32_t PixelRodDecoder::decodeFEFlags2(const uint32_t word)
{
  return ((word >> PRB_FEFlagskip2) & PRB_FEFlagmask2);
}
 
////////////////////////
// decode MCC flag errors (bits 8-12 of Pixel flag type 2 word)
////////////////////////
//template <class ROBData>
uint32_t PixelRodDecoder::decodeMCCFlags(const uint32_t word)
{
  return ((word >> PRB_MCCFlagskip) & PRB_MCCFlagmask);
}

/* // not used in the code
////////////////////////
// look for bitflips in FEFlag2 (bits 21-23 of Pixel module FEFlags word 2)
////////////////////////
//template <class ROBData> 
uint32_t PixelRodDecoder::decodeFEFlags2Bitflips(const uint32_t word)
{
uint32_t flipword = ((word >> PRB_FEFLAG2BITFLIPskip) & PRB_FEFLAG2BITFLIPmask);
uint32_t errorcount =0;
for (int i=0; i<=2; i++){    // this calculates the checksum of the flipword = the number of bitflips
if (flipword & 1)
errorcount++;
flipword >>= 1;}
return errorcount;     // returns the number of errors
}
*/


/*
////////////////////////
// decode FE flag errors (bits 0-3 of Pixel flag type 1 word)
////////////////////////
//template <class ROBData>
uint32_t PixelRodDecoder::decodeFEFlags1(const uint32_t word)
{
return ((word >> PRB_FEFlagskip1) & PRB_FEFlagmask1);
}

////////////////////////
// decode second FE number (bits 4-7 of Pixel flag type 1 word)
////////////////////////
//template <class ROBData>
uint32_t PixelRodDecoder::decodeFE2(const uint32_t word)
{
return ((word >> PRB_FE2skip) & PRB_FE2mask);
}
*/
/*
////////////////////////
// look for bitflips in FEFlag1 (bits 13-23 of Pixel module FEFlags word 1)
////////////////////////
//template <class ROBData> 
uint32_t PixelRodDecoder::decodeFEFlags1Bitflips(const uint32_t word)
{
uint32_t flipword = ((word >> PRB_FEFLAG1BITFLIPskip) & PRB_FEFLAG1BITFLIPmask);
uint32_t errorcount =0;
for (int i=0; i<=10; i++){    // this calculates the checksum of the flipword = the number of bitflips
if (flipword & 1)
errorcount++;
flipword >>= 1;}
return errorcount;     // returns the number of errors
}
*/




////////////////////////
// determine module word type
////////////////////////
//template <class ROBData> 
uint32_t PixelRodDecoder::getDataType(unsigned int rawDataWord, bool link_start) 
{
  if (link_start) { // there was a header, so if a timeout is found it's really a timeout and not an empty header, same for raw data
    if ((rawDataWord & PRB_DATAMASK) == PRB_RAWDATA ) return PRB_RAWDATA;   // module flag word found
    if (rawDataWord == PRB_TIMEOUT) return PRB_TIMEOUT;   // time out word found
  }   
  if (((rawDataWord & PRB_DATAMASK) == PRB_LINKHEADER) /*&& ((rawDataWord & PRB_LINKHEADEREMPTY) == 0 )*/) return PRB_LINKHEADER;   // module header word found
  //  if (((rawDataWord & PRB_FEERRORMASK) == PRB_FEERROR1) && ((rawDataWord & PRB_FEERROR1CHECK) == PRB_FEERROR1CHECKRESULT )) return PRB_FEERROR1;   // type 1 word found Pixel -- not used (according to Paolo Morettini's bit format table AND not really used in this code either)
  if (((rawDataWord & PRB_FEERRORMASK) == PRB_FEERROR2) && ((rawDataWord & PRB_FEERROR2CHECK) == PRB_FEERROR2CHECKRESULT )) return PRB_FEERROR2;   // type 2 word found Pixel
  if (((rawDataWord & PRB_DATAMASK) == PRB_FEERROR_IBL)) return PRB_FEERROR2;   // type word found IBL
  //  if (((rawDataWord & PRB_FEERRORMASK) == PRB_FEERROR1) && ((rawDataWord & PRB_FEERROR1CHECK_IBL) == PRB_FEERROR1)) return PRB_FEERROR1;   // type short word found IBL
  
  //  if (m_is_ibl_module) {
  if ((rawDataWord & PRB_HITMASK) == PRB_DATAWORD) return PRB_DATAWORD;   // module hit word found
  //  }
  //  else {
  //    if (((rawDataWord & PRB_DATAMASK) == PRB_DATAWORD) && ((rawDataWord & PRB_DATAWORDEMPTY) == 0 )) return PRB_DATAWORD;   // module hit word found
  //  }
  //  if (((rawDataWord & PRB_DATAMASK) == PRB_LINKTRAILER) && (((rawDataWord & PRB_LINKTRAILEREMPTY) == 0) || ((rawDataWord & PRB_LINKTRAILEREMPTY_IBL) == 0) ) ) return PRB_LINKTRAILER;   // module trailer word found
  if ((rawDataWord & PRB_DATAMASK) == PRB_LINKTRAILER) return PRB_LINKTRAILER;   // module trailer word found
  return PRB_UNKNOWNWORD;   // unknown word found
  
}


////////////////////////
// add a ROD Errors (truncation)
////////////////////////

void PixelRodDecoder::addRODError(uint32_t robid, uint32_t robStatus) {
  std::vector<IdentifierHash> idHashes;
  m_pixelCabling->getOfflineList(idHashes,robid);
  std::vector<IdentifierHash>::iterator hashIt = idHashes.begin();
  std::vector<IdentifierHash>::iterator hashEnd = idHashes.end();
  for (; hashIt != hashEnd; ++hashIt) {
    m_errors->addRODError();
    m_errors->setModuleRODErrors(*hashIt,robStatus);
  }
  return;
}

void PixelRodDecoder::addToFlaggedErrorCounter(const unsigned int & serviceCodeCounter) {
  for (unsigned int i(0); i < serviceCodeCounter; ++i)
    m_errors->addFlaggedError();
}

void PixelRodDecoder::treatmentFEFlagInfo(unsigned int serviceCode, unsigned int serviceCodeCounter) {
  switch (serviceCode) {

  case 0: // BCID counter error, from EOCHL
    ATH_MSG_DEBUG("BCID counter error (retrieved in IBL FE Flag word). Adding BCID errors");
    for (unsigned int i(0); i < serviceCodeCounter; ++i)
      m_errors->addBCIDError();
    break;

  case 1: // Hamming code error in word 0, from EOCHL
    ATH_MSG_DEBUG("Hamming code error in word 0 (retrieved in IBL FE Flag word). Adding to Flagged errors");
    addToFlaggedErrorCounter(serviceCodeCounter);
    break;

  case 2: // Hamming code error in word 1, from EOCHL
    ATH_MSG_DEBUG("Hamming code error in word 1 (retrieved in IBL FE Flag word). Adding to Flagged errors");
    addToFlaggedErrorCounter(serviceCodeCounter);
    break;

  case 3: // Hamming code error in word 2, from EOCHL
    ATH_MSG_DEBUG("Hamming code error in word 2 (retrieved in IBL FE Flag word). Adding to Flagged errors");
    addToFlaggedErrorCounter(serviceCodeCounter);
    break;

  case 4: // L1_in counter error, from EOCHL
    ATH_MSG_DEBUG("L1_in counter error (retrieved in IBL FE Flag word). Adding to Flagged errors");
    addToFlaggedErrorCounter(serviceCodeCounter);
    break;

  case 5: // L1 request counter error from EOCHL
    ATH_MSG_DEBUG("L1 request counter error (retrieved in IBL FE Flag word). Adding to Flagged errors");
    addToFlaggedErrorCounter(serviceCodeCounter);
    break;

  case 6: // L1 register error from EOCHL
    ATH_MSG_DEBUG("L1 register error (retrieved in IBL FE Flag word). Adding to Flagged errors");
    addToFlaggedErrorCounter(serviceCodeCounter);
    break;

  case 7: // L1 Trigger ID error from EOCHL
    ATH_MSG_DEBUG("L1 Trigger ID error (retrieved in IBL FE Flag word). Adding to Flagged errors");
    addToFlaggedErrorCounter(serviceCodeCounter);
    break;

  case 8: // readout processor error from EOCHL
    ATH_MSG_DEBUG("Readout processor error (retrieved in IBL FE Flag word). Adding to Flagged errors");
    addToFlaggedErrorCounter(serviceCodeCounter);
    break;

  case 9: // Fifo_Full flag pulsed from EOCHL
    ATH_MSG_DEBUG("Fifo_Full flag pulsed (retrieved in IBL FE Flag word). Adding to Flagged errors");
    addToFlaggedErrorCounter(serviceCodeCounter);
    break;

  case 10: // HitOr bus pulsed from PixelArray
    ATH_MSG_DEBUG("HitOr bus pulsed (retrieved in IBL FE Flag word). Adding to Flagged errors");
    addToFlaggedErrorCounter(serviceCodeCounter);
    break;

  // case 11 to 13 not used

  case 14: // 3 MSBs of bunch counter and 7 MSBs of L1A counter from EOCHL
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "3 MSBs of bunch counter (retrieved in IBL FE Flag word): " << ((serviceCode >> 7) & 0x7) << endreq;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "7 MSBs of L1A counter (retrieved in IBL FE Flag word): " << (serviceCode & 0x7F) << endreq;
    break;

  case 15: // Skipped trigger counter
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Skipped trigger counter (retrieved in IBL FE Flag word). There are " << serviceCodeCounter << " skipped triggers. Adding to Flagged errors" << endreq;
    addToFlaggedErrorCounter(serviceCodeCounter);
    break;

  case 16: // Truncated event flag and counter from EOCHL
    ATH_MSG_DEBUG("Truncated event flag and counter (retrieved in IBL FE Flag word). Adding to Flagged errors");
    addToFlaggedErrorCounter(serviceCodeCounter);
    break;

  // case 17 to 20 not used

  case 21: // Reset bar RA2b pulsedd from Pad, PRD
    ATH_MSG_DEBUG("Reset bar RA2b pulsed (retrieved in IBL FE Flag word). Adding to Flagged errors");
    addToFlaggedErrorCounter(serviceCodeCounter);
    break;

  case 22: // PLL generated clock phase faster than reference from CLKGEN
    ATH_MSG_DEBUG("PLL generated clock phase faster than reference (retrieved in IBL FE Flag word). Adding to Flagged errors");
    addToFlaggedErrorCounter(serviceCodeCounter);
    break;

  case 23: // Reference clock phase faster than PLL, from CLKGEN
    ATH_MSG_DEBUG("Reference clock phase faster than PLL (retrieved in IBL FE Flag word). Adding to Flagged errors");
    addToFlaggedErrorCounter(serviceCodeCounter);
    break;

  case 24: // Triple redundant mismatchfrom CNFGMEM
    ATH_MSG_DEBUG("Triple redundant mismatch (retrieved in IBL FE Flag word). Adding to Flagged errors");
    addToFlaggedErrorCounter(serviceCodeCounter);
    break;

  case 25: // Write register data error from CMD
    ATH_MSG_DEBUG("Write register data error (retrieved in IBL FE Flag word). Adding to Flagged errors");
    addToFlaggedErrorCounter(serviceCodeCounter);
    break;

  case 26: // Address error from CMD
    ATH_MSG_DEBUG("Address error (retrieved in IBL FE Flag word). Adding to Flagged errors");
    addToFlaggedErrorCounter(serviceCodeCounter);
    break;

  case 27: // Other command decoder error- see CMD section from CMD
    ATH_MSG_DEBUG("Other command decoder error- see CMD section (retrieved in IBL FE Flag word). Adding to Flagged errors");
    addToFlaggedErrorCounter(serviceCodeCounter);
    break;

  case 28: // Bit flip detected in command decoder input stream from CMD
    ATH_MSG_DEBUG("Bit flip detected in command decoder input stream (retrieved in IBL FE Flag word). Adding to Flagged errors");
    addToFlaggedErrorCounter(serviceCodeCounter);
    break;

  case 29: // SEU upset detected in command decoder (triple redundant mismatch) from CMD
    ATH_MSG_DEBUG("SEU upset detected in command decoder (triple redundant mismatch) (retrieved in IBL FE Flag word). Adding to Flagged errors");
    addToFlaggedErrorCounter(serviceCodeCounter);
    break;

  case 30: // Data bus address error from CMD
    ATH_MSG_DEBUG("Data bus address error (retrieved in IBL FE Flag word). Adding to Flagged errors");
    addToFlaggedErrorCounter(serviceCodeCounter);
    break;

  case 31: // Triple redundant mismatch from CMD
    ATH_MSG_DEBUG("Triple redundant mismatch (retrieved in IBL FE Flag word). Adding to Flagged errors");
    addToFlaggedErrorCounter(serviceCodeCounter);
    break;

  default:
    ATH_MSG_DEBUG("ServiceCode not used at the moment");
  }
}
