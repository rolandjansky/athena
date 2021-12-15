/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCDatabaseSLBToROD.h"

#include <fstream>
#include <sstream>

namespace MuonTGC_Cabling 
{

TGCDatabaseSLBToROD::TGCDatabaseSLBToROD(const std::string& filename,
                                         const std::string& blockname) 
   : TGCDatabase(TGCDatabase::SLBToROD, filename, blockname)
{
  // read out ascii file and fill database
  TGCDatabaseSLBToROD::readDB();
}

TGCDatabaseSLBToROD::TGCDatabaseSLBToROD(const TGCDatabaseSLBToROD& right)
  : TGCDatabase(right)
{
}

void TGCDatabaseSLBToROD::readDB(void) {
  std::ifstream file(m_filename.c_str());
  std::string buf;

  unsigned int space = m_blockname.find(' ');
  std::string_view module = std::string_view(m_blockname).substr(0,space);
  std::string_view type = std::string_view(m_blockname).substr(space+1);

  while(getline(file,buf)){
    if(buf.compare(0,module.size(),module)==0) break;
  }

  while(getline(file,buf)){
    if(buf.compare(1,type.size(),type)==0) break;
  }

  while(getline(file,buf)){
    if(buf.compare(0,2,"  ")!=0) break;
    std::istringstream line(buf);
    std::vector<int> entry;
    for(int i=0; i<6; i++){
      int temp=-1;
      line >> temp; 
      entry.push_back(temp);
    }
    m_database.emplace_back(std::move(entry));
  }
  
  file.close();
}

TGCDatabaseSLBToROD::~TGCDatabaseSLBToROD(void)
{
}

bool TGCDatabaseSLBToROD::update(const std::vector<int>& input)
{
  int ip = find(input);
  if(ip<0) return false;

  if(input.size()<6) { 
    // SSW block
    m_database[ip].at(1) = input.at(1);
  } else {
    // SLB block
    m_database[ip].at(4) = input.at(4);
    m_database[ip].at(5) = input.at(5);
  }

  return true;
}
 
int  TGCDatabaseSLBToROD::find(const std::vector<int>& channel) const
{
  int index=-1;

  const unsigned int size = m_database.size();

  if(channel.size()<4) { 
    // SSW block
    for(unsigned int i=0; i<size; i++){
      if(m_database[i].at(0) == channel.at(0)){
	index = i;
	break;
      }
    }
  } else {
    // SLB block
    for(unsigned int i=0; i<size; i++){
      if(m_database[i].at(3) == channel.at(3) && 
	 m_database[i].at(2) == channel.at(2) &&
	 m_database[i].at(1) == channel.at(1) &&
	 m_database[i].at(0) == channel.at(0)){
	index = i;
	break;
      }
    }
  }

  return index;
}

} // end of namespace
