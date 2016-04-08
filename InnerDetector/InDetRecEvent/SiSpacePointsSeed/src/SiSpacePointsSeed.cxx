/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class SiSpacePointsSeed
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Version 1.0 18/11/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include "GaudiKernel/MsgStream.h"
#include "SiSpacePointsSeed/SiSpacePointsSeed.h"

namespace InDet {

  /////////////////////////////////////////////////////////////////////////////////
  // Constructors
  /////////////////////////////////////////////////////////////////////////////////
    
  SiSpacePointsSeed::SiSpacePointsSeed ()
  {
    m_zvertex = 0.;
  }
 
  SiSpacePointsSeed::SiSpacePointsSeed
  (const Trk::SpacePoint*& p1,const Trk::SpacePoint*& p2,
   const double& z) 
  {
    m_spacepoints.push_back(p1);
    m_spacepoints.push_back(p2);
    m_zvertex = z;
  }

  SiSpacePointsSeed::SiSpacePointsSeed
  (const Trk::SpacePoint*& p1,const Trk::SpacePoint*& p2,const Trk::SpacePoint*& p3,
   const double& z) 
  {
    m_spacepoints.push_back(p1);
    m_spacepoints.push_back(p2);
    m_spacepoints.push_back(p3);
    m_zvertex = z;
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Copy constructor
  /////////////////////////////////////////////////////////////////////////////////

  SiSpacePointsSeed::SiSpacePointsSeed (const SiSpacePointsSeed& Se)
  {
    m_spacepoints = Se.m_spacepoints;
    m_zvertex     = Se.m_zvertex    ;
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Operator =
  /////////////////////////////////////////////////////////////////////////////////
  
  SiSpacePointsSeed& SiSpacePointsSeed::operator = (const SiSpacePointsSeed& Se) 
  {
    if(&Se!=this) {
      m_spacepoints = Se.m_spacepoints;
      m_zvertex     = Se.m_zvertex    ;
    }
    return *this;
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Destructor
  /////////////////////////////////////////////////////////////////////////////////

  SiSpacePointsSeed::~SiSpacePointsSeed() 
  {
  }

  ///////////////////////////////////////////////////////////////////
  // Dumps relevant information into the MsgStream
  ///////////////////////////////////////////////////////////////////
  
  MsgStream& SiSpacePointsSeed::dump( MsgStream& out ) const
  {

    out<<"InDet::SpacePointsSeed contains: "
       <<std::setw(3)<<m_spacepoints.size()
       <<" space points"
       <<std::endl;
    std::list<const Trk::SpacePoint*>::const_iterator is,ise = m_spacepoints.end();  
    for(is=m_spacepoints.begin(); is!=ise; ++is) {
      
      if((*is)->clusterList().second == 0) {
	out<<"pix: r,f,z = "
	   <<std::setw(12)<<std::setprecision(5)<<(*is)->globalPosition().perp()
	   <<std::setw(12)<<std::setprecision(5)<<(*is)->globalPosition().phi ()
	   <<std::setw(12)<<std::setprecision(5)<<(*is)->globalPosition().z   ()
	   <<std::endl;
      }
      else {
	out<<"sct: r,f,z = "
	   <<std::setw(12)<<std::setprecision(5)<<(*is)->globalPosition().perp()
	   <<std::setw(12)<<std::setprecision(5)<<(*is)->globalPosition().phi ()
	   <<std::setw(12)<<std::setprecision(5)<<(*is)->globalPosition().z   ()
	   <<std::endl;
      }                                    
    }
    return out;
  }

  ///////////////////////////////////////////////////////////////////
  // Dumps relevant information into the ostream
  ///////////////////////////////////////////////////////////////////

  std::ostream& SiSpacePointsSeed::dump( std::ostream& out ) const
  {
    
    out<<"InDet::SpacePointsSeed contains: "
       <<std::setw(3)<<m_spacepoints.size()
       <<" space points"
       <<std::endl;
    std::list<const Trk::SpacePoint*>::const_iterator is,ise = m_spacepoints.end();  
    for(is=m_spacepoints.begin(); is!=ise; ++is) {
      
      if((*is)->clusterList().second == 0) {
	out<<"pix: r,f,z = "
	   <<std::setw(12)<<std::setprecision(5)<<(*is)->globalPosition().perp()
	   <<std::setw(12)<<std::setprecision(5)<<(*is)->globalPosition().phi ()
	   <<std::setw(12)<<std::setprecision(5)<<(*is)->globalPosition().z   ()
	   <<std::endl;
      }
      else {
	out<<"sct: r,f,z = "
	   <<std::setw(12)<<std::setprecision(5)<<(*is)->globalPosition().perp()
	   <<std::setw(12)<<std::setprecision(5)<<(*is)->globalPosition().phi ()
	   <<std::setw(12)<<std::setprecision(5)<<(*is)->globalPosition().z   ()
	   <<std::endl;
      }                                    
    }
     return out;
  }

  ///////////////////////////////////////////////////////////////////
  // Overload of << operator MsgStream
  ///////////////////////////////////////////////////////////////////
  
  MsgStream& operator    << (MsgStream&    sl,const SiSpacePointsSeed& se)
  { 
    return se.dump(sl); 
  }
  ///////////////////////////////////////////////////////////////////
  // Overload of << operator std::ostream
  ///////////////////////////////////////////////////////////////////
  
  std::ostream& operator << (std::ostream& sl,const SiSpacePointsSeed& se)
  { 
    return se.dump(sl); 
  }
   
} // end of name space





