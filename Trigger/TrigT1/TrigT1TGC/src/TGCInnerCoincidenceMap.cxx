/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

#include "TrigT1TGC/TGCInnerCoincidenceMap.hh"
#include "TrigT1TGC/TGCDatabaseManager.hh"
#include "PathResolver/PathResolver.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"


namespace LVL1TGCTrigger {

 extern bool        g_USE_INNER;
 extern bool        g_USE_CONDDB;

TGCInnerCoincidenceMap::TGCInnerCoincidenceMap()
  :m_verName("NA"),
   m_side(0),
   m_fullCW(false),
   m_readCondKey("TGCTriggerData")
{
   StatusCode sc = m_readCondKey.initialize();
   if (sc.isFailure()) {
     return;
   }

  // intialize map
  for (size_t sec=0; sec< N_EndcapSector; sec++){
    for (size_t ssc=0; ssc< N_Endcap_SSC; ssc++){
      for (size_t input=0; input< N_Input_InnerSector; input++){
	map[input][ssc][sec].setTriggerBits(true);
      }
      flagPT[0][ssc][sec] =0; //pt1     
      flagPT[1][ssc][sec] =0; //pt2     
      flagPT[2][ssc][sec] =0; //pt3     
      flagPT[3][ssc][sec] =0; //pt4     
      flagPT[4][ssc][sec] =1; //pt5     
      flagPT[5][ssc][sec] =1; //pt6     
      for (size_t pos=0; pos< N_ROI_IN_SSC; pos++){
	flagROI[pos][ssc][sec] = 1;
      }
    }
  }
  return;
}
   
