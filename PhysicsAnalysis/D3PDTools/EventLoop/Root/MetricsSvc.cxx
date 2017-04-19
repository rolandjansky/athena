/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <EventLoop/MetricsSvc.h>
#include <RootCoreUtils/Assert.h>
#include <TBenchmark.h>
#include <TFile.h>
#include <TTree.h>
#include <TTreeCache.h>

ClassImp(EL::MetricsSvc)


namespace EL
{
  const std::string MetricsSvc::name = "Metrics";
}

void EL::MetricsSvc :: testInvariant () const
{
  RCU_INVARIANT (this != 0);
}

const char *EL::MetricsSvc :: GetName() const
{
  RCU_READ_INVARIANT (this);
  return name.c_str();
}


EL::MetricsSvc :: MetricsSvc ()
  : m_fileMetrics (0), m_jobMetrics (0), m_filesRead (0)
  , m_eventsRead (0), m_benchmark (0)
{
  RCU_NEW_INVARIANT (this);
}



EL::MetricsSvc :: ~MetricsSvc ()
{
  RCU_DESTROY_INVARIANT (this);
  if (m_benchmark) { delete m_benchmark; }
}



EL::StatusCode EL::MetricsSvc :: histInitialize ()
{ 
  RCU_CHANGE_INVARIANT (this);
  m_benchmark = new TBenchmark;
  m_benchmark->Start("loopmetrics");
  wk()->addOutput (m_fileMetrics = new TTree ("EventLoop_Metrics/cacheStats", 
					      "TTree cache stats per file"));
  wk()->addOutput (m_jobMetrics = new TTree ("EventLoop_Metrics/jobs", 
					     "event throughput per job"));
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode EL::MetricsSvc :: fileExecute ()
{
  RCU_CHANGE_INVARIANT (this);
  m_filesRead ++;
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode EL::MetricsSvc :: endOfFile ()
{
  RCU_CHANGE_INVARIANT (this);
  RCU_ASSERT(m_fileMetrics); 
  RCU_ASSERT(wk()->inputFile()); 
  RCU_ASSERT(wk()->tree()); 
  TTreeCache *tc = (TTreeCache*)wk()->inputFile()->GetCacheRead(wk()->tree());
  if (tc) {	
    Int_t nBranches = tc->GetCachedBranches()->GetEntries();
    m_fileMetrics->Branch ("nBranches", &nBranches, "nBranches/I");
    Int_t nLearn = tc->GetLearnEntries();
    m_fileMetrics->Branch ("learnEntries", &nLearn, "learnEntries/I");
    Double_t eff = tc->GetEfficiency ();
    m_fileMetrics->Branch ("cacheEfficiency", &eff, "cacheEfficiency/D");
    Double_t effRel = tc->GetEfficiencyRel ();
    m_fileMetrics->Branch ("cacheEfficiencyRel", &effRel, "cacheEfficiencyRel/D");
    Long64_t bytesRead = tc->GetBytesRead ();
    m_fileMetrics->Branch ("bytesRead", &bytesRead, "bytesRead/L");
    Int_t readCalls = tc->GetReadCalls ();
    m_fileMetrics->Branch ("readCalls", &readCalls, "readCalls/I");
    Long64_t ncBytesRead = tc->GetNoCacheBytesRead ();
    m_fileMetrics->Branch ("noCacheBytesRead", &ncBytesRead, "noCacheBytesRead/L");
    Int_t ncReadCalls = tc->GetNoCacheReadCalls ();
    m_fileMetrics->Branch ("noCacheReadCalls", &ncReadCalls, "noCacheReadCalls/I");
    Int_t readahead = TFile::GetReadaheadSize ();
    m_fileMetrics->Branch ("readaheadSize", &readahead, "readaheadSize/I");
    Long64_t bytesReadExtra = tc->GetBytesReadExtra ();
    m_fileMetrics->Branch ("bytesReadExtra", &bytesReadExtra, "bytesReadExtra/L");
    m_fileMetrics->Fill ();
  }
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode EL::MetricsSvc :: execute ()
{
  RCU_CHANGE_INVARIANT (this);
  m_eventsRead ++;
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode EL::MetricsSvc :: histFinalize ()
{
  RCU_CHANGE_INVARIANT (this);
  m_benchmark->Stop("loopmetrics");
  m_jobMetrics->Branch("eventsRead", &m_eventsRead, "eventsRead/I");
  m_jobMetrics->Branch("filesRead", &m_filesRead, "filesRead/I");
  Float_t realTime = m_benchmark->GetRealTime("loopmetrics");
  m_jobMetrics->Branch("loopWallTime", &realTime, "loopWallTime/F");
  Float_t cpuTime = m_benchmark->GetCpuTime("loopmetrics");
  m_jobMetrics->Branch("loopCpuTime", &cpuTime, "loopCpuTime/F");
  Float_t evtsWallSec = m_eventsRead / (realTime != 0 ? realTime : 1.);
  m_jobMetrics->Branch("eventsPerWallSec", &evtsWallSec, "eventsPerWallSec/F");
  Float_t evtsCpuSec = m_eventsRead / (cpuTime != 0 ? cpuTime : 1.);  
  m_jobMetrics->Branch("eventsPerCpuSec", &evtsCpuSec, "eventsPerCpuSec/F");
  m_jobMetrics->Fill();
  return EL::StatusCode::SUCCESS;
}
