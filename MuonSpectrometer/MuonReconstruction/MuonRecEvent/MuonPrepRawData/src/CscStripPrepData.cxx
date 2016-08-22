/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CscStripPrepData.cxx
//   Implementation file for class CscStripPrepData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 03/29/2005 Ketevi A. Assamagan
///////////////////////////////////////////////////////////////////

#include <new>
#include <sstream>
#include "MuonPrepRawData/CscStripPrepData.h"
#include "GaudiKernel/MsgStream.h"

namespace Muon
{


  CscStripPrepData::CscStripPrepData( const Identifier& RDOId,
                      const IdentifierHash& collectionHash,
                      const Amg::Vector2D& locpos,
                      const Amg::MatrixX* locErrMat,
                      const MuonGM::CscReadoutElement* detEl,
                      const std::vector<float>& sampleCharges,
                      float timeOfFirstSample,
                      unsigned short samplingTime 
                      ):
    PrepRawData(RDOId, locpos, locErrMat), //call base class constructor
    m_collectionHash(collectionHash),
    m_globalPosition(0),
    m_detEl(detEl),
    m_sampleCharges(sampleCharges),
    m_timeOfFirstSample(timeOfFirstSample),
    m_samplingTime(samplingTime),
    m_samplingPhase(false)
  { 
  }


  // Destructor:
  CscStripPrepData::~CscStripPrepData()
  {
      delete m_globalPosition;
  }

  // Default constructor:
  CscStripPrepData::CscStripPrepData():
    PrepRawData(),
    m_collectionHash(),
    m_globalPosition(0),
    m_detEl(0),
    m_sampleCharges(),
    m_timeOfFirstSample(0),
    m_samplingTime(0),
    m_samplingPhase(false)
  { }

  //copy constructor:
  CscStripPrepData::CscStripPrepData(const CscStripPrepData& RIO):
    PrepRawData(RIO),
    m_collectionHash( RIO.m_collectionHash ),
    m_globalPosition(0),
    m_detEl( RIO.m_detEl ),
    m_sampleCharges( RIO.m_sampleCharges ),
    m_timeOfFirstSample ( RIO.m_timeOfFirstSample ),
    m_samplingTime( RIO.m_samplingTime ),
    m_samplingPhase( RIO.m_samplingPhase )
  { }

  //assignment operator
  CscStripPrepData& CscStripPrepData::operator=(const CscStripPrepData& RIO)
  {
    if (&RIO !=this)
      {
	delete m_globalPosition;
	m_globalPosition=0;
        PrepRawData::operator=(RIO);
        m_collectionHash   = RIO.m_collectionHash;
        m_detEl  = RIO.m_detEl ;
        m_sampleCharges = RIO.m_sampleCharges;
        m_timeOfFirstSample = RIO.m_timeOfFirstSample;
        m_samplingTime = RIO.m_samplingTime;
        m_samplingPhase = RIO.m_samplingPhase;
      }
    return *this;
  }

  MsgStream& CscStripPrepData::dump( MsgStream&    stream) const
  {
    stream << MSG::INFO << "CscStripPrepData {" <<endmsg;

    std::stringstream s;
    s << std::setiosflags(std::ios::fixed);
    s << std::setprecision(4);
    s << "Samples = ";
    for( unsigned int i=0;i<m_sampleCharges.size();++i) s << m_sampleCharges[i] << " , ";
    s << std::endl;
    s << "Time of first sample= "<<this->timeOfFirstSample() << ", sampling Time (ns) " 
           << this->samplingTime() << ", sampling Phase " << this->samplingPhase();
    s << "} End CscStripPrepData" << std::endl;
    stream << s.str();

    return stream;
  }

  std::ostream& CscStripPrepData::dump( std::ostream&    stream) const
  {
    stream << "CscStripPrepData {"<<std::endl;

    std::stringstream s;   
    s << std::setiosflags(std::ios::fixed);
    s << std::setprecision(4);
    
    s << "Samples = ";
    for( unsigned int i=0;i<m_sampleCharges.size();++i) s << m_sampleCharges[i] << " , ";
    s << std::endl;
    s << "Time of first sample= "<<this->timeOfFirstSample() << ", sampling time(ns) " 
      << this->samplingTime();
    s << "} End CscStripPrepData" << std::endl;
    stream << s.str();

    return stream;
  }
  //end of classdef

}//end of ns