  TGCInnerCoincidenceMap::TGCInnerCoincidenceMap(const std::string& version,
						 int   sideID)
  :m_verName(version),
   m_side(sideID),
   m_fullCW(false),
   m_readCondKey("TGCTriggerData")
{
   StatusCode sc = m_readCondKey.initialize();
   if (sc.isFailure()) {
     return;
   }

  // intialize map
  for (size_t sec=0; sec< N_EndcapSector; sec++){
    for (size_t ssc=0; ssc< N_Endcap_SSC; ssc++){
      for (size_t input=0; input< N_Input_InnerSector; input++){
	map[input][ssc][sec].setTriggerBits(true);
      }
      flagPT[0][ssc][sec] =0; //pt1     
      flagPT[1][ssc][sec] =0; //pt2     
      flagPT[2][ssc][sec] =0; //pt3     
      flagPT[3][ssc][sec] =0; //pt4     
      flagPT[4][ssc][sec] =1; //pt5     
      flagPT[5][ssc][sec] =1; //pt6     
      for (size_t pos=0; pos< N_ROI_IN_SSC; pos++){
	flagROI[pos][ssc][sec] = 1;
      }
    }
  }

  if (!g_USE_INNER) return;
  if (g_USE_CONDDB) return;
 
  //////////////////////////////
  IMessageSvc* msgSvc = 0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  if (svcLocator->service("MessageSvc", msgSvc) == StatusCode::FAILURE) {
    return;
  }
  MsgStream log(msgSvc, "TGCInnerCoincidenceMap::TGCInnerCoincidenceMap");

  // use full CW (i.e. different maps for each side)
  m_fullCW = (m_verName == "v07");

  // read Inner Coincidence Map 
  if (this->readMap()) {
    log << MSG::INFO 
      << " TGC Inner CW version of " << m_verName << " is selected " << endmsg;
  } else {
    log << MSG::INFO  
	<< " NOT use inner station " << endmsg;
    g_USE_INNER = false;
    for (size_t sec=0; sec< N_EndcapSector; sec++){
      for (size_t ssc=0; ssc< N_Endcap_SSC; ssc++){
	flagPT[0][ssc][sec] =0; //pt1     
	flagPT[1][ssc][sec] =0; //pt2     
	flagPT[2][ssc][sec] =0; //pt3     
	flagPT[3][ssc][sec] =0; //pt4     
	flagPT[4][ssc][sec] =0; //pt5     
	flagPT[5][ssc][sec] =0; //pt6     
      }
    }    
  }

  // for debug  
  //dumpMap();
}


TGCInnerCoincidenceMap::~TGCInnerCoincidenceMap()
{
}

TGCInnerCoincidenceMap::TGCInnerCoincidenceMap(const TGCInnerCoincidenceMap& right)
  : m_readCondKey("TGCTriggerData")
{
  for (size_t sec=0; sec< N_EndcapSector; sec++){
    for (size_t ssc=0; ssc< N_Endcap_SSC; ssc++){
      for (size_t input=0; input< N_Input_InnerSector; input++){
	map[input][ssc][sec] = right.map[input][ssc][sec];
      }
      flagPT[0][ssc][sec] = right.flagPT[0][ssc][sec];
      flagPT[1][ssc][sec] = right.flagPT[1][ssc][sec];
      flagPT[2][ssc][sec] = right.flagPT[2][ssc][sec];
      flagPT[3][ssc][sec] = right.flagPT[3][ssc][sec];
      flagPT[4][ssc][sec] = right.flagPT[4][ssc][sec];
      flagPT[5][ssc][sec] = right.flagPT[5][ssc][sec];
      for (size_t pos=0; pos< N_ROI_IN_SSC; pos++){
	flagROI[pos][ssc][sec] = right.flagROI[pos][ssc][sec];
      }
    }
  }
  m_verName = right.m_verName;
  m_side=right.m_side;
  m_fullCW=right.m_fullCW;
}

TGCInnerCoincidenceMap& TGCInnerCoincidenceMap::operator=(const TGCInnerCoincidenceMap& right)
{
  if (this != &right) {
    for (size_t sec=0; sec< N_EndcapSector; sec++){
      for (size_t ssc=0; ssc< N_Endcap_SSC; ssc++){
	for (size_t input=0; input< N_Input_InnerSector; input++){
	  map[input][ssc][sec] = right.map[input][ssc][sec];
	}
	flagPT[0][ssc][sec] = right.flagPT[0][ssc][sec];
	flagPT[1][ssc][sec] = right.flagPT[1][ssc][sec];
	flagPT[2][ssc][sec] = right.flagPT[2][ssc][sec];
	flagPT[3][ssc][sec] = right.flagPT[3][ssc][sec];
	flagPT[4][ssc][sec] = right.flagPT[4][ssc][sec];
	flagPT[5][ssc][sec] = right.flagPT[5][ssc][sec];
	for (size_t pos=0; pos< N_ROI_IN_SSC; pos++){
	  flagROI[pos][ssc][sec] = right.flagROI[pos][ssc][sec];
	}
      }
    }
    m_verName = right.m_verName;
    m_side=right.m_side;
    m_fullCW=right.m_fullCW;
  }
  return  *this;
}


bool TGCInnerCoincidenceMap::readMap() 
{
  const std::string SideName[NumberOfSide] = {"A","C"};

  IMessageSvc* msgSvc = 0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  if (svcLocator->service("MessageSvc", msgSvc) == StatusCode::FAILURE) {
    return false;
  }
  MsgStream log(msgSvc, "TGCInnerCoincidenceMap::TGCInnerCoincidenceMap");

  // select right database according to a set of thresholds
  std::string dbname="";
  if (!m_fullCW) {
    dbname = "InnerCoincidenceMap." + m_verName + "._12.db";
  } else {
    dbname = "InnerCoincidenceMap." + SideName[m_side]
               + "." + m_verName + "._12.db";
  }

  //----- 
  
  std::string fullName;
  fullName = PathResolver::find_file( dbname.c_str(), "DATAPATH" );
  bool isFound =( fullName.length() > 0 );
  if( !isFound) {
    log << MSG::WARNING 
	<< " Could not found " << dbname << endmsg;
    return false ;  
  } 

  std::ifstream file(fullName.c_str(),std::ios::in);    

  enum{BufferSize=1024};
  char buf[BufferSize];
  std::string tag;

  while(file.getline(buf,BufferSize)){
    int sectorId = -1;
    int sscId    = -1;
    int use[N_PT_THRESH] = {0, 0, 0, 0, 0, 0};
    int roi[N_ROI_IN_SSC] = {1, 1, 1, 1, 1, 1, 1, 1};
    std::istringstream header(buf); 
    header >> tag;
    if(tag=="#"){ // read header part.     
      header >> sectorId >> sscId 
	     >> use[0] >> use[1] >> use[2] 
	     >> use[3] >> use[4] >> use[5] 
	     >> roi[0] >> roi[1] >> roi[2] >> roi[3]
	     >> roi[4] >> roi[5] >> roi[6] >> roi[7];
    }
    // check Id
    if( sectorId<0 || sectorId>=N_EndcapSector ||
	sscId<0    || sscId>=N_Endcap_SSC ) {
      log << MSG::WARNING 
	  << " illegal parameter in database header : " << header.str()
	  << " in file " << dbname
	  << endmsg;
      file.close();
      return false;
    }
    for (size_t pt=0; pt<N_PT_THRESH; pt++){
      flagPT[pt][sscId][sectorId] = use[pt];
    }
    for (size_t pos=0; pos< N_ROI_IN_SSC; pos++){
      flagROI[pos][sscId][sectorId] = roi[pos];
    }

    // get trigger word
    file.getline(buf,BufferSize);
    std::istringstream cont(buf);
    unsigned int word;
    for(size_t pos=0; pos<N_Input_InnerSector; pos++){
      cont >> word;
      map[pos][sscId][sectorId].setTriggerWord(word);
    }
  }
  file.close();	  

  return true;
}

// Debug purpose only
void TGCInnerCoincidenceMap::dumpMap() const
{
  // select right database according to a set of thresholds
  std::string fullName="InnerCoincidenceMap."+m_verName+"._12.out";

  std::ofstream file(fullName.c_str());    

  for (size_t sec=0; sec< N_EndcapSector; sec++){
    for (size_t ssc=0; ssc< N_Endcap_SSC; ssc++){
      file << "# " << sec << " " << ssc << " ";
      for(int i=0; i<6; i++) file << flagPT[i][ssc][sec] << " ";
      for(int i=0; i<8; i++) file << flagROI[i][ssc][sec] << " ";
      file << std::endl;
      file << map[0][ssc][sec].getTriggerWord() << " "
	   << map[1][ssc][sec].getTriggerWord() << " "
	   << map[2][ssc][sec].getTriggerWord() << " "
	   << map[3][ssc][sec].getTriggerWord() << " "
	   << std::endl;
    }
  }
  file.close();	  
}

int TGCInnerCoincidenceMap::getFlagPT(const int pt,
                                      const int ssc,
                                      const int sec)  const
{
  if ((pt<=0)||(pt>N_PT_THRESH)) return -1;
  if ((ssc<0)||(ssc>=N_Endcap_SSC)) return 0;
  if ((sec<0)||(sec>=N_EndcapSector)) return -1;

  if  (g_USE_CONDDB) {
    SG::ReadCondHandle<TGCTriggerData> readHandle{m_readCondKey};
    const TGCTriggerData* readCdo{*readHandle};
    return readCdo->getFlagPtEifi(m_side,pt-1,ssc,sec);
  } else {
    return  flagPT[pt-1][ssc][sec];
  }
}

int  TGCInnerCoincidenceMap::getFlagROI(const int roi,
                                        const int ssc,
                                        const int sec)  const
{
  if ((roi<0)||(roi>=N_ROI_IN_SSC)) return -1;
  if ((ssc<0)||(ssc>=N_Endcap_SSC)) return 0;
  if ((sec<0)||(sec>=N_EndcapSector)) return -1;

  if  (g_USE_CONDDB) {
    SG::ReadCondHandle<TGCTriggerData> readHandle{m_readCondKey};
    const TGCTriggerData* readCdo{*readHandle};
    return readCdo->getFlagRoiEifi(m_side,roi,ssc,sec);
  } else {
    return  flagROI[roi][ssc][sec];
  }
}

int TGCInnerCoincidenceMap::getTriggerBit(const int slot,
                  const int ssc,
                  const int sec,
                  const int reg,
                  const int read,
                  const int bit) const
{
  if  (g_USE_CONDDB) {
    SG::ReadCondHandle<TGCTriggerData> readHandle{m_readCondKey};
    const TGCTriggerData* readCdo{*readHandle};
    return readCdo->getTrigBitEifi(m_side,slot,ssc,sec,reg,read,bit);
  } else {
    return map[slot][ssc][sec].getTriggerBit(reg,read,bit);
  }
}
} //end of namespace bracket
