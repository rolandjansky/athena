/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   TrigMemMoni.cxx
 * @brief  Memory monitoring tool
 * @author J. Philipp Grohs, johannes_philipp.grohs@mailbox.tu-dresden.de
 *
 */

#include "ITrigMemAuditor.h"
#include "TrigMemMoni.h"
#include "TrigSteering/TrigSteer.h"
#include "TrigInterfaces/TECreateAlgo.h"

#include "GaudiKernel/Algorithm.h"

#include "TProfile.h"
#include "TH1.h"

#include <vector>

using namespace std;

bool compare_nocase(string first, string second)
{
   unsigned int i = 0;

   while((i < first.length()) && (i < second.length()))
   {
      if(tolower(first[i]) < tolower(second[i])) return true;
      else if(tolower(first[i]) > tolower(second[i])) return false;
      ++i;
   }

   if(first.length() < second.length())
      return true;
   else return false;
}

bool vmem_sort_delta(const pair<string,TrigMemMoniInfo>& lhs,
		     const pair<string,TrigMemMoniInfo>& rhs)
{
  return (abs(lhs.second.virtdelta) > abs(rhs.second.virtdelta));
}

	       
TrigMemMoni::TrigMemMoni(const std::string& type,
                         const std::string& name,
                         const IInterface* parent) :
   TrigMonitorToolBase(type, name, parent),
   m_AbsMemCurBin(0),
   m_AbsMemSize(0),
   m_AbsSysTime(0),   
   m_event(0),
   m_pRelMemHist(0),
   m_pAbsMemHist(0),   
   m_pSteering(0)
{
   declareProperty("RelativeMemory",     m_MemRelMem  = true,  "Create relative memory histogram");
   declareProperty("AbsoluteMemory",     m_MemAbsMem  = true,  "Create absolute memory histogram");
   declareProperty("AbsoluteMemoryLag",  m_AbsMemLag  = 0,     "Refresh lag in seconds");
   declareProperty("AbsoluteMemoryBins", m_AbsMemBins = 1000,  "Number of bins in absolute memory histogram");
}

TrigMemMoni::~TrigMemMoni()
{

}

StatusCode TrigMemMoni::initialize()
{
   IAuditorSvc*                  pAuditSvc      = 0;
   IAuditor*                     pAuditor       = 0;
   ITrigMemAuditor*              pTrigMemAudit  = 0;

   list<string>::iterator        AlgNameIter;
   list<string>                  AlgNameList;
  
   unsigned int                  nCounter = 0;
  
   // get pointer to steering (should be our parent)
   m_pSteering = dynamic_cast<const HLT::TrigSteer*>(parent());
  
   if(m_pSteering == 0)
   {
      msg() << MSG::ERROR << "Parent algorithm is not of type TrigSteer. Cannot create histograms." << endmsg;
      return StatusCode::FAILURE;
   }
    
   // get list of running algorithms
   for(auto AlgIter = m_pSteering->subAlgorithms()->begin();
      AlgIter != m_pSteering->subAlgorithms()->end(); ++AlgIter)
   {
      if(dynamic_cast<const HLT::TECreateAlgo*>(*AlgIter)) AlgNameList.push_back((*AlgIter)->name());
   }
  
   // sort list of algorithms alphabetically
   AlgNameList.sort(compare_nocase);

   ATH_MSG_DEBUG("Following algorithms were found:");

   // fill algorithm map
   for(AlgNameIter = AlgNameList.begin();
      AlgNameIter != AlgNameList.end(); ++AlgNameIter)
   {
      m_AlgMap[*AlgNameIter] = nCounter;
      nCounter++;
 
      ATH_MSG_DEBUG(*AlgNameIter);
   }

   if (m_AlgMap.empty()) {
     msg() << MSG::INFO << "No algorithms to monitor." << endmsg;
     m_MemRelMem = false;
   }

   // register at memory auditor
   if(!(pAuditSvc = m_pSteering->auditorSvc()) ||
      !(pAuditor = pAuditSvc->getAuditor("TrigMemAuditor")) ||
      !(pTrigMemAudit = dynamic_cast<ITrigMemAuditor*>(pAuditor)))
   {
      msg() << MSG::ERROR << "Cannot find memory auditor." << endmsg;
      return StatusCode::FAILURE;
   }
   
   else pTrigMemAudit->regclient(static_cast<ITrigMemMoni*>(this));

   return StatusCode::SUCCESS;
}

