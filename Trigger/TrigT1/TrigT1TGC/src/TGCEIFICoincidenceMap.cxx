/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCEIFICoincidenceMap.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

#include "TrigT1TGC/TGCDatabaseManager.h"
#include "PathResolver/PathResolver.h"
#include "AthenaKernel/getMessageSvc.h"

namespace LVL1TGCTrigger {

TGCEIFICoincidenceMap::TGCEIFICoincidenceMap(TGCArguments* tgcargs,
                                             const SG::ReadCondHandleKey<TGCTriggerData>& readCondKey)
  :AthMessaging(Athena::getMessageSvc(), "LVL1TGC::TGCEIFICoincidenceMap"),
   m_verName("NA"),
   m_side(0),
   m_fullCW(false),
   m_tgcArgs(tgcargs),
   m_readCondKey(readCondKey)
{
  setLevel(tgcArgs()->MSGLEVEL());

  // intialize map
  ATH_MSG_DEBUG("initialising the map");
  for (size_t sec=0; sec< N_EndcapSector; sec++){
    for (size_t ssc=0; ssc< N_Endcap_SSC; ssc++){
      for (size_t input=0; input< N_Input_InnerSector; input++){
	m_map[input][ssc][sec].setTriggerBits(true);
      }
      m_flagPT[ssc][sec] = std::bitset<N_PT_THRESH>(0x30);  // 6b'110000

      for (size_t pos=0; pos< N_ROI_IN_SSC; pos++){
	m_flagROI[pos][ssc][sec] = 1;
      }
    }
  }
  ATH_MSG_DEBUG("end of initialisation");

  return;
}
   
TGCEIFICoincidenceMap::TGCEIFICoincidenceMap(TGCArguments* tgcargs,
					     const SG::ReadCondHandleKey<TGCTriggerData>& readCondKey,
                                             const std::string& version,
					     int   sideID)
 : AthMessaging(Athena::getMessageSvc(), "LVL1TGC::TGCEIFICoincidenceMap"),
   m_verName(version),
   m_side(sideID),
   m_fullCW(false),
   m_tgcArgs(tgcargs),
   m_readCondKey(readCondKey)
{
  setLevel(tgcArgs()->MSGLEVEL());

  // initialize map
  ATH_MSG_DEBUG("initialising the map");
  for (size_t sec=0; sec< N_EndcapSector; sec++){
    for (size_t ssc=0; ssc< N_Endcap_SSC; ssc++){
      for (size_t input=0; input< N_Input_InnerSector; input++){
	m_map[input][ssc][sec].setTriggerBits(true);
      }
      m_flagPT[ssc][sec] = std::bitset<N_PT_THRESH>(0x30);  // 6b'110000

      for (size_t pos=0; pos< N_ROI_IN_SSC; pos++){
	m_flagROI[pos][ssc][sec] = 1;
      }
    }
  }
  ATH_MSG_INFO("USE_INNER = " << tgcArgs()->USE_INNER());

  if (!tgcArgs()->USE_INNER()) return;

  ATH_MSG_INFO("USE_CONDDB = " << tgcArgs()->USE_CONDDB());

  if (tgcArgs()->USE_CONDDB()) return;

  // use full CW (i.e. different maps for each side)
  m_fullCW = (m_verName == "v07");

  // read Inner Coincidence Map 
  if (this->readMap()) {
    ATH_MSG_INFO(" TGC EIFI CW version of " << m_verName << " is selected.");
  } else {
    ATH_MSG_INFO(" NOT use inner station ");
    tgcArgs()->set_USE_INNER( false );
    for (size_t sec=0; sec< N_EndcapSector; sec++){
      for (size_t ssc=0; ssc< N_Endcap_SSC; ssc++){
        m_flagPT[ssc][sec] = std::bitset<N_PT_THRESH>(0x00);  // 6b'000000
      }
    }    
  }

  // for debug  
  //dumpMap();
}


TGCEIFICoincidenceMap::TGCEIFICoincidenceMap(const TGCEIFICoincidenceMap& right)
 : AthMessaging(Athena::getMessageSvc(), "LVL1TGC::TGCEIFICoincidenceMap"),
   m_readCondKey(right.m_readCondKey)
{
  ATH_MSG_DEBUG("copy constructor");

  for (size_t sec=0; sec< N_EndcapSector; sec++){
    for (size_t ssc=0; ssc< N_Endcap_SSC; ssc++){
      for (size_t input=0; input< N_Input_InnerSector; input++){
	m_map[input][ssc][sec] = right.m_map[input][ssc][sec];
      }
      m_flagPT[ssc][sec] = right.m_flagPT[ssc][sec];

      for (size_t pos=0; pos< N_ROI_IN_SSC; pos++){
	m_flagROI[pos][ssc][sec] = right.m_flagROI[pos][ssc][sec];
      }
    }
  }
  m_verName = right.m_verName;
  m_side=right.m_side;
  m_fullCW=right.m_fullCW;
}

TGCEIFICoincidenceMap& TGCEIFICoincidenceMap::operator=(const TGCEIFICoincidenceMap& right)
{
  ATH_MSG_INFO("operator = ");

  if (this != &right) {
    for (size_t sec=0; sec< N_EndcapSector; sec++){
      for (size_t ssc=0; ssc< N_Endcap_SSC; ssc++){
	for (size_t input=0; input< N_Input_InnerSector; input++){
	  m_map[input][ssc][sec] = right.m_map[input][ssc][sec];
	}
        m_flagPT[ssc][sec] = right.m_flagPT[ssc][sec];

	for (size_t pos=0; pos< N_ROI_IN_SSC; pos++){
	  m_flagROI[pos][ssc][sec] = right.m_flagROI[pos][ssc][sec];
	}
      }
    }
    m_verName = right.m_verName;
    m_side=right.m_side;
    m_fullCW=right.m_fullCW;
  }
  return  *this;
}


bool TGCEIFICoincidenceMap::readMap() 
{
  ATH_MSG_DEBUG("readMap");
  const std::string SideName[NumberOfSide] = {"A","C"};

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
    ATH_MSG_WARNING(" Could not found " << dbname);
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
      ATH_MSG_WARNING(" illegal parameter in database header : " << header.str()
	  << " in file " << dbname);
      file.close();
      return false;
    }
    for (size_t pt=0; pt<N_PT_THRESH; pt++){
      m_flagPT[sscId][sectorId][pt] = use[pt];
    }
    for (size_t pos=0; pos< N_ROI_IN_SSC; pos++){
      m_flagROI[pos][sscId][sectorId] = roi[pos];
    }

