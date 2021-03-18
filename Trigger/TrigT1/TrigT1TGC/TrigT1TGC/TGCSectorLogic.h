/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGGSectorLogic_hh
#define TGGSectorLogic_hh

#include "TrigT1TGC/TGCArguments.h"
#include "TrigT1TGC/TGCNumbering.h"
#include "TrigT1TGC/TGCEvent.h"
#include "TrigT1TGC/TGCReadoutIndex.h"
#include "TrigT1TGC/TGCRPhiCoincidenceMatrix.h"
#include "TrigT1TGC/TGCRPhiCoincidenceOut.h"
#include "TrigT1TGC/TGCSSCController.h"
#include "TrigT1TGC/TGCSSCControllerOut.h"
#include "TrigT1TGC/TGCSLPreSelectorOut.h"
#include "TrigT1TGC/TGCSLPreSelector.h"
#include "TrigT1TGC/TGCSLSelector.h"
#include "TrigT1TGC/TGCSLSelectorOut.h"
#include "TrigT1TGC/TGCInnerTrackletSlotHolder.h"

//for Run3
#include "TrigT1TGC/TGCTrackSelector.h"

#include "StoreGate/ReadCondHandle.h"
#include "MuonCondSvc/TGCTriggerData.h"

namespace LVL1TGC {
class TGCTileMuCoincidenceLUT;
class Run2TileMuCoincidenceMap;
}

namespace LVL1TGCTrigger {

const int MaxNumberOfWireHighPtBoard = 2;

class TGCDatabaseManager;
class TGCHighPtBoard;
class TGCHighPtChipOut;
class TGCTMDB;
class TGCNSW;
class TGCNSWCoincidenceMap;
class TGCGoodMF;
class TGCEIFICoincidenceMap;

//for Run3
class TGCTrackSelectorOut;


class TGCSectorLogic
{
 public:
  TGCSectorLogic(TGCArguments*, const TGCDatabaseManager* db,
                 TGCRegionType regionIn, int id);
  virtual ~TGCSectorLogic();

  TGCSectorLogic(const TGCSectorLogic& right);

  const TGCSLSelectorOut* getSelectorOutput() const { return m_selectorOut; }
  void getTrackSelectorOutput(std::shared_ptr<TGCTrackSelectorOut> &trackSelectorOut)const ;

  int  getTileMuonWord() const;
  int  getInnerStationWord() const;

  void eraseSelectorOut(); 

  void clockIn(const SG::ReadCondHandleKey<TGCTriggerData> readCondKey,
               int bidIn);

  int getId() const;
  int getModuleID() const;
  int getOctantID() const;
  int getSideID() const;
  TGCRegionType getRegion() const { return m_region;};

  void setWireHighPtBoard(int port, TGCHighPtBoard* highPtBoard);
  void setStripHighPtBoard(TGCHighPtBoard* highPtBoard);
  int  getNumberOfWireHighPtBoard() const;
  void deleteHPBOut();

  TGCSSCController* getSSCController(){return &m_SSCController;};

  void setTMDB(const TGCTMDB* tmdb);
  void setNSW(std::shared_ptr<const TGCNSW> nsw);
  void showResult(TGCSLSelectorOut* out);
 
  int getNumberOfSubSectorCluster() const; 
  int getNumberOfSubSector() const; 

  void dec2bin(int dec, char* binstr, int length);

  int getBid() const {return m_bid;};

  void setInnerTrackletSlots(const TGCInnerTrackletSlot* innerTrackletSlots[]);
  
  TGCArguments* tgcArgs() { return m_tgcArgs; }
  const TGCArguments* tgcArgs() const { return m_tgcArgs; }

protected: 
  enum {MaxNumberOfWireHighPtBoard =2};
  void collectInput();

  void doInnerCoincidence(const SG::ReadCondHandleKey<TGCTriggerData> readCondKey,
                          int SSCId,  TGCRPhiCoincidenceOut* coincidenceOut);

