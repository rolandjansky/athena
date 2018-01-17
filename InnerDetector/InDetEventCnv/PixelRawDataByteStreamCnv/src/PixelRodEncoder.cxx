/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelRodEncoder.cxx
//   Implementation file for class PixelRodEncoder
///////////////////////////////////////////////////////////////////
// (c) ATLAS Pixel Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//  Version 00-00-00 06/03/2007 Daniel Dobos
///////////////////////////////////////////////////////////////////

#include "PixelRodEncoder.h" 
#include "InDetReadoutGeometry/SiNumerology.h"
#include <algorithm>

//using namespace std;

//#define PIXEL_DEBUG ;
//#define PLOTS ;

////////////////////////
// constructor
////////////////////////
PixelRodEncoder::PixelRodEncoder() : 
    m_PixelID(NULL),
    m_pixelCabling("dummy","dummy"), // the cabling tool is set in PixelRawContByteStreamTool
    m_PixelDetectorManager(NULL),
    m_RodBlockVersion(0),
    m_is_ibl_present(0),
    m_is_ibl_module(0),
    m_is_dbm_module(0)
{
}

////////////////////////
// destructor 
////////////////////////
PixelRodEncoder::~PixelRodEncoder() 
{
}


////////////////////////
//  fillROD() - convert Pixel RDO to a vector of 32bit words
////////////////////////
void PixelRodEncoder::fillROD(std::vector<uint32_t>& v32rod, MsgStream& log, int BCs_per_LVL1ID) 
{
#ifdef PIXEL_DEBUG
  log << MSG::VERBOSE << " #####################################################################################" << endmsg;
  log << MSG::DEBUG << "Entering PixelRodEncoder" << endmsg;
#endif

  // Loop over the Hits in a ROD
  VRDO::iterator rdo_it     = m_RDOs.begin(); 
  VRDO::iterator rdo_it_end = m_RDOs.end();

  m_is_ibl_present = false;

  const InDetDD::SiNumerology& pixSiNum = m_PixelDetectorManager->numerology(); 
  m_is_ibl_present = (pixSiNum.numLayers() == 4);
#ifdef PIXEL_DEBUG
  log << MSG::VERBOSE << "m_is_ibl_present = " << m_is_ibl_present << endmsg;
  log << MSG::DEBUG << "pixSiNum.numLayers() =  " << pixSiNum.numLayers() << endmsg;
#endif
  
  m_is_ibl_module = false;
  m_is_dbm_module = false;
#ifdef PIXEL_DEBUG
  log << MSG::VERBOSE << "in fillROD with " << BCs_per_LVL1ID << " LVL1As" << endmsg;
  log << MSG::VERBOSE << "Dimension of the RDO vector: " << m_RDOs.size() << endmsg;
#endif

  int hitDiscCnfg = 2;

  // ordering of the elements of the RDOs vector by offlineId, n5
  if (rdo_it != rdo_it_end) {
    OrderInitialRdos orderInitialRdos(m_pixelCabling, m_PixelID); 
    std::sort (rdo_it, rdo_it_end, orderInitialRdos); 
/*
#ifdef PIXEL_DEBUG
    rdo_it = m_RDOs.begin(); 
    int i(0);
    uint32_t fe;
    Identifier offlineId_last;
    for (;rdo_it != rdo_it_end; ++rdo_it) {
      if (rdo_it == m_RDOs.begin()) {
	offlineId_last = m_PixelID->wafer_id((*rdo_it)->identify());
      }
      if (m_PixelID->wafer_id((*rdo_it)->identify()) != offlineId_last) {
	log << MSG::VERBOSE << endmsg;
      }
      ++i;
      if ( (m_pixelCabling->isIBL((*rdo_it)->identify())) || 
	   (m_pixelCabling->isDBM((*rdo_it)->identify())) ) { // IBL and DBM
	fe = m_pixelCabling->getFEwrtSlink(&((*rdo_it)->identify()));
	log << MSG::VERBOSE << "IBL/DBM \t";
      }
	else {
	  fe = m_pixelCabling->getFE(&((*rdo_it)->identify()), m_PixelID->wafer_id((*rdo_it)->identify()) );
	  log << MSG::VERBOSE << "Pixels  \t";
	}
      log << MSG::VERBOSE << "offlineId: " << std::hex << m_PixelID->wafer_id((*rdo_it)->identify()) << "\t FE n.: " << fe << endmsg;
      offlineId_last = m_PixelID->wafer_id((*rdo_it)->identify());
      //log << MSG::VERBOSE << "offlineId: " << std::hex << m_PixelID->wafer_id((*rdo_it)->identify()) << "\t FE n.: " << fe << endmsg;
    }
#endif
*/
  }  
  // end of ordering of the elements of the RDOs vector by offlineId, n5 
  // NOW the RDOs should be ordered following (1) the offlineId, (2) the FE number

  rdo_it = m_RDOs.begin(); 
  if (rdo_it!=rdo_it_end) {

    const RDO * rawdata;
    Identifier offlineId;
    Identifier prev_offlineId(0x0);
    Identifier pixelId;
    bool timing_error = false;
    bool condensedMode = false;
    bool linkMasked = false;
    uint32_t linknumber(0);
    uint32_t FE(0);
    uint32_t sLink (0);
    uint32_t n5(0);
    uint32_t prev_n5(0);
//    uint32_t prev_FE(0);

    int last_BCID = 0;  // needed for encoding of timing information
    while (rdo_it != rdo_it_end) {
#ifdef PIXEL_DEBUG
      log << MSG::VERBOSE << "Inside cycle on the rdo_it" << endmsg;
 #endif
      rawdata = (*rdo_it);
      pixelId = rawdata->identify();
      offlineId = m_PixelID->wafer_id(pixelId); 

      uint32_t robId = m_pixelCabling->getRobId(offlineId); 
      uint64_t onlineId = m_pixelCabling->getOnlineId(offlineId); // (32bit) working on modules, not on single pixels 
      linknumber = (onlineId >> 24) & 0xFFFF;

      // All these functions below are methods of the class PixelRDORawData, InnerDetector/InDetRawEvent/InDetRawData
      int TOT = rawdata->getToT(); // it returns a 8 bits "word"
      int BCID = rawdata->getBCID();
      int LVL1ID = rawdata->getLVL1ID();
      int LVL1A = rawdata->getLVL1A();
 
      m_is_ibl_module = m_pixelCabling->isIBL(robId);
      m_is_dbm_module = m_pixelCabling->isDBM(robId);

#ifdef PIXEL_DEBUG
      log << MSG::DEBUG << " ********************* robId retrieved: 0x" << std::hex << robId << std::dec << endmsg;
      log << MSG::DEBUG << "offlineId retrieved: " << offlineId << endmsg;
      log << MSG::DEBUG << "onlineId retrieved: 0x" << std::hex << onlineId << ", linknumber retrieved: 0x" << linknumber << std::dec
	  << ", ToT: " << TOT << ", BCID: " << BCID << ", LVL1ID: " << LVL1ID << ", LVL1A: " << LVL1A << endmsg;
      log << MSG::DEBUG << "Is IBL = " << m_is_ibl_module << "  or is DBM = " << m_is_dbm_module << endmsg;
#endif      


//*************************************************************************************************
// *************************************    PIXEL CASE    *****************************************
//*************************************************************************************************  
      if (!(m_is_ibl_module || m_is_dbm_module)) {

#ifdef PIXEL_DEBUG
      log << MSG::DEBUG << "This is the PixelCase of the PixelRodEncoder" << endmsg;
#endif

//----------------------------------------------------------------------------------------------
//- Fill the data
//----------------------------------------------------------------------------------------------
        if (prev_offlineId != offlineId) {
	  int fake_BCID;
	  timing_error = false;
	  if (BCs_per_LVL1ID < LVL1A) { // That must not happen, if LVL1A > BCs_per_LVL1ID, BCs_perLVL1ID is wrongly set in the joboptions
#ifdef PIXEL_DEBUG
	    log << MSG::DEBUG << "LVL1A > BCs_per_LVL1ID, timing corrupt, ignoring timing." << " Set BCs per LVL1ID: " << BCs_per_LVL1ID << endmsg;
#endif
	    timing_error = true;
	  }
	  if (prev_offlineId != 0x0) {
	    v32rod.push_back(packLinkTrailer(0x0));
#ifdef PIXEL_DEBUG
	    log << MSG::DEBUG << "Pixel module trailer" << endmsg;
	    log << MSG::VERBOSE << " ------------------------------------------------------------------------------------------" << endmsg;
#endif
	  //------------------------------------------------------------------------------------
	  //- Write empty Header/Trailer pairs after the level1 accept
	  //------------------------------------------------------------------------------------
	    fake_BCID = last_BCID;
            int max_BCID = fake_BCID + BCs_per_LVL1ID - LVL1A - 1;

	    while ((fake_BCID < max_BCID) && !timing_error) {
	      fake_BCID++;
	      v32rod.push_back(packLinkHeader(linknumber, fake_BCID, LVL1ID, (LVL1ID >> 4), 0x0));
	      v32rod.push_back(packLinkTrailer(0x0));
#ifdef PIXEL_DEBUG
	      log << MSG::DEBUG << "(after) empty Pixel Module header/trailer pair written for BCID " << fake_BCID << endmsg;
#endif
	    } // end while cycle "while ((fake_BCID < max_BCID) && !timing_error)"
	  } // end if "if (prev_offlineId != 0x0) "

	//--------------------------------------------------------------------------------------
	//- Write empty Header/Trailer pairs before the level1 accept
	//--------------------------------------------------------------------------------------
	  fake_BCID = BCID - LVL1A;

	  while ((fake_BCID < BCID) && !timing_error) {
	    v32rod.push_back(packLinkHeader(linknumber, fake_BCID, LVL1ID, (LVL1ID >> 4), 0x0));
	    v32rod.push_back(packLinkTrailer(0x0));
#ifdef PIXEL_DEBUG
	    log << MSG::DEBUG << "(before) empty Pixel Module header/trailer pair written for BCID " << fake_BCID << endmsg;
#endif
	    fake_BCID++;
	  } // end while cycle "while ((fake_BCID < BCID) && !timing_error)"

	  v32rod.push_back(packLinkHeader(linknumber, BCID, LVL1ID, (LVL1ID >> 4), 0x0));
#ifdef PIXEL_DEBUG
	  log << MSG::DEBUG << "Pixel module header" << endmsg;
#endif
	} // end if "if (prev_offlineId != offlineId) "

	//--------------------------------------------------------------------------------------
	//- Write RawDataWord
	//--------------------------------------------------------------------------------------
	
	FE = m_pixelCabling->getFE(&pixelId,offlineId);
	uint32_t row = m_pixelCabling->getRow(&pixelId,offlineId);
	uint32_t column = m_pixelCabling->getColumn(&pixelId,offlineId);
        v32rod.push_back(packRawDataWord(FE, row, column, TOT));

	
	// The following was used for running a validation scrip and making validation plots
#ifdef PLOTS
	int eta_i = m_PixelID->eta_index(pixelId);
	int phi_i = m_PixelID->phi_index(pixelId);
	std::cout << "[PlotB]: " << robId << " " << eta_i << " " << phi_i << " " << TOT << std::endl; 
	std::cout << "[PlotC]: " << robId << " " << column << " " << row << " " << TOT << std::endl; 

	std::cout << "[VAL] " << std::hex << pixelId << " 0x" << robId << " 0x" << onlineId // << " " << offlineId 
		  << std::dec << " " << m_PixelID->eta_module(pixelId) << " " << m_PixelID->phi_module(pixelId)
		  << " " << m_PixelID->eta_index(pixelId) << " " << m_PixelID->phi_index(pixelId) << std::endl;
#endif

	++rdo_it;
        last_BCID = BCID;  

#ifdef PIXEL_DEBUG
        log << MSG::DEBUG << "Found hit in PIXEL with PixelID: 0x" << std::hex << pixelId << std::dec << " FE: " << FE << " Row: " << row << " Column: " << column << " TOT: " << TOT << " BCID: " << BCID << " LVL1ID: " << LVL1ID << " LVL1A: " << LVL1A << endmsg;
#endif

#ifdef PIXEL_DEBUG
        log << MSG::DEBUG << "Encoded Pixel OfflineID: 0x" << std::hex << offlineId << " OnlineID: 0x" << onlineId << " -> Linknumber: 0x" << linknumber << std::dec << endmsg;
#endif
        prev_offlineId = offlineId;
      } // end Pixel Case  




//*************************************************************************************************
// *************************************     IBL CASE     *****************************************
//*************************************************************************************************  
      else {

#ifdef PIXEL_DEBUG
	log << MSG::DEBUG << "Inside the IBL/DBM case of the PixelRodEncoder" << endmsg;
#endif

	FE = m_pixelCabling->getFEwrtSlink(&pixelId); 
	sLink = onlineId & 0xF; // extract the LSB 4 bits from the onlineId
	if (sLink > 0x3) {
	  log << MSG::WARNING << "The SLink is not in the correct range [0,3]. This is due to the non-correct onlineId/ROBID definition. Skipping this RDO" << endmsg;
	  continue; // skipping this rdo, because it gives wrong onlineID (and, possibly, other pieces of information are wrong too)
	}
	n5 = ((sLink & 0x3) << 3) | (FE & 0x7); // this variable contains the 5 "nnnnn" bits, the 2 MSB ones representing the copy of the S-Link number (0 to 3) and the 2 LSBs representing the FE number over the S-Link
#ifdef PIXEL_DEBUG
	log << MSG::VERBOSE << "FE (w.r.t. SLink) = 0x" << std::hex << FE << " sLink: 0x" << sLink << " => n5: 0x" << n5 << std::dec << endmsg;
#endif

        hitDiscCnfg = m_pixelCabling->getHitDiscCnfg(robId, FE);

//----------------------------------------------------------------------------------------------
//- Fill the data
//----------------------------------------------------------------------------------------------
#ifdef PIXEL_DEBUG
	log << MSG::VERBOSE << "(prev_offlineId != offlineId) = " << (prev_offlineId != offlineId) << "   (prev_n5 != n5) = " <<  (prev_n5 != n5) << "  " << endmsg;
	log << MSG::VERBOSE << "prev_offlineId = " <<  prev_offlineId << endmsg;
#endif
        if ((prev_offlineId != offlineId) || (prev_n5 != n5)) {
	  int fake_BCID;
	  timing_error = false;
	  if (BCs_per_LVL1ID < LVL1A) { // That must not happen, if LVL1A > BCs_per_LVL1ID, BCs_perLVL1ID is wrongly set in the joboptions
#ifdef PIXEL_DEBUG
	    log << MSG::DEBUG << "LVL1A > BCs_per_LVL1ID, timing corrupt, ignoring timing." << " Set BCs per LVL1ID: " << BCs_per_LVL1ID << endmsg;
#endif
	    timing_error = true;
	  }

	  if (prev_offlineId != 0x0) {
	    v32rod.push_back(packLinkTrailer_IBL(prev_n5, timing_error, condensedMode, linkMasked)); // Trailer for IBL
	    condensedMode = false;
#ifdef PIXEL_DEBUG
	    log << MSG::DEBUG << "IBL Module trailer (because prev_offlineId != 0x0)" << endmsg;
#endif

	  //------------------------------------------------------------------------------------
	  //- Write empty Header/Trailer pairs after the level1 accept 
	  //------------------------------------------------------------------------------------
	    fake_BCID = last_BCID;
            int max_BCID = fake_BCID + BCs_per_LVL1ID - LVL1A - 1;

	    while ((fake_BCID < max_BCID) && !timing_error) {
	      fake_BCID++;
	      v32rod.push_back(packLinkHeader_IBL(n5, fake_BCID, LVL1ID, 0x0)); // Header for IBL
	      v32rod.push_back(packLinkTrailer_IBL(n5, timing_error, condensedMode, linkMasked)); // Trailer for IBL

#ifdef PIXEL_DEBUG
	      log << MSG::DEBUG << "(after) empty IBL Module header/trailer pair written for BCID " << fake_BCID << endmsg;
#endif
	    } // end while cycle "while ((fake_BCID < max_BCID) && !timing_error)"
	  } // end if "if (prev_offlineId != 0x0) "

	//--------------------------------------------------------------------------------------
	//- Write empty Header/Trailer pairs before the level1 accept
	//--------------------------------------------------------------------------------------
	  fake_BCID = BCID - LVL1A;

	  while ((fake_BCID < BCID) && !timing_error) {
	    v32rod.push_back(packLinkHeader_IBL(n5, fake_BCID, LVL1ID, 0x0)); // Header for IBL
	    v32rod.push_back(packLinkTrailer_IBL(n5, timing_error, condensedMode, linkMasked)); // Trailer for IBL
#ifdef PIXEL_DEBUG
	    log << MSG::DEBUG << "(before) empty IBL Module header/trailer pair written for BCID " << fake_BCID << endmsg;
#endif
	    fake_BCID++;
	  } // end while cycle "while ((fake_BCID < BCID) && !timing_error)"

	  v32rod.push_back(packLinkHeader_IBL(n5, BCID, LVL1ID, 0x0)); // Header for IBL
#ifdef PIXEL_DEBUG
	  log << MSG::DEBUG << "IBL Module header" << endmsg;
#endif
	} // end if "if (prev_offlineId != offlineId) "

	//--------------------------------------------------------------------------------------
	//- Write RawData word
	//--------------------------------------------------------------------------------------
	VRDO rdos_sameIBL_offlineId; // vector containing all the rdos with the same offlineId => belonging to the same IBL FE-I4 chip

	// This loop fills the rdo_sameIBL_offlineId vector with all the RDOs that have the same offlineId and same FEw.r.t.SLink => all RDOs coming from the same FE 
	for (; (rdo_it != rdo_it_end) && (((m_pixelCabling->isIBL((*rdo_it)->identify())) && (m_is_ibl_module)) || ((m_pixelCabling->isDBM((*rdo_it)->identify())) && m_is_dbm_module)); ++rdo_it) {
	  Identifier pixelId_probe = (*rdo_it)->identify();
    	  Identifier offlineId_probe = m_PixelID->wafer_id(pixelId_probe);
	  uint32_t fe_probe = m_pixelCabling->getFEwrtSlink(&pixelId_probe);

#ifdef PIXEL_DEBUG
	  log << MSG::VERBOSE << "offlineId: " << offlineId << "    offlineId_probe: " << offlineId_probe << ",    fe: " << FE << "     fe_probe: " << fe_probe << endmsg;
#endif

      if ((offlineId_probe == offlineId) && (FE == fe_probe)) {
#ifdef PIXEL_DEBUG
        log << MSG::VERBOSE << "=======> IN " << endmsg;
#endif
	    rdos_sameIBL_offlineId.push_back((*rdo_it));
	  }
	  else {
#ifdef PIXEL_DEBUG
	    log << MSG::VERBOSE << "=======> OUT." << endmsg; // It has been checked that no RDO is being missed when running inside the following loop.
#endif
	    break;
	  }
	} 

	VRDO::iterator rdo_same_it = rdos_sameIBL_offlineId.begin();
	VRDO::iterator rdo_same_it_end = rdos_sameIBL_offlineId.end();

#ifdef PIXEL_DEBUG
	//check: list of all the rdos with same offlineId, listing also the column, the row and the Tot
	for (; rdo_same_it != rdo_same_it_end; ++rdo_same_it) {
	  //	  log << MSG::DEBUG << "Dump rdo : " << (*rdo_same_it)  << "\t";
	  Identifier pixelId_probe = (*rdo_same_it)->identify();
	  uint32_t col = m_pixelCabling->getColumn(&pixelId_probe, offlineId); // offlineId of rdos in rdos_sameIBL_offlineId vector are, of course, all equal
	  uint32_t row = m_pixelCabling->getRow(&pixelId_probe, offlineId);
	  int tot = (*rdo_same_it)->getToT();
	  log << MSG::VERBOSE << "col: " << col << " (0x" << std::hex << col << std::dec << ")\trow: "<< row << " (0x" << std::hex << row << std::dec << ")\ttot: " << tot << "(0x" <<std::hex << tot << std::dec << ")" << endmsg;
	}

	rdo_same_it = rdos_sameIBL_offlineId.begin(); 
	rdo_same_it_end = rdos_sameIBL_offlineId.end();
#endif

        // Order the RDOs within the vector rdos_sameIBL_offlineId, following the ordering rules of orderRdos
	OrderRdos orderRdos(offlineId,m_pixelCabling);
	std::sort (rdo_same_it, rdo_same_it_end, orderRdos); 

	//check:
#ifdef PIXEL_DEBUG
	rdo_same_it = rdos_sameIBL_offlineId.begin();
      	rdo_same_it_end = rdos_sameIBL_offlineId.end();

	log << MSG::DEBUG << "Re-ordered RDOs with Same offlineId:" << endmsg;
	for (; rdo_same_it != rdo_same_it_end; ++rdo_same_it) {
	  Identifier pixelId_probe = (*rdo_same_it)->identify();
	  uint32_t col = m_pixelCabling->getColumn(&pixelId_probe, offlineId);
	  uint32_t row = m_pixelCabling->getRow(&pixelId_probe, offlineId);
	  int tot = (*rdo_same_it)->getToT();
	  int eta_i = m_PixelID->eta_index(pixelId_probe);
	  int phi_i = m_PixelID->phi_index(pixelId_probe);
	  int eta_m = m_PixelID->eta_module(pixelId_probe);
	  int phi_m = m_PixelID->phi_module(pixelId_probe);
	  log << MSG::VERBOSE << "pixelId: " << pixelId_probe << ",  eta_i: " << eta_i << ", phi_i: " << phi_i << ",  eta_m: " <<  eta_m << ", phi_m: " << phi_m << endmsg;
	  log << MSG::VERBOSE << "col: 0x" << std::hex << col << std::dec << ",  row: 0x" <<std::hex << row << std::dec << ",  tot = 0x" << std::hex << tot << std::dec << endmsg;
	}
	log << MSG::DEBUG << "rdos_sameIBL_offlineId.size() = " << rdos_sameIBL_offlineId.size() << endmsg;
#endif
#ifdef PLOTS
	rdo_same_it = rdos_sameIBL_offlineId.begin();
      	rdo_same_it_end = rdos_sameIBL_offlineId.end();
	for (; rdo_same_it != rdo_same_it_end; ++rdo_same_it) {
	  Identifier pixelId_probe = (*rdo_same_it)->identify();
	  uint32_t col = m_pixelCabling->getColumn(&pixelId_probe, offlineId);
	  uint32_t row = m_pixelCabling->getRow(&pixelId_probe, offlineId);
	  int tot = (*rdo_same_it)->getToT();
	  int eta_i = m_PixelID->eta_index(pixelId_probe);
	  int phi_i = m_PixelID->phi_index(pixelId_probe);
	  std::cout << "[Plot2]: " << robId << " " << eta_i << " " << phi_i << " " << tot << std::endl; 
	  std::cout << "[Plot3]: " << robId << " " << col << " " << row << " " << tot << std::endl; 
	}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// look for adjacent cell that can allow the "compression" of the two hit info in one: 
// instead of having two hits (= 2 columns, 2 rows and 2 tots), in fact, 
// if two adjacent pixels ([col, row] and [col, row+1]) have fired (respectively tot([col, row]) and tot([col, (row+1)])
// then
// the row in the hit will be the row with lower number, the column in the hit will be the common column number 
// and the tot will be = (tot [(col, row)] << 4) | tot[(col, row+1)]
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector <uint32_t> vRows;
	std::vector <uint32_t> vCols;
	std::vector <int> vTots;
	bool doubleHit = false; 
	static const uint32_t rowsPerFE = 336;// FIXME: this is a hardcoded variable, would be better to get it from somewhere

	rdo_same_it = rdos_sameIBL_offlineId.begin();
	rdo_same_it_end = rdos_sameIBL_offlineId.end();

	VRDO::iterator rdo_test_it = rdos_sameIBL_offlineId.begin();
#ifdef PIXEL_DEBUG
	log << MSG::VERBOSE << "Looking for adjacent pixels and saving col, row and tot information." << endmsg;
#endif

	for (; rdo_same_it != rdo_same_it_end; ++rdo_same_it) {
	  doubleHit = false;
	  Identifier pixelId_probe = (*rdo_same_it)->identify();
	  uint32_t col0 = m_pixelCabling->getColumn(&pixelId_probe, offlineId);
	  uint32_t row0 = m_pixelCabling->getRow(&pixelId_probe, offlineId);
	  int totInHitWord (0);
#ifdef PLOTS
	  std::cout << "[VAL] " << std::hex << pixelId_probe << " 0x" << robId << " 0x" << onlineId // << " " << offlineId 
		    << std::dec << " " << m_PixelID->eta_module(pixelId_probe) << " " << m_PixelID->phi_module(pixelId_probe)
		    << " " << m_PixelID->eta_index(pixelId_probe) << " " << m_PixelID->phi_index(pixelId_probe) << std::endl;
#endif

	  if (row0 == rowsPerFE) {
#ifdef PIXEL_DEBUG
	    log << MSG::VERBOSE << "Hit in the last row (== 336) of the IBL FE." << endmsg;
#endif
	  }
	  else {
	    if ((rdo_same_it + 1) != rdo_same_it_end) {
	      rdo_test_it = rdo_same_it + 1;
	      Identifier pixelId_probe = (*rdo_test_it)->identify();
	      uint32_t col1 = m_pixelCabling->getColumn(&pixelId_probe, offlineId);
	      uint32_t row1 = m_pixelCabling->getRow(&pixelId_probe, offlineId);
#ifdef PLOTS
	      std::cout << "[VAL] " << std::hex << pixelId_probe << " 0x" << robId << " 0x" << onlineId // << " " << offlineId 
			<< std::dec << " " << m_PixelID->eta_module(pixelId_probe) << " " << m_PixelID->phi_module(pixelId_probe)
			<< " " << m_PixelID->eta_index(pixelId_probe) << " " << m_PixelID->phi_index(pixelId_probe) << std::endl;
#endif

#ifdef PIXEL_DEBUG
	      log << MSG::VERBOSE << "Comparing rdo[i] = " << (*rdo_same_it) << "   with rdo[i+1] = " << (*rdo_test_it) << endmsg;
	      log << MSG::VERBOSE << "   col0 = 0x" << std::hex << col0 << "   col1 = 0x" << col1 << "   row0 = 0x" << row0 << "\t row1 = 0x" << row1 << std::dec << endmsg;
#endif	    

	      if ((col1 == col0) && (row1 == (row0 + 1))) {
		doubleHit = true;
#ifdef PIXEL_DEBUG
		log << MSG::VERBOSE << "Two adjacent hits found" << endmsg;
#endif
		int tot0 = (*rdo_same_it)->getToT();
		int tot1 = (*rdo_test_it)->getToT();

                // Adjust ToT according to hitdisccnfg setting
                if (hitDiscCnfg == 2 && tot0 == 16) tot0 = 2;
                if (hitDiscCnfg == 2 && tot1 == 16) tot1 = 2;

                int overflow = 14;
                if (hitDiscCnfg == 1) overflow = 15;
                if (hitDiscCnfg == 2) overflow = 16;

                if (tot0 > overflow) tot0 = overflow;
                if (tot1 > overflow) tot1 = overflow;

                totInHitWord = (tot0 << 4) | tot1;

#ifdef PIXEL_DEBUG
		log << MSG::VERBOSE << "doubleHit = " << std::boolalpha << doubleHit << std::noboolalpha << " ===> (col0 == col1) : 0x" << std::hex << col0 << " = 0x" << col1 << ";    (row0 = row1 - 1) : 0x" << row0 << " => 0x" << row1 <<";     (tot0) : 0x" << tot0 << ", (tot1) : 0x" << tot1 << " => totInHitWord: 0x" << totInHitWord << std::dec << endmsg;
#endif
		rdo_same_it++;
	      } // end if "(col1==col0) && (row1 ==  (row0+1))"
	    } // end if "(rdo_same_it + 1) != rdo_same_it_end" 
	    
	    else { 
#ifdef PIXEL_DEBUG
	      log << MSG::VERBOSE << "last rdo with same Offline Id" << endmsg;
#endif
	    } // end if it's the last rdo with same offlineId 
	  } // end if "row0 == rowsPerFE" (== 336)

	  if (!doubleHit) {

              int tot0 = (*rdo_same_it)->getToT();

              // Adjust ToT according to hitdisccnfg setting
              if (hitDiscCnfg == 2 && tot0 == 16) tot0 = 2;

              int overflow = 14;
              if (hitDiscCnfg == 1) overflow = 15;
              if (hitDiscCnfg == 2) overflow = 16;
              if (tot0 > overflow) tot0 = overflow;

              totInHitWord = (tot0 << 4) | 0x0;

#ifdef PIXEL_DEBUG
	    log << MSG::VERBOSE << "doubleHit = " << std::boolalpha << doubleHit << std::noboolalpha << " ===> col0: 0x" << std::hex << col0 << std::dec << ";   row0: 0x" << std::hex << row0 << std::dec << "   totInHitWord: 0x" << std::hex << totInHitWord << std::dec << endmsg;
#endif
	  }
	  vCols.push_back(col0);
	  vRows.push_back(row0);
	  vTots.push_back(totInHitWord);	  
	} // end loop over the rdos with the same offlineId

	//check:
#ifdef PIXEL_DEBUG
	log << MSG::DEBUG << "CHECKs over the vectors storing columns, rows, ToTs of IBL/DBM hits:" << endmsg;
	log << MSG::VERBOSE << " vCols: " << endmsg;
	std::vector<uint32_t>::iterator vCols_it = vCols.begin();
	std::vector<uint32_t>::iterator vCols_it_end = vCols.end();
	for (; vCols_it != vCols_it_end; ++vCols_it) {
	  log << MSG::VERBOSE << "0x" << std::hex << *vCols_it << std::dec << "   ";
	}
	log << MSG::VERBOSE << endmsg;
	log << MSG::VERBOSE << "vRows: " << endmsg;
	std::vector<uint32_t>::iterator vRows_it = vRows.begin();
	std::vector<uint32_t>::iterator vRows_it_end = vRows.end();
	for (; vRows_it != vRows_it_end; ++vRows_it) {
	  log << MSG::VERBOSE << "0x" << std::hex << *vRows_it << std::dec <<"   ";
	}
	log << MSG::VERBOSE << endmsg;
	log << MSG::VERBOSE << "vTots: " << endmsg;
	std::vector<int>::iterator vTots_it = vTots.begin();
	std::vector<int>::iterator vTots_it_end = vTots.end();
	for (; vTots_it != vTots_it_end; ++vTots_it) {
	  log << MSG::VERBOSE << "0x" << std::hex << *vTots_it << std::dec << "   ";
	}
	log << MSG::VERBOSE << endmsg;
	log << MSG::DEBUG << "rdos_sameIBL_offlineId.size() = " << rdos_sameIBL_offlineId.size() << "    vRows.size() = " << vRows.size() << "    vCols.size() = " << vCols.size() << "    vTots.size() = " << vTots.size() << endmsg;
#endif

	//Packing of the IBL hits 
	if (vRows.size() >= 5) { 
#ifdef PIXEL_DEBUG
	  log << MSG::DEBUG << "5 (or more) IBL hits have been consequently found. They can be written as condensed hits" << endmsg;
#endif
	  while (vRows.size() >= 5) {
	    packIBLcondensed(v32rod, vRows, vCols, vTots);
	    condensedMode= true;

	  }
	}
	if (vRows.size() != 0) { // packing remaining non-condensed IBL hit words
	  //	  int cycleCounter(0);
      	  for (; vRows.size() != 0; ) {	  
	    v32rod.push_back(packRawDataWord_IBL(vRows.at(0), vCols.at(0), vTots.at(0), n5));	   
     	
        vRows.erase(vRows.begin());
	    vCols.erase(vCols.begin());
	    vTots.erase(vTots.begin());
	  }
	}
	
        last_BCID = BCID;  

#ifdef PIXEL_DEBUG
        log << MSG::DEBUG << "Encoded IBL OfflineID: " << std::hex << offlineId << " OnlineID: 0x" << onlineId << std::dec << endmsg;
#endif

        prev_offlineId = offlineId;
	prev_n5 = n5;
//	prev_FE = FE;
      } // end of the IBL case
    } // end WHILE cycle " while  (rdo_it!=rdo_it_end) "    

    if (m_is_ibl_module || m_is_dbm_module) {
      v32rod.push_back(packLinkTrailer_IBL(n5, timing_error, condensedMode, linkMasked));
      condensedMode = false;
#ifdef PIXEL_DEBUG
      log << MSG::DEBUG << "Module IBL/DBM trailer (at end of the loop)" << endmsg;
#endif
    }
    else {
      v32rod.push_back(packLinkTrailer(0x0));
#ifdef PIXEL_DEBUG
      log << MSG::DEBUG << "Pixel module trailer" << endmsg;
#endif
    }
  } // end if "if (rdo_it != rdo_it_end)"
#ifdef PIXEL_DEBUG  
  else {
    log << MSG::DEBUG << "rdo_it == rdo_it_end" << endmsg;
  }
#endif
  return; 
} 



