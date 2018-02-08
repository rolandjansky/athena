/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileDetDescr/TileDddbManager.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include "GaudiKernel/MsgStream.h"

#include <iostream>
#include <stdexcept>

TileDddbManager::TileDddbManager(IRDBAccessSvc* access,
		                 std::string    version_tag,
				 std::string    version_node,
                                 MsgStream *    log)
  : m_n_cuts(0)
  , m_n_saddle(0)
  , m_currentTileGlob(0)
  , m_currentTileMod(0)
  , m_currentSection(0)
  , m_currentGird(0)
  , m_currentScin(0)
  , m_currentTifg(0)
  , m_currentTicg(0)
  , m_currentTicl(0)
  , m_currentCuts(0)
  , m_currentSaddle(0)
  , m_currentTiclInd(-1)
  , m_tag(version_tag)
  , m_node(version_node)
{
  m_verbose = (log->level()<=MSG::VERBOSE);
 
  (*log) << MSG::INFO << "TileDddbManager: m_tag = " << m_tag << endreq;

  m_tiglob = access->getRecordsetPtr("TileGlobals",m_tag,m_node);
  m_n_tiglob = m_tiglob->size();
  (*log) << MSG::INFO << "TileDddbManager: n_tiglob = " << m_n_tiglob << endreq;
 
  if(access->getChildTag("TileModule",m_tag,m_node)!="") {
   m_timod = access->getRecordsetPtr("TileModule",m_tag,m_node);
  } else {
   m_timod = access->getRecordsetPtr("TileModules",m_tag,m_node);
  }
  m_n_timod = m_timod->size();
  (*log) << MSG::INFO << "TileDddbManager: n_timod = " << m_n_timod << endreq;
  
  if (access->getChildTag("TileCuts",m_tag,m_node)!="") 
   { m_buildCuts = true;
     m_cuts = access->getRecordsetPtr("TileCuts",m_tag,m_node);
     m_n_cuts = m_cuts->size();
     (*log) << MSG::INFO << "TileDddbManager: n_cuts = " << m_n_cuts << endreq;

   } else {
     m_buildCuts = false;
   }

  if (access->getChildTag("TileSaddleSup",m_tag,m_node)!="") 
   { m_buildSaddle = true;
     m_saddle = access->getRecordsetPtr("TileSaddleSup",m_tag,m_node);
     m_n_saddle = m_saddle->size();
     (*log) << MSG::INFO << "TileDddbManager: n_saddle = " << m_n_saddle << endreq;

   } else {
     m_buildSaddle = false;
   }

  m_tile = access->getRecordsetPtr("TILE",m_tag,m_node);

  m_tilb = access->getRecordsetPtr("TILB",m_tag,m_node);
  m_n_tilb = m_tilb->size();
  (*log) << MSG::INFO << "TileDddbManager: n_tilb = " << m_n_tilb << endreq;

  m_tigr = access->getRecordsetPtr("TIGR",m_tag,m_node);
  m_n_tigr = m_tigr->size();

  m_scnt = access->getRecordsetPtr("SCNT",m_tag,m_node);
  m_n_scnt = m_scnt->size();

  m_tifg = access->getRecordsetPtr("TIFG",m_tag,m_node);
  m_n_tifg = m_tifg->size();

  m_ticg = access->getRecordsetPtr("TICG",m_tag,m_node);
  m_n_ticg = m_ticg->size();

  m_ticl = access->getRecordsetPtr("TICL",m_tag,m_node);
  m_n_ticl = m_ticl->size();

  m_tileSwitches = access->getRecordsetPtr("TileSwitches",m_tag,m_node);
  m_n_tileSwitches = m_tileSwitches->size();
  (*log) << MSG::INFO << "TileDddbManager: n_tileSwitches = " << m_n_tileSwitches << endreq;


  m_EnvNum = 0;
  m_EnvSize = 0;
  m_EnvBegin = 0;
  m_modTypes.clear();
}

// ---------- T I L E -------------

int TileDddbManager::TILEnmodul() const
{
  return (*m_tile)[0]->getInt("NMODUL");
}

double TileDddbManager::TILErmim() const
{
  return (*m_tile)[0]->getDouble("RMIM");
}

double TileDddbManager::TILErmam() const
{
  return (*m_tile)[0]->getDouble("RMAM");
}

double TileDddbManager::TILEzmam() const
{
  return (*m_tile)[0]->getDouble("ZMAM");
}

double TileDddbManager::TILErmin() const
{
  return (*m_tile)[0]->getDouble("RMIN");
}

double TileDddbManager::TILErmax() const
{
  return (*m_tile)[0]->getDouble("RMAX");
}

double TileDddbManager::TILEzshift() const
{
  return (*m_tile)[0]->getDouble("ZSHIFT");
}

unsigned int TileDddbManager::GetNumTilb() const
{ 
  return m_n_tilb;
}

