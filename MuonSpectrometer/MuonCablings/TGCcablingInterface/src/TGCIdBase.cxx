/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcablingInterface/TGCIdBase.h"


TGCIdBase::TGCIdBase (TGCIdBase::IdType vtype) : 
                                               m_channelType(NoChannelIdType), 
                                               m_side(NoSideType),
                                               m_module(NoModuleType),
                                               m_signal(NoSignalType),
                                               m_multiplet(NoMultipletType),
                                               m_region(NoRegionType),
                                               m_station(-1),
                                               m_octant(-1),
                                               m_sector(-1),
                                               m_layer(-1),
                                               m_chamber(-1),
                                               m_id(-1),
                                               m_block(-1),
                                               m_channel(-1),
                                               m_event(-1),
                                               m_bunch(-1)
{
    m_idType = vtype;
}

TGCIdBase::~TGCIdBase () {}
  
 
//Interface functions
   
TGCIdBase::IdType TGCIdBase::getIdType () const { return m_idType; }

TGCIdBase::ChannelIdType TGCIdBase::getChannelIdType () const { return m_channelType; }
TGCIdBase::SideType      TGCIdBase::getSideType ()      const { return m_side; }
TGCIdBase::ModuleType    TGCIdBase::getModuleType ()    const { return m_module; }
TGCIdBase::SignalType    TGCIdBase::getSignalType ()    const { return m_signal; }
TGCIdBase::MultipletType TGCIdBase::getMultipletType () const { return m_multiplet; }
TGCIdBase::RegionType    TGCIdBase::getRegionType ()    const { return m_region; }

int TGCIdBase::getStation () const { return m_station; }
int TGCIdBase::getOctant ()  const { return m_octant; }
int TGCIdBase::getSector ()  const { return m_sector; }
int TGCIdBase::getLayer ()   const { return m_layer; }
int TGCIdBase::getChamber () const { return m_chamber; }
int TGCIdBase::getId ()      const { return m_id; }
int TGCIdBase::getBlock ()   const { return m_block; }
int TGCIdBase::getChannel () const { return m_channel; }
int TGCIdBase::getEvent ()   const { return m_event; }
int TGCIdBase::getBunch ()   const { return m_bunch; }

bool TGCIdBase::isAside ()    const { return (m_side==Aside); }
bool TGCIdBase::isCside ()    const { return (m_side==Cside); }
bool TGCIdBase::isStrip ()    const { return (m_signal==Strip); }
bool TGCIdBase::isWire ()     const { return (m_signal==Wire); }
bool TGCIdBase::isTriplet ()  const { return (m_multiplet==Triplet); }
bool TGCIdBase::isDoublet ()  const { return (m_multiplet==Doublet); }
bool TGCIdBase::isInner ()    const { return (m_multiplet==Inner); }
bool TGCIdBase::isForward ()  const { return (m_region==Forward); }
bool TGCIdBase::isEndcap ()   const { return (m_region==Endcap); }
bool TGCIdBase::isBackward () const { 
  if (isEndcap()){
    if ( !isInner() ) { 
      if(isAside()) return (m_sector%2==1);
      else          return (m_sector%2==0); 
    } else {
      // EI  
      // Special case of EI11
      if (m_sector == 15) {
	if(isAside()) return false; 
	else          return true;
      } else if (m_sector == 16) {
	if(isAside()) return true; 
	else          return false;
      } else {
	//  A-m_side phi0 F: phi1 F: phi2 B
	//  C-m_side phi0 B: phi1 B: phi2 F
	if(isAside())  return (m_sector%3==2);
	else           return (m_sector%3!=2);
      }
    }
  } else {
    if(isAside()) return true;  // all Backward
    else          return false; // all Forward 
  }
}
void TGCIdBase::setIdType (IdType vtype){m_idType = vtype;}
void TGCIdBase::setChannelIdType (ChannelIdType vtype){m_channelType = vtype;}
 
// virtual methods
void TGCIdBase::setSideType (SideType vside){m_side = vside;}
void TGCIdBase::setModuleType (ModuleType vmodule){m_module = vmodule;}
void TGCIdBase::setSignalType (SignalType vsignal){m_signal = vsignal;}
void TGCIdBase::setMultipletType (MultipletType vmultiplet){m_multiplet=vmultiplet;}
void TGCIdBase::setRegionType (RegionType vregion){m_region=vregion;}
void TGCIdBase::setStation (int vstation){m_station=vstation;}

void TGCIdBase::setOctant (int voctant){m_octant = voctant;}
void TGCIdBase::setSector (int vsector){m_sector = vsector;}

void TGCIdBase::setLayer   (int vlayer){m_layer=vlayer;}
void TGCIdBase::setChamber (int vchamber){m_chamber = vchamber;}
void TGCIdBase::setId      (int vid){ m_id = vid;}
void TGCIdBase::setBlock   (int vblock){ m_block = vblock;}
void TGCIdBase::setChannel (int vchannel){m_channel = vchannel;}
void TGCIdBase::setEvent   (int vevent){m_event = vevent;}
void TGCIdBase::setBunch   (int vbunch){m_bunch = vbunch;}

bool TGCIdBase::isValid() const {return true;}

int TGCIdBase::getGasGap (void)         const{return -1;}
int TGCIdBase::getSectorInOctant (void) const{return -1;}
int TGCIdBase::getSectorModule (void)   const {return -1;}
