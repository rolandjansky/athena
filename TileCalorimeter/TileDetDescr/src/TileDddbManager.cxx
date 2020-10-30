/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TileDetDescr/TileDddbManager.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <iostream>
#include <stdexcept>
#define MLOG(x)   if (m_log->level()<=MSG::x) *m_log << MSG::x

TileDddbManager::TileDddbManager(IRDBAccessSvc* access,
		                 const std::string&    version_tag,
				 const std::string&    version_node)
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
  m_log = new MsgStream(Athena::getMessageSvc(), "TileDddbManager");

  MLOG(INFO) << "m_tag = " << m_tag << endmsg;

  m_tiglob = access->getRecordsetPtr("TileGlobals",m_tag,m_node);
  m_n_tiglob = m_tiglob->size();
  MLOG(INFO) << "n_tiglob = " << m_n_tiglob << endmsg;
 
  if(access->getChildTag("TileModule",m_tag,m_node)!="") {
   m_timod = access->getRecordsetPtr("TileModule",m_tag,m_node);
  } else {
   m_timod = access->getRecordsetPtr("TileModules",m_tag,m_node);
  }
  m_n_timod = m_timod->size();
  MLOG(INFO) << "n_timod = " << m_n_timod << endmsg;
  
  if (access->getChildTag("TileCuts",m_tag,m_node)!="") 
   { m_buildCuts = true;
     m_cuts = access->getRecordsetPtr("TileCuts",m_tag,m_node);
     m_n_cuts = m_cuts->size();
     MLOG(INFO) << "n_cuts = " << m_n_cuts << endmsg;

   } else {
     m_buildCuts = false;
   }

  if (access->getChildTag("TileSaddleSup",m_tag,m_node)!="") 
   { m_buildSaddle = true;
     m_saddle = access->getRecordsetPtr("TileSaddleSup",m_tag,m_node);
     m_n_saddle = m_saddle->size();
     MLOG(INFO) << "n_saddle = " << m_n_saddle << endmsg;

   } else {
     m_buildSaddle = false;
   }

  m_tile = access->getRecordsetPtr("TILE",m_tag,m_node);

  m_tilb = access->getRecordsetPtr("TILB",m_tag,m_node);
  m_n_tilb = m_tilb->size();
  MLOG(INFO) << "n_tilb = " << m_n_tilb << endmsg;

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
  MLOG(INFO) << "n_tileSwitches = " << m_n_tileSwitches << endmsg;


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
    MLOG(ERROR) << "TileDddbManager::GetEnvType() - Current Envelope not set, returning -999" << endmsg;
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
      MLOG(ERROR) << "TileDddbManager::SetCurrentEnvByType() - Couldn't return begin of modules" << endmsg;
      return 0;
    }
  } else {
    m_currentTileGlob = 0;
    MLOG(ERROR) << "TileDddbManager::SetCurrentEnvByType() - Couldn't return Envelope" << endmsg;
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
      MLOG(ERROR) << "TileDddbManager::SetCurrentEnvByIndex() - Couldn't return begin of modules" << endmsg;
      return 0;
    }
  } else {
    m_currentTileGlob = 0;
    MLOG(ERROR) << "TileDddbManager::SetCurrentEnvByIndex() - Couldn't return Envelope" << endmsg;
    return 0;
  }
}

 
int TileDddbManager::GetEnvSide() const
{
  if (m_currentTileGlob) {
    return m_currentTileGlob->getInt("SIDE");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::GetEnvSide() - Current Section not set, returning -999" << endmsg;
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
    MLOG(ERROR) << "TileDddbManager::GetEnvNModules() - Current Envelope not set, returning -999" << endmsg;
    return -999;
  } 
}

double TileDddbManager::GetEnvRin() const
{
  if (m_currentTileGlob) {
    return m_currentTileGlob->getDouble("RIN");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::GetEnvRin() - Current Envelope not set, returning -999" << endmsg;
    return -999;
  } 
}

