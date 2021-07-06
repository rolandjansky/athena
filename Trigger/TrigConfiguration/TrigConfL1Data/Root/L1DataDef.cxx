/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/L1DataDef.h"

#include <iostream>
#include <stdexcept>
#include <iomanip>

using namespace std;

TrigConf::L1DataDef::TypeConfigMap_t TrigConf::L1DataDef::g_typeConfigs;
TrigConf::L1DataDef::STypeTypeMap_t TrigConf::L1DataDef::g_sTypeType;
std::vector<TrigConf::L1DataDef::TriggerType> TrigConf::L1DataDef::g_Types;
unsigned int TrigConf::L1DataDef::g_l1Version = 0;

registerTriggerType(EM, 16)
registerTriggerType(TAU, 8)
registerTriggerType(MUON,6)
registerTriggerType(TH,  8)
registerTriggerType(JET, 8)
registerTriggerType(FJET,0)
registerTriggerType(FJ,  8)
registerTriggerType(JF,  4)
registerTriggerType(JB,  4)
registerTriggerType(JE,  4)
registerTriggerType(M,   6)
registerTriggerType(TE,  8)
registerTriggerType(XE,  8)
registerTriggerType(XS,  8)

// internal
registerInternalTriggerType(RNDM, 2)
registerInternalTriggerType(PCLK, 2)
registerInternalTriggerType(BGRP, 8)


registerTriggerType(MBTS,    2)
registerTriggerType(MBTSSI, 32)
registerTriggerType(NIM,    37)
registerTriggerType(BCMCMB,  1)
registerTriggerType(CALREQ,  3)
registerTriggerType(BPTX,    2)
registerTriggerType(LUCID,   2)
registerTriggerType(TRT,     1)
registerTriggerType(BCM,     3)
registerTriggerType(ZDC,     3)
// zero bias
registerTriggerType(ZB,   10)
// NEW: Topo
registerTriggerType(TOPO, 128)
registerTriggerType(ALFA, 64)


void
TrigConf::L1DataDef::printMaxThresholds() {
   cout << "L1 Version: " << g_l1Version << endl;
   for(auto & x: g_typeConfigs) {
      cout << "Maximum number of thresholds for type " << setw(6) << x.second.name << " : " << x.second.max << endl;
   }
}

void
TrigConf::L1DataDef::setMaxThresholdsFromL1Version(unsigned int l1version) {
   // this has to come from L1Common/L1CommonVersion.h
   if(l1version==0) {
      typeConfig(EM).max     =  16;
      typeConfig(TAU).max    =   8;
      typeConfig(MUON).max   =   6;
      typeConfig(TH).max     =   8;
      typeConfig(JET).max    =   8;
      typeConfig(FJET).max   =   0;
      typeConfig(FJ).max     =   8;
      typeConfig(JF).max     =   4;
      typeConfig(JB).max     =   4;
      typeConfig(JE).max     =   4;
      typeConfig(M).max      =   6;
      typeConfig(TE).max     =   8;
      typeConfig(XE).max     =   8;
      typeConfig(XS).max     =   8;
      typeConfig(RNDM).max   =   2;
      typeConfig(PCLK).max   =   2;
      typeConfig(BGRP).max   =   8;
      typeConfig(MBTS).max   =   2;
      typeConfig(MBTSSI).max =  32;
      typeConfig(NIM).max    =  37;
      typeConfig(BCMCMB).max =   1;
      typeConfig(CALREQ).max =   3;
      typeConfig(BPTX).max   =   2;
      typeConfig(LUCID).max  =   2;
      typeConfig(TRT).max    =   1;
      typeConfig(BCM).max    =   3;
      typeConfig(ZDC).max    =   3;
      typeConfig(ZB).max     =  10;
      typeConfig(TOPO).max   =   0;
      typeConfig(ALFA).max   =   0;
   } else if (l1version==1) {
      typeConfig(EM).max     =  16;
      typeConfig(TAU).max    =  16;
      typeConfig(MUON).max   =   6;
      typeConfig(TH).max     =   8;
      typeConfig(JET).max    =  25;
      typeConfig(FJET).max   =   0;
      typeConfig(FJ).max     =   0;
      typeConfig(JF).max     =   0;
      typeConfig(JB).max     =   0;
      typeConfig(JE).max     =   0;
      typeConfig(M).max      =   6;
      typeConfig(TE).max     =  16;
      typeConfig(XE).max     =  16;
      typeConfig(XS).max     =   8;
      typeConfig(RNDM).max   =   4;
      typeConfig(PCLK).max   =   0;
      typeConfig(BGRP).max   =  16;
      typeConfig(MBTS).max   =   3;
      typeConfig(MBTSSI).max =  32;
      typeConfig(NIM).max    =  37;
      typeConfig(BCMCMB).max =   1;
      typeConfig(CALREQ).max =   3;
      typeConfig(BPTX).max   =   2;
      typeConfig(LUCID).max  =   6;
      typeConfig(TRT).max    =   1;
      typeConfig(BCM).max    =   6;
      typeConfig(ZDC).max    =   3;
      typeConfig(ZB).max     =  10;
      typeConfig(TOPO).max   = 128;
      typeConfig(ALFA).max   =  64;
   } else {
      cerr << "ERROR TrigConf::L1DataDef::setMaxThresholdsFromL1Version() L1Version not known:" << l1version << endl;
      throw(std::runtime_error("Unknown L1 Version"));
   }

   g_l1Version=l1version;

}







TrigConf::L1DataDef::TriggerTypeConfig&
TrigConf::L1DataDef::typeConfig(TriggerType tt) { 
   TypeConfigMap_t::iterator c = g_typeConfigs.find(tt);
   //   cout << "tt " << tt << endl;
   if( c==g_typeConfigs.end() ) {
      cerr << "Trigger type " << tt << " is defined in L1DataDef.h but not registered in L1DataDef.cxx" << endl;
      throw runtime_error("TriggerType not registered");
   }
   return c->second;
}


// can enable old JEP3 cabling (SLOT8, CON0): 4TE, 8XE
void
TrigConf::L1DataDef::setOldJEP3Cabling() {
   typeConfig(TE).max = 4;
   typeConfig(XE).max = 8;
   typeConfig(XS).max = 0;
}

// new JEP3 cabling (SLOT8, CON0): 8TE, 8XE, 8XS
void
TrigConf::L1DataDef::setNewJEP3Cabling() {
   typeConfig(TE).max = 8;
   typeConfig(XE).max = 8;
   typeConfig(XS).max = 8;
}

