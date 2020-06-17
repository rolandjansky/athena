/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPrepRawData/MMPrepData.h"
#include "GaudiKernel/MsgStream.h"

namespace Muon
{

  MMPrepData::MMPrepData( const Identifier& RDOId,
                          const IdentifierHash &idDE,
                          const Amg::Vector2D& locpos,
                          const std::vector<Identifier>& rdoList,
                          const Amg::MatrixX* locErrMat,
                          const MuonGM::MMReadoutElement* detEl,
			  const short int time, 
			  const int charge,
			  const float driftDist,
			  const std::vector<uint16_t>& stripNumbers, 
			  const std::vector<short int>& stripTimes, 
			  const std::vector<int>& stripCharges ) :
    MuonCluster(RDOId, idDE, locpos, rdoList, locErrMat), //call base class constructor
    m_detEl(detEl),
    m_time(time),
    m_charge(charge),
    m_driftDist(driftDist),
    m_angle(0.0),
    m_chisqProb(0.0),
    m_stripNumbers(stripNumbers),
    m_stripTimes(stripTimes),
    m_stripCharges(stripCharges),
    m_stripDriftDist(),
    m_stripDriftErrors()
  { }

  MMPrepData::MMPrepData( const Identifier& RDOId,
                          const IdentifierHash &idDE,
                          const Amg::Vector2D& locpos,
                          const std::vector<Identifier>& rdoList,
                          const Amg::MatrixX* locErrMat,
                          const MuonGM::MMReadoutElement* detEl,
			  const short int time, 
			  const int charge ) :
    MuonCluster(RDOId, idDE, locpos, rdoList, locErrMat), //call base class constructor
    m_detEl(detEl),
    m_time(time),
    m_charge(charge),
    m_driftDist(0.0),
    m_angle(0.0),
    m_chisqProb(0.0),
    m_stripNumbers(),
    m_stripTimes(),
    m_stripCharges()
  { }

  MMPrepData::MMPrepData( const Identifier& RDOId,
                          const IdentifierHash &idDE,
                          const Amg::Vector2D& locpos,
                          const std::vector<Identifier>& rdoList,
                          const Amg::MatrixX* locErrMat,
                          const MuonGM::MMReadoutElement* detEl,
			  const short int time, 
			  const int charge, 
			  const float driftDist ) :
    MuonCluster(RDOId, idDE, locpos, rdoList, locErrMat), //call base class constructor
    m_detEl(detEl),
    m_time(time),
    m_charge(charge),
    m_driftDist(driftDist),
    m_angle(0.0),
    m_chisqProb(0.0),
    m_stripNumbers(),
    m_stripTimes(),
    m_stripCharges()
  { }

  MMPrepData::MMPrepData( const Identifier& RDOId,
                          const IdentifierHash &idDE,
                          const Amg::Vector2D& locpos,
                          const std::vector<Identifier>& rdoList,
                          const Amg::MatrixX* locErrMat,
                          const MuonGM::MMReadoutElement* detEl) :
    MuonCluster(RDOId, idDE, locpos, rdoList, locErrMat), //call base class constructor
    m_detEl(detEl),
    m_time(0),
    m_charge(0),
    m_driftDist(0.0),
    m_angle(0.0),
    m_chisqProb(0.0),
    m_stripNumbers(),
    m_stripTimes(),
    m_stripCharges(),
    m_stripDriftDist(),
    m_stripDriftErrors()
  { }

  // Destructor:
  MMPrepData::~MMPrepData()
  {

  }

  // Default constructor:
  MMPrepData::MMPrepData():
    MuonCluster(),
    m_detEl(0),
    m_time(0),
    m_charge(0),
    m_driftDist(0),
    m_angle(0.0),
    m_chisqProb(0.0),
    m_stripNumbers(),
    m_stripTimes(),
    m_stripCharges(),
    m_stripDriftDist(),
    m_stripDriftErrors()    
  { }

