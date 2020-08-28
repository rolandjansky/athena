/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonReadoutGeometry/MdtDetectorElement.h"
#include "MuonReadoutGeometry/RpcDetectorElement.h"
#include "MuonReadoutGeometry/TgcDetectorElement.h"
#include "MuonReadoutGeometry/CscDetectorElement.h"
#include "MuonReadoutGeometry/MuonStation.h"
#include "MuonReadoutGeometry/GlobalUtilities.h"
#include "MuonAlignmentData/ALinePar.h"
#include "MuonAlignmentData/BLinePar.h"
#include "MuonAlignmentData/CscInternalAlignmentPar.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <TString.h> // for Form

#ifndef SIMULATIONBASE
#include "MuonCondSvc/NSWCondUtils.h"
#endif

namespace MuonGM {

MuonDetectorManager::MuonDetectorManager() {

  for (unsigned int i=0; i<NRpcStatType; i++)
    for (unsigned int j=0; j<NRpcStatEta; j++)
      for (unsigned int k=0; k<NRpcStatPhi; k++)
        for (unsigned int l=0; l<NDoubletR; l++)
          for (unsigned int h=0; h<NDoubletZ; h++)
              m_rpcArray[i][j][k][l][h] = nullptr;
  for (unsigned int i=0; i<NMdtStatType; i++)
    for (unsigned int j=0; j<NMdtStatEta; j++)
      for (unsigned int k=0; k<NMdtStatPhi; k++)
        for (unsigned int l=0; l<NMdtMultilayer; l++)
                   m_mdtArray[i][j][k][l] = nullptr;
  for (unsigned int i=0; i<NCscStatType; i++)
    for (unsigned int j=0; j<NCscStatEta; j++)
      for (unsigned int k=0; k<NCscStatPhi; k++)
        for (unsigned int l=0; l<NCscChamberLayer; l++)
                   m_cscArray[i][j][k][l] = nullptr;
  for (unsigned int i=0; i<NTgcStatType; i++)
    for (unsigned int j=0; j<NTgcStatEta; j++)
      for (unsigned int k=0; k<NTgcStatPhi; k++)
                 m_tgcArray[i][j][k] = nullptr;

  for (unsigned int j=0; j<NsTgStatEta; j++)
    for (unsigned int k=0; k<NsTgStatPhi; k++)
      for (unsigned int l=0; l<NsTgChamberLayer; l++)
    m_stgArray[j][k][l] = nullptr;

  for (unsigned int j=0; j<NMMcStatEta; j++)
    for (unsigned int k=0; k<NMMcStatPhi; k++)
      for (unsigned int l=0; l<NMMcChamberLayer; l++)
    m_mmcArray[j][k][l] = nullptr;


  for (unsigned int i=0; i<MdtRElMaxHash; ++i)
      m_mdtArrayByHash[i] = nullptr;
  for (unsigned int i=0; i<CscRElMaxHash; ++i)
      m_cscArrayByHash[i] = nullptr;
  for (unsigned int i=0; i<RpcRElMaxHash; ++i)
      m_rpcArrayByHash[i] = nullptr;
  for (unsigned int i=0; i<TgcRElMaxHash; ++i)
      m_tgcArrayByHash[i] = nullptr;

     for (unsigned int i=0; i<TgcDetElMaxHash; ++i)
         m_tgcDEArray[i] = nullptr;
  for (unsigned int i=0; i<MdtDetElMaxHash; ++i)
      m_mdtDEArray[i] = nullptr;
   for (unsigned int i=0; i<CscDetElMaxHash; ++i)
        m_cscDEArray[i] = nullptr;
     for (unsigned int i=0; i<RpcDetElMaxHash; ++i)
        m_rpcDEArray[i] = nullptr;

  m_n_mdtDE = m_n_cscDE = m_n_tgcDE = m_n_rpcDE = 0;
  m_n_mdtRE = m_n_cscRE = m_n_tgcRE = m_n_rpcRE = m_n_mmcRE = m_n_stgRE = 0;
  setName("Muon");

  m_cachingFlag      = 1;
  m_cacheFillingFlag = 1;
  m_minimalgeo = 0;
  m_includeCutouts = 0;
  m_includeCutoutsBog = 0;
  m_controlAlines = 111111;
  m_applyMdtDeformations  = 0;
  m_applyMdtAsBuiltParams = 0;
  m_useCscIntAlign = false;
  m_controlCscIlines = 111111;
  m_useCscIlinesFromGM = true;
    
  m_mdtIdHelper = nullptr;
  m_cscIdHelper = nullptr;
  m_rpcIdHelper = nullptr;
  m_tgcIdHelper = nullptr;
  m_stgcIdHelper = nullptr;
  m_mmIdHelper = nullptr;
 
}

MuonDetectorManager::~MuonDetectorManager() {


  for (unsigned int p=0;p<m_envelope.size();++p) {
      m_envelope[p]->unref();
  }


  for (unsigned int i=0; i<NRpcStatType; ++i)
    for (unsigned int j=0; j<NRpcStatEta; ++j)
      for (unsigned int k=0; k<NRpcStatPhi; ++k)
        for (unsigned int l=0; l<NDoubletR; ++l)
          for (unsigned int h=0; h<NDoubletZ; ++h) {
            delete m_rpcArray[i][j][k][l][h]; m_rpcArray[i][j][k][l][h] = nullptr;
          }
  for (unsigned int i=0; i<NMdtStatType; ++i)
    for (unsigned int j=0; j<NMdtStatEta; ++j)
      for (unsigned int k=0; k<NMdtStatPhi; ++k)
        for (unsigned int l=0; l<NMdtMultilayer; ++l) {
          delete m_mdtArray[i][j][k][l]; m_mdtArray[i][j][k][l] = nullptr;
        }
  for (unsigned int i=0; i<NTgcStatType; ++i)
    for (unsigned int j=0; j<NTgcStatEta; ++j)
      for (unsigned int k=0; k<NTgcStatPhi; ++k) {
        delete m_tgcArray[i][j][k]; m_tgcArray[i][j][k] = nullptr;
      }
  for (unsigned int i=0; i<NCscStatType; ++i)
    for (unsigned int j=0; j<NCscStatEta; ++j)
      for (unsigned int k=0; k<NCscStatPhi; ++k)
        for (unsigned int l=0; l<NCscChamberLayer; ++l) {
          delete m_cscArray[i][j][k][l]; m_cscArray[i][j][k][l] = nullptr;
        }
  for (unsigned int j=0; j<NsTgStatEta; j++)
    for (unsigned int k=0; k<NsTgStatPhi; k++)
      for (unsigned int l=0; l<NsTgChamberLayer; l++){
          delete m_stgArray[j][k][l]; m_stgArray[j][k][l] = nullptr;
      }
  for (unsigned int j=0; j<NMMcStatEta; j++)
    for (unsigned int k=0; k<NMMcStatPhi; k++)
      for (unsigned int l=0; l<NMMcChamberLayer; l++){
         delete m_mmcArray[j][k][l]; m_mmcArray[j][k][l] = nullptr;
      }

  for (unsigned int i=0; i<MdtDetElMaxHash; ++i) {
    delete m_mdtDEArray[i]; m_mdtDEArray[i] = nullptr;
  }
  for (unsigned int i=0; i<RpcDetElMaxHash; ++i) {
    delete m_rpcDEArray[i]; m_rpcDEArray[i] = nullptr;
  }
  for (unsigned int i=0; i<CscDetElMaxHash; ++i) {
    delete m_cscDEArray[i]; m_cscDEArray[i] = nullptr;
  }
  for (unsigned int i=0; i<TgcDetElMaxHash; ++i) {
    delete m_tgcDEArray[i]; m_tgcDEArray[i] = nullptr;
  }
  for (std::map< std::string, MuonStation * >::iterator i =
           m_MuonStationMap.begin(); i != m_MuonStationMap.end(); ++i)
  {
    delete i->second; i->second = nullptr;
  }
}


void MuonDetectorManager::clearCache()
{
  clearMdtCache();
  clearRpcCache();
  clearTgcCache();
  clearCscCache();    
}

void MuonDetectorManager::refreshCache()
{
  refreshMdtCache();
  refreshRpcCache();
  refreshTgcCache();
  refreshCscCache();    
}
void MuonDetectorManager::refreshMdtCache()
{
  clearMdtCache(); // NEED to fill since FillCacheInitTime = 1 is the default now.
  fillMdtCache();
}
void MuonDetectorManager::refreshRpcCache()
{
  clearRpcCache(); 
  fillRpcCache(); 
}
void MuonDetectorManager::refreshTgcCache()
{
  clearTgcCache(); 
  fillTgcCache(); 
}
void MuonDetectorManager::refreshCscCache()
{
  clearCscCache(); 
  fillCscCache(); 
}
void MuonDetectorManager::refreshMMCache()
{
  clearMMCache(); 
  fillMMCache(); 
}
void MuonDetectorManager::refreshsTgcCache()
{
  clearsTgcCache(); 
  fillsTgcCache(); 
}

   
void MuonDetectorManager::clearMdtCache()
{
    for (unsigned int i=0; i<NMdtStatType; ++i)
      for (unsigned int j=0; j<NMdtStatEta; ++j)
        for (unsigned int k=0; k<NMdtStatPhi; ++k)
          for (unsigned int l=0; l<NMdtMultilayer; ++l) {
              if (m_mdtArray[i][j][k][l]) m_mdtArray[i][j][k][l]->clearCache();
          }
}   
void MuonDetectorManager::clearRpcCache()
{
    for (unsigned int i=0; i<NRpcStatType; ++i)
      for (unsigned int j=0; j<NRpcStatEta; ++j)
        for (unsigned int k=0; k<NRpcStatPhi; ++k)
          for (unsigned int l=0; l<NDoubletR; ++l)
            for (unsigned int h=0; h<NDoubletZ; ++h) {
              if( m_rpcArray[i][j][k][l][h] ) m_rpcArray[i][j][k][l][h]->clearCache() ;
            }
}   
void MuonDetectorManager::clearTgcCache()
{
    for (unsigned int i=0; i<NTgcStatType; ++i)
      for (unsigned int j=0; j<NTgcStatEta; ++j)
        for (unsigned int k=0; k<NTgcStatPhi; ++k) {
          if ( m_tgcArray[i][j][k] ) m_tgcArray[i][j][k]->clearCache();
        }
}   
void MuonDetectorManager::clearCscCache()
{
  if (nCscRE()<1) return;
    for (unsigned int i=0; i<NCscStatType; ++i)
      for (unsigned int j=0; j<NCscStatEta; ++j)
        for (unsigned int k=0; k<NCscStatPhi; ++k)
          for (unsigned int l=0; l<NCscChamberLayer; ++l) {
            if( m_cscArray[i][j][k][l] ) m_cscArray[i][j][k][l]->clearCache();
          }
}   
void MuonDetectorManager::clearMMCache()
{
  if (nMMRE()<1) return;
      for (unsigned int j=0; j<NMMcStatEta; ++j)
        for (unsigned int k=0; k<NMMcStatPhi; ++k)
          for (unsigned int l=0; l<NMMcChamberLayer; ++l) {
              if (m_mmcArray[j][k][l]) m_mmcArray[j][k][l]->clearCache();
          }
}   
void MuonDetectorManager::clearsTgcCache()
{
  if (nsTgcRE()<1) return;
      for (unsigned int j=0; j<NsTgStatEta; ++j)
        for (unsigned int k=0; k<NsTgStatPhi; ++k)
          for (unsigned int l=0; l<NsTgChamberLayer; ++l) {
              if (m_stgArray[j][k][l]) m_stgArray[j][k][l]->clearCache();
          }
}   
void MuonDetectorManager::fillMMCache()
{
  if (nMMRE()<1) return;
      for (unsigned int j=0; j<NMMcStatEta; ++j)
        for (unsigned int k=0; k<NMMcStatPhi; ++k)
          for (unsigned int l=0; l<NMMcChamberLayer; ++l) {
              if (m_mmcArray[j][k][l]) m_mmcArray[j][k][l]->fillCache();
          }
}   
void MuonDetectorManager::fillsTgcCache()
{
  if (nsTgcRE()<1) return;
      for (unsigned int j=0; j<NsTgStatEta; ++j)
        for (unsigned int k=0; k<NsTgStatPhi; ++k)
          for (unsigned int l=0; l<NsTgChamberLayer; ++l) {
              if (m_stgArray[j][k][l]) m_stgArray[j][k][l]->fillCache();
          }
}   
void MuonDetectorManager::fillCache()
{
    MsgStream log(Athena::getMessageSvc(),"MGM::MuonDetectorManager");
    log<<MSG::INFO<<"Filling cache"<<endmsg;
    fillMdtCache();
    fillRpcCache();
    fillTgcCache();
    fillCscCache();    
}   
void MuonDetectorManager::fillMdtCache()
{
    for (unsigned int i=0; i<NMdtStatType; ++i)
      for (unsigned int j=0; j<NMdtStatEta; ++j)
        for (unsigned int k=0; k<NMdtStatPhi; ++k)
          for (unsigned int l=0; l<NMdtMultilayer; ++l) {
              if (m_mdtArray[i][j][k][l]) m_mdtArray[i][j][k][l]->fillCache();
          }
}   
void MuonDetectorManager::fillRpcCache()
{
    for (unsigned int i=0; i<NRpcStatType; ++i)
      for (unsigned int j=0; j<NRpcStatEta; ++j)
        for (unsigned int k=0; k<NRpcStatPhi; ++k)
          for (unsigned int l=0; l<NDoubletR; ++l)
            for (unsigned int h=0; h<NDoubletZ; ++h) {
              if( m_rpcArray[i][j][k][l][h] ) m_rpcArray[i][j][k][l][h]->fillCache() ;
            }
}   
void MuonDetectorManager::fillTgcCache()
{
    for (unsigned int i=0; i<NTgcStatType; ++i)
      for (unsigned int j=0; j<NTgcStatEta; ++j)
        for (unsigned int k=0; k<NTgcStatPhi; ++k) {
          if ( m_tgcArray[i][j][k] ) m_tgcArray[i][j][k]->fillCache();
        }
}   
void MuonDetectorManager::fillCscCache()
{
  if (nCscRE()<1) return;
    for (unsigned int i=0; i<NCscStatType; ++i)
      for (unsigned int j=0; j<NCscStatEta; ++j)
        for (unsigned int k=0; k<NCscStatPhi; ++k)
          for (unsigned int l=0; l<NCscChamberLayer; ++l) {
            if( m_cscArray[i][j][k][l] ) m_cscArray[i][j][k][l]->fillCache();
          }
}   
    
unsigned int MuonDetectorManager::getNumTreeTops() const {
    return m_envelope.size();
}

PVConstLink MuonDetectorManager::getTreeTop(unsigned int i) const{
    return m_envelope[i];
}
  

void MuonDetectorManager::addTreeTop( PVLink pV) {
    pV->ref();
    m_envelope.push_back(pV);
}

void MuonDetectorManager::addMuonStation (MuonStation* mst)
{
    std::string key = muonStationKey(mst->getStationType(), mst->getEtaIndex(), mst->getPhiIndex());
    m_MuonStationMap[key] = mst;
}

std::string
MuonDetectorManager::muonStationKey(std::string stName, int statEtaIndex, int statPhiIndex) const
{
    std::string key;
    if (statEtaIndex<0)
        key = stName.substr(0,3)
              +"_C_zi"+MuonGM::buildString(std::abs(statEtaIndex),2)
              +"fi"+MuonGM::buildString(statPhiIndex,2);
    else
        key = stName.substr(0,3)
              +"_A_zi"+MuonGM::buildString(std::abs(statEtaIndex),2)
              +"fi"+MuonGM::buildString(statPhiIndex,2);
    return key;
}
        
const MuonStation*
MuonDetectorManager::getMuonStation(std::string stName, int stEtaIndex, int stPhiIndex) const 
{
    
    std::string key = muonStationKey(stName, stEtaIndex, stPhiIndex);

    std::map< std::string, MuonStation *>::const_iterator it = m_MuonStationMap.find(key);
    if ( it != m_MuonStationMap.end()) return (*it).second;
    else return nullptr;
}

MuonStation*
MuonDetectorManager::getMuonStation(std::string stName, int stEtaIndex, int stPhiIndex)
{
    
    std::string key = muonStationKey(stName, stEtaIndex, stPhiIndex);

    std::map< std::string, MuonStation *>::const_iterator it = m_MuonStationMap.find(key);
    if ( it != m_MuonStationMap.end()) return (*it).second;
    else return nullptr;
}

void MuonDetectorManager::addRpcReadoutElement (RpcReadoutElement* x, Identifier id)
{
    // check if RE has id as identity 
    if (id!=x->identify()) {
      throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addRpcReadoutElement() - Trying to add RpcReadoutElement with id %s not matching the id assigned to the RpcReadoutElement %s", __FILE__, __LINE__, m_rpcIdHelper->show_to_string(id).c_str(), m_rpcIdHelper->show_to_string(x->identify()).c_str()));
    }
    
    // add RE to map by RE hash 
    IdentifierHash Idhash= x->detectorElementHash();
    if (Idhash >= RpcRElMaxHash) {  
      throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addRpcReadoutElement() - Trying to add RpcReadoutElement with detector-element-hash id %d outside boundaries 0-%d", __FILE__, __LINE__, (unsigned int)Idhash, RpcRElMaxHash));
    }
    else {
      if (m_rpcArrayByHash[Idhash]!=nullptr) {
        throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addRpcReadoutElement() - Trying to add RpcReadoutElement with detector-element-hash id %d id = %s at location already taken by %s", __FILE__, __LINE__, (unsigned int)Idhash, m_rpcIdHelper->show_to_string(id).c_str(), m_rpcIdHelper->show_to_string(m_rpcArrayByHash[Idhash]->identify()).c_str()));
      }
      m_rpcArrayByHash[Idhash] = x;
    }

    int stname_index = rpcStationTypeIdx(m_rpcIdHelper->stationName(id));
    int steta_index  = m_rpcIdHelper->stationEta(id)  + NRpcStEtaOffset;
    int stphi_index  = m_rpcIdHelper->stationPhi(id)  - 1;
    int dbr_index    = m_rpcIdHelper->doubletR(id)    - 1;
    int dbz_index    = m_rpcIdHelper->doubletZ(id)    - 1;
    int doubletPhi   = m_rpcIdHelper->doubletPhi(id);

  // BMS 5/ |stEta|= 2 / dbR = 1 and 2 / dbZ = 3
  // BMS 6/ |stEta|= 4 / dbR = 2 / dbZ = 3
  // BMS 6/ |stEta|= 4 / dbR = 1 / dbZ = 2
  // these are the special cases where we want the rpc at doubletPhi = 2
  // to be addressed with a dbz_index=dbZ+1
  if (m_rpcIdHelper->stationNameString(m_rpcIdHelper->stationName(id)) == "BMS")
  {
      if (std::abs(m_rpcIdHelper->stationEta(id)) == 2 &&
          m_rpcIdHelper->doubletZ(id) == 3)
      {
          if (doubletPhi == 2) dbz_index++;
      }
      else if (std::abs(m_rpcIdHelper->stationEta(id)) == 4 &&
               m_rpcIdHelper->doubletR(id) == 2 &&
               m_rpcIdHelper->doubletZ(id) == 3)
      {
          if (doubletPhi == 2) dbz_index++;
      }
      else if (std::abs(m_rpcIdHelper->stationEta(id)) == 4 &&
               m_rpcIdHelper->doubletR(id) == 1 &&
               m_rpcIdHelper->doubletZ(id) == 2)
      {
          if (doubletPhi == 2) dbz_index++;
      }      
  }
  
  if (stname_index<0 || stname_index >= NRpcStatType) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addRpcReadoutElement() - stNameindex out of range %d 0-%d", __FILE__, __LINE__, stname_index, NRpcStatType-1));
  }
  if (steta_index<0 || steta_index >= NRpcStatEta) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addRpcReadoutElement() - stEtaindex out of range %d 0-%d", __FILE__, __LINE__, steta_index, NRpcStatEta-1));
  }
  if (stphi_index<0 ||  stphi_index>= NRpcStatPhi) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addRpcReadoutElement() - stPhiindex out of range %d 0-%d", __FILE__, __LINE__, stphi_index, NRpcStatPhi-1));
  }
  if (dbr_index < 0 || dbr_index >= NDoubletR) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addRpcReadoutElement() - dbr_index out of range %d 0-%d", __FILE__, __LINE__, dbr_index, NDoubletR-1));
  }
  if (dbz_index < 0 || dbz_index >= NDoubletZ) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addRpcReadoutElement() - dbz_index out of range %d 0-%d", __FILE__, __LINE__, dbz_index, NDoubletZ-1));
  }

    //add here the RpcDetectorElement and/or add this readoutElement to the DetectorElement
    IdentifierHash idh = x->collectionHash();
    if (idh< RpcDetElMaxHash) 
    {
	if ( !(m_rpcDEArray[idh]) )
	{
	    m_rpcDEArray[idh] = new RpcDetectorElement(0, this, m_rpcIdHelper->elementID(id), idh);
	    m_n_rpcDE++;
	}
	m_rpcDEArray[idh]->addRpcReadoutElement(x, dbz_index);
    }
    else {
      throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addRpcReadoutElement() - Trying to add RpcDetectorElement with data-collection-hash id %d outside boundaries 0-%d", __FILE__, __LINE__, (unsigned int)idh, RpcDetElMaxHash));
    }

  if (m_rpcArray[stname_index][steta_index][stphi_index][dbr_index][dbz_index] != nullptr) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addRpcReadoutElement() - this place is taken [%d][%d][%d][%d][%d] current id is %s stored id %s", __FILE__, __LINE__, stname_index, steta_index, stphi_index, dbr_index, dbz_index, m_rpcIdHelper->show_to_string(id).c_str(), m_rpcIdHelper->show_to_string(m_rpcArray[stname_index][steta_index][stphi_index][dbr_index][dbz_index]->identify()).c_str()));
  }
  m_rpcArray[stname_index][steta_index][stphi_index][dbr_index][dbz_index] = x;
  m_n_rpcRE++;

}

