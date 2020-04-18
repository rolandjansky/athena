/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           V0Candidate.cxx  -  Description
                             -------------------
    begin   : 31-07-2005
    authors : Evelina Bouhova-Thacker (Lancater University), Rob Henderson (Lancater University)
    email   : e.bouhova@cern.ch, r.henderson@lancaster.ac.uk
    changes : 20-03-2007
              Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
              Changing private datamembers to pointers, adding clone method, equality operator etc.
 ***************************************************************************/

#include "TrkV0Vertex/V0Hypothesis.h"
#include "TrkV0Vertex/V0Candidate.h"
#include <vector>

namespace Trk {
  // standard constructor for persistency
  V0Candidate::V0Candidate(): m_v0Hyp() {}

  V0Candidate::V0Candidate(const std::vector<Trk::V0Hypothesis *>& v0Hyp) : m_v0Hyp(v0Hyp) {}

  V0Candidate::V0Candidate(std::vector<Trk::V0Hypothesis *>&& v0Hyp)
    : m_v0Hyp(std::move(v0Hyp))
  {}

  V0Candidate::V0Candidate(const V0Candidate& rhs) : m_v0Hyp(std::vector<Trk::V0Hypothesis *>())
  {
   std::vector<Trk::V0Hypothesis *>::const_iterator itr = rhs.m_v0Hyp.begin(); 
   std::vector<Trk::V0Hypothesis *>::const_iterator itre = rhs.m_v0Hyp.end();
   for(;itr!=itre;++itr) m_v0Hyp.push_back((*itr)->clone());
  }

//destructor taking care of produced pointers
  V0Candidate::~V0Candidate() 
  {
   std::vector<Trk::V0Hypothesis *>::iterator i  = m_v0Hyp.begin();
   std::vector<Trk::V0Hypothesis *>::iterator ie = m_v0Hyp.end();
   for(;i!=ie;++i)
   {
    if(0!=(*i)) delete (*i);
    (*i)=0;
   }
   m_v0Hyp.clear();  
  }

  V0Candidate & V0Candidate::operator= (const V0Candidate & rhs)
  {
   if (this!=&rhs)
   {
    
//cleaning the local vector before assignement
    std::vector<Trk::V0Hypothesis *>::iterator i = m_v0Hyp.begin();
    std::vector<Trk::V0Hypothesis *>::iterator ie = m_v0Hyp.end();
    for(;i!=ie;++i)
    {
     if(0!=(*i)) delete (*i);
     (*i)=0;
    }
    m_v0Hyp.clear();   
    
//now copying the rhs content over     
    std::vector<Trk::V0Hypothesis *>::const_iterator itr = rhs.m_v0Hyp.begin(); 
    std::vector<Trk::V0Hypothesis *>::const_iterator itre = rhs.m_v0Hyp.end();
    for(;itr!=itre;++itr) m_v0Hyp.push_back((*itr)->clone()); 
   }
   return *this;    
  }

  V0Candidate & V0Candidate::operator= (V0Candidate && rhs)
  {
   if (this!=&rhs)
   {
     m_v0Hyp = std::move (rhs.m_v0Hyp);
   }
   return *this;    
  }
} // end of namespace
