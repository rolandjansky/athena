/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonReadoutGeometry/MuonDetectorManager.h"
//
#include "AthenaKernel/getMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"
//
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
//
#include "MuonReadoutGeometry/MdtDetectorElement.h"
#include "MuonReadoutGeometry/RpcDetectorElement.h"
#include "MuonReadoutGeometry/TgcDetectorElement.h"
#include "MuonReadoutGeometry/CscDetectorElement.h"
//
#include "MuonReadoutGeometry/MuonStation.h"
//
#include "MuonReadoutGeometry/GlobalUtilities.h"
//
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "MuonIdHelpers/MdtIdHelper.h"
//
#include "MuonAlignmentData/ALinePar.h"
#include "MuonAlignmentData/BLinePar.h"
#include "MuonAlignmentData/CscInternalAlignmentPar.h"

/////#define controlAlines 111111

//#define TESTBLINES

namespace MuonGM {

MuonDetectorManager::MuonDetectorManager() {

  for (unsigned int i=0; i<NRpcStatType; i++)
    for (unsigned int j=0; j<NRpcStatEta; j++)
      for (unsigned int k=0; k<NRpcStatPhi; k++)
        for (unsigned int l=0; l<NDoubletR; l++)
          for (unsigned int h=0; h<NDoubletZ; h++)
              m_rpcArray[i][j][k][l][h] = NULL;
  for (unsigned int i=0; i<NMdtStatType; i++)
    for (unsigned int j=0; j<NMdtStatEta; j++)
      for (unsigned int k=0; k<NMdtStatPhi; k++)
        for (unsigned int l=0; l<NMdtMultilayer; l++)
                   m_mdtArray[i][j][k][l] = NULL;
  for (unsigned int i=0; i<NCscStatType; i++)
    for (unsigned int j=0; j<NCscStatEta; j++)
      for (unsigned int k=0; k<NCscStatPhi; k++)
        for (unsigned int l=0; l<NCscChamberLayer; l++)
                   m_cscArray[i][j][k][l] = NULL;
  for (unsigned int i=0; i<NTgcStatType; i++)
    for (unsigned int j=0; j<NTgcStatEta; j++)
      for (unsigned int k=0; k<NTgcStatPhi; k++)
                 m_tgcArray[i][j][k] = NULL;

  for (unsigned int i=0; i<NsTgStatType; i++)
    for (unsigned int j=0; j<NsTgStatEta; j++)
      for (unsigned int k=0; k<NsTgStatPhi; k++)
        for (unsigned int l=0; l<NsTgChamberLayer; l++)
	  m_stgArray[i][j][k][l] = NULL;

  for (unsigned int i=0; i<NMMcStatType; i++)
    for (unsigned int j=0; j<NMMcStatEta; j++)
      for (unsigned int k=0; k<NMMcStatPhi; k++)
        for (unsigned int l=0; l<NMMcChamberLayer; l++)
	  m_mmcArray[i][j][k][l] = NULL;


  for (unsigned int i=0; i<MdtRElMaxHash; ++i)
      m_mdtArrayByHash[i] = NULL;
  for (unsigned int i=0; i<CscRElMaxHash; ++i)
      m_cscArrayByHash[i] = NULL;
  for (unsigned int i=0; i<RpcRElMaxHash; ++i)
      m_rpcArrayByHash[i] = NULL;
  for (unsigned int i=0; i<TgcRElMaxHash; ++i)
      m_tgcArrayByHash[i] = NULL;

     for (unsigned int i=0; i<TgcDetElMaxHash; ++i)
         m_tgcDEArray[i] = NULL;
  for (unsigned int i=0; i<MdtDetElMaxHash; ++i)
      m_mdtDEArray[i] = NULL;
   for (unsigned int i=0; i<CscDetElMaxHash; ++i)
        m_cscDEArray[i] = NULL;
     for (unsigned int i=0; i<RpcDetElMaxHash; ++i)
        m_rpcDEArray[i] = NULL;

  m_n_mdtDE = m_n_cscDE = m_n_tgcDE = m_n_rpcDE = 0;
  m_n_mdtRE = m_n_cscRE = m_n_tgcRE = m_n_rpcRE = 0;
  m_aLineContainer = 0;
  m_bLineContainer = 0;
  m_cscALineContainer = 0;
  m_AsBuiltParamsMap = 0;
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

    //!< init here the pointer to the message svc 
  m_msgSvc = Athena::getMessageSvc();  
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
            delete m_rpcArray[i][j][k][l][h]; m_rpcArray[i][j][k][l][h] = 0;
          }
  for (unsigned int i=0; i<NMdtStatType; ++i)
    for (unsigned int j=0; j<NMdtStatEta; ++j)
      for (unsigned int k=0; k<NMdtStatPhi; ++k)
        for (unsigned int l=0; l<NMdtMultilayer; ++l) {
          delete m_mdtArray[i][j][k][l]; m_mdtArray[i][j][k][l] = 0;
        }
  for (unsigned int i=0; i<NTgcStatType; ++i)
    for (unsigned int j=0; j<NTgcStatEta; ++j)
      for (unsigned int k=0; k<NTgcStatPhi; ++k) {
        delete m_tgcArray[i][j][k]; m_tgcArray[i][j][k] = 0;
      }
  for (unsigned int i=0; i<NCscStatType; ++i)
    for (unsigned int j=0; j<NCscStatEta; ++j)
      for (unsigned int k=0; k<NCscStatPhi; ++k)
        for (unsigned int l=0; l<NCscChamberLayer; ++l) {
          delete m_cscArray[i][j][k][l]; m_cscArray[i][j][k][l] = 0;
        }

  for (unsigned int i=0; i<MdtDetElMaxHash; ++i) {
    delete m_mdtDEArray[i]; m_mdtDEArray[i] = 0;
  }
  for (unsigned int i=0; i<RpcDetElMaxHash; ++i) {
    delete m_rpcDEArray[i]; m_rpcDEArray[i] = 0;
  }
  for (unsigned int i=0; i<CscDetElMaxHash; ++i) {
    delete m_cscDEArray[i]; m_cscDEArray[i] = 0;
  }
  for (unsigned int i=0; i<TgcDetElMaxHash; ++i) {
    delete m_tgcDEArray[i]; m_tgcDEArray[i] = 0;
  }
  for (std::map< std::string, MuonStation * >::iterator i =
           m_MuonStationMap.begin(); i != m_MuonStationMap.end(); ++i)
  {
    delete i->second; i->second = 0;
  }

  for (std::vector<TgcReadoutParams*>::iterator i=m_TgcReadoutParamsVec.begin();
         i!=m_TgcReadoutParamsVec.end();++i)
  {
    delete (*i); *i = 0;
  }  

  if (0 != m_cscALineContainer) 
  {
    for (auto elem: *m_cscALineContainer )
      {
	delete elem.second; elem.second = 0;
      }
    delete m_cscALineContainer; m_cscALineContainer = 0;
  }

  if (0 != m_AsBuiltParamsMap) {
    for (auto elem: *m_AsBuiltParamsMap )
      {
	delete elem.second; elem.second = 0;
      }
    delete m_AsBuiltParamsMap; m_AsBuiltParamsMap = 0;
  }

  if (0 != m_aLineContainer)
  {
    for (auto elem: *m_aLineContainer )
      {
	delete elem.second; elem.second = 0;
      }
  }
  if (0 != m_bLineContainer)
  {
    for (auto elem: *m_bLineContainer )
      {
	delete elem.second; elem.second = 0;
      }
  }

}


void MuonDetectorManager::clearCache() const
{
  clearMdtCache();
  clearRpcCache();
  clearTgcCache();
  clearCscCache();    
}

void MuonDetectorManager::refreshCache() const
{
  refreshMdtCache();
  refreshRpcCache();
  refreshTgcCache();
  refreshCscCache();    
}
void MuonDetectorManager::refreshMdtCache() const
{
  clearMdtCache(); // no need to fill; it will happen on demand
}
void MuonDetectorManager::refreshRpcCache() const
{
  clearRpcCache(); 
  fillRpcCache(); 
}
void MuonDetectorManager::refreshTgcCache() const
{
  clearTgcCache(); 
  fillTgcCache(); 
}
void MuonDetectorManager::refreshCscCache() const
{
  clearCscCache(); 
  fillCscCache(); 
}
void MuonDetectorManager::refreshMMCache() const
{
  clearMMCache(); 
  fillMMCache(); 
}
void MuonDetectorManager::refreshsTgcCache() const
{
  clearsTgcCache(); 
  fillsTgcCache(); 
}

   
void MuonDetectorManager::clearMdtCache() const
{
    for (unsigned int i=0; i<NMdtStatType; ++i)
      for (unsigned int j=0; j<NMdtStatEta; ++j)
        for (unsigned int k=0; k<NMdtStatPhi; ++k)
          for (unsigned int l=0; l<NMdtMultilayer; ++l) {
              if (m_mdtArray[i][j][k][l]) m_mdtArray[i][j][k][l]->clearCache();
          }
}   
void MuonDetectorManager::clearRpcCache() const
{
    for (unsigned int i=0; i<NRpcStatType; ++i)
      for (unsigned int j=0; j<NRpcStatEta; ++j)
        for (unsigned int k=0; k<NRpcStatPhi; ++k)
          for (unsigned int l=0; l<NDoubletR; ++l)
            for (unsigned int h=0; h<NDoubletZ; ++h) {
              if( m_rpcArray[i][j][k][l][h] ) m_rpcArray[i][j][k][l][h]->clearCache() ;
            }
}   
void MuonDetectorManager::clearTgcCache() const
{
    for (unsigned int i=0; i<NTgcStatType; ++i)
      for (unsigned int j=0; j<NTgcStatEta; ++j)
        for (unsigned int k=0; k<NTgcStatPhi; ++k) {
          if ( m_tgcArray[i][j][k] ) m_tgcArray[i][j][k]->clearCache();
        }
}   
void MuonDetectorManager::clearCscCache() const
{
  if (nCscRE()<1) return;
    for (unsigned int i=0; i<NCscStatType; ++i)
      for (unsigned int j=0; j<NCscStatEta; ++j)
        for (unsigned int k=0; k<NCscStatPhi; ++k)
          for (unsigned int l=0; l<NCscChamberLayer; ++l) {
            if( m_cscArray[i][j][k][l] ) m_cscArray[i][j][k][l]->clearCache();
          }
}   
void MuonDetectorManager::clearMMCache() const
{
  if (nMMRE()<1) return;
    for (unsigned int i=0; i<NMMcStatType; ++i)
      for (unsigned int j=0; j<NMMcStatEta; ++j)
        for (unsigned int k=0; k<NMMcStatPhi; ++k)
          for (unsigned int l=0; l<NMMcChamberLayer; ++l) {
              if (m_mmcArray[i][j][k][l]) m_mmcArray[i][j][k][l]->clearCache();
          }
}   
void MuonDetectorManager::clearsTgcCache() const
{
  if (nsTgcRE()<1) return;
    for (unsigned int i=0; i<NsTgStatType; ++i)
      for (unsigned int j=0; j<NsTgStatEta; ++j)
        for (unsigned int k=0; k<NsTgStatPhi; ++k)
          for (unsigned int l=0; l<NsTgChamberLayer; ++l) {
              if (m_stgArray[i][j][k][l]) m_stgArray[i][j][k][l]->clearCache();
          }
}   
void MuonDetectorManager::fillMMCache() const
{
  if (nMMRE()<1) return;
    for (unsigned int i=0; i<NMMcStatType; ++i)
      for (unsigned int j=0; j<NMMcStatEta; ++j)
        for (unsigned int k=0; k<NMMcStatPhi; ++k)
          for (unsigned int l=0; l<NMMcChamberLayer; ++l) {
              if (m_mmcArray[i][j][k][l]) m_mmcArray[i][j][k][l]->fillCache();
          }
}   
void MuonDetectorManager::fillsTgcCache() const
{
  if (nsTgcRE()<1) return;
    for (unsigned int i=0; i<NsTgStatType; ++i)
      for (unsigned int j=0; j<NsTgStatEta; ++j)
        for (unsigned int k=0; k<NsTgStatPhi; ++k)
          for (unsigned int l=0; l<NsTgChamberLayer; ++l) {
              if (m_stgArray[i][j][k][l]) m_stgArray[i][j][k][l]->fillCache();
          }
}   
void MuonDetectorManager::fillCache() const
{
    MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
    log<<MSG::INFO<<"Filling cache"<<endreq;
    fillMdtCache();
    fillRpcCache();
    fillTgcCache();
    fillCscCache();    
}   
void MuonDetectorManager::fillMdtCache() const
{
    for (unsigned int i=0; i<NMdtStatType; ++i)
      for (unsigned int j=0; j<NMdtStatEta; ++j)
        for (unsigned int k=0; k<NMdtStatPhi; ++k)
          for (unsigned int l=0; l<NMdtMultilayer; ++l) {
              if (m_mdtArray[i][j][k][l]) m_mdtArray[i][j][k][l]->fillCache();
          }
}   
void MuonDetectorManager::fillRpcCache() const
{
    for (unsigned int i=0; i<NRpcStatType; ++i)
      for (unsigned int j=0; j<NRpcStatEta; ++j)
        for (unsigned int k=0; k<NRpcStatPhi; ++k)
          for (unsigned int l=0; l<NDoubletR; ++l)
            for (unsigned int h=0; h<NDoubletZ; ++h) {
              if( m_rpcArray[i][j][k][l][h] ) m_rpcArray[i][j][k][l][h]->fillCache() ;
            }
}   
void MuonDetectorManager::fillTgcCache() const
{
    for (unsigned int i=0; i<NTgcStatType; ++i)
      for (unsigned int j=0; j<NTgcStatEta; ++j)
        for (unsigned int k=0; k<NTgcStatPhi; ++k) {
          if ( m_tgcArray[i][j][k] ) m_tgcArray[i][j][k]->fillCache();
        }
}   
void MuonDetectorManager::fillCscCache() const
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
    //std::cerr<<" adding MuonStation with key "<<key<<std::endl;
    m_MuonStationMap[key] = mst;
    //std::cerr<<" pointer to a MuonStation saved in the map "<<mst<<std::endl;
}

