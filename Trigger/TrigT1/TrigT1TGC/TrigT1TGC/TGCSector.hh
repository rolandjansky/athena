/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCSector_h
#define TGCSector_h

#include "TrigT1TGC/TGCDatabaseManager.hh"
#include "TrigT1TGC/TGCTimingManager.hh"
#include "TrigT1TGC/TGCReadoutIndex.h"
#include "TrigT1TGC/TGCConnectionInPP.hh"
#include "TrigT1TGC/TGCConnectionPPToSL.hh"
#include "TrigT1TGC/TGCConnectionASDToPP.hh"
#include "TrigT1TGC/TGCRPhiCoincidenceMap.hh"
#include "TrigT1TGC/TGCInnerCoincidenceMap.hh"


#include "TrigT1TGC/TGCASDOut.h"
#include "TrigT1TGC/TGCPatchPanel.hh"

#include "TrigT1TGC/TGCWireDoubletSB.hh"
#include "TrigT1TGC/TGCStripDoubletSB.hh"
#include "TrigT1TGC/TGCWireTripletSB.hh"
#include "TrigT1TGC/TGCStripTripletSB.hh"
#include "TrigT1TGC/TGCInnerSB.hh"


#include "TrigT1TGC/TGCWireHighPtBoard.hh"
#include "TrigT1TGC/TGCStripHighPtBoard.hh"

#include "TrigT1TGC/TGCSectorLogic.hh"
#include "TrigT1TGC/TGCTMDB.h"


namespace LVL1TGCTrigger {

class TGCSector {

public:
  TGCSector(int idIn, 
	    TGCRegionType type, 
	    TGCForwardBackwardType forwardBackward, 
	    const TGCDatabaseManager* db,
	    const TGCTMDB*            tmdb);

  TGCSector();

private:
  // copy constructor and assignement operator are hidden
  TGCSector( const TGCSector& right );
  TGCSector& operator=( const TGCSector& right );

public:
  virtual ~TGCSector();

  bool hasHit() const;
  void clearNumberOfHit();
  int distributeSignal(const TGCASDOut* asdOut);

  TGCPatchPanel* getPP(int type, int index) const;
  TGCSlaveBoard* getSB(int type, int index) const;
  TGCHighPtBoard* getHPB(int type, int index) const;
  TGCSectorLogic* getSL() const;

  int getNumberOfPP(int type) const;
  int getNumberOfSB(int type) const;
  int getNumberOfHPB(int type) const;

  TGCRegionType getRegionType() const;
  int getId() const;
  void dumpModule();

  int getSideId() const { return m_sideId; }
  int getOctantId() const { return m_octantId; }
  int getModuleId() const { return m_moduleId; }

private:
  const TGCTMDB* getTMDB() const { return m_TMDB; }
  
  int getPatchPanelType(TGCSignalType signal, int layer) const;


  void setModule(const TGCConnectionPPToSL* connection);
  void setRPhiMap(const TGCRPhiCoincidenceMap* map, 
		  const TGCInnerCoincidenceMap* mapI=0);
  void setTileMuMap(const TGCTileMuCoincidenceMap* map);
  void connectPPToSB(const TGCConnectionPPToSB* connection);
  void connectSBToHPB(const TGCConnectionSBToHPB* connection);
  void connectHPBToSL(const TGCConnectionHPBToSL* connection);
  void connectAdjacentPP();
  void connectAdjacentHPB();

  friend void TGCTimingManager::startPatchPanel(TGCSector* sector, TGCDatabaseManager* db);
  friend void TGCTimingManager::startSlaveBoard(TGCSector* sector);
  friend void TGCTimingManager::startHighPtBoard(TGCSector* sector);
  friend void TGCTimingManager::startSectorLogic(TGCSector* sector);

private:
  int m_id;
  TGCRegionType m_regionType;
  int m_numberOfHit;
  int m_sideId;
  int m_octantId;
  int m_moduleId;

  TGCForwardBackwardType m_forwardBackward;
  TGCConnectionASDToPP* m_ASDToPP[NumberOfPatchPanelType];

  int m_numberOfPP[NumberOfPatchPanelType];
  TGCPatchPanel**  m_PP[NumberOfPatchPanelType];

  int m_numberOfSB[NumberOfSlaveBoardType];
  TGCSlaveBoard**  m_SB[NumberOfSlaveBoardType];

  int m_numberOfHPB[NumberOfHighPtBoardType];
  TGCHighPtBoard**  m_HPB[NumberOfHighPtBoardType];

  TGCSectorLogic* m_SL;
  const TGCTMDB* m_TMDB;
};

inline 
 bool TGCSector::hasHit() const
{
  return (m_numberOfHit>0);
}
inline 
 void TGCSector::clearNumberOfHit()
{ m_numberOfHit = 0;}
inline
 TGCPatchPanel* TGCSector::getPP(int type, int index) const
{ 
  if ((type<0) || (index<0)) return 0;
  return m_PP[type][index];
}
inline
 TGCSlaveBoard* TGCSector::getSB(int type, int index) const
{   
  if ((type<0) || (index<0)) return 0;
  return m_SB[type][index];
}
inline
 TGCHighPtBoard* TGCSector::getHPB(int type, int index) const
{   
  if ((type<0) || (index<0)) return 0;
  return m_HPB[type][index];
}
inline
 TGCSectorLogic* TGCSector::getSL() const
{ return m_SL;}
inline
 int TGCSector::getNumberOfPP(int type) const 
{ 
  if (type<0) return -1;
  return m_numberOfPP[type];
}
inline
 int TGCSector::getNumberOfSB(int type) const 
{ if (type<0) return -1;
  return m_numberOfSB[type];
}
inline
 int TGCSector::getNumberOfHPB(int type) const 
{ if (type<0) return -1;
  return m_numberOfHPB[type];
}
inline
 TGCRegionType TGCSector::getRegionType()const
{ return m_regionType;}
inline
 int TGCSector::getId() const
{ return m_id;}

} //end of namespace bracket

#endif
