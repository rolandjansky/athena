//Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "TileDCSCondAlg.h"
#include "TileConditions/TileCablingService.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

// Athena includes
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"
#include "PathResolver/PathResolver.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "CoralBase/Attribute.h"

#include <fstream>

TileDCSCondAlg::TileDCSCondAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator),
  m_cabling(nullptr)
{

}


StatusCode TileDCSCondAlg::initialize() {

  ATH_MSG_DEBUG( "In initialize()" );

  ATH_CHECK( m_hvKey.initialize(m_readHV) );

  ATH_CHECK( m_hvSetKey.initialize(m_readHVSet) );
  ATH_CHECK( m_emScaleKey.initialize(!m_readHVSet) );

  ATH_CHECK( m_statesKey.initialize(m_readStates) );


  ATH_MSG_INFO( "Will read the following folders: "
                << ((m_readHV) ? m_hvKey.key() : "") << " "
                << ((m_readHVSet) ? m_hvSetKey.key() : "") << " "
                << ((m_readStates) ? m_statesKey.key() : ""));

  //=== get TileCablingSvc
  ATH_CHECK( m_cablingSvc.retrieve() );

  //=== cache pointers to cabling helpers
  m_cabling = m_cablingSvc->cablingService();

  if (!m_cabling) {
    ATH_MSG_ERROR( "Pointer to TileCablingService is zero: " << m_cabling);
    return StatusCode::FAILURE;
  }

  std::map<TileModuleType, unsigned int> roses{{LONG_BARREL, 2}, {EXTENDED_BARREL, 3}};

  for (TileModuleType moduleType : {LONG_BARREL, EXTENDED_BARREL}) {

    unsigned int ros = roses[moduleType];
    unsigned int channel = 0;

    for (; channel < TileCalibUtils::MAX_CHAN; ++channel) {
      int pmt = m_cabling->channel2hole(ros, channel);
      m_isChannelUsed[moduleType][channel] = (pmt > 0); // positive pmt - means channel is connected

      if (moduleType == EXTENDED_BARREL) {
        m_isChannelUsed[EXTENDED_BARREL_SPECIAL_C10][channel] =
          m_isChannelUsed[EXTENDED_BARREL_NO_D4][channel] = m_isChannelUsed[EXTENDED_BARREL][channel];
      }

      pmt = std::abs(pmt);
      m_channel2pmt[moduleType][channel] = pmt - 1;
      m_pmt2channel[moduleType][pmt - 1] = channel; // note - pmt-1 is used as index
    }

    for (; channel < 68; ++channel) {
      m_channel2pmt[moduleType][channel] = channel;
      m_pmt2channel[moduleType][channel] = channel;
    }

  }

  // description of EBA15 and EBC18 channel 0-3 do not exist, channel 18,19 used
  m_isChannelUsed[EXTENDED_BARREL_NO_D4][0] = false;
  m_isChannelUsed[EXTENDED_BARREL_NO_D4][1] = false;
  m_isChannelUsed[EXTENDED_BARREL_NO_D4][2] = false;
  m_isChannelUsed[EXTENDED_BARREL_NO_D4][3] = false;
  m_isChannelUsed[EXTENDED_BARREL_NO_D4][18] = true;
  m_isChannelUsed[EXTENDED_BARREL_NO_D4][19] = true;
  // description of EB with special C10 - channel 4 (pmt 5) is not used
  m_isChannelUsed[EXTENDED_BARREL_SPECIAL_C10][4] = false;

  if (m_readHVSet) {
    // read special deltas for few unstable PMTs
    readBadHV("BadHVChannels.dat");
    if (readConfig(m_dcsChHvSetInCool, "Drawer", m_rosDrawerHVSet)) return StatusCode::FAILURE;
  }

  if (readConfig(m_dcsChStatesInCool, "_LVPS_", m_rosDrawerStates)) return StatusCode::FAILURE;
  if (readConfig(m_dcsChHvInCool, "Drawer", m_rosDrawerHV)) return StatusCode::FAILURE;

  ATH_CHECK( m_dcsStateKey.initialize() );

  return StatusCode::SUCCESS;
}


