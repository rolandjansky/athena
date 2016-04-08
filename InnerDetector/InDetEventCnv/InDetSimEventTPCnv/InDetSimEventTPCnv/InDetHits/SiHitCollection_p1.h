/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIHITCOLLECTION_P1_H
#define SIHITCOLLECTION_P1_H

/*

Persistent represenation of a SiHitContainer,
Author: Davide Costanzo

*/

#include <vector>
#include <string>
#include "SiHit_p1.h"


class SiHitCollection_p1   
{
public:
    /// typedefs
    typedef std::vector<SiHit_p1> HitVector;
    typedef HitVector::const_iterator const_iterator;
    typedef HitVector::iterator       iterator;


    /// Default constructor
    SiHitCollection_p1 ();

    // Accessors
    const std::string&  name() const; 
    const HitVector&    getVector() const;
    //private:
    std::vector<SiHit_p1>   m_cont; 
    std::string              m_name;
};


// inlines

inline
SiHitCollection_p1::SiHitCollection_p1 () {}

inline
const std::string& 
SiHitCollection_p1::name() const
{return m_name;}

inline
const std::vector<SiHit_p1>& 
SiHitCollection_p1::getVector() const 
{return m_cont;}

#endif
