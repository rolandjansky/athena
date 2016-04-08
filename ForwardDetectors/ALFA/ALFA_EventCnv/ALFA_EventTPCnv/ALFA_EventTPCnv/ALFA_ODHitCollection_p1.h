/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_ODHitCollection_p1_h
#define ALFA_ODHitCollection_p1_h

//#include "AthenaPoolUtilities/TPObjRef.h"

#include "ALFA_EventTPCnv/ALFA_ODHit_p1.h"

#include <vector>
#include <string>


class ALFA_ODHitCollection_p1  
{
public:

     typedef std::vector<ALFA_ODHit_p1> HitVector;
     typedef HitVector::const_iterator const_iterator;
     typedef HitVector::iterator       iterator;

// Default constructor
     ALFA_ODHitCollection_p1 ();
   

// Accessors
   const std::string&  name() const; 
   const HitVector&    getVector() const;
   
   //private:
     std::vector<ALFA_ODHit_p1>   m_cont; 
     std::string                  m_name;
};
   
// inlines
 
inline
ALFA_ODHitCollection_p1::ALFA_ODHitCollection_p1 () {}
 
inline
const std::string& 
ALFA_ODHitCollection_p1::name() const
{return m_name;}
  
inline
const std::vector<ALFA_ODHit_p1>& 
ALFA_ODHitCollection_p1::getVector() const 
{return m_cont;}


#endif // ALFA_HitCollection_p1_h
