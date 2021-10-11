/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/BigWheelCoincidenceLUT.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "TrigT1TGC/TGCNumbering.h"
#include "PathResolver/PathResolver.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/ReadCondHandle.h"
#include "MuonCondSvc/TGCTriggerData.h"

namespace LVL1TGC {

int BigWheelCoincidenceLUT::test(int octantId, int moduleId, int subsector, 
                                 int type, int dr, int dphi) const {
  // check pt range
  if (type<0 || type>=N_TMap ) return false; 

  int sector=(moduleId-2)/3+octantId*3;
  int phimod2 = (moduleId==2||moduleId==5||moduleId==8)&&(sector%2==1) ? 1 : 0;
  int addr=SUBSECTORADD(subsector, moduleId, phimod2,type);

  std::map<int, std::map<int, std::map<int, char> > > themap;

  if (tgcArgs()->USE_CONDDB()) {
    SG::ReadCondHandle<TGCTriggerLUTs> readHandle{m_readCondKey};
    const TGCTriggerLUTs* readCdo{*readHandle};
    themap = readCdo->getReadMapBw(m_side, m_octant);
  } else {
    themap = m_lut;
  }

  std::map<int, std::map<int, std::map<int, char> > >::const_iterator it = themap.find(addr);
  if (it==themap.end()) return false;

  std::map<int, std::map<int, char> > mapR = it->second;
  std::map<int, std::map<int, char> >::const_iterator itR=mapR.find(dr);
  if (itR==mapR.end()) return false;

  std::map<int, char> mapPhi = itR->second;
  std::map<int, char>::const_iterator itPhi=mapPhi.find(dphi);
  if (itPhi==mapPhi.end()) return false;

  char pT_char=itPhi->second;
  int pT_int=m_pTdef.find(pT_char)->second;

  return  pT_int;
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

  if (!tgcArgs()->useRun3Config()) {
    log << MSG::ERROR
        << "This class should be used only for Run-3 configuration!"
        << endmsg;
  }

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
  const int moduleNumber[NumberOfModuleType]  =
    {  0,  1,   2,   2,  3,  4,   5,   5,  6,  7,   8,  8 };
  const std::string moduleName[NumberOfModuleType]=
    {"0a","1a","2a","2b","3a","4a","5a","5b","6a","7a","8a","8b"};
  const std::string sideName[kNSide] = {"a","c"};
  const std::string octantName[kNOctant] =
    {  "0", "1", "2", "3", "4", "5", "6", "7"};
  const std::string coincidenceTypeName[NumberOfCoincidenceType] = {"HH","HL","LH","LL"};


  IMessageSvc* msgSvc = 0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  if (svcLocator->service("MessageSvc", msgSvc) == StatusCode::FAILURE) {
    return false;
  }
  MsgStream log(msgSvc, "LVL1TGC::BigWheelCoincidenceLUT");

  // initialize
  std::string buf;
  std::string fn, fullName, tag;
  int ssId;
  char delimiter = '\n';

  // loop over all files...
  for(int iModule=0; iModule<NumberOfModuleType; iModule+=1) {
    for(int iCoinType=0; iCoinType!=NumberOfCoincidenceType; iCoinType++){
      int phimod2=moduleName[iModule].find("b")!=std::string::npos ? 1 : 0;
      std::string fn = "/BW/cm_";
      if (m_fullCW) {
        fn += sideName[m_side]+octantName[m_octant]+moduleName[iModule]+coincidenceTypeName[iCoinType]+"_";
      } else {
        fn += sideName[m_side]+moduleName[iModule]+coincidenceTypeName[iCoinType]+"_Octant_";
      }
      fn += m_verName + ".db";

      int type = -1;
      int lDR, hDR, lDPhi, hDPhi;

      fullName = PathResolver::FindCalibDirectory("dev")+"/TrigT1TGC"+fn;
      if( fullName.length() == 0 ) { 
        log << MSG::ERROR
            << " Could not found "
            << fn.c_str()
            <<endmsg;
        continue;  
      } 

      std::ifstream file(fullName.c_str(),std::ios::in);   
      if(!file){
        log << MSG::ERROR
            << " Could not found "
            << fullName.c_str()
            <<endmsg;
        continue; 
      } 
      while(getline(file,buf,delimiter)){
        std::istringstream header(buf);
        header>>tag;
        if(tag=="#") { // read header part.     
          header>>ssId>>lDR>>hDR>>lDPhi>>hDPhi;
          type = getTYPE( lDR, hDR, lDPhi, hDPhi );
          // check moduleNumber and ptLevel
          if( type<0 ) {
            log << MSG::WARNING
                << " illegal parameter in database header : "
                <<  header.str()
                <<" in file "
                << fn
                <<endmsg;
            break;
          }

          // get window data
          std::map<int, std::map<int, char> >  bWindow;//<R,<~>>
          char pT;
          for(int ir=0; ir<=hDR-lDR; ir++) {
            getline(file,buf,delimiter);
            std::map<int, char> aWindow;//<Phi,pT>
            for(int iphi=0; iphi<=hDPhi-lDPhi; iphi++){
              pT = buf[iphi];
              if (pT=='X'){continue;} // none of window is opened in this dR
              aWindow[iphi+DPhi_offset] = pT;
            }
            // Warning : no window 
            if (aWindow.size()==0) {
              log << MSG::DEBUG
                  << " No window is opened for (ptLevel,ssId,mod) = ("
                  <<  ssId
                  <<" ) "
                  <<endmsg;
            } else {
              bWindow[ir+DR_offset]=aWindow;
            }
          }

          int addr = SUBSECTORADD(ssId,moduleNumber[iModule],phimod2,type);
          if (m_lut.find(addr)!=m_lut.end()) {
            log << MSG::DEBUG
                << " This subsector was already reserved."
                <<endmsg;
          } else {
            m_lut[addr]=bWindow;
          }
        }
      }
      
    }
  }
  return true;

}


} //end of namespace bracket