std::string
MuonDetectorManager::muonStationKey(std::string stName, int statEtaIndex, int statPhiIndex) const
{
    std::string key;
    if (statEtaIndex<0)
        key = stName.substr(0,3)
              +"_C_zi"+MuonGM::buildString(abs(statEtaIndex),2)
              +"fi"+MuonGM::buildString(statPhiIndex,2);
    else
        key = stName.substr(0,3)
              +"_A_zi"+MuonGM::buildString(abs(statEtaIndex),2)
              +"fi"+MuonGM::buildString(statPhiIndex,2);
    return key;
}
        
MuonStation*
MuonDetectorManager::getMuonStation(std::string stName, int stEtaIndex, int stPhiIndex) const 
{
    
    std::string key = muonStationKey(stName, stEtaIndex, stPhiIndex);

    //std::cerr<<"MuonManager:: retrieving MuonStation with key "<<key<<std::endl;
    std::map< std::string, MuonStation *>::const_iterator it = m_MuonStationMap.find(key);
    if ( it != m_MuonStationMap.end()) return (*it).second;
    else return NULL;
}

// MuonStation*
// MuonDetectorManager::getLastRecordedMuonStation () const
// {
//     std::vector<MuonStation *>::const_iterator  ic = m_MuonStationVec.end();
//     ic--;
//     //std::cerr<<" Last MuonStation has key "<<(*ic)->getKey()<<" pointer to it is "<<(*ic)<<std::endl;
//     return *ic;
// }



void MuonDetectorManager::addRpcReadoutElement (const RpcReadoutElement* x, Identifier id)
{
    // check if RE has id as identity 
    if (id!=x->identify()) 
    {
	MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
	log<<MSG::ERROR<<"Trying to add RpcReadoutElement with id "
	   <<m_rpcIdHelper->show_to_string(id)
	   <<" not matching the id assigned to the RpcReadoutElement "
	   <<m_rpcIdHelper->show_to_string(x->identify())<<endreq;
	throw;
	return;
    }
    
    // add RE to map by RE hash 
    IdentifierHash Idhash= x->detectorElementHash();
    if (Idhash >= RpcRElMaxHash)
    {  
	MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
	log<<MSG::ERROR<<"Trying to add RpcReadoutElement with detector-element-hash id "
	   <<(unsigned int)Idhash
	   <<" outside boundaries 0-"<<RpcRElMaxHash<<endreq;	
	throw;
	return;
    }
    else
    {
	if (m_rpcArrayByHash[Idhash]!=0) 
	{
	    MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
	    log<<MSG::ERROR<<"Trying to add RpcReadoutElement with detector-element-hash id "
	       <<(unsigned int)Idhash<<" id = "<<m_rpcIdHelper->show_to_string(id)
	       <<" at location already taken by "
	       <<m_rpcIdHelper->show_to_string(m_rpcArrayByHash[Idhash]->identify())
	       <<endreq;
	    throw;
	    return;
	}
	m_rpcArrayByHash[Idhash] = x;
    }
    
    int stname_indexId = m_rpcIdHelper->stationName(id);
    int stname_index = stname_indexId + NRpcStatTypeOff;
    if (stname_indexId == 53) // BME 
    {
	stname_index = NRpcStatType-2;
    }
    else if (stname_indexId == 54) // BOE 
    {
	stname_index = NRpcStatType-1;
    }

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
      if (abs(m_rpcIdHelper->stationEta(id)) == 2 &&
          m_rpcIdHelper->doubletZ(id) == 3)
      {
          if (doubletPhi == 2) dbz_index++;
      }
      else if (abs(m_rpcIdHelper->stationEta(id)) == 4 &&
               m_rpcIdHelper->doubletR(id) == 2 &&
               m_rpcIdHelper->doubletZ(id) == 3)
      {
          if (doubletPhi == 2) dbz_index++;
      }
      else if (abs(m_rpcIdHelper->stationEta(id)) == 4 &&
               m_rpcIdHelper->doubletR(id) == 1 &&
               m_rpcIdHelper->doubletZ(id) == 2)
      {
          if (doubletPhi == 2) dbz_index++;
      }      
  }
  //std::cerr<<" using indices "<<stname_index<<" "<<steta_index<<" "<<stphi_index<<" "<<dbr_index<<" "<<dbz_index<<std::endl;
  
  
  if (stname_index<0 || stname_index >= NRpcStatType) 
  {
      std::cerr<<"ERROR MuonDetectorManager::addRpcReadoutElement stNameindex out of range "
               <<stname_index<<" 0-"<<NRpcStatType-1<<std::endl;
      throw;
  }
  if (steta_index<0 || steta_index >= NRpcStatEta) 
  {
      std::cerr<<"ERROR MuonDetectorManager::addRpcReadoutElement stEtaindex out of range "
               <<steta_index<<" 0-"<<NRpcStatEta-1<<std::endl;
      throw;
  }
  if (stphi_index<0 ||  stphi_index>= NRpcStatPhi) 
  {
      std::cerr<<"ERROR MuonDetectorManager::addRpcReadoutElement stPhiindex out of range "
               <<stphi_index<<" 0-"<<NRpcStatPhi-1<<std::endl;
      throw;
  }
  if (dbr_index < 0 || dbr_index >= NDoubletR)
  {
      std::cerr<<"ERROR MuonDetectorManager::addRpcReadoutElement dbr_index out of range "
               <<dbr_index<<" 0-" <<NDoubletR-1<<std::endl;
      throw;
  }
  if (dbz_index < 0 || dbz_index >= NDoubletZ)
  {
      std::cerr<<"ERROR MuonDetectorManager::addRpcReadoutElement dbz_index out of range "
               <<dbz_index<<" 0-" <<NDoubletZ-1<<std::endl;
      throw;
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
    else
    {
	MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
	log<<MSG::ERROR<<"Trying to add RpcDetectorElement with data-collection-hash id "
	   <<(unsigned int)idh
	   <<" outside boundaries 0-"<<RpcDetElMaxHash<<endreq;	
	throw;
	return;
    }


  if (m_rpcArray[stname_index][steta_index][stphi_index][dbr_index][dbz_index] != NULL)
  {
      std::cerr<<"addRpcReadoutElement - this place is taken ["
               <<stname_index<<"]["
               <<steta_index<< "]["
               <<stphi_index<< "]["
               <<dbr_index<<   "]["
               <<dbz_index<<   "] current id is "<<m_rpcIdHelper->show_to_string(id)<<" stored id "
               <<m_rpcIdHelper->show_to_string(
                   m_rpcArray[stname_index][steta_index][stphi_index][dbr_index][dbz_index]->identify()
                   )<<std::endl;
      return;
  }
  m_rpcArray[stname_index][steta_index][stphi_index][dbr_index][dbz_index] = x;
  //std::cerr<<"\n Manager adding RpcReadoutElement with id "<<m_rpcIdHelper->show_to_string(id)
  //         <<" at "<<stname_index<<" "<<steta_index<<" "<<stphi_index<<" "<<dbr_index<<" "<<dbz_index<<std::endl;
  m_n_rpcRE++;
  //std::cerr<<"Manager m_n_rpcRE = "<<m_n_rpcRE<<std::endl;

}

const RpcReadoutElement* MuonDetectorManager::getRpcReadoutElement (const Identifier id) const
{
    //    std::cout<<"MuonDetectorManager::getRpcReadoutElement for id fields  = "<<stname_index <<" "<<steta_index <<" "<<stphi_index<<" "<<dbr_index <<" "<<dbz_index<<" "<<doubletPhi<<std::endl;
    
    int stname_indexId = m_rpcIdHelper->stationName(id);
    int stname_index = stname_indexId + NRpcStatTypeOff;
    if (stname_indexId == 53) // BME 
    {
	stname_index = NRpcStatType-2;
    }
    else if (stname_indexId == 54) // BOE 
    {
	stname_index = NRpcStatType-1;
    }
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
  if (m_rpcIdHelper->stationNameString(m_rpcIdHelper->stationName(id)) == "BMS")
  {
      if (abs(m_rpcIdHelper->stationEta(id)) == 2 &&
          m_rpcIdHelper->doubletZ(id) == 3)
      {
          if (doubletPhi == 2) dbz_index++;
      }
      else if (abs(m_rpcIdHelper->stationEta(id)) == 4 &&
               m_rpcIdHelper->doubletR(id) == 2 &&
               m_rpcIdHelper->doubletZ(id) == 3)
      {
          if (doubletPhi == 2) dbz_index++;
      }
      else if (abs(m_rpcIdHelper->stationEta(id)) == 4 &&
               m_rpcIdHelper->doubletR(id) == 1 &&
               m_rpcIdHelper->doubletZ(id) == 2)
      {
          if (doubletPhi == 2) dbz_index++;
      }      
  }
  //    std::cout<<"MuonDetectorManager::getRpcReadoutElement for id fields  = "<<stname_index <<" "<<steta_index <<" "<<stphi_index<<" "<<dbr_index <<" "<<dbz_index<<std::endl;
  
  return getRpcReadoutElement(stname_index,steta_index,stphi_index,dbr_index,dbz_index);
}

const MuonClusterReadoutElement* MuonDetectorManager::getMuonClusterReadoutElement (const Identifier id) const
{
  if( m_tgcIdHelper->is_tgc(id) ) return getTgcReadoutElement(id);
  if( m_rpcIdHelper->is_rpc(id) ) return getRpcReadoutElement(id);
  if( m_cscIdHelper->is_csc(id) ) return getCscReadoutElement(id);
  return 0;
}

