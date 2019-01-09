/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
  if(m_sector==-1) return -1;
  
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
  this->m_side=side; 
}
  
void TGCId::setModuleType (ModuleType module)  {
  this->m_module=module;
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
  this->m_signal=signal; 
  if(m_multiplet==Inner&&signal==Wire)  m_module=WI;
  if(m_multiplet==Inner&&signal==Strip)  m_module=SI;
  if(m_multiplet==Doublet&&signal==Wire)  m_module=WD;
  if(m_multiplet==Doublet&&signal==Strip) m_module=SD;
  if(m_multiplet==Triplet&&signal==Wire)  m_module=WT;
  if(m_multiplet==Triplet&&signal==Strip) m_module=ST;
}

void TGCId::setMultipletType (MultipletType multiplet)  {
  this->m_multiplet=multiplet;
  if(multiplet==Inner&&m_signal==Wire)  m_module=WI;
  if(multiplet==Inner&&m_signal==Strip)  m_module=SI;
  if(multiplet==Doublet&&m_signal==Wire)  m_module=WD;
  if(multiplet==Doublet&&m_signal==Strip) m_module=SD;
  if(multiplet==Triplet&&m_signal==Wire)  m_module=WT;
  if(multiplet==Triplet&&m_signal==Strip) m_module=ST;
}
   
void TGCId::setRegionType (RegionType region)  { 
  this->m_region=region; 
}
   
void TGCId::setStation (int station)  { 
  this->m_station=station;
  if(station==0) setMultipletType(Triplet);	  
  if(station==1) setMultipletType(Doublet);
  if(station==2) setMultipletType(Doublet);
  if(station==3) setMultipletType(Inner);
}
   
void TGCId::setSector (int sector)  { 
  this->m_sector=sector;
  if(m_region==Endcap) {
    if(m_multiplet==Inner) m_octant=sector/3;
    else m_octant=sector/6;
  }
  if(m_region==Forward) m_octant=sector/3;
}
   
void TGCId::setOctant (int octant)  { 
  this->m_octant=octant;
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
  this->m_layer=layer;
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
    if(m_sector!=-1)
      m_octant=m_sector/3;
  }
}

void TGCId::setChamber (int chamber)  { 
  this->m_chamber=chamber;
}
   
void TGCId::setId (int id)  { 
  this->m_id=id;
}
   
void TGCId::setBlock (int block)  { 
  this->m_block=block;
}
   
void TGCId::setChannel (int channel)  {
  this->m_channel=channel; 
}  
   
void TGCId::setEvent (int event) {
  this->m_event=event;
}
   
void TGCId::setBunch (int bunch) {
  this->m_bunch=bunch;
}

} // end of namespace
