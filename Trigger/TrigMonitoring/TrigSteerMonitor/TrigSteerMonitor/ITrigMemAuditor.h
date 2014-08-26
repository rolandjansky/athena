/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTEERMONITOR_ITRIGMEMAUDITOR_H
#define TRIGSTEERMONITOR_ITRIGMEMAUDITOR_H

/**
 * @file   ITrigMemAuditor.h
 * @brief  memory monitoring auditor interface
 * @author J. Philipp Grohs, johannes_philipp.grohs@mailbox.tu-dresden.de
 */
 
#include "ITrigMemMoni.h"
 

class ITrigMemAuditor
{
public:

   static const InterfaceID& interfaceID() {
     static const InterfaceID IID_ITrigMemAuditor("ITrigMemAuditor", 1 , 0); 
     return IID_ITrigMemAuditor;
   }   
   
   virtual ~ITrigMemAuditor() {}
   
   virtual void   regclient(ITrigMemMoni* pMemMoni) = 0;
};

#endif // TRIGSTEERMONITOR_ITRIGMEMAUDITOR_H