StatusCode TileDCSCondAlg::execute(const EventContext& ctx) const {

  SG::WriteCondHandle<TileDCSState> dcsState{m_dcsStateKey, ctx};

  if (dcsState.isValid()) {
    ATH_MSG_DEBUG("Found valid TileDCSState: " << dcsState.key());
    return StatusCode::SUCCESS;
  }

  std::unique_ptr<TileDCSState> dcsStateData = std::make_unique<TileDCSState>();
  dcsStateData->setGoodDrawer(m_goodDrawer.value());
  dcsStateData->setWarningDrawer(m_warningDrawer.value());

  if (m_readHV) {

    SG::ReadCondHandle<CondAttrListCollection> hv(m_hvKey, ctx);
    dcsState.addDependency(hv);
    for (const CondAttrListCollection::ChanAttrListPair& chanAttrListPair : **hv) {

      const CondAttrListCollection::ChanNum coolChannel = chanAttrListPair.first;

      const std::pair<int, int> rosDrawerPair = m_rosDrawerHV.at(coolChannel);
      unsigned int ros = rosDrawerPair.first;
      unsigned int drawer = rosDrawerPair.second;

      ATH_MSG_VERBOSE( "ros: " << ros
                       << ", drawer: " << drawer
                       << ", COOL channel: " << coolChannel
                       << ", payload: " << chanAttrListPair.second);

      int moduleType = (ros > 2) ? EXTENDED_BARREL : LONG_BARREL;
      int pmt0 = 0; // runs over paylod parameters
      for (const coral::Attribute& attribute : chanAttrListPair.second) {
        int channel = m_pmt2channel[moduleType][pmt0];
        dcsStateData->setChannelHV(ros, drawer, channel, attribute.data<float>());
        ATH_MSG_VERBOSE( "HV[" << pmt0 << "] = " << attribute.data<float>() );
        ++pmt0;
      }

    }
  }


  if (m_readHVSet) {

    SG::ReadCondHandle<CondAttrListCollection> hvSet(m_hvSetKey, ctx);
    dcsState.addDependency(hvSet);
    for (const CondAttrListCollection::ChanAttrListPair& chanAttrListPair : **hvSet) {

      const CondAttrListCollection::ChanNum coolChannel = chanAttrListPair.first;

      const std::pair<int, int> rosDrawerPair = m_rosDrawerHVSet.at(coolChannel);
      unsigned int ros = rosDrawerPair.first;
      unsigned int drawer = rosDrawerPair.second;

      ATH_MSG_VERBOSE( "ros: " << ros
                       << ", drawer: " << drawer
                       << ", COOL channel: " << coolChannel
                       << ", payload: " << chanAttrListPair.second);

      int moduleType = (ros > 2) ? EXTENDED_BARREL : LONG_BARREL;
      int pmt0 = 0; // runs over paylod parameters
      for (const coral::Attribute& attribute : chanAttrListPair.second) {
        int channel = m_pmt2channel[moduleType][pmt0];
        dcsStateData->setChannelHVSet(ros, drawer, channel, attribute.data<float>());
        ATH_MSG_VERBOSE( "HVSET[" << pmt0 << "] = " << attribute.data<float>() );
        ++pmt0;
      }

    }
  } else {
    ATH_CHECK( fillReferenceHV(*dcsStateData, dcsState, ctx) );

  }


  if (m_readStates) {

    SG::ReadCondHandle<CondAttrListCollection> states(m_statesKey, ctx);
    dcsState.addDependency(states);
    for (const CondAttrListCollection::ChanAttrListPair& chanAttrListPair : **states) {

      CondAttrListCollection::ChanNum coolChannel = chanAttrListPair.first;

      std::pair<int, int> rosDrawerPair = m_rosDrawerStates.at(coolChannel);
      unsigned int ros = rosDrawerPair.first;
      unsigned int drawer = rosDrawerPair.second;

      ATH_MSG_VERBOSE( "ros: " << ros
                       << ", drawer: " << drawer
                       << ", COOL channel: " << coolChannel
                       << ", payload: " << chanAttrListPair.second);

      for (const coral::Attribute& attribute : chanAttrListPair.second) {
        dcsStateData->setDrawerStates(ros, drawer, attribute.data<int>());
        ATH_MSG_VERBOSE( "STATES = " << attribute.data<int>() );
      }

    }
  }


  for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {

      TileDCSState::TileDCSStatus drawerStatus = dcsStateData->getDCSStatus(ros, drawer);

      int moduleType = (ros > 2) ? EXTENDED_BARREL : LONG_BARREL;
      int specialModuleType(moduleType);
      if (moduleType == EXTENDED_BARREL) {
        if ((ros == 3 && drawer == 14) || (ros == 4 && drawer == 17)) {
          specialModuleType = EXTENDED_BARREL_NO_D4; // EBA15 EBC18
        } else if (!m_cabling->C10_connected(drawer)) {
          specialModuleType = EXTENDED_BARREL_SPECIAL_C10; // EB modules with special C10
        }
      }

      for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
        if (m_isChannelUsed[specialModuleType][channel]) {

          TileDCSState::TileDCSStatus channelHVStatus(TileDCSState::UNKNOWN);

          float hvSet(dcsStateData->getChannelHVSet(ros, drawer, channel));
          float hv(dcsStateData->getChannelHV(ros, drawer, channel));

          // ignore zero "set" and negative "measured" HV, set status to unknown
          if (hvSet >= 1.0 && hv >= 0.0    // HV can't be negative
              && hvSet < 999.9 && hv < 999.9) { // HV can't be greater 930V

            std::map<int, std::pair<float, float>>::const_iterator itr =
              m_knownBadHV.find(getChannelKey(ros, drawer, channel));

            if (itr != m_knownBadHV.end()) { // known unstable PMTs

              float hvGap = (m_readHVSet) ? // take into account offset if HVSET is used
                fabs(hvSet + itr->second.second - hv) : fabs(hvSet - hv);

              if (hvGap <= m_goodHVLimit) channelHVStatus = TileDCSState::OK_KNOWNBADPMT;
              else if (hvGap <= m_warningHVLimit) channelHVStatus = TileDCSState::WARNING_KNOWNBADPMT;
              else channelHVStatus = TileDCSState::ALERT_KNOWNBADPMT;
            } else {
              float hvGap = fabs(hvSet - hv);
              if (hvGap <= m_goodHVLimit) channelHVStatus = TileDCSState::OK;
              else if (hvGap <= m_warningHVLimit) channelHVStatus = TileDCSState::WARNING;
              else channelHVStatus = TileDCSState::ALERT;
            }

          }

          dcsStateData->setDCSHVStatus(ros, drawer, channel, channelHVStatus);

          if (drawerStatus > TileDCSState::OK && !m_readHVSet) {
            dcsStateData->setDCSStatus(ros, drawer, channel, drawerStatus);
          } else {
            dcsStateData->setDCSStatus(ros, drawer, channel, std::max(drawerStatus, channelHVStatus));
          }

        } else {
          dcsStateData->setDCSHVStatus(ros, drawer, channel, TileDCSState::NON_EXISTING);
          dcsStateData->setDCSStatus(ros, drawer, channel, TileDCSState::NON_EXISTING);
        }
      }


      if (msgLvl(MSG::VERBOSE)) {
        msg(MSG::VERBOSE) << "Module = " << TileCalibUtils::getDrawerString(ros, drawer)
                          << " Status = " << drawerStatus << endmsg;

        std::string ignored = ((drawerStatus > TileDCSState::OK && !m_readHVSet) ? " ignored" : "");

        for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
          int pmt0 = m_channel2pmt[moduleType][channel];
          TileDCSState::TileDCSStatus status = dcsStateData->getDCSHVStatus(ros, drawer, channel);
          msg(MSG::VERBOSE) << "Module = " << TileCalibUtils::getDrawerString(ros, drawer)
                            << " channel = " << channel
                            << " pmt = " << pmt0 + 1
                            << " HV = " << dcsStateData->getChannelHV(ros, drawer, channel)
                            << " HVSET = " << dcsStateData->getChannelHVSet(ros, drawer, channel)
                            << " Status = " << status
                            << ignored << endmsg;
        }
      }

    }
  }

  // Define validity of the output cond object
  if(dcsState.record(std::move(dcsStateData)).isFailure()) {

    ATH_MSG_ERROR("Could not record TileDCSState object with "
                  << dcsState.key()
                  << " with EventRange " << dcsState.getRange()
                  << " into Conditions Store");

    return StatusCode::FAILURE;
  } else {

    ATH_MSG_VERBOSE("Recorded TileDCSState object with "
		    << dcsState.key()
		    << " with EventRange " << dcsState.getRange()
		    << " into Conditions Store");
  }

  return StatusCode::SUCCESS;

}



