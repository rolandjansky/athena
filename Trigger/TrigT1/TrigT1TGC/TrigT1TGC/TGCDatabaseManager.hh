/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCDatabaseManager_hh
#define TGCDatabaseManager_hh

#include "TrigT1TGC/TGCArguments.hh"
#include "TrigT1TGC/TGCNumbering.hh"
#include "TrigT1TGC/TGCElectronicsSystem.hh"
#include "TrigT1TGC/TGCConnectionPPToSB.hh"

#include "MuonCondSvc/TGCTriggerData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaKernel/MsgStreamMember.h"

#include <vector>
#include <map>
#include <string>

#include "GaudiKernel/MsgStream.h"

namespace LVL1TGCTrigger {

class TGCConnectionASDToPP;
class TGCConnectionInPP;
class TGCPatchPanel;
class TGCConnectionPPToSL;
class TGCRPhiCoincidenceMap;
class TGCEIFICoincidenceMap;
class TGCTileMuCoincidenceMap;

class TGCDatabaseManager
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

  TGCConnectionPPToSL* getConnectionPPToSL(TGCRegionType type) const;
  TGCConnectionASDToPP* getConnectionASDToPP(TGCRegionType region, int type, TGCForwardBackwardType forwardBackward) const;
  TGCRPhiCoincidenceMap* getRPhiCoincidenceMap(int sideId, int octantId) const;
  TGCEIFICoincidenceMap* getEIFICoincidenceMap(int sideId) const;
  TGCTileMuCoincidenceMap* getTileMuCoincidenceMap() const;

  TGCConnectionInPP* getConnectionInPP(TGCPatchPanel* patchPanel) const;
  void addConnectionInPP(const TGCPatchPanel* patchPanel, const TGCConnectionInPP* connectionInPP);

  void deleteConnectionPPToSL();

  static const std::string& getFilename(int type); 
  static const std::vector<std::string > splitCW(const std::string& input, char delimiter); 

  TGCArguments* tgcArgs() const;

  /** Declaring the standard message stream
   *  Returns a reference to the default message stream
   */
  MsgStream& msg(const MSG::Level lvl) const;
  /** Test method to evaluate the verbosity level */
  bool msgLvl(const MSG::Level lvl) const;
  /** set message level to propagate from the parent class */
  void setMessageLevel(const MSG::Level lvl) const;

 private:
  TGCRPhiCoincidenceMap* m_mapRphi[NumberOfSide][NumberOfOctant];
  TGCEIFICoincidenceMap* m_mapEIFI[NumberOfSide];
  TGCTileMuCoincidenceMap* m_mapTileMu;
  TGCConnectionPPToSL* m_PPToSL[NumberOfRegionType];
  TGCConnectionASDToPP* m_ASDToPP[NumberOfRegionType][NumberOfPatchPanelType][TotalNumForwardBackwardType];

  std::map<PatchPanelIDs, std::pair<const TGCConnectionInPP, PatchPanelPointers> > m_patchPanelToConnectionInPP;
  
  TGCArguments* m_tgcArgs;

  //Declaring private message stream member.
  mutable Athena::MsgStreamMember m_msg;
};

inline
TGCArguments* TGCDatabaseManager::tgcArgs() const {
  return m_tgcArgs;
}

inline 
TGCRPhiCoincidenceMap* TGCDatabaseManager::getRPhiCoincidenceMap(int sideId, int octantId) const
{
  return m_mapRphi[sideId][octantId];
}

inline 
TGCEIFICoincidenceMap* TGCDatabaseManager::getEIFICoincidenceMap(int sideId) const
{
  return m_mapEIFI[sideId];
}

inline 
TGCTileMuCoincidenceMap* TGCDatabaseManager::getTileMuCoincidenceMap() const
{
  return m_mapTileMu;
}

inline 
 TGCConnectionPPToSL* TGCDatabaseManager::getConnectionPPToSL(TGCRegionType type) const
{
  return m_PPToSL[type-1];
}

inline 
 TGCConnectionASDToPP* TGCDatabaseManager::getConnectionASDToPP(TGCRegionType region, int type, TGCForwardBackwardType forwardBackward) const
{
  return m_ASDToPP[region-1][type][forwardBackward];
}

inline MsgStream& TGCDatabaseManager::msg(const MSG::Level lvl) const { return m_msg << lvl; }

inline bool TGCDatabaseManager::msgLvl(const MSG::Level lvl) const
{
  return (m_msg.get().level() <= lvl) ? true : false;
}

inline void TGCDatabaseManager::setMessageLevel(const MSG::Level lvl) const { m_msg.get().setLevel(lvl); }


} //end of namespace bracket

#endif // TGCDatabaseManager_hh
