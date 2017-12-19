/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   TrigMemAuditor.cxx
 * @brief  Memory monitoring auditor
 * @author J. Philipp Grohs, johannes_philipp.grohs@mailbox.tu-dresden.de
 *
 */
 
#include "TrigMemAuditor.h"
#include "ITrigMemMoni.h"
#include "GaudiKernel/Memory.h"

TrigMemAuditor::TrigMemAuditor(const std::string &name, ISvcLocator *svcloc)
 : Auditor(name, svcloc)
{
}

StatusCode TrigMemAuditor::initialize()
{

  
  return StatusCode::SUCCESS;
}

void  TrigMemAuditor::beforeExecute(INamedInterface *a)
{
   if(a == NULL)
      return;
   
   m_VirtSizeMap[a->name()]   = System::virtualMemory();
   
}

void  TrigMemAuditor::afterExecute(INamedInterface *a, const StatusCode &sc)
{
  const int nClientCount = m_ClientVec.size();

  TrigMemMoniInfo      TrigMemInfo;

  if(a == NULL)
      return;

   // set up memory information structure
   TrigMemInfo.virtsize    = System::virtualMemory();
   TrigMemInfo.virtdelta   = TrigMemInfo.virtsize - m_VirtSizeMap[a->name()];
   
   // extract status code
   TrigMemInfo.statuscode  = sc.getCode();

   // inform tools about memory usage
   for(int i = 0; i != nClientCount; i++)
   {
      m_ClientVec[i]->regmeminfo(a->name(), TrigMemInfo);
   }     
   
}

void  TrigMemAuditor::regclient(ITrigMemMoni* pMemMoni)
{
   m_ClientVec.push_back(pMemMoni);
}

StatusCode TrigMemAuditor::queryInterface( const InterfaceID& riid, void** ppvIf )
{
   if(riid == ITrigMemAuditor::interfaceID())
   {
      *ppvIf = static_cast<ITrigMemAuditor*>(this);
      addRef();
      return StatusCode::SUCCESS;
   }

   return Auditor::queryInterface(riid, ppvIf);
}
