/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TileCellContainerCnv.h"
#include "CaloIdentifier/TileTBID.h"
#include "CaloDetDescr/MbtsDetDescrManager.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IChronoStatSvc.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

// false positive
// cppcheck-suppress uninitMemberVar
TileCellContainerCnv::TileCellContainerCnv(ISvcLocator* svcloc)
  : TileCellContainerCnvBase::T_AthenaPoolCustomCnv(svcloc)
 // Must create DataVector that does NOT own elements
  , m_storeGate(0)
  , m_tileTBID(0)
  , m_mbtsMgr(0)
  , m_version(0)
  , m_id()
  , m_dde()
  , m_gainIndex()
  , m_gain()
{
}


TileCellContainerCnv::~TileCellContainerCnv()
{
}

StatusCode TileCellContainerCnv::initialize()
{
    // Call base clase initialize
    if (!AthenaPoolConverter::initialize().isSuccess()) {
        return StatusCode::FAILURE;
    }

    // version 2 by default
    m_version = 2;
    
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), "TileCellContainerCnv");
    log << MSG::INFO << "TileCellContainerCnv::initialize(), packing format version " << m_version << endmsg;

    // get StoreGate service
    StatusCode sc=service("StoreGateSvc",m_storeGate);
    if (sc.isFailure()) {
      this->initIdToIndex();
      log << MSG::ERROR << "StoreGate service not found !" << endmsg;
      // log << MSG::FATAL << "StoreGate service not found !" << endmsg;
      // return StatusCode::FAILURE;
    }

    StoreGateSvc* detStore;
    sc=service("DetectorStore",detStore);
    if (sc.isFailure()) {
      this->initIdToIndex();
      log << MSG::ERROR << "DetectorStore service not found!" << endmsg;
      // log << MSG::FATAL << "DetectorStore service not found!" << endmsg;
      // return StatusCode::FAILURE;
    }

    sc = detStore->retrieve(m_tileTBID);
    if (sc.isFailure()) {
      this->initIdToIndex();
      log << MSG::ERROR << "No TileTBID helper" << endmsg;
      // log << MSG::FATAL << "No TileTBID helper" << endmsg;
      // return StatusCode::FAILURE;
    } else {
      for (int side=0; side<NSIDE; ++side) {
        for (int phi=0; phi<NPHI; ++phi) {
          for (int eta=0; eta<NETA; ++eta) {
            m_id[cell_index(side,phi,eta)] = m_tileTBID->channel_id((side>0)?1:-1,phi,eta);
          }
        }
      }
    }
    
    sc = detStore->retrieve(m_mbtsMgr);
    if (sc.isFailure()) {
      log << MSG::WARNING << "Unable to retrieve MbtsDetDescrManager from DetectorStore" << endmsg;
      memset(m_dde,0,sizeof(m_dde));
    } else {
      for (int side=0; side<NSIDE; ++side) {
        for (int phi=0; phi<NPHI; ++phi) {
          for (int eta=0; eta<NETA; ++eta) {
            m_dde[cell_index(side,phi,eta)] = m_mbtsMgr->get_element(m_id[cell_index(side,phi,eta)]);
          }
        }
      }
    }
    

    // set CaloGain <-> gain index mapping for all possible TileCal gains
    for (int i=0; i<17; ++i)  m_gainIndex[i] = 8;
    m_gain[0] = -2; // put non-existing gain here
    m_gainIndex[-CaloGain::TILELOWLOW]   = 8 + 1;  m_gain[1] = (int)CaloGain::TILELOWLOW;
    m_gainIndex[-CaloGain::TILELOWHIGH]  = 8 + 2;  m_gain[2] = (int)CaloGain::TILELOWHIGH;
    m_gainIndex[-CaloGain::TILEHIGHLOW]  = 8 + 3;  m_gain[3] = (int)CaloGain::TILEHIGHLOW;
    m_gainIndex[-CaloGain::TILEHIGHHIGH] = 8 + 4;  m_gain[4] = (int)CaloGain::TILEHIGHHIGH;
    m_gainIndex[-CaloGain::TILEONELOW]   = 8 + 5;  m_gain[5] = (int)CaloGain::TILEONELOW;
    m_gainIndex[-CaloGain::TILEONEHIGH]  = 8 + 6;  m_gain[6] = (int)CaloGain::TILEONEHIGH;
    m_gainIndex[-CaloGain::INVALIDGAIN]  = 8 + 7;  m_gain[7] = (int)CaloGain::INVALIDGAIN;
    
    return StatusCode::SUCCESS;
}

