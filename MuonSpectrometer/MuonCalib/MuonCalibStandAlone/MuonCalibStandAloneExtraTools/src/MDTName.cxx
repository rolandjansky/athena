/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Author:Marco Vanadia  vanadiam@roma1.infn.it*/

#include "MuonCalibStandAloneExtraTools/MDTName.h"

namespace MuonCalib{

  MDTName::MDTName() : eta_on(-1), eta_off(-1), sector_on(-1), sector_off(-1), side('X')
  {

  }

  MDTName::MDTName(MuonFixedId the_id)
  {
    name=TString(the_id.stationNameString());
    eta_off=the_id.eta();
    eta_on=abs(eta_off);
    if(eta_off<0) side='C';
    if(eta_off>=0) side='A';
    //Here chambers with eta=0 are included in side A;
    //if(eta_off==0) side='B'; 
    sector_off=the_id.phi();
    if((name[2]=='L')||(name[2]=='M')||(name[2]=='R')) sector_on=((the_id.phi())*2)-1;
    else sector_on=((the_id.phi())*2);
    if((name=="BML")&&(sector_on==13)&&(eta_off>3)) eta_on++;
    if((name=="BML")&&(sector_on==13)&&(eta_off<(-3))) eta_on++;
    if(((sector_on==12)||(sector_on==14))&&(name=="BOF"))
    {
      eta_on=eta_on*2-1;

    }
    if(((sector_on==12)||(sector_on==14))&&(name=="BOG"))
    {
      eta_on=eta_on*2;

    }
    if((name=="EIL")&&((sector_on==1)||(sector_on==9))&&((eta_on==4)||(eta_on==5)))
    {
      if(eta_on==4) eta_on=5;
      else eta_on=4;
    }
    if((name=="EEL")&&(sector_on==5)&&(eta_on==1))
    {
      eta_on=2;
    }
  }

  MDTName::MDTName(string the_name)
  {
    TString s(the_name);
    MDTName_init(s);
  }

  MDTName::MDTName(char* the_name)
  {
    TString s(the_name);
    MDTName_init(s);
  }

  MDTName::MDTName(TString the_name)
  {
    MDTName_init(the_name);
  }

  MDTName::MDTName(string the_name, int sector, int eta)
  {
    TString temp=the_name;
    temp+="_";
    temp+=sector;
    temp+="_";
    temp+=eta;
    MDTName_init(temp);

  }
  MDTName::MDTName(string the_name, int eta, string side, int sector)
  {
    TString temp=the_name;
    temp+=eta;
    temp+=side;
    temp+=sector;
    MDTName_init(temp);

  }
  void MDTName::MDTName_init(TString the_name)
  {
    the_name.ToUpper();
    name=the_name(0,3);
    if(the_name.Contains('_'))
      {
	the_name.Remove(0,4);
	sector_off=(TString(the_name(0,1))).Atoi();
	the_name.Remove(0,2);
	if(the_name.Contains('-'))
	  {
	    eta_off=the_name.Atoi();
	    side='C';
	    eta_on=abs(eta_off);
	  }
	else
	  {
	    eta_off=the_name.Atoi();
	    side='A';
	    //Here chambers with eta=0 are included in side A;
	    //if(eta_off==0) side='B';
	    eta_on=eta_off;
	  }

        if((name[2]=='L')||(name[2]=='M')||(name[2]=='R')) sector_on=(sector_off*2)-1;
	else sector_on=2*sector_off;
	
        if((name=="BML")&&(sector_on==13)&&(eta_off>3)) eta_on++;
        if((name=="BML")&&(sector_on==13)&&(eta_off<-3)) eta_on++;

	if(((sector_on==12)||(sector_on==14))&&(name=="BOF"))
	  {
	    eta_on=eta_on*2-1;

	  }
	if(((sector_on==12)||(sector_on==14))&&(name=="BOG"))
	  {
	    eta_on=eta_on*2;

	  }
	if((name=="EIL")&&((sector_on==1)||(sector_on==9))&&((eta_on==4)||(eta_on==5)))
	  {
	    if(eta_on==4) eta_on=5;
	    else eta_on=4;
	  }
        if((name=="EEL")&&(sector_on==5)&&(eta_on==1))
          {
             eta_on=2;
          }

      }
    else
      {
	the_name.Remove(0,3);
	eta_on=(TString(the_name(0,1))).Atoi();
	the_name.Remove(0,1);
	side=the_name[0];
	the_name.Remove(0,1);
	sector_on=the_name.Atoi();

        if((name[2]=='L')||(name[2]=='M')||(name[2]=='R')) sector_off=(sector_on+1)/2;
	else sector_off=sector_on/2;
	if(side=='C') eta_off=-eta_on;
	else eta_off=eta_on;

	if((name=="BML")&&(sector_on==13)&&(eta_on>4)&&(side=='A')) eta_off--;
	if((name=="BML")&&(sector_on==13)&&(eta_on>4)&&(side=='C')) eta_off++;

        if((name=="EEL")&&(sector_on==5)&&(eta_on==2))
          {
             if(side=='A') eta_off=1;
             if(side=='C') eta_off=-1;
          }

	if(((sector_on==12)||(sector_on==14))&&(name=="BOF"))
	  {
	    if(side=='A')
	      {
		eta_off=(eta_on+1)/2;
	      }
	    else
	      {
		eta_off=-(eta_on+1)/2;
	      }
	  }
	if(((sector_on==12)||(sector_on==14))&&(name=="BOG"))
	  {
	    if(side=='A')
	      {
		eta_off=(eta_on)/2;
	      }
	    else
	      {
		eta_off=-(eta_on)/2;
	      }
	  }
	if((name=="EIL")&&((sector_on==1)||(sector_on==9))&&((eta_on==4)||(eta_on==5)))
	  {
	    if(side=='A')
	      {
		if(eta_on==4) eta_off=5;
		else eta_off=4;
	      }
	    else
	      {
		if(eta_on==4) eta_off=-5;
		else eta_off=-4;
	      }
	  }

      }
  }



