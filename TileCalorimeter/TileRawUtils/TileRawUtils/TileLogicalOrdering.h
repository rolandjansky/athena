/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERAWUTILS_TILERAWDATAORDERING_H
#define TILERAWUTILS_TILERAWDATAORDERING_H

/** 
* @brief Order TileData Objects according to logical (offline) identifier
* @author Alexander Solodkov
*
* class for ordering any TileData Objects
* according to logical (offline) identifier 
* To be used with sort algorithm. 
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

