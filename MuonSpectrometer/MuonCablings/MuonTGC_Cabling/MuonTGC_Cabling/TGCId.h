/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CABLING_TGCID_HH
#define MUONTGC_CABLING_TGCID_HH

#include "TGCcablingInterface/TGCIdBase.h"

namespace MuonTGC_Cabling
{
 
class TGCId : public TGCIdBase
{
public:

  // Constructor & Destructor
  TGCId(TGCIdBase::IdType vtype=NoIdType) 
    : TGCIdBase(vtype) ,
    m_sectorRO(-1)
    {}

  virtual ~TGCId(void) {}
  
  virtual bool isValid(void) const { return true; }


public:  
  // <internal numbering scheme>
  // 1. IdType
  // 2. IdIndex
  // int station [0..3]
  // int sectorRO [0..11] 
  // int octant  [0..7]
  // int sector  [0..47],[0..23]
  // int layer   [0..8]
  // int chamber [0..n]
  // int id      [0..n]
  // int block   [0..n]
  // int channel [0..n]
  // int event   [1..n]
  // int bunch   [0..n]

  enum { 
    NumberOfStation = 4,
    NumberOfLayer   = 8,
    NumberOfOctant = 8,
    NumberOfReadoutSector = 12,
    NumberOfForwardSector = 24,
    NumberOfInnerSector   = 24,
    NumberOfEndcapSector  = 48
  };

  int getReadoutSector(void) const { return m_sectorRO; }
  int getSectorInReadout(void) const;
  
  virtual int getGasGap(void) const;
  virtual int getSectorInOctant(void) const;
  virtual int getSectorModule(void) const;

protected:
  void setSideType(SideType side);
  void setModuleType(ModuleType module);
  void setSignalType(SignalType signal);
  void setMultipletType(MultipletType multiplet);
  void setRegionType(RegionType region);
  void setStation(int station);

  void setReadoutSector(int sector);
  void setOctant(int octant);
  virtual void setSector(int sector);

  void setSectorModule(int sectorModule);

  void setLayer(int layer);
  void setChamber(int chamber);
  void setId(int id);
  void setBlock(int block);
  virtual void setChannel(int channel);
  void setEvent(int event);
  void setBunch(int bunch);

protected:
  int m_sectorRO;
};
  
} // end of namespace
 
#endif
