/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "TGCcabling/TGCDatabasePPToSL.h"

namespace LVL1TGCCabling8 
{

void TGCDatabasePPToSL::readDB (void) {
  std::ifstream file(m_filename.c_str());
  std::string buf;

  unsigned int space = m_blockname.find(" ");
  std::string module = m_blockname.substr(0,space);
  std::string region = m_blockname.substr(space+1,1);
  std::string type = m_blockname.substr(space+2);

  int offset=-1;
  if(type == "WT") offset = 1;
  if(type == "WD") offset = 2;
  if(type == "ST") offset = 3;
  if(type == "SD") offset = 4;
  if(type == "WI") offset = 5;
  if(type == "SI") offset = 6;
  if(type == "W")  offset = 1;
  if(type == "S")  offset = 2;

  while(getline(file,buf)){
    if(buf.substr(0,1)==region) break;
  }

  while(getline(file,buf)){
    if(buf.substr(1,module.size())==module) break;
  }

  while(getline(file,buf)){
    if(buf.substr(0,2)!="  ") break;
    if(buf.substr(2,1)=="0") offset--;
    if(offset==0){
      std::istringstream line(buf);
      std::vector<int> entry;
      for(int i=0; i<3; i++){
        int temp=-1;
        line >> temp; 
        entry.push_back(temp);
      }
      m_database.push_back(entry);
    }
  }
  
  file.close();
}

} // end of namespace