StatusCode TrigMemMoni::bookHists()
{
   size_t DotPos = 0;
   string HistPath(name());
   m_event = 0;
   
   if((DotPos = HistPath.find('.')) != string::npos)
      HistPath.replace(DotPos, 1, 1, '/');

   TrigMonGroup MonGroup(this, HistPath, TrigMonitorToolBase::expert);
  
   if(m_MemRelMem)
   {
      map<string, int>::const_iterator AlgMapIter;

      // book histogram for memory differences
      if(!(m_pRelMemHist = new TProfile))
         return StatusCode::FAILURE;

      m_pRelMemHist->SetNameTitle("DeltaVMemMonitor", "Delta VMem per algorithm");
      m_pRelMemHist->SetBins(m_AlgMap.size(), 0, m_AlgMap.size());
      m_pRelMemHist->SetYTitle("Delta VMem [kB]");

      // name bins
      for(AlgMapIter = m_AlgMap.begin();
         AlgMapIter != m_AlgMap.end(); ++AlgMapIter)
      {
         m_pRelMemHist->GetXaxis()->SetBinLabel((*AlgMapIter).second + 1, (*AlgMapIter).first.c_str());
      }

      // register histogram
      if(MonGroup.regHist(m_pRelMemHist).isFailure())
      {  
         msg() << MSG::WARNING << "Cannot register histogram " << m_pRelMemHist->GetName() << endmsg;
         delete m_pRelMemHist;
         m_MemRelMem = false; // do not fill histogram
      }
   }

   if(m_MemAbsMem)
   {
      // book histogram for absolute memory usage
      if(!(m_pAbsMemHist = new TProfile))
         return StatusCode::FAILURE;

      m_pAbsMemHist->SetNameTitle("AboluteMemoryMonitor", "Abolute Memory Monitor");
      m_pAbsMemHist->SetBins(m_AbsMemBins, 0, m_AbsMemBins * (m_AbsMemLag ? m_AbsMemLag : 1));
      m_pAbsMemHist->SetYTitle("Absolute Memory usage [kB]");
      m_pAbsMemHist->SetXTitle(m_AbsMemLag ? "Time [~Seconds]" : "Time [Events]");

      // register histogram
      if(MonGroup.regHist(m_pAbsMemHist).isFailure())
      {  
         msg() << MSG::WARNING << "Cannot register histogram " << m_pAbsMemHist->GetName() << endmsg;
         delete m_pAbsMemHist;
         m_MemAbsMem = false; // do not fill histogram
      }

      // start timer
      if(m_AbsMemLag != 0 && m_MemAbsMem)
         m_AbsSysTime = System::currentTime(System::Sec);
   }

   return StatusCode::SUCCESS;
}

StatusCode TrigMemMoni::fillHists()
{
   m_event++;

   // Print memory consumption
   if(msgLvl(MSG::VERBOSE)) {
     if (m_MemAbsMem) {
       msg() << MSG::VERBOSE << "Event #" << m_event << ": VMEM = " << m_AbsMemSize << " kB";
     }
     // Print top N algorithms
     if (m_MemRelMem) {
       const unsigned int N(3);       
       vector< pair<string,TrigMemMoniInfo> > v(m_MemMap.begin(), m_MemMap.end());
       if (v.size()>N) {
	 partial_sort(v.begin(), v.begin()+N, v.end(), vmem_sort_delta);
       }
       vector< pair<string,TrigMemMoniInfo> >::iterator it = v.begin();
       vector< pair<string,TrigMemMoniInfo> >::iterator end = (v.size()>N ? (v.begin()+N) : v.end());
       for (;it != end; ++it) {
	 if (it->second.virtdelta!=0)
	   msg() << MSG::VERBOSE << " [" << it->first << ": " << it->second.virtdelta << " kB]";
       }
     }
     msg() << endmsg;
   }
      

      
   if(m_MemRelMem)
   {
      multimap<string,TrigMemMoniInfo>::iterator MemMapIter;
   
      for(MemMapIter = m_MemMap.begin(); MemMapIter != m_MemMap.end(); ++MemMapIter)
      {
         m_pRelMemHist->Fill((double)m_AlgMap[(*MemMapIter).first], (double)(*MemMapIter).second.virtdelta);
      }

      m_MemMap.clear();
   }

   if(m_MemAbsMem)
   {     
      if(m_AbsMemLag == 0 || (System::currentTime(System::Sec) - m_AbsSysTime) >= m_AbsMemLag)
      {
         // update memory map
         int CurBinTmp = m_AbsMemCurBin;

         m_AbsMemBinMap[CurBinTmp] = m_AbsMemSize;

         for(int i = 0; i != 4; i++)
         {
            CurBinTmp == m_AbsMemBins ? CurBinTmp = 1 : CurBinTmp++;
            m_AbsMemBinMap[CurBinTmp] = 0;
         }

         m_AbsMemCurBin == m_AbsMemBins ? m_AbsMemCurBin = 1 : m_AbsMemCurBin++;

         // refill histogram
         m_pAbsMemHist->Reset("M");

         for(std::map<int, int>::iterator MemMapIter = m_AbsMemBinMap.begin();
            MemMapIter != m_AbsMemBinMap.end(); ++MemMapIter)
         {
            m_pAbsMemHist->Fill(m_pAbsMemHist->GetBinCenter((*MemMapIter).first),
               (double)(*MemMapIter).second/1024);
         }

         if(m_AbsMemLag != 0)
            m_AbsSysTime = System::currentTime(System::Sec);
      }
   }

   return StatusCode::SUCCESS;
}

void TrigMemMoni::regmeminfo(const string& algoname, TrigMemMoniInfo& meminfo)
{
   if(m_MemRelMem)
   {
      if(m_AlgMap.find(algoname) != m_AlgMap.end())
         m_MemMap.insert(pair<string,TrigMemMoniInfo>(algoname, meminfo));
   }

   if(m_MemAbsMem)
   {
      m_AbsMemSize = meminfo.virtsize;
   }
}

StatusCode TrigMemMoni::queryInterface( const InterfaceID& riid, void** ppvIf )
{
   if(riid == ITrigMemMoni::interfaceID())
   {
      *ppvIf = static_cast<ITrigMemMoni*>(this);
      addRef();
      return StatusCode::SUCCESS;
   }

   return TrigMonitorToolBase::queryInterface(riid, ppvIf);
}