////////////////////////
// encode module Header for Pixels 
// Pixel Header: 001PtlbxxnnnnnnnMMMMLLLLBBBBBBBB,
////////////////////////
uint32_t PixelRodEncoder::packLinkHeader(uint32_t module, uint32_t bcid, uint32_t lvl1id, uint32_t lvl1idskip, uint32_t errors) {
  lvl1idskip = 0;   // FIXME LVL1IDskip hardcoded as 0
  uint32_t result = 0;
  result = PRB_LINKHEADER | ((bcid & PRB_BCIDmask) << PRB_BCIDskip) | ((lvl1id & PRB_L1IDmask) << PRB_L1IDskip) | ((lvl1idskip & PRB_L1IDSKIPmask) << PRB_L1IDSKIPskip) | ((module & PRB_MODULEmask) << PRB_MODULEskip) | ((errors & PRB_HEADERERRORSmask) << PRB_HEADERERRORSskip);
#ifdef PLOTS
  std::cout << "[PlotA]:0x " << std::hex << result << std::dec << std::endl;
  std::cout << "[PlotA]:(dec) " << result << std::endl;
#endif
  return result;
}


////////////////////////
// encode module Header for IBL
// IBL Header:   001nnnnnFLLLLLLLLLLLLLBBBBBBBBBB
////////////////////////
uint32_t PixelRodEncoder::packLinkHeader_IBL(uint32_t module, uint32_t bcid, uint32_t lvl1id, uint32_t feFlag) {
  uint32_t result = 0;
  result = PRB_LINKHEADER | ((bcid & PRB_BCIDmask_IBL) << PRB_BCIDskip_IBL) | ((lvl1id & PRB_L1IDmask_IBL) << PRB_L1IDskip_IBL) | ((module & PRB_MODULEmask_IBL) << PRB_MODULEskip_IBL) | ((feFlag & PRB_FeI4BFLAGmask_IBL) << PRB_FeI4BFLAGskip_IBL); 
#ifdef PIXEL_DEBUG
  //  std::cout << "IBL HEADER: linkNum (=n): 0x" << std::hex << module << std::dec << ", bcid: " << bcid << ", lvl1id: " << lvl1id << ", feFlag: " << feFlag << "====> Result: 0x" << std::hex << result << std::dec << std::endl;
#endif
#ifdef PLOTS
  std::cout << "[Plot1]:0x " << std::hex << result << std::dec << std::endl;
  std::cout << "[Plot1]:(dec) " << result << std::endl;
#endif
  return result;
}