double TileDddbManager::GetEnvRout() const
{
  if (m_currentTileGlob) {
    return m_currentTileGlob->getDouble("ROUT");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::GetEnvRout() - Current Envelope not set, returning -999" << endmsg;
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
    MLOG(ERROR) << "TileDddbManager::GetEnvZLength() - Current Envelope not set, returning -999" << endmsg;
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
    MLOG(ERROR) << "TileDddbManager::GetEnvDPhi() - Current Envelope not set, returning -999" << endmsg;
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
    MLOG(ERROR) << "TileDddbManager::GetEnvDZ() - Current Envelope not set, returning -999" << endmsg;
    return -999;
  } 
}

double TileDddbManager::GetEnvDX() const
{
  if (m_currentTileGlob) {
    return m_currentTileGlob->getDouble("DX");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::GetEnvDX() - Current Envelope not set, returning -999" << endmsg;
    return -999;
  } 
}

double TileDddbManager::GetEnvDY() const
{
  if (m_currentTileGlob) {
    return m_currentTileGlob->getDouble("DY");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::GetEnvDY() - Current Envelope not set, returning -999" << endmsg;
    return -999;
  } 
}

double TileDddbManager::GetEnvDPsi() const
{
  if (m_currentTileGlob) {
    return m_currentTileGlob->getDouble("DPSI");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::GetEnvDPsi() - Current Envelope not set, returning -999" << endmsg;
    return -999;
  } 
}

double TileDddbManager::GetEnvDTheta() const
{
  if (m_currentTileGlob) {
    return m_currentTileGlob->getDouble("DTHETA");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::GetEnvDTheta() - Current Envelope not set, returning -999" << endmsg;
    return -999;
  } 
}

double TileDddbManager::GetEnvZShift() const
{
  if(m_currentTileGlob) {
    return m_currentTileGlob->getDouble("ZSHIFT");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::GetEnvZShift() - Current Envelope not set, returning -999" << endmsg;
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
      MLOG(ERROR) << "TileDddbManager::SetCurrentModuleByIndex() - Module number " << Id << " is out of range" << endmsg;
      return 0;
    }
    else {
      m_currentTileMod = (*m_timod)[Id + m_EnvBegin];
      FillModTypes();
      return 1;
    }
  }
  else {
    MLOG(ERROR) << "TileDddbManager::SetCurrentModuleByIndex() - Current Envelope not set, returning -999" << endmsg;
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
      MLOG(ERROR) << "TileDddbManager::SetCurrentModuleByNumber() - Couldn't return Module" << endmsg;
      return 0;
    }
  }
  else {
    MLOG(ERROR) << "TileDddbManager::SetCurrentModuleByNumber() - Current Envelope not set, returning -999" << endmsg;
    return -999;
  } 
}

int TileDddbManager::GetModNumber() const
{
  if (m_currentTileMod) {
    return m_currentTileMod->getInt("MODNUM");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::GetModNumber() - Current Module not set, returning -999" << endmsg;
    return -999;
  }  
}

int TileDddbManager::GetModType() const
{
  if(m_currentTileMod) {
    return m_currentTileMod->getInt("TYP");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::GetModType() - Current Module not set, returning -999" << endmsg;
    return -999;
  }
}

double TileDddbManager::GetModDX() const
{
  if (m_currentTileMod) {
    return m_currentTileMod->getDouble("DX");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::GetModDX() - Current Module not set, returning -999" << endmsg;
    return -999;
  }
}

double TileDddbManager::GetModDY() const
{
  if (m_currentTileMod) {
    return m_currentTileMod->getDouble("DY");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::GetModDY() - Current Module not set, returning -999" << endmsg;
    return -999;
  }
}

double TileDddbManager::GetModDZ() const
{
  if (m_currentTileMod) {
    return m_currentTileMod->getDouble("DZ");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::GetModDZ() - Current Module not set, returning -999" << endmsg;
    return -999;
  }
}

double TileDddbManager::GetModDPhi() const
{
  if (m_currentTileMod) {
    return m_currentTileMod->getDouble("DPHI");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::GetModDPhi() - Current Module not set, returning -999" << endmsg;
    return -999;
  }
}

double TileDddbManager::GetModDPsi() const
{
  if (m_currentTileMod) {
    return m_currentTileMod->getDouble("DPSI");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::GetModDPsi() - Current Module not set, returning -999" << endmsg;
    return -999;
  }
}

