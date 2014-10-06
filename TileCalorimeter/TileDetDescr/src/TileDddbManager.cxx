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
  : n_cuts(0)
  , n_saddle(0)
  , currentTileGlob(0)
  , currentTileMod(0)
  , currentSection(0)
  , currentGird(0)
  , currentScin(0)
  , currentTifg(0)
  , currentTicg(0)
  , currentTicl(0)
  , currentCuts(0)
  , currentSaddle(0)
  , currentTiclInd(-1)
  , mTag(version_tag)
  , mNode(version_node)
{
  m_verbose = (log->level()<=MSG::VERBOSE);
 
  (*log) << MSG::INFO << "TileDddbManager: mTag = " << mTag << endreq;

  tiglob = access->getRecordsetPtr("TileGlobals",mTag,mNode);
  n_tiglob = tiglob->size();
  (*log) << MSG::INFO << "TileDddbManager: n_tiglob = " << n_tiglob << endreq;
 
  if(access->getChildTag("TileModule",mTag,mNode)!="") {
   timod = access->getRecordsetPtr("TileModule",mTag,mNode);
  } else {
   timod = access->getRecordsetPtr("TileModules",mTag,mNode);
  }
  n_timod = timod->size();
  (*log) << MSG::INFO << "TileDddbManager: n_timod = " << n_timod << endreq;
  
  if (access->getChildTag("TileCuts",mTag,mNode)!="") 
   { m_buildCuts = true;
     cuts = access->getRecordsetPtr("TileCuts",mTag,mNode);
     n_cuts = cuts->size();
     (*log) << MSG::INFO << "TileDddbManager: n_cuts = " << n_cuts << endreq;

   } else {
     m_buildCuts = false;
   }

  if (access->getChildTag("TileSaddleSup",mTag,mNode)!="") 
   { m_buildSaddle = true;
     saddle = access->getRecordsetPtr("TileSaddleSup",mTag,mNode);
     n_saddle = saddle->size();
     (*log) << MSG::INFO << "TileDddbManager: n_saddle = " << n_saddle << endreq;

   } else {
     m_buildSaddle = false;
   }

  tile = access->getRecordsetPtr("TILE",mTag,mNode);

  tilb = access->getRecordsetPtr("TILB",mTag,mNode);
  n_tilb = tilb->size();
  (*log) << MSG::INFO << "TileDddbManager: n_tilb = " << n_tilb << endreq;

  tigr = access->getRecordsetPtr("TIGR",mTag,mNode);
  n_tigr = tigr->size();

  scnt = access->getRecordsetPtr("SCNT",mTag,mNode);
  n_scnt = scnt->size();

  tifg = access->getRecordsetPtr("TIFG",mTag,mNode);
  n_tifg = tifg->size();

  ticg = access->getRecordsetPtr("TICG",mTag,mNode);
  n_ticg = ticg->size();

  ticl = access->getRecordsetPtr("TICL",mTag,mNode);
  n_ticl = ticl->size();

  tileSwitches = access->getRecordsetPtr("TileSwitches",mTag,mNode);
  n_tileSwitches = tileSwitches->size();
  (*log) << MSG::INFO << "TileDddbManager: n_tileswitches = " << n_tileSwitches << endreq;


  EnvNum = 0;
  EnvSize = 0;
  EnvBegin = 0;
  mModTypes.clear();
}

// ---------- T I L E -------------

int TileDddbManager::TILEnmodul() const
{
  return (*tile)[0]->getInt("NMODUL");
}

double TileDddbManager::TILErmim() const
{
  return (*tile)[0]->getDouble("RMIM");
}

double TileDddbManager::TILErmam() const
{
  return (*tile)[0]->getDouble("RMAM");
}

double TileDddbManager::TILEzmam() const
{
  return (*tile)[0]->getDouble("ZMAM");
}

double TileDddbManager::TILErmin() const
{
  return (*tile)[0]->getDouble("RMIN");
}

double TileDddbManager::TILErmax() const
{
  return (*tile)[0]->getDouble("RMAX");
}

double TileDddbManager::TILEzshift() const
{
  return (*tile)[0]->getDouble("ZSHIFT");
}

unsigned int TileDddbManager::GetNumTilb() const
{ 
  return n_tilb;
}

