/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERAWUTILS_TILERAWDATAORDERING_H
#define TILERAWUTILS_TILERAWDATAORDERING_H

/** 
* @brief Order TileData Objects according to hardware (online) identifier
* @author Alexander Solodkov
*
* class for ordering TileRawData Objects
* according to hardware identifier (channel number) 
* To be used with sort algorithm. 
*/

#include "TileEvent/TileRawData.h"

class TileRawDataOrdering
{

  public: 
    /** Constructor 
    */ 
    TileRawDataOrdering () {}

    /** Operator for sorting 
     */ 
    bool operator () (const TileRawData* ch1, const TileRawData* ch2)
       { return ( ch1->adc_HWID() < ch2->adc_HWID() ); }

};

#endif 

