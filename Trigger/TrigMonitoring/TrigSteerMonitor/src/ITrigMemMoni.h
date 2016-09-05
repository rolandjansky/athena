/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTEERMONITOR_ITRIGMEMMONI_H
#define TRIGSTEERMONITOR_ITRIGMEMMONI_H

/**
 * @file   ITrigMemMoni.h
 * @brief  memory monitoring tool interface
 * @author J. Philipp Grohs, johannes_philipp.grohs@mailbox.tu-dresden.de
 */
 
 #include "GaudiKernel/IAlgTool.h"
 
 #include <string>
 

struct TrigMemMoniInfo
{
  TrigMemMoniInfo() : virtsize(0), virtdelta(0), statuscode(0) {}
  int   virtsize;   // current size of virtual memory
  int   virtdelta;  // difference of virtual memory size
  int   statuscode; // status code of the algorithm
};

class ITrigMemMoni
{  
public:

   ///  Retrieve interface ID 
   static const InterfaceID& interfaceID() {
     static const InterfaceID IID_ITrigMemMoni("ITrigMemMoni", 1 , 0);      
     return IID_ITrigMemMoni;
   } 

   virtual ~ITrigMemMoni() {}

   virtual void regmeminfo(const std::string& algoname, TrigMemMoniInfo& meminfo) = 0;
};

#endif