  string MDTName::getOnlineName()
  {
    TString the_name(name);
    the_name+=eta_on;
    the_name+=side;
    if(sector_on<10) the_name+="0";
    the_name+=sector_on;
    return (string)the_name;

  }
  string MDTName::getOfflineName()
  {
    TString the_name(name);
    the_name+='_';
    the_name+=sector_off;
    the_name+='_';
    the_name+=eta_off;
    return (string)the_name;
  }

  bool MDTName::isBarrel()
  {
    if((name=="BIS")&&(eta_on==7)) return false;
    if((name=="BIS")&&(eta_on==8)) return false;
    if((name=="BEE")) return false;
    if(name[0]=='B') return true;
    return false;
  }
  bool MDTName::isEndcap()
  {
    if(name[0]=='E') return true;
    if((name=="BIS")&&(eta_on==7)) return true;
    if((name=="BIS")&&(eta_on==8)) return true;
    if((name=="BEE")) return true;
    return false;
  }
  bool MDTName::isInner()
  {
    if(name[1]=='I') return true;
    return false;
  }
  bool MDTName::isMiddle()
  {
    if(name[1]=='M') return true;
    return false;
  }
  bool MDTName::isOuter()
  {
    if(name[1]=='O') return true;
    return false;
  }
  bool MDTName::isExtra()
  {
    if(name[1]=='E') return true;
    return false;
  }
  bool MDTName::isForward()
  {
    if (eta_off>=0) return true; //Here chambers with eta=0 are treated as Forward
    return false;
  }
  bool MDTName::isBackward()
  {
    if(eta_off<0) return true;
    return false;
  }
  bool MDTName::isLarge()
  {
    if(name[2]=='L') return true;
    return false;
  }
  bool MDTName::isSmall()
  {
    if(name[2]=='S') return true;
       return false;
  }
  int MDTName::getOnlineSector()
  {
     return sector_on;
  }
  int MDTName::getOfflineSector()
  {
    return sector_off;
  }
  int MDTName::getOnlineEta()
  {
    return eta_on;
  }
  int MDTName::getOfflineEta()
  {
    return eta_off;
  }
  string MDTName::getRegion()
  {
    string temp="Barrel";
    if(this->isEndcap()) temp="Endcap";
    return temp;
  }
  string MDTName::getStation()
  {
    string temp="";
    temp+=name[1];
    return temp;
  }
  string MDTName::getSize()
  {
    string temp="";
    temp+=name[2];
    return temp;
  }
  string MDTName::getSide()
  {
    string temp="";
    temp+=side;
    return temp;
  }
  string MDTName::getName()
  {
    return (string)name;
  }
  TString MDTName::OnlineToOfflineName(TString the_name)
  {
    MDTName temp(the_name);
    return temp.getOfflineName();
  }
  TString MDTName::OnlineToOfflineName(char* the_name)
  {
    MDTName temp(the_name);
    return temp.getOfflineName();
  }
  TString MDTName::OnlineToOfflineName(string the_name)
  {
    MDTName temp(the_name);
    return temp.getOfflineName();
  }
  TString MDTName::OfflineToOnlineName(TString the_name)
  {
    MDTName temp(the_name);
    return temp.getOnlineName();
  }
  TString MDTName::OfflineToOnlineName(char* the_name)
  {
    MDTName temp(the_name);
    return temp.getOnlineName();
  }
  TString MDTName::OfflineToOnlineName(string the_name)
  {
    MDTName temp(the_name);
    return temp.getOnlineName();
  }


}//end of namespace MuonCalib