//------------ TILE GLOBALS--------------------
int TileDddbManager::GetNumberOfEnv() const
{  
  return m_n_tiglob;
}

int TileDddbManager::GetEnvType() const
{
  if (m_currentTileGlob) {
    return m_currentTileGlob->getInt("PART");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Envelope not set, returning -999\n";
    return -999;
  }  
}

int TileDddbManager::SetCurrentEnvByType(unsigned int envelopeT)
{
  m_currentTileGlob = 0;
  m_currentTileMod = 0;
  m_currentSection = 0;
  m_modTypes.clear();

  unsigned int ind2 = 0;
  while (((*m_tiglob)[ind2]->getInt("PART") != static_cast<int>(envelopeT)) && (++ind2 < m_n_tiglob))
    {
    }
  if (ind2 < m_n_tiglob) {
    m_currentTileGlob = (*m_tiglob)[ind2];
    m_EnvSize = m_currentTileGlob->getInt("NMODULES");
    unsigned int ind3 = 0;
    while (((*m_timod)[ind3]->getInt("PART") != static_cast<int>(envelopeT)) && (++ind3 < m_n_timod))
      {
      }
    if (ind3 < m_n_timod) {
        m_EnvBegin = ind3;
        return 1;
    }
    else {
      m_EnvSize = 0;
      m_EnvBegin = 0;
      std::cerr << "TileDddbManager couldn't return begin of modules\n";
      return 0;
    }
  } else {
    m_currentTileGlob = 0;
    std::cerr << "TileDddbManager couldn't return Envelope\n";
    return 0;
  } 
}

int TileDddbManager::SetCurrentEnvByIndex(unsigned int envelopeI)
{
  m_currentTileGlob = 0;
  m_currentTileMod = 0;
  m_currentSection = 0;
  m_modTypes.clear();

  if (envelopeI < m_n_tiglob) {
    m_currentTileGlob = (*m_tiglob)[envelopeI];
    m_EnvNum = m_currentTileGlob->getInt("PART");
    m_EnvSize = m_currentTileGlob->getInt("NMODULES");
    unsigned int ind1 = 0;
//    while (((*m_timod)[ind1]->getInt("PART") != static_cast<int>(m_EnvNum)) && (++ind1 < m_n_timod)) 
//      {
//      }
    // FIXME: temporary fix for wrong PART in modules table
    for ( ; ind1 < m_n_timod; ++ind1) {
      int part = (*m_timod)[ind1]->getInt("PART");
      if ( m_tag == "TileCal-02" ) {
        if (part == 3) part = 5;
        else if (part == 2) part = 3;
      }
      if ( part == static_cast<int>(m_EnvNum)) break;
    }
    if (ind1 < m_n_timod) {
      m_EnvBegin = ind1;
      return 1;
    }
    else {
      m_EnvSize = 0;
      m_EnvBegin = 0;
      std::cerr << "TileDddbManager couldn't return begin of modules\n";
      return 0;
    }
  } else {
    m_currentTileGlob = 0;
    std::cerr << "TileDddbManager couldn't return Envelope\n";
    return 0;
  }
}

 
int TileDddbManager::GetEnvSide() const
{
  if (m_currentTileGlob) {
    return m_currentTileGlob->getInt("SIDE");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999\n";
    return -999;
  }  
}

int TileDddbManager::GetEnvNModules() const
{
  if (m_currentTileGlob) {
    return m_currentTileGlob->getInt("NMODULES");
    //sb return 1;
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Envelope not set, returning -999\n";
    return -999;
  } 
}

double TileDddbManager::GetEnvRin() const
{
  if (m_currentTileGlob) {
    return m_currentTileGlob->getDouble("RIN");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Envelope not set, returning -999\n";
    return -999;
  } 
}

double TileDddbManager::GetEnvRout() const
{
  if (m_currentTileGlob) {
    return m_currentTileGlob->getDouble("ROUT");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Envelope not set, returning -999\n";
    return -999;
  } 
}

double TileDddbManager::GetEnvZLength() const
{
  if (m_currentTileGlob) {
//    return m_currentTileGlob->getDouble("ZLENGTH");
    // FIXME: temporary correction for finger length
    //        all fingers for the testbeam are "small fingers"
    double len =m_currentTileGlob->getDouble("ZLENGTH");
    if (len > 564 + 37.7*2 && m_tag == "TileCal-02") {
      len = 564 + 37.7*2;
    }
    return len;
  }  
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Envelope not set, returning -999\n";
    return -999;
  } 
}

double TileDddbManager::GetEnvDPhi() const
{
  if (m_currentTileGlob) {
//    return m_currentTileGlob->getDouble("DPHI");
    // FIXME: temporary fix for wrong dphi in table
    double dphi = m_currentTileGlob->getDouble("DPHI");
    if ( dphi != 0 && m_tag == "TileCal-02") {
      dphi -= 360.0/64.0/2.;
    }
    return dphi;
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Envelope not set, returning -999\n";
    return -999;
  } 
}