const MMReadoutElement* MuonDetectorManager::getMMRElement_fromIdFields   (int iStname, int iStEta, int iStPhi, int imL) const
{
  int stname_index = iStname;
  int steta_index = iStEta+NMMcStEtaOffset;
  if (iStEta>0) steta_index = steta_index-1;
  int stphi_index = 2*(iStPhi-1);
  if (iStname==1) stphi_index = stphi_index+1; // small sectors 
  int ml_index = imL-1;

  if (stname_index<0 || stname_index > NMMcStatType) 
  {
      std::cerr<<"ERROR MuonDetectorManager::getMMRElement stNameindex out of range "
               <<stname_index<<" 0(=Large)-"<<NMMcStatType<<"(=Small)"<<std::endl;
      throw;
  }
  stname_index = 0;
  if (steta_index<0 || steta_index >= NMMcStatEta) 
  {
      std::cerr<<"ERROR MuonDetectorManager::getMMRElement stEtaindex out of range "
               <<steta_index<<" 0-"<<NMMcStatEta-1<<std::endl;
      throw;
  }
  if (stphi_index<0 ||  stphi_index>= NMMcStatPhi) 
  {
      std::cerr<<"ERROR MuonDetectorManager::getMMRElement stPhiindex out of range "
               <<stphi_index<<" 0-"<<NMMcStatPhi-1<<std::endl;
      throw;
  }
  if (ml_index < 0 || ml_index >= NMMcChamberLayer)
  {
      std::cerr<<"ERROR MuonDetectorManager::getMMRElement ml_index out of range "
               <<ml_index<<" 0-" <<NMMcChamberLayer-1<<std::endl;
      throw;
  }
    
  return m_mmcArray[stname_index][steta_index][stphi_index][ml_index];
}
void MuonDetectorManager::addMMReadoutElement_withIdFields   (const MMReadoutElement* x,  int iStname, int iStEta, int iStPhi, int imL)
{
  std::cout<<" Willing to add MM RE with IdFields: iStName, iStEta, iStPhi, imL = "<<iStname<<" "<<iStEta<<" "<<iStPhi<<" "<<imL<<std::endl;
  int stname_index = iStname;
  int steta_index = iStEta+NMMcStEtaOffset;
  if (iStEta>0) steta_index = steta_index-1;
  int stphi_index = 2*(iStPhi-1);
  if (iStname==1) stphi_index = stphi_index+1; // small sectors 
  int ml_index = imL-1;

  if (stname_index<0 || stname_index > NMMcStatType) 
  {
      std::cerr<<"ERROR MuonDetectorManager::addMMReadoutElement stNameindex out of range "
               <<stname_index<<" 0(=Large)-"<<NMMcStatType<<"(=Small)"<<std::endl;
      throw;
  }
  stname_index = 0;
  if (steta_index<0 || steta_index >= NMMcStatEta) 
  {
      std::cerr<<"ERROR MuonDetectorManager::addMMReadoutElement stEtaindex out of range "
               <<steta_index<<" 0-"<<NMMcStatEta-1<<std::endl;
      throw;
  }
  if (stphi_index<0 ||  stphi_index>= NMMcStatPhi) 
  {
      std::cerr<<"ERROR MuonDetectorManager::addMMReadoutElement stPhiindex out of range "
               <<stphi_index<<" 0-"<<NMMcStatPhi-1<<std::endl;
      throw;
  }
  if (ml_index < 0 || ml_index >= NMMcChamberLayer)
  {
      std::cerr<<"ERROR MuonDetectorManager::addMMReadoutElement ml_index out of range "
               <<ml_index<<" 0-" <<NMMcChamberLayer-1<<std::endl;
      throw;
  }
    
  //  m_mdtArray[stName][stEta+8][stPhi+8][ml] = x;
  std::cout<<" ... using internal indices  ["
	   <<stname_index<<"]["
	   <<steta_index<< "]["
	   <<stphi_index<< "]["
	   <<ml_index<<    "]"<<std::endl;
  if (m_mmcArray[stname_index][steta_index][stphi_index][ml_index] != NULL)
    {
      std::cerr<<"addMMRaedoutElement - this place is taken ["
               <<stname_index<<"]["
               <<steta_index<< "]["
               <<stphi_index<< "]["
               <<ml_index<<    "] ......... this RE cannot be added"<<std::endl;
      return;
    }

  m_mmcArray[stname_index][steta_index][stphi_index][ml_index] = x;
  
  m_n_mmcRE++;
  
}
void MuonDetectorManager::addsTgcReadoutElement_withIdFields   (const sTgcReadoutElement* x,  int iStname, int iStEta, int iStPhi, int imL)
{
  std::cout<<" Willing to add sTGC RE with IdFields: iStName, iStEta, iStPhi, imL = "<<iStname<<" "<<iStEta<<" "<<iStPhi<<" "<<imL<<std::endl;
  int stname_index = iStname;
  int steta_index = iStEta+NsTgStEtaOffset;
  if (iStEta>0) steta_index = steta_index-1;
  int stphi_index = 2*(iStPhi-1);
  if (iStname==1) stphi_index = stphi_index+1; // small sectors 
  int ml_index = imL-1;

  if (stname_index<0 || stname_index > NsTgStatType) 
  {
      std::cerr<<"ERROR MuonDetectorManager::addsTgcReadoutElement stNameindex out of range "
               <<stname_index<<" 0(=Large)-"<<NsTgStatType<<"(=Small)"<<std::endl;
      throw;
  }
  stname_index = 0;
  if (steta_index<0 || steta_index >= NsTgStatEta) 
  {
      std::cerr<<"ERROR MuonDetectorManager::addsTgcReadoutElement stEtaindex out of range "
               <<steta_index<<" 0-"<<NsTgStatEta-1<<std::endl;
      throw;
  }
  if (stphi_index<0 ||  stphi_index>= NsTgStatPhi) 
  {
      std::cerr<<"ERROR MuonDetectorManager::addsTgcReadoutElement stPhiindex out of range "
               <<stphi_index<<" 0-"<<NsTgStatPhi-1<<std::endl;
      throw;
  }
  if (ml_index < 0 || ml_index >= NsTgChamberLayer)
  {
      std::cerr<<"ERROR MuonDetectorManager::addsTgcReadoutElement ml_index out of range "
               <<ml_index<<" 0-" <<NsTgChamberLayer-1<<std::endl;
      throw;
  }
    

  std::cout<<" ... using internal indices  ["
	   <<stname_index<<"]["
	   <<steta_index<< "]["
	   <<stphi_index<< "]["
	   <<ml_index<<    "]"<<std::endl;
  if (m_stgArray[stname_index][steta_index][stphi_index][ml_index] != NULL)
    {
      std::cerr<<"addsTgcReadoutElement - this place is taken ["
               <<stname_index<<"]["
               <<steta_index<< "]["
               <<stphi_index<< "]["
               <<ml_index<<    "] ......... this RE cannot be added"<<std::endl;
      return;
    }

  m_stgArray[stname_index][steta_index][stphi_index][ml_index] = x;
  
  m_n_stgRE++;
  
}
const sTgcReadoutElement* MuonDetectorManager::getsTgcRElement_fromIdFields   (int iStname, int iStEta, int iStPhi, int imL) const
{
  int stname_index = iStname;
  int steta_index = iStEta+NsTgStEtaOffset;
  if (iStEta>0) steta_index = steta_index-1;
  int stphi_index = 2*(iStPhi-1);
  if (iStname==1) stphi_index = stphi_index+1; // small sectors 
  int ml_index = imL-1;

  if (stname_index<0 || stname_index > NsTgStatType) 
  {
      std::cerr<<"ERROR MuonDetectorManager::getsTgcRElement stNameindex out of range "
               <<stname_index<<" 0(=Large)-"<<NsTgStatType<<"(=Small)"<<std::endl;
      throw;
  }
  stname_index = 0;
  if (steta_index<0 || steta_index >= NsTgStatEta) 
  {
      std::cerr<<"ERROR MuonDetectorManager::getsTgcRElement stEtaindex out of range "
               <<steta_index<<" 0-"<<NsTgStatEta-1<<std::endl;
      throw;
  }
  if (stphi_index<0 ||  stphi_index>= NsTgStatPhi) 
  {
      std::cerr<<"ERROR MuonDetectorManager::getsTgcRElement stPhiindex out of range "
               <<stphi_index<<" 0-"<<NsTgStatPhi-1<<std::endl;
      throw;
  }
  if (ml_index < 0 || ml_index >= NsTgChamberLayer)
  {
      std::cerr<<"ERROR MuonDetectorManager::getsTgcRElement ml_index out of range "
               <<ml_index<<" 0-" <<NsTgChamberLayer-1<<std::endl;
      throw;
  }    
  return m_stgArray[stname_index][steta_index][stphi_index][ml_index];
}

