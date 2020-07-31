/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_ClusterOnTrack.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetPrepRawData/SCT_Cluster.h"

#include "GaudiKernel/MsgStream.h"
#include <ostream>
#include <limits>


// Constructor with parameters - no global position assigned
InDet::SCT_ClusterOnTrack::SCT_ClusterOnTrack(const InDet::SCT_Cluster* RIO, 
           const Trk::LocalParameters& locpars, 
           const Amg::MatrixX& locerr, 
           const IdentifierHash& idDE,
           bool isbroad) : 
  SiClusterOnTrack(locpars, locerr, idDE, RIO->identify(),isbroad) //call base class constructor
{
  m_detEl=RIO->detectorElement();
  m_positionAlongStrip=std::numeric_limits<double>::quiet_NaN();
  m_rio.setElement(RIO);

  // Set global position
  Amg::Vector2D lpos(localParameters().get(Trk::locX), m_positionAlongStrip);
  m_globalPosition = detectorElement()->surface(identify()).localToGlobalPos(lpos);
}

// Constructor with parameters
InDet::SCT_ClusterOnTrack::SCT_ClusterOnTrack(const InDet::SCT_Cluster* RIO, 
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
  std::unique_ptr<const Amg::Vector2D>
    lpos{detectorElement()->surface( identify() ).positionOnSurface(globalPosition)};
					
//storing the position along the strip if available
  m_positionAlongStrip = (lpos) ? (*lpos)[Trk::locY]:0.; 
}

InDet::SCT_ClusterOnTrack::SCT_ClusterOnTrack( const ElementLinkToIDCSCT_ClusterContainer& RIO,
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
    m_rio(RIO),
    m_detEl(nullptr),
    m_positionAlongStrip (positionAlongStrip)
{
}


// Default constructor:
InDet::SCT_ClusterOnTrack::SCT_ClusterOnTrack():
  SiClusterOnTrack(),
  m_rio(),
  m_detEl(nullptr),
  m_positionAlongStrip(std::numeric_limits<double>::quiet_NaN())
{}


const Trk::Surface& InDet::SCT_ClusterOnTrack::associatedSurface() const
{ 
    return ( detectorElement()->surface()); 
}
  
void InDet::SCT_ClusterOnTrack::setValues(const Trk::TrkDetElementBase* detEl, const Trk::PrepRawData* )
{
    m_detEl = dynamic_cast< const InDetDD::SiDetectorElement* >(detEl);
    if (m_detEl) {
      // Set global position after setting the detector element
      Amg::Vector2D lpos(localParameters().get(Trk::locX),
                         m_positionAlongStrip);
      m_globalPosition =
        detectorElement()->surface(identify()).localToGlobalPos(lpos);
    }
}

MsgStream& InDet::SCT_ClusterOnTrack::dump( MsgStream& sl ) const
{
  SiClusterOnTrack::dump(sl);// use dump(...) from SiClusterOnTrack
  return sl;
}

std::ostream& InDet::SCT_ClusterOnTrack::dump( std::ostream& sl ) const
{
  SiClusterOnTrack::dump(sl);// use dump(...) from SiClusterOnTrack
  return sl;
}
