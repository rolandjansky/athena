/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "TGCcabling/TGCDatabaseSLBToROD.h"

namespace LVL1TGCCabling8 
{

void TGCDatabaseSLBToROD::readDB (void) {
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
    for(int i=0; i<4; i++){
      int temp=-1;
      line >> temp; 
      entry.push_back(temp);
    }
    m_database.emplace_back(std::move(entry));
  }
  
  file.close();
}

} // end of namespace
