/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileConditions/TileCablingService.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/MbtsDetDescrManager.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileEvent/TileCellCollection.h"
#include "TileEvent/TileCell.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

#include "TileByteStream/TileHid2RESrcID.h"
#include "TileByteStream/TileCellCont.h"

#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ToolHandle.h"

#include <iostream>
#include <algorithm>

typedef std::pair<int, int> int_pair;

bool sort_pred(const int_pair& left, const int_pair& right) {
  return left.second < right.second;
}

TileCellCont::TileCellCont()
  : m_it(0)
  , m_event(0)
  , m_MBTS(0)
  , m_MBTS_channel(0)
  , m_src(0) {
}

StatusCode TileCellCont::initialize() {

#ifndef NDEBUG
  std::cout << "TileCellCont\t\t DEBUG \t constructor" << std::endl;
#endif

  ISvcLocator* svcLoc = Gaudi::svcLocator();

  StoreGateSvc* detStore = 0;
  if (svcLoc->service("DetectorStore", detStore).isFailure()) {
    std::cout << "Could not locate DetectorStore" << std::endl;
    return StatusCode::FAILURE;
  }

  // Get the TileID helper from the detector store
  const TileID* tileID;
  if (detStore->retrieve(tileID, "TileID").isFailure()) {
    std::cout << "Could not get TileID helper !" << std::endl;
    return StatusCode::FAILURE;
  }
  // Get the TileHWID helper from the detector store
  const TileHWID* tileHWID;
  if (detStore->retrieve(tileHWID, "TileHWID").isFailure()) {
    std::cout << "Could not get TileHWID helper !" << std::endl;
    return StatusCode::FAILURE;
  }

  // Get pointer to TileDetDescrManager
  const TileDetDescrManager* tileMgr;
  if (detStore->retrieve(tileMgr).isFailure()) {
    std::cout << "Unable to retrieve TileDetDescrManager from DetectorStore" << std::endl;
    return StatusCode::FAILURE;
  }

  // Get pointer to MbtsDetDescrManager
  const MbtsDetDescrManager* mbtsMgr;
  if (detStore->retrieve(mbtsMgr).isFailure()) {
    std::cout << "Warning: unable to retrieve MbtsDetDescrManager from DetectorStore" << std::endl;
    mbtsMgr = 0;
  }

  ToolHandle<ITileBadChanTool> badChanTool("TileBadChanTool");
  if (badChanTool.retrieve().isFailure()) {
    std::cout << "TileCellCont:initialize ERROR: Can not retrieve TileBadChanTool" << std::endl;
    return StatusCode::FAILURE;
  }

  if ((badChanTool->initialize()).isFailure()) {
    std::cout << "TileCellCont:initialize ERROR: Can not initialize TileBadChanTool" << std::endl;
    return StatusCode::FAILURE;
  }
  if ( !m_src ){ // if nothing set, use 2017
       std::cout << "TileCellCont::initialize ERROR : TileHid2RESrc has to be initialized before this" << std::endl;
  }

  // Get pointer to TileCablingService
  TileCablingService* cabling = TileCablingService::getInstance();
  int maxChannels = cabling->getMaxChannels();

  //m_hash.initialize(0);
  m_mbts_rods.clear();
  m_mbts_IDs.clear();
  int mbts_count = 0;
  int ID_of_Col = 0;
  std::vector<int> rodids;
  m_MBTS = new TileCellCollection(ID_of_Col, SG::OWN_ELEMENTS);
  for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
      int frag = tileHWID->frag(ros, drawer);
      int rodid = m_src->getRodID(frag);
      rodids.push_back(rodid);
      m_mapMBTS[frag] = 0xFFFF;
      // One event number per collection
      m_eventNumber.push_back(0xFFFFFFFF);

      // Tries to find the TileRawChannel -> TileCell correpondence
      int index, pmt, cell_hash;
      std::vector<int> Rw2Pmt;
      Rw2Pmt.resize(maxChannels, -1);
      std::vector<int> Rw2Cell;
      Rw2Cell.resize(maxChannels, -1);
      std::vector<int_pair> tmp;

      bool one_good = false;
      for (int channel = 0; channel < maxChannels; ++channel) {
        HWIdentifier channelID = tileHWID->channel_id(ros, drawer, channel);
        Identifier cell_id = cabling->h2s_cell_id_index(channelID, index, pmt);
        if (index == -2) { // MBTS cell, only one per drawer
          m_mbts_rods.push_back(m_src->getRodID(frag));
          m_mbts_IDs.push_back(((ros - 1) * TileCalibUtils::MAX_DRAWER + drawer));
          CaloDetDescrElement * caloDDE = (mbtsMgr) ? mbtsMgr->get_element(cell_id) : NULL;
          TileCell* myMBTSCell = new TileCell(caloDDE, cell_id, 0.0, 0.0, 0, 0, CaloGain::TILEONELOW);
          m_MBTS->push_back(myMBTSCell);
          m_mapMBTS[frag] = mbts_count;
          mbts_count++;
          m_MBTS_channel = channel;
        } else if (index >= 0) { // normal cell
          one_good = one_good || (!badChanTool->getChannelStatus(channelID).isBad());
          Rw2Pmt[channel] = pmt;
          if (channel > 0 || ros != 2) { // ignoring D0 (first channel) in negative barrel 
            cell_hash = tileID->cell_hash(cell_id);
            tmp.push_back(int_pair(channel, cell_hash));
          }
        }
      } // End of for over TileRawChannel


      // create new cell collection which will own all elements
      TileCellCollection* newColl = new TileCellCollection(frag, SG::OWN_ELEMENTS);
      this->push_back(newColl);
      if (!one_good) m_masked.push_back(frag);

      // sort index according to cell hash and put it in Rw2Cell vector
      // create TileCells in appropriate order and put them in newColl
      if (tmp.size() > 0) {
        std::sort(tmp.begin(), tmp.end(), sort_pred); // sort according to cell hash index
        index = -1;
        cell_hash = -1;
        for (unsigned int i = 0; i < tmp.size(); ++i) {
          if (cell_hash != tmp[i].second) {
            cell_hash = tmp[i].second;
            ++index;
            CaloDetDescrElement * caloDDE = tileMgr->get_cell_element((IdentifierHash) cell_hash);
            TileCell * pCell = new TileCell(caloDDE, 0.0, 0.0, 0, 0, CaloGain::TILELOWLOW);
            newColl->push_back(pCell);
          }
          Rw2Cell[tmp[i].first] = index;
        }
      }

      if (drawer == 0 || ros == 3 || ros == 4) {
#ifndef NDEBUG
        int idxraw = 0;
        for (std::vector<int>::iterator i = Rw2Cell.begin(); i != Rw2Cell.end(); ++i) {
          if ((*i) != -1) std::cout << "Channel : " << idxraw++ << " connected to cell " << (*i) << std::endl;
        }
#endif
        // One needs to keep track of Rw2Cell
        for (std::vector<int>::iterator i = Rw2Cell.begin(); i != Rw2Cell.end(); ++i)
          m_Rw2Cell[ros - 1].push_back(*i);
        for (std::vector<int>::iterator i = Rw2Pmt.begin(); i != Rw2Pmt.end(); ++i)
          m_Rw2Pmt[ros - 1].push_back(*i);
      } // End of if first drawer of Barrel or Ext

    } // end of drawer for
  } // end of ros for
