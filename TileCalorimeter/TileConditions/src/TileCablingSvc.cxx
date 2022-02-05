/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi includes
#include "StoreGate/StoreGateSvc.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

// Calo includes
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/TileTBID.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

// Tile includes
#include "TileConditions/TileCablingSvc.h"
#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileDetDescr/TileDetDescrManager.h"


#include <cstdlib>


//
//____________________________________________________________________
static const InterfaceID IID_TileCablingSvc("TileCablingSvc", 1, 0);
const InterfaceID& TileCablingSvc::interfaceID() {
  return IID_TileCablingSvc;
}

//
//_____________________________________________________________________________
TileCablingSvc::TileCablingSvc(const std::string& name, ISvcLocator* pSvcLocator)
    : AthService(name, pSvcLocator)
  , m_cablingService(0)
  , m_detStore("DetectorStore", name)
{
  declareProperty("ConnectedDrawers", m_connectedDrawers, "List of connected drawer ranges: starts,end1,start2,end2,...");
  declareProperty("CablingType", m_cablingType = -9, "Cabling type: -1 - testbeam, 0 - old simulation, 1 - without MBTS, 2 - with MBTS instead of crack scin, 3 - MBTS in spare channels, 4 - RUN2 cabling, 5 - RUN2a cabling");
  declareProperty("UseCache", m_useCache = true, "Use cache for channel_id to cell_id conversion");
}

