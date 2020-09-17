/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/checker_macros.h"
#include "InDetPerformanceMonitoring/EventAnalysis.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TProfile2D.h"


namespace{
  template <class HistoArrayType>
  void registerHistogramType
    (ITHistSvc& histSvc,
     const HistoArrayType & h, const std::string &sampleName, const std::string & suffix){
    unsigned int u = 1;
    const std::string titleRoot{"/ESD/" + sampleName + suffix};
    for ( auto & thisHisto:h ){
      histSvc.regHist( titleRoot + std::to_string(u), thisHisto.second ).ignore();
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
void EventAnalysis::Init()
{
  // This must be called by an inheriting class in order to book
  //  & register histograms.
  BookHistograms();
  Register();
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
void EventAnalysis::Register()
{
  ServiceHandle<ITHistSvc> histSvc ("THistSvc", "EventAnalysis");
  
  // Register histograms in monitoring tool
  registerHistogramType(*histSvc, m_x1DHistograms, m_xSampleName, "/1dhisto_");
  registerHistogramType(*histSvc, m_x2DHistograms, m_xSampleName, "/2dhisto_");

  registerHistogramType(*histSvc, m_x1DProfHistograms, m_xSampleName, "/1dprof_");
  registerHistogramType(*histSvc, m_x2DProfHistograms, m_xSampleName, "/2dprof_");
}


//=============================================================================
