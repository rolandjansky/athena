/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCDatabaseManager.h"
#include "TrigT1TGC/TGCConnectionPPToSL.h"
#include "TrigT1TGC/TGCRPhiCoincidenceMap.h"
#include "TrigT1TGC/TGCEIFICoincidenceMap.h"
#include "TrigT1TGC/TGCTileMuCoincidenceMap.h"
#include "TrigT1TGC/TGCNSWCoincidenceMap.h"
#include "TrigT1TGC/TGCGoodMF.h"
#include "TrigT1TGC/TGCConnectionASDToPP.h"
#include "TrigT1TGC/TGCConnectionInPP.h"
#include "TrigT1TGC/TGCPatchPanel.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

namespace LVL1TGCTrigger {

TGCConnectionInPP* TGCDatabaseManager::getConnectionInPP(TGCPatchPanel* patchPanel) const 
{
  if(!patchPanel) return 0;

  PatchPanelIDs patchPanelIDs(5, -1);
  patchPanelIDs.at(0) = patchPanel->getRegion(); 
  patchPanelIDs.at(1) = patchPanel->getType();
  patchPanelIDs.at(2) = patchPanel->getId();
  patchPanelIDs.at(3) = (patchPanel->getAdjacentPP(0) ? patchPanel->getAdjacentPP(0)->getId() : -1); 
  patchPanelIDs.at(4) = (patchPanel->getAdjacentPP(1) ? patchPanel->getAdjacentPP(1)->getId() : -1);
  
  std::map<PatchPanelIDs, std::pair<const TGCConnectionInPP, PatchPanelPointers> >::const_iterator cit 
    = m_patchPanelToConnectionInPP.find(patchPanelIDs);
  // If this PatchPanel is already registered, create a clone connectionInPP
  if(cit!=m_patchPanelToConnectionInPP.end()) { 
    TGCConnectionInPP* newConnectionInPP = new TGCConnectionInPP(((*cit).second).first);
    PatchPanelPointers oldPatchPanelPointers = ((*cit).second).second; 
    // Need to replace PatchPanel pointers of newConnectionInPP by ones of patchPanel
    bool isReplaced = newConnectionInPP->replacePatchPanelPointers(patchPanel, oldPatchPanelPointers);
    if(!isReplaced) {
      delete newConnectionInPP;
      newConnectionInPP = 0;
    }
    return newConnectionInPP;
  }
  
  return 0;
}

void TGCDatabaseManager::addConnectionInPP(const TGCPatchPanel* patchPanel, 
					   const TGCConnectionInPP* connectionInPP) 
{
  if(!patchPanel || !connectionInPP) return; 

  PatchPanelIDs patchPanelIDs(5, -1);
  patchPanelIDs.at(0) = patchPanel->getRegion(); 
  patchPanelIDs.at(1) = patchPanel->getType();
  patchPanelIDs.at(2) = patchPanel->getId();
  patchPanelIDs.at(3) = (patchPanel->getAdjacentPP(0) ? patchPanel->getAdjacentPP(0)->getId() : -1); 
  patchPanelIDs.at(4) = (patchPanel->getAdjacentPP(1) ? patchPanel->getAdjacentPP(1)->getId() : -1);

  PatchPanelPointers patchPanelPointers;
  patchPanelPointers.push_back(patchPanel);
  patchPanelPointers.push_back(patchPanel->getAdjacentPP(0));
  patchPanelPointers.push_back(patchPanel->getAdjacentPP(1));

  TGCConnectionInPP newConnectionInPP(*connectionInPP);
  std::pair<const TGCConnectionInPP, PatchPanelPointers> cInPP_PPPs(newConnectionInPP, patchPanelPointers); 

  m_patchPanelToConnectionInPP.insert(std::pair<PatchPanelIDs, std::pair<const TGCConnectionInPP, PatchPanelPointers> >
				      (patchPanelIDs, cInPP_PPPs));
}

TGCDatabaseManager::TGCDatabaseManager(TGCArguments* tgcargs)
 : AthMessaging(Athena::getMessageSvc(), "LVL1TGC::TGCDatabaseManager"),
   m_mapTileMu(0),
   m_tgcArgs(tgcargs)
{
  setLevel(tgcArgs()->MSGLEVEL());

  int i,j,k;
  for( j=0; j<NumberOfRegionType; j+=1){
    for( i=0; i<NumberOfPatchPanelType; i+=1){
       for( k=0; k<TotalNumForwardBackwardType; k+=1){
         m_ASDToPP[j][i][k] = 0;
       }
    }
  }
  for( i=0; i<NumberOfRegionType; i+=1){
    m_PPToSL[i] = 0;
  }
  for (int side=0; side<NumberOfSide; side +=1) {
    m_mapEIFI[side] = 0;
    for (int oct=0; oct<NumberOfOctant; oct++) {
      m_mapRphi[side][oct] = 0;
    }
  }

}

TGCDatabaseManager::TGCDatabaseManager(TGCArguments* tgcargs,
				       const SG::ReadCondHandleKey<TGCTriggerData>& readCondKey,
				       const std::string& ver, bool )
 : AthMessaging(Athena::getMessageSvc(), "LVL1TGC::TGCDatabaseManager"),
   m_tgcArgs(tgcargs)
{
  setLevel(tgcArgs()->MSGLEVEL());

  bool status = true;

  ATH_MSG_DEBUG("Read database for connection from ASD to PP.");
  int i,j,k;
  for( j=0; j<NumberOfRegionType; j+=1) {
    for( i=0; i<NumberOfPatchPanelType; i+=1){
       for( k=0; k<TotalNumForwardBackwardType; k+=1){
	 m_ASDToPP[j][i][k] = new TGCConnectionASDToPP;
	 status = status && 
	   m_ASDToPP[j][i][k]->readData((TGCRegionType)(j+1),i,(TGCForwardBackwardType)k);
       }
    }
  }
  for( i=0; i<NumberOfRegionType; i+=1){
    m_PPToSL[i] = new TGCConnectionPPToSL;
    status = status && m_PPToSL[i]->readData((TGCRegionType)(i+1));
  }

  // CW for SL
  std::string ver_BW   = ver;
  std::string ver_EIFI = ver;
  std::string ver_TILE = ver;
  std::string ver_NSW   = ver;
  std::string ver_HotRoI   = ver;

  std::vector<std::string> vers = TGCDatabaseManager::splitCW(ver, '_');
  if (vers.size() == 3) { // for Run2
    ver_BW   = "v" + vers[2];
    ver_EIFI = "v" + vers[1];
    ver_TILE = "v" + vers[0];
  }
  else if(vers.size() == 5 && tgcArgs()->useRun3Config()) { // for Run3
    ver_BW   = "v" + vers[4];
    ver_EIFI = "v" + vers[3];
    ver_TILE = "v" + vers[2];
    ver_NSW  = "v" + vers[1];
    ver_HotRoI = "v" + vers[0];
  }

  // RPhi Coincidence Map
  for (int side=0; side<NumberOfSide; side +=1) {
    for (int oct=0; oct<NumberOfOctant; oct++) {
      m_mapRphi[side][oct] = new TGCRPhiCoincidenceMap(tgcArgs(),readCondKey, ver_BW, side, oct);
    }
  }

  // EIFI Coincidence Map
  ATH_MSG_DEBUG("start to create EIFI coincidence map.");
  for (int side=0; side<NumberOfSide; side +=1) {
    m_mapEIFI[side] = new TGCEIFICoincidenceMap(tgcArgs(), readCondKey, ver_EIFI, side);
  }

  // Tile-Mu coincidence Map
  m_mapTileMu = new TGCTileMuCoincidenceMap(tgcArgs(), readCondKey, ver_TILE);


  if(tgcArgs()->useRun3Config()){

    // NSW coincidence Map
    if(tgcArgs()->USE_NSW()){
      for (int side=0; side<NumberOfSide; side +=1) {
	for (int oct=0; oct<NumberOfOctant; oct++) {
	  for(int mod=0; mod<NumberOfModuleInBW; mod++){
	    // NSW Coincidence Map
	    m_mapNSW[side][oct][mod].reset(new TGCNSWCoincidenceMap(tgcArgs(),ver_NSW,side,oct,mod));
	  }
	}
      }
    }

    //Hot RoI LUT
    m_mapGoodMF.reset(new TGCGoodMF(tgcArgs(),ver_HotRoI));
  }
}

void TGCDatabaseManager::deleteConnectionPPToSL()
{
  for(int i=0; i<NumberOfRegionType; i+=1){
    delete m_PPToSL[i];
    m_PPToSL[i]=0;
  }
}

TGCDatabaseManager::~TGCDatabaseManager()
{

  int i,j,k;
  for( j=0; j<NumberOfRegionType; j+=1){
    for( i=0; i<NumberOfPatchPanelType; i+=1){
      for( k=0; k<TotalNumForwardBackwardType; k+=1){
	if(m_ASDToPP[j][i][k]!=0) delete m_ASDToPP[j][i][k];
	m_ASDToPP[j][i][k]=0;
      }
    }
    if(m_PPToSL[j]!=0) delete m_PPToSL[j];
    m_PPToSL[j]=0;
  }

  for (int side=0; side<NumberOfSide; side +=1) {
    for (int oct=0; oct<NumberOfOctant; oct++)
       delete m_mapRphi[side][oct];
  }

  for(int side=0; side<NumberOfSide; side +=1) {
    delete m_mapEIFI[side];
  }

  delete m_mapTileMu;
  m_mapTileMu = 0;
}

TGCDatabaseManager::TGCDatabaseManager(const TGCDatabaseManager& right)
  : AthMessaging(Athena::getMessageSvc(), "LVL1TGC::TGCDatabaseManager")
{
  for(int j=0; j<NumberOfRegionType; j+=1){
    for(int i=0; i<NumberOfPatchPanelType; i+=1){
      for(int  k=0; k<TotalNumForwardBackwardType; k+=1){
	m_ASDToPP[j][i][k] = 0;
      }
    }
  }
  for( int i=0; i<NumberOfRegionType; i+=1) m_PPToSL[i] = 0;
  for (int side=0; side<NumberOfSide; side +=1) {
    m_mapEIFI[side] =0;
    
    for (int oct=0; oct<NumberOfOctant; oct++) {
      m_mapRphi[side][oct] = 0;
    }
  }
  m_mapTileMu = 0;

  *this = right;
}

TGCDatabaseManager&
TGCDatabaseManager::operator=(const TGCDatabaseManager& right)
{
  if(this!=&right){
    for( int j=0; j<NumberOfRegionType; j+=1){
      for( int i=0; i<NumberOfPatchPanelType; i+=1){
	for( int k=0; k<TotalNumForwardBackwardType; k+=1){
	  if(m_ASDToPP[j][i][k]!=0) delete m_ASDToPP[j][i][k];
	  m_ASDToPP[j][i][k] = new TGCConnectionASDToPP;
	  *m_ASDToPP[j][i][k] = *right.m_ASDToPP[j][i][k];
	}
      }
    }
    for(int i=0; i<NumberOfRegionType; i+=1){
      if(m_PPToSL[i]!=0) delete m_PPToSL[i];
      m_PPToSL[i] = new TGCConnectionPPToSL( *right.m_PPToSL[i] );
    }
    
    for (int side=0; side<NumberOfSide; side +=1) {
      if (m_mapEIFI[side]!=0) delete m_mapEIFI[side];
      m_mapEIFI[side] = new TGCEIFICoincidenceMap(*(right.m_mapEIFI[side]));
      
      for (int oct=0; oct<NumberOfOctant; oct++) {
	if(m_mapRphi[side][oct]!=0) delete m_mapRphi[side][oct];
	m_mapRphi[side][oct] = new TGCRPhiCoincidenceMap(*(right.m_mapRphi[side][oct]));
      }
    }

  m_mapTileMu = new TGCTileMuCoincidenceMap(*(right.m_mapTileMu));

    m_patchPanelToConnectionInPP = right.m_patchPanelToConnectionInPP;
  }
  return *this;
}

std::string TGCDatabaseManager::getFilename(int type)
{
  switch (type) {
   case 0: //ASD2PP
    return "MuonTGC_Cabling_ASD2PP.db";
   case 1: //PP
    return "MuonTGC_Cabling_PP.db";
   case 2: //PP2SL
    return "MuonTGC_Cabling_PP2SL.db";
  }
  return "";
}

const std::vector<std::string> TGCDatabaseManager::splitCW(const std::string& input, char delimiter)
{
  std::istringstream stream(input);

  std::string field;
  std::vector<std::string> result;
  while (std::getline(stream, field, delimiter)) {
    result.push_back(field);
  }
  return result;
}

} //end of namespace bracket