//_____________________________________________________________________________
int TileDCSCondAlg::readConfig(const std::string& fileName,
                               const std::string& subStr,
                               std::vector<std::pair<int, int>>& rosDrawer) {

  std::string fullFileName = PathResolver::find_file(fileName, "DATAPATH");
  std::ifstream file(fullFileName.c_str());
  if (!file.is_open()) {
    ATH_MSG_ERROR( "Can't find " << fileName);
    return 2;
  } else {
    ATH_MSG_DEBUG( "Reading file " << fullFileName);
  }

  rosDrawer.resize(257, std::make_pair<int, int>(0xFFFF, 0xFFFF));

  std::string compName[5] = { "XXX", "ATLTILLV01", "ATLTILLV02", "ATLTILLV00", "ATLTILLV03" };

  unsigned int ind, ros, module;
  std::string s, line;
  size_t pos;

  while (true) {

    file >> s >> ind >> line;

    if (file.eof() || file.fail()) break;

    for (ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
      pos = line.find(compName[ros]);
      if (pos != std::string::npos) break;
    }

    if (ros == TileCalibUtils::MAX_ROS) {
      msg(MSG::WARNING) << "File " << fileName << " Wrong computer name in " << line << endmsg;
      continue;
    }

    pos = line.find(subStr);
    if (pos == std::string::npos) {
      ATH_MSG_WARNING( "File " << fileName << " Word " << subStr << " not found in " << line);
      continue;
    }

    module = atoi(&line[pos + subStr.size()]);
    if (module < 1 || module > TileCalibUtils::MAX_DRAWER) {
      ATH_MSG_WARNING( "File " << fileName << " Wrong module number in " << line);
      continue;
    }

    ATH_MSG_VERBOSE("Module = " << TileCalibUtils::getDrawerString(ros, module - 1) << " ind = " << ind);

    if (rosDrawer.size() <= ind) {
      rosDrawer.resize(ind + 1, std::make_pair<int, int> (0xFFFF, 0xFFFF));
    }

    rosDrawer[ind] = std::make_pair<int, int>(ros, (module - 1));

  }

  file.close();

  return 0;
}