////////////////////////
// encode IBL non-condensed hit word: 0-8: row,9-15: column, 16-23:TOT, 24-28: nLink   ----> 100xxnnnTTTTTTTTCCCCCCCRRRRRRRRR
////////////////////////
uint32_t PixelRodEncoder::packRawDataWord_IBL(uint32_t row, uint32_t column, int ToT, uint32_t nLink) {
  uint32_t result = 0;
  result = PRB_DATAWORD | ((row & PRB_ROWmask_IBL) << PRB_ROWskip_IBL) | ((column & PRB_COLUMNmask_IBL) << PRB_COLUMNskip_IBL) | ((ToT & PRB_TOTmask) << PRB_TOTskip) | ((nLink & PRB_LINKNUMHITmask_IBL) << PRB_LINKNUMHITskip_IBL);
#ifdef PIXEL_DEBUG
  //  std::cout << "IBL NON-CONDENSED HIT: nLink: 0x" << std::hex << nLink << ", row: 0x" << row << ",  col: 0x" << column << ",  tot: 0x" << ToT << " ===> encoded IBL word: 0x" << result << std::dec << std::endl;
#endif
#ifdef PLOTS
  std::cout << "[Plot1]:0x " << std::hex << result << std::dec << std::endl;
  std::cout << "[Plot1]:(dec) " << result << std::endl;
#endif
  return result;
}

