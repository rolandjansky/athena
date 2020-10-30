
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArMonitoring/LArStrHelper.h"




std::string LArStrHelper::febNameString(bool isBarrel,int pos_neg ,int feedthrough,int slot) const {
  std::string eb=isBarrel ? "Barrel" : "Endcap";
  std::string ac=(pos_neg==1) ? "A" : "C";
  return eb+ac+Form("ft%02d",feedthrough)+Form("slot%02d",slot);
}


/** fix custom strings to avoid errors due to uppr-lower cases */
std::string LArStrHelper::fixFEBname(TString nm) const {
  TString res=nm;
  res.ToLower();
  res.ReplaceAll("barrela","BarrelA");
  res.ReplaceAll("barrelc","BarrelC");
  res.ReplaceAll("endcapa","EndcapA");
  res.ReplaceAll("endcapc","EndcapC");
  res.ReplaceAll("endcapa","EndcapA");
  //check for single digit slots of feedthrough and try to correct them
  int pos = res.Index("ft")+2;
  TString num=res(pos,2);
  if(!num.IsDec()) {
    num=num(0,1);
    if(num.IsDec()) res.Replace(pos,1,"0"+num,2);
  }
  pos = res.Index("slot")+4;
  num = res(pos,2);
  if(num.Length()<2) {
    if(num.IsDec()) res.Replace(pos,1,"0"+num,2);
  }
  return (std::string)res;
}
