/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <stdlib.h>
#include "LArDigitization/LArHitEMap.h"

#include "CaloDetDescr/CaloDetDescrElement.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"


//For the buildWindow function
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IService.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/StoreGate.h"
#include "GeneratorObjects/McEventCollection.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"
#include "GaudiKernel/Bootstrap.h"
#include "AthenaKernel/getMessageSvc.h"


LArHitEMap::~LArHitEMap() {}

//bool LArHitEMap::Initialize(std::vector<bool>& flags, bool windows, bool digit)
LArHitEMap::LArHitEMap(const LArOnOffIdMapping* cabling, const CaloCell_ID* cellid, const CaloDetDescrManager* cddMgr, bool digit) :
  m_cabling(cabling),
  m_calocell_id(cellid),
  m_cddMgr(cddMgr) {

  //the last cell of the FCAL is the hash-max for LAr (ignore the Tile part) 
  IdentifierHash fcalCellMin, fcalCellMax;
  cellid->calo_cell_hash_range(CaloCell_ID::LARFCAL,fcalCellMin,fcalCellMax);

  //fill energy map up to fcal-hashmax(= lar-hashmax) 
  m_emap.resize(fcalCellMax);
  if (digit) m_digmap.resize(fcalCellMax,0);
}


// add energy using the calo-cell hash
bool LArHitEMap::AddEnergy(const IdentifierHash index, const float energy, const float time) {
  if(index >= m_emap.size()) return(false);
  m_emap[index].AddHit(energy,time);
  return true; 
}

// add energy using identifier
bool LArHitEMap::AddEnergy(const Identifier cellid, const float energy, const float time) {
  IdentifierHash idHash=m_calocell_id->calo_cell_hash(cellid);
  return AddEnergy(idHash,energy,time);
}

bool LArHitEMap::AddDigit(const LArDigit* digit) {
  const HWIdentifier ch_id = digit->channelID();
  if (m_cabling->isOnlineConnected(ch_id)) {
    Identifier cellid=m_cabling->cnvToIdentifier(ch_id);
    IdentifierHash h=m_calocell_id->calo_cell_hash(cellid);
   
    if (h>=m_digmap.size()) return false ;
    m_digmap[h]=digit;
    return true;
  }
  else 
    return false;
}

int LArHitEMap::GetNbCells(void) const
{
  return m_emap.size()  ;
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
    const McEventCollection* mcCollptr = nullptr;
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
// GU 20-june-2006 use barcode between 10001 and 20000 to select primary particles //AV2020: not sure if it works
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


    if ( etaPart.size() == 0) return true;
    const float pi=2*asin(1.);

//    std::cout << "LArHitEmap:size of map " << m_emap.size() << std::endl;
    for (unsigned int i=0; i < m_emap.size(); i++) 
    {
     LArHitList& theLArHitList = m_emap[i];
     const CaloDetDescrElement* calodde = m_cddMgr->get_element(IdentifierHash(i));
     double eta=calodde->eta();
     double phi=calodde->phi();
     for(unsigned int iPart=0;iPart<etaPart.size();++iPart)
       {
	 double deltaPhi=fmod(phiPart[iPart]-phi+3*pi,2*pi)-pi;
	 double deltaEta=etaPart[iPart]-eta;
	 if( std::fabs(deltaPhi)<dphi/2. &&
	     std::fabs(deltaEta)<deta/2. )
	   {
	     theLArHitList.setInWindows(); 
	     break;
	   }
       }  // loop over particles
    }    // loop over cells
  return true;
}