////////////////////////
// encode PIXEL hit word: bits 0-7:row,8-12:column,16-23:TOT,24-27:FE ----> 100xFFFFTTTTTTTTxxxCCCCCRRRRRRRR
////////////////////////
uint32_t PixelRodEncoder::packRawDataWord(uint32_t FE, uint32_t row, uint32_t column, uint32_t ToT) {

  uint32_t result = 0;
  result = PRB_DATAWORD | ((row & PRB_ROWmask) << PRB_ROWskip) | ((column & PRB_COLUMNmask) << PRB_COLUMNskip) | ((ToT & PRB_TOTmask) << PRB_TOTskip) | ((FE & PRB_FEmask) << PRB_FEskip); 
#ifdef PLOTS
  std::cout << "[PlotA]:0x " << std::hex << result << std::dec << std::endl;
  std::cout << "[PlotA]:(dec) " << result << std::endl;
#endif
  return result;
}


////////////////////////
// encode PIXEL module trailer (bits 26-28:trailer errors)
////////////////////////
uint32_t PixelRodEncoder::packLinkTrailer(uint32_t errors) {
  uint32_t result = PRB_LINKTRAILER | ((errors & PRB_TRAILERERRORSmask) << PRB_TRAILERERRORSskip);
#ifdef PLOTS
  std::cout << "[PlotA]:0x " << std::hex << result << std::dec << std::endl;
  std::cout << "[PlotA]:(dec) " << result << std::endl;
#endif
  return result;
}


