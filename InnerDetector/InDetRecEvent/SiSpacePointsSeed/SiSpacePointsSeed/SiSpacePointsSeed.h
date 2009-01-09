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
    const std::list<const Trk::SpacePoint*>& spacePoints() const;
    const double&                            zVertex    () const;
    virtual MsgStream&    dump(MsgStream&    out) const ;
    virtual std::ostream& dump(std::ostream& out) const ;
    
    /////////////////////////////////////////////////////////////////////////////////
    // Protected data members
    /////////////////////////////////////////////////////////////////////////////////
    
  protected:
    
    std::list<const Trk::SpacePoint*> m_spacepoints;
    double                            m_zvertex    ;  
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

  inline void SiSpacePointsSeed::setZVertex(const double& z) 
    {
      m_zvertex = z;
    }

  inline const double& SiSpacePointsSeed::zVertex() const 
    {
      return m_zvertex;
    }
  
} // end of name space

#endif  // SiSpacePointsSeed_h
