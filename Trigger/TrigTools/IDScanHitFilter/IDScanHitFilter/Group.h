/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// filename: Group.h
// 
// author: Nikos Konstantinidis
//         nk@hep.ucl.ac.uk
//		 
// 
// Description: - The Groups of IdScanSpPoints after pattern recognition
// 
// date: 29/07/2003
// 
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////

#if ! defined( IDSCAN_GROUP_H )
#define IDSCAN_GROUP_H


#include <list>
#include <iostream>
#include <iomanip>


#include "IDScanHitFilter/IdScanSpPoint.h"

typedef std::vector<IdScanSpPoint*> hitVec;

class Group 
{
 public:

 Group() : m_bin1(0), m_bin2(0), m_nEntries(0), 
    m_eta(0), m_phi0(0), m_d0(0), m_z0(0) { };

  void AddHits( std::list<IdScanSpPoint* >& binhits, long bin1, long bin2 );
  long NumberOfHits() const;
  std::list< IdScanSpPoint* >&       groupHits();
  const std::list< IdScanSpPoint* >& groupHits() const;
  double bin1() const;
  double bin2() const;

  double getEta()   const { return m_eta;   };
  double getPhi0()  const { return m_phi0;  };
  double getPtInv() const { return m_ptInv; };
  double getd0()    const { return m_d0;    };
  double getz0()    const { return m_z0;    };
  double getChi2()  const { return m_chi2;  };

  void setEta( double eta)     { m_eta   = eta;   };
  void setPhi0( double phi0)   { m_phi0  = phi0;  };
  void setPtInv( double ptinv) { m_ptInv = ptinv; };
  void setd0( double d0)       { m_d0    = d0;    };
  void setz0( double z0)       { m_z0    = z0;    };
  void setChi2( double chi2)   { m_chi2    = chi2;};

  void removeDuplicates();

  bool operator < (const Group& rhs)
    { return fabs(m_ptInv) > fabs(rhs.m_ptInv); };

 private:
  std::list< IdScanSpPoint* > m_HitList;   // "list" of ptrs to SpPs making the group
  long m_bin1;
  long m_bin2;
  long m_nEntries;

  double m_eta;
  double m_phi0;
  double m_ptInv;
  double m_d0;
  double m_z0;
  double m_chi2;

};


typedef std::list< Group >             GroupList;



inline std::ostream& operator<<(std::ostream& s, const Group& g)
{ 
  const std::list<IdScanSpPoint*>&           hits = g.groupHits(); 
  s << "[ eta="   << g.getEta() 
    << "\tphi="   << g.getPhi0() 
    << "\tzed="   << g.getz0() 
    << "\tnhits=" << hits.size() 
    << "\taddr=" << std::hex << (void*)&g << std::dec << "\t]";
  std::list<IdScanSpPoint*>::const_iterator  hitr(hits.begin());   
  std::list<IdScanSpPoint*>::const_iterator  hend(hits.end());   
  while ( hitr!=hend ) s << "\n\t" << *(*hitr++); 
  return s;
}




inline double Group::bin1() const
{
  return double(m_bin1)/double(m_nEntries);
}


inline double Group::bin2() const
{
  return double(m_bin2)/double(m_nEntries);
}

inline void Group::AddHits( std::list< IdScanSpPoint* >& binhits, long bin1, long bin2 ) 
{ 
  long entries = binhits.size();
  m_nEntries += entries;
  m_bin1 += entries*bin1;
  m_bin2 += entries*bin2;

  m_HitList.splice(m_HitList.end(), binhits); 
}

  
inline long Group::NumberOfHits() const 
{ 
  return m_HitList.size(); 
}


inline const std::list< IdScanSpPoint* >& Group::groupHits() const 
{ 
  return m_HitList;
}


inline std::list< IdScanSpPoint* >& Group::groupHits()
{
  return m_HitList;
}


inline void Group::removeDuplicates()
{
  //  m_HitList.sort( );
  // m_HitList.unique();
  m_HitList.sort(orderInIndex());
  m_HitList.unique(equalIds());
  m_HitList.sort(orderInLayer());
}

#endif

