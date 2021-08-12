/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

#include "TrigT1TGC/TGCRPhiCoincidenceMap.h"
#include "TrigT1TGC/TGCDatabaseManager.h"
#include "PathResolver/PathResolver.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/ReadCondHandle.h"
#include "MuonCondSvc/TGCTriggerData.h"

namespace LVL1TGCTrigger {

uint8_t TGCRPhiCoincidenceMap::test(int octantId, int moduleId, int subsector, 
                                    int type, int dr, int dphi) const
{
  if (type<0 || type>=N_TMap) return 0;  // check the range of coincidence type

  uint16_t sector = (moduleId-2)/3 + octantId*3;  // sector number in the forward region (0...23)
  // calculate whether the front or back side of the forward chamber
  uint8_t phimod2 = (moduleId==2||moduleId==5||moduleId==8)&&(sector%2==1) ? 1 : 0;

  uint32_t addr = ((type & TGCTriggerData::TYPE_MASK)<<TGCTriggerData::TYPE_SHIFT) +
                  ((phimod2 & TGCTriggerData::PHIMOD2_MASK)<<TGCTriggerData::PHIMOD2_SHIFT) +
                  ((moduleId & TGCTriggerData::MODULE_MASK)<<TGCTriggerData::MODULE_SHIFT) +
                  ((subsector & TGCTriggerData::ROI_MASK)<<TGCTriggerData::ROI_SHIFT) +
                  (((dr-DR_offset) & TGCTriggerData::DR_MASK)<<TGCTriggerData::DR_SHIFT) +
                  (((dphi-DPhi_offset) & TGCTriggerData::DPHI_MASK)<<TGCTriggerData::DPHI_SHIFT);

  uint8_t content = 0x0;   // outside from defined window, i.e. pT=0

  if(tgcArgs()->USE_CONDDB()) {
    SG::ReadCondHandle<TGCTriggerData> readHandle{m_readCondKey};
    const TGCTriggerData* readCdo{*readHandle};

    bool fullCW = (readCdo->getType(TGCTriggerData::CW_BW) == "full");
    if(fullCW) addr += (m_side<<TGCTriggerData::SIDE_SHIFT) +
                       ((m_octant & TGCTriggerData::OCTANT_MASK)<<TGCTriggerData::OCTANT_SHIFT);
    content = readCdo->getBigWheelPt(addr);
  } else {
    if(m_fullCW) addr += (m_side<<TGCTriggerData::SIDE_SHIFT) +
                         ((m_octant & TGCTriggerData::OCTANT_MASK)<<TGCTriggerData::OCTANT_SHIFT);

    std::unordered_map<uint32_t, uint8_t>::const_iterator it = m_ptmap.find(addr);
    if(it != m_ptmap.end()) content = it->second;
  }

  return content;
}



TGCRPhiCoincidenceMap::TGCRPhiCoincidenceMap(TGCArguments* tgcargs,
					     const SG::ReadCondHandleKey<TGCTriggerData>& readCondKey,
					     int   sideId, int octantId)
 : m_numberOfDR(0), m_numberOfDPhi(0),
   m_side(sideId),
   m_octant(octantId),
   m_fullCW(false),
   m_tgcArgs(tgcargs),
   m_readCondKey(readCondKey)
{
  if (!tgcArgs()->USE_CONDDB()) {
    if(!tgcArgs()->useRun3Config()){
      this->readMap();  // read Coincidence Map for Run2 (6 thresholds)
    }
  } 
}


TGCRPhiCoincidenceMap::~TGCRPhiCoincidenceMap()
{
}

TGCRPhiCoincidenceMap::TGCRPhiCoincidenceMap(const TGCRPhiCoincidenceMap& right)
: m_readCondKey(right.m_readCondKey)
{
  m_numberOfDR=right.m_numberOfDR;
  m_numberOfDPhi=right.m_numberOfDPhi;
  m_verName=right.m_verName;
  m_side=right.m_side;
  m_octant=right.m_octant;
  m_fullCW=right.m_fullCW;
  if(m_verName!="NA" && !tgcArgs()->useRun3Config()) this->readMap();  // read Coincidence Map for Run2 (6 thresholds)
}


TGCRPhiCoincidenceMap& TGCRPhiCoincidenceMap::operator=(const TGCRPhiCoincidenceMap& right)
{
   if (this != &right) {
    m_numberOfDR=right.m_numberOfDR;
    m_numberOfDPhi=right.m_numberOfDPhi;
    m_verName=right.m_verName;
    m_side=right.m_side;
    m_octant=right.m_octant;
    m_fullCW=right.m_fullCW;
    if(m_verName!="NA" && !tgcArgs()->useRun3Config()) this->readMap();  // read Coincidence Map for Run2 (6 thresholds)
   }

  return *this;
}

bool TGCRPhiCoincidenceMap::readMap() 
{
  const uint8_t kNMODULETYPE = 12;
  const uint8_t modulenumber[kNMODULETYPE] = {0, 1, 2, 2, 3, 4, 5, 5, 6, 7, 8, 8};
  const std::string modulename[kNMODULETYPE] = {"0","1","2a","2b","3","4","5a","5b","6","7","8a","8b"};
  const std::string sidename[NumberOfSide] = {"A","C"};

  IMessageSvc* msgSvc = 0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  if (svcLocator->service("MessageSvc", msgSvc) == StatusCode::FAILURE) {
    return false;
  }
  MsgStream log(msgSvc, "TGCRPhiCoincidenceMap::TGCRPhiCoincidenceMap");

  uint32_t octaddr = (m_side<<TGCTriggerData::SIDE_SHIFT) +
                     ((m_octant & TGCTriggerData::OCTANT_MASK)<<TGCTriggerData::OCTANT_SHIFT);

  // loop over all files...
  for(int iModule=0; iModule<kNMODULETYPE; iModule+=1) {

    std::ostringstream fn;
    fn << "RPhiCoincidenceMap.";
    if(m_fullCW) {
      if( (m_side>=0) && (m_side<NumberOfSide) &&
          (m_octant>=0) && (m_octant<NumberOfOctant)) {
        fn << sidename[m_side] << m_octant << ".";
      }
    }
    fn << "mod" << modulename[iModule] << "." << m_verName  << "._12.db";

    std::string fullname = PathResolver::find_file(fn.str().c_str(), "DATAPATH");
    if( fullname.length() == 0 ) {
      log << MSG::ERROR
          << " Could not found "
          << fn.str().c_str() << endmsg;
      return false ;
    }

    uint32_t phimod2 = (modulename[iModule].find("b") != std::string::npos) ? 1 : 0;
    uint32_t modaddr = ((modulenumber[iModule] & TGCTriggerData::MODULE_MASK)<<TGCTriggerData::MODULE_SHIFT) +
                       ((phimod2 & TGCTriggerData::PHIMOD2_MASK)<<TGCTriggerData::PHIMOD2_SHIFT);

    std::ifstream file(fullname.c_str(),std::ios::in);
    enum{BufferSize=1024};
    char buf[BufferSize];

    while(file.getline(buf,BufferSize)) {

      std::istringstream header(buf);
      std::string tag;

      header>>tag;
      if(tag=="#") { // read header part.

        uint16_t ptLevel, roi, mod;
        int16_t lDR, hDR, lDPhi, hDPhi;
        header >> ptLevel >> roi >> mod >> lDR >> hDR >> lDPhi >> hDPhi;

        int16_t type = getTYPE( lDR, hDR, lDPhi, hDPhi );

        // check moduleNumber and ptLevel
        if(mod != modulenumber[iModule] || ptLevel > N_PT_THRESH || type<0 ) {
          log << MSG::WARNING
              << " illegal parameter in database header : "
              << header.str()
              << " in file " << fn.str()
              << endmsg;
          break;
        }

        uint32_t cwaddr = ((uint8_t(type) & TGCTriggerData::TYPE_MASK)<<TGCTriggerData::TYPE_SHIFT) + 
                          ((roi & TGCTriggerData::ROI_MASK)<<TGCTriggerData::ROI_SHIFT);

        // get window data
        file.getline(buf,BufferSize);
        std::istringstream cont(buf);

        for(uint8_t ir=0; ir<=hDR-DR_offset; ir++) {
          uint32_t draddr = (ir & TGCTriggerData::DR_MASK)<<TGCTriggerData::DR_SHIFT;

          uint32_t bit;
	  cont >> bit;
	  if (bit == 0) continue;   // none of window is opened in this dR

          for(uint8_t iphi=0; iphi<hDPhi-DPhi_offset; iphi++) {
            if(bit>>iphi & 0x1) {
              uint32_t theaddr = octaddr + modaddr + cwaddr + draddr + iphi;
              m_ptmap[theaddr] = (uint8_t)(ptLevel & TGCTriggerData::PT_MASK);
            }
          }
	}
      }
    }
  }

  return true;
}


} //end of namespace bracket
