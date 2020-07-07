/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibUtils/RtDataFromFile.h"
#include "MuonCalibStl/DeleteObject.h"

#include <TString.h> // for Form
#include <string>

#define M_MAX_RTS 100000

namespace MuonCalib {
  
  std::istream& RtDataFromFile::read( std::istream& is )
  {
    std::string version;
    std::string rts_str;

    // read number of rts in file
    is >> version >> rts_str;

    unsigned long int pos = 0;
    std::string::size_type start = rts_str.find_first_not_of(" ",pos); 
    if(start == std::string::npos) throw std::runtime_error(Form("File: %s, Line: %d\nRtDataFromFile::read() - problems extracting m_rts, exiting", __FILE__, __LINE__));

    std::string::size_type stop = rts_str.find_first_of(" ",start+1); 
    if (stop == std::string::npos) stop = rts_str.size(); 
    m_rts = std::stoi(rts_str.substr(start,stop-start),nullptr); 

    m_rts = (m_rts>M_MAX_RTS) ? M_MAX_RTS : m_rts;
    for( unsigned int i=0;i<m_rts;++i){ 
      if(version != "v0.0") { 	 
	// read in additional information 	 
	RtFullInfo fi; 	 
	is >> fi; 	 
      }
      // create empty rt
      RtRelation* rt = new RtRelation();
      
      // read rt from file
      is >> *rt;

      // store rt
      m_rtRelations.push_back(rt);
    }

    return is;
  }
    
  std::ostream& RtDataFromFile::write( std::ostream& os , int region) const 	 
  { 	 
    if( m_rts != m_rtRelations.size() ){
      MsgStream log(Athena::getMessageSvc(),"RtDataFromFile");
      log<<MSG::WARNING<<"write() <inconsistent rt count>"<<endmsg;
    } 	 
    if( region >= static_cast<int>(m_rts) ) {
      MsgStream log(Athena::getMessageSvc(),"RtDataFromFile");
      log<<MSG::WARNING<<"write() <requested not existent region>"<<endmsg;
    } 	 
    os << "v" << m_major_version << "." << m_minor_version << " 1" ;// full info added 	 
    
    if( m_fullInfo[region] ) os << *(m_fullInfo[region]); 	 
    if( m_rtRelations[region] ) os << *(m_rtRelations[region]); 	 
    return os; 	 
  }
  
  std::ostream& RtDataFromFile::write( std::ostream& os ) const
  {
    if( m_rts != m_rtRelations.size() ){
      MsgStream log(Athena::getMessageSvc(),"RtDataFromFile");
      log<<MSG::WARNING<<"write() <inconsistent rt count>"<<endmsg;
    }
    	 
    os << "v1.0 " << m_rts ;// full info added
    
    for( unsigned int i=0;i<m_rtRelations.size();++i ){  
      if( m_fullInfo[i] ) os << *(m_fullInfo[i]);
      if( m_rtRelations[i] ) os << *(m_rtRelations[i]);
    }
    return os;
  }

  void RtDataFromFile::write_forDB( FILE *frt, FILE *frtt, FILE *frtr, FILE *frts , int region) const          
  {    
    if( m_rts != m_rtRelations.size() ){
      MsgStream log(Athena::getMessageSvc(),"RtDataFromFile");
      log<<MSG::WARNING<<"write_forDB() <inconsistent rt count>"<<endmsg;
    }          
    if( region >= static_cast<int>(m_rts) ) {
      MsgStream log(Athena::getMessageSvc(),"RtDataFromFile");
      log<<MSG::WARNING<<"write_forDB() <requested not existent region>"<<endmsg;
    }                  
    if( m_fullInfo[region] ){
      (m_fullInfo[region])->write_forDB(frt);
    }
    if( m_rtRelations[region] ){
      (m_rtRelations[region])->write_forDB(frtt,frtr,frts); 
    }
    return;    
  }
  
  void RtDataFromFile::write_forDB( FILE *frt, FILE *frtt, FILE *frtr, FILE *frts ) const
  {
    if( m_rts != m_rtRelations.size() ){
      MsgStream log(Athena::getMessageSvc(),"RtDataFromFile");
      log<<MSG::WARNING<<"write_forDB() <inconsistent rt count>"<<endmsg;
    }
    
    for( unsigned int i=0;i<m_rtRelations.size();++i ){  
      if( m_rtRelations[i] ){
      (m_rtRelations[i])->write_forDB(frtt,frtr,frts); 
      }
      if( m_fullInfo[i] ){
      (m_fullInfo[i])->write_forDB(frt);
      }
    }
    return;
  }


}

std::istream& operator>> (std::istream& is, MuonCalib::RtDataFromFile& data)
{
  return data.read(is);
}

std::ostream& operator<< (std::ostream& os, const MuonCalib::RtDataFromFile& data)
{
  return data.write(os);
}
