/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_MUONROICHAINTESTALG_H
#define TRIGGER_MUONROICHAINTESTALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrigMuonCoinHierarchy/MuonRoiChain.h"

class ITHistSvc;
class TH2I;

namespace Trigger {
  class IMuonRoiChainSvc;

  class MuonRoiChainTestAlg : public AthAlgorithm {
  public:
    /** Constructor */
    MuonRoiChainTestAlg(const std::string& name, ISvcLocator* pSvcLocator);

    /** Initialize */
    StatusCode initialize();
    /** Process one event */
    StatusCode execute();
    /** Finalize */
    StatusCode finalize();

  private:
    /** The number of trigger sectors */
    enum NSECTORS { 
      NSIDES = 2, 
      NENDCAPTRIGGERSECTORS = 48, 
      NFORWARDTRIGGERSECTORS = 24, 
      NTRIGGERSECTORS = NSIDES*(NENDCAPTRIGGERSECTORS+NFORWARDTRIGGERSECTORS) 
    }; 
    
    /** IMuonRoiChainSvc */
    IMuonRoiChainSvc* m_muonRoiChainSvc;

    /** ITHistSvc */
    ITHistSvc* m_thistSvc;

    /** TH2I for the relationship of Muon_ROI (LVL1_ROI) -> MuCTPI_RDO dataWord (MUCTPI_RDO) */
    TH2I *m_h_ROI_RDO; 
    /** TH2I for the relationship of MuCTPI_RDO dataWord (MUCTPI_RDO) -> Muon_ROI (LVL1_ROI) */
    TH2I *m_h_RDO_ROI; 

    /** TH2I for the relationship of MuCTPI_RDO dataWord (MUCTPI_RDO) 
	-> TgcCoinData (Sector Logic) (TrigT1CoinDataCollection) */  
    TH2I *m_h_RDO_TgcCoinData;
    /** TH2I for the relationship of TgcCoinData (Sector Logic) (TrigT1CoinDataCollection) 
	-> MuCTPI_RDO dataWord (MUCTPI_RDO) */ 
    TH2I *m_h_TgcCoinData_RDO;
  };
}

#endif // TRIGGER_MUONROICHAINTESTALG_H
