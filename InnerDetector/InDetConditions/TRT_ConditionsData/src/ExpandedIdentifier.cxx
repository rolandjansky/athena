/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_ConditionsData/ExpandedIdentifier.h"
#include <iomanip>

namespace TRTCond
{
 
  void ExpandedIdentifier::print(std::ostream& os) const { 
    for(int i=BARRELEC; i<=STRAW; ++i) os << name(i) << "=" << int(id(i)) << "," ;
    os << "level=" << level() << std::endl ;
  }

  void ExpandedIdentifier::write( std::ostream& os) const {
    int bec        = level() >= BARRELEC   ? id(BARRELEC) : -3 ;
    int layer      = level() >= LAYERWHEEL ? id(LAYERWHEEL) : -1 ;
    int sector     = level() >= MODULE     ? id(MODULE) : -1 ;
    int strawlayer = level() >= STRAWLAYER ? id(STRAWLAYER) : -1 ;
    int straw      = level() >= STRAW      ? id(STRAW) : -1 ;
    os << std::setiosflags(std::ios_base::right)
       << std::setw(2) << bec 
       << std::setw(3) << layer 
       << std::setw(3) << sector
       << std::setw(3) << strawlayer
       << std::setw(3) << straw ;
  }
  
  void ExpandedIdentifier::read( std::istream& is) {
    int bec,layer,sector,strawlayer,straw ;
    is >> bec >> layer  >> sector >>  strawlayer >> straw ;
    int level = STRAW ;
    if( straw<0      ) { level = STRAWLAYER ; straw = 0 ; }
    if( strawlayer<0 ) { level = MODULE ;     strawlayer = 0 ; }
    if( sector<0     ) { level = LAYERWHEEL ; sector = 0 ; }
    if( layer<0      ) { level = BARRELEC ;   layer = 0 ; }
    if( bec<-2       ) { level = DETECTOR ;   bec = -2 ; }
    *this = TRTCond::ExpandedIdentifier( bec,layer,sector,strawlayer,straw,level ) ;
  }

}
