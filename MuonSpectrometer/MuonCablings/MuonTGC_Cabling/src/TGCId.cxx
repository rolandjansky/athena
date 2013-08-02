/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  side = v_side; 
}
  
void TGCId::setModuleType(ModuleType v_module)  {
  module=v_module;
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
   
void TGCId::setSignalType(SignalType v_signal)  { 
  signal = v_signal; 
  if(multiplet==Inner&&signal==Wire)  module=WI;
  if(multiplet==Inner&&signal==Strip)  module=SI;
  if(multiplet==Doublet&&signal==Wire)  module=WD;
  if(multiplet==Doublet&&signal==Strip) module=SD;
  if(multiplet==Triplet&&signal==Wire)  module=WT;
  if(multiplet==Triplet&&signal==Strip) module=ST;
}

void TGCId::setMultipletType(MultipletType v_multiplet)  {
  this->multiplet = v_multiplet;
  if(multiplet==Inner&&signal==Wire)  module=WI;
  if(multiplet==Inner&&signal==Strip)  module=SI;
  if(multiplet==Doublet&&signal==Wire)  module=WD;
  if(multiplet==Doublet&&signal==Strip) module=SD;
  if(multiplet==Triplet&&signal==Wire)  module=WT;
  if(multiplet==Triplet&&signal==Strip) module=ST;
}
   
void TGCId::setRegionType(RegionType v_region)  { 
  region = v_region; 
}
   
void TGCId::setStation(int v_station)  { 
  station = v_station;
  if(station==0) setMultipletType(Triplet);	  
  if(station==1) setMultipletType(Doublet);
  if(station==2) setMultipletType(Doublet);
  if(station==3) setMultipletType(Inner);
}
   
void TGCId::setReadoutSector(int v_sector)  { 
  sectorRO = v_sector;
}

void TGCId::setSector(int v_sector)  { 
  sector = v_sector;
  if(region==Endcap) {
    if(multiplet==Inner) {
      octant=sector /(NumberOfInnerSector/NumberOfOctant);
      sectorRO=sector /(NumberOfInnerSector/NumberOfReadoutSector);
    } else {
      octant=sector /(NumberOfEndcapSector/NumberOfOctant);
      sectorRO=sector /(NumberOfEndcapSector/NumberOfReadoutSector);
    }
  } else if(region==Forward) {
    octant=sector /(NumberOfForwardSector/NumberOfOctant);
    sectorRO=sector/(NumberOfForwardSector/NumberOfReadoutSector);
  }
}
   
void TGCId::setOctant(int v_octant)  { 
  octant = v_octant;
}

int TGCId::getSectorModule(void) const {
  if(sector==-1) return -1;

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

// before this method, set octant.
void TGCId::setSectorModule(int sectorModule) {
  if(octant <0) return;

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
    setSector(sectorId[sectorModule] + octant*(NumberOfEndcapSector/NumberOfOctant));

  } else if(regionId[sectorModule]==1){ 
    setRegionType(Forward); 
    setSector(sectorId[sectorModule] + octant*(NumberOfForwardSector/NumberOfOctant));
  } else  {                             
    setMultipletType(Inner);
    if(regionId[sectorModule]==2){ setRegionType(Endcap); }
    if(regionId[sectorModule]==3){ setRegionType(Forward); }
    setSector(sectorId[sectorModule] + octant*(NumberOfInnerSector/NumberOfOctant));
  }
 
 }
   
void TGCId::setLayer(int v_layer)  { 
  layer=v_layer;
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

void TGCId::setChamber(int v_chamber)  { 
  chamber = v_chamber;
}
   
void TGCId::setId(int v_id)  { 
  id=v_id;
}
   
void TGCId::setBlock(int v_block)  { 
  block=v_block;
}
   
void TGCId::setChannel(int v_channel)  {
  channel=v_channel; 
}  
   
void TGCId::setEvent(int v_event) {
  event=v_event;
}
   
void TGCId::setBunch(int v_bunch) {
  bunch=v_bunch;
}

} // end of namespace
