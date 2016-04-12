/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CoWTools/CoWRecordStats.h"
#include <cstring>
#include <cstdio>

const std::map<std::string,int> CoWTools::CoWRecordStats::s_vmFlags={
  {"rd",0x00000001},
  {"wr",0x00000002},
  {"ex",0x00000004},
  {"sh",0x00000008},
  {"mr",0x00000010},
  {"mw",0x00000020},
  {"me",0x00000040},
  {"ms",0x00000060},
  {"gd",0x00000080},
  {"pf",0x00000100},
  {"dw",0x00000200},
  {"lo",0x00000400},
  {"io",0x00000800},
  {"sr",0x00001000},
  {"rr",0x00002000},
  {"dc",0x00004000},
  {"de",0x00008000},
  {"ac",0x00010000},
  {"nr",0x00020000},
  {"ht",0x00040000},
  {"nl",0x00080000},
  {"ar",0x00100000},
  {"dd",0x00200000},
  {"sd",0x00400000},
  {"mm",0x00800000},
  {"hg",0x01000000},
  {"nh",0x02000000},
  {"mg",0x04000000}
};

void CoWTools::CoWRecordStats::parseRecord(std::istream &in){
  std::string line;
  int pos=0;
  //std::cout<<"in MS"<<std::endl;
  while(!(std::getline(in,line).eof())){
    //std::cout<<"MS "<<line<<std::endl;
    int len=0;
    unsigned long val=0;
    char buff[20];
    sscanf(line.c_str(),"%19s%n",buff,&len);
    if(strcmp(buff,"VmFlags:")==0){
      len++;
      while(len<(int)line.size()){
	//std::cerr<<"substr="<<line.substr(len,2)<<std::endl;
	val|=CoWTools::CoWRecordStats::s_vmFlags.at(line.substr(len,2));
	len+=3;
      }
    }else{
      sscanf(line.c_str()+len,"%8lu",&val);
    }
    m_vals[pos]=val;
    pos++;
  }
}

namespace CoWTools{

  std::ostream & operator<<(std::ostream &out, const CoWRecordStats &m){
    if(m.m_summary){
      char buff[2048];
      snprintf(buff,2048,"VMem= %8lld, RSS= %8lld, PSS= %8lld, Shared= %8lld, Private= %8lld, Swap= %8lld Anon= %8lld",
	       m.m_vals[0],m.m_vals[1],m.m_vals[2],m.m_vals[3]+m.m_vals[4],m.m_vals[5]+m.m_vals[6],m.m_vals[10],m.m_vals[8]);
      out<<buff;
    }else{
      char buff[4000];
      snprintf(buff,4000,
	       "Size:           %8lld kB\n" \
	       "Rss:            %8lld kB\n" \
	       "Pss:            %8lld kB\n" \
	       "Shared_Clean:   %8lld kB\n" \
	       "Shared_Dirty:   %8lld kB\n" \
	       "Private_Clean:  %8lld kB\n" \
	       "Private_Dirty:  %8lld kB\n" \
	       "Referenced:     %8lld kB\n" \
	       "Anonymous:      %8lld kB\n" \
	       "AnonHugePages:  %8lld kB\n" \
	       "Swap:           %8lld kB\n" \
	       "KernelPageSize: %8lld kB\n" \
	       "MMUPageSize:    %8lld kB\n" \
	       "Locked:         %8lld kB\n", 
	       m.m_vals[0],
	       m.m_vals[1],
	       m.m_vals[2],
	       m.m_vals[3],
	       m.m_vals[4],
	       m.m_vals[5],
	       m.m_vals[6],
	       m.m_vals[7],
	       m.m_vals[8],
	       m.m_vals[9],
	       m.m_vals[10],
	       m.m_vals[11],
	       m.m_vals[12],
	       m.m_vals[13]);
      out<<buff;
    }
    return out;
  }
}

