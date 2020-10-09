/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCSector_h
#define TGCSector_h

#include "TrigT1TGC/TGCArguments.h"
#include "TrigT1TGC/TGCDatabaseManager.h"
#include "TrigT1TGC/TGCTimingManager.h"
#include "TrigT1TGC/TGCReadoutIndex.h"
#include "TrigT1TGC/TGCConnectionInPP.h"
#include "TrigT1TGC/TGCConnectionPPToSL.h"
#include "TrigT1TGC/TGCConnectionASDToPP.h"
#include "TrigT1TGC/TGCRPhiCoincidenceMap.h"


#include "TrigT1TGC/TGCASDOut.h"
#include "TrigT1TGC/TGCPatchPanel.h"

#include "TrigT1TGC/TGCWireDoubletSB.h"
#include "TrigT1TGC/TGCStripDoubletSB.h"
#include "TrigT1TGC/TGCWireTripletSB.h"
#include "TrigT1TGC/TGCStripTripletSB.h"
#include "TrigT1TGC/TGCInnerSB.h"


#include "TrigT1TGC/TGCWireHighPtBoard.h"
#include "TrigT1TGC/TGCStripHighPtBoard.h"

#include "TrigT1TGC/TGCSectorLogic.h"
#include "TrigT1TGC/TGCTMDB.h"
#include "TrigT1TGC/TGCNSW.h"
#include "TrigT1TGC/TGCGoodMF.h"

namespace LVL1TGCTrigger {

class TGCEIFICoincidenceMap;

class TGCSector {

public:
  TGCSector(TGCArguments*,
	    int idIn, 
	    TGCRegionType type, 
	    TGCForwardBackwardType forwardBackward, 
	    const TGCDatabaseManager* db,
	    const TGCTMDB*            tmdb,
	    std::shared_ptr<const TGCNSW>             nsw
	    );

  TGCSector(TGCArguments*);

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
  TGCSectorLogic* getSL() { return m_SL; }

  int getNumberOfPP(int type) const;
  int getNumberOfSB(int type) const;
  int getNumberOfHPB(int type) const;

  TGCRegionType getRegionType() const;
  int getId() const;
  void dumpModule();

  int getSideId() const { return m_sideId; }
  int getOctantId() const { return m_octantId; }
  int getModuleId() const { return m_moduleId; }

  TGCArguments* tgcArgs() { return m_tgcArgs; }
  const TGCArguments* tgcArgs() const { return m_tgcArgs; }

private:
  const TGCTMDB* getTMDB() const { return m_TMDB; }
  std::shared_ptr<const TGCNSW>   getNSW() const{ return m_NSW; }
  
  int getPatchPanelType(TGCSignalType signal, int layer) const;


  void setModule(const TGCConnectionPPToSL* connection);
  void setRPhiMap(const TGCRPhiCoincidenceMap* map, 
		  const TGCEIFICoincidenceMap* mapI=0);
  void setTileMuMap(const TGCTileMuCoincidenceMap* map);
  void setNSWMap(std::shared_ptr<const TGCNSWCoincidenceMap> mapNSW);
  void setGoodMFMap(const TGCGoodMF* mapGoodMF);
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
  const TGCConnectionASDToPP* m_ASDToPP[NumberOfPatchPanelType];

  int m_numberOfPP[NumberOfPatchPanelType];
  TGCPatchPanel**  m_PP[NumberOfPatchPanelType];

  int m_numberOfSB[NumberOfSlaveBoardType];
  TGCSlaveBoard**  m_SB[NumberOfSlaveBoardType];

  int m_numberOfHPB[NumberOfHighPtBoardType];
  TGCHighPtBoard**  m_HPB[NumberOfHighPtBoardType];

  TGCSectorLogic* m_SL;
  const TGCTMDB* m_TMDB;
  std::shared_ptr<const TGCNSW>  m_NSW;

  TGCArguments* m_tgcArgs;

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
