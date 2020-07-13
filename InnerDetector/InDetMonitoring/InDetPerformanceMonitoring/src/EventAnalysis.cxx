/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/checker_macros.h"
#include "InDetPerformanceMonitoring/EventAnalysis.h"
#include "InDetPerformanceMonitoring/PerfMonServices.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TProfile2D.h"


namespace{
  template <class HistoArrayType>
  void registerHistogramType ATLAS_NOT_THREAD_SAFE // Thread unsafe PerfMonServices class is used.
    (const HistoArrayType & h, const std::string &sampleName, const std::string & suffix){
    unsigned int u = 1;
    const std::string titleRoot{"/ESD/" + sampleName + suffix};
    for ( auto & thisHisto:h ){
      PerfMonServices::getHistogramService()->regHist( titleRoot + std::to_string(u), thisHisto.second ).ignore();
      ++u;
    }
  }
}

//=============================================================================
// Constructors & Destructors
//=============================================================================

EventAnalysis::EventAnalysis()
{
  m_uPassedEvents = 0;
}

EventAnalysis::~EventAnalysis()
{
}

//=============================================================================
// Public Accessors
//=============================================================================
void EventAnalysis::Init ATLAS_NOT_THREAD_SAFE () // Thread unsafe Register method is used.
{
  // This must be called by an inheriting class in order to book
  //  & register histograms.
  BookHistograms();
  Register();
}

bool EventAnalysis::Reco()
{
  // This must be overriden by an inheriting class.
  return false;
}

//=============================================================================
// Protected Accessors
//=============================================================================
void EventAnalysis::BookHistograms()
{
  // This must be overriden by an inheriting class.
}

//=============================================================================
// Private Accessors
//=============================================================================
void EventAnalysis::Register ATLAS_NOT_THREAD_SAFE () // Thread unsafe registerHistogramType fuction is used.
{
  // Register histograms in monitoring tool
  registerHistogramType(m_x1DHistograms, m_xSampleName, "/1dhisto_");
  registerHistogramType(m_x2DHistograms, m_xSampleName, "/2dhisto_");

  registerHistogramType(m_x1DProfHistograms, m_xSampleName, "/1dprof_");
  registerHistogramType(m_x2DProfHistograms, m_xSampleName, "/2dprof_");
}


//=============================================================================
