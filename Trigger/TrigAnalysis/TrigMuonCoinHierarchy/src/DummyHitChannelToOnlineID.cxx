/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonCoinHierarchy/DummyHitChannelToOnlineID.h"

namespace Trigger{

  DummyHitChannelToOnlineID::DummyHitChannelToOnlineID(
						       std::string pre_isAside,
						       int pre_sector,
						       int pre_phi,
						       std::string pre_isForward,
						       std::string pre_isStrip,
						       int pre_layer,
						       int pre_chamber,
						       int pre_channel           
                  
						       )
    :m_isAside(pre_isAside),
     m_sector(pre_sector),
     m_phi(pre_phi),
     m_isForward(pre_isForward),
     m_isStrip(pre_isStrip),
     m_layer(pre_layer),
     m_chamber(pre_chamber),
     m_channel(pre_channel),
     m_triggerSectorId(0)
  {
    if(m_isAside=="A" && m_isForward=="E"){
      if(m_sector==1){
        if(m_phi==0){m_triggerSectorId=47;}
        else if(m_phi==1){m_triggerSectorId=48;}
        else if(m_phi==2){m_triggerSectorId=1;}
        else if(m_phi==3){m_triggerSectorId=2;}
      }
      else{
        m_triggerSectorId = 4*(m_sector-1)+(m_phi-1);
      }
    }
    else if(m_isAside=="A" && m_isForward=="F"){
      if(m_sector==1){
        if(m_phi==0){m_triggerSectorId=72;}
        else if(m_phi==2){m_triggerSectorId=49;}
      }
      else{
        m_triggerSectorId = 2*(m_sector-1)+48+m_phi/2;
      }
    }
    else if(m_isAside=="C" && m_isForward=="E"){
      if(m_sector==1){
        if(m_phi==0){m_triggerSectorId=119;}
        else if(m_phi==1){m_triggerSectorId=120;}
        else if(m_phi==2){m_triggerSectorId=73;}
        else if(m_phi==3){m_triggerSectorId=74;}
      }
      else{
        m_triggerSectorId = 4*(m_sector-1)+(m_phi-1)+72;
      }
    }
    else if(m_isAside=="C" && m_isForward=="F"){
      if(m_sector==1){
        if(m_phi==0){m_triggerSectorId=144;}
        else if(m_phi==2){m_triggerSectorId=121;}
      }
      else{
        m_triggerSectorId = 2*(m_sector-1)+120+m_phi/2;
      }
    }
  }

  DummyHitChannelToOnlineID::~DummyHitChannelToOnlineID()
  {
  }

  int DummyHitChannelToOnlineID::GetSubsystemNumber()
  {
    int subsystemNumber=0;
    if(m_isAside=="A"){ subsystemNumber=1; }
    else if(m_isAside=="C"){ subsystemNumber=-1; }
    return subsystemNumber;
  }

