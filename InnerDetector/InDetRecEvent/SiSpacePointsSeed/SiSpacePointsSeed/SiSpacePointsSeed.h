/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiSpacePointsSeed
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for space points seeds
// Comments:
// Space points in  m_spacepoints collection should be sorted in radius oder
//              first space point has smallest radius
//              last  space point has bigest   radius
//              m_zvertex is z-coordinate in beam region for x=y=0.
//
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 18/11/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiSpacePointsSeed_h
#define SiSpacePointsSeed_h
#include <list>
#include "TrkSpacePoint/SpacePoint.h"

class MsgStream;

namespace InDet {

  class SiSpacePointsSeed {
    
    /////////////////////////////////////////////////////////////////////////////////
    // Public methods:
    /////////////////////////////////////////////////////////////////////////////////
    
  public:
    
    SiSpacePointsSeed();
    SiSpacePointsSeed
      (const Trk::SpacePoint*&,const Trk::SpacePoint*&,
       const double&);
    SiSpacePointsSeed
      (const Trk::SpacePoint*&,const Trk::SpacePoint*&,const Trk::SpacePoint*&,
       const double&);
    SiSpacePointsSeed(const SiSpacePointsSeed&);
    SiSpacePointsSeed& operator = (const SiSpacePointsSeed&);
    virtual ~SiSpacePointsSeed();
    void                                     erase();
    void                                     add(const Trk::SpacePoint*&);
    void                                     setZVertex(const double&);
    void                                     setD0     (const double&);
    void                                     setEta    (const double&);
    void                                     setX1     (const double&);
    void                                     setX2     (const double&);
    void                                     setX3     (const double&);
    void                                     setY1     (const double&);
    void                                     setY2     (const double&);
    void                                     setY3     (const double&);
    void                                     setZ1     (const double&);
    void                                     setZ2     (const double&);
    void                                     setZ3     (const double&);
    void                                     setR1     (const double&);
    void                                     setR2     (const double&);
    void                                     setR3     (const double&);
    void                                     setDZDR_B (const double&);
    void                                     setDZDR_T (const double&);
    void                                     setPt     (const double&);
    
    const std::list<const Trk::SpacePoint*>& spacePoints() const;
    const double&                            zVertex    () const;
    const double&                            d0         () const;
    const double&                            eta        () const;
    const double&                            x1         () const;
    const double&                            x2         () const;
    const double&                            x3         () const;
    const double&                            y1         () const;
    const double&                            y2         () const;
    const double&                            y3         () const;
    const double&                            z1         () const;
    const double&                            z2         () const;
    const double&                            z3         () const;
    const double&                            r1         () const;
    const double&                            r2         () const;
    const double&                            r3         () const;
    const double&                            dzdr_b     () const;
    const double&                            dzdr_t     () const;
    const double&                            pt         () const;
    
    
    virtual MsgStream&    dump(MsgStream&    out) const ;
    virtual std::ostream& dump(std::ostream& out) const ;
    
    /////////////////////////////////////////////////////////////////////////////////
    // Protected data members
    /////////////////////////////////////////////////////////////////////////////////
    
  protected:
    
    std::list<const Trk::SpacePoint*> m_spacepoints;
    double                            m_zvertex    ;  
    double                            m_d0         ;
    double                            m_eta        ;
    double                            m_x1         ;
    double                            m_x2         ;
    double                            m_x3         ;
    double                            m_y1         ;
    double                            m_y2         ;
    double                            m_y3         ;
    double                            m_z1         ;
    double                            m_z2         ;
    double                            m_z3         ;
    double                            m_r1         ;
    double                            m_r2         ;
    double                            m_r3         ;
    double                            m_dzdr_b     ;
    double                            m_dzdr_t     ;
    double                            m_pt         ;
    
    
  };

  MsgStream&    operator << (MsgStream&   ,const SiSpacePointsSeed&);
  std::ostream& operator << (std::ostream&,const SiSpacePointsSeed&); 

  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////
 
  inline const std::list<const Trk::SpacePoint*>& SiSpacePointsSeed::spacePoints() const 
    {
      return this->m_spacepoints;
    }

  inline void SiSpacePointsSeed::erase() 
    {
      m_spacepoints.erase(m_spacepoints.begin(),m_spacepoints.end());
    }

