/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RpcClusterOnTrack.cxx
//   Implementation file for class RpcClusterOnTrack
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 06/07/2004  Ketevi A. Assamagan
// adapted from  Veronique Boisvert
///////////////////////////////////////////////////////////////////

#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "TrkEventPrimitives/LocalParameters.h"

namespace Muon
{

// Default constructor:
RpcClusterOnTrack::RpcClusterOnTrack()
    :
    MuonClusterOnTrack(), // call base class ctor
   m_rio(),
    m_detEl(nullptr),
    m_time(0.0)
{}

// copy constructor:
RpcClusterOnTrack::RpcClusterOnTrack( const RpcClusterOnTrack& rot)
    :
    MuonClusterOnTrack(rot), // base clas ctor
    m_rio(rot.m_rio),
    m_detEl(rot.m_detEl),
    m_time(rot.m_time)
{}


// Constructor with parameters
RpcClusterOnTrack::RpcClusterOnTrack(
                     const RpcPrepData* RIO,
                     const Trk::LocalParameters& locpos,
                     const Amg::MatrixX& locerr,
                     double positionAlongStrip,
                     float time) 
    :
    MuonClusterOnTrack(locpos, locerr, RIO->identify(), positionAlongStrip ), //call base class constructor
    m_detEl( RIO->detectorElement() ),
    m_time(time)
{
    //Set EL
    // m_rio = ElementLinkToIDC_RPC_Container("RPC_Measurements", RIO->getHashAndIndex().hashAndIndex(), RIO);
    m_rio.setElement(RIO);
 }


RpcClusterOnTrack::RpcClusterOnTrack(
                     const ElementLinkToIDC_RPC_Container& RIO,
                     const Trk::LocalParameters& locpos,
                     const Amg::MatrixX& locerr,
                     const Identifier& id,
                     const MuonGM::RpcReadoutElement* detEl,
                     double positionAlongStrip,
                     float time) 
    :
    MuonClusterOnTrack(locpos, locerr, id, positionAlongStrip ), //call base class constructor
    m_rio (RIO),
    m_detEl( detEl ),
    m_time(time)
{
}

// Destructor:
RpcClusterOnTrack::~RpcClusterOnTrack()
{
    // we don't own the m_rio object (it belongs to SG), so don't delete it.
}

// assignment operator:
RpcClusterOnTrack& RpcClusterOnTrack::operator=( const RpcClusterOnTrack& rot)
{
    if ( &rot != this)
    {
        MuonClusterOnTrack::operator=(rot);//base class ass. op.
        m_rio = rot.m_rio;
        m_detEl = rot.m_detEl;
        m_time = rot.m_time;
    }
    return *this;
}

MsgStream& RpcClusterOnTrack::dump( MsgStream&    stream) const
{
  stream << MSG::INFO<<"RpcClusterOnTrack {"<<std::endl;
  stream <<"Time: "<<m_time<<std::endl;
  MuonClusterOnTrack::dump(stream); 

  stream<<"}"<<endmsg;
  return stream;
}

std::ostream& RpcClusterOnTrack::dump( std::ostream&    stream) const
{
  stream << "RpcClusterOnTrack {"<<std::endl;
  stream <<"Time: "<<m_time<<std::endl;

  MuonClusterOnTrack::dump(stream); 

  stream<<"}"<<std::endl;
  return stream;
}

}


