/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/BigWheelCoincidenceLUT.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "TrigT1TGC/TGCNumbering.h"
#include "TrigT1TGC/TGCDatabaseManager.h"
#include "PathResolver/PathResolver.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/ReadCondHandle.h"

namespace LVL1TGC {

int8_t BigWheelCoincidenceLUT::test(int octantId, int moduleId, int subsector, 
                                    int type, int dr, int dphi) const {
  if (type < TGCTriggerLUTs::COIN_HH || type > TGCTriggerLUTs::COIN_LL) return 0;  // no candidate

  int phimod2 = 0;
  if (moduleId == 2 || moduleId == 5 || moduleId == 8) {   // forward sectors
    int sector = (moduleId-2) / 3 + octantId * 3;   // sector number assuming the forward sector
    phimod2 = (sector%2 == 1) ? 1 : 0;
  }

  uint32_t addr = ((type & TGCTriggerLUTs::TYPE_MASK)<<TGCTriggerLUTs::TYPE_SHIFT) +
                  ((phimod2 & TGCTriggerLUTs::PHIMOD2_MASK)<<TGCTriggerLUTs::PHIMOD2_SHIFT) +
                  ((moduleId & TGCTriggerLUTs::MODULE_MASK)<<TGCTriggerLUTs::MODULE_SHIFT) +
                  ((subsector & TGCTriggerLUTs::ROI_MASK)<<TGCTriggerLUTs::ROI_SHIFT) +
                  (((dr+TGCTriggerLUTs::DR_HIGH_RANGE) & TGCTriggerLUTs::DR_MASK)<<TGCTriggerLUTs::DR_SHIFT) +
                  (((dphi+TGCTriggerLUTs::DPHI_HIGH_RANGE) & TGCTriggerLUTs::DPHI_MASK)<<TGCTriggerLUTs::DPHI_SHIFT);

  int8_t content = 0x0;   // outside from defined window, i.e. pT=0

  if(tgcArgs()->USE_CONDDB()) {
    SG::ReadCondHandle<TGCTriggerLUTs> readHandle{m_readCondKey};
    const TGCTriggerLUTs* readCdo{*readHandle};
    bool fullCW = (readCdo->getType(TGCTriggerLUTs::CW_BW) == "full");
    if(fullCW) addr += (m_side<<TGCTriggerLUTs::SIDE_SHIFT) +
                       ((m_octant & TGCTriggerLUTs::OCTANT_MASK)<<TGCTriggerLUTs::OCTANT_SHIFT);

    content = readCdo->getBigWheelPt(addr);
  } else {
    if(m_fullCW) addr += (m_side<<TGCTriggerLUTs::SIDE_SHIFT) +
                         ((m_octant & TGCTriggerLUTs::OCTANT_MASK)<<TGCTriggerLUTs::OCTANT_SHIFT);

    std::unordered_map<uint32_t, char>::const_iterator it = m_lut.find(addr);
    if(it != m_lut.end()) {
      char pt_char = it->second;
      content = m_pTdef.find(pt_char)->second;
    }
  }

  return content;
}


BigWheelCoincidenceLUT::BigWheelCoincidenceLUT(LVL1TGCTrigger::TGCArguments* tgcargs,
					       const SG::ReadCondHandleKey<TGCTriggerLUTs>& readKey,
                                               const std::string& version)
: m_verName(version),
  m_side(0),  m_octant(0),
  m_fullCW(false),
  m_tgcArgs(tgcargs),
  m_readCondKey(readKey) {
  IMessageSvc* msgSvc = 0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  if (svcLocator->service("MessageSvc", msgSvc) == StatusCode::FAILURE) {
    return;
  }
  MsgStream log(msgSvc, "LVL1TGC::BigWheelCoincidenceLUT");

  log << MSG::INFO
      << " BigWheel LUT version of " << m_verName << " is selected." << endmsg;

  if (!tgcArgs()->USE_CONDDB()) {
    // read LUT contents from local files
    this->readMap();
  } 
}

BigWheelCoincidenceLUT::~BigWheelCoincidenceLUT() {
}

bool BigWheelCoincidenceLUT::readMap() 
{
  const uint8_t kNMODULETYPE = 12;
  const uint8_t modulenumber[kNMODULETYPE] = {0, 1, 2, 2, 3, 4, 5, 5, 6, 7, 8, 8};
  const std::string modulename[kNMODULETYPE] = {"0a","1a","2a","2b","3a","4a","5a","5b","6a","7a","8a","8b"};
  const std::string sidename[kNSide] = {"a","c"};

  const std::string octantName[kNOctant] =
    {  "0", "1", "2", "3", "4", "5", "6", "7"};
  const std::string coincidenceTypeName[N_COIN_TYPE] = {"HH", "HL", "LH", "LL"};

  IMessageSvc* msgSvc = 0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  if (svcLocator->service("MessageSvc", msgSvc) == StatusCode::FAILURE) {
    return false;
  }
  MsgStream log(msgSvc, "LVL1TGC::BigWheelCoincidenceLUT");

  uint32_t octaddr = (m_side<<TGCTriggerLUTs::SIDE_SHIFT) +
                     ((m_octant & TGCTriggerLUTs::OCTANT_MASK)<<TGCTriggerLUTs::OCTANT_SHIFT);

  // loop over all files...
  for (int iModule=0; iModule < kNMODULETYPE; iModule+=1) {
    uint32_t phimod2 = (modulename[iModule].find("b") != std::string::npos) ? 1 : 0;
    uint32_t modaddr = ((modulenumber[iModule] & TGCTriggerLUTs::MODULE_MASK)<<TGCTriggerLUTs::MODULE_SHIFT) +
                       ((phimod2 & TGCTriggerLUTs::PHIMOD2_MASK)<<TGCTriggerLUTs::PHIMOD2_SHIFT);

    for (int iCoinType=0; iCoinType != N_COIN_TYPE; iCoinType++) {
      std::string fn = "/BW/cm_";
      if (m_fullCW) {
        fn += sidename[m_side] + octantName[m_octant] + modulename[iModule]+coincidenceTypeName[iCoinType]+"_";
      } else {
        fn += sidename[m_side] + modulename[iModule] + coincidenceTypeName[iCoinType]+"_Octant_";
      }
      fn += m_verName + ".db";

      int type = -1;
      int lDR, hDR, lDPhi, hDPhi;
      std::string fullName = PathResolver::FindCalibDirectory("dev")+"/TrigT1TGC"+fn;
      if( fullName.length() == 0 ) {
        log << MSG::ERROR << " Could not found " << fn.c_str() << endmsg;
        continue;
      }

      std::ifstream file(fullName.c_str(),std::ios::in);
      if(!file){
        log << MSG::ERROR << " Could not found " << fullName.c_str() << endmsg;
        continue;
      }

      std::string buf, tag;
      char delimiter = '\n';
      while (getline(file,buf,delimiter)){
        std::istringstream header(buf);
        header>>tag;

        if (tag == "#") {    // read header part
          int roi;
          header >> roi >> lDR >> hDR >> lDPhi >> hDPhi;
          type = getTYPE(lDR, hDR, lDPhi, hDPhi);
          // check moduleNumber and ptLevel
          if(type < 0) {
            log << MSG::WARNING
                << " illegal parameter in database header : " << header.str() << " in file " << fn << endmsg;
            break;
          }

          uint32_t cwaddr = ((uint8_t(type) & TGCTriggerLUTs::TYPE_MASK)<<TGCTriggerLUTs::TYPE_SHIFT) +
                            ((roi & TGCTriggerLUTs::ROI_MASK)<<TGCTriggerLUTs::ROI_SHIFT);

          for(uint8_t ir=lDR+TGCTriggerLUTs::DR_HIGH_RANGE; ir <= hDR+TGCTriggerLUTs::DR_HIGH_RANGE; ir++) {
            uint32_t draddr = (ir & TGCTriggerLUTs::DR_MASK)<<TGCTriggerLUTs::DR_SHIFT;

            // get window data
            getline(file, buf, delimiter);

            for(uint8_t iphi=lDPhi+TGCTriggerLUTs::DPHI_HIGH_RANGE; iphi <= hDPhi+TGCTriggerLUTs::DPHI_HIGH_RANGE; iphi++) {
              uint32_t theaddr = octaddr + modaddr + cwaddr + draddr + iphi;
              char pt = buf[iphi-lDPhi-TGCTriggerLUTs::DPHI_HIGH_RANGE];
              if (pt == 'X') continue;   // not opened
              m_lut[theaddr] = pt;
            }
          }
        }   // if (tag == "#")
      }   // while (getline(...))
    }   // for (int iCoinType)
  }   // for (int iModule)

  return true;
}


} //end of namespace bracket