void MuonDetectorManager::addMMReadoutElement (const MMReadoutElement* x, Identifier id)
{
    if (id!=x->identify()) 
    {
	MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
	log<<MSG::ERROR<<"Trying to add MMReadoutElement with id "
	   <<m_mmIdHelper->show_to_string(id)
	   <<" not matching the id assigned to the MMReadoutElement "
	   <<m_mmIdHelper->show_to_string(x->identify())<<endreq;
	throw;
	return;
    }
    // do nothing for the moment 
    //
}    
void MuonDetectorManager::addsTgcReadoutElement (const sTgcReadoutElement* x, Identifier id)
{
    if (id!=x->identify()) 
    {
	MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
	log<<MSG::ERROR<<"Trying to add sTgcReadoutElement with id "
	   <<m_stgcIdHelper->show_to_string(id)
	   <<" not matching the id assigned to the sTgcReadoutElement "
	   <<m_stgcIdHelper->show_to_string(x->identify())<<endreq;
	throw;
	return;
    }
    // do nothing for the moment 
    //
}    
void MuonDetectorManager::addMdtReadoutElement (const MdtReadoutElement* x, Identifier id)
{
    if (id!=x->identify()) 
    {
	MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
	log<<MSG::ERROR<<"Trying to add MdtReadoutElement with id "
	   <<m_mdtIdHelper->show_to_string(id)
	   <<" not matching the id assigned to the MdtReadoutElement "
	   <<m_mdtIdHelper->show_to_string(x->identify())<<endreq;
	throw;
	return;
    }
    
    int stName = m_mdtIdHelper->stationName(id);
    int stEta  = m_mdtIdHelper->stationEta(id);
    int stPhi  = m_mdtIdHelper->stationPhi(id);
    int ml     = m_mdtIdHelper->multilayer(id);
    
    
    //add here the MdtReadoutElement to the array by RE hash
    //use already known RE hash 
    IdentifierHash Idhash = 0;
    Idhash = x->detectorElementHash();
    if (Idhash >= MdtRElMaxHash)
    {  
	MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
	log<<MSG::ERROR<<"Trying to add MdtReadoutElement with detector-element-hash id "
	   <<(unsigned int)Idhash
	   <<" outside boundaries 0-"<<MdtRElMaxHash<<endreq;	
	throw;
	return;
    }
    else
    {
	if (m_mdtArrayByHash[Idhash]!=0) 
	{
	    MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
	    log<<MSG::ERROR<<"Trying to add MdtReadoutElement with detector-element-hash id "
	       <<(unsigned int)Idhash<<" id = "<<m_mdtIdHelper->show_to_string(id)
	       <<" at location already taken by "
	       <<m_mdtIdHelper->show_to_string(m_mdtArrayByHash[Idhash]->identify())
	       <<endreq;
	    throw;
	    return;
	}
	m_mdtArrayByHash[Idhash] = x;
    }
    //add here the MdtDetectorElement and/or add this readoutElement to the DetectorElement
    //use already known data-collection hash 
    IdentifierHash idh = x->collectionHash();
    if (idh< MdtDetElMaxHash) 
    {
	if ( !(m_mdtDEArray[idh]) )
	{
	    m_mdtDEArray[idh] = new MdtDetectorElement(0, this, m_mdtIdHelper->elementID(id), idh);
	    m_n_mdtDE++;
	}
	m_mdtDEArray[idh]->addMdtReadoutElement(x, ml);
    }
    else
    {
	MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
	log<<MSG::ERROR<<"Trying to add MdtDetectorElement with data-collection-hash id "
	   <<(unsigned int)idh
	   <<" outside boundaries 0-"<<MdtDetElMaxHash<<endreq;	
	throw;
	return;
    }

    

  int stname_index = stName;
  if (stName == 49) {
      //std::cerr<<" Here we meet stName = "<<stName<<" for "<<m_mdtIdHelper->stationNameString(stName);
      stname_index = NMdtStatType-4;       //EIS
      //std::cerr<<" stname_index = "<<stname_index<<std::endl;
  }
  else if (stName == 52) {
      //std::cerr<<" Here we meet stName = "<<stName<<" for "<<m_mdtIdHelper->stationNameString(stName);
      stname_index = NMdtStatType-3;  //BIM
      //std::cerr<<" stname_index = "<<stname_index<<std::endl;
  } 
  else if (stName == 53) {
      //std::cerr<<" Here we meet stName = "<<stName<<" for "<<m_mdtIdHelper->stationNameString(stName);
      stname_index = NMdtStatType-2;  //BME
      //std::cerr<<" stname_index = "<<stname_index<<std::endl;
  }
  else if (stName == 54) {
      //std::cerr<<" Here we meet stName = "<<stName<<" for "<<m_mdtIdHelper->stationNameString(stName);
      stname_index = NMdtStatType-1;  //BOE
      //std::cerr<<" stname_index = "<<stname_index<<std::endl;
  }
  int steta_index = stEta+NMdtStEtaOffset;
  int stphi_index = stPhi-1;
  int ml_index = ml-1;

  if (stname_index<0 || stname_index >= NMdtStatType) 
  {
      std::cerr<<"ERROR MuonDetectorManager::addMdtReadoutElement stNameindex out of range "
               <<stname_index<<" 0-"<<NMdtStatType-1<<std::endl;
      throw;
  }
  if (steta_index<0 || steta_index >= NMdtStatEta) 
  {
      std::cerr<<"ERROR MuonDetectorManager::addMdtReadoutElement stEtaindex out of range "
               <<steta_index<<" 0-"<<NMdtStatEta-1<<std::endl;
      throw;
  }
  if (stphi_index<0 ||  stphi_index>= NMdtStatPhi) 
  {
      std::cerr<<"ERROR MuonDetectorManager::addMdtReadoutElement stPhiindex out of range "
               <<stphi_index<<" 0-"<<NMdtStatPhi-1<<std::endl;
      throw;
  }
  if (ml_index < 0 || ml_index >= NMdtMultilayer)
  {
      std::cerr<<"ERROR MuonDetectorManager::addMdtReadoutElement ml_index out of range "
               <<ml_index<<" 0-" <<NMdtMultilayer-1<<std::endl;
      throw;
  }
    
  //  m_mdtArray[stName][stEta+8][stPhi+8][ml] = x;
  if (m_mdtArray[stname_index][steta_index][stphi_index][ml_index] != NULL)
      std::cerr<<"addMdtReadoutElement - this place is taken ["
               <<stname_index<<"]["
               <<steta_index<< "]["
               <<stphi_index<< "]["
               <<ml_index<<    "] current id is "<<m_mdtIdHelper->show_to_string(id)<<" stored id "
               <<m_mdtIdHelper->show_to_string(m_mdtArray[stname_index][steta_index][stphi_index][ml_index]->identify())
               <<std::endl;
//   std::cerr<<"Manager adding MdtReadoutElement with id "<<m_mdtIdHelper->show_to_string(id)
//            <<" at "<<stname_index<<" "<<steta_index<<" "<<stphi_index<<" "<<ml_index<<std::endl;
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

void MuonDetectorManager::addCscReadoutElement (const CscReadoutElement* x, Identifier id)
{
    if (id!=x->identify()) 
    {
	MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
	log<<MSG::ERROR<<"Trying to add CscReadoutElement with id "
	   <<m_cscIdHelper->show_to_string(id)
	   <<" not matching the id assigned to the CscReadoutElement "
	   <<m_cscIdHelper->show_to_string(x->identify())<<endreq;
	throw;
    }
    
    // add here RE to array by hash 
    IdentifierHash Idhash = x->detectorElementHash();
    if (Idhash >= CscRElMaxHash)
    {  
	MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
	log<<MSG::ERROR<<"Trying to add CscReadoutElement with detector-element-hash id "
	   <<(unsigned int)Idhash
	   <<" outside boundaries 0-"<<CscRElMaxHash<<endreq;	
	throw;
    }	
    else
    {
	if (m_cscArrayByHash[Idhash]!=0) 
	{
	    MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
	    log<<MSG::ERROR<<"Trying to add CscReadoutElement with detector-element-hash id "
	       <<(unsigned int)Idhash<<" id = "<<m_cscIdHelper->show_to_string(id)
	       <<" at location already taken by "
	       <<m_cscIdHelper->show_to_string(m_cscArrayByHash[Idhash]->identify())
	       <<endreq;
	    throw;
	}
	m_cscArrayByHash[Idhash] = x;
    }

    //add here the CscDetectorElement and/or add this readoutElement to the DetectorElement
    IdentifierHash idh = x->collectionHash(); 
    if ( idh< CscDetElMaxHash ) 
    {
	if ( !(m_cscDEArray[idh]) )
	{
	    m_cscDEArray[idh] = new CscDetectorElement(0, this, m_cscIdHelper->elementID(id), idh);
	    m_n_cscDE++;
	}
	m_cscDEArray[idh]->setReadoutElement(x);
    }
    else
    {
	MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
	log<<MSG::ERROR<<"Trying to add CscDetectorElement with data-collection-hash id "
	   <<(unsigned int)idh
	   <<" outside boundaries 0-"<<CscDetElMaxHash<<endreq;	
	throw;
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

    if (stname_index<0 || stname_index >= NCscStatType) 
    {
        std::cerr<<"ERROR MuonDetectorManager::addCscReadoutElement stNameindex out of range "
                 <<stname_index<<" 0-"<<NCscStatType-1<<std::endl;
        throw;
    }
    if (steta_index<0 || steta_index >= NCscStatEta) 
    {
        std::cerr<<"ERROR MuonDetectorManager::addCscReadoutElement stEtaindex out of range "
                 <<steta_index<<" 0-"<<NCscStatEta-1<<std::endl;
        throw;
    }
    if (stphi_index<0 ||  stphi_index>= NCscStatPhi) 
    {
        std::cerr<<"ERROR MuonDetectorManager::addCscReadoutElement stPhiindex out of range "
                 <<stphi_index<<" 0-"<<NCscStatPhi-1<<std::endl;
        throw;
    }
    if (ml_index < 0 || ml_index >= NCscChamberLayer)
    {
        std::cerr<<"ERROR MuonDetectorManager::addCscReadoutElement ml_index out of range "
                 <<ml_index<<" 0-" <<NCscChamberLayer-1<<std::endl;
        throw;
    }
    
    //  m_cscArray[stName][stEta+8][stPhi+8][ml] = x;
    if (m_cscArray[stname_index][steta_index][stphi_index][ml_index] != NULL)
        std::cerr<<"addCscReadoutElement - this place is taken ["
                 <<stname_index<<"]["
                 <<steta_index<< "]["
                 <<stphi_index<< "]["
                 <<ml_index<<    "] current id is "<<m_cscIdHelper->show_to_string(id)<<" stored id "
                 <<m_cscIdHelper->show_to_string(m_cscArray[stname_index][steta_index][stphi_index][ml_index]->identify())
                 <<std::endl;
    m_cscArray[stname_index][steta_index][stphi_index][ml_index] = x;
//     std::cerr<<"Registered csc r.e. at location ["
//              <<stname_index<<"]["<<steta_index<<"]["<<stphi_index<<"]["<<ml_index<<"]"<<std::endl;
//    std::cerr<<"Manager adding CscReadoutElement with id "<<m_cscIdHelper->show_to_string(id)
//             <<" at "<<stname_index<<" "<<steta_index<<" "<<stphi_index<<" "<<ml_index<<std::endl;
    m_n_cscRE++;

}

const CscReadoutElement* MuonDetectorManager::getCscReadoutElement(Identifier id) const
{ 

  int stName = m_cscIdHelper->stationName(id);
  int stEta  = m_cscIdHelper->stationEta(id);
  int stPhi  = m_cscIdHelper->stationPhi(id);
  int ml     = m_cscIdHelper->chamberLayer(id);
//   std::cout<<"Trying to get csc-readout-elements from an id with name,eta,phi,ml "
//            <<stName<<" "<<stEta<<" "<<stPhi<<" "<<ml<<std::endl;

  int stname_index = stName + NCscStatTypeOff;
  int steta_index = stEta+NCscStEtaOffset;
  if (steta_index > 1) steta_index=1;
  int stphi_index = stPhi-1;
  int ml_index = ml-1;

  return getCscReadoutElement(stname_index,steta_index,stphi_index,ml_index);

}

void MuonDetectorManager::addTgcReadoutElement (const TgcReadoutElement* x, Identifier id)
{

    if (id!=x->identify()) 
    {
	MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
	log<<MSG::ERROR<<"Trying to add TgcReadoutElement with id "
	   <<m_tgcIdHelper->show_to_string(id)
	   <<" not matching the id assigned to the TgcReadoutElement "
	   <<m_tgcIdHelper->show_to_string(x->identify())<<endreq;
	throw;
    }

    // add RE to array by RE hash
    IdentifierHash Idhash = x->detectorElementHash();
    if (Idhash >= TgcRElMaxHash)
    {  
	MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
	log<<MSG::ERROR<<"Trying to add TgcReadoutElement with detector-element-hash id "
	   <<(unsigned int)Idhash
	   <<" outside boundaries 0-"<<TgcRElMaxHash<<endreq;	
	throw;
    }		
    else
    {
      //MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
      if (m_tgcArrayByHash[Idhash]!=0) 
	{
	  MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
	    log<<MSG::ERROR<<"Trying to add TgcReadoutElement with detector-element-hash id "
	       <<(unsigned int)Idhash<<" id = "<<m_tgcIdHelper->show_to_string(id)
	       <<" at location already taken by "
	       <<m_tgcIdHelper->show_to_string(m_tgcArrayByHash[Idhash]->identify())
	       <<endreq;
	    throw;
	}
	m_tgcArrayByHash[Idhash] = x;
	//log<<MSG::DEBUG<<"Adding TgcReadoutElement at loc. detector-element-hash id "<<(unsigned int)Idhash<<" for ext id = "<<m_tgcIdHelper->show_to_string(id)<<endreq;
    }

    //add here the TgcDetectorElement and/or add this readoutElement to the DetectorElement
    IdentifierHash idh = x->collectionHash();
    if (idh< TgcDetElMaxHash) 
    {
	if ( !(m_tgcDEArray[idh]) )
	{
	    m_tgcDEArray[idh] = new TgcDetectorElement(0, this, m_tgcIdHelper->elementID(id), idh);
	    m_n_tgcDE++;
	}
	m_tgcDEArray[idh]->setReadoutElement(x);
    }
    else
    {
	MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
	log<<MSG::ERROR<<"Trying to add TgcDetectorElement with data-collection-hash id "
	   <<(unsigned int)idh
	   <<" outside boundaries 0-"<<TgcDetElMaxHash<<endreq;	
	throw;
    }
    

    int stname_index = m_tgcIdHelper->stationName(id) + NTgcStatTypeOff;
    int zi = m_tgcIdHelper->stationEta(id);
    int steta_index  = zi  + NTgcStEtaOffset;
    if (zi>0) steta_index--;  
    int stphi_index  = m_tgcIdHelper->stationPhi(id)  - 1;

    if (stname_index<0 || stname_index >= NTgcStatType) 
    {
        std::cerr<<"ERROR MuonDetectorManager::addTgcReadoutElement stNameindex out of range "
                 <<stname_index<<" 0-"<<NTgcStatType-1<<std::endl;
        throw;
    }
    if (steta_index<0 || steta_index >= NTgcStatEta) 
    {
        std::cerr<<"ERROR MuonDetectorManager::addTgcReadoutElement stEtaindex out of range "
                 <<steta_index<<" 0-"<<NTgcStatEta-1<<std::endl;
        throw;
    }
    if (stphi_index<0 ||  stphi_index>= NTgcStatPhi) 
    {
        std::cerr<<"ERROR MuonDetectorManager::addTgcReadoutElement stPhiindex out of range "
                 <<stphi_index<<" 0-"<<NTgcStatPhi-1<<std::endl;
        throw;
    }
    if (m_tgcArray[stname_index][steta_index][stphi_index] != NULL)
        std::cerr<<"addTgcReadoutElement - this place is taken ]"
                 <<stname_index<<"]["
                 <<steta_index<< "]["
                 <<stphi_index<< "] current id is "<<m_tgcIdHelper->show_to_string(id)<<" stored id "
                 <<m_tgcIdHelper->show_to_string(m_tgcArray[stname_index][steta_index][stphi_index]->identify())
                 <<std::endl;
    m_tgcArray[stname_index][steta_index][stphi_index] = x;
//     std::cerr<<"Registered tgc r.e. at location ["
//              <<stname_index<<"]["<<steta_index<<"]["<<stphi_index<<"]"<<std::endl;
//    std::cerr<<"Manager adding TgcReadoutElement with id "<<m_tgcIdHelper->show_to_string(id)
//             <<" at "<<stname_index<<" "<<steta_index<<" "<<stphi_index<<std::endl;
  m_n_tgcRE++;
}

const TgcReadoutElement* MuonDetectorManager::getTgcReadoutElement (Identifier id) const
{
    //    std::cerr<<" Trying to get TgcReadoutElement with id "<<m_tgcIdHelper->show_to_string(id)<<std::endl;
    
//     int stName = m_tgcIdHelper->stationName(id);
//     int stEta  = m_tgcIdHelper->stationEta(id);
//     int stPhi  = m_tgcIdHelper->stationPhi(id);

    int stname_index = m_tgcIdHelper->stationName(id) + NTgcStatTypeOff;
    int zi = m_tgcIdHelper->stationEta(id);
    int steta_index  = zi  + NTgcStEtaOffset;
    if (zi>0) steta_index--;  
    int stphi_index  = m_tgcIdHelper->stationPhi(id)  - 1;

//     std::cerr<<"Returning tgc r.e. at location ["
//              <<stname_index<<"]["<<steta_index<<"]["<<stphi_index<<"]"<<std::endl;
    
    return getTgcReadoutElement(stname_index,steta_index,stphi_index);
}

const MMReadoutElement*  MuonDetectorManager::getMMReadoutElement(Identifier id) const
{
  int isSmall = m_mmIdHelper->isSmall(id);
  int phiCor = m_mmIdHelper->stationPhi(id);
  int mlCor  = m_mmIdHelper->multilayer(id);
  return getMMRElement_fromIdFields(isSmall,m_mmIdHelper->stationEta(id),phiCor,mlCor );
}

const sTgcReadoutElement*  MuonDetectorManager::getsTgcReadoutElement(Identifier id) const
{
  int isSmall = m_stgcIdHelper->isSmall(id);
  int phiCor = m_stgcIdHelper->stationPhi(id);
  int mlCor  = m_stgcIdHelper->multilayer(id);
  return getsTgcRElement_fromIdFields(isSmall,m_stgcIdHelper->stationEta(id),phiCor,mlCor );
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

const MdtReadoutElement* MuonDetectorManager::getMdtReadoutElement(int i1, int i2, int i3, int i4)const
{
  if (i1<0 || i1 >= NMdtStatType) 
  {
      std::cerr<<"ERROR MuonDetectorManager::getMdtReadoutElement stNameindex out of range "
               <<i1<<" 0-"<<NMdtStatType-1<<std::endl;
      throw;
  }
  if (i2<0 || i2 >= NMdtStatEta) 
  {
      std::cerr<<"ERROR MuonDetectorManager::getMdtReadoutElement stEtaindex out of range "
               <<i2<<" 0-"<<NMdtStatEta-1<<std::endl;
      throw;
  }
  if (i3<0 ||  i3 >= NMdtStatPhi) 
  {
      std::cerr<<"ERROR MuonDetectorManager::getMdtReadoutElement stPhiindex out of range "
               <<i3<<" 0-"<<NMdtStatPhi-1<<std::endl;
      throw;
  }
  if (i4 < 0 || i4 >= NMdtMultilayer)
  {
      std::cerr<<"ERROR MuonDetectorManager::getMdtReadoutElement ml_index out of range "
               <<i4<<" 0-" <<NMdtMultilayer-1<<std::endl;
      throw;
  }    

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

const CscReadoutElement* MuonDetectorManager::getCscReadoutElement(int i1, int i2, int i3, int i4)const
{
  if (i1<0 || i1 >= NCscStatType) 
  {
      std::cerr<<"ERROR MuonDetectorManager::getCscReadoutElement stNameindex out of range "
               <<i1<<" 0-"<<NCscStatType-1<<std::endl;
      throw;
  }
  if (i2<0 || i2 >= NCscStatEta) 
  {
      std::cerr<<"ERROR MuonDetectorManager::getCscReadoutElement stEtaindex out of range "
               <<i2<<" 0-"<<NCscStatEta-1<<std::endl;
      throw;
  }
  if (i3<0 ||  i3 >= NCscStatPhi) 
  {
      std::cerr<<"ERROR MuonDetectorManager::getCscReadoutElement stPhiindex out of range "
               <<i3<<" 0-"<<NCscStatPhi-1<<std::endl;
      throw;
  }
  if (i4 < 0 || i4 >= NCscChamberLayer)
  {
      std::cerr<<"ERROR MuonDetectorManager::getCscReadoutElement ml_index out of range "
               <<i4<<" 0-" <<NCscChamberLayer-1<<std::endl;
      throw;
  }    

    return m_cscArray[i1][i2][i3][i4];
}
const RpcReadoutElement* MuonDetectorManager::getRpcRElement_fromIdFields(int i1,
                                                                          int i2,
                                                                          int i3, int i4, int i5, int i6) const
{
    int stname_indexId = i1; 
    int stname_index = stname_indexId + NRpcStatTypeOff;
    if (stname_indexId == 53) // BME 
    {
	stname_index = NRpcStatType-2;
    }
    else if (stname_indexId == 54) // BOE 
    {
	stname_index = NRpcStatType-1;
    }


      int steta_index  = i2 + NRpcStEtaOffset;
      int stphi_index  = i3 - 1;
      int dbr_index    = i4 - 1;
      int dbz_index    = i5 - 1;
      int doubletPhi   = i6;
      

      int dbr = i4;
      int dbz = i5;
      int absEta = abs(i2);


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
	      // std::cout<<" here we are dbz_index ="<<dbz_index<<std::endl;
	      // std::cout<<" this function was called with st/eta/phi/rdr/rdz/dbp ="<<i1<<" "<<i2<<" "<<i3<<" "<<i4<<" "<<i5<<" "<<i6<<std::endl;
	      // cannot just exit because this dbz_index is actually used for dbPhi=2  
	       return NULL;	   
          }      
      }
      
      return getRpcReadoutElement(stname_index, steta_index, stphi_index, dbr_index, dbz_index);
}
const RpcReadoutElement* MuonDetectorManager::getRpcReadoutElement(int i1, int i2, int i3, int i4, int i5)const
{
  if (i1<0 || i1 >= NRpcStatType) 
  {
      std::cerr<<"ERROR MuonDetectorManager::getRpcReadoutElement stNameindex out of range "
               <<i1<<" 0-"<<NRpcStatType-1<<std::endl;
      throw;
  }
  if (i2<0 || i2 >= NRpcStatEta) 
  {
      std::cerr<<"ERROR MuonDetectorManager::getRpcReadoutElement stEtaindex out of range "
               <<i2<<" 0-"<<NRpcStatEta-1<<std::endl;
      return 0;
      //throw;
  }
  if (i3<0 ||  i3 >= NRpcStatPhi) 
  {
      std::cerr<<"ERROR MuonDetectorManager::getRpcReadoutElement stPhiindex out of range "
               <<i3<<" 0-"<<NRpcStatPhi-1<<std::endl;
      throw;
  }
  if (i4 < 0 || i4 >= NDoubletR)
  {
      std::cerr<<"ERROR MuonDetectorManager::getRpcReadoutElement dbr_index out of range "
               <<i4<<" 0-" <<NDoubletR-1<<std::endl;
      throw;
  }
  if (i5 < 0 || i5 >= NDoubletZ)
  {
      std::cerr<<"ERROR MuonDetectorManager::getRpcReadoutElement dbz_index out of range "
               <<i5<<" 0-" <<NDoubletZ-1<<std::endl;
      return 0;
      //throw;
  }
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
const TgcReadoutElement* MuonDetectorManager::getTgcReadoutElement(int i1, int i2, int i3)const
{
//     std::cerr<<" MuonDetectorManager::getTgcReadoutElement with i1,i2,i3 "<<i1<<" "<<i2<<" "<<i3<<std::endl;
    if (i1<0 || i1 >= NTgcStatType) 
    {
        std::cerr<<"ERROR MuonDetectorManager::getTgcReadoutElement stNameindex out of range "
                 <<i1<<" 0-"<<NTgcStatType-1<<std::endl;
        throw;
    }
    if (i2<0 || i2 >= NTgcStatEta) 
    {
        std::cerr<<"ERROR MuonDetectorManager::getTgcReadoutElement stEtaindex out of range "
                 <<i2<<" 0-"<<NTgcStatEta-1<<std::endl;
        throw;
    }
    if (i3<0 ||  i3>= NTgcStatPhi) 
    {
        std::cerr<<"ERROR MuonDetectorManager::getTgcReadoutElement stPhiindex out of range "
                 <<i3<<" 0-"<<NTgcStatPhi-1<<std::endl;
        throw;
    }
//     std::cout<<" getting the array component ..."<<std::endl;
    return m_tgcArray[i1][i2][i3];
}

void
MuonDetectorManager::initABlineContainers() const
{
    MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
    m_aLineContainer = new ALineMapContainer;
    m_bLineContainer = new BLineMapContainer;

    if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"Init A/B Line Containers - pointers are <"
       <<(uintptr_t)m_aLineContainer<<"> and <"<<(uintptr_t)m_bLineContainer<<">"<<endreq;

    // loop over stations to fill the A-line map at start-up
    for (std::map< std::string, MuonStation * >::const_iterator ist=m_MuonStationMap.begin();
         ist!=m_MuonStationMap.end(); ++ist)
    {
        MuonStation * ms = (*ist).second;
        int jff = ms->getPhiIndex();
        int jzz = ms->getEtaIndex();
        std::string stType = ms->getStationType();
       
        ALinePar* newALine = new ALinePar();
        newALine->setAmdbId(stType, jff, jzz, 0);
	if (ms->hasALines()) 
	{
	  newALine->setParameters(ms->getALine_tras(),ms->getALine_traz(),ms->getALine_trat(),
				  ms->getALine_rots(),ms->getALine_rotz(),ms->getALine_rott());	
	}
	else
	{
	  if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"No starting A-lines for Station "<<stType<<" Jzz/Jff "<<jzz<<"/"<<jff<<endreq;
	  newALine->setParameters(0.,0.,0.,0.,0.,0.);				  
	}
        newALine->isNew(true);

        Identifier id;
        //= m_mdtIdHelper->elementID(stType, jzz, jff);
        if (stType.substr(0,1)=="T") 
        {
            // TGC case
            int stPhi = MuonGM::stationPhiTGC(stType,jff,jzz,geometryVersion());
            int stEta = 1; // stEta for the station is stEta for the first component chamber
            if (jzz<0) stEta = -1; // stEta for the station is stEta for the first component chamber
            id = m_tgcIdHelper->elementID(stType, stEta, stPhi);
            if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"Filling A-line container with entry for key = "<<m_tgcIdHelper->show_to_string(id)<<std::endl;
        }
        else if (stType.substr(0,1)=="C")
        {
            // CSC case
            id = m_cscIdHelper->elementID(stType, jzz, jff);
            if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"Filling A-line container with entry for key = "<<m_cscIdHelper->show_to_string(id)<<std::endl;
        }
        else if (stType.substr(0,3)=="BML" && abs(jzz)==7) 
        {
            // RPC case
            id = m_rpcIdHelper->elementID(stType, jzz, jff, 1);
            if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"Filling A-line container with entry for key = "<<m_rpcIdHelper->show_to_string(id)<<std::endl;
        }
        else
        {
            id = m_mdtIdHelper->elementID(stType, jzz, jff);
            if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"Filling A-line container with entry for key = "<<m_mdtIdHelper->show_to_string(id)<<std::endl;
        }
        m_aLineContainer->insert(std::make_pair(id,(ALinePar*)newALine));
        if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"<Filling A-line container with entry for key >"<<m_mdtIdHelper->show_to_string(id)<<std::endl;
    }
    log<<MSG::INFO<<"Init A/B Line Containers - done - size is respectively "
       << m_aLineContainer->size()<<"/"<<m_bLineContainer->size()<<endreq;
}


StatusCode
MuonDetectorManager::updateAlignment(const ALineMapContainer *  alineData) const
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
    //    IMessageSvc* m_msgSvc = Athena::getMessageSvc();
    MsgStream log(m_msgSvc, "MGM::MuonDetectorManager::updateAlignment");
    //log<<MSG::INFO<<"In updateAlignment()  --- pointer to the container is <"<<alineData<<">"<<endreq;
    //log<<MSG::INFO<<"In updateAlignment()  --- pointer to the container (casted to int) <"
    //<<(int)alineData<<">"<<endreq;
    if (alineData == NULL)
    {
	log<<MSG::WARNING<<"Undefined pointer to temporary A-line container - nothing to do here"<<endreq;
	return StatusCode::SUCCESS;
    }
    if (alineData->size()==0)
    {
	log<<MSG::WARNING<<"Empty temporary A-line container - nothing to do here"<<endreq;
	return StatusCode::SUCCESS;
    }
    else log<<MSG::INFO<<"temporary A-line container with size = "<<alineData->size()<<endreq;
    

    // loop over the container of the updates passed by the MuonAlignmentDbTool
    ciALineMap cialine = alineData->begin();//ALineMapBegin();
    unsigned int nLines = 0;
    unsigned int nUpdates = 0;
    for (; cialine != alineData->end(); cialine++)
    {
        nLines++;
	//std::cout<<"Aline n. "<<nLines<<std::endl;
        Identifier ALineId = (*cialine).first;
        ALinePar* ALine = (*cialine).second;
        std::string stType = "";
        int jff = 0;
        int jzz = 0;
        int job = 0;
        ALine->getAmdbId(stType, jff, jzz, job);
        if (!ALine->isNew())
        {            
            log<<MSG::WARNING
               <<"ALinePar with AmdbId "
               <<stType<<" "<<jzz<<" "<<jff<<" "<<job
               <<" is not new *** skipping"<<endreq;
            continue;
        }
	if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG
					<<"ALinePar with AmdbId "
					<<stType<<" "<<jzz<<" "<<jff<<" "<<job<<" is new ID = "<<m_mdtIdHelper->show_to_string(ALineId)<<endreq;
        if (job == 0) 
        {
            MuonStation* thisStation = getMuonStation(stType, jzz, jff);
            if (thisStation)
            {
		//std::cout<<"This station is found "<<std::endl;
                float s,z,t,ths,thz,tht;
                ALine->getParameters(s,z,t,ths,thz,tht);
		if  (    m_controlAlines%10        == 0)           tht = 0.;
		if  (int(m_controlAlines/10)%10    == 0)           thz = 0.;
		if  (int(m_controlAlines/100)%10   == 0)           ths = 0.;
		if  (int(m_controlAlines/1000)%10  == 0)           t   = 0.;
		if  (int(m_controlAlines/10000)%10 == 0)           z   = 0.;
		if  (int(m_controlAlines/100000)%10== 0)           s   = 0.;
		if  (m_controlAlines!=111111) ALine->setParameters(s,z,t,ths,thz,tht);
                if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"Setting delta transform for Station "<<stType<<" "<<jzz<<" "<<jff<<" "<<" params are = "<<s<<" "<<z<<" "<<t<<" "<<ths<<" "<<thz<<" "<<tht<<endreq;
                thisStation->setDelta_fromAline( s,z,t,ths,thz,tht );
#ifdef TESTBLINES
		ALine->setParameters( 0., 0., 0., 0., 0., 0.);
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
                nUpdates++;
            }
            else
            {                
                log<<MSG::WARNING
                   <<"ALinePar with AmdbId "<<stType<<" "<<jzz<<" "<<jff<<" "<<job
                   <<" *** No MuonStation found \n PLEASE CHECK FOR possible MISMATCHES between alignment constants from COOL and Geometry Layout in use"<<endreq;
		continue;
            }
            // record this A-line in the historical A-line container
            ciALineMap ci = ALineMapBegin();
            //if (stType.substr(0,1)=="T")
            //{
            //    log<<MSG::WARNING<<"A-lines for TGC ... need to fix the Identifier ----- skipping"<<endreq;
            //    continue;
            //}
            //Identifier id = m_mdtIdHelper->elementID(stType, jzz, jff);
            if((ci = ALineContainer()->find(ALineId)) != ALineMapEnd())
            {
                if (log.level()<=MSG::DEBUG) log << MSG::DEBUG<< "Updating extisting entry in A-line container for Station "
						 <<stType<<" at Jzz/Jff "<<jzz<<"/"<< jff << endreq;
		ALinePar * oldALine =  (*ci).second;
                ALineContainer()->erase(ALineId);
		delete oldALine;
            }
            else 
            {
                if (log.level()<=MSG::DEBUG) log << MSG::DEBUG<< "               New entry in A-line container for Station "
                    <<stType<<" at Jzz/Jff "<<jzz<<"/"<< jff <<" --- in the container with key "<< m_mdtIdHelper->show_to_string(ALineId)<< endreq;
            }
            ALineContainer()->insert(std::make_pair(ALineId,ALine));
        }
        else
        {// job different than 0 (standard for TGC conditions for Sept 2010 repro.)            
            if (stType.substr(0,1)=="T") 
	      {
		if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG
		   <<"ALinePar with AmdbId "<<stType<<" "<<jzz<<" "<<jff<<" "<<job
		   <<" has JOB not 0 - this is expected for TGC"<<endreq;
	      }
	    else 
	    {
		log<<MSG::WARNING
		   <<"ALinePar with AmdbId "<<stType<<" "<<jzz<<" "<<jff<<" "<<job
		   <<" has JOB not 0 - this is NOT EXPECTED yet for non TGC chambers - skipping this A-line"<<endreq;
		continue;
	    }
            MuonStation* thisStation = getMuonStation(stType, jzz, jff);
            if (thisStation)
            {
		//std::cout<<"This station is found "<<std::endl;
                float s,z,t,ths,thz,tht;
                ALine->getParameters(s,z,t,ths,thz,tht);
		if  (    m_controlAlines%10        == 0)           tht = 0.;
		if  (int(m_controlAlines/10)%10    == 0)           thz = 0.;
		if  (int(m_controlAlines/100)%10   == 0)           ths = 0.;
		if  (int(m_controlAlines/1000)%10  == 0)           t   = 0.;
		if  (int(m_controlAlines/10000)%10 == 0)           z   = 0.;
		if  (int(m_controlAlines/100000)%10== 0)           s   = 0.;
		if  (m_controlAlines!=111111) ALine->setParameters(s,z,t,ths,thz,tht);
                if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"Setting delta transform for component "<<job<<" of Station "<<stType<<" "<<jzz<<" "<<jff<<" "<<" params are = "<<s<<" "<<z<<" "<<t<<" "<<ths<<" "<<thz<<" "<<tht<<endreq;
                thisStation->setDelta_fromAline_forComp( job, s,z,t,ths,thz,tht );
                if (cacheFillingFlag()) {
		  thisStation->getMuonReadoutElement(job)->clearCache();
		  thisStation->getMuonReadoutElement(job)->fillCache();
		}
		else 
		  {
		    thisStation->getMuonReadoutElement(job)->refreshCache();
		  }
                nUpdates++;
            }
            else
            {                
                log<<MSG::WARNING
                   <<"ALinePar with AmdbId "<<stType<<" "<<jzz<<" "<<jff<<" "<<job
                   <<" *** No MuonStation found \n PLEASE CHECK FOR possible MISMATCHES between alignment constants from COOL and Geometry Layout in use"<<endreq;
		continue;
            }
            // record this A-line in the historical A-line container
            ciALineMap ci = ALineMapBegin();
            if((ci = ALineContainer()->find(ALineId)) != ALineMapEnd())
            {
                if (log.level()<=MSG::DEBUG) log << MSG::DEBUG<< "Updating existing entry in A-line container for Station "
                    <<stType<<" at Jzz/Jff "<<jzz<<"/"<< jff << endreq;
		ALinePar * oldALine =  (*ci).second;
                ALineContainer()->erase(ALineId);
		delete oldALine; oldALine=0;
            }
            else 
            {
                if (log.level()<=MSG::DEBUG) log << MSG::DEBUG<< "               New entry in A-line container for Station Component "
                    <<stType<<" at Jzz/Jff/Job "<<jzz<<"/"<< jff <<"/"<<job<<" --- in the container with key "<< m_mdtIdHelper->show_to_string(ALineId)<< endreq;
            }
            ALineContainer()->insert(std::make_pair(ALineId,ALine));
        }
    }
    log<<MSG::INFO<<"# of A-lines read from the ALineMapContainer in StoreGate is "<<nLines<<endreq;
    log<<MSG::INFO<<"# of deltaTransforms updated according to A-lines         is "<<nUpdates<<endreq;
    log<<MSG::INFO<<"# of entries in the A-lines historical container          is "<<ALineContainer()->size()<<endreq;
    
    return StatusCode::SUCCESS;
}

StatusCode
MuonDetectorManager::updateDeformations(const BLineMapContainer * blineData) const
{
#ifdef TESTBLINES
  {
    for (std::map< std::string, MuonStation *>::const_iterator it(m_MuonStationMap.begin()), end(m_MuonStationMap.end());
	it!=end; ++it) {
      MuonStation* station = it->second;
      station->clearBLineCache();
      BLinePar* BLine = new BLinePar();
      //ine->setParameters(bz,bp,bn,sp,sn,tw,pg,tr,eg,ep,en)
      BLine->setParameters(0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.);
      station->setBline(BLine);
      if (cacheFillingFlag()) station->fillBLineCache();
    }
  }
#endif

    MsgStream log(m_msgSvc, "MGM::MuonDetectorManager::updateDeformations");
    log<<MSG::INFO<<"In updateDeformations()"<<endreq;
    if (!applyMdtDeformations()){
      log<<MSG::INFO<<"Mdt deformations are not enabled - nothing to do here"<<endreq;
      return StatusCode::SUCCESS;
    }

    if (blineData == NULL)
    {
	log<<MSG::WARNING<<"Undefined pointer to temporary B-line container - nothing to do here"<<endreq;
	return StatusCode::SUCCESS;
    }
    if (blineData->size()==0)
    {
	log<<MSG::WARNING<<"Empty temporary B-line container - nothing to do here"<<endreq;
	return StatusCode::SUCCESS;
    }
    else log<<MSG::INFO<<"temporary B-line container with size = "<<blineData->size()<<endreq;
    

    // loop over the container of the updates passed by the MuonAlignmentDbTool
    ciBLineMap cibline = blineData->begin();
    unsigned int nLines = 0;
    unsigned int nUpdates = 0;
    for (; cibline != blineData->end(); cibline++)
    {
        nLines++;
        Identifier BLineId = (*cibline).first;
        BLinePar*  BLine   = (*cibline).second;
        std::string stType = "";
        int jff = 0;
        int jzz = 0;
        int job = 0;
#ifdef TESTBLINES
      //BLine->setParameters(bz,bp,bn,sp,sn,tw,pg,tr,eg,ep,en)
	BLine->setParameters(0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.);
#endif	
	if (mdtDeformationFlag()>999999) 
	  {
	    // first reset everything 
	    BLine->setParameters(0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.);
	    // now apply user choice 
	    int choice = mdtDeformationFlag();
	  //                               BLine->setParameters(bz,bp,bn,sp,sn,tw,pg,tr,eg,ep,en)
 	    if (int(choice%     10)>0    ) BLine->setParameters(0.,0.,0.,BLine->sp(),BLine->sn(),BLine->tw(),0.,0.,BLine->eg(),BLine->ep(),100.);
 	    if (int(choice%    100)>9    ) BLine->setParameters(0.,0.,0.,BLine->sp(),BLine->sn(),BLine->tw(),0.,0.,BLine->eg(),100.,BLine->en());
 	    if (int(choice%   1000)>99   ) BLine->setParameters(0.,0.,0.,BLine->sp(),BLine->sn(),BLine->tw(),0.,0.,100.,BLine->ep(),BLine->en());
 	    if (int(choice%  10000)>999  ) BLine->setParameters(0.,0.,0.,BLine->sp(),BLine->sn(),100.,0.,0.,BLine->eg(),BLine->ep(),BLine->en());
 	    if (int(choice% 100000)>9999 ) BLine->setParameters(0.,0.,0.,BLine->sp(),100.,BLine->tw(),0.,0.,BLine->eg(),BLine->ep(),BLine->en());
 	    if (int(choice%1000000)>99999) BLine->setParameters(0.,0.,0.,100.,BLine->sn(),BLine->tw(),0.,0.,BLine->eg(),BLine->ep(),BLine->en());
	    if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"Testing B-lines: control flag "<<choice<<" hard coding Bline = ( bz="
					    <<BLine->bz()<<" bp="
					    <<BLine->bp()<<" bn="
					    <<BLine->bn()<<" sp="
					    <<BLine->sp()<<" sn="
					    <<BLine->sn()<<" tw="
					    <<BLine->tw()<<" pg="
					    <<BLine->pg()<<" tr="
					    <<BLine->tr()<<" eg="
					    <<BLine->eg()<<" ep="
					    <<BLine->ep()<<" en="
					    <<BLine->en()<<")"<<endreq;
	  }
        BLine->getAmdbId(stType, jff, jzz, job);
	if (stType.substr(0,1)=="T" || stType.substr(0,1)=="C" || (stType.substr(0,3)=="BML" && abs(jzz)==7) ) {
	  if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG
               <<"BLinePar with AmdbId "
               <<stType<<" "<<jzz<<" "<<jff<<" "<<job
               <<" is not a MDT station - skipping"<<endreq;
            continue;
	}
	if (mdtDeformationFlag()==2 && (stType.substr(0,3)=="BEE" || stType.substr(0,1)=="E")) // MDT deformations are requested for Barrel(ASAP) only !!!!
	  {
	    if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" mdtDeformationFlag()=="<<mdtDeformationFlag()<<" stName = "<<stType.substr(0,3)<<" barrel / ec initial = "
					    <<stType.substr(0,1)<<" 	 skipping this b-line"<<endreq;    
	    continue; // MDT deformations are requested for Barrel(ASAP) only !!!!
	  }
	if (mdtDeformationFlag()==3 && (stType.substr(0,3)!="BEE" && stType.substr(0,1)=="B")) 
	  {
	    if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" mdtDeformationFlag()=="<<mdtDeformationFlag()<<" stName = "<<stType.substr(0,3)<<" barrel / ec initial = "
					    <<stType.substr(0,1)<<" 	 skipping this b-line"<<endreq;    
	    continue; // MDT deformations are requested for Endcap(ARAMYS) only !!!!
	  }
	if (mdtDeformationFlag()==0) {
	  if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<" mdtDeformationFlag()==0 skipping this b-line"<<endreq;
	  continue; // should never happen...
	}  
        if (!BLine->isNew())
        {            
            log<<MSG::WARNING
               <<"BLinePar with AmdbId "
               <<stType<<" "<<jzz<<" "<<jff<<" "<<job
               <<" is not new *** skipping"<<endreq;
            continue;
        }
	if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG
	   <<"BLinePar with AmdbId "
	   <<stType<<" "<<jzz<<" "<<jff<<" "<<job<<" is new ID = "<<m_mdtIdHelper->show_to_string(BLineId)<<endreq;
        if (job == 0) 
        {
            MuonStation* thisStation = getMuonStation(stType, jzz, jff);
            if (thisStation)
            {
                if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"Setting deformation parameters for Station "<<stType<<" "<<jzz<<" "<<jff<<" "<<endreq;
                thisStation->clearBLineCache();
                thisStation->setBline(BLine);
                if (cacheFillingFlag()) thisStation->fillBLineCache();
                nUpdates++;
            }
            else
            {                
                log<<MSG::WARNING
                   <<"BLinePar with AmdbId "<<stType<<" "<<jzz<<" "<<jff<<" "<<job
                   <<" *** No MuonStation found \n PLEASE CHECK FOR possible MISMATCHES between alignment constants from COOL and Geometry Layout in use"<<endreq;
		continue;
            }
            // record this B-line in the historical B-line container
            ciBLineMap ci = BLineMapBegin();
            if((ci = BLineContainer()->find(BLineId)) != BLineMapEnd())
            {
                if (log.level()<=MSG::DEBUG) log << MSG::DEBUG<< "Updating existing entry in B-line container for Station "
                    <<stType<<" at Jzz/Jff "<<jzz<<"/"<< jff << endreq;
		BLinePar * oldBLine =  (*ci).second;
                BLineContainer()->erase(BLineId);
		delete oldBLine; oldBLine=0;
            }
            else 
            {
                if (log.level()<=MSG::DEBUG) log << MSG::DEBUG<< "               New entry in B-line container for Station "
                    <<stType<<" at Jzz/Jff "<<jzz<<"/"<< jff <<" --- in the container with key "<< m_mdtIdHelper->show_to_string(BLineId)<< endreq;
            }
            BLineContainer()->insert(std::make_pair(BLineId,BLine));
        }
        else
        {            
            log<<MSG::WARNING
               <<"BLinePar with AmdbId "<<stType<<" "<<jzz<<" "<<jff<<" "<<job<<" has JOB not 0 "<<endreq;
            return StatusCode::FAILURE;
        }
    }
    log<<MSG::INFO<<"# of B-lines read from the ALineMapContainer in StoreGate   is "<<nLines<<endreq;
    log<<MSG::INFO<<"# of deform-Transforms updated according to B-lines         is "<<nUpdates<<endreq;
    log<<MSG::INFO<<"# of entries in the B-lines historical container            is "<<BLineContainer()->size()<<endreq;
    
    return StatusCode::SUCCESS; 
}

void MuonDetectorManager::storeTgcReadoutParams(TgcReadoutParams* x)
{
    m_TgcReadoutParamsVec.push_back(x);
}

StatusCode MuonDetectorManager::initCSCInternalAlignmentMap() const
{
  MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");

  if (!m_useCscIlinesFromGM)
    {
      log<<MSG::INFO<<"Init of CSC I-Lines will be done via Conditions DB"<<endreq;
      m_cscALineContainer = new CscInternalAlignmentMapContainer;

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
              CscInternalAlignmentPar* newILine = new CscInternalAlignmentPar();
              newILine->setAmdbId(stType, jff, jzz, job, wlay);
              if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"No starting I-Lines or reseting them for Station "<<stType<<" Jzz/Jff/Wlay "<<jzz<<"/"<<jff<<"/"<<wlay<<endreq;
              // there is no way to check if the RE already has parameters set - always overwriting them.
              newILine->setParameters(0.,0.,0.,0.,0.,0.);
              newILine->isNew(true);
              Identifier idp = m_cscIdHelper->parentID(ms->getMuonReadoutElement(job)->identify());
              Identifier id = m_cscIdHelper->channelID(idp, 2, wlay, 0, 1);
              if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"<Filling I-Line container with entry for key >"<<m_cscIdHelper->show_to_string(id)<<std::endl;
              m_cscALineContainer->insert(std::make_pair(id,(CscInternalAlignmentPar*)newILine));
          }
      }
      log<<MSG::INFO<<"Init I-Line Container - done - size is respectively " << m_cscALineContainer->size()<<endreq;
    }
  if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"Init CSC I-Line Containers - pointer is <"
     <<(uintptr_t)m_cscALineContainer<<">"<<endreq;

  if (m_cscALineContainer == NULL)
    {
      log<<MSG::INFO<<"No Aline for CSC wire layers loaded "<<endreq;
    }
  else 
    {
      log<<MSG::INFO<<"I-Line for CSC wire layers loaded (Csc Internal Alignment)"<<endreq;
      if (m_useCscIntAlign) log<<"According to configuration they WILL be used "<<endreq; 
      else log<<"According to configuration parameters they WILL BE UPDATED FROM CONDDB "<<endreq;
    }
  return StatusCode::SUCCESS;
}
StatusCode
MuonDetectorManager::updateCSCInternalAlignmentMap(const CscInternalAlignmentMapContainer* ilineData) const
{
    MsgStream log(m_msgSvc, "MGM::MuonDetectorManager::updateCSCInternalAlignment");
    if (ilineData == NULL)
    {
	log<<MSG::WARNING<<"Undefined pointer to temporary CSC I-line container - nothing to do here"<<endreq;
	return StatusCode::SUCCESS;
    }
    if (ilineData->size()==0)
    {
	log<<MSG::WARNING<<"Empty temporary CSC I-line container - nothing to do here"<<endreq;
	return StatusCode::SUCCESS;
    }
    else log<<MSG::INFO<<"temporary CSC I-line container with size = "<<ilineData->size()<<endreq;
    

    // loop over the container of the updates passed by the MuonAlignmentDbTool
    ciCscInternalAlignmentMap ciiline = ilineData->begin();
    unsigned int nLines = 0;
    unsigned int nUpdates = 0;
    for (; ciiline != ilineData->end(); ciiline++)
    {
        nLines++;
	//std::cout<<"Aline n. "<<nLines<<std::endl;
        Identifier ILineId = (*ciiline).first;
        CscInternalAlignmentPar* ILine = (*ciiline).second;
        std::string stType = "";
        int jff = 0;
        int jzz = 0;
        int job = 0;
        int jlay = 0;
        ILine->getAmdbId(stType, jff, jzz, job, jlay);
        if (!ILine->isNew())
        {            
            log<<MSG::WARNING
               <<"CscInternalAlignmentPar with AmdbId "
               <<stType<<" "<<jzz<<" "<<jff<<" "<<job <<" "<<jlay
               <<" is not new *** skipping"<<endreq;
            continue;
        }
	if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG
					<<"CscInternalAlignmentPar with AmdbId "
					<<stType<<" "<<jzz<<" "<<jff<<" "<<job<<" "<<jlay<<" is new ID = "<<m_cscIdHelper->show_to_string(ILineId)<<endreq;
        if (job == 3) 
        {
            MuonStation* thisStation = getMuonStation(stType, jzz, jff);
            if (thisStation)
            {
		//std::cout<<"This station is found "<<std::endl;
                float tras, traz, trat, rots, rotz, rott;
                ILine->getParameters(tras,traz,trat,rots,rotz,rott);
                int choice = CscIlinesFlag();
		if (    choice%10        == 0)           tras = 0.;
		if (int(choice/10)%10    == 0)           rotz = 0.;
		if (int(choice/100)%10   == 0)           rots = 0.;
		if (int(choice/1000)%10  == 0)           trat   = 0.;
		if (int(choice/10000)%10 == 0)           traz   = 0.;
		if (int(choice/100000)%10== 0)           traz   = 0.;
		if (m_controlCscIlines!=111111) ILine->setParameters(tras,traz,trat,rots,rotz,rott);
                if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"Setting CSC I-Lines for Station "<<stType<<" "<<jzz<<" "<<jff<<" "<<job<<" "<<jlay<<" "<<" params are = "<<tras<<" "<<traz<<" "<<trat<<" "<<rots<<" "<<rotz<<" "<<rott<<endreq;
                const CscReadoutElement* CscRE = dynamic_cast<const CscReadoutElement*> (thisStation->getMuonReadoutElement(job));
                if(!CscRE) log<<MSG::ERROR<<"The CSC I-lines container includes stations which are no CSCs! This is impossible." <<endreq;
                else {
                  CscReadoutElement* CscREnoConst = const_cast<CscReadoutElement*>  (CscRE);
                  CscREnoConst->setCscInternalAlignmentPar(ILine);
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
                log<<MSG::WARNING
                   <<"CscInternalAlignmentPar with AmdbId "<<stType<<" "<<jzz<<" "<<jff<<" "<<job<<" "<<jlay
                   <<" *** No MuonStation found \n PLEASE CHECK FOR possible MISMATCHES between alignment constants from COOL and Geometry Layout in use"<<endreq;
		continue;
            }
            ciCscInternalAlignmentMap ci = CscALineMapBegin();
            if((ci = CscInternalAlignmentContainer()->find(ILineId)) != CscALineMapEnd())
            {
                if (log.level()<=MSG::DEBUG) log << MSG::DEBUG<< "Updating extisting entry in CSC I-line container for Station "
						 <<stType<<" at Jzz/Jff/Jlay "<<jzz<<"/"<< jff <<"/"<< jlay<< endreq;
		CscInternalAlignmentPar * oldILine =  (*ci).second;
                CscInternalAlignmentContainer()->erase(ILineId);
		delete oldILine; oldILine=0;
            }
            else 
            {
                if (log.level()<=MSG::DEBUG) log << MSG::DEBUG<< "               New entry in CSC I-line container for Station "
                    <<stType<<" at Jzz/Jff/Jlay "<<jzz<<"/"<< jff<<"/"<< jlay <<" --- in the container with key "<< m_cscIdHelper->show_to_string(ILineId)<< endreq;
            }
            CscInternalAlignmentContainer()->insert(std::make_pair(ILineId,ILine));
        }
        else
        {           
            log<<MSG::ERROR<<"job for CSC I-Lines= "<<job<<" is not 3 => This is not valid."<<endreq;
        }
    }
    log<<MSG::INFO<<"# of CSC I-lines read from the ILineMapContainer in StoreGate is "<<nLines<<endreq;
    log<<MSG::INFO<<"# of deltaTransforms updated according to A-lines             is "<<nUpdates<<endreq;
    log<<MSG::INFO<<"# of entries in the CSC I-lines historical container          is "<<CscInternalAlignmentContainer()->size()<<endreq;
    
    return StatusCode::SUCCESS;
}
StatusCode
MuonDetectorManager::updateAsBuiltParams(const MdtAsBuiltMapContainer* asbuiltData) const
{
    MsgStream log(m_msgSvc, "MGM::MuonDetectorManager::updateAsBuiltParams");
    if (asbuiltData == NULL)
    {
	log<<MSG::WARNING<<"Undefined pointer to temporary As-Built container - nothing to do here"<<endreq;
	return StatusCode::SUCCESS;
    }
    if (asbuiltData->size()==0)
    {
	log<<MSG::WARNING<<"Empty temporary As-Built container - nothing to do here"<<endreq;
	return StatusCode::SUCCESS;
    }
    else log<<MSG::INFO<<"temporary As-Built container with size = "<<asbuiltData->size()<<endreq;

    if (!m_AsBuiltParamsMap) {
      log << MSG::INFO << "Creating the Mdt AsBuilt paramerter map" << endreq; 
      m_AsBuiltParamsMap = new MdtAsBuiltMapContainer();
    }

    // loop over the container of the updates passed by the MuonAlignmentDbTool
    ciMdtAsBuiltMap ciasbuilt = asbuiltData->begin();
    unsigned int nLines = 0;
    unsigned int nUpdates = 0;
    for (; ciasbuilt != asbuiltData->end(); ciasbuilt++)
    {
        nLines++;
	  //std::cout<<"Aline n. "<<nLines<<std::endl;
        Identifier AsBuiltId = (*ciasbuilt).first;
        MdtAsBuiltPar* AsBuiltPar = (*ciasbuilt).second;
        std::string stType = "";
        int jff = 0;
        int jzz = 0;
        int job = 0;
        AsBuiltPar->getAmdbId(stType, jff, jzz, job);
        if (!AsBuiltPar->isNew())
        {            
            if (log.level()<=MSG::DEBUG)
               log<<MSG::DEBUG <<"MdtAsBuiltPar with AmdbId "
               <<stType<<" "<<jzz<<" "<<jff<<" "<<job
               <<" is not new *** skipping"<<endreq;
            continue;
        } else {

          ciMdtAsBuiltMap ci = MdtAsBuiltMapBegin();
          if((ci = MdtAsBuiltContainer()->find(AsBuiltId)) != MdtAsBuiltMapEnd())
          {
            if (log.level()<=MSG::DEBUG) log << MSG::DEBUG<< "Updating extisting entry in AsBuilt container for Station "
                                             <<stType<<" at Jzz/Jff "<<jzz<<"/"<< jff << endreq;
            MdtAsBuiltPar* oldAsBuilt =  (*ci).second;
            MdtAsBuiltContainer()->erase(AsBuiltId);
            delete oldAsBuilt; oldAsBuilt=0;
          } else {
            if (log.level()<=MSG::DEBUG) log << MSG::DEBUG<< "New entry in AsBuilt container for Station "
                    <<stType<<" at Jzz/Jff "<<jzz<<"/"<< jff<<" --- in the container with key "<< m_mdtIdHelper->show_to_string(AsBuiltId)<< endreq;
          }
          MdtAsBuiltContainer()->insert(std::make_pair(AsBuiltId,AsBuiltPar));
        }
        if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG <<"MdtAsBuiltPar with AmdbId "
                 <<stType<<" "<<jzz<<" "<<jff<<" "<<job<<" is new ID = "<<m_mdtIdHelper->show_to_string(AsBuiltId)<<endreq;

        MuonStation* thisStation = getMuonStation(stType, jzz, jff);
        if (thisStation) {
            if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"Setting as-built parameters for Station "<<stType<<" "<<jzz<<" "<<jff<<" "<<endreq;
            thisStation->clearBLineCache();
            thisStation->setMdtAsBuiltParams(AsBuiltPar);
            if (cacheFillingFlag()) thisStation->fillBLineCache();
            nUpdates++;
        } else {
            log<<MSG::WARNING <<"MdtAsBuiltPar with AmdbId "<<stType<<" "<<jzz<<" "<<jff<<" "<<job 
               <<" *** No MuonStation found \n PLEASE CHECK FOR possible MISMATCHES between alignment constants from COOL and Geometry Layout in use"<<endreq;
          continue;
        }

    }
    log<<MSG::INFO<<"# of MDT As-Built read from the MdtAsBuiltMapContainer in StoreGate is "<<nLines<<endreq;
    log<<MSG::INFO<<"# of deltaTransforms updated according to As-Built                  is "<<nUpdates<<endreq;
    log<<MSG::INFO<<"# of entries in the MdtAsBuilt historical container                 is "<<MdtAsBuiltContainer()->size()<<endreq;
    
    return StatusCode::SUCCESS;
}
void MuonDetectorManager::storeCscInternalAlignmentParams(CscInternalAlignmentPar* x)
{

  MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
  if (!m_cscALineContainer) {
    log<<MSG::INFO<<"Creating now the CscInternalAlignmentMapContainer"<<endreq; 
    m_cscALineContainer = new CscInternalAlignmentMapContainer;
  }
  
  std::string stName="XXX";
  int jff = 0;
  int jzz = 0;
  int job = 0;
  int wlayer = 0;
  x->getAmdbId(stName, jff, jzz, job, wlayer);
  // chamberLayer is always 2 => job is always 3 
  int chamberLayer = 2;
  if (job != 3) log<<MSG::WARNING<<"job = "<<job<<" is not 3 => chamberLayer should be 1 - not existing ! setting 2"<<endreq;
  Identifier id = m_cscIdHelper->channelID(stName, jzz, jff, chamberLayer, wlayer, 0, 1);
  
  m_cscALineContainer->insert(std::make_pair(id,x));
  if (log.level()<=MSG::DEBUG) {
    log<<MSG::DEBUG<<"Adding Aline for CSC wire layer: "<<m_cscIdHelper->show_to_string(id)<<endreq; 
    log<<MSG::DEBUG<<"CscInternalAlignmentMapContainer has currently size "<<m_cscALineContainer->size()<<endreq;
  }
}

void MuonDetectorManager::storeMdtAsBuiltParams(MdtAsBuiltPar* params) {
  MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");

  if (!m_AsBuiltParamsMap) {
    log << MSG::INFO << "Creating the Mdt AsBuilt paramerter map" << endreq; 
    m_AsBuiltParamsMap = new MdtAsBuiltMapContainer;
  }

  std::string stName="XXX";
  int jff = 0;
  int jzz = 0;
  int job = 0;
  params->getAmdbId(stName, jff, jzz, job);
  Identifier id = mdtIdHelper()->elementID(stName, jzz, jff);
  if (!id.is_valid()) {
    log << MSG::ERROR << "Invalid MDT identifiers: sta=" << stName << " eta=" << jzz << " phi=" << jff << endreq;
    return;
  }

  ciMdtAsBuiltMap ci = MdtAsBuiltMapBegin();
  if((ci = MdtAsBuiltContainer()->find(id)) != MdtAsBuiltMapEnd())
  {
    if (log.level()<=MSG::DEBUG) log << MSG::DEBUG<< "Updating extisting entry in AsBuilt container for Station "
						 <<stName<<" at Jzz/Jff "<<jzz<<"/"<< jff << endreq;
    MdtAsBuiltPar* oldAsBuilt =  (*ci).second;
    MdtAsBuiltContainer()->erase(id);
    delete oldAsBuilt; oldAsBuilt=0;
  } else {
    if (log.level()<=MSG::DEBUG) log << MSG::DEBUG<< "New entry in AsBuilt container for Station "
                    <<stName<<" at Jzz/Jff "<<jzz<<"/"<< jff<<" --- in the container with key "<< m_mdtIdHelper->show_to_string(id)<< endreq;
  }
  MdtAsBuiltContainer()->insert(std::make_pair(id,params));

  return;
}

MdtAsBuiltPar* MuonDetectorManager::getMdtAsBuiltParams(Identifier id) {
  MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
  if (!MdtAsBuiltContainer()) {
    log << MSG::DEBUG << "No Mdt AsBuilt parameter container available" << endreq;
    return 0;
  }
  iMdtAsBuiltMap iter = MdtAsBuiltContainer()->find(id);
  if (iter == MdtAsBuiltContainer()->end()) {
    log << MSG::DEBUG << "No Mdt AsBuilt parameters for station " << id.getString() 
      << " sta=" << mdtIdHelper()->stationNameString(mdtIdHelper()->stationName(id))
      << " eta=" << mdtIdHelper()->stationEta(id)
      << " phi=" << mdtIdHelper()->stationPhi(id)
      << endreq;
    return 0;
  }
  return iter->second;
}

} // namespace MuonGM
