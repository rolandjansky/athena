/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CoWTools/CoWRecord.h"
#include <cstdio>

void CoWTools::CoWRecord::parseRecord(std::istream &in){
  std::string line;
  std::getline(in,line);
  int len=0;
  char pbuff[5]={"----"};
  //int dmin,dmaj;
  sscanf(line.c_str(),"%08lx-%08lx %4s %08llx %02x:%02x %80lu %n",
	 &addrStart,
	 &addrEnd,
	 pbuff,
	 &offset,
	 &dmaj,
	 &dmin,
	 &inode,
	 &len);
  //std::cout<<"MR len= "<<len<<" lineLen="<<line.size()<<", "<<line<<std::endl;
  for(int i=0;i<3;i++){
    perms|=((pbuff[i]!='-')<<(4-i));
  }
  perms |= (pbuff[3]=='p');
  m_ms.parseRecord(in);
}
