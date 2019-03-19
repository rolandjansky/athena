/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMULTIVARHYPO_RINGERHELPER_H
#define TRIGMULTIVARHYPO_RINGERHELPER_H

#include <vector>
#include <string>


namespace Ringer{
  
  enum CaloRingsLayer{
    PS   = 0,
    EM1  = 1,
    EM2  = 2,
    EM3  = 3,
    HAD1 = 4,
    HAD2 = 5,
    HAD3 = 6
  };
  /*
   * rings map into std::vector
   * 0:7   PS; 8:71  EM1;  72:79 EM2;  80:87 EM3
   * 88:91 HAD1; 92:95 HAD2; 96:99 HAD3
   */
  void parseTrigCaloRingsLayers( unsigned layer, unsigned &minRing, unsigned &maxRing, 
                                 std::string &caloLayerName);
  

 
  

} // namespace TrigCaloRingsHelper
#endif /* TrigCaloRingerHelper */