//
//_____________________________________________________________________________
StatusCode TileCablingSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (TileCablingSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = dynamic_cast<TileCablingSvc*>(this);
  } else {
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

//
//_____________________________________________________________________________
StatusCode TileCablingSvc::initialize ATLAS_NOT_THREAD_SAFE () {

  ATH_MSG_DEBUG( "In initialize() " );

  //=== creating cabling singleton
  TileCablingService* cablingService = TileCablingService::getInstance_nc();
  m_cablingService = cablingService;
  if (!m_cablingService) {
    ATH_MSG_ERROR( "Cannot get instance of TileCablingService" );
    return StatusCode::FAILURE;
  }

  //=== 
  CHECK( m_detStore.retrieve() );

  //=== retrieve all helpers from detector store
  const CaloLVL1_ID* caloID(0);
  CHECK( m_detStore->retrieve(caloID) );

  const TileID* tileID(0);
  CHECK( m_detStore->retrieve(tileID) );

  const TileTBID* tileTBID(0);
  CHECK( m_detStore->retrieve(tileTBID) );

  const TileHWID* tileHWID(0);
  CHECK( m_detStore->retrieve(tileHWID) );

  //=== Initialize TileCablingService singleton
  cablingService->setCaloLVL1(caloID);
  cablingService->setTileID(tileID);
  cablingService->setTileTBID(tileTBID);
  cablingService->setTileHWID(tileHWID);

  //=== set connected drawers if non-empty list
  if (m_connectedDrawers.size() > 1) {
    //===disconnect all drawers first
    for (int ros = 1; ros < 5; ++ros) {
      for (int drawer = 0; drawer < 64; ++drawer) {
        cablingService->setConnected(ros, drawer, false);
      }
    }
    int count = 0;

    msg(MSG::INFO) << "Connected drawer list:" << MSG::hex;
    for (unsigned int dr = 1; dr < m_connectedDrawers.size(); dr += 2) {
      int frag1 = std::strtol(m_connectedDrawers[dr - 1].data(), NULL, 0);
      int frag2 = std::strtol(m_connectedDrawers[dr].data(), NULL, 0);
      for (int frag = frag1; frag <= frag2; ++frag) {
        int ros = frag >> 8;
        int drawer = frag & 0xFF;
        if (ros >= 1 && ros < 5 && drawer >= 0 && drawer < 64) {
          cablingService->setConnected(ros, drawer, true);
          msg(MSG::INFO) << " 0x" << frag;
          char module[3];
          sprintf(module, "%2.2d", drawer + 1);
          switch (ros) {
            case 1:
              msg(MSG::INFO) << "/LBA" << module;
              break;
            case 2:
              msg(MSG::INFO) << "/LBC" << module;
              break;
            case 3:
              msg(MSG::INFO) << "/EBA" << module;
              break;
            case 4:
              msg(MSG::INFO) << "/EBC" << module;
              break;
            default:
              msg(MSG::INFO) << "/XXX" << module;
              break;
          }
          if (++count > 3) {
            count = 0;
            msg(MSG::INFO) << endmsg;
            msg(MSG::INFO) << "                      " << MSG::hex;
          }
        }
      }
    }
    msg(MSG::INFO) << MSG::dec << endmsg;
  }

  const IGeoModelSvc* geoModel = 0;
  StatusCode sc = service("GeoModelSvc", geoModel);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not locate GeoModelSvc"  );
  } else {
    // check the DetDescr version
    std::string atlasVersion = geoModel->atlasVersion();
    int ctb = atlasVersion.compare(0, 9, "ATLAS-CTB");
    int geo  = atlasVersion.compare(0,9,"ATLAS-GEO");
    int run1 = atlasVersion.compare(0,8,"ATLAS-R1");
    int ibl  = atlasVersion.compare(0,9,"ATLAS-IBL");
    int run2 = atlasVersion.compare(0,8,"ATLAS-R2");
    int run3 = atlasVersion.compare(0,8,"ATLAS-R3");
    int upg  = atlasVersion.compare(0,7,"ATLAS-P") ;
    int comm = atlasVersion.compare(0,10,"ATLAS-Comm");

    bool upgradeA = (tileID->cell_hash_max() == MAX_TILE_CELLS_UPGRADEA);
    bool upgradeBC = (tileID->cell_hash_max() == MAX_TILE_CELLS_UPGRADEBC);
    bool upgradeABC = (tileID->cell_hash_max() == MAX_TILE_CELLS_UPGRADEABC);

    // choose which geometries are true RUN2 geometries to apply run2 cabling
    bool nothing_found = (ctb*geo*run1*ibl*run2*upg*comm != 0);
    GeoModel::GeoConfig geoConfig = geoModel->geoConfig();
    bool RUN2 = (nothing_found && geoConfig==GeoModel::GEO_RUN2) || (run2 == 0);
    bool RUN3 = (nothing_found && geoConfig==GeoModel::GEO_RUN3) || (run3 == 0);
    bool RUN4 = (nothing_found && geoConfig==GeoModel::GEO_RUN4);
    //|| (ibl == 0 || upg == 0);

    if (RUN2) {

      ATH_MSG_INFO( "RUN2 ATLAS geometry flag detected for geometry: " << atlasVersion );
      if (upgradeA) {
        ATH_MSG_INFO( "RUN2 ATLAS UpgradeA geometry flag detected for geometry: " << atlasVersion );
        m_cablingType = TileCablingService::UpgradeA;
      } else if (upgradeBC) {
        ATH_MSG_INFO( "RUN2 ATLAS UpgradeBC geometry flag detected for geometry: " << atlasVersion );
        m_cablingType = TileCablingService::UpgradeBC;
      } else if (upgradeABC) {
        ATH_MSG_INFO( "RUN2 ATLAS UpgradeABC geometry flag detected for geometry: " << atlasVersion );
        m_cablingType = TileCablingService::UpgradeABC;
      } else {
        if (m_cablingType == TileCablingService::RUN2Cabling) {
          ATH_MSG_INFO( "Cabling for RUN2 (2014-2017) ATLAS geometry is set via jobOptions " );
        } else if (m_cablingType == TileCablingService::RUN2aCabling) {
          ATH_MSG_INFO( "Cabling for RUN2a (2018) ATLAS geometry is set via jobOptions " );
        } else if (m_cablingType < TileCablingService::RUN2Cabling) {
          ATH_MSG_INFO( "Setting RUN2 (2014-2017) cabling" );
          m_cablingType = TileCablingService::RUN2Cabling;
        } else {
          ATH_MSG_INFO( "Using cabling type " << m_cablingType << " from jobOptions " );
        }
      }

    } else if (RUN3) {

      ATH_MSG_INFO( "RUN3 ATLAS geometry flag detected for geometry: " << atlasVersion );
      if (m_cablingType == TileCablingService::RUN3Cabling) {
        ATH_MSG_INFO( "Cabling for RUN3 ATLAS geometry is set via jobOptions " );
      } else if ( m_cablingType < TileCablingService::RUN2Cabling) {
        ATH_MSG_INFO( "Setting RUN3 cabling" );
        m_cablingType = TileCablingService::RUN3Cabling;
      } else {
        ATH_MSG_INFO( "Using cabling type " << m_cablingType << " from jobOptions " );
      }

    } else if (RUN4) {
      ATH_MSG_INFO( "RUN4 ATLAS geometry detected - use RUN1 cabling: " << atlasVersion );
      m_cablingType = TileCablingService::MBTSOnly;
    } else if (m_cablingType == TileCablingService::RUN3Cabling) {
      ATH_MSG_INFO( "Cabling for RUN3 ATLAS geometry is set via jobOptions " );
    } else if (m_cablingType == TileCablingService::RUN2Cabling) {
      ATH_MSG_INFO( "Cabling for RUN2 (2014-2017) ATLAS geometry is set via jobOptions " );
    } else if (m_cablingType == TileCablingService::RUN2aCabling) {
      ATH_MSG_INFO( "Cabling for RUN2a (2018) ATLAS geometry is set via jobOptions " );
    }  else if (ctb == 0) {
      ATH_MSG_INFO( "CTB geometry detected: " << atlasVersion );
      m_cablingType = TileCablingService::TestBeam;
    } else if (geo == 0 || run1 == 0 || ibl == 0 || run2 == 0 || upg == 0) {
      ATH_MSG_INFO( "RUN1 ATLAS geometry detected: " << atlasVersion );
      m_cablingType = TileCablingService::MBTSOnly;
    } else if (comm == 0) {
      ATH_MSG_INFO( "RUN1 ATLAS Commissioning geometry detected: " << atlasVersion );
      m_cablingType = TileCablingService::MBTSOnly;
    } else {
      ATH_MSG_WARNING( "Old ATLAS geometry detected - most probably it's a mistake: " << atlasVersion );
      // in this case change cabling type only if it is not set via jobOptions
      if (m_cablingType < -1 || m_cablingType >= TileCablingService::UnknownCabling)
        m_cablingType = TileCablingService::OldSim;
      else
        ATH_MSG_INFO( "Using cabling type from jobOptions " );
    }
  }

  ATH_MSG_INFO( "Setting Cabling type to " << m_cablingType );
  ATH_CHECK( cablingService->setCablingType((TileCablingService::TileCablingType) m_cablingType) );
  ATH_MSG_DEBUG( "Maximum number of gains: " <<  m_cablingService->getMaxChannels());

  // ------------ Setting TileCal channel hashes in CaloDDE -------------
  // ---------- it can be done only after we set cabling type -----------
  const TileDetDescrManager* tileMgr;
  if (m_detStore->retrieve(tileMgr).isFailure()) {
    ATH_MSG_WARNING( "Unable to retrieve TileDetDescrManager from DetectorStore" );
    ATH_MSG_WARNING( "Will not set Tile online hash ID in CaloDetDescrElements" );
  } else {
    ATH_MSG_VERBOSE( "List of disconnected cells" );

    unsigned int nCellMax = tileMgr->tile_cell_size();
    IdContext chContext = tileHWID->channel_context();

    for (unsigned int cell = 0; cell < nCellMax; ++cell) {
      CaloDetDescrElement* caloDDE = tileMgr->get_cell_element(cell);

      if (caloDDE) {
        Identifier cell_id = tileID->cell_id(caloDDE->subcalo_hash());
        int nchan = (tileID->is_tile_gapscin(cell_id)) ? 1 : 2;
        IdentifierHash pmtHash[2] = { TileHWID::NOT_VALID_HASH, TileHWID::NOT_VALID_HASH };

        bool disconnected = true;
        for (int pmt = 0; pmt < nchan; ++pmt) {
          Identifier id1 = tileID->pmt_id(cell_id, pmt);
          HWIdentifier hw1 = m_cablingService->s2h_channel_id(id1);
          tileHWID->get_hash(hw1, pmtHash[pmt], &chContext);
          Identifier id2 = m_cablingService->h2s_pmt_id(hw1);
          disconnected &= (id2 != id1);
        }
        if (disconnected) m_disconnectedCells.push_back(cell_id);

        IdentifierHash oldPmtHash[2] = { caloDDE->onl1(), caloDDE->onl2() };
        if ((oldPmtHash[0] != TileHWID::NOT_VALID_HASH || oldPmtHash[1] != TileHWID::NOT_VALID_HASH)
            && (pmtHash[0] != oldPmtHash[0] || pmtHash[1] != oldPmtHash[1])) {

          ATH_MSG_WARNING( "changing channel hash for cell hash= " << cell
                          << " id= " << tileID->to_string(cell_id, -2)
                          << " from (" << oldPmtHash[0] << "," << oldPmtHash[1]
                          << ") to (" << pmtHash[0] << "," << pmtHash[1] << ")" );

        }

        caloDDE->set_online(pmtHash[0], pmtHash[1]);
        if (msgLvl(MSG::VERBOSE)) {
          if (disconnected) {

            msg(MSG::VERBOSE) << "cell_id " << tileID->to_string(cell_id, -2) << " ch_id";
            if (pmtHash[0] != TileHWID::NOT_VALID_HASH)
              msg(MSG::VERBOSE) << " " << tileHWID->to_string(m_cablingService->s2h_channel_id(tileID->pmt_id(cell_id, 0)), -1);
            else
              msg(MSG::VERBOSE) << " invalid";
            if (nchan > 1) {
              if (pmtHash[1] != TileHWID::NOT_VALID_HASH)
                msg(MSG::VERBOSE) << " " << tileHWID->to_string(m_cablingService->s2h_channel_id(tileID->pmt_id(cell_id, 1)), -1);
              else
                msg(MSG::VERBOSE) << " invalid";
            }
            msg(MSG::VERBOSE) << endmsg;
          }
        }
      }
    }

    ATH_MSG_VERBOSE( "Total: " << m_disconnectedCells.size() << " disconnected cells");
  }

  if (msgLvl(MSG::VERBOSE)) {

    // checking all HWIDs
    std::cout << "===============================" << std::endl;

    for (int ros = 1; ros < 5; ++ros) {
      for (int drawer = 0; drawer < 64; ++drawer) {
        int drIdx1 = TileCalibUtils::getDrawerIdx(ros, drawer);
        for (int channel = 0; channel < 48; ++channel) {

          HWIdentifier hwid1 = tileHWID->channel_id(ros, drawer, channel);
          if (hwid1.is_valid()) {
            std::cout << " hw1=" << tileHWID->to_string(hwid1, -1);
            Identifier id1 = m_cablingService->h2s_pmt_id(hwid1);
            if (id1.is_valid()) {
              std::cout << " id1=" << tileID->to_string(id1, -1);
              HWIdentifier hwid2 = m_cablingService->s2h_channel_id(id1);
              if (hwid2.is_valid()) {
                if (hwid2 != hwid1) std::cout << " hw1!=hw2=" << tileHWID->to_string(hwid2, -1);
                Identifier id2 = m_cablingService->h2s_pmt_id(hwid2);
                if (id2.is_valid()) {
                  if (id2 != id1)
                    std::cout << " id1!=id2=" << tileID->to_string(id2, -1);
                  else if (tileTBID->is_tiletb(id1))
                      std::cout << ((tileTBID->eta(id1)>1) ? " E4'" : " MBTS");
                  else
                    std::cout << " cell_hash " << tileID->cell_hash(tileID->cell_id(id1));
                  int drIdx2 = tileHWID->drawerIdx(hwid1);
                  if (drIdx1 != drIdx2) {
                    std::cout << "  ERROR in drawer index " << drIdx1 << " != " << drIdx2
                        << " end-begin=" << (tileHWID->drawer_end() - tileHWID->drawer_begin())
                        << std::hex << " 0x" << hwid1 << " 0x"
                        << (*(tileHWID->drawer_begin() + drIdx1)) << std::dec;
                  } else {
                    std::cout << "  drawer index " << drIdx2;
                  }
                } else {
                  std::cout << " id2=invalid";
                }
              } else {
                std::cout << " hw2=invalid";
              }
            } else {
              std::cout << " id1=invalid";
            }
          } else {
            std::cout << " hw1=invalid";
          }
          std::cout << std::endl;
        }
      }
    }

    // checking D4 in few modules
    std::cout << "=== D4 cells in special EB ====" << std::endl;

    int d4module[4] = { 14, 15, 18, 19 };
    for (int side = -1; side < 2; side += 2) {
      for (int imod = 0; imod < 4; ++imod) {
        int module = d4module[imod] - 1;
        for (int pmt = 0; pmt < 2; ++pmt) {

          Identifier id1 = tileID->pmt_id(3, side, module, 8, 2, pmt);
          if (id1.is_valid()) {
            std::cout << " id1=" << tileID->to_string(id1, -1);
            HWIdentifier hw1 = m_cablingService->s2h_channel_id(id1);
            if (hw1.is_valid()) {
              std::cout << " hw1=" << tileHWID->to_string(hw1, -1);
              Identifier id2 = m_cablingService->h2s_pmt_id(hw1);
              if (id2.is_valid()) {
                if (id2 != id1) std::cout << " id1!=id2=" << tileID->to_string(id2, -1);
                HWIdentifier hw2 = m_cablingService->s2h_channel_id(id2);
                if (hw2.is_valid()) {
                  if (hw2 != hw1) std::cout << " hwid1!=hwid2=" << tileHWID->to_string(hw2, -1);
                } else {
                  std::cout << " hw2=invalid";
                }
              } else {
                std::cout << " id2=invalid";
              }
            } else {
              std::cout << " hw1=invalid";
            }
          } else {
            std::cout << " id1=invalid";
          }
          std::cout << std::endl;
        }
      }
    }

    // checking C10 in few modules
    std::cout << "=== C10 cells in special EB ===" << std::endl;

    int c10module[8] = { 39, 40, 41, 42, 55, 56, 57, 58 };
    for (int side = -1; side < 2; side += 2) {
      for (int imod = 0; imod < 8; ++imod) {
        int module = c10module[imod] - 1;
        for (int pmt = 0; pmt < 2; ++pmt) {

          Identifier id1 = tileID->pmt_id(3, side, module, 9, 1, pmt);
          if (id1.is_valid()) {
            std::cout << " id1=" << tileID->to_string(id1, -1);
            HWIdentifier hw1 = m_cablingService->s2h_channel_id(id1);
            if (hw1.is_valid()) {
              std::cout << " hw1=" << tileHWID->to_string(hw1, -1);
              Identifier id2 = m_cablingService->h2s_pmt_id(hw1);
              if (id2.is_valid()) {
                if (id2 != id1) std::cout << " id1!=id2=" << tileID->to_string(id2, -1);
                HWIdentifier hw2 = m_cablingService->s2h_channel_id(id2);
                if (hw2.is_valid()) {
                  if (hw2 != hw1) std::cout << " hwid1!=hwid2=" << tileHWID->to_string(hw2, -1);
                } else {
                  std::cout << " hw2=invalid";
                }
              } else {
                std::cout << " id2=invalid";
              }
            } else {
              std::cout << " hw1=invalid";
            }
          } else {
            std::cout << " id1=invalid";
          }
          std::cout << std::endl;
        }
      }
    }

    // checking E1
    std::cout << "========= E1 cells =========" << std::endl;

    for (int side = -1; side < 2; side += 2) {
      for (int module = 0; module < 64; ++module) {
        for (int tower = 10; tower < 11; tower += 1) {

          Identifier id1 = tileID->pmt_id(3, side, module, tower, 3, 0);
          if (id1.is_valid()) {
            std::cout << " id1=" << tileID->to_string(id1, -1);
            HWIdentifier hw1 = m_cablingService->s2h_channel_id(id1);
            if (hw1.is_valid()) {
              std::cout << " hw1=" << tileHWID->to_string(hw1, -1);
              Identifier id2 = m_cablingService->h2s_pmt_id(hw1);
              if (id2.is_valid()) {
                if (id2 != id1) std::cout << " id1!=id2=" << tileID->to_string(id2, -1);
                HWIdentifier hw2 = m_cablingService->s2h_channel_id(id2);
                if (hw2.is_valid()) {
                  if (hw2 != hw1) std::cout << " hwid1!=hwid2=" << tileHWID->to_string(hw2, -1);
                } else {
                  std::cout << " hw2=invalid";
                }
              } else {
                std::cout << " id2=invalid";
              }
            } else {
              std::cout << " hw1=invalid";
            }
          } else {
            std::cout << " id1=invalid";
          }
          std::cout << std::endl;
        }
      }
    }

    // checking E2
    std::cout << "========= E2 cells =========" << std::endl;

    for (int side = -1; side < 2; side += 2) {
      for (int module = 0; module < 64; ++module) {
        for (int tower = 11; tower < 12; tower += 1) {

          Identifier id1 = tileID->pmt_id(3, side, module, tower, 3, 0);
          if (id1.is_valid()) {
            std::cout << " id1=" << tileID->to_string(id1, -1);
            HWIdentifier hw1 = m_cablingService->s2h_channel_id(id1);
            if (hw1.is_valid()) {
              std::cout << " hw1=" << tileHWID->to_string(hw1, -1);
              Identifier id2 = m_cablingService->h2s_pmt_id(hw1);
              if (id2.is_valid()) {
                if (id2 != id1) std::cout << " id1!=id2=" << tileID->to_string(id2, -1);
                HWIdentifier hw2 = m_cablingService->s2h_channel_id(id2);
                if (hw2.is_valid()) {
                  if (hw2 != hw1) std::cout << " hwid1!=hwid2=" << tileHWID->to_string(hw2, -1);
                } else {
                  std::cout << " hw2=invalid";
                }
              } else {
                std::cout << " id2=invalid";
              }
            } else {
              std::cout << " hw1=invalid";
            }
          } else {
            std::cout << " id1=invalid";
          }
          std::cout << std::endl;
        }
      }
    }

    // checking E3,E4
    std::cout << "========= E3,E4 cells =========" << std::endl;

    for (int side = -1; side < 2; side += 2) {
      for (int module = 0; module < 64; ++module) {
        for (int tower = 13; tower < 16; tower += 2) {

          Identifier id1 = tileID->pmt_id(3, side, module, tower, 3, 0);
          if (id1.is_valid()) {
            std::cout << " id1=" << tileID->to_string(id1, -1);
            HWIdentifier hw1 = m_cablingService->s2h_channel_id(id1);
            if (hw1.is_valid()) {
              std::cout << " hw1=" << tileHWID->to_string(hw1, -1);
              Identifier id2 = m_cablingService->h2s_pmt_id(hw1);
              if (id2.is_valid()) {
                if (id2 != id1) std::cout << " id1!=id2=" << tileID->to_string(id2, -1);
                HWIdentifier hw2 = m_cablingService->s2h_channel_id(id2);
                if (hw2.is_valid()) {
                  if (hw2 != hw1) std::cout << " hwid1!=hwid2=" << tileHWID->to_string(hw2, -1);
                } else {
                  std::cout << " hw2=invalid";
                }
              } else {
                std::cout << " id2=invalid";
              }
            } else {
              std::cout << " hw1=invalid";
            }
          } else {
            std::cout << " id1=invalid";
          }
          std::cout << std::endl;
        }
      }
    }

    // checking MBTS
    std::cout << "========= MBTS cells ==========" << std::endl;

    for (int side = -1; side < 2; side += 2) {
      for (int module = 0; module < 8; ++module) {
        for (int tower = 0; tower < 2; ++tower) {

          Identifier id1 = tileTBID->channel_id(side, module, tower);
          if (id1.is_valid()) {
            std::cout << " id1=" << tileTBID->to_string(id1);
            HWIdentifier hw1 = m_cablingService->s2h_channel_id(id1);
            if (hw1.is_valid()) {
              std::cout << " hw1=" << tileHWID->to_string(hw1, -1);
              Identifier id2 = m_cablingService->h2s_pmt_id(hw1);
              if (id2.is_valid()) {
                if (id2 != id1) std::cout << " id1!=id2=" << tileTBID->to_string(id2);
                HWIdentifier hw2 = m_cablingService->s2h_channel_id(id2);
                if (hw2.is_valid()) {
                  if (hw2 != hw1) std::cout << " hwid1!=hwid2=" << tileHWID->to_string(hw2, -1);
                } else {
                  std::cout << " hw2=invalid";
                }
              } else {
                std::cout << " id2=invalid";
              }
            } else {
              std::cout << " hw1=invalid";
            }
          } else {
            std::cout << " id1=invalid";
          }
          std::cout << std::endl;
        }
      }
    }

    // checking E4'
    if (m_cablingType == TileCablingService::RUN2Cabling || m_cablingType == TileCablingService::RUN2aCabling) {

      std::cout << "========= E4' cells ==========" << std::endl;
    
      for (int side = -1; side < 0; side += 2) {
        for (int module = 0; module < 4; ++module) {
          for (int tower = 2; tower < 3; ++tower) {
    
            Identifier id1 = tileTBID->channel_id(side, module, tower);
            if (id1.is_valid()) {
              std::cout << " id1=" << tileTBID->to_string(id1);
              HWIdentifier hw1 = m_cablingService->s2h_channel_id(id1);
              if (hw1.is_valid()) {
                std::cout << " hw1=" << tileHWID->to_string(hw1, -1);
                Identifier id2 = m_cablingService->h2s_pmt_id(hw1);
                if (id2.is_valid()) {
                  if (id2 != id1) std::cout << " id1!=id2=" << tileTBID->to_string(id2);
                  HWIdentifier hw2 = m_cablingService->s2h_channel_id(id2);
                  if (hw2.is_valid()) {
                    if (hw2 != hw1) std::cout << " hwid1!=hwid2=" << tileHWID->to_string(hw2, -1);
                  } else {
                    std::cout << " hw2=invalid";
                  }
                } else {
                  std::cout << " id2=invalid";
                }
              } else {
                std::cout << " hw1=invalid";
              }
            } else {
              std::cout << " id1=invalid";
            }
            std::cout << std::endl;
          }
        }
      }
    }

    std::cout << "===============================" << std::endl;
  }

  if (m_useCache) cablingService->fillH2SIdCache();

  return StatusCode::SUCCESS;
}

//
//_____________________________________________________________________________
StatusCode TileCablingSvc::finalize() {
  ATH_MSG_DEBUG( "finalized" );
  return StatusCode::SUCCESS;
}