    // get trigger word
    file.getline(buf,BufferSize);
    std::istringstream cont(buf);
    unsigned int word;
    for(size_t pos=0; pos<N_Input_InnerSector; pos++){
      cont >> word;
      m_map[pos][sscId][sectorId].setTriggerWord(word);
    }
  }
  file.close();	  

  return true;
}

// Debug purpose only
void TGCEIFICoincidenceMap::dumpMap() const
{
  // select right database according to a set of thresholds
  std::string fullName="InnerCoincidenceMap."+m_verName+"._12.out";

  std::ofstream file(fullName.c_str());    

  for (size_t sec=0; sec< N_EndcapSector; sec++){
    for (size_t ssc=0; ssc< N_Endcap_SSC; ssc++){
      file << "# " << sec << " " << ssc << " ";
      for(int i=0; i<6; i++) file << m_flagPT[ssc][sec].test(i) << " ";
      for(int i=0; i<8; i++) file << m_flagROI[i][ssc][sec] << " ";
      file << std::endl;
      file << m_map[0][ssc][sec].getTriggerWord() << " "
	   << m_map[1][ssc][sec].getTriggerWord() << " "
	   << m_map[2][ssc][sec].getTriggerWord() << " "
	   << m_map[3][ssc][sec].getTriggerWord() << " "
	   << std::endl;
    }
  }
  file.close();	  
}

int TGCEIFICoincidenceMap::getFlagPT(const int pt,
                                     const int ssc,
                                     const int sec)  const
{
  if ((pt<=0)||(pt>N_PT_THRESH)) return -1;
  if ((ssc<0)||(ssc>=N_Endcap_SSC)) return 0;
  if ((sec<0)||(sec>=N_EndcapSector)) return -1;

  if  (tgcArgs()->USE_CONDDB()) {
    SG::ReadCondHandle<TGCTriggerData> readHandle{m_readCondKey};
    const TGCTriggerData* readCdo{*readHandle};
    return readCdo->getFlagPtEifi(m_side,pt-1,ssc,sec);
  } else {
    return  m_flagPT[ssc][sec].test(pt-1);
  }
}

int  TGCEIFICoincidenceMap::getFlagROI(const int roi,
                                       const int ssc,
                                       const int sec)  const
{
  if ((roi<0)||(roi>=N_ROI_IN_SSC)) return -1;
  if ((ssc<0)||(ssc>=N_Endcap_SSC)) return 0;
  if ((sec<0)||(sec>=N_EndcapSector)) return -1;

  if  (tgcArgs()->USE_CONDDB()) {
    SG::ReadCondHandle<TGCTriggerData> readHandle{m_readCondKey};
    const TGCTriggerData* readCdo{*readHandle};
    return readCdo->getFlagRoiEifi(m_side,roi,ssc,sec);
  } else {
    return  m_flagROI[roi][ssc][sec];
  }
}

int TGCEIFICoincidenceMap::getTriggerBit(const int slot,
                                         const int ssc,
                                         const int sec,
                                         const int reg,
                                         const int read,
                                         const int bit) const
{
  if  (tgcArgs()->USE_CONDDB()) {
    SG::ReadCondHandle<TGCTriggerData> readHandle{m_readCondKey};
    const TGCTriggerData* readCdo{*readHandle};
    return readCdo->getTrigBitEifi(m_side,slot,ssc,sec,reg,read,bit);
  } else {
    return m_map[slot][ssc][sec].getTriggerBit(reg,read,bit);
  }
}

}   // end of namespace