  //copy constructor:
  MMPrepData::MMPrepData(const MMPrepData& RIO):
    MuonCluster(RIO),
    m_detEl( RIO.m_detEl ),
    m_time(RIO.m_time),
    m_charge(RIO.m_charge),
    m_driftDist(RIO.m_driftDist),
    m_angle(RIO.m_angle),
    m_chisqProb(RIO.m_chisqProb),
    m_stripNumbers(RIO.m_stripNumbers),
    m_stripTimes(RIO.m_stripTimes),
    m_stripCharges(RIO.m_stripCharges),
    m_stripDriftDist(RIO.m_stripDriftDist),
    m_stripDriftErrors(RIO.m_stripDriftErrors)
  { }

  //move constructor:
  MMPrepData::MMPrepData(MMPrepData&& RIO):
    MuonCluster(std::move(RIO)),
    m_detEl( RIO.m_detEl ),
    m_time(RIO.m_time),
    m_charge(RIO.m_charge),
    m_driftDist(RIO.m_driftDist),
    m_angle(RIO.m_angle),
    m_chisqProb(RIO.m_chisqProb),
    m_stripNumbers(RIO.m_stripNumbers),
    m_stripTimes(RIO.m_stripTimes),
    m_stripCharges(RIO.m_stripCharges),
    m_stripDriftDist(RIO.m_stripDriftDist),
    m_stripDriftErrors(RIO.m_stripDriftErrors)
  { }

  /// set the micro-tpc quantities
  void MMPrepData::setMicroTPC(float angle, float chisqProb)
  {
    m_angle = angle;
    m_chisqProb = chisqProb;
  }

  /// set drift distances and errors
  void MMPrepData::setDriftDist(const std::vector<float>& driftDist, const std::vector<Amg::MatrixX>& driftDistErrors)
  {
    m_stripDriftDist = driftDist;
    m_stripDriftErrors = driftDistErrors;
  }

  void MMPrepData::setAuthor(MMPrepData::Author author){
    m_author = author;
  }

  //assignment operator
  MMPrepData&
  MMPrepData::operator=(const MMPrepData& RIO)
  {
    if (&RIO !=this)
      {
	MuonCluster::operator=(RIO);
	m_detEl =  RIO.m_detEl ;
	m_time = RIO.m_time;
	m_charge = RIO.m_charge;
	m_driftDist = RIO.m_driftDist;
	m_angle = RIO.m_angle;
	m_chisqProb = RIO.m_chisqProb;
	m_stripNumbers = RIO.m_stripNumbers;
	m_stripTimes = RIO.m_stripTimes;
	m_stripCharges = RIO.m_stripCharges;
	m_stripDriftDist = RIO.m_stripDriftDist;
	m_stripDriftErrors = RIO.m_stripDriftErrors;
      }
    return *this;

  }

  MMPrepData&
  MMPrepData::operator=(MMPrepData&& RIO)
  {
    if (&RIO !=this)
      {
	MuonCluster::operator=(std::move(RIO));
	m_detEl =  RIO.m_detEl ;
	m_time =  RIO.m_time ;
	m_charge =  RIO.m_charge ;
	m_driftDist = RIO.m_driftDist;
	m_angle = RIO.m_angle;
	m_chisqProb = RIO.m_chisqProb;
	m_stripNumbers = RIO.m_stripNumbers;
	m_stripTimes = RIO.m_stripTimes;
	m_stripCharges = RIO.m_stripCharges;
	m_stripDriftDist = RIO.m_stripDriftDist;
	m_stripDriftErrors = RIO.m_stripDriftErrors;
      }
    return *this;

  }

  MsgStream&
  MMPrepData::dump( MsgStream&    stream) const
  {
    stream << MSG::INFO<<"MMPrepData {"<<std::endl;
    
    MuonCluster::dump(stream);
 
    stream<<"}"<<endmsg;

    return stream;
  }

  std::ostream&
  MMPrepData::dump( std::ostream&    stream) const
  {
    stream << "MMPrepData {"<<std::endl;
    
    MuonCluster::dump(stream);

    stream<<"}"<<std::endl;

    return stream;
  }
  //end of classdef
}//end of ns

