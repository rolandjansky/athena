/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCDatabaseManager_hh
#define TGCDatabaseManager_hh

#include <vector>
#include <map>
#include <string>

#include "TrigT1TGC/TGCNumbering.hh"
#include "TrigT1TGC/TGCElectronicsSystem.hh"
#include "TrigT1TGC/TGCConnectionPPToSB.hh"
#include "MuonCondSvc/TGCTriggerData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "GaudiKernel/MsgStream.h"

namespace LVL1TGCTrigger {

class TGCConnectionASDToPP;
class TGCConnectionInPP;
class TGCPatchPanel;
class TGCConnectionPPToSL;
class TGCRPhiCoincidenceMap;
class TGCInnerCoincidenceMap;
class TGCTileMuCoincidenceMap;

class TGCDatabaseManager {
  typedef std::vector<int> PatchPanelIDs; 
  typedef std::vector<const TGCPatchPanel*> PatchPanelPointers; 

public:
  TGCConnectionPPToSL* getConnectionPPToSL(TGCRegionType type) const;
  TGCConnectionASDToPP* getConnectionASDToPP(TGCRegionType region, int type, TGCForwardBackwardType forwardBackward) const;
  TGCRPhiCoincidenceMap* getRPhiCoincidenceMap(int sideId, int octantId) const;
  TGCInnerCoincidenceMap* getInnerCoincidenceMap(int sideId) const;
  TGCTileMuCoincidenceMap* getTileMuCoincidenceMap() const;

  TGCConnectionInPP* getConnectionInPP(TGCPatchPanel* patchPanel) const;
  void addConnectionInPP(const TGCPatchPanel* patchPanel, const TGCConnectionInPP* connectionInPP);

  TGCDatabaseManager();
  TGCDatabaseManager(const SG::ReadCondHandleKey<TGCTriggerData>& readCondKey,
                     const std::string& version, bool flag=false);
  ~TGCDatabaseManager();
  TGCDatabaseManager(const TGCDatabaseManager& right);
  TGCDatabaseManager& operator=(const TGCDatabaseManager& right);

  void deleteConnectionPPToSL();

  static const std::string& getFilename(int type); 
  static const std::vector<std::string > splitCW(const std::string& input, char delimiter); 


private:
  TGCRPhiCoincidenceMap* mapRphi[NumberOfSide][NumberOfOctant];
  TGCInnerCoincidenceMap* mapInner[NumberOfSide];
  TGCTileMuCoincidenceMap* mapTileMu;
  TGCConnectionPPToSL* PPToSL[NumberOfRegionType];
  TGCConnectionASDToPP* ASDToPP[NumberOfRegionType][NumberOfPatchPanelType][TotalNumForwardBackwardType];

  std::map<PatchPanelIDs, std::pair<const TGCConnectionInPP, PatchPanelPointers> > m_patchPanelToConnectionInPP;
};

inline 
TGCRPhiCoincidenceMap* TGCDatabaseManager::getRPhiCoincidenceMap(int sideId, int octantId) const
{
  return mapRphi[sideId][octantId];
}

inline 
TGCInnerCoincidenceMap* TGCDatabaseManager::getInnerCoincidenceMap(int sideId) const
{
  return mapInner[sideId];
}

inline 
TGCTileMuCoincidenceMap* TGCDatabaseManager::getTileMuCoincidenceMap() const
{
  return mapTileMu;
}

inline 
 TGCConnectionPPToSL* TGCDatabaseManager::getConnectionPPToSL(TGCRegionType type) const
{
  return PPToSL[type-1];
}

inline 
 TGCConnectionASDToPP* TGCDatabaseManager::getConnectionASDToPP(TGCRegionType region, int type, TGCForwardBackwardType forwardBackward) const
{
  return ASDToPP[region-1][type][forwardBackward];
}

} //end of namespace bracket

#endif // TGCDatabaseManager_hh
