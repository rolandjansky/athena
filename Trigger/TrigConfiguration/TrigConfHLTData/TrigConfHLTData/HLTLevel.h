/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_HLTLevel
#define TrigConf_HLTLevel

#include <string>

namespace TrigConf {
   
   enum HLTLevel { L2=0, EF, HLT };

   inline HLTLevel str2lvl(const std::string& level) {
      return (level=="L2" || level=="l2") ? L2 :
         ((level=="EF" || level=="ef") ? EF : HLT);
   }

   inline std::string lvl2str(HLTLevel level) {
      return level==L2 ? "L2" :
         (level==EF ? "EF" : "HLT");
   }

}

#endif
