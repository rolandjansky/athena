/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CoWTools/Monitor.h"
#include "CoWTools/StrStream.h"
#include "CoWTools/MStrStream.h"
#include "CoWTools/VecStream.h"
#include <sstream>
#include <fstream>

namespace CoWTools{
  Monitor::Monitor(std::string &out){
    m_out=new CoWTools::StrStream(out);
    m_stats=parseSmaps();
  }
  
  Monitor::Monitor(MsgStream &out){
    m_out=new CoWTools::MStrStream(out);
    m_stats=parseSmaps();
  }
  
  Monitor::Monitor(std::vector<long> &out){
    m_out=new CoWTools::VecStream(out);
    m_stats=parseSmaps();
  }
  Monitor::~Monitor(){
    m_stats=parseSmaps()-m_stats;
    m_out->putResults(m_stats);
    delete m_out;
  }

  const CoWTools::CoWRecordStats Monitor::parseSmaps() const{
    std::ifstream pfile("/proc/self/smaps");
    CoWTools::CoWRecordStats total,curr;
    if(pfile.is_open()){
      std::string line;
      std::stringstream ss;
      std::string libLine;
      while(!std::getline(pfile,line).eof()){
	if(line.empty())continue;
	if(line.size()>30 && line.at(0)!='V'){
	  ss.seekg(0);
	  curr.parseRecord(ss);
	  total+=curr;
	  ss.str("");
	  ss.clear();
	  continue;
	}
	ss<<line<<std::endl;
	ss.clear();
      }
      if(ss.str().size()>200){
	curr.parseRecord(ss);
	total+=curr;
      }
      pfile.close();
    }
    return total;
  }
}