const RpcReadoutElement* MuonDetectorManager::getRpcReadoutElement (const Identifier id) const
{
  int stationName = m_rpcIdHelper->stationName(id);
  int stname_index = rpcStationTypeIdx(stationName);
  int steta_index  = m_rpcIdHelper->stationEta(id)  + NRpcStEtaOffset;
  int stphi_index  = m_rpcIdHelper->stationPhi(id)  - 1;
  int dbr_index    = m_rpcIdHelper->doubletR(id)    - 1;
  int dbz_index    = m_rpcIdHelper->doubletZ(id)    - 1;
  int doubletPhi  = m_rpcIdHelper->doubletPhi(id);

  // BMS 5/ |stEta|= 2 / dbR = 1 and 2 / dbZ = 3
  // BMS 6/ |stEta|= 4 / dbR = 2 / dbZ = 3
  // BMS 6/ |stEta|= 4 / dbR = 1 / dbZ = 2
  // these are the special cases where we want the rpc at doubletPhi = 2
  // to be addressed with a dbz_index=dbZ+1
  if (m_rpcIdHelper->stationNameString(stationName) == "BMS") {
      if (std::abs(m_rpcIdHelper->stationEta(id)) == 2 &&
          m_rpcIdHelper->doubletZ(id) == 3)
      {
          if (doubletPhi == 2) dbz_index++;
      }
      else if (std::abs(m_rpcIdHelper->stationEta(id)) == 4 &&
               m_rpcIdHelper->doubletR(id) == 2 &&
               m_rpcIdHelper->doubletZ(id) == 3)
      {
          if (doubletPhi == 2) dbz_index++;
      }
      else if (std::abs(m_rpcIdHelper->stationEta(id)) == 4 &&
               m_rpcIdHelper->doubletR(id) == 1 &&
               m_rpcIdHelper->doubletZ(id) == 2)
      {
          if (doubletPhi == 2) dbz_index++;
      }      
  }
  
  return getRpcReadoutElement(stname_index,steta_index,stphi_index,dbr_index,dbz_index);
}

