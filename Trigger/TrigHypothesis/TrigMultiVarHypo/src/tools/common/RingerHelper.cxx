/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigMultiVarHypo/tools/common/RingerHelper.h"
using namespace Ringer;
  
void parseTrigCaloRingsLayers( unsigned layer, unsigned &minRing, unsigned &maxRing, 
                               std::string &caloLayerName)
{
  if(layer == PS   ){minRing = 0;   maxRing = 7 ; caloLayerName = "PreSampler";}
  if(layer == EM1  ){minRing = 8;   maxRing = 71; caloLayerName = "EM1";}
  if(layer == EM2  ){minRing = 72;  maxRing = 79; caloLayerName = "EM2";}
  if(layer == EM3  ){minRing = 80;  maxRing = 87; caloLayerName = "EM3";}
  if(layer == HAD1 ){minRing = 88;  maxRing = 91; caloLayerName = "HAD1";}
  if(layer == HAD2 ){minRing = 92;  maxRing = 95; caloLayerName = "HAD2";}
  if(layer == HAD3 ){minRing = 96;  maxRing = 99; caloLayerName = "HAD3";}
}