//_____________________________________________________________________________
int TileDCSCondAlg::readBadHV(const std::string& fileName) {

  std::string fullFileName = PathResolver::find_file(fileName, "DATAPATH");
  std::ifstream file(fullFileName.c_str());
  if (!file.is_open()) {
    ATH_MSG_WARNING( "Can't find " << fileName);
    return 1;
  } else {
    ATH_MSG_DEBUG("Reading file " << fullFileName);
  }

  std::string partName[5] = { "AUX", "LBA", "LBC", "EBA", "EBC" };

  unsigned int ros, module, pmt;
  float delta, offset;
  char ll[256];
  std::string line;
  size_t pos = 0;

  while (true) {

    file.getline(ll, sizeof(ll));

    if (file.eof() || file.fail()) break;

    line = std::string(ll);

    for (ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
      pos = line.find(partName[ros]);
      if (pos != std::string::npos) break;
    }
    if (ros == TileCalibUtils::MAX_ROS) {
      ATH_MSG_WARNING( "File " << fileName << " Partition name not found in " << line);
      continue;
    }

    module = atoi(&line[pos + 3]);
    if (module < 1 || module > TileCalibUtils::MAX_DRAWER) {
      ATH_MSG_WARNING( "File " << fileName << " Wrong module number in " << line);
      continue;
    }

    pos = line.find("PMT number");
    if (pos == std::string::npos) {
      ATH_MSG_WARNING( "File " << fileName << " Word 'PMT number' not found in " << line);
      continue;
    }

    pmt = atoi(&line[pos + 10]);
    if (pmt < 1 || pmt > TileCalibUtils::MAX_CHAN) {
      ATH_MSG_WARNING( "File " << fileName << " Wrong pmt number in " << line);
      continue;
    }

    pos = line.find("Step Value is");
    if (pos == std::string::npos) {
      ATH_MSG_WARNING( "File " << fileName << " Word 'Step Value is' not found in " << line);
      continue;
    }

    delta = atof(&line[pos + 13]);
    if (delta <= 0.0 || delta > 1000.0) {
      ATH_MSG_WARNING( "File " << fileName << " Wrong delta number in " << line);
      continue;
    }

    pos = line.find("Offset is:");
    if (pos == std::string::npos) {
      ATH_MSG_WARNING( "File " << fileName << " Word 'Offset is:' not found in " << line);
      continue;
    }

    offset = atof(&line[pos + 10]);
    if (offset < -1000.0 || offset > 1000.0) {
      ATH_MSG_WARNING( "File " << fileName << " Wrong offset number in " << line);
      continue;
    }

    ATH_MSG_WARNING( "Module = " << TileCalibUtils::getDrawerString(ros, module - 1)
                    << " pmt = " << pmt
                    << " delta = " << delta
                    << " offset = " << offset);

    int moduleType = (ros > 2) ? EXTENDED_BARREL : LONG_BARREL;
    int channel = m_pmt2channel[moduleType][pmt - 1];
    m_knownBadHV[getChannelKey(ros, module - 1, channel)] = std::make_pair(delta, offset);

  }

  file.close();

  return 0;
}

