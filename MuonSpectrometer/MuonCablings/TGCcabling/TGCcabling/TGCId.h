/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCID_HH
#define TGCID_HH

#include "TGCcablingInterface/TGCIdBase.h"

namespace LVL1TGCCabling8
{
 
class TGCId : public TGCIdBase
{
public:
 
  // Constructor & Destructor
  TGCId (TGCIdBase::IdType vtype=NoIdType) 
       : TGCIdBase(vtype) 
        {}

   virtual ~TGCId (void) {}

   virtual bool isValid (void) const { return true; }
  

public:  
  // <internal numbering scheme>
  // 1. IdType
   // 2. IdIndex
  // int station [0..3]
  // int octant  [0..7]
  // int sector  [0..47],[0..23]
  // int layer   [0..8]
  // int chamber [0..n]
  // int id      [0..n]
  // int block   [0..n]
  // int channel [0..n]
  // int event   [1..n]
  // int bunch   [0..n]

  
  virtual int getGasGap (void) const;
  virtual int getSectorInOctant (void) const;
  virtual int getSectorModule (void) const;

protected:
  virtual void setSideType (SideType side);
  virtual void setModuleType (ModuleType module);
  virtual void setSignalType (SignalType signal);
  virtual void setMultipletType (MultipletType multiplet);
  virtual void setRegionType (RegionType region);
  virtual void setStation (int station);
  virtual void setSector (int sector);
  virtual void setOctant (int octant);
  virtual void setSectorModule (int sectorModule);
  virtual void setLayer (int layer);
  virtual void setChamber (int chamber);
  virtual void setId (int id);
  virtual void setBlock (int block);
  virtual void setChannel (int channel);
  virtual void setEvent (int event);
  virtual void setBunch (int bunch);

};
  
} // end of namespace
 
#endif
