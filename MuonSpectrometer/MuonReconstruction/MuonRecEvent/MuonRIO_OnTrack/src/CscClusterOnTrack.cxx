/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CscClusterOnTrack.cxx
//   Implementation file for class CscClusterOnTrack
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 06/07/2004  Ketevi A. Assamagan
// adapted from  Veronique Boisvert
///////////////////////////////////////////////////////////////////

#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "TrkEventPrimitives/LocalParameters.h"

namespace Muon
{

  // Default constructor:
  CscClusterOnTrack::CscClusterOnTrack():
    MuonClusterOnTrack(), // call base class ctor
    m_rio(),
    m_detEl(nullptr),
    m_status(CscStatusUndefined),
    m_timeStatus(CscTimeStatusUndefined),
    m_time(9999.0)
  {}

  // copy constructor:
  CscClusterOnTrack::CscClusterOnTrack( const CscClusterOnTrack& rot):
    MuonClusterOnTrack(rot), // base clas ctor
    m_rio(rot.m_rio),
    m_detEl(rot.m_detEl),
    m_status(rot.m_status),
    m_timeStatus(rot.m_timeStatus),
    m_time(rot.m_time)
  {}

  // Constructor with parameters
  CscClusterOnTrack::CscClusterOnTrack(
                       const CscPrepData* RIO,
                       const Trk::LocalParameters& locpos,
                       const Amg::MatrixX& locerr,
                       double positionAlongStrip,
                       CscClusterStatus status,
                       CscTimeStatus timeStatus,
                       float time ) :
    MuonClusterOnTrack(locpos, locerr, RIO->identify(), positionAlongStrip), //call base class constructor
    m_detEl( RIO->detectorElement() ),
    m_status( status ),
    m_timeStatus( timeStatus ),
    m_time(time)
{
    //Set EL
    // m_rio = ElementLinkToIDC_CSC_Container("CSC_Clusters", RIO->getHashAndIndex().hashAndIndex(), RIO);
    m_rio.setElement(RIO);
}

  CscClusterOnTrack::CscClusterOnTrack(
                       const ElementLinkToIDC_CSC_Container& RIO,
                       const Trk::LocalParameters& locpos,
                       const Amg::MatrixX& locerr,
                       const Identifier& id,
                       const MuonGM::CscReadoutElement*  detEl,
                       double positionAlongStrip,
                       CscClusterStatus status,
                       CscTimeStatus timeStatus,
                       float time ) :
    MuonClusterOnTrack(locpos, locerr, id, positionAlongStrip), //call base class constructor
    m_rio( RIO ),
    m_detEl( detEl ),
    m_status( status ),
    m_timeStatus( timeStatus ),
    m_time(time)
{
}

  // Destructor:
  CscClusterOnTrack::~CscClusterOnTrack()
  {
  }

  // assignment operator:
  CscClusterOnTrack& CscClusterOnTrack::operator=( const CscClusterOnTrack& rot)
  {
    if ( &rot != this)
      {
        MuonClusterOnTrack::operator=(rot);//base class ass. op.
        m_rio = rot.m_rio;
        m_detEl = rot.m_detEl;
        m_status = rot.m_status;
        m_timeStatus = rot.m_timeStatus;
        m_time = rot.m_time;
      }
    return *this;
  }
  
  MsgStream& CscClusterOnTrack::dump( MsgStream&    stream) const
  {
    stream << MSG::INFO<<"CscClusterOnTrack {"<<std::endl;
    stream <<"Time: "<<m_time<<std::endl;
    stream <<"Status: "<<m_status<<std::endl;
    stream <<"TimeStatus: "<<m_timeStatus<<std::endl;

    MuonClusterOnTrack::dump(stream); 

    stream << "Cluster Status: "<<toString(m_status)<<std::endl;
    stream << "Cluster Time Status: "<<toString(m_timeStatus)<<std::endl;

    stream<<"}"<<endmsg;
    return stream;
  }

  std::ostream& CscClusterOnTrack::dump( std::ostream&    stream) const
  {
    stream << "CscClusterOnTrack {"<<std::endl;
    stream <<"Time: "<<m_time<<std::endl;
    stream <<"Status: "<<m_status<<std::endl;
    stream <<"TimeStatus: "<<m_timeStatus<<std::endl;

    MuonClusterOnTrack::dump(stream); 

    stream << "Cluster Status: "<<toString(m_status)<<std::endl;
    stream << "Cluster Time Status: "<<toString(m_timeStatus)<<std::endl;

    stream<<"}"<<std::endl;
    return stream;
  }

}


