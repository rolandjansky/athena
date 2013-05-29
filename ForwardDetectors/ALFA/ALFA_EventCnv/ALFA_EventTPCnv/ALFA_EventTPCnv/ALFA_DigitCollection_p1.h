/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_DigitCollection_p1_h
#define ALFA_DigitCollection_p1_h

#include "ALFA_EventTPCnv/ALFA_Digit_p1.h"

#include "AthenaPoolUtilities/TPObjRef.h"

#include <vector>
#include <string>


// class ALFA_DigitCollection_p1  
// {
// public:
// 
//      typedef std::vector<ALFA_Digit_p1> HitVector;
//      typedef HitVector::const_iterator const_iterator;
//      typedef HitVector::iterator       iterator;
// 
// // Default constructor
//      ALFA_DigitCollection_p1 ();
// 
// 
// //inline ALFA_DigitCollection_p1::ALFA_DigitCollection_p1 () {}
// 
// // Accessors
//    const std::string&  name() const; 
//    const HitVector&    getVector() const;
//    
// private:
//      std::vector<ALFA_Digit_p1>   m_cont; 
//      std::string                m_name;
// };
//    
// // inlines
//  
// inline
// ALFA_DigitCollection_p1::ALFA_DigitCollection_p1 () {}
//  
// inline
// const std::string& 
// ALFA_DigitCollection_p1::name() const
// {return m_name;}
//   
// inline
// const std::vector<ALFA_Digit_p1>& 
// ALFA_DigitCollection_p1::getVector() const 
// {return m_cont;}

class ALFA_DigitCollection_p1: public std::vector<ALFA_Digit_p1> 
{   
  public:  
  ALFA_DigitCollection_p1() {};
};

#endif // ALFA_DigitCollection_p1_h
