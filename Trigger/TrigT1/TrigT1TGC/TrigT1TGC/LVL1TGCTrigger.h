/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1TGC_LVL1TGCTRIGGER_H
#define TRIGT1TGC_LVL1TGCTRIGGER_H

#include "AthenaBaseComps/AthAlgorithm.h"

// STL
#include <string>
#include <vector>
#include <map>

// Gaudi includes
#include "Gaudi/Property.h"

// Other stuff
#include "TrigT1Interfaces/SlinkWord.h"
#include "TrigT1Interfaces/Lvl1MuCTPIInput.h"
#include "TrigT1Interfaces/Lvl1MuCTPIInputPhase1.h"
#include "Identifier/Identifier.h"

// EIFI-SL connection
#include "TrigT1TGC/TGCInnerTrackletSlotHolder.h"

#include "StoreGate/ReadCondHandle.h"
#include "MuonCondSvc/TGCTriggerData.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "MuonDigitContainer/TgcDigit.h"

#include "TrigT1TGC/TGCArguments.h"
#include "MuonDigitContainer/TgcDigitContainer.h"

// Tile-Muon
#include "TileEvent/TileMuContainer.h"
#include "TileEvent/TileMuonReceiverObj.h"

//NSW Trigger Output
#include "MuonRDO/NSW_TrigRawDataContainer.h"

class TgcRdo;
class TgcRawData;
class ITGCcablingSvc;


namespace LVL1TGCTrigger {
  
  /**
   *
   *  @short Dump LVL1TGCTrigger::LVL1TGCTrigger for main algorithm
   *  
   *  Input  : TgcDigitContainer (digits)
   *  Output : LVL1MUONIF::Lvl1MuSectorLogicData (trigger decision)
   *
   *  @see TgcDigitContainer
   *  @see LVL1MUONIF::Lvl1MuSectorLogicData
   * 
   *  @author Chihiro Omachi <omati@phys.sci.kobe-u.ac.jp>
   *  @author Masaya Ishino <Masaya.Ishino@cern.ch>
   *  @author Naoko Kanaya <Naoko.Kanaya@cern.ch>
   *  @author Hisaya Kurashige <Hisaya.Kurashige@cern.ch>
   *
   * 
   */
  class TGCSector;
  class TGCSLSelectorOut;//for Run2
  class TGCTrackSelectorOut;//for Run3
  class TGCElectronicsSystem;
  class TGCTimingManager;
  class TGCDatabaseManager;
  class TGCEvent;
  
  class LVL1TGCTrigger : public AthAlgorithm
  {
  public:
    
    /** standard constructor and destructor for algorithms
     */
    LVL1TGCTrigger( const std::string& name, ISvcLocator* pSvcLocator ) ;
    ~LVL1TGCTrigger();
    
    // standard algorithm methods:
    StatusCode initialize() ;
    StatusCode execute() ;
    StatusCode finalize() ;
    
  private:
    StatusCode processOneBunch(const TgcDigitContainer*,
			       LVL1MUONIF::Lvl1MuCTPIInput*,
			       LVL1MUONIF::Lvl1MuCTPIInputPhase1*);
    void doMaskOperation(const TgcDigitContainer* ,std::map<Identifier, int>& );
    void fillTGCEvent(std::map<Identifier, int>& ,  TGCEvent&);
    
    // Fill TMDB event data
    StatusCode fillTMDB();

    // Fill NSW event data
    StatusCode fillNSW();

    
    // record bare-RDO for LowPT coincidences (on m_OutputTgcRDO=True):
    void recordRdoSLB(TGCSector *);
    
    // record bare-RDO for HighPT coincidences (on OutputTgcRDO=True):
    void recordRdoHPT(TGCSector *);
    
    // record bare-RDO for Inner coincidences (on OutputTgcRDO=True):
    void recordRdoInner(TGCSector *);
    
    // record bare-RDO for R-phi coincidences (on m_OutputTgcRDO=True):
    void recordRdoSL(TGCSector *, unsigned int );
    
    std::map<std::pair<int, int>, TgcRdo*>  m_tgcrdo;
    
    // Retrieve Masked channel list
    StatusCode getMaskedChannel();
    
    // pointers to various external services
    const ITGCcablingSvc*       m_cabling ;
    