//------------ TILE GLOBALS--------------------
int TileDddbManager::GetNumberOfEnv() const
{  
  return n_tiglob;
}

int TileDddbManager::GetEnvType() const
{
  if (currentTileGlob) {
    return currentTileGlob->getInt("PART");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Envelope not set, returning -999\n";
    return -999;
  }  
}

int TileDddbManager::SetCurrentEnvByType(unsigned int envelopeT)
{
  currentTileGlob = 0;
  currentTileMod = 0;
  currentSection = 0;
  mModTypes.clear();

  unsigned int ind2 = 0;
  while (((*tiglob)[ind2]->getInt("PART") != static_cast<int>(envelopeT)) && (++ind2 < n_tiglob))
    {
    }
  if (ind2 < n_tiglob) {
    currentTileGlob = (*tiglob)[ind2];
    EnvSize = currentTileGlob->getInt("NMODULES");
    unsigned int ind3 = 0;
    while (((*timod)[ind3]->getInt("PART") != static_cast<int>(envelopeT)) && (++ind3 < n_timod))
      {
      }
    if (ind3 < n_timod) {
        EnvBegin = ind3;
        return 1;
    }
    else {
      EnvSize = 0;
      EnvBegin = 0;
      std::cerr << "TileDddbManager couldn't return begin of modules\n";
      return 0;
    }
  } else {
    currentTileGlob = 0;
    std::cerr << "TileDddbManager couldn't return Envelope\n";
    return 0;
  } 
}

int TileDddbManager::SetCurrentEnvByIndex(unsigned int envelopeI)
{
  currentTileGlob = 0;
  currentTileMod = 0;
  currentSection = 0;
  mModTypes.clear();

  if (envelopeI < n_tiglob) {
    currentTileGlob = (*tiglob)[envelopeI];
    EnvNum = currentTileGlob->getInt("PART");
    EnvSize = currentTileGlob->getInt("NMODULES");
    unsigned int ind1 = 0;
//    while (((*timod)[ind1]->getInt("PART") != static_cast<int>(EnvNum)) && (++ind1 < n_timod)) 
//      {
//      }
    // FIXME: temporary fix for wrong PART in modules table
    for ( ; ind1 < n_timod; ++ind1) {
      int part = (*timod)[ind1]->getInt("PART");
      if ( mTag == "TileCal-02" ) {
        if (part == 3) part = 5;
        else if (part == 2) part = 3;
      }
      if ( part == static_cast<int>(EnvNum)) break;
    }
    if (ind1 < n_timod) {
      EnvBegin = ind1;
      return 1;
    }
    else {
      EnvSize = 0;
      EnvBegin = 0;
      std::cerr << "TileDddbManager couldn't return begin of modules\n";
      return 0;
    }
  } else {
    currentTileGlob = 0;
    std::cerr << "TileDddbManager couldn't return Envelope\n";
    return 0;
  }
}

 
int TileDddbManager::GetEnvSide() const
{
  if (currentTileGlob) {
    return currentTileGlob->getInt("SIDE");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999\n";
    return -999;
  }  
}

int TileDddbManager::GetEnvNModules() const
{
  if (currentTileGlob) {
    return currentTileGlob->getInt("NMODULES");
    //sb return 1;
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Envelope not set, returning -999\n";
    return -999;
  } 
}

double TileDddbManager::GetEnvRin() const
{
  if (currentTileGlob) {
    return currentTileGlob->getDouble("RIN");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Envelope not set, returning -999\n";
    return -999;
  } 
}

double TileDddbManager::GetEnvRout() const
{
  if (currentTileGlob) {
    return currentTileGlob->getDouble("ROUT");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Envelope not set, returning -999\n";
    return -999;
  } 
}

