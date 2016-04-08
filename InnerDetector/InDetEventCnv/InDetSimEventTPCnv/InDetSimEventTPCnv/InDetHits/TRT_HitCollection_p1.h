/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_HITCOLLECTION_P1_H
#define TRT_HITCOLLECTION_P1_H

/*

Persistent represenation of a TRT_HitContainer,
Author: Davide Costanzo

*/

#include <vector>
#include <string>
#include "TRT_Hit_p1.h"


class TRT_HitCollection_p1   
{
public:
    /// typedefs
    typedef std::vector<TRT_Hit_p1> HitVector;
    typedef HitVector::const_iterator const_iterator;
    typedef HitVector::iterator       iterator;


    /// Default constructor
    TRT_HitCollection_p1 ();

    // Accessors
    const std::string&  name() const; 
    const HitVector&    getVector() const;
    //private:
    std::vector<TRT_Hit_p1>   m_cont; 
    std::string              m_name;
};


// inlines

inline
TRT_HitCollection_p1::TRT_HitCollection_p1 () {}

inline
const std::string& 
TRT_HitCollection_p1::name() const
{return m_name;}

inline
const std::vector<TRT_Hit_p1>& 
TRT_HitCollection_p1::getVector() const 
{return m_cont;}

#endif
