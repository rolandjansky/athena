/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcablingInterface/TGCIdBase.h"


TGCIdBase::TGCIdBase (TGCIdBase::IdType vtype) : 
                                               channelType(NoChannelIdType), 
                                               side(NoSideType),
                                               module(NoModuleType),
                                               signal(NoSignalType),
                                               multiplet(NoMultipletType),
                                               region(NoRegionType),
                                               station(-1),
                                               octant(-1),
                                               sector(-1),
                                               layer(-1),
                                               chamber(-1),
                                               id(-1),
                                               block(-1),
                                               channel(-1),
                                               event(-1),
                                               bunch(-1)
{
    idType = vtype;
}

TGCIdBase::~TGCIdBase () {}
  
 
//Interface functions
   
TGCIdBase::IdType TGCIdBase::getIdType () const { return idType; }

TGCIdBase::ChannelIdType TGCIdBase::getChannelIdType () const { return channelType; }
TGCIdBase::SideType      TGCIdBase::getSideType ()      const { return side; }
TGCIdBase::ModuleType    TGCIdBase::getModuleType ()    const { return module; }
TGCIdBase::SignalType    TGCIdBase::getSignalType ()    const { return signal; }
TGCIdBase::MultipletType TGCIdBase::getMultipletType () const { return multiplet; }
TGCIdBase::RegionType    TGCIdBase::getRegionType ()    const { return region; }

int TGCIdBase::getStation () const { return station; }
int TGCIdBase::getOctant ()  const { return octant; }
int TGCIdBase::getSector ()  const { return sector; }
int TGCIdBase::getLayer ()   const { return layer; }
int TGCIdBase::getChamber () const { return chamber; }
int TGCIdBase::getId ()      const { return id; }
int TGCIdBase::getBlock ()   const { return block; }
int TGCIdBase::getChannel () const { return channel; }
int TGCIdBase::getEvent ()   const { return event; }
int TGCIdBase::getBunch ()   const { return bunch; }

bool TGCIdBase::isAside ()    const { return (side==Aside); }
bool TGCIdBase::isCside ()    const { return (side==Cside); }
bool TGCIdBase::isStrip ()    const { return (signal==Strip); }
bool TGCIdBase::isWire ()     const { return (signal==Wire); }
bool TGCIdBase::isTriplet ()  const { return (multiplet==Triplet); }
bool TGCIdBase::isDoublet ()  const { return (multiplet==Doublet); }
bool TGCIdBase::isInner ()    const { return (multiplet==Inner); }
bool TGCIdBase::isForward ()  const { return (region==Forward); }
bool TGCIdBase::isEndcap ()   const { return (region==Endcap); }
bool TGCIdBase::isBackward () const { 
  if (isEndcap()){
    if ( !isInner() ) { 
      if(isAside()) return (sector%2==1);
      else          return (sector%2==0); 
    } else {
      // EI  
      // Special case of EI11
      if (sector == 15) {
	if(isAside()) return false; 
	else          return true;
      } else if (sector == 16) {
	if(isAside()) return true; 
	else          return false;
      } else {
	//  A-side phi0 F: phi1 F: phi2 B
	//  C-side phi0 B: phi1 B: phi2 F
	if(isAside())  return (sector%3==2);
	else           return (sector%3!=2);
      }
    }
  } else {
    if(isAside()) return true;  // all Backward
    else          return false; // all Forward 
  }
}
void TGCIdBase::setIdType (IdType vtype){idType = vtype;}
void TGCIdBase::setChannelIdType (ChannelIdType vtype){channelType = vtype;}
 
// virtual methods
void TGCIdBase::setSideType (SideType vside){side = vside;}
void TGCIdBase::setModuleType (ModuleType vmodule){module = vmodule;}
void TGCIdBase::setSignalType (SignalType vsignal){signal = vsignal;}
void TGCIdBase::setMultipletType (MultipletType vmultiplet){multiplet=vmultiplet;}
void TGCIdBase::setRegionType (RegionType vregion){region=vregion;}
void TGCIdBase::setStation (int vstation){station=vstation;}

void TGCIdBase::setOctant (int voctant){octant = voctant;}
void TGCIdBase::setSector (int vsector){sector = vsector;}

void TGCIdBase::setLayer   (int vlayer){layer=vlayer;}
void TGCIdBase::setChamber (int vchamber){chamber = vchamber;}
void TGCIdBase::setId      (int vid){ id = vid;}
void TGCIdBase::setBlock   (int vblock){ block = vblock;}
void TGCIdBase::setChannel (int vchannel){channel = vchannel;}
void TGCIdBase::setEvent   (int vevent){event = vevent;}
void TGCIdBase::setBunch   (int vbunch){bunch = vbunch;}

bool TGCIdBase::isValid() const {return true;}

int TGCIdBase::getGasGap (void)         const{return -1;}
int TGCIdBase::getSectorInOctant (void) const{return -1;}
int TGCIdBase::getSectorModule (void)   const {return -1;}