const MuonClusterReadoutElement* MuonDetectorManager::getMuonClusterReadoutElement (const Identifier id) const
{
  if( m_tgcIdHelper->is_tgc(id) ) return getTgcReadoutElement(id);
  if( m_rpcIdHelper->is_rpc(id) ) return getRpcReadoutElement(id);
  if( m_cscIdHelper->is_csc(id) ) return getCscReadoutElement(id);
  if( m_mmIdHelper->is_mm(id) ) return getMMReadoutElement(id);
  if( m_stgcIdHelper->is_stgc(id) ) return getsTgcReadoutElement(id);
  return nullptr;
}

const MMReadoutElement* MuonDetectorManager::getMMRElement_fromIdFields(int isSmall, int stEta, int stPhi, int ml) const
{
  int steta_index = stEta+NMMcStEtaOffset;
  if (stEta>0) steta_index = steta_index-1;
  int stphi_index = 2*(stPhi-1);
  if (isSmall==1) stphi_index = stphi_index+1; // small sectors
  int ml_index = ml-1;

  if (steta_index<0 || steta_index >= NMMcStatEta) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::getMMRElement_fromIdFields() - stEtaindex out of range %d 0-%d", __FILE__, __LINE__, steta_index, NMMcStatEta-1));
  }
  if (stphi_index<0 ||  stphi_index>= NMMcStatPhi) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::getMMRElement_fromIdFields() - stPhiindex out of range %d 0-%d", __FILE__, __LINE__, stphi_index, NMMcStatPhi-1));
  }
  if (ml_index < 0 || ml_index >= NMMcChamberLayer) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::getMMRElement_fromIdFields() - ml_index out of range %d 0-%d", __FILE__, __LINE__, ml_index, NMMcChamberLayer-1));
  }
  return m_mmcArray[steta_index][stphi_index][ml_index];
}
void MuonDetectorManager::addMMReadoutElement_withIdFields(MMReadoutElement* x, int isSmall, int stEta, int stPhi, int ml)
{
  int steta_index = stEta+NMMcStEtaOffset;
  if (stEta>0) steta_index = steta_index-1;
  int stphi_index = 2*(stPhi-1);
  if (isSmall==1) stphi_index = stphi_index+1; // small sectors
  int ml_index = ml-1;

  if (steta_index<0 || steta_index >= NMMcStatEta) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addMMReadoutElement_withIdFields() - stEtaindex out of range %d 0-%d", __FILE__, __LINE__, steta_index, NMMcStatEta-1));
  }
  if (stphi_index<0 ||  stphi_index>= NMMcStatPhi) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addMMReadoutElement_withIdFields() - stPhiindex out of range %d 0-%d", __FILE__, __LINE__, stphi_index, NMMcStatPhi-1));
  }
  if (ml_index < 0 || ml_index >= NMMcChamberLayer) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addMMReadoutElement_withIdFields() - ml_index out of range %d 0-%d", __FILE__, __LINE__, ml_index, NMMcChamberLayer-1));
  }
  if (m_mmcArray[steta_index][stphi_index][ml_index] != nullptr) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addMMReadoutElement_withIdFields() - this place is taken [%d][%d][%d] ......... this RE cannot be added", __FILE__, __LINE__, steta_index, stphi_index, ml_index));
  }
  m_mmcArray[steta_index][stphi_index][ml_index] = x;
  m_n_mmcRE++;
}
void MuonDetectorManager::addsTgcReadoutElement_withIdFields(sTgcReadoutElement* x, int isSmall, int stEta, int stPhi, int ml)
{
  int steta_index = stEta+NsTgStEtaOffset;
  if (stEta>0) steta_index = steta_index-1;
  int stphi_index = 2*(stPhi-1);
  if (isSmall==1) stphi_index = stphi_index+1; // small sectors
  int ml_index = ml-1;

  if (steta_index<0 || steta_index >= NsTgStatEta) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addsTgcReadoutElement_withIdFields() - stEtaindex out of range %d 0-%d", __FILE__, __LINE__, steta_index, NsTgStatEta-1));
  }
  if (stphi_index<0 ||  stphi_index>= NsTgStatPhi) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addsTgcReadoutElement_withIdFields() - stPhiindex out of range %d 0-%d", __FILE__, __LINE__, stphi_index, NsTgStatPhi-1));
  }
  if (ml_index < 0 || ml_index >= NsTgChamberLayer) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addsTgcReadoutElement_withIdFields() - ml_index out of range %d 0-%d", __FILE__, __LINE__, ml_index, NsTgChamberLayer-1));
  }
  if (m_stgArray[steta_index][stphi_index][ml_index] != nullptr) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addsTgcReadoutElement_withIdFields() - this place is taken [%d][%d][%d] ......... this RE cannot be added", __FILE__, __LINE__, steta_index, stphi_index, ml_index));
  }
  m_stgArray[steta_index][stphi_index][ml_index] = x;
  m_n_stgRE++;
}
const sTgcReadoutElement* MuonDetectorManager::getsTgcRElement_fromIdFields(int isSmall, int stEta, int stPhi, int ml) const
{
  int steta_index = stEta+NsTgStEtaOffset;
  if (stEta>0) steta_index = steta_index-1;
  int stphi_index = 2*(stPhi-1);
  if (isSmall==1) stphi_index = stphi_index+1; // small sectors
  int ml_index = ml-1;

  if (steta_index<0 || steta_index >= NsTgStatEta) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::getsTgcRElement_fromIdFields() - stEtaindex out of range %d 0-%d", __FILE__, __LINE__, steta_index, NsTgStatEta-1));
  }
  if (stphi_index<0 ||  stphi_index>= NsTgStatPhi) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::getsTgcRElement_fromIdFields() - stPhiindex out of range %d 0-%d", __FILE__, __LINE__, stphi_index, NsTgStatPhi-1));
  }
  if (ml_index < 0 || ml_index >= NsTgChamberLayer) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::getsTgcRElement_fromIdFields() - ml_index out of range %d 0-%d", __FILE__, __LINE__, ml_index, NsTgChamberLayer-1));
  }
  return m_stgArray[steta_index][stphi_index][ml_index];
}

void MuonDetectorManager::addMMReadoutElement (MMReadoutElement* x, Identifier id)
{
  if (id!=x->identify()) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addMMReadoutElement() - Trying to add MMReadoutElement with id %s not matching the id assigned to the MMReadoutElement %s", __FILE__, __LINE__, m_mmIdHelper->show_to_string(id).c_str(), m_mmIdHelper->show_to_string(x->identify()).c_str()));
  }
}    
void MuonDetectorManager::addsTgcReadoutElement (sTgcReadoutElement* x, Identifier id)
{
  if (id!=x->identify()) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addsTgcReadoutElement() - Trying to add sTgcReadoutElement with id %s not matching the id assigned to the sTgcReadoutElement %s", __FILE__, __LINE__, m_stgcIdHelper->show_to_string(id).c_str(), m_stgcIdHelper->show_to_string(x->identify()).c_str()));
  }
}    
void MuonDetectorManager::addMdtReadoutElement (MdtReadoutElement* x, Identifier id)
{
    if (id!=x->identify()) {
      throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addMdtReadoutElement() - Trying to add MdtReadoutElement with id %s not matching the id assigned to the MdtReadoutElement %s", __FILE__, __LINE__, m_mdtIdHelper->show_to_string(id).c_str(), m_mdtIdHelper->show_to_string(x->identify()).c_str()));
    }
    
    int stName = m_mdtIdHelper->stationName(id);
    int stEta  = m_mdtIdHelper->stationEta(id);
    int stPhi  = m_mdtIdHelper->stationPhi(id);
    int ml     = m_mdtIdHelper->multilayer(id);
    
    
    //add here the MdtReadoutElement to the array by RE hash
    //use already known RE hash 
    IdentifierHash Idhash = 0;
    Idhash = x->detectorElementHash();
    if (Idhash >= MdtRElMaxHash) {
      throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addMdtReadoutElement() - Trying to add MdtReadoutElement with detector-element-hash id %d outside boundaries 0-%d", __FILE__, __LINE__, (unsigned int)Idhash, MdtRElMaxHash));
    } else {
      if (m_mdtArrayByHash[Idhash]!=nullptr) {
        throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addMdtReadoutElement() - Trying to add MdtReadoutElement with detector-element-hash id %d id = %s at location already taken by %s", __FILE__, __LINE__, (unsigned int)Idhash, m_mdtIdHelper->show_to_string(id).c_str(), m_mdtIdHelper->show_to_string(m_mdtArrayByHash[Idhash]->identify()).c_str()));
      }
      m_mdtArrayByHash[Idhash] = x;
    }
    //add here the MdtDetectorElement and/or add this readoutElement to the DetectorElement
    //use already known data-collection hash 
    IdentifierHash idh = x->collectionHash();
    if (idh< MdtDetElMaxHash) {
	if ( !(m_mdtDEArray[idh]) ) {
	    m_mdtDEArray[idh] = new MdtDetectorElement(0, this, m_mdtIdHelper->elementID(id), idh);
	    m_n_mdtDE++;
	}
	m_mdtDEArray[idh]->addMdtReadoutElement(x, ml);
    } else {
      throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addMdtReadoutElement() - Trying to add MdtDetectorElement with data-collection-hash id %d outside boundaries 0-%d", __FILE__, __LINE__, (unsigned int)idh, MdtDetElMaxHash));
    }

  int stname_index = stName;
  if (stName == 49) {
      stname_index = NMdtStatType-4;       //EIS
  }
  else if (stName == 52) {
      stname_index = NMdtStatType-3;  //BIM
  } 
  else if (stName == 53) {
      stname_index = NMdtStatType-2;  //BME
  }
  else if (stName == 54) {
      stname_index = NMdtStatType-1;  //BOE
  }
  int steta_index = stEta+NMdtStEtaOffset;
  int stphi_index = stPhi-1;
  int ml_index = ml-1;

  if (stname_index<0 || stname_index >= NMdtStatType) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addMdtReadoutElement() - stNameindex out of range %d 0-%d", __FILE__, __LINE__, stname_index, NMdtStatType-1));
  }
  if (steta_index<0 || steta_index >= NMdtStatEta) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addMdtReadoutElement() - stEtaindex out of range %d 0-%d", __FILE__, __LINE__, steta_index, NMdtStatEta-1));
  }
  if (stphi_index<0 ||  stphi_index>= NMdtStatPhi) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addMdtReadoutElement() - stPhiindex out of range %d 0-%d", __FILE__, __LINE__, stphi_index, NMdtStatPhi-1));
  }
  if (ml_index < 0 || ml_index >= NMdtMultilayer) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addMdtReadoutElement() - ml_index out of range %d 0-%d", __FILE__, __LINE__, ml_index, NMdtMultilayer-1));
  }
  if (m_mdtArray[stname_index][steta_index][stphi_index][ml_index] != nullptr) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addMdtReadoutElement() - this place is taken [%d][%d][%d][%d] current id is %s stored id %s", __FILE__, __LINE__, stname_index, steta_index, stphi_index, ml_index, m_mdtIdHelper->show_to_string(id).c_str(), m_mdtIdHelper->show_to_string(m_mdtArray[stname_index][steta_index][stphi_index][ml_index]->identify()).c_str()));
  }
  m_mdtArray[stname_index][steta_index][stphi_index][ml_index] = x;
  
  m_n_mdtRE++;
}

const MdtReadoutElement* MuonDetectorManager::getMdtReadoutElement(Identifier id) const
{ 

  int stName = m_mdtIdHelper->stationName(id);
  int stEta  = m_mdtIdHelper->stationEta(id);
  int stPhi  = m_mdtIdHelper->stationPhi(id);
  int ml     = m_mdtIdHelper->multilayer(id);

  int stname_index = stName;
  if (stName == 49) stname_index = NMdtStatType-4;
  else if (stName == 52) stname_index = NMdtStatType-3;
  else if (stName == 53) stname_index = NMdtStatType-2;
  else if (stName == 54) stname_index = NMdtStatType-1;
  int steta_index = stEta+NMdtStEtaOffset;
  int stphi_index = stPhi-1;
  int ml_index = ml-1;

  return getMdtReadoutElement(stname_index,steta_index,stphi_index,ml_index);

}

