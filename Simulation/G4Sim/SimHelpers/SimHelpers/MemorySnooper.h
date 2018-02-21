/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MemorySnooper_H
#define MemorySnooper_H

#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>

class MemorySnooper
{
public:
  MemorySnooper(std::string where)
  {
    int pid=getpid();
    std::ostringstream is;
    is<<pid;
    std::string spid=is.str();
    std::string filename="mem"+spid+".txt";
    std::string temp="cat /proc/"+spid+"/status | grep VmSize >"+filename;
    system(temp.c_str());
    std::ifstream in(filename.c_str());
    std::string text,mem,text1;
        in>>text>>mem>>text1;
    temp="rm -f "+filename;
    system(temp.c_str());
    float memsize=atof(mem.c_str());
    m_imem=int(memsize);
    std::cout << "*  Memory snooper called at " << where
              << " with VMEM: " << m_imem << "kB" << std::endl;
    //std::cout<<"*****************************************************"<<std::endl
    //         <<"*                                                   *"<<std::endl
    //         <<"  Memory snooper called here "<<where<<std::endl
    //         <<"  process size at this point (VmMem): "<<m_imem<<" kBytes"<<std::endl
    //         <<"*                                                   *"<<std::endl
    //         <<"*****************************************************"<<std::endl;
  }
  int GetMemorySize() const {return m_imem;}
private:
  int m_imem;
};

#endif
