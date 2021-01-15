/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TgcClusterOnTrack.cxx
//   Implementation file for class TgcClusterOnTrack
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 06/07/2004  Ketevi A. Assamagan
// adapted from  Veronique Boisvert
///////////////////////////////////////////////////////////////////

#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "TrkEventPrimitives/LocalParameters.h"

namespace Muon
{

// Default constructor:
TgcClusterOnTrack::TgcClusterOnTrack():
        MuonClusterOnTrack(), // call base class ctor
        m_rio(),
        m_detEl(nullptr)
{}

// copy constructor:
TgcClusterOnTrack::TgcClusterOnTrack( const TgcClusterOnTrack& rot):
        MuonClusterOnTrack(rot), // base clas ctor
        m_rio(rot.m_rio),
        m_detEl(rot.m_detEl)
{}

// Constructor with parameters
TgcClusterOnTrack::TgcClusterOnTrack(
                     const TgcPrepData* RIO,
                     const Trk::LocalParameters& locpos,
                     const Amg::MatrixX& locerr,
                     double positionAlongStrip) :
  MuonClusterOnTrack(locpos, locerr, RIO->identify(), positionAlongStrip), //call base class constructor
  m_detEl( RIO->detectorElement() )
{
    //Set EL
    // m_rio = ElementLinkToIDC_TGC_Container("TGC_Measurements", RIO->getHashAndIndex().hashAndIndex(), RIO);
    m_rio.setElement(RIO);
}

TgcClusterOnTrack::TgcClusterOnTrack(
                     const ElementLinkToIDC_TGC_Container& RIO,
                     const Trk::LocalParameters& locpos,
                     const Amg::MatrixX& locerr,
                     const Identifier& id,
                     const MuonGM::TgcReadoutElement* detEl,
                     double positionAlongStrip) :
  MuonClusterOnTrack(locpos, locerr, id, positionAlongStrip), //call base class constructor
  m_rio(RIO),
  m_detEl( detEl )
{
}

// Destructor:
TgcClusterOnTrack::~TgcClusterOnTrack()
{
    // we don't own the m_rio object (it belongs to SG), so don't delete it.
}

// assignment operator:
TgcClusterOnTrack& TgcClusterOnTrack::operator=( const TgcClusterOnTrack& rot)
{
    if ( &rot != this)
    {
        MuonClusterOnTrack::operator=(rot);//base class ass. op.
        m_rio = rot.m_rio;
        m_detEl = rot.m_detEl;
    }
    return *this;
}

MsgStream& TgcClusterOnTrack::dump( MsgStream&    stream) const
{
    stream << MSG::INFO<<"TgcClusterOnTrack {"<<std::endl;

    MuonClusterOnTrack::dump(stream); 

    stream<<"}"<<endmsg;
    return stream;
}

std::ostream& TgcClusterOnTrack::dump( std::ostream&    stream) const
{
    stream << "TgcClusterOnTrack {"<<std::endl;

    MuonClusterOnTrack::dump(stream); 

    stream<<"}"<<std::endl;
    return stream;
}


}


