/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHITCONTAINER_P1_H
#define LARHITCONTAINER_P1_H

/**

@class LArHitContainer_p1 
@brief Persistent represenation of a LArHitContainer,
@author: Davide Costanzo

*/

#include <vector>
#include <string>
#include "LArSimEventTPCnv/LArHit_p1.h"


class LArHitContainer_p1   
{
public:
    /// typedefs
    typedef std::vector<LArHit_p1> HitVector;
    typedef HitVector::const_iterator const_iterator;
    typedef HitVector::iterator       iterator;


    /// Default constructor
    LArHitContainer_p1 ();

    // Accessors
    const std::string&  name() const; 
    const HitVector&    getVector() const;
    //private:
    std::vector<LArHit_p1>   m_cont; 
    std::string              m_name;
};


// inlines

inline
LArHitContainer_p1::LArHitContainer_p1 () {}

inline
const std::string& 
LArHitContainer_p1::name() const
{return m_name;}

inline
const std::vector<LArHit_p1>& 
LArHitContainer_p1::getVector() const 
{return m_cont;}

#endif
