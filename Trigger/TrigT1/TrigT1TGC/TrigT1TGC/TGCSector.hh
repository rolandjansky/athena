/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

  int getSideId() const { return sideId; }
  int getOctantId() const { return octantId; }
  int getModuleId() const { return moduleId; }

private:
  const TGCTMDB* getTMDB() const { return TMDB; }
  
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
  int id;
  TGCRegionType regionType;
  int numberOfHit;
  int sideId;
  int octantId;
  int moduleId;

  TGCForwardBackwardType forwardBackward;
  TGCConnectionASDToPP* ASDToPP[NumberOfPatchPanelType];

  int numberOfPP[NumberOfPatchPanelType];
  TGCPatchPanel**  PP[NumberOfPatchPanelType];

  int numberOfSB[NumberOfSlaveBoardType];
  TGCSlaveBoard**  SB[NumberOfSlaveBoardType];

  int numberOfHPB[NumberOfHighPtBoardType];
  TGCHighPtBoard**  HPB[NumberOfHighPtBoardType];

  TGCSectorLogic* SL;
  const TGCTMDB* TMDB;
};

inline 
 bool TGCSector::hasHit() const
{
  return (numberOfHit>0);
}
inline 
 void TGCSector::clearNumberOfHit()
{ numberOfHit = 0;}
inline
 TGCPatchPanel* TGCSector::getPP(int type, int index) const
{ 
  if ((type<0) || (index<0)) return 0;
  return PP[type][index];
}
inline
 TGCSlaveBoard* TGCSector::getSB(int type, int index) const
{   
  if ((type<0) || (index<0)) return 0;
  return SB[type][index];
}
inline
 TGCHighPtBoard* TGCSector::getHPB(int type, int index) const
{   
  if ((type<0) || (index<0)) return 0;
  return HPB[type][index];
}
inline
 TGCSectorLogic* TGCSector::getSL() const
{ return SL;}
inline
 int TGCSector::getNumberOfPP(int type) const 
{ 
  if (type<0) return -1;
  return numberOfPP[type];
}
inline
 int TGCSector::getNumberOfSB(int type) const 
{ if (type<0) return -1;
  return numberOfSB[type];
}
inline
 int TGCSector::getNumberOfHPB(int type) const 
{ if (type<0) return -1;
  return numberOfHPB[type];
}
inline
 TGCRegionType TGCSector::getRegionType()const
{ return regionType;}
inline
 int TGCSector::getId() const
{ return id;}

} //end of namespace bracket

#endif