StatusCode TileDCSCondAlg::fillReferenceHV(TileDCSState& dcsState,
					   SG::WriteCondHandle<TileDCSState>& wh, 
                                           const EventContext& ctx) const {

  SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey, ctx);
  wh.addDependency(emScale);

  float laserReferenceHV = -5.;
  float cesReferenceHV = -5.;

  for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
    int moduleType = (ros > 2) ? EXTENDED_BARREL : LONG_BARREL;

    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
      unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);

      for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
        int pmt0 = m_channel2pmt[moduleType][channel];

        cesReferenceHV = emScale->getCesiumReferenceHV(drawerIdx, channel);
        laserReferenceHV = emScale->getLaserReferenceHV(drawerIdx, channel);

        if (laserReferenceHV >= 0.) {
          dcsState.setChannelHVSet(ros, drawer, channel, laserReferenceHV);
        } else {
          dcsState.setChannelHVSet(ros, drawer, channel, cesReferenceHV);
        }

        if (msgLvl(MSG::VERBOSE)) {
          msg(MSG::VERBOSE) << "Module = " << TileCalibUtils::getDrawerString(ros, drawer)
                            << " channel = " << channel
                            << " pmt = " << pmt0 + 1
                            << " HVref = " << dcsState.getChannelHVSet(ros, drawer, channel);

          if (laserReferenceHV >= 0.) {
            msg(MSG::VERBOSE) << " Taken from Laser run (replace "
                              << cesReferenceHV << " cesium value)" << endmsg;
          } else {
            msg(MSG::VERBOSE) << endmsg;
          }
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode TileDCSCondAlg::finalize() {
  return StatusCode::SUCCESS;
}
