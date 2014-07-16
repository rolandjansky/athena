/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEEVENT_TILERAWDATAORDERING_H
#define TILEEVENT_TILERAWDATAORDERING_H

/** class for ordering any TileData Objects
   * according to logical (offline) identifier 
   * To be used with sort algorithm. 
   * @author A. Solodkov
   * @version  0-0-2 , Jan 30, 2003
   */

template <class _TElement>
class TileLogicalOrdering
{

  public: 
    /** Constructor 
    */ 
    TileLogicalOrdering () {}

    /** Operator for sorting 
     */ 
    bool operator () (const _TElement* ch1, const _TElement* ch2)
       { return ( ch1->identify() < ch2->identify() ); }

};

#endif 

