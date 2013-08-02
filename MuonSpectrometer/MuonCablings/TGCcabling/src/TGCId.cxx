/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling/TGCId.h"

namespace LVL1TGCCabling8
{

int TGCId::getGasGap (void) const {
  if(getLayer()==0||getLayer()==3||getLayer()==5||getLayer()==7)
    return 1;
  if(getLayer()==1||getLayer()==4||getLayer()==6||getLayer()==8)
    return 2;
  if(getLayer()==2)
    return 3;
  return -1;
}

int TGCId::getSectorInOctant (void) const {
  if(isInner()){
    return getSector()%3;
  }
  if(isEndcap()){
    return getSector()%6;
  }
  if(isForward()){
    return getSector()%3;
  }
  return -1;
}
  
int TGCId::getSectorModule (void) const {
  if(sector==-1) return -1;
  
  if(isEndcap()){
    if(isInner()) return getSectorInOctant()+9;
    return getSectorInOctant()+getSectorInOctant()/2;
  }
  if(isForward()){
    if(isInner()) return getSectorInOctant()+12;
    return getSectorInOctant()*3+2;
  }
  return -1;
}

void TGCId::setSideType (SideType side) {
  this->side=side; 
}
  
void TGCId::setModuleType (ModuleType module)  {
  this->module=module;
  if(module==WI){
    setSignalType(Wire);
    setMultipletType(Inner);
  }
  if(module==SI){
    setSignalType(Strip);
    setMultipletType(Inner);
  }
  if(module==WD){
    setSignalType(Wire);
    setMultipletType(Doublet);
  }
  if(module==SD){
    setSignalType(Strip);
    setMultipletType(Doublet);
  }
  if(module==WT){
    setSignalType(Wire);
    setMultipletType(Triplet);
  }
  if(module==ST){
    setSignalType(Strip);
    setMultipletType(Triplet);
  }
}
   
void TGCId::setSignalType (SignalType signal)  { 
  this->signal=signal; 
  if(multiplet==Inner&&signal==Wire)  module=WI;
  if(multiplet==Inner&&signal==Strip)  module=SI;
  if(multiplet==Doublet&&signal==Wire)  module=WD;
  if(multiplet==Doublet&&signal==Strip) module=SD;
  if(multiplet==Triplet&&signal==Wire)  module=WT;
  if(multiplet==Triplet&&signal==Strip) module=ST;
}

void TGCId::setMultipletType (MultipletType multiplet)  {
  this->multiplet=multiplet;
  if(multiplet==Inner&&signal==Wire)  module=WI;
  if(multiplet==Inner&&signal==Strip)  module=SI;
  if(multiplet==Doublet&&signal==Wire)  module=WD;
  if(multiplet==Doublet&&signal==Strip) module=SD;
  if(multiplet==Triplet&&signal==Wire)  module=WT;
  if(multiplet==Triplet&&signal==Strip) module=ST;
}
   
void TGCId::setRegionType (RegionType region)  { 
  this->region=region; 
}
   
void TGCId::setStation (int station)  { 
  this->station=station;
  if(station==0) setMultipletType(Triplet);	  
  if(station==1) setMultipletType(Doublet);
  if(station==2) setMultipletType(Doublet);
  if(station==3) setMultipletType(Inner);
}
   
void TGCId::setSector (int sector)  { 
  this->sector=sector;
  if(region==Endcap) {
    if(multiplet==Inner) octant=sector/3;
    else octant=sector/6;
  }
  if(region==Forward) octant=sector/3;
}
   
void TGCId::setOctant (int octant)  { 
  this->octant=octant;
}

// before this method, set octant.
void TGCId::setSectorModule (int sectorModule) {
  if(sectorModule>=0&&sectorModule<9){
    if(sectorModule%3==2){
      setRegionType(Forward);
      if(getOctant()!=-1)
	setSector(getOctant()*3+sectorModule/3);
    } else {
      setRegionType(Endcap);
      if(getOctant()!=-1)
	setSector(getOctant()*6+sectorModule-sectorModule/3);
    }
  } else if(sectorModule>=9&&sectorModule<12) {
    setRegionType(Endcap);
    if(getOctant()!=-1)
      setSector(getOctant()*3+sectorModule-9);
  } else if(sectorModule>=12&&sectorModule<15) {
    setRegionType(Forward);
    if(getOctant()!=-1)
      setSector(getOctant()*3+sectorModule-12);
  }
}
   
void TGCId::setLayer (int layer)  { 
  this->layer=layer;
  if(layer>=0&&layer<=2){
    setMultipletType(Triplet);
    setStation(0);
  }
  if(layer>=3&&layer<=4){
    setMultipletType(Doublet);
    setStation(1);
  }
  if(layer>=5&&layer<=6){
    setMultipletType(Doublet);
    setStation(2);
  }
  if(layer>=7&&layer<=8){
    setMultipletType(Inner);
    setStation(3);
    if(sector!=-1)
      octant=sector/3;
  }
}

void TGCId::setChamber (int chamber)  { 
  this->chamber=chamber;
}
   
void TGCId::setId (int id)  { 
  this->id=id;
}
   
void TGCId::setBlock (int block)  { 
  this->block=block;
}
   
void TGCId::setChannel (int channel)  {
  this->channel=channel; 
}  
   
void TGCId::setEvent (int event) {
  this->event=event;
}
   
void TGCId::setBunch (int bunch) {
  this->bunch=bunch;
}

} // end of namespace
