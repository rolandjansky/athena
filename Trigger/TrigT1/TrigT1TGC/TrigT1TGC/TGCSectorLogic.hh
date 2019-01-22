/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGGSectorLogic_hh
#define TGGSectorLogic_hh

#include "TrigT1TGC/TGCNumbering.hh"
#include "TrigT1TGC/TGCEvent.h"
#include "TrigT1TGC/TGCReadoutIndex.h"
#include "TrigT1TGC/TGCInnerCoincidenceMap.hh"
#include "TrigT1TGC/TGCTileMuCoincidenceMap.hh"
#include "TrigT1TGC/TGCRPhiCoincidenceMap.hh"
#include "TrigT1TGC/TGCRPhiCoincidenceMatrix.hh"
#include "TrigT1TGC/TGCRPhiCoincidenceOut.hh"
#include "TrigT1TGC/TGCSSCController.hh"
#include "TrigT1TGC/TGCSSCControllerOut.hh"
#include "TrigT1TGC/TGCSLPreSelectorOut.hh"
#include "TrigT1TGC/TGCSLPreSelector.hh"
#include "TrigT1TGC/TGCSLSelector.hh"
#include "TrigT1TGC/TGCSLSelectorOut.hh"
#include "TrigT1TGC/TGCInnerTrackletSlotHolder.hh"

#include "StoreGate/ReadCondHandle.h"
#include "MuonCondSvc/TGCTriggerData.h"

namespace LVL1TGCTrigger {

const int MaxNumberOfWireHighPtBoard = 2;

class  TGCHighPtBoard;
class  TGCHighPtChipOut;
class  TGCTMDB;

class TGCSectorLogic {
public:
  TGCSectorLogic(const TGCSectorLogic& right);

  TGCSLSelectorOut* getSelectorOutput() const;
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

  void setRPhiMap(const TGCRPhiCoincidenceMap* map, 
		  const TGCInnerCoincidenceMap* mapI=0);
  void setInnerMap(const TGCInnerCoincidenceMap* mapI);
  void setTileMuMap(const TGCTMDB* tmdb,
		    const TGCTileMuCoincidenceMap* mapTM);
  void showResult(TGCSLSelectorOut* out);
 
  TGCSectorLogic(TGCRegionType regionIn, int id);
  ~TGCSectorLogic();

  int getNumberOfSubSectorCluster() const; 
  int getNumberOfSubSector() const; 

  void dec2bin(int dec, char* binstr, int length);

  int getBid() const {return m_bid;};

  void setInnerTrackletSlots(const TGCInnerTrackletSlot* innerTrackletSlots[]);
  
protected: 
  enum {MaxNumberOfWireHighPtBoard =2};
  void collectInput();

  void doInnerCoincidence(const SG::ReadCondHandleKey<TGCTriggerData> readCondKey,
                          int SSCId,  TGCRPhiCoincidenceOut* coincidenceOut);

private:
  TGCSectorLogic& operator=(const TGCSectorLogic& right);

private:

  int m_bid;

  int m_id;
  int m_sectorId, m_moduleId;
  int m_sideId, m_octantId;
  TGCRegionType m_region;
  int  m_NumberOfWireHighPtBoard;

  TGCSSCController m_SSCController;
  TGCRPhiCoincidenceMatrix m_matrix;
  const TGCInnerCoincidenceMap*  m_mapInner;
  const TGCTileMuCoincidenceMap*  m_mapTileMu;
  const TGCTMDB*            m_pTMDB;
  TGCSLPreSelector m_preSelector; 
  TGCSLSelector m_selector;
  TGCSLSelectorOut* m_selectorOut; 
  int m_wordTileMuon;
  int m_wordInnerStation;

  TGCHighPtBoard* m_wireHighPtBoard[MaxNumberOfWireHighPtBoard];
  TGCHighPtChipOut* m_wireHighPtChipOut[MaxNumberOfWireHighPtBoard];
  TGCHighPtBoard* m_stripHighPtBoard;
  TGCHighPtChipOut* m_stripHighPtChipOut;

  // for inner trigger
  const TGCInnerTrackletSlot* m_innerTrackletSlots[TGCInnerTrackletSlotHolder::NUMBER_OF_SLOTS_PER_TRIGGER_SECTOR];
  bool m_useInner;
  bool m_useTileMu;
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

#ifdef GENRPHIMAP
  extern std::ofstream rphimap;
#endif

} //end of namespace bracket

#endif
