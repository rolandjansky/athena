/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: TGCElectronicsSystem.cxx,v 1.13 2009-05-04 11:39:44 isaya Exp $
#include "TrigT1TGC/TGCElectronicsSystem.hh"
#include "TrigT1TGC/TGCASDOut.h"
#include "TrigT1TGC/TGCDatabaseManager.hh"
#include "TrigT1TGC/TGCSector.hh"

#include <iostream>
#include <vector>

namespace LVL1TGCTrigger {

void TGCElectronicsSystem::distributeSignal(LVL1TGCTrigger::TGCEvent* event)
{
  std::vector<LVL1TGCTrigger::TGCASDOut*>& vecASDOut= event->GetASDOutVector();
  event=0;
  size_t i;
  for( i=0; i< vecASDOut.size(); i++) {
    const LVL1TGCTrigger::TGCASDOut* asd= vecASDOut[i];
    int side = asd->GetTGCReadoutIndex().GetSideNumber();
    int octant = asd->GetTGCReadoutIndex().GetOctantNumber();
    int module = asd->GetTGCReadoutIndex().GetModuleNumber();

    if(getSector(side, octant, module)){
      if(side < NumberOfSide && octant < NumberOfOctant) 
        getSector(side, octant, module)->distributeSignal(asd);
    }
    asd = 0;
  }
  
}

TGCElectronicsSystem::TGCElectronicsSystem(TGCDatabaseManager* database,
					   bool                isAtlas):
  DB(database),
  tmdb(0),
  fAtlas(isAtlas)
{
  // TileMu
  tmdb = new TGCTMDB();

  int side,oct,mod,SectorId;
  LVL1TGCTrigger::TGCRegionType RegionType;
  LVL1TGCTrigger::TGCForwardBackwardType forwardBackward;
  for(side=0; side < NumberOfSide; side++){
    for(oct=0; oct < NumberOfOctant; oct++){
      for(mod=0; mod < NumberOfModule; mod++){
        sector[side][oct][mod]=0;
        SectorId   = getSectorId(side,oct,mod);
        RegionType = getRegionType(mod);
        forwardBackward = getForwardBackward(side,oct,mod);
        sector[side][oct][mod] = new TGCSector(SectorId, RegionType, 
					       forwardBackward, 
					       DB,
					       tmdb);
      } // loop module
    } // loop octant
  } //loop side


}

LVL1TGCTrigger::TGCRegionType TGCElectronicsSystem::getRegionType(int mod) const
{
  // Module:                
  //   <----phi-----      
  //  7   6  4   3  1   0   Endcap                    
  //    8      5      2     Forward
  //
  //  Inner
  //   <----phi-----      
  //      11   10   9   Endcap                   
  //  14     13   12    Forward

  if(mod<9)
    if(mod%3==2) return LVL1TGCTrigger::Forward;
    else return LVL1TGCTrigger::Endcap;
  else if(mod<12)
    return LVL1TGCTrigger::Endcap;
  else
    return LVL1TGCTrigger::Forward;
}

TGCForwardBackwardType TGCElectronicsSystem::getForwardBackward(int side, int oct, int mod) const
{
  bool isEndcap = false;
  bool isInner = false;
  int sec=-1;
  if(mod<9){
    if(mod%3==2)
      sec = mod/3+oct*3;
    if(mod%3==0||mod%3==1){
      sec = mod-mod/3+oct*6;
      isEndcap = true;
    }
  } else if(mod<12){
    isEndcap = true;
    isInner = true;
    sec = mod%3+oct*3;
  } else {
    isInner = true;
    sec = mod%3+oct*3;
  }
  
  TGCForwardBackwardType forwardBackward = ForwardSector;
  if (fAtlas) {
    // for new cabling service 
    // A-side : side == 0
    // C-side : side == 1 
    bool isAside = (side ==0);

    // backward/Forward definition is same 
    // compared with TGCIdBase::isBackward() in TGCcablingInterface
    // because strip layer is swapped in default
    if ( isEndcap ){
      if (!isInner){
	if(isAside) {
	  if (sec%2==0) forwardBackward = ForwardSector;
	  else          forwardBackward = BackwardSector;
	} else {
	  if (sec%2==1) forwardBackward = ForwardSector;
	  else          forwardBackward = BackwardSector;
	}
      } else {
	// EI
	// Special case of EI11
	if (sec == 15) {
	  if(isAside ) forwardBackward = ForwardSector;
	  else          forwardBackward = BackwardSector;
	} else if (sec == 16) {
	  if(isAside ) forwardBackward = BackwardSector;
	  else          forwardBackward = ForwardSector;
	} else {
	  if (isAside) {  
	    if (sec%3==2) forwardBackward = BackwardSector;
	    else          forwardBackward = ForwardSector;
	  } else {
	    if (sec%3!=2) forwardBackward = BackwardSector;
	  else          forwardBackward = ForwardSector;
	  }
	}
      }
    } else {
      if(isAside) forwardBackward = BackwardSector; // all Backward
      else         forwardBackward = ForwardSector;  // all Forward 
    }
  } else {
    // old cabling (8-fold)
    if (sec%2==side) forwardBackward = BackwardSector;
  }
  return forwardBackward;
}

int TGCElectronicsSystem::getSectorId(int side, int oct, int mod) const
{
  return NumberOfModule*NumberOfOctant*side+NumberOfModule*oct+mod;
}

TGCSector* TGCElectronicsSystem::getSector(LVL1TGCTrigger::TGCReadoutIndex index) const
{ 
  if (index.GetSideNumber()<0) return 0;
  return sector[index.GetSideNumber()][index.GetOctantNumber()][index.GetModuleNumber()];
}

TGCElectronicsSystem::~TGCElectronicsSystem()
{
  int i,j,k;
  for( i=0; i<NumberOfSide; i+=1){
    for( j=0; j<NumberOfOctant; j+=1){
      for( k=0; k<NumberOfModule; k+=1){
        if(sector[i][j][k]!=0) delete sector[i][j][k];
        sector[i][j][k]=0;
      } // loop module
    } // loop octant
  } // loop side

  if (tmdb) delete tmdb;
}

TGCElectronicsSystem::TGCElectronicsSystem(const TGCElectronicsSystem& right)
{
  fAtlas = right.fAtlas;
  if(DB!=0) delete DB;
  DB = new TGCDatabaseManager;
  *DB = *right.DB;

  int i,j,k;
  for( i=0; i<NumberOfSide; i+=1){
    for( j=0; j<NumberOfOctant; j+=1) {
      for( k=0; k<NumberOfModule; k+=1){
        if(sector[i][j][k]!=0) delete sector[i][j][k];
        sector[i][j][k]=0;
        sector[i][j][k] = new TGCSector;
        if(right.sector[i][j][k]!=0)  {
	  *sector[i][j][k]=*right.sector[i][j][k];
	}
      } // loop module
    } // loop octant
  } // loop side

  tmdb = right.tmdb;
}

TGCElectronicsSystem& TGCElectronicsSystem::operator=(const TGCElectronicsSystem& right)
{
  if(this!=&right){
    if(DB!=0) delete DB;
    DB = new TGCDatabaseManager;
    *DB = *right.DB;

    int i,j,k;
    for( i=0; i<NumberOfSide; i+=1) {
      for( j=0; j<NumberOfOctant; j+=1) {
	for( k=0; k<NumberOfModule; k+=1){
	  if(sector[i][j][k]!=0) delete sector[i][j][k];
	  sector[i][j][k]=0;
	  sector[i][j][k] = new TGCSector;
	  if(right.sector[i][j][k]!=0){
	    *sector[i][j][k]=*right.sector[i][j][k];
	  }
	} // loop module
      } // loop octant
    } // loop side
    tmdb = right.tmdb;  
  }  
  return *this;
}
  
} //end of namespace bracket
