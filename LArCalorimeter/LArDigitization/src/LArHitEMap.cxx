/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <stdlib.h>
#include "LArDigitization/LArHitEMap.h"

#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/LArID.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "LArCabling/LArCablingService.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/Algorithm.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/DataHandle.h"
#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/getMessageSvc.h"


LArHitEMap::LArHitEMap(void):
  //m_storeGateSvc(0),
  m_larem_id(0),
  m_larhec_id(0),
  m_larfcal_id(0),
  m_cablingService("LArCablingService"),
  m_initialized(false)
{
        m_ncellem=0;
        m_ncellhec=0;
        m_ncellfcal=0;
	return;
}

LArHitEMap::~LArHitEMap(void)
{
    for(unsigned int it=0; it < m_emap.size(); ++it)
    {
      if (m_emap[it] != 0)  delete m_emap[it];
    }

    return;
}

bool LArHitEMap::initialized()
{
  return m_initialized;
}


bool LArHitEMap::Initialize(std::vector<bool>& flags, bool windows, bool digit)
{
// retrieve CaloIdManager from DetectorStore
  StoreGateSvc* detStore;
  StatusCode sc = Gaudi::svcLocator()->service("DetectorStore", detStore);
  if (sc.isFailure()) {
   MsgStream log(Athena::getMessageSvc(), "LArHitEMap");
   log << MSG::WARNING << "Unable to retrieve DetectorStore" << endmsg;
   return false;
  }
  sc = detStore->retrieve( m_caloIdMgr );
  if (sc.isFailure()) {
   MsgStream log(Athena::getMessageSvc(), "LArHitEMap");
   log << MSG::WARNING << "Unable to retrieve CaloIdMgr in LArHitEMap " << endmsg;
   return false;
  }
  m_larem_id   = m_caloIdMgr->getEM_ID();
  m_larhec_id  = m_caloIdMgr->getHEC_ID();
  m_larfcal_id = m_caloIdMgr->getFCAL_ID();

  if (windows) {
// retrieve CaloDetDescrMgr only if need to access eta-phi cell positions
    sc = detStore->retrieve(m_calodetdescrmgr);
    if (sc.isFailure()) {
       MsgStream log(Athena::getMessageSvc(), "LArHitEMap");
       log << MSG::WARNING << "Unable to retrieve CaloDetDescrMgr in LArHitEMap "  << endmsg;
       return false;
    }
  }

  if (m_cablingService.retrieve().isFailure()) {
   MsgStream log(Athena::getMessageSvc(), "LArHitEMap");
   log << MSG::WARNING << "Unable to retrieve LArCablingService in tools " << endmsg;
   return false;
  }
  

  int size;

  m_ncellem=0;
  m_ncellhec=0;
  m_ncellfcal=0;

  if (flags[EMBARREL_INDEX] || flags[EMENDCAP_INDEX]) {
    m_ncellem=m_larem_id->channel_hash_max();
  }
  if (flags[HADENDCAP_INDEX]) {
    m_ncellhec=m_larhec_id->channel_hash_max();
  }
  if (flags[FORWARD_INDEX]) {
    m_ncellfcal=m_larfcal_id->channel_hash_max();
  }

  MsgStream log(Athena::getMessageSvc(), "LArHitEMap");
  log << MSG::INFO << "LArHitEmap:Number of cells " << m_ncellem << " " << m_ncellhec << " "
            << m_ncellfcal << endmsg;
  size = m_ncellem + m_ncellhec + m_ncellfcal;
  m_emap.resize(size,0);

  if (digit) m_digmap.resize(size,0);

  const CaloDetDescrElement* calodde=0;
  if (flags[EMBARREL_INDEX] || flags[EMENDCAP_INDEX]) {
   for (int i=0;i<m_ncellem;i++){
    IdentifierHash idHash = i;
    Identifier id = m_larem_id->channel_id(idHash);
    bool em_barrel = m_larem_id->is_em_barrel(id);
// skip if cell in barrel and barrel not requested
    if ( (!flags[EMBARREL_INDEX]) && em_barrel) continue;
// skip if cell in endcap and endcap not requested
    if ( (!flags[EMENDCAP_INDEX]) && !em_barrel) continue;
    if (windows) calodde = m_calodetdescrmgr->get_element(id);
    LArHitList* theLArHitList = new LArHitList(id,calodde,m_cablingService);
    m_emap[idHash] = theLArHitList;
   }
  }

  if (flags[HADENDCAP_INDEX]) {
   for (int i=0;i<m_ncellhec;i++){
    IdentifierHash idHash = i;
    Identifier id = m_larhec_id->channel_id(idHash);
    if (windows) calodde = m_calodetdescrmgr->get_element(id);
    LArHitList* theLArHitList = new LArHitList(id,calodde,m_cablingService);
    m_emap[idHash+m_ncellem] = theLArHitList;
   }
  }

  if (flags[FORWARD_INDEX]) {
   for (int i=0;i<m_ncellfcal;i++){
    IdentifierHash idHash = i;
    Identifier id = m_larfcal_id->channel_id(idHash);
    if (windows) calodde = m_calodetdescrmgr->get_element(id);
    LArHitList* theLArHitList = new LArHitList(id,calodde,m_cablingService);
    m_emap[idHash+m_ncellem+m_ncellhec] = theLArHitList;
   }
  }

  m_initialized=true; 

  return true ;
}