double TileDddbManager::GetEnvDZ() const
{
  if (m_currentTileGlob) {
//    return m_currentTileGlob->getDouble("DZ");
    // FIXME: temporary correction for finger length
    //        all fingers for the testbeam are "small fingers"
    double dz = m_currentTileGlob->getDouble("DZ");
    if ( dz > 20 && m_tag == "TileCal-02") {
      dz += 43.05 - 37.7;
    }
    return dz;
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Envelope not set, returning -999\n";
    return -999;
  } 
}

double TileDddbManager::GetEnvDX() const
{
  if (m_currentTileGlob) {
    return m_currentTileGlob->getDouble("DX");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Envelope not set, returning -999\n";
    return -999;
  } 
}

double TileDddbManager::GetEnvDY() const
{
  if (m_currentTileGlob) {
    return m_currentTileGlob->getDouble("DY");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Envelope not set, returning -999\n";
    return -999;
  } 
}

double TileDddbManager::GetEnvDPsi() const
{
  if (m_currentTileGlob) {
    return m_currentTileGlob->getDouble("DPSI");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Envelope not set, returning -999\n";
    return -999;
  } 
}

double TileDddbManager::GetEnvDTheta() const
{
  if (m_currentTileGlob) {
    return m_currentTileGlob->getDouble("DTHETA");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Envelope not set, returning -999\n";
    return -999;
  } 
}

double TileDddbManager::GetEnvZShift() const
{
  if(m_currentTileGlob) {
    return m_currentTileGlob->getDouble("ZSHIFT");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Envelope not set, returning -999\n";
    return -999;
  } 
}


//------------ TILE MODULES -------------------
int TileDddbManager::SetCurrentModuleByIndex(unsigned int Id)
{
  m_currentTileMod = 0;
  m_currentSection = 0;
  m_modTypes.clear();
  
  if (m_currentTileGlob) {
  //Modules enumerated from zero, so SetCurrentModuleByIndex(0) gives the first module in the Envelope
    if(Id >= m_EnvSize) { 
      std::cerr <<"\nTileDddbManager: module number is out of range\n";
      return 0;
    }
    else {
      m_currentTileMod = (*m_timod)[Id + m_EnvBegin];
      FillModTypes();
      return 1;
    }
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Envelope not set, returning -999\n";
    return -999;
  } 
}

int TileDddbManager::SetCurrentModuleByNumber(unsigned int Number)
{
  m_currentTileMod = 0;
  m_currentSection = 0;
  m_modTypes.clear();

  if (m_currentTileGlob) {
    //Input here number from column "MODNUM"
    unsigned int ind4 = m_EnvBegin;
    unsigned int EnvEnd = m_EnvBegin + m_EnvSize;
    while (((*m_timod)[ind4]->getInt("MODNUM") != static_cast<int>(Number))&&(++ind4 < EnvEnd))
      {
      }
    if(ind4 < EnvEnd){
      m_currentTileMod = (*m_timod)[ind4];
      FillModTypes();
      return 1;
    }
    else{
      m_currentTileMod = 0;
      std::cerr << "TileDddbManager couldn't return Module\n";
      return 0;
    }
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Envelope not set, returning -999\n";
    return -999;
  } 
}

int TileDddbManager::GetModNumber() const
{
  if (m_currentTileMod) {
    return m_currentTileMod->getInt("MODNUM");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Module not set, returning -999\n";
    return -999;
  }  
}

int TileDddbManager::GetModType() const
{
  if(m_currentTileMod) {
    return m_currentTileMod->getInt("TYP");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Module not set, returning -999\n";
    return -999;
  }
}

double TileDddbManager::GetModDX() const
{
  if (m_currentTileMod) {
    return m_currentTileMod->getDouble("DX");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Module not set, returning -999\n";
    return -999;
  }
}

double TileDddbManager::GetModDY() const
{
  if (m_currentTileMod) {
    return m_currentTileMod->getDouble("DY");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Module not set, returning -999\n";
    return -999;
  }
}

double TileDddbManager::GetModDZ() const
{
  if (m_currentTileMod) {
    return m_currentTileMod->getDouble("DZ");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Module not set, returning -999\n";
    return -999;
  }
}

double TileDddbManager::GetModDPhi() const
{
  if (m_currentTileMod) {
    return m_currentTileMod->getDouble("DPHI");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Module not set, returning -999\n";
    return -999;
  }
}

double TileDddbManager::GetModDPsi() const
{
  if (m_currentTileMod) {
    return m_currentTileMod->getDouble("DPSI");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Module not set, returning -999\n";
    return -999;
  }
}

double TileDddbManager::GetModDTheta() const
{
  if (m_currentTileMod) {
    return m_currentTileMod->getDouble("DTHETA");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Module not set, returning -999\n";
    return -999;
  }
}