void MuonDetectorManager::addCscReadoutElement (CscReadoutElement* x, Identifier id)
{
    if (id!=x->identify()) {
      throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addCscReadoutElement() - Trying to add CscReadoutElement with id %s not matching the id assigned to the CscReadoutElement %s", __FILE__, __LINE__, m_cscIdHelper->show_to_string(id).c_str(), m_cscIdHelper->show_to_string(x->identify()).c_str()));
    }
    
    // add here RE to array by hash 
    IdentifierHash Idhash = x->detectorElementHash();
    if (Idhash >= CscRElMaxHash) {
      throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addCscReadoutElement() - Trying to add CscReadoutElement with detector-element-hash id %d outside boundaries 0-%d", __FILE__, __LINE__, (unsigned int)Idhash, CscRElMaxHash));
    } else {
      if (m_cscArrayByHash[Idhash]!=nullptr) {
        throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addCscReadoutElement() - Trying to add CscReadoutElement with detector-element-hash id %d id = %s at location already taken by %s", __FILE__, __LINE__, (unsigned int)Idhash, m_cscIdHelper->show_to_string(id).c_str(), m_cscIdHelper->show_to_string(m_cscArrayByHash[Idhash]->identify()).c_str()));
      }
      m_cscArrayByHash[Idhash] = x;
    }

    //add here the CscDetectorElement and/or add this readoutElement to the DetectorElement
    IdentifierHash idh = x->detectorElementHash(); 
    if ( idh< CscDetElMaxHash ) {
	if ( !(m_cscDEArray[idh]) ) {
	    m_cscDEArray[idh] = new CscDetectorElement(0, this, m_cscIdHelper->elementID(id), idh);
	    m_n_cscDE++;
	}
	m_cscDEArray[idh]->setReadoutElement(x);
    } else {
      throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addCscReadoutElement() - Trying to add CscDetectorElement with data-collection-hash id %d outside boundaries 0-%d", __FILE__, __LINE__, (unsigned int)idh, CscDetElMaxHash));
    }


    int stName = m_cscIdHelper->stationName(id);
    int stEta  = m_cscIdHelper->stationEta(id);
    int stPhi  = m_cscIdHelper->stationPhi(id);
    int ml     = m_cscIdHelper->chamberLayer(id);

    int stname_index = stName + NCscStatTypeOff;
    int steta_index = stEta+NCscStEtaOffset;
    if (steta_index == 2) steta_index=1;
    int stphi_index = stPhi-1;
    int ml_index = ml-1;

    if (stname_index<0 || stname_index >= NCscStatType) {
      throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addCscReadoutElement() - stNameindex out of range %d 0-%d", __FILE__, __LINE__, stname_index, NCscStatType-1));
    }
    if (steta_index<0 || steta_index >= NCscStatEta) {
      throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addCscReadoutElement() - stEtaindex out of range %d 0-%d", __FILE__, __LINE__, steta_index, NCscStatEta-1));
    }
    if (stphi_index<0 ||  stphi_index>= NCscStatPhi) {
      throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addCscReadoutElement() - stPhiindex out of range %d 0-%d", __FILE__, __LINE__, stphi_index, NCscStatPhi-1));
    }
    if (ml_index < 0 || ml_index >= NCscChamberLayer) {
      throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addCscReadoutElement() - ml_index out of range %d 0-%d", __FILE__, __LINE__, ml_index, NCscChamberLayer-1));
    }
    if (m_cscArray[stname_index][steta_index][stphi_index][ml_index] != nullptr) {
      throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addCscReadoutElement() - this place is taken [%d][%d][%d][%d] current id is %s stored id %s", __FILE__, __LINE__, stname_index, steta_index, stphi_index, ml_index, m_cscIdHelper->show_to_string(id).c_str(), m_cscIdHelper->show_to_string(m_cscArray[stname_index][steta_index][stphi_index][ml_index]->identify()).c_str()));
    }
    m_cscArray[stname_index][steta_index][stphi_index][ml_index] = x;
    m_n_cscRE++;

}

const CscReadoutElement* MuonDetectorManager::getCscReadoutElement(Identifier id) const
{ 

  int stName = m_cscIdHelper->stationName(id);
  int stEta  = m_cscIdHelper->stationEta(id);
  int stPhi  = m_cscIdHelper->stationPhi(id);
  int ml     = m_cscIdHelper->chamberLayer(id);

  int stname_index = stName + NCscStatTypeOff;
  int steta_index = stEta+NCscStEtaOffset;
  if (steta_index > 1) steta_index=1;
  int stphi_index = stPhi-1;
  int ml_index = ml-1;

  return getCscReadoutElement(stname_index,steta_index,stphi_index,ml_index);

}

void MuonDetectorManager::addTgcReadoutElement (TgcReadoutElement* x, Identifier id)
{
    if (id!=x->identify()) {
      throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addTgcReadoutElement() - Trying to add TgcReadoutElement with id %s not matching the id assigned to the TgcReadoutElement %s", __FILE__, __LINE__, m_tgcIdHelper->show_to_string(id).c_str(), m_tgcIdHelper->show_to_string(x->identify()).c_str()));
    }

    // add RE to array by RE hash
    IdentifierHash Idhash = x->detectorElementHash();
    if (Idhash >= TgcRElMaxHash) {
      throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addTgcReadoutElement() - Trying to add TgcReadoutElement with detector-element-hash id %d outside boundaries 0-%d", __FILE__, __LINE__, (unsigned int)Idhash, TgcRElMaxHash));
    } else {
      if (m_tgcArrayByHash[Idhash]!=nullptr) {
        throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addTgcReadoutElement() - Trying to add TgcReadoutElement with detector-element-hash id %d id = %s at location already taken by %s", __FILE__, __LINE__, (unsigned int)Idhash, m_tgcIdHelper->show_to_string(id).c_str(), m_tgcIdHelper->show_to_string(m_tgcArrayByHash[Idhash]->identify()).c_str()));
      }
      m_tgcArrayByHash[Idhash] = x;
    }

    //add here the TgcDetectorElement and/or add this readoutElement to the DetectorElement
    IdentifierHash idh = x->collectionHash();
    if (idh< TgcDetElMaxHash) {
	if ( !(m_tgcDEArray[idh]) ) {
	    m_tgcDEArray[idh] = new TgcDetectorElement(0, this, m_tgcIdHelper->elementID(id), idh);
	    m_n_tgcDE++;
	}
	m_tgcDEArray[idh]->setReadoutElement(x);
    } else {
      throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addTgcReadoutElement() - Trying to add TgcDetectorElement with data-collection-hash id %d outside boundaries 0-%d", __FILE__, __LINE__, (unsigned int)idh, TgcDetElMaxHash));
    }
    

    int stname_index = m_tgcIdHelper->stationName(id) + NTgcStatTypeOff;
    int zi = m_tgcIdHelper->stationEta(id);
    int steta_index  = zi  + NTgcStEtaOffset;
    if (zi>0) steta_index--;  
    int stphi_index  = m_tgcIdHelper->stationPhi(id)  - 1;

    if (stname_index<0 || stname_index >= NTgcStatType) {
      throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addTgcReadoutElement() - stNameindex out of range %d 0-%d", __FILE__, __LINE__, stname_index, NTgcStatType-1));
    }
    if (steta_index<0 || steta_index >= NTgcStatEta) {
      throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addTgcReadoutElement() - stEtaindex out of range %d 0-%d", __FILE__, __LINE__, steta_index, NTgcStatEta-1));
    }
    if (stphi_index<0 ||  stphi_index>= NTgcStatPhi) {
      throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addTgcReadoutElement() - stPhiindex out of range %d 0-%d", __FILE__, __LINE__, stphi_index, NTgcStatPhi-1));
    }
    if (m_tgcArray[stname_index][steta_index][stphi_index] != nullptr) {
      throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::addTgcReadoutElement() - this place is taken [%d][%d][%d] current id is %s stored id %s", __FILE__, __LINE__, stname_index, steta_index, stphi_index, m_tgcIdHelper->show_to_string(id).c_str(), m_tgcIdHelper->show_to_string(m_tgcArray[stname_index][steta_index][stphi_index]->identify()).c_str()));
    }

    m_tgcArray[stname_index][steta_index][stphi_index] = x;

  m_n_tgcRE++;
}

const TgcReadoutElement* MuonDetectorManager::getTgcReadoutElement (Identifier id) const
{

    int stname_index = m_tgcIdHelper->stationName(id) + NTgcStatTypeOff;
    int zi = m_tgcIdHelper->stationEta(id);
    int steta_index  = zi  + NTgcStEtaOffset;
    if (zi>0) steta_index--;  
    int stphi_index  = m_tgcIdHelper->stationPhi(id)  - 1;

    return getTgcReadoutElement(stname_index,steta_index,stphi_index);
}

const MMReadoutElement* MuonDetectorManager::getMMReadoutElement(Identifier id) const
{
  return getMMRElement_fromIdFields(m_mmIdHelper->isSmall(id),m_mmIdHelper->stationEta(id),m_mmIdHelper->stationPhi(id),m_mmIdHelper->multilayer(id));
}

const sTgcReadoutElement* MuonDetectorManager::getsTgcReadoutElement(Identifier id) const
{
  return getsTgcRElement_fromIdFields(m_stgcIdHelper->isSmall(id),m_stgcIdHelper->stationEta(id),m_stgcIdHelper->stationPhi(id),m_stgcIdHelper->multilayer(id));
}

const MdtReadoutElement* MuonDetectorManager::getMdtRElement_fromIdFields(int i1,
                                                                          int i2,
                                                                          int i3,
                                                                          int i4) const
{
  int stname_index = i1;
  if (stname_index == 49) {
      stname_index = NMdtStatType-4;       //EIS
  }
  else if (stname_index == 52) {
      stname_index = NMdtStatType-3;  //BIM
  }
  else if (stname_index == 53) {
      stname_index = NMdtStatType-2;  //BME
  }
  else if (stname_index == 54) {
      stname_index = NMdtStatType-1;  //BOE
  }
  //  if (stname_index == 49) stname_index = NMdtStatType-1;
  int steta_index = i2+NMdtStEtaOffset;
  int stphi_index = i3-1;
  int ml_index = i4-1;
  
  return getMdtReadoutElement(stname_index,steta_index,stphi_index,ml_index);
}

void MuonDetectorManager::checkMdtReadoutElementIndices(int i1, int i2, int i3, int i4)const
{
  if (i1<0 || i1 >= NMdtStatType) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::getMdtReadoutElement() - stNameindex out of range %d 0-%d", __FILE__, __LINE__, i1, NMdtStatType-1));
  }
  if (i2<0 || i2 >= NMdtStatEta) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::getMdtReadoutElement() - stEtaindex out of range %d 0-%d", __FILE__, __LINE__, i2, NMdtStatEta-1));
  }
  if (i3<0 ||  i3 >= NMdtStatPhi) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::getMdtReadoutElement() - stPhiindex out of range %d 0-%d", __FILE__, __LINE__, i3, NMdtStatPhi-1));
  }
  if (i4 < 0 || i4 >= NMdtMultilayer) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::getMdtReadoutElement() - ml_index out of range %d 0-%d", __FILE__, __LINE__, i4, NMdtMultilayer-1));
  }    
}

const MdtReadoutElement* MuonDetectorManager::getMdtReadoutElement(int i1, int i2, int i3, int i4)const
{
  checkMdtReadoutElementIndices(i1, i2, i3, i4);
  return m_mdtArray[i1][i2][i3][i4];
}

MdtReadoutElement* MuonDetectorManager::getMdtReadoutElement(int i1, int i2, int i3, int i4)
{
  checkMdtReadoutElementIndices(i1, i2, i3, i4);
  return m_mdtArray[i1][i2][i3][i4];
}