void LArHitEMap::EnergyReset(void)
{

  for(unsigned int it=0; it < m_emap.size(); ++it)
  {
    if (m_emap[it] != 0)  m_emap[it]->Reset();
  }
  return;
}

void LArHitEMap::DigitReset()
{
  for (unsigned int i=0;i<m_digmap.size();i++)  m_digmap[i]=NULL;
  return;
}


// add energy using directly internal index
bool LArHitEMap::AddEnergy(unsigned int index,float energy,float time)
{
  if(index > m_emap.size()) return(false);
  if(m_emap[index] == 0) {
    // std::cout << "LArHitEmap:hit found for index " << index << "but no calodd" << std::endl;
    return(true);  // case of non existing cells
  }
  m_emap[index]->AddHit(energy,time);
  return(true); 
}

// add energy using identifier
bool LArHitEMap::AddEnergy(const Identifier & cellid, float energy, float time)
{
  IdentifierHash idHash;
  int offset=-1;
  if(m_larem_id->is_lar_em(cellid)) 
  {
    offset=0;
    idHash=m_larem_id->channel_hash(cellid);
  }
  else if(m_larhec_id->is_lar_hec(cellid))
  {
    offset = m_ncellem;
    idHash=m_larhec_id->channel_hash(cellid);
  }
  else if(m_larfcal_id->is_lar_fcal(cellid))
  {
    offset = m_ncellem+m_ncellhec;
    idHash=m_larfcal_id->channel_hash(cellid);
  }
  if (offset <0) return(false);
  unsigned int index = (int)idHash+offset;
  if(index >= m_emap.size()) return(false);
  if(m_emap[index] == 0) {
    return(true);  // case of non existing cells
  }
  m_emap[index]->AddHit(energy,time);
  return true;
}

bool LArHitEMap::AddDigit(const LArDigit* digit)
{
 HWIdentifier ch_id = digit->channelID();
 if (m_cablingService->isOnlineConnected(ch_id)) {
   Identifier cellid = m_cablingService->cnvToIdentifier(ch_id);
   IdentifierHash idHash; 
   int offset=-1;
   if(m_larem_id->is_lar_em(cellid) && m_ncellem>0 ) 
   {
    offset=0;
    idHash=m_larem_id->channel_hash(cellid);
   }
   else if(m_larhec_id->is_lar_hec(cellid) && m_ncellhec>0 )
   {
    offset = m_ncellem;
    idHash=m_larhec_id->channel_hash(cellid);
   }
   else if(m_larfcal_id->is_lar_fcal(cellid) && m_ncellfcal>0)
   {
    offset = m_ncellem+m_ncellhec;
    idHash=m_larfcal_id->channel_hash(cellid);
   }
   if (offset <0) return false ;
   unsigned int index = (int)idHash+offset;
   if(index >= m_digmap.size()) return false ;
   m_digmap[index]=digit;
   return true;
 }
 else {
  return false;
 }
}


int LArHitEMap::GetNbCells(void)
{
  return m_emap.size()  ;
}

