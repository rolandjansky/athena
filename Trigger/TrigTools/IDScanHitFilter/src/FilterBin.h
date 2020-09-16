/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// filename: FilterBin.h
// 
// author: Nikos Konstantinidis
//         nk@hep.ucl.ac.uk
//		 
// 
// Description: - The (eta,phi) bins for the HitFilter
// 
// date: 29/07/2003
// 
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////

#if ! defined( IDSCAN_FILTERBIN_H )
#define IDSCAN_FILTERBIN_H

#include "FixedBitSet.h"
#include "IdScanSpPoint.h"

#include "CxxUtils/checker_macros.h"

#include <iostream>
#include <list>
#include <map>

class FilterBin 
{
 public:
  
  long NumberOfHits()   const;
  long NumberOfLayers() const;

  void AddLdirect( long layer_id );
  void AddLindirect( FixedBitSet& bits );

  void AddHit( IdScanSpPoint* spptr, long maxLayers, long maxBarrelLayer);
  void AddTriplet( IdScanSpPoint* iPtr,
		   IdScanSpPoint* jPtr,
		   IdScanSpPoint* kPtr );

  void exchangeBits( FilterBin& otherBin );
  std::list<IdScanSpPoint* >&       hitList();
  const std::list<IdScanSpPoint* >& hitList() const;

private:
  void neighborKeys( long key, long* np ) const; 

private:
  std::list<IdScanSpPoint* > m_HitList;   // "list" of ptrs to SpPs falling in bin

  FixedBitSet m_Ldirect;                        // keeps direct layers
  FixedBitSet m_Ltotal;                         // keeps direct+indirect layers
};


inline std::ostream& operator<<(std::ostream& s, const FilterBin& f) { 
  s << "[ Nhits="    << f.NumberOfHits() 
    << "\t Nlayers=" << f.NumberOfLayers() << "\taddr=" << (void*)&f << "\t]";
  
  std::list<IdScanSpPoint*>::const_iterator spitr(f.hitList().begin());
  std::list<IdScanSpPoint*>::const_iterator spend(f.hitList().end());
  for ( ; spitr!=spend ; spitr++ ) s << "\n\t" << *(*spitr);
  return s;
}


typedef std::map<int, FilterBin>         FilterMap;


inline long FilterBin::NumberOfHits()   const { 
  return m_HitList.size();  
}


inline long FilterBin::NumberOfLayers() const {
  return m_Ltotal.count();  
}


inline void FilterBin::AddLdirect( long layer_id ) { 
  m_Ldirect.set( layer_id );     
  m_Ltotal.set( layer_id );     
}


inline void FilterBin::AddLindirect( FixedBitSet& bits ) { 
  m_Ltotal.Or( bits );   
}


inline void FilterBin::AddHit(IdScanSpPoint* spptr, long maxLayers, long maxBarrelLayer) 
{ 

  m_HitList.push_back( spptr ); 
  long lr = spptr->layer();
  this->AddLdirect( lr );
  if ( lr==0 || lr==maxBarrelLayer ){
    this->AddLdirect( maxLayers+lr );
  }

}


inline void FilterBin::AddTriplet( IdScanSpPoint* ip, 
				   IdScanSpPoint* jp, 
				   IdScanSpPoint* kp ) 
{ 

  m_HitList.push_back( ip );  this->AddLdirect( ip->layer() );
  m_HitList.push_back( jp );  this->AddLdirect( jp->layer() );
  m_HitList.push_back( kp );  this->AddLdirect( kp->layer() );
 
}


inline void FilterBin::exchangeBits( FilterBin& otherBin )
{
  m_Ltotal.Or( otherBin.m_Ldirect ); 
  otherBin.AddLindirect(m_Ldirect);
}


inline std::list<IdScanSpPoint* >& FilterBin::hitList()
{
  return m_HitList;
}

inline const std::list<IdScanSpPoint* >& FilterBin::hitList() const
{
  return m_HitList;
}

#endif