const CscReadoutElement* MuonDetectorManager::getCscRElement_fromIdFields(int i1,
                                                                          int i2,
                                                                          int i3,
                                                                          int i4) const
{
  int stname_index = i1 + NCscStatTypeOff;
  int steta_index = i2+NCscStEtaOffset;
  if (steta_index > 1) steta_index=1;
  int stphi_index = i3-1;
  int ml_index = i4-1;
  
  return getCscReadoutElement(stname_index, steta_index, stphi_index, ml_index);
}

void MuonDetectorManager::checkCscReadoutElementIndices(int i1, int i2, int i3, int i4)const
{
  if (i1<0 || i1 >= NCscStatType) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::getCscReadoutElement() - stNameindex out of range %d 0-%d", __FILE__, __LINE__, i1, NCscStatType-1));
  }
  if (i2<0 || i2 >= NCscStatEta) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::getCscReadoutElement() - stEtaindex out of range %d 0-%d", __FILE__, __LINE__, i2, NCscStatEta-1));
  }
  if (i3<0 ||  i3 >= NCscStatPhi) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::getCscReadoutElement() - stPhiindex out of range %d 0-%d", __FILE__, __LINE__, i3, NCscStatPhi-1));
  }
  if (i4 < 0 || i4 >= NCscChamberLayer) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::getCscReadoutElement() - ml_index out of range %d 0-%d", __FILE__, __LINE__, i4, NCscChamberLayer-1));
  }    
}

const CscReadoutElement* MuonDetectorManager::getCscReadoutElement(int i1, int i2, int i3, int i4)const
{
  checkCscReadoutElementIndices(i1, i2, i3, i4);
  return m_cscArray[i1][i2][i3][i4];
}

CscReadoutElement* MuonDetectorManager::getCscReadoutElement(int i1, int i2, int i3, int i4)
{
  checkCscReadoutElementIndices(i1, i2, i3, i4);
  return m_cscArray[i1][i2][i3][i4];
}

const RpcReadoutElement* MuonDetectorManager::getRpcRElement_fromIdFields(int i1,
                                                                          int i2,
                                                                          int i3, int i4, int i5, int i6) const
{
      int steta_index  = i2 + NRpcStEtaOffset;
      int stphi_index  = i3 - 1;
      int dbr_index    = i4 - 1;
      int dbz_index    = i5 - 1;
      int doubletPhi   = i6;
      

      int dbr = i4;
      int dbz = i5;
      int absEta = std::abs(i2);


      // BMS 5/ |stEta|= 2 / dbR = 1 and 2 / dbZ = 3
      // BMS 6/ |stEta|= 4 / dbR = 2 / dbZ = 3
      // BMS 6/ |stEta|= 4 / dbR = 1 / dbZ = 2
      // these are the special cases where we want the rpc at doubletPhi = 2
      // to be addressed with a dbz_index=dbZ+1
      if (i1 == 3) // BMS
      {
          if (absEta == 2 && dbz == 3)
          {
              if (doubletPhi == 2) dbz_index++;
          }
          else if ( absEta == 4 &&
                    dbr == 2 &&
                    dbz == 3)
          {
              if (doubletPhi == 2) dbz_index++;
          }
          else if ( absEta == 4 &&
                    dbr == 1 &&
                    dbz == 2)
          {
              if (doubletPhi == 2) dbz_index++;
          }      
          else if ( absEta == 4 &&
                    dbr == 1 &&
                    dbz == 3)
          {
	      // not a valid case 
 	      // dbz_index = 10;
	      // cannot just exit because this dbz_index is actually used for dbPhi=2  
	       return nullptr;
          }      
      }
      
      return getRpcReadoutElement(i1, steta_index, stphi_index, dbr_index, dbz_index);
}

void MuonDetectorManager::checkRpcReadoutElementIndices(int i1, int i2, int i3, int i4, int i5)const
{
  if (i1<0 || i1 >= NRpcStatType) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::getRpcReadoutElement() - stNameindex out of range %d 0-%d", __FILE__, __LINE__, i1, NRpcStatType-1));
  }
  if (i2<0 || i2 >= NRpcStatEta) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::getRpcReadoutElement() - stEtaindex out of range %d 0-%d", __FILE__, __LINE__, i2, NRpcStatEta-1));
  }
  if (i3<0 || i3 >= NRpcStatPhi) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::getRpcReadoutElement() - stPhiindex out of range %d 0-%d", __FILE__, __LINE__, i3, NRpcStatPhi-1));
  }
  if (i4 < 0 || i4 >= NDoubletR) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::getRpcReadoutElement() - dbr_index out of range %d 0-%d", __FILE__, __LINE__, i4, NDoubletR-1));
  }
  if (i5 < 0 || i5 >= NDoubletZ) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::getRpcReadoutElement() - dbz_index out of range %d 0-%d", __FILE__, __LINE__, i5, NDoubletZ-1));
  }
}

const RpcReadoutElement* MuonDetectorManager::getRpcReadoutElement(int i1, int i2, int i3, int i4, int i5)const
{
  checkRpcReadoutElementIndices(i1, i2, i3, i4, i5);
  return m_rpcArray[i1][i2][i3][i4][i5];
}

RpcReadoutElement* MuonDetectorManager::getRpcReadoutElement(int i1, int i2, int i3, int i4, int i5)
{
  checkRpcReadoutElementIndices(i1, i2, i3, i4, i5);
  return m_rpcArray[i1][i2][i3][i4][i5];
}

const TgcReadoutElement* MuonDetectorManager::getTgcRElement_fromIdFields(int i1,
                                                                          int i2,
                                                                          int i3) const
{
    int stname_index = i1 + NTgcStatTypeOff;
    int zi = i2;
    int steta_index  = zi  + NTgcStEtaOffset;
    if (zi>0) steta_index--;  
    int stphi_index  = i3  - 1;
    
    return getTgcReadoutElement(stname_index, steta_index, stphi_index);
}

void MuonDetectorManager::checkTgcReadoutElementIndices (int i1, int i2, int i3)const
{
  if (i1<0 || i1 >= NTgcStatType) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::getTgcReadoutElement() - stNameindex out of range %d 0-%d", __FILE__, __LINE__, i1, NTgcStatType-1));
  }
  if (i2<0 || i2 >= NTgcStatEta) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::getTgcReadoutElement() - stEtaindex out of range %d 0-%d", __FILE__, __LINE__, i2, NTgcStatEta-1));
  }
  if (i3<0 ||  i3>= NTgcStatPhi) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonDetectorManager::getTgcReadoutElement() - stPhiindex out of range %d 0-%d", __FILE__, __LINE__, i3, NTgcStatPhi-1));
  }
}

const TgcReadoutElement* MuonDetectorManager::getTgcReadoutElement(int i1, int i2, int i3)const
{
  checkTgcReadoutElementIndices (i1, i2, i3);
  return m_tgcArray[i1][i2][i3];
}

TgcReadoutElement* MuonDetectorManager::getTgcReadoutElement(int i1, int i2, int i3)
{
  checkTgcReadoutElementIndices (i1, i2, i3);
  return m_tgcArray[i1][i2][i3];
}

void
MuonDetectorManager::initABlineContainers()
{
    MsgStream log(Athena::getMessageSvc(),"MGM::MuonDetectorManager");
    m_aLineContainer.clear();
    m_bLineContainer.clear();

    if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "Init A/B Line Containers - pointers are <" <<(uintptr_t)&m_aLineContainer << "> and <" << (uintptr_t)&m_bLineContainer << ">" << endmsg;

    // loop over stations to fill the A-line map at start-up
    for (std::map< std::string, MuonStation * >::const_iterator ist=m_MuonStationMap.begin();
         ist!=m_MuonStationMap.end(); ++ist)
    {
        MuonStation * ms = (*ist).second;
        int jff = ms->getPhiIndex();
        int jzz = ms->getEtaIndex();
        std::string stType = ms->getStationType();
       
        ALinePar newALine;
        newALine.setAmdbId(stType, jff, jzz, 0);
	if (ms->hasALines()) 
	{
	  newALine.setParameters(ms->getALine_tras(),ms->getALine_traz(),ms->getALine_trat(),
                                 ms->getALine_rots(),ms->getALine_rotz(),ms->getALine_rott());	
	}
	else
	{
	  if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"No starting A-lines for Station "<<stType<<" Jzz/Jff "<<jzz<<"/"<<jff<<endmsg;
	  newALine.setParameters(0.,0.,0.,0.,0.,0.);				  
	}
        newALine.isNew(true);

        Identifier id;
        //= m_mdtIdHelper->elementID(stType, jzz, jff);
        if (m_tgcIdHelper && stType.substr(0,1)=="T") 
        {
            // TGC case
            int stPhi = MuonGM::stationPhiTGC(stType,jff,jzz,geometryVersion());
            int stEta = 1; // stEta for the station is stEta for the first component chamber
            if (jzz<0) stEta = -1; // stEta for the station is stEta for the first component chamber
            id = m_tgcIdHelper->elementID(stType, stEta, stPhi);
            if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"Filling A-line container with entry for key = "<<m_tgcIdHelper->show_to_string(id)<<endmsg;
        }
        else if (m_cscIdHelper && stType.substr(0,1)=="C")
        {
            // CSC case
            id = m_cscIdHelper->elementID(stType, jzz, jff);
            if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"Filling A-line container with entry for key = "<<m_cscIdHelper->show_to_string(id)<<endmsg;
        }
        else if (m_rpcIdHelper && stType.substr(0,3)=="BML" && std::abs(jzz)==7) 
        {
            // RPC case
            id = m_rpcIdHelper->elementID(stType, jzz, jff, 1);
            if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"Filling A-line container with entry for key = "<<m_rpcIdHelper->show_to_string(id)<<endmsg;
        }
        else if (m_mdtIdHelper)
        {
            id = m_mdtIdHelper->elementID(stType, jzz, jff);
            if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"Filling A-line container with entry for key = "<<m_mdtIdHelper->show_to_string(id)<<endmsg;
        }
        m_aLineContainer.emplace(id, std::move(newALine));
        if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"<Filling A-line container with entry for key >"<<m_mdtIdHelper->show_to_string(id)<<endmsg;
    }

#ifndef SIMULATIONBASE
    if(m_stgcIdHelper && m_mmIdHelper && !(m_NSWABLinesAsciiSideA.empty())){

      ALineMapContainer writeALines;
      BLineMapContainer writeBLines;
      MuonCalib::NSWCondUtils::setNSWABLinesFromAscii(m_NSWABLinesAsciiSideA, writeALines, writeBLines, m_stgcIdHelper, m_mmIdHelper);
      for(auto it = writeALines.cbegin(); it != writeALines.cend(); ++it){
        Identifier id = it->first;
        ALinePar aline = it->second;
        m_aLineContainer.emplace(id, std::move(aline));
      }

      for(auto it = writeBLines.cbegin(); it != writeBLines.cend(); ++it){
        Identifier id = it->first;
        BLinePar bline = it->second;
        m_bLineContainer.emplace(id, std::move(bline));
      }

      if(!m_cscIdHelper && !(m_NSWABLinesAsciiSideC.empty())){
        ALineMapContainer writeALines;
        BLineMapContainer writeBLines;
        MuonCalib::NSWCondUtils::setNSWABLinesFromAscii(m_NSWABLinesAsciiSideC, writeALines, writeBLines, m_stgcIdHelper, m_mmIdHelper);

        for(auto it = writeALines.cbegin(); it != writeALines.cend(); ++it)
        {
          Identifier id = it->first;
          ALinePar aline = it->second;
          m_aLineContainer.emplace(id, std::move(aline));
        }

        for(auto it = writeBLines.cbegin(); it != writeBLines.cend(); ++it)
        {
          Identifier id = it->first;
          BLinePar bline = it->second;
          m_bLineContainer.emplace(id, std::move(bline));
        }
      }
    }
#endif

    log << MSG::INFO << "Init A/B Line Containers - done - size is respectively " << m_aLineContainer.size() << "/" << m_bLineContainer.size() << endmsg;
}


