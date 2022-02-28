/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

#include "TrigT1TGC/TGCBIS78CoincidenceMap.h"
#include "TrigT1TGC/BIS78TrigOut.h"
#include "TrigT1TGC/TGCDatabaseManager.h"
#include "PathResolver/PathResolver.h"

#include "MuonCondInterface/ITGCTriggerDbTool.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include "TrigT1TGC/TGCArguments.h"

namespace LVL1TGCTrigger {


  TGCBIS78CoincidenceMap::TGCBIS78CoincidenceMap(TGCArguments* tgcargs,const std::string& version)
    :m_verName(version),
     m_condDbTool("TGCTriggerDbTool"),
     m_tgcArgs(tgcargs)
  {

    m_msg = Athena::MsgStreamMember("LVL1TGCTrigger::TGCBIS78CoincidenceMap::TGCBIS78CoincidenceMap");
    m_msg.get().setLevel(tgcArgs()->MSGLEVEL());

    if(!tgcArgs()->USE_BIS78()){return;}

    // initialize map
    for (size_t sec=0; sec< N_EndcapSector; sec++){
      for (size_t ssc=0; ssc< N_Endcap_SSC; ssc++){
	for (size_t pos=0; pos< N_ROI_IN_SSC; pos++){
	  m_flagROI[pos][ssc][sec] = 1;
	}
      }
    }

    for(int dr=0;dr!=N_dEta;dr++){
      for(int dphi=0;dphi!=N_dPhi;dphi++){
	m_CW[dr][dphi].resize(m_NumberOfRoI[m_region]);
	std::fill(m_CW[dr][dphi].begin(), m_CW[dr][dphi].end(),0);
      }
    }

    //---------Read out CW data---------
    if(!this->readMap() ){
      // BIS78 trigger flag is set to false when the map reading failed.
      tgcArgs()->set_USE_BIS78(false);
    }

    if (this->readMap()) {
      m_msg << MSG::INFO 
	    << " TGC BIS78 CW version of " << m_verName << " is selected " << endmsg;
    } else {
      m_msg << MSG::INFO  
	    << " NOT use BIS78 " << endmsg;
    }
  }

  TGCBIS78CoincidenceMap::~TGCBIS78CoincidenceMap()
  {
  }



  //TGC-BIS78 Eta-Phi Coincidence
  int TGCBIS78CoincidenceMap::TGCBIS78_pt(const BIS78TrigOut *bis78Out,int /*roi*/) const
  {
    std::vector<uint8_t> bis78Eta_vec=bis78Out->getBIS78eta();
    std::vector<uint8_t> bis78Phi_vec=bis78Out->getBIS78phi();
    std::vector<uint8_t> bis78Deta_vec=bis78Out->getBIS78Deta();
    std::vector<uint8_t> bis78Dphi_vec=bis78Out->getBIS78Dphi();
    std::vector<uint8_t> bis78flag3over3eta_vec=bis78Out->getBIS78flag3over3eta();
    std::vector<uint8_t> bis78flag3over3phi_vec=bis78Out->getBIS78flag3over3phi();
    
    int pt=0;
    // temporal algorithm
    for(unsigned int bis78hit_id=0;bis78hit_id!=bis78Eta_vec.size();bis78hit_id++){
      // calculate dR, dPhi bin # from eta/phiIndex?
      // for now, if there is a hit at BIS78, it returns true always
      pt=1;
    }
    
    return pt;
  }

  int TGCBIS78CoincidenceMap::getFlagROI(const int roi,
					 const int ssc,
					 const int sec,
					 const int side) const
  {
    if (side==0) return 0; // BIS78 only in A side
    if ((roi<0)||(roi>=N_ROI_IN_SSC)) return -1;
    if ((ssc<0)||(ssc>=N_Endcap_SSC)) return 0;
    if ((sec<0)||(sec>=N_EndcapSector)) return -1;

    // which inner LUT will be implemented
    return m_flagROI[roi][ssc][sec];
  }

  bool TGCBIS78CoincidenceMap::readMap() 
  {
    // no LUT available for TGC-BIS78 coincidence
    // just return true for now
    
    return true;
  } 
  
} //end of namespace bracket
