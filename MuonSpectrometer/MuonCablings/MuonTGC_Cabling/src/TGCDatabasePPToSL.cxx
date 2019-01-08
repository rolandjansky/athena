/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCDatabasePPToSL.h"

#include <fstream>
#include <sstream>

namespace MuonTGC_Cabling 
{

TGCDatabasePPToSL::TGCDatabasePPToSL(std::string filename, std::string blockname)
    : TGCDatabase(TGCDatabase::PPToSL, filename, blockname)
{
  // read out ascii file and fill database
  if(m_database.size()==0) readDB();
}

TGCDatabasePPToSL::TGCDatabasePPToSL(const TGCDatabasePPToSL& right)
  : TGCDatabase(right)
{
}
 

void TGCDatabasePPToSL::readDB(void) {
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

TGCDatabasePPToSL::~TGCDatabasePPToSL(void)
{
}

bool TGCDatabasePPToSL::update(const std::vector<int>& input)
{
  int ip = find(input);
  if(ip<0) return false;
  
  const unsigned int input_size = input.size();

  const unsigned int database_size = m_database[ip].size();
  for(unsigned int i=1; i<database_size; i++){
    if(i<input_size){
      m_database[ip].at(i) = input.at(i);
    } else {
      m_database[ip].at(i) = -1;
    }
  }
   
  return true;
}
 
int  TGCDatabasePPToSL::find(const std::vector<int>& channel) const
{
  int index=-1;
  const unsigned int size = m_database.size();
  for(unsigned int i=0; i<size; i++){
    if(m_database[i].at(0) == channel.at(0)){
      index = i;
      break;
    }
  }
  return index;
}

} // end of namespace
