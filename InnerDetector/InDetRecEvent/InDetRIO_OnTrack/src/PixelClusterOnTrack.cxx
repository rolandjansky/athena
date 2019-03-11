/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelClusterOnTrack.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "GaudiKernel/MsgStream.h"
#include <ostream>


// Constructor with parameters - no global position specified
InDet::PixelClusterOnTrack::PixelClusterOnTrack(
  const InDet::PixelCluster* RIO, 
  const Trk::LocalParameters& locpars, 
  const Amg::MatrixX& locerr, 
  const IdentifierHash& idDE,
  bool,
  bool isbroad 
  ) 
  : 
  InDet::SiClusterOnTrack(locpars, locerr, idDE, RIO->identify(),isbroad), //call base class constructor
  m_hasClusterAmbiguity(RIO->isAmbiguous()),
  m_isFake(RIO->isFake()),
  m_energyLoss(RIO->energyLoss()),
  m_detEl( RIO->detectorElement() )
{
  m_rio.setElement(RIO);
}

// Constructor with parameters
InDet::PixelClusterOnTrack::PixelClusterOnTrack(
    const InDet::PixelCluster* RIO, 
    const Trk::LocalParameters& locpars, 
    const Amg::MatrixX& locerr, 
    const IdentifierHash& idDE,
    const Amg::Vector3D& globalPosition,
    bool,
    bool isbroad
    ) 
    : 
    InDet::SiClusterOnTrack(locpars, locerr, idDE, RIO->identify(), globalPosition, isbroad), //call base class constructor
    m_hasClusterAmbiguity(RIO->isAmbiguous()),
    m_isFake(RIO->isFake()),
    m_energyLoss(RIO->energyLoss()),
    m_detEl( RIO->detectorElement() )
{
  m_rio.setElement(RIO);
}


InDet::PixelClusterOnTrack::PixelClusterOnTrack
  ( const ElementLinkToIDCPixelClusterContainer& RIO,
    const Trk::LocalParameters& locpars, 
    const Amg::MatrixX& locerr, 
    const IdentifierHash& idDE,
    const Identifier& id,
    float energyLoss,
    bool isFake,
    bool hasClusterAmbiguity,
    bool isbroad)
 : InDet::SiClusterOnTrack(locpars, locerr, idDE, id, isbroad),
  m_rio (RIO),
  m_hasClusterAmbiguity (hasClusterAmbiguity),
  m_isFake (isFake),
  m_energyLoss (energyLoss),
  m_detEl (nullptr)
{
}
    


// Destructor:
InDet::PixelClusterOnTrack::~PixelClusterOnTrack()
{}

// Default constructor:
InDet::PixelClusterOnTrack::PixelClusterOnTrack()
    :
    InDet::SiClusterOnTrack(),
    m_rio(),
    m_hasClusterAmbiguity(false),
    m_isFake(false),
    m_energyLoss(0.),
    m_detEl(0)
{}




const Trk::Surface& InDet::PixelClusterOnTrack::associatedSurface() const
{ return ( detectorElement()->surface()); }
      
              
void InDet::PixelClusterOnTrack::setValues(const Trk::TrkDetElementBase* detEl, const Trk::PrepRawData* /*prd*/)
{
    m_detEl = dynamic_cast< const InDetDD::SiDetectorElement* >(detEl);
}

  

MsgStream& InDet::PixelClusterOnTrack::dump( MsgStream& sl ) const
{
    sl<<"PixelClusterOnTrack {"<<endmsg;
    InDet::SiClusterOnTrack::dump(sl); // use common dump(...) from SiClusterOnTrack
    sl<<"Ganged cluster ambiguity: "<<hasClusterAmbiguity()
      <<", fake: " << isFake()
      <<", dedX: " << energyLoss()
      <<endmsg;
    sl<<"}"<<endmsg;
    return sl;
}

std::ostream& InDet::PixelClusterOnTrack::dump( std::ostream& sl ) const
{
    sl<<"PixelClusterOnTrack {"<<std::endl;
    InDet::SiClusterOnTrack::dump(sl);// use common dump(...) from SiClusterOnTrack
    sl<<"Ganged cluster ambiguity: "<<hasClusterAmbiguity()
      <<", fake: " << isFake()
      <<", dedX: " << energyLoss()
      <<std::endl;
    sl<<"}"<<std::endl;
    return sl;
}