  inline void SiSpacePointsSeed::add(const Trk::SpacePoint*& p) 
    {
      m_spacepoints.push_back(p);
    }

  inline void SiSpacePointsSeed::setD0(const double& d0)
    {
      m_d0 = d0;
    }
 
  inline void SiSpacePointsSeed::setZVertex(const double& z) 
    {
      m_zvertex = z;
    }
 
  inline void SiSpacePointsSeed::setEta(const double& eta)
    {
      m_eta = eta;
    }
 
  inline void SiSpacePointsSeed::setX1(const double& x1)
    {
      m_x1 = x1;
    }
 
  inline void SiSpacePointsSeed::setX2(const double& x2)
    {
      m_x2 = x2;
    }
 
  inline void SiSpacePointsSeed::setX3(const double& x3)
    {
      m_x3 = x3;
    }
 
  inline void SiSpacePointsSeed::setY1(const double& y1)
    {
      m_y1 = y1;
    }
 
  inline void SiSpacePointsSeed::setY2(const double& y2)
    {
      m_y2 = y2;
    }
 
  inline void SiSpacePointsSeed::setY3(const double& y3)
    {
      m_y3 = y3;
    }
 
  inline void SiSpacePointsSeed::setZ1(const double& z1)
    {
      m_z1 = z1;
    }
 
  inline void SiSpacePointsSeed::setZ2(const double& z2)
    {
      m_z2 = z2;
    }
 
  inline void SiSpacePointsSeed::setZ3(const double& z3)
    {
      m_z3 = z3;
    }
 
  inline void SiSpacePointsSeed::setR1(const double& r1)
    {
      m_r1 = r1;
    }
 
  inline void SiSpacePointsSeed::setR2(const double& r2)
    {
      m_r2 = r2;
    }
 
  inline void SiSpacePointsSeed::setR3(const double& r3)
    {
      m_r3 = r3;
    }
 
  inline void SiSpacePointsSeed::setDZDR_B(const double& dzdr)
    {
      m_dzdr_b = dzdr;
    }
 
  inline void SiSpacePointsSeed::setDZDR_T(const double& dzdr)
    {
      m_dzdr_t = dzdr;
    }
 
  inline void SiSpacePointsSeed::setPt(const double& pt)
    {
      m_pt = pt;
    }
 
  inline const double& SiSpacePointsSeed::d0() const
    {
      return m_d0;
    }
 
  inline const double& SiSpacePointsSeed::zVertex() const 
    {
      return m_zvertex;
    }
   
  inline const double& SiSpacePointsSeed::eta() const
    {
      return m_eta;
    }
 
  inline const double& SiSpacePointsSeed::x1() const
    {
      return m_x1;
    }
 
  inline const double& SiSpacePointsSeed::x2() const
    {
      return m_x2;
    }
 
  inline const double& SiSpacePointsSeed::x3() const
    {
      return m_x3;
    }
 
  inline const double& SiSpacePointsSeed::y1() const
    {
      return m_y1;
    }
 
  inline const double& SiSpacePointsSeed::y2() const
    {
      return m_y2;
    }
 
  inline const double& SiSpacePointsSeed::y3() const
    {
      return m_y3;
    }
 
  inline const double& SiSpacePointsSeed::z1() const
    {
      return m_z1;
    }
 
  inline const double& SiSpacePointsSeed::z2() const
    {
      return m_z2;
    }
 
  inline const double& SiSpacePointsSeed::z3() const
    {
      return m_z3;
    }
 
  inline const double& SiSpacePointsSeed::r1() const
    {
      return m_r1;
    }
 
  inline const double& SiSpacePointsSeed::r2() const
    {
      return m_r2;
    }
 
  inline const double& SiSpacePointsSeed::r3() const
    {
      return m_r3;
    }
 
  inline const double& SiSpacePointsSeed::dzdr_b() const
    {
      return m_dzdr_b;
    }
 
  inline const double& SiSpacePointsSeed::dzdr_t() const
    {
      return m_dzdr_t;
    }
  
  inline const double& SiSpacePointsSeed::pt() const
    {
      return m_pt;
    }
  
} // end of name space

#endif  // SiSpacePointsSeed_h