////////////////////////
// encode IBL module trailer (bits 26-28:trailer errors)
////////////////////////
uint32_t PixelRodEncoder::packLinkTrailer_IBL(uint32_t linknum, bool timeOutErrorBit, bool condensedModeBit, bool linkMasked) {
  //  return PRB_LINKTRAILER |((timeOutErrorBit & PRB_TIMEOUTERRORmask_IBL) << PRB_TIMEOUTERRORskip_IBL) | ((condensedModeBit & PRB_CONDENSEDMODEmask_IBL) << PRB_CONDENSEDMODEskip_IBL) | ((linkMasked & PRB_LINKMASKEDmask_IBL) << PRB_LINKMASKEDskip_IBL)  | ((linknum & PRB_LINKNUMTRAILERmask_IBL) << PRB_LINKNUMTRAILERskip_IBL);
  uint32_t result;
  result = PRB_LINKTRAILER | (timeOutErrorBit  << PRB_TIMEOUTERRORskip_IBL) | (condensedModeBit << PRB_CONDENSEDMODEskip_IBL) | (linkMasked  << PRB_LINKMASKEDskip_IBL)  | ((linknum & PRB_LINKNUMTRAILERmask_IBL) << PRB_LINKNUMTRAILERskip_IBL);
#ifdef PIXEL_DEBUG
  //  std::cout << "IBL TRAILER: linknum = 0x" << std::hex << linknum << ",  timeOutErrorBit: 0x" << timeOutErrorBit << ",  condensedModeBit: 0x" << condensedModeBit << ",  linkMasked: 0x" << linkMasked << ", ===> Result: 0x" << result << std::dec << std::endl;
#endif
#ifdef PLOTS
  std::cout << "[Plot1]:0x " << std::hex << result << std::dec << std::endl;
  std::cout << "[Plot1]:(dec) " << result << std::endl;
#endif
  return result;
}