    // useful functions
    int getCharge(int dR, int Zdir);
    void extractFromString(std::string, std::vector<int>&);
    bool addRawData(TgcRawData *);
    int getLPTTypeInRawData(int type);
    void FillSectorLogicData(LVL1MUONIF::Lvl1MuSectorLogicData* sldata,
			     const TGCSLSelectorOut *selectorOut,
			     unsigned int subsystem);
    void FillSectorLogicData(LVL1MUONIF::Lvl1MuSectorLogicDataPhase1* sldata,
			     const TGCTrackSelectorOut *trackSelectorOut);
    
    
    // Properties

    // Location of LVL1MUONIF::Lvl1MuSectorLogicData (output from SL)
    StringProperty m_keyMuCTPIInput_TGC{this,"MuCTPIInput_TGC","L1MuctpiStoreTGC"};

    // Version of Coincidence Window
    StringProperty m_VerCW{this,"VersionCW","00_07_0022"};// TILE_EIFI_BW
    
    StringProperty    m_MaskFileName12{this,"MaskFileName12",""};   //!< property, see @link LVL1TGCTrigger::LVL1TGCTrigger @endlink
    ShortProperty     m_CurrentBunchTag{this,"CurrentBunchTag",TgcDigit::BC_CURRENT};  //!< property, see @link LVL1TGCTrigger::LVL1TGCTrigger @endlink
    BooleanProperty   m_ProcessAllBunches{this,"ProcessAllBunhes",true};
    BooleanProperty   m_OutputTgcRDO{this,"OutputTgcRDO",true};   //!< property, see @link LVL1TGCTrigger::LVL1TGCTrigger @endlink
    
    // expert usage
    BooleanProperty m_USE_CONDDB{this, "USE_CONDDB", true};  //< flag to use the Condition DB
    BooleanProperty m_SHPTORED  {this, "SHPTORED",   true};  //< flag for E1/E2 chamber ORED in Strip HPT
    BooleanProperty m_USEINNER  {this, "USEINNER",   true};  //< flag for using Inner Station for SL
    BooleanProperty   m_INNERVETO{this,"INNERVETO",true}; // flag for using VETO by Inner Station for SL
    BooleanProperty   m_FULLCW{this,"FULLCW",false};   // flag for using differne CW for each octant
    BooleanProperty   m_TILEMU{this,"TILEMU",false};   // flag for using TileMu
    BooleanProperty   m_USENSW{this,"USENSW",false};     // flag for using NSW
    BooleanProperty   m_useRun3Config{this,"useRun3Config",false}; // flag for using switch between Run3 and Run2 algorithms
    
    bool              m_firstTime{true};
    uint16_t          m_bctagInProcess;

    TGCDatabaseManager *m_db;
    TGCTimingManager *m_TimingManager;
    TGCElectronicsSystem *m_system;
    
    int m_nEventInSector;
    
    // EIFI-SL connection
    TGCInnerTrackletSlotHolder m_innerTrackletSlotHolder;
    
    // getCabling method
    StatusCode getCabling();
    
    // log
    bool                m_debuglevel;
    
    TGCArguments m_tgcArgs;
    TGCArguments* tgcArgs();

    SG::ReadHandleKey<TgcDigitContainer> m_keyTgcDigit{this,"InputData_perEvent","TGC_DIGITS","Location of TgcDigitContainer"};
    SG::ReadHandleKey<TileMuonReceiverContainer> m_keyTileMu{this,"TileMuRcv_Input","TileMuRcvCnt","Location of TileMuonReceiverContainer"};
    SG::ReadHandleKey<Muon::NSW_TrigRawDataContainer> m_keyNSWTrigOut{this,"NSWTrigger_Input","NSWTRGRDO","Location of NSW_TrigRawDataContainer"};
    SG::ReadCondHandleKey<TGCTriggerData> m_readCondKey{this,"ReadCondKey","TGCTriggerData"};
    SG::WriteHandleKey<LVL1MUONIF::Lvl1MuCTPIInput> m_muctpiKey{this, "MuctpiLocationTGC", "L1MuctpiStoreTGC", "Location of muctpi for Tgc"};
    SG::WriteHandleKey<LVL1MUONIF::Lvl1MuCTPIInputPhase1> m_muctpiPhase1Key{this, "MuctpiPhase1LocationTGC", "L1MuctpiStoreTGC", "Location of muctpiPhase1 for Tgc"};

    /// mask channel map
    std::map<Identifier, int> m_MaskedChannel;

  }; // class LVL1TGCTrigger

  inline TGCArguments* LVL1TGCTrigger::tgcArgs() {
    return &m_tgcArgs;
  }
  
} // namespace LVL1TGCTrigger

#endif // not TRIGT1TGC_LVL1TGCTRIGGER_H
