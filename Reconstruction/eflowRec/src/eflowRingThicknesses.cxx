/*   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration */ 

#include "eflowRec/eflowRingThicknesses.h"

double eflowRingThicknesses::ringThickness(const eflowCaloENUM& layer) {
 
  double EM1 = 0.0491122;   
  double EM2 = 0.0175171;  
  double EM3 = 0.0278496;   
  double HEC = 0.0700683;   
  double Tile1Or2 = HEC;   
  double Tile3 =  0.111398;

  switch(layer){

    case eflowCalo::EMB1 : return EM1;   
    case eflowCalo::EMB2 : return EM2;   
    case eflowCalo::EMB3 : return EM3;   
    case eflowCalo::EME1 : return EM1;   
    case eflowCalo::EME2 : return EM2;   
    case eflowCalo::EME3 : return EM3;   
    case eflowCalo::HEC1 : return HEC;   
    case eflowCalo::HEC2 : return HEC;   
    case eflowCalo::HEC3 : return HEC;   
    case eflowCalo::HEC4 : return HEC;   
    case eflowCalo::Tile1 : return Tile1Or2;   
    case eflowCalo::Tile2 : return Tile1Or2;   
    case eflowCalo::Tile3 : return Tile3;   
    case eflowCalo::Unknown : return 0.0;
  }
  //if no case was found return 0.0   
  return 0.0;
}