double TileDddbManager::GetEnvZLength() const
{
  if (currentTileGlob) {
//    return currentTileGlob->getDouble("ZLENGTH");
    // FIXME: temporary correction for finger length
    //        all fingers for the testbeam are "small fingers"
    double len =currentTileGlob->getDouble("ZLENGTH");
    if (len > 564 + 37.7*2 && mTag == "TileCal-02") {
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
  if (currentTileGlob) {
//    return currentTileGlob->getDouble("DPHI");
    // FIXME: temporary fix for wrong dphi in table
    double dphi = currentTileGlob->getDouble("DPHI");
    if ( dphi != 0 && mTag == "TileCal-02") {
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
  if (currentTileGlob) {
//    return currentTileGlob->getDouble("DZ");
    // FIXME: temporary correction for finger length
    //        all fingers for the testbeam are "small fingers"
    double dz = currentTileGlob->getDouble("DZ");
    if ( dz > 20 && mTag == "TileCal-02") {
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
  if (currentTileGlob) {
    return currentTileGlob->getDouble("DX");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Envelope not set, returning -999\n";
    return -999;
  } 
}

double TileDddbManager::GetEnvDY() const
{
  if (currentTileGlob) {
    return currentTileGlob->getDouble("DY");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Envelope not set, returning -999\n";
    return -999;
  } 
}

double TileDddbManager::GetEnvDPsi() const
{
  if (currentTileGlob) {
    return currentTileGlob->getDouble("DPSI");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Envelope not set, returning -999\n";
    return -999;
  } 
}

double TileDddbManager::GetEnvDTheta() const
{
  if (currentTileGlob) {
    return currentTileGlob->getDouble("DTHETA");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Envelope not set, returning -999\n";
    return -999;
  } 
}

double TileDddbManager::GetEnvZShift() const
{
  if(currentTileGlob) {
    return currentTileGlob->getDouble("ZSHIFT");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Envelope not set, returning -999\n";
    return -999;
  } 
}


//------------ TILE MODULES -------------------
int TileDddbManager::SetCurrentModuleByIndex(unsigned int Id)
{
  currentTileMod = 0;
  currentSection = 0;
  mModTypes.clear();
  
  if (currentTileGlob) {
  //Modules enumerated from zero, so SetCurrentModuleByIndex(0) gives the first module in the Envelope
    if(Id >= EnvSize) { 
      std::cerr <<"\nTileDddbManager: module number is out of range\n";
      return 0;
    }
    else {
      currentTileMod = (*timod)[Id + EnvBegin];
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
  currentTileMod = 0;
  currentSection = 0;
  mModTypes.clear();

  if (currentTileGlob) {
    //Input here number from column "MODNUM"
    unsigned int ind4 = EnvBegin;
    unsigned int EnvEnd = EnvBegin + EnvSize;
    while (((*timod)[ind4]->getInt("MODNUM") != static_cast<int>(Number))&&(++ind4 < EnvEnd))
      {
      }
    if(ind4 < EnvEnd){
      currentTileMod = (*timod)[ind4];
      FillModTypes();
      return 1;
    }
    else{
      currentTileMod = 0;
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
  if (currentTileMod) {
    return currentTileMod->getInt("MODNUM");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Module not set, returning -999\n";
    return -999;
  }  
}

int TileDddbManager::GetModType() const
{
  if(currentTileMod) {
    return currentTileMod->getInt("TYP");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Module not set, returning -999\n";
    return -999;
  }
}

double TileDddbManager::GetModDX() const
{
  if (currentTileMod) {
    return currentTileMod->getDouble("DX");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Module not set, returning -999\n";
    return -999;
  }
}

double TileDddbManager::GetModDY() const
{
  if (currentTileMod) {
    return currentTileMod->getDouble("DY");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Module not set, returning -999\n";
    return -999;
  }
}

double TileDddbManager::GetModDZ() const
{
  if (currentTileMod) {
    return currentTileMod->getDouble("DZ");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Module not set, returning -999\n";
    return -999;
  }
}

double TileDddbManager::GetModDPhi() const
{
  if (currentTileMod) {
    return currentTileMod->getDouble("DPHI");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Module not set, returning -999\n";
    return -999;
  }
}

double TileDddbManager::GetModDPsi() const
{
  if (currentTileMod) {
    return currentTileMod->getDouble("DPSI");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Module not set, returning -999\n";
    return -999;
  }
}

double TileDddbManager::GetModDTheta() const
{
  if (currentTileMod) {
    return currentTileMod->getDouble("DTHETA");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Module not set, returning -999\n";
    return -999;
  }
}


//------------ TILE MODULE TYPES -------------------
int TileDddbManager::SetCurrentTypeByIndex(unsigned int Id)
{
  if (Id < mModTypes.size()) {
    return SetCurrentSection(mModTypes[Id]);
  } else {
    std::cerr << "ERROR: type index (" << Id << ") is out of range (" << mModTypes.size() << ")" << std::endl;
    currentSection = 0;
    return 0;
  }
}

int TileDddbManager::SetCurrentTypeByNumber(unsigned int Number)
{
  return SetCurrentSection(Number);
}

unsigned int TileDddbManager::GetModType(unsigned int Id) const
{
  if (Id < mModTypes.size()) {
    return mModTypes[Id];
  } else {
    return 999;
  }
}

unsigned int TileDddbManager::GetModTypeSize() const
{
  return mModTypes.size();
}

void TileDddbManager::FillModTypes()
{
  mModTypes.clear();

  int fullType = GetModType();
  
  while (fullType > 0) {
    unsigned int subType = fullType%100;
    mModTypes.push_back(subType);
    fullType /= 100;
  }
  SetCurrentTypeByIndex(0);
}


// ------------ S E C T I O N ------------------


int TileDddbManager::SetCurrentSectionByIndex(unsigned int IdSection)
{
  if (IdSection < n_tilb) {
    currentSection = (*tilb)[IdSection];
    return 1;
  }
  else {
    std::cerr << "ERROR: section index (" << IdSection << ") is out of range (" << n_tilb << ")" << std::endl;
    currentSection = 0;
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

  while (((*tilb)[ind]->getInt("SECTION") != static_cast<int>(section)) && (++ind < n_tilb)) 
  {
  }

  if(ind >= n_tilb) {
    if (print) std::cerr << "\nTileDddbManager::SetCurrentSection ERROR! Unable to find the section\n";
    currentSection = 0;
    return 0;
  }
  else {
    currentSection = (*tilb)[ind];
    return 1;
  }
}

int TileDddbManager::TILBsection() const
{
  if (currentSection) {
    return currentSection->getInt("SECTION");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999\n";
    return -999;
  }
}

int TileDddbManager::TILBnperiod() const
{
  if (currentSection) {
    return currentSection->getInt("NPERIOD");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999\n";
    return -999;
  }
}

int TileDddbManager::TILBnmodul() const
{
  if (currentSection) {
    return currentSection->getInt("NMODUL");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999\n";
    return -999;
  }
}

double TileDddbManager::TILBrmin() const
{
  if (currentSection) {
    return currentSection->getDouble("RMIN");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBrmax() const
{
  if (currentSection) {
    return currentSection->getDouble("RMAX");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBrminimal() const
{
  if (currentSection) {
    return currentSection->getDouble("RMINIMAL");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBrmaximal() const
{
  if (currentSection) {
    return currentSection->getDouble("RMAXIMAL");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBdzperio() const
{
  if (currentSection) {
    return currentSection->getDouble("DZPERIO");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBdrfront() const
{
  if(currentSection) {
    return currentSection->getDouble("DRFRONT");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBdzend() const
{
  if (currentSection) {
    return currentSection->getDouble("DZEND");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBflangex() const
{
  if (currentSection) {
    return currentSection->getDouble("FLANGEX");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBflangey() const
{
  if (currentSection) {
    return currentSection->getDouble("FLANGEY");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBzoffset() const
{
  if (currentSection) {
    return currentSection->getDouble("ZOFFSET");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBphigap() const
{
  if (currentSection) {
    return currentSection->getDouble("PHIGAP");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBisci_st() const
{
  if (currentSection) {
    return currentSection->getInt("ISCI_ST");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBdzmodul() const
{
  if (currentSection) {
    return currentSection->getDouble("DZMODUL");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBdzmast() const
{
  if (currentSection) {
    return currentSection->getDouble("DZMAST");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBdzspac() const
{
  if (currentSection) {
    return currentSection->getDouble("DZSPAC");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBdzend1() const
{
  if (currentSection) {
    return currentSection->getDouble("DZEND1");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TILBdzend2() const
{
  if (currentSection) {
    return currentSection->getDouble("DZEND2");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999.9\n";
    return -999.9;
  }
}

int TileDddbManager::TILBngirder() const
{
  if (currentSection) {
    return currentSection->getInt("NGIRDER");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999\n";
    return -999;
  }
}

int TileDddbManager::TILBnscin() const
{
  if (currentSection) {
    return currentSection->getInt("NSCIN");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999\n";
    return -999;
  }
}

int TileDddbManager::TILBfingpattern() const
{
  if (currentSection) {
    return currentSection->getInt("FINGPATTERN");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999\n";
    return -999;
  }
}

int TileDddbManager::TILBnpercutpos() const
{
  if (currentSection) {
    return currentSection->getInt("NPERCUTPOS");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999\n";
    return -999;
  }
}

int TileDddbManager::TILBnpercutneg() const
{
  if (currentSection) {
    return currentSection->getInt("NPERCUTNEG");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999\n";
    return -999;
  }
}


int TileDddbManager::TILBcurscint() const
{
  if(currentSection) {
    if(currentSection->isFieldNull("CURSCINT"))
      return 0;
    else
      return currentSection->getInt("CURSCINT");
  }
  else {
    std::cerr <<"\nTileDddbManager ERROR! Current Section not set, returning -999\n";
    return -999;
  }
}

double TileDddbManager::TILBdzgir() const
{
  if (currentSection) {
    return currentSection->getDouble("DZGIR");
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

  while (((*scnt)[ind]->getInt("ITEM") != item) && (++ind < n_scnt)) 
  {
  }

  if(ind >= n_scnt)
  {
    std::cerr << "\nTileDddbManager::SetCurrentScin ERROR! Unable to find the scintillator, item = " << item << "\n";
    return 0;
  }
  else
  {
    currentScin = (*scnt)[ind];
    return 1;
  }
}

int TileDddbManager::SCNTitem() const
{
  if(currentScin)
    return currentScin->getInt("ITEM");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current Scintillator not set, returning -999\n";
    return -999;
  }
}

double TileDddbManager::SCNTdr() const
{
  if(currentScin)
    return currentScin->getDouble("DR");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current Scintillator not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::SCNTrc() const
{
  if(currentScin)
    return currentScin->getDouble("RC");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current Scintillator not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::SCNTzp() const
{
  if(currentScin)
    return currentScin->getDouble("ZP");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current Scintillator not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::SCNTdrw() const
{
  if(currentScin)
    return currentScin->getDouble("DRW");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current Scintillator not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::SCNTdt() const
{
  if(currentScin)
    return currentScin->getDouble("DT");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current Scintillator not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::SCNTdtw() const
{
  if(currentScin)
    return currentScin->getDouble("DTW");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current Scintillator not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::SCNTdphi() const
{
  if(currentScin)
    return currentScin->getDouble("DPHI");
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

     while (((*cuts)[ind]->getString("VOLUME") != name) && (++ind < n_cuts)) 
     { 
     }

     if(ind >= n_cuts)
     { std::cerr << "\nTileDddbManager::SetCuts ERROR! Unable to find the currentCuts volume name = " << name << "\n";
       return 0;
     }
     else
     { currentCuts = (*cuts)[ind];
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
   if (currentCuts)
     return currentCuts->getString("VOLUME");
   else
   { std::cerr << "\nTileDddbManager ERROR! Current VOLUME dimension not set, returning -999.9\n";
     return output;
   } 
}

double TileDddbManager::CutsDX1() const
{
  if (currentCuts)
    return (double)currentCuts->getFloat("DX1");
  else
  { std::cerr << "\nTileDddbManager ERROR! Current dX1 dimension not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::CutsDX2() const
{
  if (currentCuts)
    return (double)currentCuts->getFloat("DX2");
  else
  { std::cerr << "\nTileDddbManager ERROR! Current dX2 dimension not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::CutsDY1() const
{
  if (currentCuts)
    return (double)currentCuts->getFloat("DY1");
  else
  { std::cerr << "\nTileDddbManager ERROR! Current dY1 dimension not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::CutsDY2() const
{
  if (currentCuts)
    return (double)currentCuts->getFloat("DY2");
  else
  { std::cerr << "\nTileDddbManager ERROR! Current dY2 dimension not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::CutsDZ1() const
{
  if (currentCuts)
    return (double)currentCuts->getFloat("DZ1");
  else
  { std::cerr << "\nTileDddbManager ERROR! Current dZ1 dimension not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::CutsXpos() const
{
  if (currentCuts)
    return (double)currentCuts->getFloat("XPOS");
  else
  { std::cerr << "\nTileDddbManager ERROR! Current Xpos dimension not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::CutsYpos() const
{
  if (currentCuts)
    return (double)currentCuts->getFloat("YPOS");
  else
  { std::cerr << "\nTileDddbManager ERROR! Current Ypos dimension not set, returning -999.9\n";
    return -999.9;
  }
}
 
// -------------------- SADDLE SUPPORT ---------------------

int TileDddbManager::SetCurrentSaddle(unsigned int index)
{
  if (m_buildSaddle) 
   { if(index >= n_saddle)
     { return 0;
     }
      else
     { currentSaddle = (*saddle)[index];
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
  if (currentSaddle)
    return (double)currentSaddle->getFloat("DZSADDLESUPPORT");
  else
  { std::cerr << "\nTileDddbManager ERROR! DzSaddleSupport dimension not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::RadiusSaddle() const
{
  if (currentSaddle)
    return (double)currentSaddle->getFloat("RADIUSSADDLE");
  else
  { std::cerr << "\nTileDddbManager ERROR! RadiusSaddle dimension not set, returning -999.9\n";
    return -999.9;
  }
}

// -------------------- G I R D E R  ---------------------
int TileDddbManager::SetCurrentGird(int item)
{
  unsigned int ind = 0;

  while (((*tigr)[ind]->getInt("ITEM") != item) && (++ind < n_tigr)) 
  {
  }

  if(ind >= n_tigr)
  {
    //std::cerr << "\nTileDddbManager::SetCurrentGird ERROR! Unable to find the girder element,item= " << item << "\n";
    return 0;
  }
  else
  {
    currentGird = (*tigr)[ind];
    return 1;
  }
}

int TileDddbManager::TIGRitem() const
{
  if(currentGird)
    return currentGird->getInt("ITEM");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current Girder element not set, returning -999\n";
    return -999;
  }
}

int TileDddbManager::TIGRmaterial() const
{
  if(currentGird)
    return currentGird->getInt("MATERIAL");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current Girder element not set, returning -999\n";
    return -999;
  }
}

double TileDddbManager::TIGRrc() const
{
  if(currentGird)
    return currentGird->getDouble("RC");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current Girder element not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TIGRdr() const
{
  if(currentGird)
    return currentGird->getDouble("DR");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current Girder element not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TIGRdw() const
{
  if(currentGird)
    return currentGird->getDouble("DW");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current Girder element not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TIGRoff() const
{
  if(currentGird)
    return currentGird->getDouble("OFF");
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

  while (((*tifg)[ind]->getInt("SECTION") != section) && (++ind < n_tifg)) 
  {
  }

  if(ind >= n_tifg)
  {
    //    std::cerr << "\nTileDddbManager::SetCurrentTifg ERROR! Unable to find the finger, section = " << section << "\n";
    return 0;
  }
  else
  {
    currentTifg = (*tifg)[ind];
    return 1;
  }
}

int TileDddbManager::TIFGsection() const
{
  if(currentTifg)
    return currentTifg->getInt("SECTION");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TIFG not set, returning -999\n";
    return -999;
  }
}

int TileDddbManager::TIFGnelem() const
{
  if(currentTifg)
    return currentTifg->getInt("NELEM");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TIFG not set, returning -999\n";
    return -999;
  }
}

double TileDddbManager::TIFGdz() const
{
  if(currentTifg)
    return currentTifg->getDouble("DZ");
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

  while (((*ticg)[ind]->getInt("ITEM") != item) && (++ind < n_ticg)) 
  {
  }

  if(ind >= n_ticg)
  {
    //    std::cerr << "\nTileDddbManager::SetCurrentTicg ERROR! Unable to find the finger element, item = " << item << "\n";
    return 0;
  }
  else
  {
    currentTicg = (*ticg)[ind];
    return 1;
  }
}

int TileDddbManager::TICGitem() const
{
  if(currentTicg)
    return currentTicg->getInt("ITEM");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICG not set, returning -999\n";
    return -999;
  }
}

int TileDddbManager::TICGmaterial() const
{
  if(currentTicg)
    return currentTicg->getInt("MATERIAL");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICG not set, returning -999\n";
    return -999;
  }
}

int TileDddbManager::TICGshape() const
{
  if(currentTicg)
    return currentTicg->getInt("SHAPE");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICG not set, returning -999\n";
    return -999;
  }
}

double TileDddbManager::TICGrc() const
{
  if(currentTicg)
    return currentTicg->getDouble("RC");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICG not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICGdr() const
{
  if(currentTicg)
    return currentTicg->getDouble("DR");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICG not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICGdx1() const
{
  if(currentTicg)
    return currentTicg->getDouble("DX1");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICG not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICGdx2() const
{
  if(currentTicg)
    return currentTicg->getDouble("DX2");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICG not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICGoff() const
{
  if(currentTicg)
    return currentTicg->getDouble("OFF");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICG not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICGdz() const
{
  if(currentTicg)
    return currentTicg->getDouble("DZ");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICG not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICGzc() const
{
  if(currentTicg)
    return currentTicg->getDouble("ZC");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICG not set, returning -999.9\n";
    return -999.9;
  }
}

unsigned int TileDddbManager::GetNumTicl() const
{ 
  return n_ticl;
}

// -------------------- T I C L  ---------------------
int TileDddbManager::SetCurrentTicl(int detector, double sample, double tower)
{
  unsigned int detector_ind = 0;

  // Looking for the first cell of given detector
  while (((*ticl)[detector_ind]->getInt("DETECTOR") != detector) && (++detector_ind < n_ticl)) 
  {
  }

  if(detector_ind >= n_ticl)
  {
    //    std::cerr << "\nTileDddbManager::SetCurrentTicl ERROR! Unable to find any cell for the detector = " 
    //	      << detector << "\n";
    return 0;
  }
  else
  {
    // Looking for the first cell in given sample of given detector
    unsigned int sample_ind = detector_ind;

    while (((*ticl)[sample_ind]->getInt("SAMPLE") != sample) && (++sample_ind < n_ticl)) 
    {
    }

    if(sample_ind >= n_ticl)
    {
      //      std::cerr << "\nTileDddbManager::SetCurrentTicl ERROR! Unable to find any cell for sample = " 
      //		<< sample << " in the detector = " << detector << "\n";
      return 0;
    }
    else
    {
      // Looking for the cell with give tower inside found sample
      unsigned int ind = sample_ind;

      while (((*ticl)[ind]->getInt("TOWER") != tower) && (++ind < n_ticl))
      {
      }

      if(ind >= n_ticl)
      {
	//	std::cerr << "\nTileDddbManager::SetCurrentTicl ERROR! Unable to find any cell for tower = " 
	//		  << tower << " in the detector = " << detector << " and sample = " << sample << "\n";
	return 0;
      }
      else
      {
	currentTicl = (*ticl)[ind];
	currentTiclInd = ind;
	return 1;
      }
    }
  }
}

int TileDddbManager::SetCurrentTicl(unsigned int index)
{
  if(index >= n_ticl)
  {
    //    std::cerr << "\nTileDddbManager::SetCurrentTicl ERROR! The requested index = "  
    //	      << index << " out of range.\n";
    return 0;
  }
  else
  {
    currentTicl = (*ticl)[index];
    currentTiclInd = index;
    return 1;
  }
}

int TileDddbManager::SetFirstTiclInDet(int detector)
{
  unsigned int ind = 0;

  // Looking for the first cell of given detector
  while (((*ticl)[ind]->getInt("DETECTOR") != detector) && (++ind < n_ticl)) 
  {
  }

  if(ind >= n_ticl)
    return 0;  // FAILURE

  currentTicl = (*ticl)[ind];
  currentTiclInd = ind;
  return 1;  // SUCCESS

}

int TileDddbManager::SetFirstTiclInDetSamp(int detector, double sample)
{
  unsigned int detector_ind = 0;

  // Looking for the first cell of given detector
  while (((*ticl)[detector_ind]->getInt("DETECTOR") != detector) && (++detector_ind < n_ticl)) 
  {
  }

  if(detector_ind >= n_ticl)
    return 0;  // FAILURE
  else
  {
    // Looking for the first cell in given sample of given detector
    unsigned int sample_ind = detector_ind;

    while (((*ticl)[sample_ind]->getInt("SAMPLE") != sample) && (++sample_ind < n_ticl)) 
    {
    }

    if(sample_ind >= n_ticl)
      return 0;  // FAILURE

    currentTicl = (*ticl)[sample_ind];
    currentTiclInd = sample_ind;
    return 1;  // SUCCESS
  }
}

int TileDddbManager::SetNextTiclInDet()
{
  if(currentTiclInd<0) return 0; // FAILURE

  int currentDetector = (*ticl)[currentTiclInd]->getInt("DETECTOR");

  if((++currentTiclInd == static_cast<int>(n_ticl))||((*ticl)[currentTiclInd]->getInt("DETECTOR") != currentDetector))
  {
    currentTiclInd--;
    return 0;  // FAILURE
  }

  currentTicl = (*ticl)[currentTiclInd];
  return 1;
}

int TileDddbManager::SetNextTiclInDetSamp()
{
  if(currentTiclInd<0) return 0; // FAILURE

  int currentDetector = (*ticl)[currentTiclInd]->getInt("DETECTOR");
  float currentSample = (*ticl)[currentTiclInd]->getInt("SAMPLE");

  if((++currentTiclInd == static_cast<int>(n_ticl))||
     ((*ticl)[currentTiclInd]->getInt("DETECTOR") != currentDetector)||
     ((*ticl)[currentTiclInd]->getInt("SAMPLE") != currentSample))
  {
    currentTiclInd--;
    return 0;  // FAILURE
  }

  currentTicl = (*ticl)[currentTiclInd];
  return 1;
}

int TileDddbManager::TICLdetector() const
{
  if(currentTicl)
    return currentTicl->getInt("DETECTOR");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICL not set, returning -999\n";
    return -999;
  }
}

double TileDddbManager::TICLncell() const
{
  if(currentTicl)
    return currentTicl->getInt("NCELL");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICL not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICLtower() const
{
  if(currentTicl)
    return currentTicl->getInt("TOWER");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICL not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICLsample() const
{
  if(currentTicl)
    return currentTicl->getInt("SAMPLE");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICL not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICLeta() const
{
  if(currentTicl)
    return currentTicl->getDouble("ETA");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICL not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICLdeta() const
{
  if(currentTicl)
    return currentTicl->getDouble("DETA");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICL not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICLfirstrow() const
{
  if(currentTicl)
    return currentTicl->getInt("FIRSTROW");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICL not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICLlastrow() const
{
  if(currentTicl)
    return currentTicl->getInt("LASTROW");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICL not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICLntilesrow(unsigned int ind) const
{
  if(currentTicl)
    return currentTicl->getInt("NTILESROW",ind);
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICL not set, returning 9999\n";
    return 9999;
  }
}

double TileDddbManager::TICLnpmt() const
{
  if(currentTicl)
    return currentTicl->getInt("NPMT");
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICL not set, returning -999.9\n";
    return -999.9;
  }
}

double TileDddbManager::TICLholes(unsigned int ind) const
{
  if(currentTicl)
    return currentTicl->getInt("HOLES",ind);
  else
  {
    std::cerr << "\nTileDddbManager ERROR! Current TICL not set, returning 9999\n";
    return 9999;
  }
}

bool TileDddbManager::addPlatesToCell() const
{
  if (tileSwitches)
    {
      if ((*tileSwitches)[0]->isFieldNull("ADDPLATESTOCELL"))
        {
          // std::cerr << "\nTileDddbManager   WARNING TileSwitches(ADDPLATESTOCELL) is EMPTY, returning TRUE\n\n";
          return true;
        }
      else
	{  
	  return (*tileSwitches)[0]->getInt("ADDPLATESTOCELL");
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
  if (tileSwitches)
    {
        try {
          if ((*tileSwitches)[0]->isFieldNull("USHAPE"))
            {
              // std::cerr << "\nTileDddbManager   WARNING TileSwitches(USHAPE) is EMPTY, returning Ushape= 0\n\n";
              return 0;
            }
          else
	    {  
	      return (*tileSwitches)[0]->getInt("USHAPE");
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