////////////////////////
// Encode IBL Condensed hit words: 
// for IBL condensed word: (R = row, C = column, T = TOT)
//     1st word:   101 RRRRRTTTTTTTTCCCCCCCRRRRRRRRR
//     2nd word:   1 CCCRRRRRRRRRTTTTTTTTCCCCCCCRRRR
//     3rd word:   1 TTTCCCCCCCRRRRRRRRRTTTTTTTTCCCC
//     4th word:   111 TTTTTTTTCCCCCCCRRRRRRRRRTTTTT
////////////////////////

void PixelRodEncoder::packIBLcondensed(std::vector <uint32_t> & v32rod, std::vector <uint32_t> & vRows, std::vector <uint32_t> & vCols, std::vector<int> & vTots)
{
#ifdef PIXEL_DEBUG
//  std::cout << "Inside packIBLcondensed function\n";
#endif

  unsigned int condWord[nCondensedWords];
  condWord[0] = PRB_FIRSTHITCONDENSEDWORD | vRows[0] | (vCols[0] << skipRow) | (vTots[0] << (skipRow + skipCol) | ((vRows[1] & mask5) << (skipRow + skipCol + skipTOT)));

  condWord[1] = PRB_MIDDLEHITCONDENSEDWORD | (vRows[1] >> skip5) | (vCols[1] << skip4) | (vTots[1] << (skip4 + skipCol)) | (vRows[2] << (skip4+skipCol+skipTOT)) | ((vCols[2] & mask3) << (skip4+skipCol+skipTOT+skipRow));

  condWord[2] = PRB_MIDDLEHITCONDENSEDWORD | (vCols[2] >> skip3) | (vTots[2] << skip4) | (vRows[3] << (skip4+skipTOT)) | (vCols[3] << (skip4+skipTOT+skipRow)) | ((vTots[3] & mask3) << (skip4+skipTOT+skipRow+skipCol));

  condWord[3] = PRB_DATAMASK | (vTots[3] >> skip3) | (vRows[4] << skip5) | (vCols[4] << (skip5+skipRow)) | (vTots[4] << (skip5+skipRow+skipCol));

  for (int j(0); j < 4; ++j) {
    v32rod.push_back(condWord[j]); // Filling the ROD vector here
#ifdef PIXEL_DEBUG
    //    std::cout << "condensedWord: 0x" << std::hex << condWord[j] << std::dec << std::endl;
#endif
#ifdef PLOTS
  std::cout << "[Plot1]:0x " << std::hex << condWord[j] << std::dec << std::endl;
  std::cout << "[Plot1]:(dec) " << condWord[j] << std::endl;
#endif   
  }

  vRows.erase (vRows.begin(), vRows.begin() + 5); 
  vCols.erase (vCols.begin(), vCols.begin() + 5); 
  vTots.erase (vTots.begin(), vTots.begin() + 5); 
#ifdef PIXEL_DEBUG
//  std::cout << "\tvRows.size() = " << vRows.size() << "\tvCols.size() = " << vCols.size() << "\tvTots.size() = " << vTots.size() << std::endl;
#endif
  return;
}


