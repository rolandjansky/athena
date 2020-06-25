/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_RTDATAFROMFILE_H
#define MUONCALIB_RTDATAFROMFILE_H

#include "MdtCalibUtils/RtData_t_r_reso.h"
#include "MdtCalibData/RtFullInfo.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <vector>
#include <iostream>

namespace MuonCalib {

  /**
   Manages the I/O of the Rt realtions from/to file. 
  */
  class RtDataFromFile {
  public:
    typedef RtData_t_r_reso                     RtRelation;
    typedef std::vector<RtRelation*>            RtRelations;
  public:
    RtDataFromFile() : m_rts(0), m_major_version(1), m_minor_version(0)  {}
    ~RtDataFromFile()=default;
    
    /** return number of rt relations */
    unsigned int nRts() const { return m_rts; }

    /** retrieve rt-relation for a give regionId */
    RtRelation* getRt( unsigned int regionId ) const { 
      if( regionId >= (unsigned int)m_rts ){
        MsgStream log(Athena::getMessageSvc(),"RtDataFromFile");
        log<<MSG::WARNING<<"getRt() <regionId out of range> " << regionId << " size " << m_rts <<endmsg;
        return 0;
      }
      return m_rtRelations[regionId];
    }

    /** set total number of regions */
    void setNRts( unsigned int nrts ) { m_rts = nrts; m_rtRelations.resize(nrts); m_fullInfo.resize(nrts);}

    /** RtDataFromFile takes ownership of rt */
    bool addRt( int regionId, RtRelation* rt ){
      if( regionId < 0 || regionId >= (int)m_rts ){
        MsgStream log(Athena::getMessageSvc(),"RtDataFromFile");
        log<<MSG::WARNING<<"addRt() <regionId out of range> " << regionId << " size " << m_rts <<endmsg;
        return false;
      }
      if( m_rtRelations[regionId] != 0 ){
        MsgStream log(Athena::getMessageSvc(),"RtDataFromFile");
        log<<MSG::WARNING<<"addRt() <rt already set>" << endmsg;
        return false;
      }

      m_rtRelations[regionId] = rt;

      return true;
    }
    bool addRt( int regionId, RtRelation* rt , const RtFullInfo* info){ 	 
      bool result=addRt(regionId,rt); 	 
      if( result ){ 	 
	m_fullInfo[regionId] = info; 	 
      } 	 
      return result; 	 
    }
    std::istream& read( std::istream& is );
    std::ostream& write( std::ostream& os , int region) const;
    std::ostream& write( std::ostream& os ) const;
    void write_forDB( FILE *frt, FILE *frtt, FILE *frtr, FILE *frts , int region) const;
    void write_forDB( FILE *frt, FILE *frtt, FILE *frtr, FILE *frts) const;
    inline void setVersion(int major, int minor)
    	{
	m_major_version=major;
	m_minor_version=minor;
	}
  private:
    /** total number of regions */
    unsigned int m_rts;
    
    /** rt relations */
    RtRelations m_rtRelations; 
    std::vector<const RtFullInfo*> m_fullInfo;
    /** format version */
    int m_major_version, m_minor_version;
  };

}

std::istream& operator>> (std::istream& is, MuonCalib::RtDataFromFile& data);

std::ostream& operator<< (std::ostream& os, const MuonCalib::RtDataFromFile& data);


#endif
