/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// original filename: FilterBin.h
// 
// author: Nikos Konstantinidis
//         nk@hep.ucl.ac.uk
//		 
// 
// Description: - The 2D bins for the Hough Transform accumulator
// 
// date: 29/07/2003
// 
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////

#ifndef __HOUGH_BIN_2D_H__
#define __HOUGH_BIN_2D_H__

#include "TrigInDetPattRecoTools/Fixed32BitSet.h"
#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "TrigInDetPattRecoEvent/TrigSiSpacePointUtils.h"
#include <iostream>
#include <list>
#include <map>

class HoughBin2D {

 public:  

  long NumberOfHits()   const;
  long NumberOfLayers() const;

  void AddLdirect( long layer_id );
  void AddLindirect( Fixed32BitSet& bits );

  void AddSpacePoint(const TrigSiSpacePointBase*);
  void AddDoublet(const TrigSiSpacePointBase*, const TrigSiSpacePointBase*);
  void AddTriplet(const TrigSiSpacePointBase*, const TrigSiSpacePointBase*, const TrigSiSpacePointBase*);

  void exchangeBits( HoughBin2D& otherBin );
  std::list<const TrigSiSpacePointBase* >&       spacePoints() {
    return m_spList;
  }
  const std::list<const TrigSiSpacePointBase* >& spacePoints() const {
    return m_spList;
  }

private:
  void neighborKeys( long key, long* np ) const; 

private:

  std::list<const TrigSiSpacePointBase*> m_spList;   // "list" of ptrs to SpPs falling in bin

  Fixed32BitSet m_Ldirect;                        // keeps direct layers
  Fixed32BitSet m_Ltotal;                         // keeps direct+indirect layers
};


inline std::ostream& operator<<(std::ostream& s, const HoughBin2D& f) { 
  s << "[ Nhits="    << f.NumberOfHits() 
    << "\t Nlayers=" << f.NumberOfLayers() << "\taddr=" << (void*)&f << "\t]";
  
  std::list<const TrigSiSpacePointBase* >::const_iterator spitr(f.spacePoints().begin());
  std::list<const TrigSiSpacePointBase* >::const_iterator spend(f.spacePoints().end());
  for ( ; spitr!=spend ; spitr++ ) s << "\n\t" << *(*spitr);
  return s;
}


typedef std::map<int, HoughBin2D>   HoughAccumulatorType;

inline long HoughBin2D::NumberOfHits()   const { 
  return m_spList.size();  
}

inline long HoughBin2D::NumberOfLayers() const {
  return m_Ltotal.count();  
}

inline void HoughBin2D::AddLdirect( long layer_id ) { 
  m_Ldirect.set( layer_id );     
  m_Ltotal.set( layer_id );     
}

inline void HoughBin2D::AddLindirect( Fixed32BitSet& bits ) { 
  m_Ltotal.Or( bits );   
}

inline void HoughBin2D::AddSpacePoint(const TrigSiSpacePointBase* spptr) { 

  m_spList.push_back( spptr ); 
  long lr = spptr->layer();
  AddLdirect( lr );
  if ( lr==0 || lr==6 ){ // NB: check with Nikos if this is still needed
    AddLdirect( MAX_SILICON_LAYER_NUM+lr );
  }

}

inline void HoughBin2D::AddDoublet( const TrigSiSpacePointBase* ip, 
				   const TrigSiSpacePointBase* jp) { 

  m_spList.push_back( ip );  this->AddLdirect( ip->layer() );
  m_spList.push_back( jp );  this->AddLdirect( jp->layer() );
 
}

inline void HoughBin2D::AddTriplet( const TrigSiSpacePointBase* ip, 
				   const TrigSiSpacePointBase* jp, 
				   const TrigSiSpacePointBase* kp ) { 

  m_spList.push_back( ip );  this->AddLdirect( ip->layer() );
  m_spList.push_back( jp );  this->AddLdirect( jp->layer() );
  m_spList.push_back( kp );  this->AddLdirect( kp->layer() );
 
}


inline void HoughBin2D::exchangeBits( HoughBin2D& otherBin ) {
  m_Ltotal.Or( otherBin.m_Ldirect ); 
  otherBin.AddLindirect(m_Ldirect);
}


#endif
