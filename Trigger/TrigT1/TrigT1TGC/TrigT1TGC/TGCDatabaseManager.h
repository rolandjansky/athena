/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCDatabaseManager_hh
#define TGCDatabaseManager_hh

#include "TrigT1TGC/TGCArguments.h"
#include "TrigT1TGC/TGCNumbering.h"
#include "TrigT1TGC/TGCElectronicsSystem.h"
#include "TrigT1TGC/TGCConnectionPPToSB.h"

#include "MuonCondSvc/TGCTriggerData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaBaseComps/AthMessaging.h"

#include <vector>
#include <map>
#include <string>

namespace LVL1TGCTrigger {

class TGCConnectionASDToPP;
class TGCConnectionInPP;
class TGCPatchPanel;
class TGCConnectionPPToSL;
class TGCRPhiCoincidenceMap;
class TGCEIFICoincidenceMap;
class TGCTileMuCoincidenceMap;
class TGCNSWCoincidenceMap;
class TGCGoodMF;

class TGCDatabaseManager : public AthMessaging
{
  typedef std::vector<int> PatchPanelIDs; 
  typedef std::vector<const TGCPatchPanel*> PatchPanelPointers; 

 public:
  TGCDatabaseManager(TGCArguments*);
  TGCDatabaseManager(TGCArguments*,
                     const SG::ReadCondHandleKey<TGCTriggerData>& readCondKey,
                     const std::string& version, bool flag=false);
  virtual ~TGCDatabaseManager();

  TGCDatabaseManager(const TGCDatabaseManager& right);
  TGCDatabaseManager& operator=(const TGCDatabaseManager& right);

  const TGCConnectionPPToSL* getConnectionPPToSL(TGCRegionType type) const;
  const TGCConnectionASDToPP* getConnectionASDToPP(TGCRegionType region, int type, TGCForwardBackwardType forwardBackward) const;
  const TGCRPhiCoincidenceMap* getRPhiCoincidenceMap(int sideId, int octantId) const;
  const TGCEIFICoincidenceMap* getEIFICoincidenceMap(int sideId) const;
  const TGCTileMuCoincidenceMap* getTileMuCoincidenceMap() const;
  std::shared_ptr<TGCNSWCoincidenceMap> getNSWCoincidenceMap(int sideId, int octantId, int moduleId) const;
  TGCGoodMF* getGoodMFMap() const;

  TGCConnectionInPP* getConnectionInPP(TGCPatchPanel* patchPanel) const;
  void addConnectionInPP(const TGCPatchPanel* patchPanel, const TGCConnectionInPP* connectionInPP);

  void deleteConnectionPPToSL();

  static std::string getFilename(int type);
  static const std::vector<std::string > splitCW(const std::string& input, char delimiter); 

  TGCArguments* tgcArgs() { return m_tgcArgs; }
  const TGCArguments* tgcArgs() const { return m_tgcArgs; }

 private:
  enum {NumberOfModuleInBW=9};

  TGCRPhiCoincidenceMap* m_mapRphi[NumberOfSide][NumberOfOctant];
  TGCEIFICoincidenceMap* m_mapEIFI[NumberOfSide];
  TGCTileMuCoincidenceMap* m_mapTileMu;
  std::array<std::array<std::array<std::shared_ptr<TGCNSWCoincidenceMap>, NumberOfModuleInBW>, NumberOfOctant>, NumberOfSide> m_mapNSW;
  TGCConnectionPPToSL* m_PPToSL[NumberOfRegionType];
  TGCConnectionASDToPP* m_ASDToPP[NumberOfRegionType][NumberOfPatchPanelType][TotalNumForwardBackwardType];
  TGCGoodMF* m_mapGoodMF;

  std::map<PatchPanelIDs, std::pair<const TGCConnectionInPP, PatchPanelPointers> > m_patchPanelToConnectionInPP;
  
  TGCArguments* m_tgcArgs;
};

inline 
const TGCRPhiCoincidenceMap* TGCDatabaseManager::getRPhiCoincidenceMap(int sideId, int octantId) const
{
  return m_mapRphi[sideId][octantId];
}

inline 
const TGCEIFICoincidenceMap* TGCDatabaseManager::getEIFICoincidenceMap(int sideId) const
{
  return m_mapEIFI[sideId];
}

inline 
const TGCTileMuCoincidenceMap* TGCDatabaseManager::getTileMuCoincidenceMap() const
{
  return m_mapTileMu;
}

inline 
std::shared_ptr<TGCNSWCoincidenceMap> TGCDatabaseManager::getNSWCoincidenceMap(int sideId, int octantId, int moduleId) const
{
  return m_mapNSW[sideId][octantId][moduleId];
}

inline
TGCGoodMF* TGCDatabaseManager::getGoodMFMap() const
{
      return m_mapGoodMF;
}

inline 
 const TGCConnectionPPToSL* TGCDatabaseManager::getConnectionPPToSL(TGCRegionType type) const
{
  return m_PPToSL[type-1];
}

inline 
 const TGCConnectionASDToPP* TGCDatabaseManager::getConnectionASDToPP(TGCRegionType region, int type, TGCForwardBackwardType forwardBackward) const
{
  return m_ASDToPP[region-1][type][forwardBackward];
}

} //end of namespace bracket

#endif // TGCDatabaseManager_hh
