/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi includes
#include "GaudiKernel/Service.h"
#include "GaudiKernel/Property.h"

// Atlas includes
#include "AthenaKernel/errorcheck.h"

// Calo includes
#include "CaloIdentifier/TileID.h"
#include "CaloEvent/CaloCellContainer.h"

// Tile includes
#include "TileRecUtils/TileCellMaskingTool.h"
#include "TileIdentifier/TileHWID.h"
#include "TileEvent/TileCell.h"

#include <CLHEP/Units/SystemOfUnits.h>

using CLHEP::MeV;

TileCellMaskingTool::TileCellMaskingTool(const std::string& type
    , const std::string& name, const IInterface* parent)
  : AthAlgTool(type, name, parent)
  , m_tileID(0)
  , m_tileHWID(0)
{
  declareInterface<ICaloCellMakerTool>( this );

  declareProperty("RejectedTileDrawer", m_rejectedTileDrawer) ;
  declareProperty("RejectedTileMB", m_rejectedTileMB) ;
  declareProperty("RejectedTileDigitizer", m_rejectedTileDigitizer) ;
  declareProperty("RejectedTileDMU", m_rejectedTileDMU) ;
  declareProperty("RejectedTileChannels", m_rejectedTileChannels) ;

  declareProperty("BadChannelZeroEnergy",m_zeroEnergy = 0.5 * MeV); // half a MeV in both PMTs i.e. one MeV in a cell
}

///////////////////////////////////////////////////////////////////////////////