  int DummyHitChannelToOnlineID::GetOctantNumber()
  {
    int octantNumber=-1;
    if(m_triggerSectorId>=1 && m_triggerSectorId<=4){ octantNumber=0; }
    else if(m_triggerSectorId>=5 && m_triggerSectorId<=10){ octantNumber=1; }
    else if(m_triggerSectorId>=11 && m_triggerSectorId<=16){ octantNumber=2; }
    else if(m_triggerSectorId>=17 && m_triggerSectorId<=22){ octantNumber=3; }
    else if(m_triggerSectorId>=23 && m_triggerSectorId<=28){ octantNumber=4; }
    else if(m_triggerSectorId>=29 && m_triggerSectorId<=34){ octantNumber=5; }
    else if(m_triggerSectorId>=35 && m_triggerSectorId<=40){ octantNumber=6; }
    else if(m_triggerSectorId>=41 && m_triggerSectorId<=46){ octantNumber=7; }
    else if(m_triggerSectorId>=47 && m_triggerSectorId<=48){ octantNumber=0; }
    else if(m_triggerSectorId>=49 && m_triggerSectorId<=50){ octantNumber=0; }
    else if(m_triggerSectorId>=51 && m_triggerSectorId<=53){ octantNumber=1; }
    else if(m_triggerSectorId>=54 && m_triggerSectorId<=56){ octantNumber=2; }
    else if(m_triggerSectorId>=57 && m_triggerSectorId<=59){ octantNumber=3; }
    else if(m_triggerSectorId>=60 && m_triggerSectorId<=62){ octantNumber=4; }
    else if(m_triggerSectorId>=63 && m_triggerSectorId<=65){ octantNumber=5; }
    else if(m_triggerSectorId>=66 && m_triggerSectorId<=68){ octantNumber=6; }
    else if(m_triggerSectorId>=69 && m_triggerSectorId<=71){ octantNumber=7; }
    else if(m_triggerSectorId==72) { octantNumber=0; }
    else if(m_triggerSectorId>=73 && m_triggerSectorId<=76){ octantNumber=0; }
    else if(m_triggerSectorId>=77 && m_triggerSectorId<=82){ octantNumber=1; }
    else if(m_triggerSectorId>=83 && m_triggerSectorId<=88){ octantNumber=2; }
    else if(m_triggerSectorId>=89 && m_triggerSectorId<=94){ octantNumber=3; }
    else if(m_triggerSectorId>=95 && m_triggerSectorId<=100){ octantNumber=4; }
    else if(m_triggerSectorId>=101 && m_triggerSectorId<=106){ octantNumber=5; }
    else if(m_triggerSectorId>=107 && m_triggerSectorId<=112){ octantNumber=6; }
    else if(m_triggerSectorId>=113 && m_triggerSectorId<=118){ octantNumber=7; }
    else if(m_triggerSectorId>=119 && m_triggerSectorId<=120){ octantNumber=0; }
    else if(m_triggerSectorId>=121 && m_triggerSectorId<=122){ octantNumber=0; }
    else if(m_triggerSectorId>=123 && m_triggerSectorId<=125){ octantNumber=1; }
    else if(m_triggerSectorId>=126 && m_triggerSectorId<=128){ octantNumber=2; }
    else if(m_triggerSectorId>=129 && m_triggerSectorId<=131){ octantNumber=3; }
    else if(m_triggerSectorId>=132 && m_triggerSectorId<=134){ octantNumber=4; }
    else if(m_triggerSectorId>=135 && m_triggerSectorId<=137){ octantNumber=5; }
    else if(m_triggerSectorId>=138 && m_triggerSectorId<=140){ octantNumber=6; }
    else if(m_triggerSectorId>=141 && m_triggerSectorId<=143){ octantNumber=7; }
    else if(m_triggerSectorId==144) { octantNumber=0; }

    return octantNumber;
  }

  int DummyHitChannelToOnlineID::GetModuleNumber()
  {
    int moduleNumber=-1;
    if(m_triggerSectorId>=1 && m_triggerSectorId<=48){
      if(m_triggerSectorId%6==0){ moduleNumber=1; }
      else if(m_triggerSectorId%6==1){ moduleNumber=3; }
      else if(m_triggerSectorId%6==2){ moduleNumber=4; }
      else if(m_triggerSectorId%6==3){ moduleNumber=6; }
      else if(m_triggerSectorId%6==4){ moduleNumber=7; }
      else if(m_triggerSectorId%6==5){ moduleNumber=0; }  
    }
    else if(m_triggerSectorId>=73 && m_triggerSectorId<=120){
      if(m_triggerSectorId%6==0){ moduleNumber=1; }
      else if(m_triggerSectorId%6==1){ moduleNumber=3; }
      else if(m_triggerSectorId%6==2){ moduleNumber=4; }
      else if(m_triggerSectorId%6==3){ moduleNumber=6; }
      else if(m_triggerSectorId%6==4){ moduleNumber=7; }
      else if(m_triggerSectorId%6==5){ moduleNumber=0; }
    }
    else if(m_triggerSectorId>=49 && m_triggerSectorId<=72){
      if(m_triggerSectorId%3==0){ moduleNumber=2; }
      else if(m_triggerSectorId%3==1){ moduleNumber=5; }
      else if(m_triggerSectorId%3==2){ moduleNumber=8; }
    }
    
    else if(m_triggerSectorId>=121 && m_triggerSectorId<=144){
      if(m_triggerSectorId%3==0){ moduleNumber=2; }
      else if(m_triggerSectorId%3==1){ moduleNumber=5; }
      else if(m_triggerSectorId%3==2){ moduleNumber=8; }
    }
    return moduleNumber;
  }

  int DummyHitChannelToOnlineID::GetLayerNumber()
  {
    int layerNumber=-1;
    layerNumber = m_layer-1;
    return layerNumber;
  }

  int DummyHitChannelToOnlineID::GetRNumber()
  {
    int rNumber=-1;
    if(m_isForward=="F"){ rNumber = 0; }
    else{	rNumber = m_chamber; }
    return rNumber;
  }

  int DummyHitChannelToOnlineID::GetWireOrStrip()
  {
    int wireOrStrip=-1;
    if(m_isStrip=="S"){ wireOrStrip = 1; }
    else if(m_isStrip=="W"){ wireOrStrip = 0; }
    return wireOrStrip;
  }

  int DummyHitChannelToOnlineID::GetChannelNumber()
  {
    int channelNumber=-1;
    channelNumber=m_channel;
    return channelNumber;
  }

} // end of namespace Trigger
