/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibUtils/RtData_t_r_reso.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <stdio.h>

#define M_MAX_PARS 1000

namespace MuonCalib {

  std::istream& RtData_t_r_reso::read( std::istream& is )
  {
    std::string dummy;

    double t(0),r(0),reso(0);
  
    // read region id and number of points in rt
    is >> dummy >> m_regionId >> m_npars;
    m_npars = (m_npars>M_MAX_PARS) ? M_MAX_PARS: m_npars;
    // reseve space in vectors
    m_timeVec.reserve(m_npars);
    m_radiusVec.reserve(m_npars);
    m_resoVec.reserve(m_npars);

    // read data from file 
    for( unsigned int i=0;i<m_npars;++i){
      // check if eof reached before last entry was read
      if( is.eof() ){
        MsgStream log(Athena::getMessageSvc(),"RtData_t_r_reso");
        log<<MSG::WARNING<<"read() <unexpected eof> -> exit after " << i-1 << " entries out of " << m_npars << " were read from file" <<endmsg;
        reset();
        return is;
      }

      // read next entry
      is >> r >> t >> reso;
      m_timeVec.push_back(t); 
      m_radiusVec.push_back(r);
      m_resoVec.push_back(reso);
    }

    // reading success, data valid
    m_isValid = true;

    return is;
  }

  std::ostream& RtData_t_r_reso::write( std::ostream& os ) const
  {
    if( m_isValid ){
      os << " dummy "<<m_regionId << " " << m_npars << std::endl;
      for( unsigned int i=0;i<m_npars;++i){
	os << "  " << m_radiusVec[i] << " " << m_timeVec[i] << " " << m_resoVec[i] << std::endl;
      }
    }else{
      MsgStream log(Athena::getMessageSvc(),"RtData_t_r_reso");
      log<<MSG::WARNING<<"write() <data not valid>" <<endmsg;
    }
  
    return os;
  }

  void RtData_t_r_reso::write_forDB( FILE *frtt, FILE *frtr, FILE *frts ) const
  {
 
    if( m_isValid ){
      fprintf(frtt,"%d",m_npars);
      fprintf(frtr,"%d",m_npars);
      fprintf(frts,"%d",m_npars);

      // loop over data and write to std::ostream
      for( unsigned int i=0;i<m_npars;++i){
      fprintf(frtt,",%f",m_timeVec[i]);
      fprintf(frtr,",%f",m_radiusVec[i]);
      fprintf(frts,",%f",m_resoVec[i]);
      }
      fprintf(frtt,"\n");
      fprintf(frtr,"\n");
      fprintf(frts,"\n");
    }else{
      MsgStream log(Athena::getMessageSvc(),"RtData_t_r_reso");
      log<<MSG::WARNING<<"write_forDB() <data not valid>" <<endmsg;
    }
  
    return;
  }


  void RtData_t_r_reso::reset()
  {
    m_isValid = false;
    m_timeVec.clear();
    m_radiusVec.clear();
    m_resoVec.clear();
  }

  std::istream& operator>> ( std::istream& is, RtData_t_r_reso& data )
  {
    return data.read(is);
  }

  std::ostream& operator<< ( std::ostream& os, const RtData_t_r_reso& data )
  {
    return data.write( os );
  }


}
