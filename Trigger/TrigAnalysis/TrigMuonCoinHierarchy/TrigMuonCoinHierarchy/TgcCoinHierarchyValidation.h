/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_TGCCOINHIERARCHYVALIDATION_H
#define TRIGGER_TGCCOINHIERARCHYVALIDATION_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrigMuonCoinHierarchy/TgcCoinHierarchy.h"
#include "TrigMuonCoinHierarchy/TgcCoinHierarchyClassifyTool.h"
#include "MuonRDO/TgcRdo.h"

//#include "TGCcablingInterface/ITGCcablingServerSvc.h"
//#include "MuonReadoutGeometry/MuonDetectorManager.h"
//#include "MuonIdHelpers/TgcIdHelper.h"

#include <iostream>
#include <fstream>
#include <string>

#define dbg(){std::cout<<"dbg>>"<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;}

class TgcIdHelper;
class ITGCcablingSvc;

namespace Trigger {
  class ITgcCoinHierarchySvc;

  class TgcCoinHierarchyValidation : public AthAlgorithm {
  public:
    /** Constructor */
    TgcCoinHierarchyValidation(const std::string& name, ISvcLocator* pSvcLocator);

    /** Initialize */
    StatusCode initialize();
    /** Process one event */
    StatusCode execute();
    /** Finalize */
    StatusCode finalize();

    /** DO ANALYSIS */
    bool doAnalysis();
    bool doAnalysisHit();

  private:
    /** The number of trigger sectors */
    enum NSECTORS { 
      NSIDES = 2, 
      NENDCAPTRIGGERSECTORS = 48, 
      NFORWARDTRIGGERSECTORS = 24, 
      NTRIGGERSECTORS = NSIDES*(NENDCAPTRIGGERSECTORS+NFORWARDTRIGGERSECTORS) 
    }; 

    /* FLAGS */
    bool m_doCoin;
    bool m_doHit;

    /** ITgcCoinHierarchySvc */
    ITgcCoinHierarchySvc* m_tgcCoinSvc;

    /** TgcIdHelper and TgcCablingSvc */
    const TgcIdHelper *m_tgcHelper;
    const ITGCcablingSvc *m_tgcCabling;

    // TIMING
    std::string m_timingName;
    unsigned int m_bc;

    // others
    int getSTATION(const Identifier id);
    int getStEta(const Identifier id);
    int getStPhi(const Identifier id);
    int getGasgap(const Identifier id);
    int getChannel(const Identifier id);
    int getIsStrip(const Identifier id);

    /* make log */
    std::ofstream *m_ofs;
    bool makeLog(const char *msg);
    std::string m_logName;


  };
}

#endif // TRIGGER_TGCCOINHIERARCHYVALIDATION_H
