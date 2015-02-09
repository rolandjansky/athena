/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


///////////////////////////////////////////////////////////////////
// RIO_OnTrack.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

//Trk
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkPrepRawData/PrepRawData.h"
// Gaudi & AthenaCommon
#include "Identifier/Identifier.h"
#include "GaudiKernel/MsgStream.h"
#include <string>
#include <typeinfo>

unsigned int Trk::RIO_OnTrack::s_numberOfInstantiations=0;

// Constructor with parameters:
Trk::RIO_OnTrack::RIO_OnTrack(  const Trk::LocalParameters& locpars,
                                const Amg::MatrixX& loccov,
                                const Identifier& id) 
  : MeasurementBase(locpars,loccov),
    m_identifier(id)
{
#ifndef NDEBUG
   s_numberOfInstantiations++; // new RIO_OnTrack, so increment total count
#endif
}

// Destructor:
Trk::RIO_OnTrack::~RIO_OnTrack()
{
#ifndef NDEBUG
    s_numberOfInstantiations--; // delete RIO_OnTrack, so decrement total count
#endif
}

// default constructor:
Trk::RIO_OnTrack::RIO_OnTrack() :
    m_identifier()
{
#ifndef NDEBUG
   s_numberOfInstantiations++; // new RIO_OnTrack, so increment total count
#endif
}  

// copy constructor:
Trk::RIO_OnTrack::RIO_OnTrack( const Trk::RIO_OnTrack& rot) :
    Trk::MeasurementBase(rot),
    m_identifier( rot.m_identifier )
{
#ifndef NDEBUG
   s_numberOfInstantiations++; // new RIO_OnTrack, so increment total count
#endif
}

// assignment operator:
Trk::RIO_OnTrack& Trk::RIO_OnTrack::operator=(const RIO_OnTrack& rot)
{
    if ( &rot != this) {
      Trk::MeasurementBase::operator=(rot);
      m_identifier         = rot.m_identifier; 
    }
    return *this;
}

MsgStream& Trk::RIO_OnTrack::dump( MsgStream& sl ) const
{
    sl << "Trk::RIO_OnTrack { "<< endreq;
    sl << "\t  identifier = "<< identify() << endreq;
    sl << "\t  position = (" 
       << localParameters() 
       << endreq;
    sl << "\t  has Error Matrix: "<< endreq;
    sl<< localCovariance() <<"}"<< endreq; 

    if (prepRawData()!=0) {
        sl<<"PrepRawData: "<< (*prepRawData()) << endreq;
    }else{
        sl<<"PrepRawData: NULL"<<endreq;
    }
    return sl;
}

std::ostream& Trk::RIO_OnTrack::dump( std::ostream& sl ) const
{
    sl << "Trk::RIO_OnTrack { "<<std::endl;
    sl << "\t  identifier = "<< identify() << std::endl;
    sl << "\t  position = (" 
       << localParameters()
       << std::endl;
    sl << "\t  has Error Matrix: " << std::endl;
    sl << localCovariance() <<" }" << std::endl; 
    
    if (prepRawData()!=0) {
        sl <<"PrepRawData: "<< (*prepRawData()) << std::endl;
    }else{
        sl<<"PrepRawData: NULL" << std::endl;
    }    
    return sl;
}

unsigned int Trk::RIO_OnTrack::numberOfInstantiations()
{
  return s_numberOfInstantiations;
}