//------------ TILE MODULE TYPES -------------------
int TileDddbManager::SetCurrentTypeByIndex(unsigned int Id)
{
  if (Id < m_modTypes.size()) {
    return SetCurrentSection(m_modTypes[Id]);
  } else {
    std::cerr << "ERROR: type index (" << Id << ") is out of range (" << m_modTypes.size() << ")" << std::endl;
    m_currentSection = 0;
    return 0;
  }
}

int TileDddbManager::SetCurrentTypeByNumber(unsigned int Number)
{
  return SetCurrentSection(Number);
}

unsigned int TileDddbManager::GetModType(unsigned int Id) const
{
  if (Id < m_modTypes.size()) {
    return m_modTypes[Id];
  } else {
    return 999;
  }
}

unsigned int TileDddbManager::GetModTypeSize() const
{
  return m_modTypes.size();
}

void TileDddbManager::FillModTypes()
{
  m_modTypes.clear();

  int fullType = GetModType();
  
  while (fullType > 0) {
    unsigned int subType = fullType%100;
    m_modTypes.push_back(subType);
    fullType /= 100;
  }
  SetCurrentTypeByIndex(0);
}


// ------------ S E C T I O N ------------------


int TileDddbManager::SetCurrentSectionByIndex(unsigned int IdSection)
{
  if (IdSection < m_n_tilb) {
    m_currentSection = (*m_tilb)[IdSection];
    return 1;
  }
  else {
    std::cerr << "ERROR: section index (" << IdSection << ") is out of range (" << m_n_tilb << ")" << std::endl;
    m_currentSection = 0;
    return 0;
  }
}

int TileDddbManager::SetCurrentSectionByNumber(unsigned int section)
{
  return SetCurrentSection(section);
}

int TileDddbManager::SetCurrentSection(unsigned int section, bool print)
{
  unsigned int ind = 0;

  while (((*m_tilb)[ind]->getInt("SECTION") != static_cast<int>(section)) && (++ind < m_n_tilb)) 
  {
  }

  if(ind >= m_n_tilb) {
    if (print) std::cerr << "\nTileDddbManager::SetCurrentSection ERROR! Unable to find the section\n";
    m_currentSection = 0;
    return 0;
  }
  else {
    m_currentSection = (*m_tilb)[ind];
    return 1;
  }
}

int TileDddbManager::TILBsection() const
{
  if (m_currentSection) {
    return m_currentSection->getInt("SECTION");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999\n";
    return -999;
  }
}

int TileDddbManager::TILBnperiod() const
{
  if (m_currentSection) {
    return m_currentSection->getInt("NPERIOD");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999\n";
    return -999;
  }
}

int TileDddbManager::TILBnmodul() const
{
  if (m_currentSection) {
    return m_currentSection->getInt("NMODUL");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999\n";
    return -999;
  }
}

