/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
         TGCIdBase.h

    Author  : H.Kurashige           Sep. 2007
    Email   : Hisaya.Kurashige@cern.ch
    Description : Identifier for TGCcabling
  
***************************************************************************/

#ifndef TGCIDBASE_HH
#define TGCIDBASE_HH

class TGCIdBase
{
public:
  enum IdType {NoIdType=-1,
               Channel,Module,
	       MaxIdType};

  enum SideType      {NoSideType=-1,      Aside,Cside,                   
                      MaxSideType};
  enum ModuleType    {NoModuleType=-1,    WD,SD,WT,ST,WI,SI,              
                      MaxModuleType};
  enum SignalType    {NoSignalType=-1,    Wire,Strip,                    
                      MaxSignalType};
  enum MultipletType {NoMultipletType=-1, Doublet,Triplet,Inner,
                      MaxChamberType};
  enum RegionType    {NoRegionType=-1,    Endcap,Forward,                
                      MaxRegionType};

  enum ChannelIdType {NoChannelIdType=-1,
		      ASDIn,ASDOut,PPIn,PPOut,SLBIn,SLBOut,HPBIn,
		      MaxChannelIdType};
 
  // Constructor & Destructor
  TGCIdBase (IdType vtype=NoIdType);
  virtual ~TGCIdBase (void);
  
 
public:   
  IdType getIdType (void) const;
  ChannelIdType getChannelIdType (void) const;

  SideType    getSideType (void) const;
  ModuleType  getModuleType (void) const;
  SignalType  getSignalType (void) const;
  MultipletType  getMultipletType (void) const;
  RegionType  getRegionType (void) const;

  int getStation (void) const;
  int getOctant (void) const;
  virtual int getSector (void) const;
  int getLayer (void) const;
  int getChamber (void) const;
  int getId (void) const;
  int getBlock (void) const;
  virtual int getChannel (void) const;
  int getEvent (void) const;
  int getBunch (void) const;

  bool isAside (void) const;
  bool isCside (void) const;
  bool isStrip (void) const;
  bool isWire (void) const;
  bool isTriplet (void) const;
  bool isDoublet (void) const;
  bool isInner (void) const;
  bool isForward (void) const;
  bool isEndcap (void) const;
  bool isBackward (void) const;
  
  void setIdType (IdType vtype);
  void setChannelIdType (ChannelIdType vtype);
   
 // virtual methods
  virtual void setSideType (SideType vside);
  virtual void setModuleType (ModuleType vmodule);
  virtual void setSignalType (SignalType vsignal);
  virtual void setMultipletType (MultipletType vmultiplet);
  virtual void setRegionType (RegionType vregion);
  virtual void setStation (int vstation);

  virtual void setOctant (int voctant);
  virtual void setSector (int vsector);

  virtual void setLayer (int vlayer);
  virtual void setChamber (int vchamber);
  virtual void setId (int vid);
  virtual void setBlock (int vblock);
  virtual void setChannel (int vchannel);
  virtual void setEvent (int vevent);
  virtual void setBunch (int vbunch);

  virtual bool isValid() const;

  virtual int getGasGap (void) const;
  virtual int getSectorInOctant (void) const;
  virtual int getSectorModule (void) const;

protected:
  IdType m_idType;
  ChannelIdType m_channelType;

protected:
  SideType      m_side;
  ModuleType    m_module;
  SignalType    m_signal;
  MultipletType m_multiplet;
  RegionType    m_region;
  int m_station;
  int m_octant;
  int m_sector;
  int m_layer;
  int m_chamber;
  int m_id;
  int m_block;
  int m_channel;
  int m_event;
  int m_bunch;
};
  
 
#endif
