/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCElectronicsSystem_hh
#define TGCElectronicsSystem_hh

//#include "TrigT1TGC/TGCNumbering.hh"
#include "TrigT1TGC/TGCReadoutIndex.h"

namespace LVL1TGCTrigger {

const int NumberOfSide = 2;
const int NumberOfOctant = 8;
const int NumberOfModule = 15; 
const int NumberOfSignalTypes = 2; // 1=WireGroup, 2=Strip
const int NumberOfLayers = 9;      // 0,1,2: Tpl, 3,4: Dbl, 5,6: Inner 8,9
const int NumberOfRegions = 2;     // 1=Forward, 2=ENdcap 

class TGCEvent;
//class TGCASDOut;
class TGCDatabaseManager;
class TGCSector;
class TGCTMDB;

class TGCElectronicsSystem {
public:
  void distributeSignal(TGCEvent* event);
  int getNumberOfSide() const { return NumberOfSide;};
  int getNumberOfSector() const { return NumberOfOctant*NumberOfModule;};
  int getNumberOfOctant() const { return NumberOfOctant;};
  int getNumberOfModule() const { return NumberOfModule;};

  TGCRegionType getRegionType(int mod) const;
  TGCForwardBackwardType getForwardBackward(int side, int oct, int mod) const;
  int getSectorId(int side, int oct, int mod) const;
  TGCSector* getSector(TGCReadoutIndex index) const;
  TGCSector* getSector(int side, int oct, int mod) const { 
    if ( (side<0) || (oct<0) || (mod<0) ) return 0;
    return m_sector[side][oct][mod];
  };
  TGCTMDB* getTMDB() const {return m_tmdb;}

  TGCElectronicsSystem(TGCDatabaseManager* database, bool isAtlas=true);
  ~TGCElectronicsSystem();

private:
  // hide default/copy constructor and assignment operator
  TGCElectronicsSystem();
  TGCElectronicsSystem(const TGCElectronicsSystem& right);
  TGCElectronicsSystem& operator=(const TGCElectronicsSystem& right);

private:
  TGCDatabaseManager* m_DB;
  TGCSector* m_sector[NumberOfSide][NumberOfOctant][NumberOfModule];
  TGCTMDB*   m_tmdb;
};


} //end of namespace bracket

#endif // TGCElectronicsSystem_hh