double TileDddbManager::TILBrmin() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("RMIN");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBrmax() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("RMAX");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBrminimal() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("RMINIMAL");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBrmaximal() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("RMAXIMAL");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBdzperio() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("DZPERIO");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBdrfront() const
{
  if(m_currentSection) {
    return m_currentSection->getDouble("DRFRONT");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBdzend() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("DZEND");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBflangex() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("FLANGEX");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBflangey() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("FLANGEY");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBzoffset() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("ZOFFSET");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBphigap() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("PHIGAP");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBisci_st() const
{
  if (m_currentSection) {
    return m_currentSection->getInt("ISCI_ST");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBdzmodul() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("DZMODUL");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBdzmast() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("DZMAST");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBdzspac() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("DZSPAC");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBdzend1() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("DZEND1");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBdzend2() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("DZEND2");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

int TileDddbManager::TILBngirder() const
{
  if (m_currentSection) {
    return m_currentSection->getInt("NGIRDER");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999\n";
    return -999;
  }
}

int TileDddbManager::TILBnscin() const
{
  if (m_currentSection) {
    return m_currentSection->getInt("NSCIN");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999\n";
    return -999;
  }
}

int TileDddbManager::TILBfingpattern() const
{
  if (m_currentSection) {
    return m_currentSection->getInt("FINGPATTERN");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999\n";
    return -999;
  }
}

int TileDddbManager::TILBnpercutpos() const
{
  if (m_currentSection) {
    return m_currentSection->getInt("NPERCUTPOS");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999\n";
    return -999;
  }
}

int TileDddbManager::TILBnpercutneg() const
{
  if (m_currentSection) {
    return m_currentSection->getInt("NPERCUTNEG");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999\n";
    return -999;
  }
}


int TileDddbManager::TILBcurscint() const
{
  if(m_currentSection) {
    if(m_currentSection->isFieldNull("CURSCINT"))
      return 0;
    else
      return m_currentSection->getInt("CURSCINT");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999\n";
    return -999;
  }
}

double TileDddbManager::TILBdzgir() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("DZGIR");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999\n";
    return -999;
  }
}

// -------------------- S C I N T I L L A T O R ---------------------
int TileDddbManager::SetCurrentScin(int item)
{
  unsigned int ind = 0;

  while (((*m_scnt)[ind]->getInt("ITEM") != item) && (++ind < m_n_scnt)) 
  {
  }

  if(ind >= m_n_scnt)
  {
    std::cerr << "\nTileDddbManager::SetCurrentScin ERROR! Unable to find the scintillator, item = " << item << "\n";
    return 0;
  }
  else
  {
    m_currentScin = (*m_scnt)[ind];
    return 1;
  }
}

int TileDddbManager::SCNTitem() const
{
  if(m_currentScin)
    return m_currentScin->getInt("ITEM");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current Scintillator not set, returning -999\n";
    return -999;
  }
}

double TileDddbManager::SCNTdr() const
{
  if(m_currentScin)
    return m_currentScin->getDouble("DR");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current Scintillator not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::SCNTrc() const
{
  if(m_currentScin)
    return m_currentScin->getDouble("RC");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current Scintillator not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::SCNTzp() const
{
  if(m_currentScin)
    return m_currentScin->getDouble("ZP");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current Scintillator not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::SCNTdrw() const
{
  if(m_currentScin)
    return m_currentScin->getDouble("DRW");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current Scintillator not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::SCNTdt() const
{
  if(m_currentScin)
    return m_currentScin->getDouble("DT");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current Scintillator not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::SCNTdtw() const
{
  if(m_currentScin)
    return m_currentScin->getDouble("DTW");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current Scintillator not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::SCNTdphi() const
{
  if(m_currentScin)
    return m_currentScin->getDouble("DPHI");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current Scintillator not set, returning -999.9\n";
    return -999.9;
  }
}


// -------------------- Extended Barrel C U T S ---------------------

int TileDddbManager::SetCurrentCuts(std::string input)
{
  if (m_buildCuts) 
    {std::string name(input);
     unsigned int ind = 0;

     while (((*m_cuts)[ind]->getString("VOLUME") != name) && (++ind < m_n_cuts)) 
     { 
     }

     if(ind >= m_n_cuts)
     { std::cerr << "\nTileDddbManager::SetCuts ERROR! Unable to find the m_currentCuts volume name = " << name << "\n";
       return 0;
     }
     else
     { m_currentCuts = (*m_cuts)[ind];
       return 1;
     }
    }
   std::cout <<"\nTileDddbManager::SetCuts WARNING Ext.Barrel Cuts unavailable in touched DB"<<"\n";
   return 1;
} 

bool TileDddbManager::BoolCuts()
{ if (m_buildCuts) 
  { return m_buildCuts;
  } else
  { //std::cerr << "TileDddbManager::BoolCuts Unable " << "\n";
    return false;
  }
}

std::string TileDddbManager::CutsName() const
{std::string output ="-999.9";
   if (m_currentCuts)
     return m_currentCuts->getString("VOLUME");
   else
   { std::cerr << "\nTileDddbManager ERROR! Current VOLUME dimension not set, returning -999.9\n";
     return output;
   } 
}

double TileDddbManager::CutsDX1() const
{
  if (m_currentCuts)
    return (double)m_currentCuts->getFloat("DX1");
  else
  { std::cerr << "\nTileDddbManager ERROR! Current dX1 dimension not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::CutsDX2() const
{
  if (m_currentCuts)
    return (double)m_currentCuts->getFloat("DX2");
  else
  { std::cerr << "\nTileDddbManager ERROR! Current dX2 dimension not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::CutsDY1() const
{
  if (m_currentCuts)
    return (double)m_currentCuts->getFloat("DY1");
  else
  { std::cerr << "\nTileDddbManager ERROR! Current dY1 dimension not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::CutsDY2() const
{
  if (m_currentCuts)
    return (double)m_currentCuts->getFloat("DY2");
  else
  { std::cerr << "\nTileDddbManager ERROR! Current dY2 dimension not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::CutsDZ1() const
{
  if (m_currentCuts)
    return (double)m_currentCuts->getFloat("DZ1");
  else
  { std::cerr << "\nTileDddbManager ERROR! Current dZ1 dimension not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::CutsXpos() const
{
  if (m_currentCuts)
    return (double)m_currentCuts->getFloat("XPOS");
  else
  { std::cerr << "\nTileDddbManager ERROR! Current Xpos dimension not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::CutsYpos() const
{
  if (m_currentCuts)
    return (double)m_currentCuts->getFloat("YPOS");
  else
  { std::cerr << "\nTileDddbManager ERROR! Current Ypos dimension not set, returning -999.9\n";
    return -999.9;
  }
}
 
// -------------------- SADDLE SUPPORT ---------------------

int TileDddbManager::SetCurrentSaddle(unsigned int index)
{
  if (m_buildSaddle) 
   { if(index >= m_n_saddle)
     { return 0;
     }
      else
     { m_currentSaddle = (*m_saddle)[index];
       return 1;
     }
   }
   std::cout <<"\nTileDddbManager::SetCurrentSaddle WARNING Saddle unavailable in touched DB"<<"\n";
   return 1;
}

bool TileDddbManager::BoolSaddle()
{ if (m_buildSaddle) 
  { return m_buildSaddle;
  } else
  { //std::cerr << "TileDddbManager::BoolSaddle Unable " << "\n";
    return false;
  }
}

double TileDddbManager::DzSaddleSupport() const
{
  if (m_currentSaddle)
    return (double)m_currentSaddle->getFloat("DZSADDLESUPPORT");
  else
  { std::cerr << "\nTileDddbManager ERROR! DzSaddleSupport dimension not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::RadiusSaddle() const
{
  if (m_currentSaddle)
    return (double)m_currentSaddle->getFloat("RADIUSSADDLE");
  else
  { std::cerr << "\nTileDddbManager ERROR! RadiusSaddle dimension not set, returning -999.9\n";
    return -999.9;
  }
}

// -------------------- G I R D E R  ---------------------
int TileDddbManager::SetCurrentGird(int item)
{
  unsigned int ind = 0;

  while (((*m_tigr)[ind]->getInt("ITEM") != item) && (++ind < m_n_tigr)) 
  {
  }

  if(ind >= m_n_tigr)
  {
    //std::cerr << "\nTileDddbManager::SetCurrentGird ERROR! Unable to find the girder element,item= " << item << "\n";
    return 0;
  }
  else
  {
    m_currentGird = (*m_tigr)[ind];
    return 1;
  }
}

int TileDddbManager::TIGRitem() const
{
  if(m_currentGird)
    return m_currentGird->getInt("ITEM");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current Girder element not set, returning -999\n";
    return -999;
  }
}

int TileDddbManager::TIGRmaterial() const
{
  if(m_currentGird)
    return m_currentGird->getInt("MATERIAL");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current Girder element not set, returning -999\n";
    return -999;
  }
}

double TileDddbManager::TIGRrc() const
{
  if(m_currentGird)
    return m_currentGird->getDouble("RC");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current Girder element not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TIGRdr() const
{
  if(m_currentGird)
    return m_currentGird->getDouble("DR");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current Girder element not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TIGRdw() const
{
  if(m_currentGird)
    return m_currentGird->getDouble("DW");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current Girder element not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TIGRoff() const
{
  if(m_currentGird)
    return m_currentGird->getDouble("OFF");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current Girder element not set, returning -999.9\n";
    return -999.9;
  }
}

// -------------------- T I F G  ---------------------
int TileDddbManager::SetCurrentTifg(int section)
{
  unsigned int ind = 0;

  while (((*m_tifg)[ind]->getInt("SECTION") != section) && (++ind < m_n_tifg)) 
  {
  }

  if(ind >= m_n_tifg)
  {
    //    std::cerr << "\nTileDddbManager::SetCurrentTifg ERROR! Unable to find the finger, section = " << section << "\n";
    return 0;
  }
  else
  {
    m_currentTifg = (*m_tifg)[ind];
    return 1;
  }
}

int TileDddbManager::TIFGsection() const
{
  if(m_currentTifg)
    return m_currentTifg->getInt("SECTION");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TIFG not set, returning -999\n";
    return -999;
  }
}

int TileDddbManager::TIFGnelem() const
{
  if(m_currentTifg)
    return m_currentTifg->getInt("NELEM");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TIFG not set, returning -999\n";
    return -999;
  }
}

double TileDddbManager::TIFGdz() const
{
  if(m_currentTifg)
    return m_currentTifg->getDouble("DZ");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TIFG not set, returning -999.9\n";
    return -999.9;
  }
}

// -------------------- T I C G  ---------------------
int TileDddbManager::SetCurrentTicg(int item)
{
  unsigned int ind = 0;

  while (((*m_ticg)[ind]->getInt("ITEM") != item) && (++ind < m_n_ticg)) 
  {
  }

  if(ind >= m_n_ticg)
  {
    //    std::cerr << "\nTileDddbManager::SetCurrentTicg ERROR! Unable to find the finger element, item = " << item << "\n";
    return 0;
  }
  else
  {
    m_currentTicg = (*m_ticg)[ind];
    return 1;
  }
}

int TileDddbManager::TICGitem() const
{
  if(m_currentTicg)
    return m_currentTicg->getInt("ITEM");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICG not set, returning -999\n";
    return -999;
  }
}

int TileDddbManager::TICGmaterial() const
{
  if(m_currentTicg)
    return m_currentTicg->getInt("MATERIAL");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICG not set, returning -999\n";
    return -999;
  }
}

int TileDddbManager::TICGshape() const
{
  if(m_currentTicg)
    return m_currentTicg->getInt("SHAPE");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICG not set, returning -999\n";
    return -999;
  }
}

double TileDddbManager::TICGrc() const
{
  if(m_currentTicg)
    return m_currentTicg->getDouble("RC");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICG not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICGdr() const
{
  if(m_currentTicg)
    return m_currentTicg->getDouble("DR");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICG not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICGdx1() const
{
  if(m_currentTicg)
    return m_currentTicg->getDouble("DX1");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICG not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICGdx2() const
{
  if(m_currentTicg)
    return m_currentTicg->getDouble("DX2");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICG not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICGoff() const
{
  if(m_currentTicg)
    return m_currentTicg->getDouble("OFF");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICG not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICGdz() const
{
  if(m_currentTicg)
    return m_currentTicg->getDouble("DZ");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICG not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICGzc() const
{
  if(m_currentTicg)
    return m_currentTicg->getDouble("ZC");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICG not set, returning -999.9\n";
    return -999.9;
  }
}

unsigned int TileDddbManager::GetNumTicl() const
{ 
  return m_n_ticl;
}

// -------------------- T I C L  ---------------------
int TileDddbManager::SetCurrentTicl(int detector, double sample, double tower)
{
  unsigned int detector_ind = 0;

  // Looking for the first cell of given detector
  while (((*m_ticl)[detector_ind]->getInt("DETECTOR") != detector) && (++detector_ind < m_n_ticl)) 
  {
  }

  if(detector_ind >= m_n_ticl)
  {
    //    std::cerr << "\nTileDddbManager::SetCurrentTicl ERROR! Unable to find any cell for the detector = " 
    //	      << detector << "\n";
    return 0;
  }
  else
  {
    // Looking for the first cell in given sample of given detector
    unsigned int sample_ind = detector_ind;

    while (((*m_ticl)[sample_ind]->getInt("SAMPLE") != sample) && (++sample_ind < m_n_ticl)) 
    {
    }

    if(sample_ind >= m_n_ticl)
    {
      //      std::cerr << "\nTileDddbManager::SetCurrentTicl ERROR! Unable to find any cell for sample = " 
      //		<< sample << " in the detector = " << detector << "\n";
      return 0;
    }
    else
    {
      // Looking for the cell with give tower inside found sample
      unsigned int ind = sample_ind;

      while (((*m_ticl)[ind]->getInt("TOWER") != tower) && (++ind < m_n_ticl))
      {
      }

      if(ind >= m_n_ticl)
      {
	//	std::cerr << "\nTileDddbManager::SetCurrentTicl ERROR! Unable to find any cell for tower = " 
	//		  << tower << " in the detector = " << detector << " and sample = " << sample << "\n";
	return 0;
      }
      else
      {
	m_currentTicl = (*m_ticl)[ind];
	m_currentTiclInd = ind;
	return 1;
      }
    }
  }
}

int TileDddbManager::SetCurrentTicl(unsigned int index)
{
  if(index >= m_n_ticl)
  {
    //    std::cerr << "\nTileDddbManager::SetCurrentTicl ERROR! The requested index = "  
    //	      << index << " out of range.\n";
    return 0;
  }
  else
  {
    m_currentTicl = (*m_ticl)[index];
    m_currentTiclInd = index;
    return 1;
  }
}

int TileDddbManager::SetFirstTiclInDet(int detector)
{
  unsigned int ind = 0;

  // Looking for the first cell of given detector
  while (((*m_ticl)[ind]->getInt("DETECTOR") != detector) && (++ind < m_n_ticl)) 
  {
  }

  if(ind >= m_n_ticl)
    return 0;  // FAILURE

  m_currentTicl = (*m_ticl)[ind];
  m_currentTiclInd = ind;
  return 1;  // SUCCESS

}

int TileDddbManager::SetFirstTiclInDetSamp(int detector, double sample)
{
  unsigned int detector_ind = 0;

  // Looking for the first cell of given detector
  while (((*m_ticl)[detector_ind]->getInt("DETECTOR") != detector) && (++detector_ind < m_n_ticl)) 
  {
  }

  if(detector_ind >= m_n_ticl)
    return 0;  // FAILURE
  else
  {
    // Looking for the first cell in given sample of given detector
    unsigned int sample_ind = detector_ind;

    while (((*m_ticl)[sample_ind]->getInt("SAMPLE") != sample) && (++sample_ind < m_n_ticl)) 
    {
    }

    if(sample_ind >= m_n_ticl)
      return 0;  // FAILURE

    m_currentTicl = (*m_ticl)[sample_ind];
    m_currentTiclInd = sample_ind;
    return 1;  // SUCCESS
  }
}

int TileDddbManager::SetNextTiclInDet()
{
  if(m_currentTiclInd<0) return 0; // FAILURE

  int currentDetector = (*m_ticl)[m_currentTiclInd]->getInt("DETECTOR");

  if((++m_currentTiclInd == static_cast<int>(m_n_ticl))||((*m_ticl)[m_currentTiclInd]->getInt("DETECTOR") != currentDetector))
  {
    m_currentTiclInd--;
    return 0;  // FAILURE
  }

  m_currentTicl = (*m_ticl)[m_currentTiclInd];
  return 1;
}

int TileDddbManager::SetNextTiclInDetSamp()
{
  if(m_currentTiclInd<0) return 0; // FAILURE

  int currentDetector = (*m_ticl)[m_currentTiclInd]->getInt("DETECTOR");
  float currentSample = (*m_ticl)[m_currentTiclInd]->getInt("SAMPLE");

  if((++m_currentTiclInd == static_cast<int>(m_n_ticl))||
     ((*m_ticl)[m_currentTiclInd]->getInt("DETECTOR") != currentDetector)||
     ((*m_ticl)[m_currentTiclInd]->getInt("SAMPLE") != currentSample))
  {
    m_currentTiclInd--;
    return 0;  // FAILURE
  }

  m_currentTicl = (*m_ticl)[m_currentTiclInd];
  return 1;
}

int TileDddbManager::TICLdetector() const
{
  if(m_currentTicl)
    return m_currentTicl->getInt("DETECTOR");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICL not set, returning -999\n";
    return -999;
  }
}

double TileDddbManager::TICLncell() const
{
  if(m_currentTicl)
    return m_currentTicl->getInt("NCELL");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICL not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICLtower() const
{
  if(m_currentTicl)
    return m_currentTicl->getInt("TOWER");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICL not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICLsample() const
{
  if(m_currentTicl)
    return m_currentTicl->getInt("SAMPLE");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICL not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICLeta() const
{
  if(m_currentTicl)
    return m_currentTicl->getDouble("ETA");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICL not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICLdeta() const
{
  if(m_currentTicl)
    return m_currentTicl->getDouble("DETA");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICL not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICLfirstrow() const
{
  if(m_currentTicl)
    return m_currentTicl->getInt("FIRSTROW");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICL not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICLlastrow() const
{
  if(m_currentTicl)
    return m_currentTicl->getInt("LASTROW");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICL not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICLntilesrow(unsigned int ind) const
{
  if(m_currentTicl)
    return m_currentTicl->getInt("NTILESROW",ind);
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICL not set, returning 9999\n";
    return 9999;
  }
}

double TileDddbManager::TICLnpmt() const
{
  if(m_currentTicl)
    return m_currentTicl->getInt("NPMT");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICL not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICLholes(unsigned int ind) const
{
  if(m_currentTicl)
    return m_currentTicl->getInt("HOLES",ind);
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICL not set, returning 9999\n";
    return 9999;
  }
}

bool TileDddbManager::addPlatesToCell() const
{
  if (m_tileSwitches)
    {
      if ((*m_tileSwitches)[0]->isFieldNull("ADDPLATESTOCELL"))
        {
          // std::cerr << "\nTileDddbManager   WARNING TileSwitches(ADDPLATESTOCELL) is EMPTY, returning TRUE\n\n";
          return true;
        }
      else
	{  
	  return (*m_tileSwitches)[0]->getInt("ADDPLATESTOCELL");
	}
    }
  else
    {
      std::cerr << "\nTileDddbManager ERROR! TileSwitches is not set, returning addPlatesToCell= TRUE\n\n";
      return true;
    }
}

int TileDddbManager::Ushape() const
{
  if (m_tileSwitches)
    {
        try {
          if ((*m_tileSwitches)[0]->isFieldNull("USHAPE"))
            {
              // std::cerr << "\nTileDddbManager   WARNING TileSwitches(USHAPE) is EMPTY, returning Ushape= 0\n\n";
              return 0;
            }
          else
	    {  
	      return (*m_tileSwitches)[0]->getInt("USHAPE");
	    }
        } catch (std::exception& e) {
          std::cout << "\nTileDddbManager   WARNING TileSwitches table does not contain field USHAPE, returning Ushape= 0 \n\n";
          return 0;
      }
    }
  else
    {
      std::cerr << "\nTileDddbManager  ERROR! TileSwitches is not set, returning Ushape= 0\n\n";
      return 0;
    }
}

bool TileDddbManager::glue() const
{
  if (m_tileSwitches)
    {
      if ((*m_tileSwitches)[0]->isFieldNull("GLUE"))
        {
          // std::cerr << "\nTileDddbManager   WARNING TileSwitches(GLUE) is EMPTY, returning TRUE\n\n";
          return true;
        }
      else
	{
	  return (*m_tileSwitches)[0]->getInt("GLUE");
	}
    }
  else
    {
      std::cerr << "\nTileDddbManager ERROR! TileSwitches is not set, returning glue= TRUE\n\n";
      return true;
    }
}

