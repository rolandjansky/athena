///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileDigitsContainer_p1.h 
// Persistent represenation of a TileDigitsContainer
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef TILEDIGITSCONTAINER_P1_H
#define TILEDIGITSCONTAINER_P1_H

#include "TileTPCnv/TileDigits_p1.h"
#include <vector>

class TileDigitsContainer_p1   
{
public:
    /// typedefs
    typedef std::vector<TileDigits_p1> ElemVector;
    typedef ElemVector::const_iterator const_iterator;
    typedef ElemVector::iterator       iterator;

    /// Default constructor
    TileDigitsContainer_p1 () : m_param(), m_cont() {}

    // Accessors
    const std::vector<unsigned int>& getParam() const {return m_param;}
    const ElemVector&               getVector() const {return m_cont;}

    // Clear methods
    void clear() {m_param.clear(); m_cont.clear(); }
    void reserve(unsigned int size1, unsigned int size2) { m_param.reserve(size1);  m_cont.reserve(size2); }
  
    // Fill methods
    void push_back_param (unsigned int param) {m_param.push_back(param); }
    TileDigits_p1 * newElem () { m_cont.push_back(TileDigits_p1()); return &m_cont.back(); }
      
private:
    std::vector<unsigned int>    m_param;
    std::vector<TileDigits_p1>   m_cont; 
};

#endif