void TileCellContainerCnv::initIdToIndex()
{
  for (int side=0; side<NSIDE; ++side) {
    for (int phi=0; phi<NPHI; ++phi) {
      for (int eta=0; eta<NETA; ++eta) {
        m_id[cell_index(side,phi,eta)] = (Identifier)cell_index(side,phi,eta);
      }
    }
  }
}

TileCellVec* TileCellContainerCnv::createPersistent(TileCellContainer* cont)
{
    // Convert every TileCell to 3 32-bit integers: ID,Ene, and (time,qual,qain)

    MsgStream log(msgSvc(),"TileCellContainerCnv" );
    MSG::Level logLevel = log.level();
    bool lDebug = (logLevel<=MSG::DEBUG);
    bool lVerbose = (logLevel<=MSG::VERBOSE);

    std::string name = m_storeGate->proxy(cont)->name();
    auto vecCell = std::make_unique<TileCellVec>();
    vecCell->reserve(NCELLMBTS);

    if (lDebug) log << MSG::DEBUG << "storing TileCells from " << name << " in POOL" << endmsg;

    vecCell->push_back(m_version);
    int nMBTSfound=0;

    std::vector<const TileCell *> allCells;
    
    switch (m_version) {

    case 1: // 3 words per cell, energy scale factor is 1000, time scale factor is 100
      for (const TileCell* cell : *cont) {
        if (lVerbose)
          log << MSG::VERBOSE 
              << "ene=" << cell->energy()
              << " time=" << cell->time() 
              << " qual=" << (int)cell->qual1()
              << " gain=" << (int)cell->gain()
              << endmsg;
        unsigned int id  = cell->ID().get_identifier32().get_compact();
        int          ene = round32(cell->energy() * 1000.);
        unsigned int tim = 0x8000 + round16(cell->time()*100.);
        unsigned int qua = std::max(0, std::min(0xFF, (int)cell->qual1()));
        unsigned int gai = std::max(0, std::min(0xFF,   0x80   + (int)(cell->gain())));
        unsigned int tqg = (tim<<16) | (qua<<8) | gai;
        vecCell->push_back(id);
        vecCell->push_back((unsigned int)ene);
        vecCell->push_back(tqg);
        if (lVerbose)
          log << MSG::VERBOSE << "packing cell in three words " 
              << MSG::hex << id << " " << ene << " " << tqg << MSG::dec << endmsg;
      }
      break;

    case 2: // 1 or 2 words for MBTS cells, 3 words for others, energy scale factor is 1000, time scale factor is 100

      // prepare vector with all cells first, expect at least 32 MBTS cells
      allCells.resize(NCELLMBTS);
      for (const TileCell* cell : *cont) {
        Identifier id = cell->ID();
        if (m_tileTBID->is_tiletb(id)) {
          int side = std::max(0,m_tileTBID->type(id));
          int phi  = m_tileTBID->module(id);
          int eta  = m_tileTBID->channel(id);
          int ind  = cell_index(side,phi,eta);
          if (eta<NETA && phi<NPHI && ind < NCELLMBTS) {
            allCells[ind] = cell;
            ++nMBTSfound;
          } else {
            allCells.push_back(cell);
          }
        } else {
          allCells.push_back(cell);
        }
      }

     if (nMBTSfound>0) {

      // save first 32 cells (MBTS) without identifiers, 2 words per cell, put zeros for empty cells
      // if MBTS energy is in pCb, then LSB corresponds to 1/12 ADC count of high gain
      for (int ind=0; ind<NCELLMBTS; ++ind) {
        int energy = 0;
        int time   = 0;
        int quality= 0;
        int gain   = m_gain[0]; // non-existing gain in CaloGain - to mark non-existing cells
        const TileCell* cell = allCells[ind];
        if (cell) {
          energy = round32(cell->energy() * 1000.);
          time   = round16(cell->time() * 100.);
          quality= cell->qual1();
          gain   = cell->gain();

          if (lVerbose)
            log << MSG::VERBOSE 
                << "ind="  << ind
                << " ene=" << cell->energy()
                << " time=" << cell->time() 
                << " qual=" << (int)cell->qual1()
               << " gain=" << (int)cell->gain()
                << endmsg;
        } 
        else {
          if (lVerbose)
            log << MSG::VERBOSE 
                << "ind="  << ind
                << " create MBTS cell with zero energy"
                << endmsg;
        }
        
        // put correct MBTS cells in one word
        if (time == 0 && // expect time to be equal to zero
            -0x10000 < energy && energy < 0xEFFFF && // expect energy within (-65,980) pCb 
            -17 < gain && gain < 0 ) { // expext only gains in TileCal range

          unsigned int ene = energy+0x10000;  // shift by 65 pCb (65*10^3 because of scaling)
          unsigned int qua = std::max(0, std::min(0xFF, quality)); // 8 bits for quality
          unsigned int gai = m_gainIndex[-gain];
          unsigned int gqe = (gai << 28) | (qua<<20) | ene; // upper most bit is always 1 here
          vecCell->push_back(gqe);

          if (lVerbose)
            log << MSG::VERBOSE << "packing cell " << ind << " in one word "
                << MSG::hex << gqe << MSG::dec << endmsg;

        } else { // cells with time, use 2 words for channel
                 // but make sure that upper most bit in energy word is zero

          unsigned int ene = std::max(0, std::min(0x7FFFFFFF, 0x40000000 + energy));
          unsigned int tim = std::max(0, std::min(0xFFFF, 0x8000 + time));
          unsigned int qua = std::max(0, std::min(0xFF, quality)); // 8 bits for quality
          unsigned int gai = std::max(0, std::min(0xFF,   0x80   + gain));
          unsigned int tqg = (tim<<16) | (qua<<8) | gai;
          vecCell->push_back(ene);
          vecCell->push_back(tqg);

          if (lVerbose)
            log << MSG::VERBOSE << "packing cell " << ind << " in two words "
                << MSG::hex << ene << " " << tqg << MSG::dec << endmsg;
        }
      }

     } else {

       (*vecCell)[0] = 1; // no MBTS found - use version 1 for packing
     }

      // keep all other cells (if any) with identifiers, 3 words per cell
      for (unsigned int ind=NCELLMBTS; ind<allCells.size(); ++ind) {
        
        const TileCell* cell = allCells[ind];

        if (lVerbose)
          log << MSG::VERBOSE 
              << "ind="  << ind
              << " ene=" << cell->energy()
              << " time=" << cell->time() 
              << " qual=" << (int)cell->qual1()
              << " gain=" << (int)cell->gain()
              << endmsg;

        unsigned int id  = cell->ID().get_identifier32().get_compact();
        int          ene = round32(cell->energy() * 1000.);
        unsigned int tim = 0x8000 + round16(cell->time()*100.);
        unsigned int qua = std::max(0, std::min(0xFF, (int)cell->qual1()));
        unsigned int gai = std::max(0, std::min(0xFF,   0x80   + (int)(cell->gain())));
        unsigned int tqg = (tim<<16) | (qua<<8) | gai;
        vecCell->push_back(id);
        vecCell->push_back((unsigned int)ene);
        vecCell->push_back(tqg);

        if (lVerbose)
          log << MSG::VERBOSE << "packing cell " << ind << " in three words "
              << MSG::hex << id << " " << ene << " " << tqg << MSG::dec << endmsg;
      }
      break;

    default:

      log << MSG::ERROR << "Unknown version of TileCellVec, ver="<<m_version << endmsg;

    }

    if (lDebug) log << MSG::DEBUG << "Storing data vector of size " << vecCell->size() << " with version " << vecCell->front() << endmsg;

    return vecCell.release();
}

