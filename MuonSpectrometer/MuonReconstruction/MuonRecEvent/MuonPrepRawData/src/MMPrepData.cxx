/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
    m_stripDriftErrors(),
    m_author (SimpleClusterBuilder)
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
    m_stripCharges(),
    m_author (SimpleClusterBuilder)
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
    m_stripCharges(),
    m_author (SimpleClusterBuilder)
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
    m_stripDriftErrors(),
    m_author (SimpleClusterBuilder)
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
    m_stripDriftErrors(),
    m_author (SimpleClusterBuilder)
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
    m_stripDriftErrors(RIO.m_stripDriftErrors),
    m_author(RIO.m_author)
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


  void MMPrepData::setDriftDist(const std::vector<float>& driftDist, const std::vector<float>& stripDriftErrors_0_0, const std::vector<float>& stripDriftErrors_1_1){
    m_stripDriftDist = driftDist;
    
    if(stripDriftErrors_0_0.size() != stripDriftErrors_1_1.size()){
      //ATH_MSG_FATAL("trying to set MMPrepData uncertainties with unequal number of elements");
    }
    m_stripDriftErrors.clear();
    for(uint i_strip = 0; i_strip < stripDriftErrors_1_1.size(); i_strip++){
      Amg::MatrixX tmp(2,2);
      tmp(0,0) = stripDriftErrors_0_0.at(i_strip);
      tmp(1,1) = stripDriftErrors_1_1.at(i_strip);
      m_stripDriftErrors.push_back(tmp);
    }
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
	m_author = RIO.m_author;
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

  const std::vector<float> MMPrepData::stripDriftErrors_0_0 () const {
    std::vector<float> ret;
    for (const Amg::MatrixX& mat: m_stripDriftErrors) {
      ret.push_back(mat(0,0));
    }
    return ret;
  }
  
  const std::vector<float> MMPrepData::stripDriftErrors_1_1 () const {
    std::vector<float> ret;
    for (const Amg::MatrixX& mat: m_stripDriftErrors) {
      ret.push_back(mat(1,1));
    }
    return ret;
  }


  //end of classdef
}//end of ns

