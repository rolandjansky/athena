/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTEERMONITOR_TRIGMEMAUDITOR_H
#define TRIGSTEERMONITOR_TRIGMEMAUDITOR_H

/**
 * @file   TrigMemAuditor.h
 * @brief  memory monitoring auditor
 * @author J. Philipp Grohs, johannes_philipp.grohs@mailbox.tu-dresden.de
 */

#include "GaudiKernel/Auditor.h"
#include "ITrigMemAuditor.h"

#include <vector>

class TrigMemAuditor : public ITrigMemAuditor, virtual public Auditor
{
public:

   TrigMemAuditor(const std::string &name, ISvcLocator *svcloc);
   
   virtual StatusCode   initialize();
   virtual void      beforeExecute(INamedInterface *);
   virtual void      afterExecute(INamedInterface *, const StatusCode &);
   
   virtual void      regclient(ITrigMemMoni* pMemMoni);
   
   StatusCode queryInterface( const InterfaceID& riid, void** ppvIf );
   
private:

   std::vector<ITrigMemMoni*> m_ClientVec;
   std::map<std::string, int> m_VirtSizeMap;
};

#endif // TRIGSTEERMONITOR_TRIGMEMAUDITOR_H
