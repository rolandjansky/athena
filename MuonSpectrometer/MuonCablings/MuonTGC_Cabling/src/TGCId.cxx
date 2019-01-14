/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCId.h"

namespace MuonTGC_Cabling
{

int TGCId::getGasGap(void) const {
  if(getLayer()==0||getLayer()==3||getLayer()==5||getLayer()==7)
    return 1;
  if(getLayer()==1||getLayer()==4||getLayer()==6||getLayer()==8)
    return 2;
  if(getLayer()==2)
    return 3;
  return -1;
}

int TGCId::getSectorInOctant(void) const {
  if(isInner()){
    return getSector()%(NumberOfInnerSector/NumberOfOctant);
  }
  if(isEndcap()){
    return getSector()%(NumberOfEndcapSector/NumberOfOctant);
  }
  if(isForward()){
    return getSector()%(NumberOfForwardSector/NumberOfOctant);
  }
  return -1;
}

int TGCId::getSectorInReadout(void) const {
  if(isInner()){
    return getSector()%(NumberOfInnerSector/NumberOfReadoutSector);
  }
  if(isEndcap()){
    return getSector()%(NumberOfEndcapSector/NumberOfReadoutSector);
  }
  if(isForward()){
    return getSector()%(NumberOfForwardSector/NumberOfReadoutSector);
  }
  return -1;
}
  

void TGCId::setSideType(SideType v_side) {
  m_side = v_side; 
}
  
void TGCId::setModuleType(ModuleType v_module)  {
  m_module=v_module;
  if(m_module==WI){
    setSignalType(Wire);
    setMultipletType(Inner);
  }
  if(m_module==SI){
    setSignalType(Strip);
    setMultipletType(Inner);
  }
  if(m_module==WD){
    setSignalType(Wire);
    setMultipletType(Doublet);
  }
  if(m_module==SD){
    setSignalType(Strip);
    setMultipletType(Doublet);
  }
  if(m_module==WT){
    setSignalType(Wire);
    setMultipletType(Triplet);
  }
  if(m_module==ST){
    setSignalType(Strip);
    setMultipletType(Triplet);
  }
}
   
void TGCId::setSignalType(SignalType v_signal)  { 
  m_signal = v_signal; 
  if(m_multiplet==Inner&&m_signal==Wire)  m_module=WI;
  if(m_multiplet==Inner&&m_signal==Strip)  m_module=SI;
  if(m_multiplet==Doublet&&m_signal==Wire)  m_module=WD;
  if(m_multiplet==Doublet&&m_signal==Strip) m_module=SD;
  if(m_multiplet==Triplet&&m_signal==Wire)  m_module=WT;
  if(m_multiplet==Triplet&&m_signal==Strip) m_module=ST;
}

void TGCId::setMultipletType(MultipletType v_multiplet)  {
  this->m_multiplet = v_multiplet;
  if(m_multiplet==Inner&&m_signal==Wire)  m_module=WI;
  if(m_multiplet==Inner&&m_signal==Strip)  m_module=SI;
  if(m_multiplet==Doublet&&m_signal==Wire)  m_module=WD;
  if(m_multiplet==Doublet&&m_signal==Strip) m_module=SD;
  if(m_multiplet==Triplet&&m_signal==Wire)  m_module=WT;
  if(m_multiplet==Triplet&&m_signal==Strip) m_module=ST;
}
   
void TGCId::setRegionType(RegionType v_region)  { 
  m_region = v_region; 
}
   
void TGCId::setStation(int v_station)  { 
  m_station = v_station;
  if(m_station==0) setMultipletType(Triplet);	  
  if(m_station==1) setMultipletType(Doublet);
  if(m_station==2) setMultipletType(Doublet);
  if(m_station==3) setMultipletType(Inner);
}
   
void TGCId::setReadoutSector(int v_sector)  { 
  m_sectorRO = v_sector;
}

void TGCId::setSector(int v_sector)  { 
  m_sector = v_sector;
  if(m_region==Endcap) {
    if(m_multiplet==Inner) {
      m_octant=m_sector /(NumberOfInnerSector/NumberOfOctant);
      m_sectorRO=m_sector /(NumberOfInnerSector/NumberOfReadoutSector);
    } else {
      m_octant=m_sector /(NumberOfEndcapSector/NumberOfOctant);
      m_sectorRO=m_sector /(NumberOfEndcapSector/NumberOfReadoutSector);
    }
  } else if(m_region==Forward) {
    m_octant=m_sector /(NumberOfForwardSector/NumberOfOctant);
    m_sectorRO=m_sector/(NumberOfForwardSector/NumberOfReadoutSector);
  }
}
   
void TGCId::setOctant(int v_octant)  { 
  m_octant = v_octant;
}

int TGCId::getSectorModule(void) const {
  if(m_sector==-1) return -1;

  static int moduleEndcap[6]={ 0, 1, 3, 4, 6, 7};
  static int moduleForward[3]={ 2, 5, 8};
  static int moduleEI[3]={  9, 10, 11};
  static int moduleFI[3]={ 12, 13, 14};
 
  if(isEndcap()){
    if(isInner()) return moduleEI[getSectorInOctant()];
    return moduleEndcap[getSectorInOctant()];
  }
  if(isForward()){
    if(isInner()) return moduleFI[getSectorInOctant()];
    return moduleForward[getSectorInOctant()];
  }
  return -1;
}

// before this method, set m_octant.
void TGCId::setSectorModule(int sectorModule) {
  if(m_octant <0) return;

  const int MaxModuleInOctant = 15;
  static int regionId[MaxModuleInOctant] ={ 
    0, 0, 1, 0, 0, 1, 0, 0, 1, 2, 2, 2, 3, 3, 3 
  } ;
  static int sectorId[MaxModuleInOctant] ={ 
    0, 1, 0, 2, 3, 1, 4, 5, 2, 0, 1, 2, 0, 1, 2 
  };  

  if(sectorModule< 0 || sectorModule>=MaxModuleInOctant) return;

  if(regionId[sectorModule]==0){ 
    setRegionType(Endcap); 
    setSector(sectorId[sectorModule] + m_octant*(NumberOfEndcapSector/NumberOfOctant));

  } else if(regionId[sectorModule]==1){ 
    setRegionType(Forward); 
    setSector(sectorId[sectorModule] + m_octant*(NumberOfForwardSector/NumberOfOctant));
  } else  {                             
    setMultipletType(Inner);
    if(regionId[sectorModule]==2){ setRegionType(Endcap); }
    if(regionId[sectorModule]==3){ setRegionType(Forward); }
    setSector(sectorId[sectorModule] + m_octant*(NumberOfInnerSector/NumberOfOctant));
  }
 
 }
   
void TGCId::setLayer(int v_layer)  { 
  m_layer=v_layer;
  if(m_layer>=0&&m_layer<=2){
    setMultipletType(Triplet);
    setStation(0);
  }
  if(m_layer>=3&&m_layer<=4){
    setMultipletType(Doublet);
    setStation(1);
  }
  if(m_layer>=5&&m_layer<=6){
    setMultipletType(Doublet);
    setStation(2);
  }
  if(m_layer>=7&&m_layer<=8){
    setMultipletType(Inner);
    setStation(3);
    if(m_sector!=-1)
      m_octant=m_sector/3;
  }
}

void TGCId::setChamber(int v_chamber)  { 
  m_chamber = v_chamber;
}
   
void TGCId::setId(int v_id)  { 
  m_id=v_id;
}
   
void TGCId::setBlock(int v_block)  { 
  m_block=v_block;
}
   
void TGCId::setChannel(int v_channel)  {
  m_channel=v_channel; 
}  
   
void TGCId::setEvent(int v_event) {
  m_event=v_event;
}
   
void TGCId::setBunch(int v_bunch) {
  m_bunch=v_bunch;
}

} // end of namespace