double TileDddbManager::GetModDTheta() const
{
  if (m_currentTileMod) {
    return m_currentTileMod->getDouble("DTHETA");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::GetModDTheta() - Current Module not set, returning -999" << endmsg;
    return -999;
  }
}


//------------ TILE MODULE TYPES -------------------
int TileDddbManager::SetCurrentTypeByIndex(unsigned int Id)
{
  if (Id < m_modTypes.size()) {
    return SetCurrentSection(m_modTypes[Id]);
  } else {
    MLOG(ERROR) << "TileDddbManager::SetCurrentTypeByIndex() - Type index (" << Id << ") is out of range (" << m_modTypes.size() << ")" << endmsg;
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
    MLOG(ERROR) << "TileDddbManager::SetCurrentSectionByIndex() - Section index (" << IdSection << ") is out of range (" << m_n_tilb << ")" << endmsg;
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
    if (print) MLOG(ERROR) << "TileDddbManager::SetCurrentSection() - Unable to find the section" << endmsg;
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
    MLOG(ERROR) << "TileDddbManager::TILBsection() - Current Section not set, returning -999" << endmsg;
    return -999;
  }
}

int TileDddbManager::TILBnperiod() const
{
  if (m_currentSection) {
    return m_currentSection->getInt("NPERIOD");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::TILBnperiod() - Current Section not set, returning -999" << endmsg;
    return -999;
  }
}

int TileDddbManager::TILBnmodul() const
{
  if (m_currentSection) {
    return m_currentSection->getInt("NMODUL");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::TILBnmodul() - Current Section not set, returning -999" << endmsg;
    return -999;
  }
}

