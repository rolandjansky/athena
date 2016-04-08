/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOL_MEMORYREPORT_H
#define POOL_MEMORYREPORT_H


#include <string>


/** class to print out process size and resident size
  clearly not elegent, but it works!
  you supply a string so you can see in the output where you called it from
*/
class MemoryReport
{
public:
  ///
  MemoryReport(const std::string & s="");

  ///
  ~MemoryReport(){}

  //private:

  // this is wrong as it is os specific not compiler specific...
  //#ifdef __SUNPRO_CC
  //string command(const string & pid);
  //#endif


}; 

#endif