StatusCode
MuonDetectorManager::updateAlignment(const ALineMapContainer& alineData)
{
#ifdef TESTBLINES
  {
    for (std::map< std::string, MuonStation *>::const_iterator it(m_MuonStationMap.begin()), end(m_MuonStationMap.end());
	it!=end; ++it) {
      MuonStation* station = it->second;
      station->setDelta_fromAline( 0., 0., 0., 0., 0., 0.); //double tras, double traz, double trat, double rots, double rotz, double rott
      if (cacheFillingFlag()) 
      {
	station->clearCache();
	station->fillCache();
      }
      else 
      {
	station->refreshCache();
      }
    }
  }
#endif
    MsgStream log(Athena::getMessageSvc(),"MGM::MuonDetectorManager");
    if (alineData.empty()) {
	log<<MSG::WARNING<<"Empty temporary A-line container - nothing to do here"<<endmsg;
	return StatusCode::SUCCESS;
    }
    else log<<MSG::INFO<<"temporary A-line container with size = "<<alineData.size()<<endmsg;
    

    // loop over the container of the updates passed by the MuonAlignmentDbTool
    unsigned int nLines = 0;
    unsigned int nUpdates = 0;
    for (const auto& [ALineId, ALine] : alineData)
    {
        nLines++;
        std::string stType = "";
        int jff = 0;
        int jzz = 0;
        int job = 0;
        ALine.getAmdbId(stType, jff, jzz, job);
        if (!ALine.isNew())
        {            
            log<<MSG::WARNING
               <<"ALinePar with AmdbId "
               <<stType<<" "<<jzz<<" "<<jff<<" "<<job
               <<" is not new *** skipping"<<endmsg;
            continue;
        }
	if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG
					<<"ALinePar with AmdbId "
					<<stType<<" "<<jzz<<" "<<jff<<" "<<job<<" is new ID = "<<m_mdtIdHelper->show_to_string(ALineId)<<endmsg;

        
        MuonStation* thisStation = getMuonStation(stType, jzz, jff);
        if (!thisStation) {
          log<<MSG::WARNING
             <<"ALinePar with AmdbId "<<stType<<" "<<jzz<<" "<<jff<<" "<<job
             <<" *** No MuonStation found \n PLEASE CHECK FOR possible MISMATCHES between alignment constants from COOL and Geometry Layout in use"<<endmsg;
          continue;
        }

        if (job != 0) {
          // job different than 0 (standard for TGC conditions for Sept 2010 repro.)
          if (stType.substr(0,1)=="T") 
          {
            if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG
                                            <<"ALinePar with AmdbId "<<stType<<" "<<jzz<<" "<<jff<<" "<<job
                                            <<" has JOB not 0 - this is expected for TGC"<<endmsg;
          }
          else 
          {
            log<<MSG::WARNING
               <<"ALinePar with AmdbId "<<stType<<" "<<jzz<<" "<<jff<<" "<<job
               <<" has JOB not 0 - this is NOT EXPECTED yet for non TGC chambers - skipping this A-line"<<endmsg;
            continue;
          }
        }

        // record this A-line in the historical A-line container
        auto [it, flag] = m_aLineContainer.insert_or_assign (ALineId, ALine);
        ALinePar& newALine = it->second;
        if (flag)
        {
          if (log.level()<=MSG::DEBUG) log << MSG::DEBUG<< "               New entry in A-line container for Station "
                                           <<stType<<" at Jzz/Jff "<<jzz<<"/"<< jff <<" --- in the container with key "<< m_mdtIdHelper->show_to_string(ALineId)<< endmsg;
        }
        else {
          if (log.level()<=MSG::DEBUG) log << MSG::DEBUG<< "Updating extisting entry in A-line container for Station "
                                           <<stType<<" at Jzz/Jff "<<jzz<<"/"<< jff << endmsg;
        }

        if (job == 0) 
        {
          float s,z,t,ths,thz,tht;
          newALine.getParameters(s,z,t,ths,thz,tht);
          if  (    m_controlAlines%10        == 0)           tht = 0.;
          if  (int(m_controlAlines/10)%10    == 0)           thz = 0.;
          if  (int(m_controlAlines/100)%10   == 0)           ths = 0.;
          if  (int(m_controlAlines/1000)%10  == 0)           t   = 0.;
          if  (int(m_controlAlines/10000)%10 == 0)           z   = 0.;
          if  (int(m_controlAlines/100000)%10== 0)           s   = 0.;
          if  (m_controlAlines!=111111) newALine.setParameters(s,z,t,ths,thz,tht);
          if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"Setting delta transform for Station "<<stType<<" "<<jzz<<" "<<jff<<" "<<" params are = "<<s<<" "<<z<<" "<<t<<" "<<ths<<" "<<thz<<" "<<tht<<endmsg;
          thisStation->setDelta_fromAline( s,z,t,ths,thz,tht );
#ifdef TESTBLINES
          newALine.setParameters( 0., 0., 0., 0., 0., 0.);
          thisStation->setDelta_fromAline( 0., 0., 0., 0., 0., 0.);
#endif
          if (cacheFillingFlag()) 
          {
            thisStation->clearCache();
            thisStation->fillCache();
          }
          else 
          {
            thisStation->refreshCache();
          }
        }
        else
        {
          // job different than 0 (standard for TGC conditions for Sept 2010 repro.)            
          float s,z,t,ths,thz,tht;
          newALine.getParameters(s,z,t,ths,thz,tht);
          if  (    m_controlAlines%10        == 0)           tht = 0.;
          if  (int(m_controlAlines/10)%10    == 0)           thz = 0.;
          if  (int(m_controlAlines/100)%10   == 0)           ths = 0.;
          if  (int(m_controlAlines/1000)%10  == 0)           t   = 0.;
          if  (int(m_controlAlines/10000)%10 == 0)           z   = 0.;
          if  (int(m_controlAlines/100000)%10== 0)           s   = 0.;
          if  (m_controlAlines!=111111) newALine.setParameters(s,z,t,ths,thz,tht);
          if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"Setting delta transform for component "<<job<<" of Station "<<stType<<" "<<jzz<<" "<<jff<<" "<<" params are = "<<s<<" "<<z<<" "<<t<<" "<<ths<<" "<<thz<<" "<<tht<<endmsg;
          thisStation->setDelta_fromAline_forComp( job, s,z,t,ths,thz,tht );
          if (cacheFillingFlag()) {
            thisStation->getMuonReadoutElement(job)->clearCache();
            thisStation->getMuonReadoutElement(job)->fillCache();
          }
          else 
          {
            thisStation->getMuonReadoutElement(job)->refreshCache();
          }
        }
        nUpdates++;
    }
    log<<MSG::INFO<<"# of A-lines read from the ALineMapContainer in StoreGate is "<<nLines<<endmsg;
    log<<MSG::INFO<<"# of deltaTransforms updated according to A-lines         is "<<nUpdates<<endmsg;
    log<<MSG::INFO<<"# of entries in the A-lines historical container          is "<<ALineContainer()->size()<<endmsg;
    
    return StatusCode::SUCCESS;
}

StatusCode
MuonDetectorManager::updateDeformations(const BLineMapContainer& blineData)
{
#ifdef TESTBLINES
  {
    for (std::map< std::string, MuonStation *>::const_iterator it(m_MuonStationMap.begin()), end(m_MuonStationMap.end());
	it!=end; ++it) {
      MuonStation* station = it->second;
      station->clearBLineCache();
      BLinePar* BLine = new BLinePar();
      BLine->setParameters(0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.);
      station->setBline(BLine);
      if (cacheFillingFlag()) station->fillBLineCache();
    }
  }
#endif

    MsgStream log(Athena::getMessageSvc(),"MGM::MuonDetectorManager");
    log<<MSG::INFO<<"In updateDeformations()"<<endmsg;
    if (!applyMdtDeformations()){
      log<<MSG::INFO<<"Mdt deformations are not enabled - nothing to do here"<<endmsg;
      return StatusCode::SUCCESS;
    }

    if (blineData.empty())
    {
	log<<MSG::WARNING<<"Empty temporary B-line container - nothing to do here"<<endmsg;
	return StatusCode::SUCCESS;
    }
    else log<<MSG::INFO<<"temporary B-line container with size = "<<blineData.size()<<endmsg;
    

    // loop over the container of the updates passed by the MuonAlignmentDbTool
    unsigned int nLines = 0;
    unsigned int nUpdates = 0;
    for (auto [BLineId, BLine] : blineData)
    {
        nLines++;
        std::string stType = "";
        int jff = 0;
        int jzz = 0;
        int job = 0;
#ifdef TESTBLINES
	BLine.setParameters(0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.);
#endif	
	if (mdtDeformationFlag()>999999) 
	  {
	    // first reset everything 
	    BLine.setParameters(0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.);
	    // now apply user choice 
	    int choice = mdtDeformationFlag();
 	    if (int(choice%     10)>0    ) BLine.setParameters(0.,0.,0.,BLine.sp(),BLine.sn(),BLine.tw(),0.,0.,BLine.eg(),BLine.ep(),100.);
 	    if (int(choice%    100)>9    ) BLine.setParameters(0.,0.,0.,BLine.sp(),BLine.sn(),BLine.tw(),0.,0.,BLine.eg(),100.,BLine.en());
 	    if (int(choice%   1000)>99   ) BLine.setParameters(0.,0.,0.,BLine.sp(),BLine.sn(),BLine.tw(),0.,0.,100.,BLine.ep(),BLine.en());
 	    if (int(choice%  10000)>999  ) BLine.setParameters(0.,0.,0.,BLine.sp(),BLine.sn(),100.,0.,0.,BLine.eg(),BLine.ep(),BLine.en());
 	    if (int(choice% 100000)>9999 ) BLine.setParameters(0.,0.,0.,BLine.sp(),100.,BLine.tw(),0.,0.,BLine.eg(),BLine.ep(),BLine.en());
 	    if (int(choice%1000000)>99999) BLine.setParameters(0.,0.,0.,100.,BLine.sn(),BLine.tw(),0.,0.,BLine.eg(),BLine.ep(),BLine.en());
	    if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"Testing B-lines: control flag "<<choice<<" hard coding Bline = ( bz="
					    <<BLine.bz()<<" bp="
					    <<BLine.bp()<<" bn="
					    <<BLine.bn()<<" sp="
					    <<BLine.sp()<<" sn="
					    <<BLine.sn()<<" tw="
					    <<BLine.tw()<<" pg="
					    <<BLine.pg()<<" tr="
					    <<BLine.tr()<<" eg="
					    <<BLine.eg()<<" ep="
					    <<BLine.ep()<<" en="
					    <<BLine.en()<<")"<<endmsg;
	  }
        BLine.getAmdbId(stType, jff, jzz, job);
	if (stType.substr(0,1)=="T" || stType.substr(0,1)=="C" || (stType.substr(0,3)=="BML" && std::abs(jzz)==7) ) {
	  if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG
               <<"BLinePar with AmdbId "
               <<stType<<" "<<jzz<<" "<<jff<<" "<<job
               <<" is not a MDT station - skipping"<<endmsg;
            continue;
	}
	if (mdtDeformationFlag()==2 && (stType.substr(0,3)=="BEE" || stType.substr(0,1)=="E")) // MDT deformations are requested for Barrel(ASAP) only !!!!
	  {
	    if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" mdtDeformationFlag()=="<<mdtDeformationFlag()<<" stName = "<<stType.substr(0,3)<<" barrel / ec initial = "
					    <<stType.substr(0,1)<<" 	 skipping this b-line"<<endmsg;    
	    continue; // MDT deformations are requested for Barrel(ASAP) only !!!!
	  }
	if (mdtDeformationFlag()==3 && (stType.substr(0,3)!="BEE" && stType.substr(0,1)=="B")) 
	  {
	    if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" mdtDeformationFlag()=="<<mdtDeformationFlag()<<" stName = "<<stType.substr(0,3)<<" barrel / ec initial = "
					    <<stType.substr(0,1)<<" 	 skipping this b-line"<<endmsg;    
	    continue; // MDT deformations are requested for Endcap(ARAMYS) only !!!!
	  }
	if (mdtDeformationFlag()==0) {
	  if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" mdtDeformationFlag()==0 skipping this b-line"<<endmsg;
	  continue; // should never happen...
	}  
        if (!BLine.isNew())
        {            
            log<<MSG::WARNING
               <<"BLinePar with AmdbId "
               <<stType<<" "<<jzz<<" "<<jff<<" "<<job
               <<" is not new *** skipping"<<endmsg;
            continue;
        }
	if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG
	   <<"BLinePar with AmdbId "
	   <<stType<<" "<<jzz<<" "<<jff<<" "<<job<<" is new ID = "<<m_mdtIdHelper->show_to_string(BLineId)<<endmsg;
        if (job == 0) 
        {
            MuonStation* thisStation = getMuonStation(stType, jzz, jff);
            if (!thisStation) {
              log<<MSG::WARNING
                 <<"BLinePar with AmdbId "<<stType<<" "<<jzz<<" "<<jff<<" "<<job
                 <<" *** No MuonStation found \n PLEASE CHECK FOR possible MISMATCHES between alignment constants from COOL and Geometry Layout in use"<<endmsg;
              continue;
            }

            // record this B-line in the historical B-line container
            auto [it, flag] = m_bLineContainer.insert_or_assign (BLineId, BLine);
            if (flag)
            {
              if (log.level()<=MSG::DEBUG) log << MSG::DEBUG<< "               New entry in B-line container for Station "
                    <<stType<<" at Jzz/Jff "<<jzz<<"/"<< jff <<" --- in the container with key "<< m_mdtIdHelper->show_to_string(BLineId)<< endmsg;
            }
            else {
              if (log.level()<=MSG::DEBUG) log << MSG::DEBUG<< "Updating existing entry in B-line container for Station "
                    <<stType<<" at Jzz/Jff "<<jzz<<"/"<< jff << endmsg;
            }

            if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"Setting deformation parameters for Station "<<stType<<" "<<jzz<<" "<<jff<<" "<<endmsg;
            thisStation->clearBLineCache();
            thisStation->setBline(&it->second);
            if (cacheFillingFlag()) thisStation->fillBLineCache();
            nUpdates++;
        }
        else
        {            
            log<<MSG::WARNING
               <<"BLinePar with AmdbId "<<stType<<" "<<jzz<<" "<<jff<<" "<<job<<" has JOB not 0 "<<endmsg;
            return StatusCode::FAILURE;
        }
    }
    log<<MSG::INFO<<"# of B-lines read from the ALineMapContainer in StoreGate   is "<<nLines<<endmsg;
    log<<MSG::INFO<<"# of deform-Transforms updated according to B-lines         is "<<nUpdates<<endmsg;
    log<<MSG::INFO<<"# of entries in the B-lines historical container            is "<<BLineContainer()->size()<<endmsg;
    
    return StatusCode::SUCCESS; 
}