#ifndef NDEBUG
  std::cout << "Number of RODs is : " << m_mbts_rods.size() << std::endl;
  for (unsigned int k = 0; k < m_mbts_rods.size(); k++)
    std::cout << " MBTS RODs : " << m_mbts_rods[k] << std::endl;
#endif
  m_hash.initialize(0,rodids);

#ifndef NDEBUG
  for (int i = 0; i < m_hash.max(); ++i) {
    TileCellCollection *aa = this->at(i);
    TileCell* bb = aa->at(0);
    std::cout << "TileCellCont\t\t DEBUG \t" << i << " " << std::hex << m_hash.identifier(i) << std::dec << " " << aa->identify() << " " << bb->eta() << " " <<  bb->phi() << std::endl;
    // A collection per ROD/ROB/HashId
  } // end of for id
#endif
  m_it = new std::vector<TileCellCollection*>::const_iterator();

  return StatusCode::SUCCESS;
}

// This WILL NOT trigger BSCNV. This assumes BSCNV was done before
const std::vector<TileCellCollection*>::const_iterator&
TileCellCont::find(const unsigned int& rodid) const {
  *m_it = (std::vector<TileCellCollection*>::const_iterator) ((*this).begin() + rodid);
  return *m_it;
}

StatusCode TileCellCont::finalize() {

  // delete the pointer to collection
  delete m_it;

  // Delete m_RwCells
  for (int i = 0; i < 4; i++)
    m_Rw2Cell[i].clear();
  for (int i = 0; i < 4; i++)
    m_Rw2Pmt[i].clear();

  // Delete Collections and cells
  for (unsigned int i = 0; i < this->size(); i++) {
    // Delete collections. Own cells -> also destroyed
    delete ((TileCellCollection*) ((*this)[i]));
  }
  // Destroy also MBTS collection
  delete m_MBTS;
  m_mapMBTS.clear();
  m_mbts_rods.clear();
  m_mbts_IDs.clear();
  m_masked.clear();
  this->clear();
  return StatusCode::SUCCESS;

}

unsigned int TileCellCont::find_rod(const unsigned int& rodid) const {
  unsigned int rodidx = m_hash.identifier(rodid);
  return rodidx;
}

