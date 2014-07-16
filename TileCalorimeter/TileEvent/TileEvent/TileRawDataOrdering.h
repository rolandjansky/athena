/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEEVENT_TILERAWDATAORDERING_H
#define TILEEVENT_TILERAWDATAORDERING_H

/** class for ordering TileRawData Objects
   * according to hardware identifier (channel number) 
   * To be used with sort algorithm. 
   * @author A. Solodkov
   * @version  0-0-2 , Jan 30, 2003
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