void MuonDetectorManager::storeTgcReadoutParams(std::unique_ptr<const TgcReadoutParams> x)
{
  m_TgcReadoutParamsVec.push_back(std::move(x));
}

StatusCode MuonDetectorManager::initCSCInternalAlignmentMap()
{
  MsgStream log(Athena::getMessageSvc(),"MGM::MuonDetectorManager");

  if (!m_useCscIlinesFromGM)
    {
      log<<MSG::INFO<<"Init of CSC I-Lines will be done via Conditions DB"<<endmsg;
      m_cscALineContainer.clear();

      for (std::map< std::string, MuonStation * >::const_iterator ist=m_MuonStationMap.begin();
           ist!=m_MuonStationMap.end(); ++ist)
      {
          MuonStation * ms = (*ist).second;
          std::string stType = ms->getStationType();
          if (stType.substr(0,1)!="C") continue;

          int jff = ms->getPhiIndex();
          int jzz = ms->getEtaIndex();
          int job = 3; // it's always like this for CSCs

          for (unsigned int wlay =1; wlay<5; ++wlay) 
          {
              CscInternalAlignmentPar newILine;
              newILine.setAmdbId(stType, jff, jzz, job, wlay);
              if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"No starting I-Lines or reseting them for Station "<<stType<<" Jzz/Jff/Wlay "<<jzz<<"/"<<jff<<"/"<<wlay<<endmsg;
              // there is no way to check if the RE already has parameters set - always overwriting them.
              newILine.setParameters(0.,0.,0.,0.,0.,0.);
              newILine.isNew(true);
              Identifier idp = m_cscIdHelper->parentID(ms->getMuonReadoutElement(job)->identify());
              Identifier id = m_cscIdHelper->channelID(idp, 2, wlay, 0, 1);
              if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"<Filling I-Line container with entry for key >"<<m_cscIdHelper->show_to_string(id)<<endmsg;
              m_cscALineContainer.emplace(id, newILine);
          }
      }
      log<<MSG::INFO<<"Init I-Line Container - done - size is respectively " << m_cscALineContainer.size()<<endmsg;
    }
  if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"Init CSC I-Line Containers - pointer is <"
     <<(uintptr_t)&m_cscALineContainer<<">"<<endmsg;

  log<<MSG::INFO<<"I-Line for CSC wire layers loaded (Csc Internal Alignment)"<<endmsg;
  if (m_useCscIntAlign) log<<MSG::INFO<<"According to configuration they WILL be used "<<endmsg; 
  else log<<MSG::INFO<<"According to configuration parameters they WILL BE UPDATED FROM CONDDB "<<endmsg;
  return StatusCode::SUCCESS;
}
StatusCode
MuonDetectorManager::updateCSCInternalAlignmentMap(const CscInternalAlignmentMapContainer& ilineData)
{
    MsgStream log(Athena::getMessageSvc(),"MGM::MuonDetectorManager");
    if (ilineData.empty()) {
	log<<MSG::WARNING<<"Empty temporary CSC I-line container - nothing to do here"<<endmsg;
	return StatusCode::SUCCESS;
    }
    else log<<MSG::INFO<<"temporary CSC I-line container with size = "<<ilineData.size()<<endmsg;
    

    // loop over the container of the updates passed by the MuonAlignmentDbTool
    unsigned int nLines = 0;
    unsigned int nUpdates = 0;
    for (const auto& [ILineId, ILine] : ilineData)
    {
        nLines++;
        std::string stType = "";
        int jff = 0;
        int jzz = 0;
        int job = 0;
        int jlay = 0;
        ILine.getAmdbId(stType, jff, jzz, job, jlay);
        if (!ILine.isNew())
        {            
            log<<MSG::WARNING
               <<"CscInternalAlignmentPar with AmdbId "
               <<stType<<" "<<jzz<<" "<<jff<<" "<<job <<" "<<jlay
               <<" is not new *** skipping"<<endmsg;
            continue;
        }
	if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG
					<<"CscInternalAlignmentPar with AmdbId "
					<<stType<<" "<<jzz<<" "<<jff<<" "<<job<<" "<<jlay<<" is new ID = "<<m_cscIdHelper->show_to_string(ILineId)<<endmsg;
        if (job == 3) 
        {
            MuonStation* thisStation = getMuonStation(stType, jzz, jff);
            if (!thisStation)
            {
              log<<MSG::WARNING
                 <<"CscInternalAlignmentPar with AmdbId "<<stType<<" "<<jzz<<" "<<jff<<" "<<job<<" "<<jlay
                 <<" *** No MuonStation found \n PLEASE CHECK FOR possible MISMATCHES between alignment constants from COOL and Geometry Layout in use"<<endmsg;
              continue;
            }
            
            auto [it, flag] = m_cscALineContainer.insert_or_assign (ILineId, ILine);
            if (flag) {
              if (log.level()<=MSG::DEBUG) log << MSG::DEBUG<< "               New entry in CSC I-line container for Station "
                                               <<stType<<" at Jzz/Jff/Jlay "<<jzz<<"/"<< jff<<"/"<< jlay <<" --- in the container with key "<< m_cscIdHelper->show_to_string(ILineId)<< endmsg;
            }
            else {
              if (log.level()<=MSG::DEBUG) log << MSG::DEBUG<< "Updating extisting entry in CSC I-line container for Station "
                                               <<stType<<" at Jzz/Jff/Jlay "<<jzz<<"/"<< jff <<"/"<< jlay<< endmsg;
            }

            CscInternalAlignmentPar& newILine = it->second;
            float tras, traz, trat, rots, rotz, rott;
            newILine.getParameters(tras,traz,trat,rots,rotz,rott);
            int choice = CscIlinesFlag();
            if (    choice%10        == 0)           tras = 0.;
            if (int(choice/10)%10    == 0)           rotz = 0.;
            if (int(choice/100)%10   == 0)           rots = 0.;
            if (int(choice/1000)%10  == 0)           trat   = 0.;
            if (int(choice/10000)%10 == 0)           traz   = 0.;
            if (int(choice/100000)%10== 0)           traz   = 0.;
            if (m_controlCscIlines!=111111) newILine.setParameters(tras,traz,trat,rots,rotz,rott);
            if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"Setting CSC I-Lines for Station "<<stType<<" "<<jzz<<" "<<jff<<" "<<job<<" "<<jlay<<" "<<" params are = "<<tras<<" "<<traz<<" "<<trat<<" "<<rots<<" "<<rotz<<" "<<rott<<endmsg;
            CscReadoutElement* CscRE = dynamic_cast<CscReadoutElement*> (thisStation->getMuonReadoutElement(job));
            if(!CscRE) log<<MSG::ERROR<<"The CSC I-lines container includes stations which are no CSCs! This is impossible." <<endmsg;
            else {
              CscRE->setCscInternalAlignmentPar(newILine);
            }
            if (cacheFillingFlag()) 
            {
              thisStation->clearCache();
              thisStation->fillCache();
            }
            else 
            {
              thisStation->refreshCache();
            }
            nUpdates++;

        }
        else
        {           
            log<<MSG::ERROR<<"job for CSC I-Lines= "<<job<<" is not 3 => This is not valid."<<endmsg;
        }
    }
    log<<MSG::INFO<<"# of CSC I-lines read from the ILineMapContainer in StoreGate is "<<nLines<<endmsg;
    log<<MSG::INFO<<"# of deltaTransforms updated according to A-lines             is "<<nUpdates<<endmsg;
    log<<MSG::INFO<<"# of entries in the CSC I-lines historical container          is "<<CscInternalAlignmentContainer()->size()<<endmsg;
    
    return StatusCode::SUCCESS;
}
StatusCode
MuonDetectorManager::updateAsBuiltParams(const MdtAsBuiltMapContainer& asbuiltData)
{
    MsgStream log(Athena::getMessageSvc(),"MGM::MuonDetectorManager");
    if (asbuiltData.empty()) {
	log<<MSG::WARNING<<"Empty temporary As-Built container - nothing to do here"<<endmsg;
	return StatusCode::SUCCESS;
    } else log<<MSG::INFO<<"temporary As-Built container with size = "<<asbuiltData.size()<<endmsg;

    // loop over the container of the updates passed by the MuonAlignmentDbTool
    unsigned int nLines = 0;
    unsigned int nUpdates = 0;
    for (const auto& [AsBuiltId, AsBuiltPar] : asbuiltData)
    {
        nLines++;
        std::string stType = "";
        int jff = 0;
        int jzz = 0;
        int job = 0;
        AsBuiltPar.getAmdbId(stType, jff, jzz, job);
        if (!AsBuiltPar.isNew())
        {            
            if (log.level()<=MSG::DEBUG)
               log<<MSG::DEBUG <<"MdtAsBuiltPar with AmdbId "
               <<stType<<" "<<jzz<<" "<<jff<<" "<<job
               <<" is not new *** skipping"<<endmsg;
            continue;
        }

        auto [it, flag] = m_AsBuiltParamsMap.insert_or_assign (AsBuiltId, AsBuiltPar);
        if (flag) {
          if (log.level()<=MSG::DEBUG) log << MSG::DEBUG<< "New entry in AsBuilt container for Station "
                                           <<stType<<" at Jzz/Jff "<<jzz<<"/"<< jff<<" --- in the container with key "<< m_mdtIdHelper->show_to_string(AsBuiltId)<< endmsg;
        }
        else {
          if (log.level()<=MSG::DEBUG) log << MSG::DEBUG<< "Updating extisting entry in AsBuilt container for Station "
                                           <<stType<<" at Jzz/Jff "<<jzz<<"/"<< jff << endmsg;
        }

        if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG <<"MdtAsBuiltPar with AmdbId "
                 <<stType<<" "<<jzz<<" "<<jff<<" "<<job<<" is new ID = "<<m_mdtIdHelper->show_to_string(AsBuiltId)<<endmsg;

        MuonStation* thisStation = getMuonStation(stType, jzz, jff);
        if (thisStation) {
            if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"Setting as-built parameters for Station "<<stType<<" "<<jzz<<" "<<jff<<" "<<endmsg;
            thisStation->clearBLineCache();
            thisStation->setMdtAsBuiltParams(&it->second);
            if (cacheFillingFlag()) thisStation->fillBLineCache();
            nUpdates++;
        } else {
            log<<MSG::WARNING <<"MdtAsBuiltPar with AmdbId "<<stType<<" "<<jzz<<" "<<jff<<" "<<job 
               <<" *** No MuonStation found \n PLEASE CHECK FOR possible MISMATCHES between alignment constants from COOL and Geometry Layout in use"<<endmsg;
          continue;
        }

    }
    log<<MSG::INFO<<"# of MDT As-Built read from the MdtAsBuiltMapContainer in StoreGate is "<<nLines<<endmsg;
    log<<MSG::INFO<<"# of deltaTransforms updated according to As-Built                  is "<<nUpdates<<endmsg;
    log<<MSG::INFO<<"# of entries in the MdtAsBuilt historical container                 is "<<MdtAsBuiltContainer()->size()<<endmsg;
    
    return StatusCode::SUCCESS;
}
void MuonDetectorManager::storeCscInternalAlignmentParams(const CscInternalAlignmentPar& x)
{

  MsgStream log(Athena::getMessageSvc(),"MGM::MuonDetectorManager");
  
  std::string stName="XXX";
  int jff = 0;
  int jzz = 0;
  int job = 0;
  int wlayer = 0;
  x.getAmdbId(stName, jff, jzz, job, wlayer);
  // chamberLayer is always 2 => job is always 3 
  int chamberLayer = 2;
  if (job != 3) log<<MSG::WARNING<<"job = "<<job<<" is not 3 => chamberLayer should be 1 - not existing ! setting 2"<<endmsg;
  Identifier id = m_cscIdHelper->channelID(stName, jzz, jff, chamberLayer, wlayer, 0, 1);
  
  m_cscALineContainer.emplace(id, x);
  if (log.level()<=MSG::DEBUG) {
    log<<MSG::DEBUG<<"Adding Aline for CSC wire layer: "<<m_cscIdHelper->show_to_string(id)<<endmsg; 
    log<<MSG::DEBUG<<"CscInternalAlignmentMapContainer has currently size "<<m_cscALineContainer.size()<<endmsg;
  }
}