  void doInnerCoincidenceRun3(int SSCId,  TGCRPhiCoincidenceOut* coincidenceOut);

  void doTGCNSWCoincidence(TGCRPhiCoincidenceOut* coincidenceOut); 
  bool doTILECoincidence(TGCRPhiCoincidenceOut* coincidenceOut); 
  bool doTGCEICoincidence(TGCRPhiCoincidenceOut* coincidenceOut);
  bool doTGCFICoincidence(TGCRPhiCoincidenceOut* coincidenceOut);

private:
  TGCSectorLogic& operator=(const TGCSectorLogic& right);

private:
  bool hitTileMu(const uint8_t& mask, const uint8_t& hit6, const uint8_t& hit56) const;

  int m_bid;

  int m_id;
  int m_sectorId, m_moduleId;
  int m_sideId, m_octantId;
  TGCRegionType m_region;
  int  m_NumberOfWireHighPtBoard;

  bool m_useEIFI{false};
  bool m_useTileMu{false};
  bool m_useGoodMF{false};
  bool m_nswSide{false};

  TGCSSCController m_SSCController;

  // Run-3 and Run-2 Coincidence LUTs and Window maps
  TGCRPhiCoincidenceMatrix m_matrix;
  const TGCEIFICoincidenceMap*  m_mapEIFI;
  std::shared_ptr<const LVL1TGC::TGCTileMuCoincidenceLUT> m_tileMuLUT;
  const TGCTMDB*            m_pTMDB;
  std::shared_ptr<const TGCNSW>               m_nsw;
  std::shared_ptr<const TGCNSWCoincidenceMap> m_mapNSW;
  std::shared_ptr<const TGCGoodMF>            m_mapGoodMF;

  // Run-2 Coincidence Maps
  std::shared_ptr<const LVL1TGC::Run2TileMuCoincidenceMap> m_mapRun2TileMu;

  // for Run2
  TGCSLPreSelector m_preSelector; 
  TGCSLSelector m_selector;
  TGCSLSelectorOut* m_selectorOut;
  
  // for Run3
  TGCTrackSelector m_trackSelector;
  std::shared_ptr<TGCTrackSelectorOut> m_trackSelectorOut;
 
  int m_wordTileMuon;
  int m_wordInnerStation;

  TGCHighPtBoard* m_wireHighPtBoard[MaxNumberOfWireHighPtBoard];
  TGCHighPtChipOut* m_wireHighPtChipOut[MaxNumberOfWireHighPtBoard];
  TGCHighPtBoard* m_stripHighPtBoard;
  TGCHighPtChipOut* m_stripHighPtChipOut;

  // for inner trigger
  const TGCInnerTrackletSlot* m_innerTrackletSlots[TGCInnerTrackletSlotHolder::NUMBER_OF_SLOTS_PER_TRIGGER_SECTOR];
  TGCArguments* m_tgcArgs;
};

inline
 int TGCSectorLogic::getNumberOfSubSectorCluster() const
{
  if (m_region==FORWARD) return 8;
  else  return 19;
}

inline
 int TGCSectorLogic::getNumberOfSubSector() const
{
  if (m_region==FORWARD) return 8*8;
  else  return (18*8+4);
}

inline
 int  TGCSectorLogic::getNumberOfWireHighPtBoard() const
{
  return m_NumberOfWireHighPtBoard; 
}


inline 
 int TGCSectorLogic::getId() const
{
  return m_id;
}

inline
 int TGCSectorLogic::getModuleID() const
{
  return m_moduleId;
}


inline 
 int TGCSectorLogic::getOctantID() const
{
  return m_octantId;
}

inline 
 int TGCSectorLogic::getSideID() const
{
  return m_sideId;
}

inline   
 int  TGCSectorLogic::getTileMuonWord() const
{
  return m_wordTileMuon;
}

inline   
 int  TGCSectorLogic::getInnerStationWord() const
{
  return m_wordInnerStation;
}

} //end of namespace bracket

#endif