int LArHitEMap::find(Identifier cellid)
{

  IdentifierHash idHash;
  int offset=-1;
  if(m_larem_id->is_lar_em(cellid))
  {
    offset=0;
    idHash=m_larem_id->channel_hash(cellid);
  }
  else if(m_larhec_id->is_lar_hec(cellid))
  {
    offset = m_ncellem;
    idHash=m_larhec_id->channel_hash(cellid);
  }
  else if(m_larfcal_id->is_lar_fcal(cellid))
  {
    offset = m_ncellem+m_ncellhec;
    idHash=m_larfcal_id->channel_hash(cellid);
  }
  if (offset<0) return(-1);
  unsigned int index = (int)idHash+offset;
  if(m_emap[index] == 0) { return -1;}
  else {return index;}
}

std::vector<std::pair<float,float> > * LArHitEMap::GetTimeE(unsigned int index)
{
 if (index > m_emap.size()) return(0);
 if (m_emap[index] != 0) {
  LArHitList* data = m_emap[index];
  std::vector<std::pair<float,float> > * TimeE = data->getData();
  return(TimeE);
 }
 else {return(0);}
}

bool LArHitEMap::BuildWindows(float deta,float dphi, float ptmin)
{
// get list of particles
    std::vector<double> phiPart;
    std::vector<double> etaPart;

//    std::cout << " in BuildWindows " << deta << dphi << ptmin << std::endl;
    etaPart.clear();
    phiPart.clear();
    //get pointer of MC collection
    StoreGateSvc* SgSvc = StoreGate::pointer();
    const DataHandle<McEventCollection> mcCollptr;
    if ( SgSvc->retrieve(mcCollptr,"").isFailure() ) {
       MsgStream log(Athena::getMessageSvc(), "LArHitEMap");
       log << MSG::WARNING 
           << "LArHitEMap:cannot retrieve McEventCollection  (keyless)"
           << endmsg;
        return false;
    }
    McEventCollection::const_iterator itr;
//    std::cout << " start loop over particles " << std::endl;
    for (itr = mcCollptr->begin(); itr!=mcCollptr->end(); ++itr) {
      HepMC::GenEvent::particle_const_iterator itrPart;
      for (itrPart = (*itr)->particles_begin(); itrPart!=(*itr)->particles_end(); ++itrPart )
      {
         HepMC::GenParticle *part=*itrPart;
         //works only for photons(22) and electrons(11) primary particle from Geant (status>1000)
         // with pt>5 GeV
// GU 20-june-2006 use barcode between 10001 and 20000 to select primary particles
//         std::cout << " pdg_id,status,barcode,pt " << part->pdg_id() << " " 
//           <<  part->status() << " " << part->barcode() << " " <<
//                part->momentum().perp() << std::endl;
         if(   (part->pdg_id()==22 || abs(part->pdg_id())==11 || part->pdg_id()==111) 
            && part->barcode()>10000 && part->barcode()<20000
            && part->momentum().perp()> ptmin)
         {
//          std::cout << "good particle found ! " << part->pdg_id() << std::endl;
          etaPart.push_back(part->momentum().pseudoRapidity());
          phiPart.push_back(part->momentum().phi());
         }
      }
    }

//    std::cout << " Number of true particles " << etaPart.size() << std::endl;
//    for(unsigned int iPart=0;iPart<etaPart.size();++iPart)
//    {
//     std::cout << "eta,phi " << etaPart[iPart] << " " << phiPart[iPart] << std::endl;
//    }

    if ( etaPart.size() == 0) return true;
    const float pi=2*asin(1.);

//    std::cout << "LArHitEmap:size of map " << m_emap.size() << std::endl;
    for (unsigned int i=0; i < m_emap.size(); i++) 
    {
     LArHitList* theLArHitList = m_emap[i];
     if (theLArHitList != 0 ) {
       const CaloDetDescrElement* calodde = theLArHitList->getCaloDDE();
       double eta=calodde->eta();
       double phi=calodde->phi();
       for(unsigned int iPart=0;iPart<etaPart.size();++iPart)
       {
        double deltaPhi=fmod(phiPart[iPart]-phi+3*pi,2*pi)-pi;
        double deltaEta=etaPart[iPart]-eta;
        if( std::fabs(deltaPhi)<dphi/2. &&
          std::fabs(deltaEta)<deta/2. )
        {
          theLArHitList->setInWindows(); 
          break;
        }
       }  // loop over particles
     }    // hitlist >0
    }    // loop over cells
  return true;
}