double TileDddbManager::TILBrmin() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("RMIN");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::TILBrmin() - Current Section not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TILBrmax() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("RMAX");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::TILBrmax() - Current Section not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TILBrminimal() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("RMINIMAL");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::TILBrminimal() - Current Section not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TILBrmaximal() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("RMAXIMAL");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::TILBrmaximal() - Current Section not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TILBdzperio() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("DZPERIO");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::TILBdzperio() - Current Section not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TILBdrfront() const
{
  if(m_currentSection) {
    return m_currentSection->getDouble("DRFRONT");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::TILBdrfront() - Current Section not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TILBdzend() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("DZEND");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::TILBdzend() - Current Section not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TILBflangex() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("FLANGEX");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::TILBflangex() - Current Section not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TILBflangey() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("FLANGEY");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::TILBflangey() - Current Section not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TILBzoffset() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("ZOFFSET");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::TILBzoffset() - Current Section not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TILBphigap() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("PHIGAP");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::TILBphigap() - Current Section not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TILBisci_st() const
{
  if (m_currentSection) {
    return m_currentSection->getInt("ISCI_ST");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::TILBisci_st() - Current Section not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TILBdzmodul() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("DZMODUL");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::TILBdzmodul() - Current Section not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TILBdzmast() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("DZMAST");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::TILBdzmast() - Current Section not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TILBdzspac() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("DZSPAC");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::TILBdzspac() - Current Section not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TILBdzend1() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("DZEND1");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::TILBdzend1() - Current Section not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TILBdzend2() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("DZEND2");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::TILBdzend2() - Current Section not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

int TileDddbManager::TILBngirder() const
{
  if (m_currentSection) {
    return m_currentSection->getInt("NGIRDER");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::TILBngirder() - Current Section not set, returning -999" << endmsg;
    return -999;
  }
}

int TileDddbManager::TILBnscin() const
{
  if (m_currentSection) {
    return m_currentSection->getInt("NSCIN");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::TILBnscin() - Current Section not set, returning -999" << endmsg;
    return -999;
  }
}

int TileDddbManager::TILBfingpattern() const
{
  if (m_currentSection) {
    return m_currentSection->getInt("FINGPATTERN");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::TILBfingpattern() - Current Section not set, returning -999" << endmsg;
    return -999;
  }
}

int TileDddbManager::TILBnpercutpos() const
{
  if (m_currentSection) {
    return m_currentSection->getInt("NPERCUTPOS");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::TILBnpercutpos() - Current Section not set, returning -999" << endmsg;
    return -999;
  }
}

int TileDddbManager::TILBnpercutneg() const
{
  if (m_currentSection) {
    return m_currentSection->getInt("NPERCUTNEG");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::TILBnpercutneg() - Current Section not set, returning -999" << endmsg;
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
    MLOG(ERROR) << "TileDddbManager::TILBcurscint() - Current Section not set, returning -999" << endmsg;
    return -999;
  }
}

double TileDddbManager::TILBdzgir() const
{
  if (m_currentSection) {
    return m_currentSection->getDouble("DZGIR");
  }
  else {
    MLOG(ERROR) << "TileDddbManager::TILBdzgir() - Current Section not set, returning -999" << endmsg;
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
    MLOG(ERROR) << "TileDddbManager::SetCurrentScin() - Unable to find the scintillator, item = " << item << endmsg;
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
    MLOG(ERROR) << "TileDddbManager::SCNTitem() - Current Scintillator not set, returning -999" << endmsg;
    return -999;
  }
}

double TileDddbManager::SCNTdr() const
{
  if(m_currentScin)
    return m_currentScin->getDouble("DR");
  else
  {
    MLOG(ERROR) << "TileDddbManager::SCNTdr() - Current Scintillator not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::SCNTrc() const
{
  if(m_currentScin)
    return m_currentScin->getDouble("RC");
  else
  {
    MLOG(ERROR) << "TileDddbManager::SCNTrc() - Current Scintillator not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::SCNTzp() const
{
  if(m_currentScin)
    return m_currentScin->getDouble("ZP");
  else
  {
    MLOG(ERROR) << "TileDddbManager::SCNTzp() - Current Scintillator not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::SCNTdrw() const
{
  if(m_currentScin)
    return m_currentScin->getDouble("DRW");
  else
  {
    MLOG(ERROR) << "TileDddbManager::SCNTdrw() - Current Scintillator not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::SCNTdt() const
{
  if(m_currentScin)
    return m_currentScin->getDouble("DT");
  else
  {
    MLOG(ERROR) << "TileDddbManager::SCNTdt() - Current Scintillator not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::SCNTdtw() const
{
  if(m_currentScin)
    return m_currentScin->getDouble("DTW");
  else
  {
    MLOG(ERROR) << "TileDddbManager::SCNTdtw() - Current Scintillator not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::SCNTdphi() const
{
  if(m_currentScin)
    return m_currentScin->getDouble("DPHI");
  else
  {
    MLOG(ERROR) << "TileDddbManager::SCNTdphi() - Current Scintillator not set, returning -999.9" << endmsg;
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
     { MLOG(ERROR) << "TileDddbManager::SetCurrentCuts() - Unable to find the m_currentCuts volume name = " << name << endmsg;
       return 0;
     }
     else
     { m_currentCuts = (*m_cuts)[ind];
       return 1;
     }
    }
   MLOG(WARNING) << "TileDddbManager::SetCurrentCuts() - SetCuts Ext.Barrel Cuts unavailable in touched DB";
   return 1;
} 

bool TileDddbManager::BoolCuts()
{ if (m_buildCuts) 
  { return m_buildCuts;
  } else
    { MLOG(DEBUG) << "TileDddbManager::BoolCuts() - return false" << endmsg;
    return false;
  }
}

std::string TileDddbManager::CutsName() const
{std::string output ="-999.9";
   if (m_currentCuts)
     return m_currentCuts->getString("VOLUME");
   else
   { MLOG(ERROR) << "TileDddbManager::CutsName() - Current VOLUME dimension not set, returning -999.9" << endmsg;
     return output;
   } 
}

double TileDddbManager::CutsDX1() const
{
  if (m_currentCuts)
    return (double)m_currentCuts->getFloat("DX1");
  else
  { MLOG(ERROR) << "TileDddbManager::CutsDX1() - Current dX1 dimension not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::CutsDX2() const
{
  if (m_currentCuts)
    return (double)m_currentCuts->getFloat("DX2");
  else
  { MLOG(ERROR) << "TileDddbManager::CutsDX2() - Current dX2 dimension not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::CutsDY1() const
{
  if (m_currentCuts)
    return (double)m_currentCuts->getFloat("DY1");
  else
  { MLOG(ERROR) << "TileDddbManager::CutsDY1() - Current dY1 dimension not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::CutsDY2() const
{
  if (m_currentCuts)
    return (double)m_currentCuts->getFloat("DY2");
  else
  { MLOG(ERROR) << "TileDddbManager::CutsDY2() - Current dY2 dimension not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::CutsDZ1() const
{
  if (m_currentCuts)
    return (double)m_currentCuts->getFloat("DZ1");
  else
  { MLOG(ERROR) << "TileDddbManager::CutsDZ1() - Current dZ1 dimension not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::CutsXpos() const
{
  if (m_currentCuts)
    return (double)m_currentCuts->getFloat("XPOS");
  else
  { MLOG(ERROR) << "TileDddbManager::CutsXpos() - Current Xpos dimension not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::CutsYpos() const
{
  if (m_currentCuts)
    return (double)m_currentCuts->getFloat("YPOS");
  else
  { MLOG(ERROR) << "TileDddbManager::CutsYpos() - Current Ypos dimension not set, returning -999.9" << endmsg;
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
   MLOG(WARNING) << "TileDddbManager::SetCurrentSaddle() - Saddle unavailable in touched DB"<<endmsg;
   return 1;
}

bool TileDddbManager::BoolSaddle()
{ if (m_buildSaddle) 
  { return m_buildSaddle;
  } else
  { MLOG(DEBUG) << "TileDddbManager::BoolSaddle() - return false" << endmsg;
    return false;
  }
}

double TileDddbManager::DzSaddleSupport() const
{
  if (m_currentSaddle)
    return (double)m_currentSaddle->getFloat("DZSADDLESUPPORT");
  else
  { MLOG(ERROR) << "TileDddbManager::DzSaddleSupport() - dimension not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::RadiusSaddle() const
{
  if (m_currentSaddle)
    return (double)m_currentSaddle->getFloat("RADIUSSADDLE");
  else
  { MLOG(ERROR) << "TileDddbManager::RadiusSaddle() - dimension not set, returning -999.9";
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
    MLOG(DEBUG) << "TileDddbManager::SetCurrentGird() - Unable to find the girder element, item= " << item << endmsg;
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
    MLOG(ERROR) << "TileDddbManager::TIGRitem() - Current Girder element not set, returning -999" << endmsg;
    return -999;
  }
}

int TileDddbManager::TIGRmaterial() const
{
  if(m_currentGird)
    return m_currentGird->getInt("MATERIAL");
  else
  {
    MLOG(ERROR) << "TileDddbManager::TIGRmaterial() - Current Girder element not set, returning -999" << endmsg;
    return -999;
  }
}

double TileDddbManager::TIGRrc() const
{
  if(m_currentGird)
    return m_currentGird->getDouble("RC");
  else
  {
    MLOG(ERROR) << "TileDddbManager::TIGRrc() - Current Girder element not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TIGRdr() const
{
  if(m_currentGird)
    return m_currentGird->getDouble("DR");
  else
  {
    MLOG(ERROR) << "TileDddbManager::TIGRdr() - Current Girder element not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TIGRdw() const
{
  if(m_currentGird)
    return m_currentGird->getDouble("DW");
  else
  {
    MLOG(ERROR) << "TileDddbManager::TIGRdw() - Current Girder element not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TIGRoff() const
{
  if(m_currentGird)
    return m_currentGird->getDouble("OFF");
  else
  {
    MLOG(ERROR) << "TileDddbManager::TIGRoff() - Current Girder element not set, returning -999.9" << endmsg;
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
    MLOG(DEBUG) << "TileDddbManager::SetCurrentTifg() - Unable to find the finger, section = " << section << endmsg;
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
    MLOG(ERROR) << "TileDddbManager::TIFGsection() - Current TIFG not set, returning -999" << endmsg;
    return -999;
  }
}

int TileDddbManager::TIFGnelem() const
{
  if(m_currentTifg)
    return m_currentTifg->getInt("NELEM");
  else
  {
    MLOG(ERROR) << "TileDddbManager::TIFGnelem() - Current TIFG not set, returning -999" << endmsg;
    return -999;
  }
}

double TileDddbManager::TIFGdz() const
{
  if(m_currentTifg)
    return m_currentTifg->getDouble("DZ");
  else
  {
    MLOG(ERROR) << "TileDddbManager::TIFGdz() - Current TIFG not set, returning -999.9" << endmsg;
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
    MLOG(DEBUG) << "TileDddbManager::SetCurrentTicg() - Unable to find the finger element, item = " << item << endmsg;
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
    MLOG(ERROR) << "TileDddbManager::TICGitem() - Current TICG not set, returning -999" << endmsg;
    return -999;
  }
}

int TileDddbManager::TICGmaterial() const
{
  if(m_currentTicg)
    return m_currentTicg->getInt("MATERIAL");
  else
  {
    MLOG(ERROR) << "TileDddbManager::TICGmaterial() - Current TICG not set, returning -999" << endmsg;
    return -999;
  }
}

int TileDddbManager::TICGshape() const
{
  if(m_currentTicg)
    return m_currentTicg->getInt("SHAPE");
  else
  {
    MLOG(ERROR) << "TileDddbManager::TICGshape() - Current TICG not set, returning -999" << endmsg;
    return -999;
  }
}

double TileDddbManager::TICGrc() const
{
  if(m_currentTicg)
    return m_currentTicg->getDouble("RC");
  else
  {
    MLOG(ERROR) << "TileDddbManager::TICGrc() - Current TICG not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TICGdr() const
{
  if(m_currentTicg)
    return m_currentTicg->getDouble("DR");
  else
  {
    MLOG(ERROR) << "TileDddbManager::TICGdr() - Current TICG not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TICGdx1() const
{
  if(m_currentTicg)
    return m_currentTicg->getDouble("DX1");
  else
  {
    MLOG(ERROR) << "TileDddbManager::TICGdx1() - Current TICG not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TICGdx2() const
{
  if(m_currentTicg)
    return m_currentTicg->getDouble("DX2");
  else
  {
    MLOG(ERROR) << "TileDddbManager::TICGdx2() - Current TICG not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TICGoff() const
{
  if(m_currentTicg)
    return m_currentTicg->getDouble("OFF");
  else
  {
    MLOG(ERROR) << "TileDddbManager::TICGoff() - Current TICG not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TICGdz() const
{
  if(m_currentTicg)
    return m_currentTicg->getDouble("DZ");
  else
  {
    MLOG(ERROR) << "TileDddbManager::TICGdz() - Current TICG not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TICGzc() const
{
  if(m_currentTicg)
    return m_currentTicg->getDouble("ZC");
  else
  {
    MLOG(ERROR) << "TileDddbManager::TICGzc() - Current TICG not set, returning -999.9" << endmsg;
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
    MLOG(DEBUG) << "TileDddbManager::SetCurrentTicl() - Unable to find any cell for the detector = " << detector << endmsg;
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
      MLOG(DEBUG) << "TileDddbManager::SetCurrentTicl() - Unable to find any cell for sample = "
                  << sample << " in the detector = " << detector << endmsg;
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
	MLOG(DEBUG) << "TileDddbManager::SetCurrentTicl() - Unable to find any cell for tower = "
                    << tower << " in the detector = " << detector << " and sample = " << sample << endmsg;
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
    MLOG(DEBUG) << "TileDddbManager::SetCurrentTicl() - The requested index = " << index << " out of range." << endmsg;
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
    MLOG(ERROR) << "TileDddbManager::TICLdetector() - Current TICL not set, returning -999" << endmsg;
    return -999;
  }
}

double TileDddbManager::TICLncell() const
{
  if(m_currentTicl)
    return m_currentTicl->getInt("NCELL");
  else
  {
    MLOG(ERROR) << "TileDddbManager::TICLncell() - Current TICL not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TICLtower() const
{
  if(m_currentTicl)
    return m_currentTicl->getInt("TOWER");
  else
  {
    MLOG(ERROR) << "TileDddbManager::TICLtower() - Current TICL not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TICLsample() const
{
  if(m_currentTicl)
    return m_currentTicl->getInt("SAMPLE");
  else
  {
    MLOG(ERROR) << "TileDddbManager::TICLsample() - Current TICL not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TICLeta() const
{
  if(m_currentTicl)
    return m_currentTicl->getDouble("ETA");
  else
  {
    MLOG(ERROR) << "TileDddbManager::TICLeta() - Current TICL not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TICLdeta() const
{
  if(m_currentTicl)
    return m_currentTicl->getDouble("DETA");
  else
  {
    MLOG(ERROR) << "TileDddbManager::TICLdeta() - Current TICL not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TICLfirstrow() const
{
  if(m_currentTicl)
    return m_currentTicl->getInt("FIRSTROW");
  else
  {
    MLOG(ERROR) << "TileDddbManager::TICLfirstrow() - Current TICL not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TICLlastrow() const
{
  if(m_currentTicl)
    return m_currentTicl->getInt("LASTROW");
  else
  {
    MLOG(ERROR) << "TileDddbManager::TICLlastrow() - Current TICL not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TICLntilesrow(unsigned int ind) const
{
  if(m_currentTicl)
    return m_currentTicl->getInt("NTILESROW",ind);
  else
  {
    MLOG(ERROR) << "TileDddbManager::TICLntilesrow() - Current TICL not set, returning 9999" << endmsg;
    return 9999;
  }
}

double TileDddbManager::TICLnpmt() const
{
  if(m_currentTicl)
    return m_currentTicl->getInt("NPMT");
  else
  {
    MLOG(ERROR) << "TileDddbManager::TICLnpmt() - Current TICL not set, returning -999.9" << endmsg;
    return -999.9;
  }
}

double TileDddbManager::TICLholes(unsigned int ind) const
{
  if(m_currentTicl)
    return m_currentTicl->getInt("HOLES",ind);
  else
  {
    MLOG(ERROR) << "TileDddbManager::TICLholes() - Current TICL not set, returning 9999" << endmsg;
    return 9999;
  }
}

bool TileDddbManager::addPlatesToCell() const
{
  if (m_tileSwitches)
    {
      if ((*m_tileSwitches)[0]->isFieldNull("ADDPLATESTOCELL"))
        {
          MLOG(DEBUG) << "TileDddbManager::addPlatesToCell() - TileSwitches(ADDPLATESTOCELL) is EMPTY, returning TRUE" << endmsg;
          return true;
        }
      else
	{  
	  return (*m_tileSwitches)[0]->getInt("ADDPLATESTOCELL");
	}
    }
  else
    {
      MLOG(ERROR) << "TileDddbManager::addPlatesToCell() - TileSwitches is not set, returning addPlatesToCell= TRUE" << endmsg;
      return true;
    }
}

int TileDddbManager::uShape() const
{
  if (m_tileSwitches)
    {
        try {
          if ((*m_tileSwitches)[0]->isFieldNull("USHAPE"))
            {
              MLOG(DEBUG) << "TileDddbManager::uShape() - TileSwitches(USHAPE) is EMPTY, returning uShape= 0" << endmsg;
              return 0;
            }
          else
	    {  
	      return (*m_tileSwitches)[0]->getInt("USHAPE");
	    }
        } catch (std::exception& e) {
          MLOG(WARNING) << "TileDddbManager::uShape() - TileSwitches table does not contain field USHAPE, returning uShape= 0" << endmsg;
          return 0;
      }
    }
  else
    {
      MLOG(ERROR) << "TileDddbManager::uShape() - TileSwitches is not set, returning uShape= 0" << endmsg;
      return 0;
    }
}

int TileDddbManager::glue() const
{
  if (m_tileSwitches)
    {
        try {
          if ((*m_tileSwitches)[0]->isFieldNull("GLUE"))
            {
              MLOG(DEBUG) << "TileDddbManager::glue() - TileSwitches(GLUE) is EMPTY, returning glue= 1" << endmsg;
              return 1;
            }
          else
            {
              return (*m_tileSwitches)[0]->getInt("GLUE");
            }
        } catch (std::exception& e) {
          MLOG(WARNING) << "TileDddbManager::glue() - TileSwitches table does not contain field GLUE, returning glue= 1" << endmsg;
          return 1;
      }
    }
  else
    {
      MLOG(ERROR) << "TileDddbManager::glue() - TileSwitches is not set, returning glue= 1" << endmsg;
      return 1;
    }
}