StatusCode TileCellMaskingTool::initialize() {

  ATH_MSG_DEBUG( " TileCellMaskingTool called " );

  CHECK( detStore() -> retrieve(m_tileID, "TileID") );
  CHECK( detStore() -> retrieve(m_tileHWID, "TileHWID") );

  CHECK( fillIncludedCellsMap() );
  ATH_MSG_DEBUG( " Will exclude "
                << m_includedCellsMap.size() - m_includedCellsMap.count()
                << " channels " );

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////

StatusCode TileCellMaskingTool::fillIncludedCellsMap() {

  m_includedCellsMap.set();

  int ros = 0, drw = 0, index = -1;

  std::vector<std::string>::const_iterator it_dr = m_rejectedTileDrawer.begin();
  std::vector<std::string>::const_iterator it_dr_e = m_rejectedTileDrawer.end();
  for (; it_dr != it_dr_e; it_dr++) {
    std::stringstream dris;
    dris << (*it_dr);
    dris >> ros >> drw;
    killer("drawer", ros, drw, index);
  }

  std::vector<std::string>::const_iterator it_mb = m_rejectedTileMB.begin();
  std::vector<std::string>::const_iterator it_mb_e = m_rejectedTileMB.end();
  for (; it_mb != it_mb_e; it_mb++) {
    std::stringstream dris;
    dris << (*it_mb);
    dris >> ros >> drw >> index;
    killer("mb", ros, drw, index);
  }

  std::vector<std::string>::const_iterator it_dig = m_rejectedTileDigitizer.begin();
  std::vector<std::string>::const_iterator it_dig_e = m_rejectedTileDigitizer.end();
  for (; it_dig != it_dig_e; it_dig++) {
    std::stringstream dris;
    dris << (*it_dig);
    dris >> ros >> drw >> index;
    killer("dig", ros, drw, index);
  }

  std::vector<std::string>::const_iterator it_dmu = m_rejectedTileDMU.begin();
  std::vector<std::string>::const_iterator it_dmu_e = m_rejectedTileDMU.end();
  for (; it_dmu != it_dmu_e; it_dmu++) {
    std::stringstream dris;
    dris << (*it_dmu);
    dris >> ros >> drw >> index;
    killer("dmu", ros, drw, index);
  }

  std::vector<std::string>::const_iterator it = m_rejectedTileChannels.begin();
  std::vector<std::string>::const_iterator it_e = m_rejectedTileChannels.end();
  for (; it != it_e; it++) {
    std::stringstream dris;
    dris << (*it);
    dris >> ros >> drw >> index;
    killer("channel", ros, drw, index);
  }

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////

void TileCellMaskingTool::killer(std::string component, int ros, int drw, int index) {

  int begin = 0, end = 0;

  if (component == "drawer") {
    begin = 0;
    end = 48;
  } else if (component == "mb") {
    begin = (4 - index) * 12;
    end = begin + 12;
  } else if (component == "dig") {
    begin = (8 - index) * 6;
    end = begin + 6;
  } else if (component == "dmu") {
    begin = index * 3;
    end = begin + 3;
  } else {
    begin = index;
    end = begin + 1;
  }

  if (ros < 1 || ros > 4) {
    ATH_MSG_WARNING( " wrong selection of ros = " << ros << " in TileCellMaskingTool::killer ");
  } else if (drw < 0 || drw > 63) {
    ATH_MSG_WARNING( " wrong selection of drawer = " << drw << " in TileCellMaskingTool::killer ");
  } else if (begin < 0 || end > 48) {
    ATH_MSG_WARNING( " wrong selection of "<< component
                    << " = " << index << " in TileCellMaskingTool::killer ");
  } else {
    msg(MSG::INFO) << " killing ros " << ros << " drawer " << drw;
    if (component != "drawer") {
      msg(MSG::INFO) << component << " " << index << endmsg;
    } else {
      msg(MSG::INFO) << endmsg;
    }

    for (int channel = begin; channel < end; ++channel) {
      HWIdentifier chid = m_tileHWID->channel_id(ros, drw, channel);
      IdentifierHash hash = m_tileHWID->get_channel_hash(chid);
      m_includedCellsMap.reset(hash);
      ATH_MSG_DEBUG( "deleting channel " << m_tileHWID->to_string(chid,-1)
                    << " hash " << hash );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////

StatusCode TileCellMaskingTool::process (CaloCellContainer* theCont,
                                         const EventContext& /*ctx*/) const
{
  int n_cells = 0, n_masked_1 = 0, n_masked_2 = 0, n_masked_12 = 0;
  double ene_before = 0.0, ene_after = 0.0;

  size_t firstCell = theCont->indexFirstCellCalo(CaloCell_ID::TILE);
  size_t lastCell = theCont->indexLastCellCalo(CaloCell_ID::TILE);

  for (size_t iCell = firstCell; iCell <= lastCell; ++iCell) {

    CaloCell* aCell = (*theCont)[iCell];
    const CaloDetDescrElement * caloDDE = aCell->caloDDE();

    if (caloDDE->is_tile()) { // check if cell belongs to TileCalorimeter

      if (msgLvl(MSG::DEBUG)) {
        ++n_cells;
        ene_before += aCell->energy();
      }

      IdentifierHash hash1 = caloDDE->onl1();
      IdentifierHash hash2 = caloDDE->onl2();

      bool bit1 = m_includedCellsMap.test(hash1); // true - good channel
      bool bit2 = m_includedCellsMap.test(hash2); // false - channel should be masked

      TileCell* pCell = (TileCell*) aCell;
      int gain1 = pCell->gain1();
      int gain2 = pCell->gain2();

      if (bit1) { // first is good

        if (!bit2) { // second is bad

          if (msgLvl(MSG::DEBUG)) {
            ++n_masked_12;
            if (msgLvl(MSG::VERBOSE)) {
              msg(MSG::VERBOSE) << " second channel is OFF"
                                << ", hash2: " << hash2
                                << "  before " << pCell->ene1()
                                << " + " << pCell->ene2()
                                << " = " << pCell->energy();
            }
          }

          float ene1 = pCell->ene1();
          pCell->setEnergy(ene1, ene1, gain1, gain1); // use energy/gain from first pmt for both pmts
          pCell->setTime(pCell->time1());  // use time from first pmt as cell time
          pCell->setQuality(pCell->qual1(), (pCell->qbit1() | TileCell::MASK_BADCH), 1); // change quality flag for second pmt

          if (msgLvl(MSG::VERBOSE)) {
            msg(MSG::VERBOSE) << "  after " << pCell -> ene1()
                              << " + " << pCell -> ene2()
                              << " = " << pCell -> energy() << endmsg;
          }
        }
      } else { // first is bad

        if (hash2 == TileHWID::NOT_VALID_HASH) { // cells wih single PMT

          if (msgLvl(MSG::DEBUG)) {
            ++n_masked_1;
            if (msgLvl(MSG::VERBOSE)) {
              msg(MSG::VERBOSE) << " channel in GAP is OFF"
                                << ", hash1: " << hash1
                                << "  before " << pCell->ene1()
                                << " + " << pCell->ene2()
                                << " = " << pCell->energy();
            }
          }

          if (gain1 == CaloGain::INVALIDGAIN) {
            pCell->setEnergy(m_zeroEnergy, 0.0, TileID::LOWGAIN, CaloGain::INVALIDGAIN); // reset energy completely, indicate problem putting low gain
          } else {
            pCell->setEnergy(m_zeroEnergy, 0.0); // reset energy completely without changing gain
          }
          pCell->setTime(0.0); // reset time completely
          pCell->setQuality(255, TileCell::MASK_BADCH, 0); // reset quality flag for first pmt
          pCell->setQuality(0, TileCell::MASK_BADCH, 1); // reset quality flag for second pmt

          if (msgLvl(MSG::VERBOSE)) {
            msg(MSG::VERBOSE) << "  after " << pCell->ene1()
                              << " + " << pCell->ene2()
                              << " = " << pCell->energy() << endmsg;
          }

        } else if (bit2) { // second is good

          if (msgLvl(MSG::DEBUG)) {
            ++n_masked_12;
            if (msgLvl(MSG::VERBOSE)) {
              msg(MSG::VERBOSE) << " first  channel is OFF"
                                << ", hash1: " << hash1
                                << "  before " << pCell->ene1()
                                << " + " << pCell->ene2()
                                << " = " << pCell->energy();
            }
          }

          float ene2 = pCell->ene2();
          pCell->setEnergy(ene2, ene2, gain2, gain2); // use energy/gain from second pmt for both pmts
          pCell->setTime(pCell->time2());  // use time from second pmt as cell time
          pCell->setQuality(pCell->qual2(), (pCell->qbit2() | TileCell::MASK_BADCH), 0); // change quality flag for first pmt

          if (msgLvl(MSG::VERBOSE))
            msg(MSG::VERBOSE) << "  after " << pCell->ene1()
                              << " + " << pCell->ene2()
                              << " = " << pCell->energy() << endmsg;

        } else { // second is bad

          if (msgLvl(MSG::DEBUG)) {
            ++n_masked_2;
            if (msgLvl(MSG::VERBOSE)) {
              msg(MSG::VERBOSE) << " both channels are OFF"
                                << ", hash1: " << hash1
                                << ", hash2: " << hash2
                                << "  before " << pCell->ene1()
                                << " + " << pCell->ene2()
                                << " = " << pCell->energy();
            }
          }

          if (gain1 == CaloGain::INVALIDGAIN || gain2 == CaloGain::INVALIDGAIN) {
            if (gain1 == CaloGain::INVALIDGAIN) gain1 = 0; // this is TileID::LOWGAIN; - commented out to make Coverity happy
            if (gain2 == CaloGain::INVALIDGAIN) gain2 = 0; // this is TileID::LOWGAIN; - commented out to make Coverity happy
            pCell->setEnergy(m_zeroEnergy, m_zeroEnergy, gain1, gain2); // reset energy completely, indicate problem putting low gain
          } else {
            pCell->setEnergy(m_zeroEnergy, m_zeroEnergy); // reset energy completely without changing gain
          }
          pCell->setTime(0.0);  // reset time completely
          pCell->setQuality(255, TileCell::MASK_BADCH, 0); // reset quality flag for first pmt
          pCell->setQuality(255, TileCell::MASK_BADCH, 1); // reset quality flag for second pmt

          if (msgLvl(MSG::VERBOSE)) {
            msg(MSG::VERBOSE) << "  after " << pCell->ene1()
                              << " + " << pCell->ene2()
                              << " = " << pCell->energy() << endmsg;
          }
        }
      }
      ene_after += aCell->energy();
    }
  }

  ATH_MSG_DEBUG( " Ncells: " << n_cells
                << " N masked gap/normal/half-masked: " << n_masked_1 << " " << n_masked_2 << " " << n_masked_12
                << " Ene before/after/delta: " << ene_before << " " << ene_after << " " << ene_after-ene_before );

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////

StatusCode TileCellMaskingTool::finalize() {

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////

bool TileCellMaskingTool::channel_is_good(HWIdentifier &hwid) {

  IdentifierHash ChHash = m_tileHWID->get_channel_hash(m_tileHWID->channel_id(hwid));

  return m_includedCellsMap.test(ChHash);
}
