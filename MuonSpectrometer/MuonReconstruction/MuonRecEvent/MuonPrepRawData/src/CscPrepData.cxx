/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CscPrepData.cxx
//   Implementation file for class CscPrepData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 03/29/2005 Ketevi A. Assamagan
///////////////////////////////////////////////////////////////////

#include <new>
#include <sstream>
#include "MuonPrepRawData/CscPrepData.h"
#include "GaudiKernel/MsgStream.h"

namespace Muon
{

  CscPrepData::CscPrepData( const Identifier& RDOId,
                            const IdentifierHash& idDE,
                            const Amg::Vector2D& locpos,
                            const std::vector<Identifier>& rdoList,
                            const Amg::MatrixX* locErrMat,
                            const MuonGM::CscReadoutElement* detEl,
                            const int charge,
                            const double time,
                            const CscClusterStatus status,
                            const CscTimeStatus timeStatus) :
    MuonCluster(RDOId, idDE, locpos, rdoList, locErrMat), //call base class constructor
    m_detEl(detEl),
    m_charge(charge),
    m_time(time),
    m_status(status),
    m_timeStatus(timeStatus)
  { }


  // Destructor:
  CscPrepData::~CscPrepData()
  {
  }

  // Default constructor:
  CscPrepData::CscPrepData():
    MuonCluster(),
    m_detEl(0),
    m_charge(0),
    m_time(0),
    m_status(CscStatusUndefined),
    m_timeStatus(CscTimeStatusUndefined)
  { }


  MsgStream& CscPrepData::dump( MsgStream&    stream) const
  {
    stream << MSG::INFO << "CscPrepData {" <<std::endl;

    MuonCluster::dump(stream);

    std::stringstream s;
    s << "Charge = "<<this->charge() << ", ";
    s << std::setiosflags(std::ios::fixed);
    s << std::setprecision(4);
    s << "Time = "<<this->time() << ", ";
    s << "Status = "<<this->status();    
    s << "TimeStatus = "<<this->timeStatus();    
    s << "}" << std::endl;
    stream << s.str();

    return stream;
  }

  std::ostream& CscPrepData::dump( std::ostream&    stream) const
  {
    stream << "CscPrepData {"<<std::endl;
    
    MuonCluster::dump(stream);
    
    std::stringstream s;
    s << "Charge = " << this->charge() << ", ";
    s << std::setiosflags(std::ios::fixed);
    s << std::setprecision(4);
    s << "Time = " << this->time() << ", ";
    s << "Status = "<<this->status();    
    s << "TimeStatus = "<<this->timeStatus();    
    s << "}" <<std::endl;
    stream << s.str();

    return stream;
  }
  //end of classdef

}//end of ns
