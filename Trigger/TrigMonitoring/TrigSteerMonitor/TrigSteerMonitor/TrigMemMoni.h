/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTEERMONITOR_TRIGMEMMONI_H
#define TRIGSTEERMONITOR_TRIGMEMMONI_H

/**
 * @file   TrigMemMoni.h
 * @brief  memory monitoring tool
 * @author J. Philipp Grohs, johannes_philipp.grohs@mailbox.tu-dresden.de
 */

// Trigger includes
#include "TrigMonitorBase/TrigMonitorToolBase.h"
#include "ITrigMemMoni.h"

#include <map>
#include <string>

class TProfile;

// Forward declararations
namespace HLT {
  class TrigSteer;
}

//class TProfile;

class TrigMemMoni : public TrigMonitorToolBase, public ITrigMemMoni
{  
public:

   TrigMemMoni(const std::string& type, const std::string& name, const IInterface* parent);

   virtual ~TrigMemMoni();

   virtual StatusCode initialize();
   virtual StatusCode bookHists();
   virtual StatusCode fillHists();
   virtual void       regmeminfo(const std::string& algoname, TrigMemMoniInfo& meminfo);
  
   // to allow access to the ITrigMemMoni interface
   StatusCode queryInterface( const InterfaceID& riid, void** ppvIf );

private:

   bool     m_MemRelMem;
   bool     m_MemAbsMem;
   int      m_AbsMemBins;
   int      m_AbsMemCurBin;
   int      m_AbsMemSize;
   longlong m_AbsMemLag;
   longlong m_AbsSysTime;
   int      m_event;

   // profile histogram to store data in
   TProfile*   m_pRelMemHist;
   TProfile*   m_pAbsMemHist;

   // Pointer to steering (partent algo of this tool)
   const HLT::TrigSteer* m_pSteering;
  
   // Map of running algorithms
   std::map<std::string, int>                   m_AlgMap;
   std::multimap<std::string, TrigMemMoniInfo>  m_MemMap;
   std::map<int, int>                           m_AbsMemBinMap;
};

#endif