////////////////////////
// function used by std::sort to order the RDO vector that contains RDOs with the same offlineId
// It orders the rdos_sameIBL_offlineId by //column (ascending order), row (ascending order, once the column is the same)
// example: coordinates (col, row): (1,1), (3,6), (1,2), (4,82), (4,81) become:  (1,1), (1,2), (3,6), (4,81), (4,82).
////////////////////////
/*
bool OrderRdos::operator () (const PixelRDORawData* rdo0, const PixelRDORawData* rdo1) //, const Identifier & offlineId
{

  Identifier pixelId0 = rdo0->identify();
  uint32_t col0 = m_pixelCabling->getColumn(&pixelId0, m_offlineId);
  Identifier pixelId1 = rdo1->identify();
  uint32_t col1 = m_pixelCabling->getColumn(&pixelId1, m_offlineId);
  if (col0 < col1) {return true;}
  if (col0 == col1) {
    uint32_t row0 = m_pixelCabling->getRow(&pixelId0, m_offlineId);
    uint32_t row1 = m_pixelCabling->getRow(&pixelId1, m_offlineId);
    return (row0<row1);
  }
  else {return false;}
}
*/

bool OrderRdos::operator () (const PixelRDORawData* rdo0, const PixelRDORawData* rdo1) //, const Identifier & offlineId
{
  //  const uint32_t halfCols = 40; // this is the number of the FE-I4 columns / 2, because the two tokens in the FE-I4 run from the double column 0 to 19, and then from 39 to 20.
  // This corresponds to column 1 to 40, and 79-80, 77-78, ... to 41-42.
  Identifier pixelId0 = rdo0->identify();
  uint32_t col0 = m_pixelCabling->getColumn(&pixelId0, m_offlineId);
  uint32_t row0 = m_pixelCabling->getRow(&pixelId0, m_offlineId);
  Identifier pixelId1 = rdo1->identify();
  uint32_t col1 = m_pixelCabling->getColumn(&pixelId1, m_offlineId);
  uint32_t row1 = m_pixelCabling->getRow(&pixelId1, m_offlineId);


  // Decide if (col0, row0) should be inserted in front of (col1, row1):

  // Check if both hits are in same column
  if (col0 == col1) return (row0 < row1);

  // If not, check if they are in same double column
  else if (((col0 == col1-1) && (col1%2 == 0)) || ((col1 == col0-1) && (col0%2 == 0))) {

      // If rows are equal, sort by ascending column
      if (row0 == row1) return (col0 < col1);
      // If rows are unequal, sort by ascending row
      else return (row0 < row1);
  }

  // Not in same double column: Separate between FE halfs
  else {

      // If both hits are in second FE half: Sort by descending col
      if (col0 > 40 && col1 > 40) return (col0 > col1);

      // Otherwise, sort by ascending col
      else return (col0 < col1);
  }



  /*
  // First FE half
  if (col0 <= 40 && col1 <= 40) {

      // Check if both hits are in the same double column
      if (col1 == col0 || ((col1 == col0+1) && (col1%2 == 0))) {

          // If yes, sort by lowest row.
          // Unless rows are equal, in which case sort on lowest column.
          if (row0 != row1) return row0 < row1;
          else return col0 < col1;
      }

      // If no, sort by lowest column
      else return col0 < col1;
  }

  // Second FE half
  else if (col0 > 40 && col1 > 40) {

      // Check if both hits are in the same double column
      if (col1 == col0 || ((col1 == col0+1) && (col1%2 == 0))) {

          // If yes, sort by lowest row.
          // Unless rows are equal, in which case sort on lowest column.
          if (row0 != row1) return row0 < row1;
          else return col0 < col1;
      }

      // If no, sort by highest column
      else return col0 > col1;

  }

  // Hits are on separate halfs
  else return col0 < col1;
  */

}

  /*
  // all this is done to take into account the token running on the two halves of the FE-I4 and the double column.
  if (col0 == col1) {
    //uint32_t row0 = m_pixelCabling->getRow(&pixelId0, m_offlineId);
    //uint32_t row1 = m_pixelCabling->getRow(&pixelId1, m_offlineId);
    return (row0<row1);
  }
  // first half FE-I4 (columns 1 to 40) normal ordering
  if (col1 <= 40) {return (col0 < col1);}

  // second half FE-I4 (columns 41 to 80): decreasing order of pairs of columns
  else { // (col1 > 40)
    if (col0 <= 40) {return true;}
    else { // both col0 and col1 > 40
      if (abs(col0 - col1) > 1) {return (col0 > col1);} // they differ more than 1, so they're simply ordered in decreasing order
      else { // their difference is == 1
        if (((col0 % 2) == 1)) {return true; } // col0 is odd:  has to go first
        else {return false;}  // col0 is even: has to go afterwards
      }
    }
  }
  return false;
}
*/



 bool OrderInitialRdos::operator() (const PixelRDORawData* rdo0, const PixelRDORawData* rdo1)
{
  Identifier pixelId0 = rdo0->identify();
  Identifier offlineId0 = m_PixelID->wafer_id(pixelId0);
  Identifier pixelId1 = rdo1->identify();
  Identifier offlineId1 = m_PixelID->wafer_id(pixelId1);
  if (offlineId0 < offlineId1) {
    return true;
  }
  if (offlineId0 == offlineId1) {
    if ( (m_pixelCabling->isIBL(pixelId0)) || (m_pixelCabling->isDBM(pixelId0)) ) { // IBL and DBM
      uint32_t fe0 = m_pixelCabling->getFEwrtSlink(&pixelId0);
      uint32_t fe1 = m_pixelCabling->getFEwrtSlink(&pixelId1);
      return (fe0 < fe1);
    }
    else { // PixelCase
      uint32_t fe0 = m_pixelCabling->getFE(&pixelId0, offlineId0);
      uint32_t fe1 = m_pixelCabling->getFE(&pixelId1, offlineId1);
      return (fe0 < fe1);

      //      return false;
    }
  }
  else {return false; }
}