TileCellContainer* TileCellContainerCnv::createTransient()
{
    // Fill TileCellContainer from vector, creating cells from 3 integers 

    MsgStream log(msgSvc(), "TileCellContainerCnv");
    MSG::Level logLevel = log.level();
    bool lDebug = (logLevel<=MSG::DEBUG);
    bool lVerbose = (logLevel<=MSG::VERBOSE);

    std::unique_ptr<TileCellVec> vec(this->poolReadObject<TileCellVec>());

    if (lDebug) log << MSG::DEBUG << "Read TileCell Vec, size " << vec->size() << endmsg;

    // create the TileCellContainer
    auto cont = std::make_unique<TileCellContainer>();

    TileCellVec::const_iterator it   = vec->begin();
    TileCellVec::const_iterator last = vec->end();

    unsigned int version = *it++;
    int iCell = 0;
    
    switch (version) {
    case 1:

      for (; it != last; ) {

        Identifier id(Identifier32(*it++));
        int ene = (int)(*it++);
        unsigned int tqg = *it++;
      
        float ener = ene*1e-3;
        float time = ((int)(tqg>>16) - 0x8000 ) * 0.01;
        uint16_t qual = ((tqg>>8) & 0xFF);
        uint16_t qbit = TileCell::MASK_CMPC | TileCell::MASK_TIME;
        int   gain = (int)(tqg & 0xFF) - 0x80;

        if (lVerbose) {
          log << MSG::VERBOSE << "reading cell " << (iCell++) << " "
              << MSG::hex << id << MSG::dec << " " << ene << " " 
              << MSG::hex << tqg << MSG::dec << endmsg;
          log << MSG::VERBOSE << "ene=" << ener << " time=" << time 
              << " qual=" << qual << " gain=" << gain << endmsg;
        }
        
        TileCell * cell = new TileCell(NULL,id,ener,time,qual,qbit,(CaloGain::CaloGain)gain);
        cont->push_back(cell);
      }
      break;

    case 2:
      
      for (; it != last; ) {

        Identifier id;
        CaloDetDescrElement * dde = NULL;
        float ener = 0.0;
        float time = 0.0;
        uint16_t qual = 0;
        uint16_t qbit = TileCell::MASK_CMPC | TileCell::MASK_TIME;
        int gain = m_gain[0]; // non-existing gain in CaloGain - to mark non-existing cells
        
        if (lVerbose)
          log << MSG::VERBOSE << "reading cell " << iCell << " ";

        if (iCell < NCELLMBTS) { // first 32 cells are MBTS cells without identifier

          id = m_id[iCell]; // identifier is taken from array
          dde = m_dde[iCell]; // mbtsDDE is taken from array

          int ene = (int)(*it++); // first word is energy

          if (lVerbose)
            log << MSG::hex << id << " " << ene << " " << MSG::dec;

          if (ene < 0 ) { // upper most bit is set, it means that everything is packed in one word

            if (lVerbose)
              log << endmsg;

            time = 0.0;   // time was zero and it was not saved
            ener = ((ene & 0xFFFFF) - 0x10000) * 1e-3;
            qual = ((ene>>20) & 0xFF);
            gain = m_gain[((ene>>28) & 0x7)]; // gain is taken from array
            
          } else { // two words packing
          
            unsigned int tqg = *it++;
            if (lVerbose)
              log << MSG::hex << tqg << MSG::dec << endmsg;

            ener = (ene - 0x40000000) * 1e-3;
            time = ((int)(tqg>>16) - 0x8000 ) * 0.01;
            qual = ((tqg>>8) & 0xFF);
            gain = (int)(tqg & 0xFF) - 0x80;
          }

        } else { // three words packing for remaining cells

          id = Identifier(Identifier32(*it++));
          int ene = (int)(*it++);
          unsigned int tqg = *it++;
      
          if (lVerbose)
            log << MSG::hex << id << MSG::dec << " " << ene << " " 
                << MSG::hex << tqg << MSG::dec << endmsg;

          ener = ene*1e-3;
          time = ((int)(tqg>>16) - 0x8000 ) * 0.01;
          qual = ((tqg>>8) & 0xFF);
          gain = (int)(tqg & 0xFF) - 0x80;
        }

        if (lVerbose)
          log << MSG::VERBOSE << "ene=" << ener << " time=" << time 
              << " qual=" << qual << " gain=" << gain << endmsg;

        if (gain != m_gain[0]) { // don't create cells with non-existing gain
          TileCell * cell = new TileCell(dde,id,ener,time,qual,qbit,(CaloGain::CaloGain)gain);
          cont->push_back(cell);
        }
        else {
          if (lVerbose)
            log << MSG::VERBOSE << "Don't create MBTS cell with invalid gain" << endmsg;
        }
        ++iCell;
      }
      break;
      
    default:

      log << MSG::ERROR << "Unknown version of TileCellVec, ver="<<version << endmsg;
    }
    
    return cont.release();
}

