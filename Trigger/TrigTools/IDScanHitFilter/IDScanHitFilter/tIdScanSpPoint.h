/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// filename: IdScanSpPoint.h
// 
// author: Nikos Konstantinidis
//         nk@hep.ucl.ac.uk
//		 
// 
// Description: - Internal IdScan space point
// 
// date: 24/07/2003
// 
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////


#if ! defined( IDSCAN_TIDSCANSPPOINT_H )
#define IDSCAN_TIDSCANSPPOINT_H

#include <iostream>


template<class SpacePointClass> class tIdScanSpPoint
{
 public:

  tIdScanSpPoint( const SpacePointClass* spPtr, double eta ) :
    m_commonSP(spPtr),
    m_eta(eta)
    { };

  int index()      const { return m_index; } 
  int index(int i)       { return m_index=i; }

  const SpacePointClass* commonSP() const {  return m_commonSP; };

  double phi()  const {  return m_commonSP->phi(); };
  double rho()  const {  return m_commonSP->r();   };
  double z()    const {  return m_commonSP->z();   };

  double eta()        const {  return m_eta;        };
  double rotatedPhi() const {  return m_rotatedPhi; };

  long   layer()      const {  return m_commonSP->layer(); };

  double dphi() const {  return m_commonSP->dphi(); };
  double dr()   const {  return m_commonSP->dr();   };
  double dz()   const {  return m_commonSP->dz();   };

  int  used() const    { return m_used; };
  void setUsed(int u)  { m_used=u; };

  void setRotatedPhi(double phi) { m_rotatedPhi = phi; }

 private:
  
  const SpacePointClass* m_commonSP;

  double   m_eta;
  double   m_rotatedPhi;

  int m_used;
  int m_index;
};


template<class SpacePointClass>
inline std::ostream& operator<<(std::ostream& s, 
                                const tIdScanSpPoint<SpacePointClass>& sp) 
{ 
  return   s << "[ i="     << sp.index() 
	     << "\tlyr="   << sp.layer() 
	     << "\trho="   << sp.rho() 
	     << "\tphi="   << sp.phi() 
	     << "\tzed="   << sp.z() 
	     << "\t( eta=" << sp.eta() << " )\t]";
}



template<class SpacePointClass> 
struct torderInRho
{
//  this is a comparison function needed by std::sort to sort the hits of a list in increasing rho
//
  bool operator()(const SpacePointClass* sp1, const SpacePointClass* sp2) const
  {
    return ( sp1->rho() < sp2->rho() );
  }
};

template<class SpacePointClass> 
struct torderInLayer
{
  bool operator()(SpacePointClass* sp1, SpacePointClass* sp2) const
  {
    return ( sp1->layer() < sp2->layer() );
  }
};

template<class SpacePointClass> 
struct torderInIndex
{
  bool operator()(SpacePointClass* sp1, SpacePointClass* sp2) const
  {
    return ( sp1->index() < sp2->index() );
  }
};

template<class SpacePointClass> 
struct tequalIds
{
  bool operator()(SpacePointClass* sp1, SpacePointClass* sp2) const
  {
    return ( sp1->index() == sp2->index() );
  }
};


#endif

