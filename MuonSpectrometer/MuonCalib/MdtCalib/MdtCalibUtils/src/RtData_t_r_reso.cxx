/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibUtils/RtData_t_r_reso.h"
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
	std::cout << "RtData_t_r_reso::read ERROR <unexpected eof>" << std::endl;
	std::cout << " -> exit after " << i-1 << " entries out of " << m_npars 
		  << " were read from file" << std::endl;
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
      //os << " RegionId " << m_regionId << " " << m_npars << std::endl;
      os << " dummy "<<m_regionId << " " << m_npars << std::endl;
      // loop over data and write to std::ostream
      for( unsigned int i=0;i<m_npars;++i){
	os << "  " << m_radiusVec[i] << " " << m_timeVec[i] << " " << m_resoVec[i] << std::endl;
      }
    }else{
      std::cout << "RtData_t_r_reso::write ERROR <data not valid>" << std::endl;
    }
  
    return os;
  }

  void RtData_t_r_reso::write_forDB( FILE *frtt, FILE *frtr, FILE *frts ) const
  {
    //    int mdt_rt_id=98; // FAB:280208: to be set!!!
    //    int mdt_rt_map_t_id=97; // FAB:280208: to be set!!!
    //    int mdt_rt_map_r_id=96; // FAB:280208: to be set!!!
    //    int mdt_rt_map_s_id=95; // FAB:280208: to be set!!!
 
    if( m_isValid ){
      //      fprintf(frtt," %d,%d,%d,%d",mdt_rt_map_t_id,mdt_rt_id,mdt_rt_id,m_npars);
      //      fprintf(frtr," %d,%d,%d,%d",mdt_rt_map_r_id,mdt_rt_id,mdt_rt_map_t_id,m_npars);
      //      fprintf(frts," %d,%d,%d",mdt_rt_map_s_id,mdt_rt_id,m_npars);
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
      std::cout << "RtData_t_r_reso::write_forDB ERROR <data not valid>" << std::endl;
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
