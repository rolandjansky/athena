/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWUTILS_LARTT_SAMPLING_H 
#define LARRAWUTILS_LARTT_SAMPLING_H 

#include "Identifier/IdentifierHash.h" 

// 
// This class encapsulates a combined identifier (TrigTower + Sampling)
//   To be used in  raw data access.
//    TrigTower should be a valid TT IdentifierHash,  
//    sampling is an unsigned int from 0-3. 
// 

class LArTT_Sampling
{
 public: 
   // Constructors
   LArTT_Sampling(const IdentifierHash& hash, unsigned int sam) ; 
   LArTT_Sampling(unsigned int ) ; 

   // Access
   unsigned int combinedId(); 
   unsigned int sampling  (); 
   IdentifierHash TT_id    (); 
   
 private:

  unsigned int m_id; 

}; 

inline 
LArTT_Sampling:: LArTT_Sampling(const IdentifierHash& hash, 
				unsigned int sam)
{
  unsigned int i=(unsigned int)hash; 
  m_id = (i<<2)+ sam; 
  return ; 
}


inline 
LArTT_Sampling:: LArTT_Sampling(unsigned int id)
{
  m_id = id; 
  return ; 
}

inline 
unsigned int LArTT_Sampling::combinedId()
{
  return m_id; 
}

inline 
unsigned int LArTT_Sampling::sampling()
{
  unsigned int id  = m_id & 3; 
  return id;  
}

inline 
IdentifierHash LArTT_Sampling::TT_id()
{
  unsigned int id  = m_id >>2; 
  return IdentifierHash(id) ;  
}

#endif 
