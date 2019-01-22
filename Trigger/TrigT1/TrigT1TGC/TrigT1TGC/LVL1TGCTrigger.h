/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// LVL1TGCTrigger.h
#ifndef TRIGT1TGC_LVL1TGCTRIGGER_H
#define TRIGT1TGC_LVL1TGCTRIGGER_H

#include "AthenaBaseComps/AthAlgorithm.h"

// STL
#include <string>
#include <vector>
#include <map>

// Gaudi includes
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"

// Other stuff
#include "TrigT1Interfaces/SlinkWord.h"
#include "TrigT1Interfaces/Lvl1MuCTPIInput.h"
#include "Identifier/Identifier.h"

// EIFI-SL connection
#include "TrigT1TGC/TGCInnerTrackletSlotHolder.hh"

#include "StoreGate/ReadCondHandle.h"
#include "MuonCondSvc/TGCTriggerData.h"

class TgcRdo;
class TgcRawData;
class ITGCcablingSvc;
class TgcDigitContainer; 

namespace TrigConf {
  class ILVL1ConfigSvc;
}

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
class TGCSLSelectorOut;
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
  // Process one bunch 
  StatusCode processOneBunch(const DataHandle<TgcDigitContainer>&  , 
                             LVL1MUONIF::Lvl1MuCTPIInput* ); 
  void doMaskOperation(const DataHandle<TgcDigitContainer>& ,std::map<Identifier, int>& );
  void fillTGCEvent(std::map<Identifier, int>& ,  TGCEvent&); 

  // Fill TMDB event data
  StatusCode fillTMDB();

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
  std::map<Identifier, int> m_MaskedChannel;   

  // pointers to various external services
  ServiceHandle<StoreGateSvc> m_sgSvc;  
  StoreGateSvc *m_detectorStore;
  const ITGCcablingSvc*       m_cabling ;

  // useful functions
  int getCharge(int dR, int Zdir);  
  void extractFromString(std::string, std::vector<int>&);    
  bool addRawData(TgcRawData *);
  int getLPTTypeInRawData(int type);
  void FillSectorLogicData(LVL1MUONIF::Lvl1MuSectorLogicData* sldata,
			   const TGCSLSelectorOut *selectorOut, 
			   unsigned int subsystem);
 

 
  // Properties
 
  // Location of LVL1MUONIF::Lvl1MuSectorLogicData (output from SL)
  StringProperty m_keyMuCTPIInput_TGC;

  // Version of Coincidence Window
  StringProperty m_VerCW;

  // Location of TgcDigitContainer
  StringProperty m_keyTgcDigit ;

  // Location of TileMuonReceiverContainer
  StringProperty m_keyTileMu ;
  
  StringProperty    m_MaskFileName ;   //!< property, see @link LVL1TGCTrigger::LVL1TGCTrigger @endlink 
  StringProperty    m_MaskFileName12 ;   //!< property, see @link LVL1TGCTrigger::LVL1TGCTrigger @endlink 
  ShortProperty     m_CurrentBunchTag;  //!< property, see @link LVL1TGCTrigger::LVL1TGCTrigger @endlink 
  BooleanProperty   m_ProcessAllBunches; 
  BooleanProperty   m_OutputTgcRDO;   //!< property, see @link LVL1TGCTrigger::LVL1TGCTrigger @endlink  

  // expert usage
  BooleanProperty   m_STRICTWD;
  BooleanProperty   m_STRICTWT;
  BooleanProperty   m_STRICTSD;
  BooleanProperty   m_STRICTST;
  BooleanProperty   m_OUTCOINCIDENCE; //!< property, see @link LVL1TGCTrigger::LVL1TGCTrigger @endlink
  BooleanProperty   m_SINGLEBEAM; // for the single beam run
  BooleanProperty   m_MUHALO; // flag for including MUHALO (i.e. 2-st coin ) in pt=1
  BooleanProperty   m_SHPTORED; // flag for E1/E2 chamber ORED in Strip HPT
  BooleanProperty   m_USEINNER; // flag for using Inner Station for SL
  BooleanProperty   m_INNERVETO; // flag for using VETO by Inner Station for SL
  BooleanProperty   m_FULLCW;   // flag for using differne CW for each octant
  BooleanProperty   m_TILEMU;   // flag for using TileMu

  uint16_t          m_bctagInProcess;

  TGCDatabaseManager *m_db;
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
  TGCTimingManager *m_TimingManager;
  TGCElectronicsSystem *m_system;
  
  int m_nEventInSector;

  // EIFI-SL connection
  TGCInnerTrackletSlotHolder m_innerTrackletSlotHolder;
  
  // getCabling method 
  StatusCode getCabling();

  // log 
  mutable MsgStream   m_log;
  bool                m_debuglevel;

  SG::ReadCondHandleKey<TGCTriggerData> m_readCondKey;
}; // class LVL1TGCTrigger


} // namespace LVL1TGCTrigger

#endif // not TRIGT1TGC_LVL1TGCTRIGGER_H
