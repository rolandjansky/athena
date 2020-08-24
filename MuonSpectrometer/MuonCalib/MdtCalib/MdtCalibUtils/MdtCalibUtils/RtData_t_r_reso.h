/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_RTDATA_T_R_RESO_H
#define MUONCALIB_RTDATA_T_R_RESO_H

#include <iostream>
#include <vector>
#include <iostream>

namespace MuonCalib {

/**
 Represents an Rt relation in the form of a set of (time,radius,resolution) points.
*/
class RtData_t_r_reso {
 public:
  typedef std::vector<double> DataVec;
 public:
  RtData_t_r_reso() : m_isValid(false), m_npars(0), m_regionId(0) {}

  bool isValid() const { return m_isValid; }
  void isValid(bool f) { m_isValid = f; }

  unsigned int regionId() const { return m_regionId; }

  const DataVec& times() const { return m_timeVec; }
  const DataVec& radii() const { return m_radiusVec; }
  const DataVec& resolution() const { return m_resoVec; }

  void setRegionId( int id ) { m_regionId = id; }

  /** returns falls if the current time is smaller then the previous one */
  bool addEntry( double t, double r , double reso ){
    m_timeVec.push_back(t);
    m_radiusVec.push_back(r);
    m_resoVec.push_back(reso);
    m_npars = m_timeVec.size();
    return true;
  }

  void reset();
  std::istream& read( std::istream& is );
  std::ostream& write( std::ostream& os ) const;
  void write_forDB( FILE *frtt, FILE *frtr, FILE *frts ) const;
  private:
  
  bool         m_isValid;
     
  unsigned int m_npars;
  int          m_regionId;
  
  DataVec      m_timeVec;
  DataVec      m_radiusVec;
  DataVec      m_resoVec;
 };

 std::istream& operator>> ( std::istream& is, RtData_t_r_reso& data );
 std::ostream& operator<< ( std::ostream& os, const RtData_t_r_reso& data );

}

#endif