void MuonDetectorManager::storeMdtAsBuiltParams(const MdtAsBuiltPar& params) {
  MsgStream log(Athena::getMessageSvc(),"MGM::MuonDetectorManager");

  std::string stName="XXX";
  int jff = 0;
  int jzz = 0;
  int job = 0;
  params.getAmdbId(stName, jff, jzz, job);
  Identifier id = mdtIdHelper()->elementID(stName, jzz, jff);
  if (!id.is_valid()) {
    log << MSG::ERROR << "Invalid MDT identifiers: sta=" << stName << " eta=" << jzz << " phi=" << jff << endmsg;
    return;
  }

  if (m_AsBuiltParamsMap.insert_or_assign (id, params).second)
  {
    if (log.level()<=MSG::DEBUG) log << MSG::DEBUG<< "New entry in AsBuilt container for Station "
                    <<stName<<" at Jzz/Jff "<<jzz<<"/"<< jff<<" --- in the container with key "<< m_mdtIdHelper->show_to_string(id)<< endmsg;
  }
  else{
    if (log.level()<=MSG::DEBUG) log << MSG::DEBUG<< "Updating extisting entry in AsBuilt container for Station "
						 <<stName<<" at Jzz/Jff "<<jzz<<"/"<< jff << endmsg;
  }

  return;
}

const MdtAsBuiltPar* MuonDetectorManager::getMdtAsBuiltParams(Identifier id) const {
  if (!MdtAsBuiltContainer()) {
    MsgStream log(Athena::getMessageSvc(),"MGM::MuonDetectorManager");
    log << MSG::DEBUG << "No Mdt AsBuilt parameter container available" << endmsg;
    return nullptr;
  }
  ciMdtAsBuiltMap iter = m_AsBuiltParamsMap.find(id);
  if (iter == m_AsBuiltParamsMap.end()) {
    MsgStream log(Athena::getMessageSvc(),"MGM::MuonDetectorManager");
    log << MSG::DEBUG << "No Mdt AsBuilt parameters for station " << id.getString() 
      << " sta=" << mdtIdHelper()->stationNameString(mdtIdHelper()->stationName(id))
      << " eta=" << mdtIdHelper()->stationEta(id)
      << " phi=" << mdtIdHelper()->stationPhi(id)
      << endmsg;
    return nullptr;
  }
  return &iter->second;
}

const MdtReadoutElement* MuonDetectorManager::getMdtReadoutElement(IdentifierHash id) const {
#ifndef NDEBUG
  if ( id >= MdtRElMaxHash) {
    MsgStream log(Athena::getMessageSvc(),"MGM::MuonDetectorManager");
    log << MSG::WARNING << " try to getMdtReadoutElement with hashId " << (unsigned int)id << " outside range 0-" << MdtRElMaxHash-1 << endmsg;
    return nullptr;
  }
#endif 
  return m_mdtArrayByHash[id];
}

const RpcReadoutElement* MuonDetectorManager::getRpcReadoutElement(IdentifierHash id) const {
#ifndef NDEBUG
  if (id >= RpcRElMaxHash) {
    MsgStream log(Athena::getMessageSvc(),"MGM::MuonDetectorManager");
    log << MSG::WARNING << " try to getRpcReadoutElement with hashId " << (unsigned int)id << " outside range 0-" << RpcRElMaxHash-1 << endmsg;
    return nullptr;
  }
#endif 
  return  m_rpcArrayByHash[id];
}

const TgcReadoutElement* MuonDetectorManager::getTgcReadoutElement(IdentifierHash id) const {
#ifndef NDEBUG
  if (id >= TgcRElMaxHash) {
    MsgStream log(Athena::getMessageSvc(),"MGM::MuonDetectorManager");
    log << MSG::WARNING << " try to getTgcReadoutElement with hashId " << (unsigned int)id << " outside range 0-" << TgcRElMaxHash-1 << endmsg;
    return nullptr;
  }
#endif 
 return m_tgcArrayByHash[id];
}

const CscReadoutElement* MuonDetectorManager::getCscReadoutElement(IdentifierHash id) const {
#ifndef NDEBUG
  if (id >= CscRElMaxHash) {
    MsgStream log(Athena::getMessageSvc(),"MGM::MuonDetectorManager");
    log << MSG::WARNING << " try to getCscReadoutElement with hashId " << (unsigned int)id << " outside range 0-" << CscRElMaxHash-1 << endmsg;
    return nullptr;
  }
#endif
  return m_cscArrayByHash[id];
}
    
const MdtDetectorElement* MuonDetectorManager::getMdtDetectorElement(IdentifierHash id) const {
#ifndef NDEBUG
  if ( id >= MdtDetElMaxHash) {
    MsgStream log(Athena::getMessageSvc(),"MGM::MuonDetectorManager");
    log << MSG::WARNING << " try to getMdtDetectorElement with hashId " << (unsigned int)id << " outside range 0-" << MdtDetElMaxHash-1 << endmsg;
    return nullptr;
  }
#endif 
  return m_mdtDEArray[id];
}

const TgcDetectorElement* MuonDetectorManager::getTgcDetectorElement(IdentifierHash id) const {
#ifndef NDEBUG
  if ( id >= TgcDetElMaxHash) {
    MsgStream log(Athena::getMessageSvc(),"MGM::MuonDetectorManager");
    log << MSG::WARNING << " try to getTgcDetectorElement with hashId " << (unsigned int)id << " outside range 0-" << TgcDetElMaxHash-1 << endmsg;
    return nullptr;
  }
#endif 
  return m_tgcDEArray[id];
}

const CscDetectorElement* MuonDetectorManager::getCscDetectorElement(IdentifierHash id) const {
#ifndef NDEBUG
  if ( id >= CscDetElMaxHash) {
    MsgStream log(Athena::getMessageSvc(),"MGM::MuonDetectorManager");
    log << MSG::WARNING << " try to getCscDetectorElement with hashId " << (unsigned int)id << " outside range 0-" << CscDetElMaxHash-1 << endmsg;
    return nullptr;
  }
#endif 
  return m_cscDEArray[id];
}

const RpcDetectorElement* MuonDetectorManager::getRpcDetectorElement(IdentifierHash id) const {
#ifndef NDEBUG
  if ( id >= RpcDetElMaxHash) {
    MsgStream log(Athena::getMessageSvc(),"MGM::MuonDetectorManager");
    log << MSG::WARNING<<" try to getRpcDetectorElement with hashId " << (unsigned int)id << " outside range 0-" << RpcDetElMaxHash-1 << endmsg;
    return nullptr;
  }
#endif
  return m_rpcDEArray[id];
}

unsigned int MuonDetectorManager::rpcStationTypeIdx(const int stationName) const {
  if (stationName==m_rpcIdHelper->stationNameIndex("BML")) return RpcStatType::BML;
  else if (stationName==m_rpcIdHelper->stationNameIndex("BMS")) return RpcStatType::BMS;
  else if (stationName==m_rpcIdHelper->stationNameIndex("BOL")) return RpcStatType::BOL;
  else if (stationName==m_rpcIdHelper->stationNameIndex("BOS")) return RpcStatType::BOS;
  else if (stationName==m_rpcIdHelper->stationNameIndex("BMF")) return RpcStatType::BMF;
  else if (stationName==m_rpcIdHelper->stationNameIndex("BOF")) return RpcStatType::BOF;
  else if (stationName==m_rpcIdHelper->stationNameIndex("BOG")) return RpcStatType::BOG;
  else if (stationName==m_rpcIdHelper->stationNameIndex("BME")) return RpcStatType::BME;
  else if (stationName==m_rpcIdHelper->stationNameIndex("BIR")) return RpcStatType::BIR;
  else if (stationName==m_rpcIdHelper->stationNameIndex("BIM")) return RpcStatType::BIM;
  else if (stationName==m_rpcIdHelper->stationNameIndex("BIL")) return RpcStatType::BIL;
  else if (stationName==m_rpcIdHelper->stationNameIndex("BIS")) return RpcStatType::BIS;
  return RpcStatType::UNKNOWN;
}

int MuonDetectorManager::rpcStationName(const int stationIndex) const {
  if (stationIndex==RpcStatType::BML) return m_rpcIdHelper->stationNameIndex("BML");
  else if (stationIndex==RpcStatType::BMS) return m_rpcIdHelper->stationNameIndex("BMS");
  else if (stationIndex==RpcStatType::BOL) return m_rpcIdHelper->stationNameIndex("BOL");
  else if (stationIndex==RpcStatType::BOS) return m_rpcIdHelper->stationNameIndex("BOS");
  else if (stationIndex==RpcStatType::BMF) return m_rpcIdHelper->stationNameIndex("BMF");
  else if (stationIndex==RpcStatType::BOF) return m_rpcIdHelper->stationNameIndex("BOF");
  else if (stationIndex==RpcStatType::BOG) return m_rpcIdHelper->stationNameIndex("BOG");
  else if (stationIndex==RpcStatType::BME) return m_rpcIdHelper->stationNameIndex("BME");
  else if (stationIndex==RpcStatType::BIR) return m_rpcIdHelper->stationNameIndex("BIR");
  else if (stationIndex==RpcStatType::BIM) return m_rpcIdHelper->stationNameIndex("BIM");
  else if (stationIndex==RpcStatType::BIL) return m_rpcIdHelper->stationNameIndex("BIL");
  else if (stationIndex==RpcStatType::BIS) return m_rpcIdHelper->stationNameIndex("BIS");
  return -1;
}

} // namespace MuonGM
