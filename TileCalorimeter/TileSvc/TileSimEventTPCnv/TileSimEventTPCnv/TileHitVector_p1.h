/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEHITVECTOR_P1_H
#define TILEHITVECTOR_P1_H

/*

Persistent represenation of a TileHitVector
Author: Davide Costanzo

*/

#include "TileSimEventTPCnv/TileHit_p1.h"
#include <vector>
#include <string>


class TileHitVector_p1 {

  public:
    /// typedefs
    typedef std::vector<TileHit_p1> HitVector;
    typedef HitVector::const_iterator const_iterator;
    typedef HitVector::iterator       iterator;
  
    /// Default constructor
    TileHitVector_p1 () : m_cont(), m_name() {}
    
    // Accessors
    const std::string&  name() const {return m_name;}
    const HitVector&    getVector() const {return m_cont;}

  private:
    std::vector<TileHit_p1> m_cont; 
    std::string             m_name;
};

#endif
