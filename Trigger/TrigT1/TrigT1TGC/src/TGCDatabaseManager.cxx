/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCDatabaseManager.hh"
#include "TrigT1TGC/TGCConnectionPPToSL.hh"
#include "TrigT1TGC/TGCRPhiCoincidenceMap.hh"
#include "TrigT1TGC/TGCInnerCoincidenceMap.hh"
#include "TrigT1TGC/TGCTileMuCoincidenceMap.hh"
#include "TrigT1TGC/TGCConnectionASDToPP.hh"
#include "TrigT1TGC/TGCConnectionInPP.hh"
#include "TrigT1TGC/TGCPatchPanel.hh"
#include <fstream>
#include <iostream>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

namespace LVL1TGCTrigger {

 extern bool        g_DEBUGLEVEL;

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

TGCDatabaseManager::TGCDatabaseManager()
  : mapTileMu(0)
{
  // default constructor 
  int i,j,k;
  for( j=0; j<NumberOfRegionType; j+=1){
    for( i=0; i<NumberOfPatchPanelType; i+=1){
       for( k=0; k<TotalNumForwardBackwardType; k+=1){
         ASDToPP[j][i][k] = 0;
       }
    }
  }
  for( i=0; i<NumberOfRegionType; i+=1){
    PPToSL[i] = 0;
  }
  for (int side=0; side<NumberOfSide; side +=1) {
    mapInner[side] = 0;
    for (int oct=0; oct<NumberOfOctant; oct++) {
      mapRphi[side][oct] = 0;
    }
  }

}

TGCDatabaseManager::TGCDatabaseManager(const std::string& ver, bool )
{
  bool status = true;

  int i,j,k;
  for( j=0; j<NumberOfRegionType; j+=1) {
    for( i=0; i<NumberOfPatchPanelType; i+=1){
       for( k=0; k<TotalNumForwardBackwardType; k+=1){
	 ASDToPP[j][i][k] = new TGCConnectionASDToPP;
	 status = status && 
	   ASDToPP[j][i][k]->readData((TGCRegionType)(j+1),i,(TGCForwardBackwardType)k);
       }
    }
  }
  for( i=0; i<NumberOfRegionType; i+=1){
    PPToSL[i] = new TGCConnectionPPToSL;
    status = status && PPToSL[i]->readData((TGCRegionType)(i+1));
  }

  if (g_DEBUGLEVEL) {
    IMessageSvc* msgSvc = 0;
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    if (svcLocator->service("MessageSvc", msgSvc) == StatusCode::FAILURE) {
      // do nothing
    } else {
      MsgStream log(msgSvc, "TGCDatabaseManager::TGCDatabaseManager");
      log << MSG::DEBUG 
	  << " Fail to read data from database" 
	  << endmsg;
    }
  }

  // CW for SL
  std::string ver_BW   = ver;
  std::string ver_EIFI = ver;
  std::string ver_TILE = ver;

  std::vector<std::string> vers = TGCDatabaseManager::splitCW(ver, '_');
  if (vers.size() == 3) {
    ver_BW   = "v" + vers[2];
    ver_EIFI = "v" + vers[1];
    ver_TILE = "v" + vers[0];
  }

  // RPhi Coincidence Map
  for (int side=0; side<NumberOfSide; side +=1) {
    for (int oct=0; oct<NumberOfOctant; oct++) {
       mapRphi[side][oct] = new TGCRPhiCoincidenceMap(ver_BW, side, oct);
    }
  }

  // Inner Coincidence Map
  for (int side=0; side<NumberOfSide; side +=1) {
    mapInner[side] = new TGCInnerCoincidenceMap(ver_EIFI, side);
  }

  // Tile-Mu coincidence Map
  mapTileMu = new TGCTileMuCoincidenceMap(ver_TILE);
   
 
}

void TGCDatabaseManager::deleteConnectionPPToSL()
{
  for(int i=0; i<NumberOfRegionType; i+=1){
    delete PPToSL[i];
    PPToSL[i]=0;
  }
}

TGCDatabaseManager::~TGCDatabaseManager()
{

  int i,j,k;
  for( j=0; j<NumberOfRegionType; j+=1){
    for( i=0; i<NumberOfPatchPanelType; i+=1){
      for( k=0; k<TotalNumForwardBackwardType; k+=1){
	if(ASDToPP[j][i][k]!=0) delete ASDToPP[j][i][k];
	ASDToPP[j][i][k]=0;
      }
    }
    if(PPToSL[j]!=0) delete PPToSL[j];
    PPToSL[j]=0;
  }

  for (int side=0; side<NumberOfSide; side +=1) {
    for (int oct=0; oct<NumberOfOctant; oct++)
       delete mapRphi[side][oct];
  }

  for(int side=0; side<NumberOfSide; side +=1) {
    delete mapInner[side];
  }

  delete mapTileMu;
  mapTileMu = 0;
}

TGCDatabaseManager::TGCDatabaseManager(const TGCDatabaseManager& right)
{
  for(int j=0; j<NumberOfRegionType; j+=1){
    for(int i=0; i<NumberOfPatchPanelType; i+=1){
      for(int  k=0; k<TotalNumForwardBackwardType; k+=1){
	ASDToPP[j][i][k] = 0;
      }
    }
  }
  for( int i=0; i<NumberOfRegionType; i+=1) PPToSL[i] = 0;
  for (int side=0; side<NumberOfSide; side +=1) {
    mapInner[side] =0;
    
    for (int oct=0; oct<NumberOfOctant; oct++) {
      mapRphi[side][oct] = 0;
    }
  }
  mapTileMu = 0;

  *this = right;
}

TGCDatabaseManager&
TGCDatabaseManager::operator=(const TGCDatabaseManager& right)
{
  if(this!=&right){
    for( int j=0; j<NumberOfRegionType; j+=1){
      for( int i=0; i<NumberOfPatchPanelType; i+=1){
	for( int k=0; k<TotalNumForwardBackwardType; k+=1){
	  if(ASDToPP[j][i][k]!=0) delete ASDToPP[j][i][k];
	  ASDToPP[j][i][k] = new TGCConnectionASDToPP;
	  *ASDToPP[j][i][k] = *right.ASDToPP[j][i][k];
	}
      }
    }
    for(int i=0; i<NumberOfRegionType; i+=1){
      if(PPToSL[i]!=0) delete PPToSL[i];
      PPToSL[i] = new TGCConnectionPPToSL( *right.PPToSL[i] );
    }
    
    for (int side=0; side<NumberOfSide; side +=1) {
      if (mapInner[side]!=0) delete mapInner[side];
      mapInner[side] = new TGCInnerCoincidenceMap(*(right.mapInner[side]));
      
      for (int oct=0; oct<NumberOfOctant; oct++) {
	if(mapRphi[side][oct]!=0) delete mapRphi[side][oct];
	mapRphi[side][oct] = new TGCRPhiCoincidenceMap(*(right.mapRphi[side][oct]));
      }
    }

  mapTileMu = new TGCTileMuCoincidenceMap(*(right.mapTileMu));

    m_patchPanelToConnectionInPP = right.m_patchPanelToConnectionInPP;
  }
  return *this;
}

const std::string& TGCDatabaseManager::getFilename(int type)
{
  static std::string fn="";
  switch (type) {
   case 0: //ASD2PP
    fn = "MuonTGC_Cabling_ASD2PP.db";
    break; 
   case 1: //PP
    fn = "MuonTGC_Cabling_PP.db";
    break;
   case 2: //PP2SL
    fn = "MuonTGC_Cabling_PP2SL.db";
    break;
   default:
    break;
  }
  return fn;
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
