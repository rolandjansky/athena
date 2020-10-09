/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITk_Strip_ClusterOnTrack.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

#include <new>
#include "InDetRIO_OnTrack/ITk_Strip_ClusterOnTrack.h"
#include "InDetPrepRawData/ITk_Strip_Cluster.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "GaudiKernel/MsgStream.h"
#include <ostream>
#include <limits>



// Constructor with parameters - no global position assigned
InDet::ITk_Strip_ClusterOnTrack::ITk_Strip_ClusterOnTrack(const InDet::ITk_Strip_Cluster* RIO, 
           const Trk::LocalParameters& locpars, 
           const Amg::MatrixX& locerr, 
           const IdentifierHash& idDE,
           bool isbroad) : 
  SiClusterOnTrack(locpars, locerr, idDE, RIO->identify(),isbroad) //call base class constructor
{
  //m_rio = ElementLinkToIDCITk_Strip_ClusterContainer("ITk_Strip_Clusters", RIO->getHashAndIndex().hashAndIndex(), RIO);
  m_detEl=nullptr;
  m_positionAlongStrip=std::numeric_limits<double>::quiet_NaN();
  m_rio.setElement(RIO);
}

// Constructor with parameters
InDet::ITk_Strip_ClusterOnTrack::ITk_Strip_ClusterOnTrack(const InDet::ITk_Strip_Cluster* RIO, 
				   const Trk::LocalParameters& locpars,
				   const Amg::MatrixX& locerr, 
				   const IdentifierHash& idDE,
				   const Amg::Vector3D& globalPosition,
                                   bool isbroad) : 
  SiClusterOnTrack(locpars, locerr, idDE, RIO->identify(), globalPosition, isbroad), //call base class constructor
  m_detEl( RIO->detectorElement() )
{
  m_rio.setElement(RIO);
  
//constructing local position provided a global one  
  const Amg::Vector2D * lpos =  detectorElement()->surface( identify() ).
                                positionOnSurface(globalPosition);
					
//storing the position along the strip if available
  m_positionAlongStrip = (lpos) ? (*lpos)[Trk::locY]:0.; 
  delete lpos;
}

InDet::ITk_Strip_ClusterOnTrack::ITk_Strip_ClusterOnTrack( const ElementLinkToIDCITk_Strip_ClusterContainer& RIO,
                                               const Trk::LocalParameters& locpars, 
                                               const Amg::MatrixX& locerr, 
                                               IdentifierHash idDE,
                                               const Identifier& id,
                                               bool isbroad,
                                               double positionAlongStrip)
  : SiClusterOnTrack(locpars,
                     locerr,
                     idDE,
                     id,
                     isbroad),
    m_rio (RIO),
    m_detEl (nullptr),
    m_positionAlongStrip (positionAlongStrip)
{}


// Destructor:
InDet::ITk_Strip_ClusterOnTrack::~ITk_Strip_ClusterOnTrack()
{}

// Default constructor:
InDet::ITk_Strip_ClusterOnTrack::ITk_Strip_ClusterOnTrack():
  SiClusterOnTrack(),
  m_rio(),
  m_detEl(0),
  m_positionAlongStrip(std::numeric_limits<double>::quiet_NaN())
{}

// copy constructor:
InDet::ITk_Strip_ClusterOnTrack::ITk_Strip_ClusterOnTrack( const InDet::ITk_Strip_ClusterOnTrack& rot):
  SiClusterOnTrack(rot),
  m_rio(rot.m_rio),
  m_detEl(rot.m_detEl),
  m_positionAlongStrip(rot.m_positionAlongStrip)
{}

// assignment operator:
InDet::ITk_Strip_ClusterOnTrack& InDet::ITk_Strip_ClusterOnTrack::operator=( const InDet::ITk_Strip_ClusterOnTrack& rot){
  if ( &rot != this) {
    InDet::SiClusterOnTrack::operator=(rot);//base class ass. op.
    m_rio                 = rot.m_rio;
    m_detEl               = rot.m_detEl;
    m_positionAlongStrip  = rot.m_positionAlongStrip;
  }
  return *this;
}

const Trk::Surface& InDet::ITk_Strip_ClusterOnTrack::associatedSurface() const
{ 
    return ( detectorElement()->surface()); 
}
  
void InDet::ITk_Strip_ClusterOnTrack::setValues(const Trk::TrkDetElementBase* detEl, const Trk::PrepRawData* )
{
    m_detEl = dynamic_cast< const InDetDD::SiDetectorElement* >(detEl);
}
  
const  Amg::Vector3D& InDet::ITk_Strip_ClusterOnTrack::globalPosition() const
  {
  
   //checking whether the globalposition is available
   if(!m_globalPosition)
   {
    
    // calculate global position from the position of the strip and the position along the strip
     Amg::Vector2D lpos( localParameters().get(Trk::locX), m_positionAlongStrip );
     m_globalPosition = detectorElement()->surface( identify() ).localToGlobal( lpos );            
   }
    
   return (*m_globalPosition);
} 

MsgStream& InDet::ITk_Strip_ClusterOnTrack::dump( MsgStream& sl ) const
{
  SiClusterOnTrack::dump(sl);// use dump(...) from SiClusterOnTrack
  return sl;
}

std::ostream& InDet::ITk_Strip_ClusterOnTrack::dump( std::ostream& sl ) const
{
  SiClusterOnTrack::dump(sl);// use dump(...) from SiClusterOnTrack
  return sl;
}
