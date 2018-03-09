/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 *   */

#ifndef TrigEgammaEmulationTool_TrigEgammaInfo_H
#define TrigEgammaEmulationTool_TrigEgammaInfo_H

namespace Trig{
  // Trigger Information struct
  struct Info
  {
    //TrigInfo info{trigger,type,l1item,l1type,pidname,decorator,isL1,perf,idperf,hltcalo,ringer,etcut,etthr,l1thr};
    std::string trigName; //Trigger Name
    std::string type; //Electron or Photon
    std::string strThrHLT;
    std::string L1Item; //L1 item for HLT
    std::string L1Type; //VHI
    std::string pidname; //Loose, Medium, Tight, etc...
    std::string pidDecorator; //Aux decoration
    std::string lhinfo;
    bool isolation;
    bool isL1; // Level1 Trigger
    bool perf; // Performance chain
    bool idperf; // Performance chain
    bool hltcalo;
    bool ringer; // ringer chain
    bool etcut; // Et cut only chain
    bool doL2Electron;
    bool applyEFCaloPid;
    float thrHLT; // HLT Et threshold
    float thrL1; // L1 Et threshold
  
  };
}
#endif

