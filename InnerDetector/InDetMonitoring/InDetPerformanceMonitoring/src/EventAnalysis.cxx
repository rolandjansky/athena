/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include "InDetPerformanceMonitoring/EventAnalysis.h"
#include "InDetPerformanceMonitoring/PerfMonServices.h"

#include <sstream>

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
void EventAnalysis::Register()
{
  // Go off and does whatever the hell Athena does to histograms.
  Register_1DHistos();
  Register_2DHistos();

  Register_1DProfHistos();
  Register_2DProfHistos();
}

void EventAnalysis::Register_1DHistos()
{
  // Iterate over all the 1D Histograms
  unsigned int u = 1;
  std::map<unsigned int, TH1F*>::iterator xIter =  m_x1DHistograms.begin();

  while ( xIter != m_x1DHistograms.end() )
  {
    // Set up an appropriate string & cycle the u_int counter.
    std::stringstream xTmp;  xTmp << u;  ++u;
    PerfMonServices::getHistogramService()->regHist( "/ESD/" + m_xSampleName + "/1dhisto_" + xTmp.str(), xIter->second );
    xIter++;
  }
}

void EventAnalysis::Register_2DHistos()
{
  // Iterate over all the 2D Histograms
  unsigned int u = 1;
  std::map<unsigned int, TH2F*>::iterator xIter =  m_x2DHistograms.begin();

  while ( xIter != m_x2DHistograms.end() )
  {
    std::stringstream xTmp;  xTmp << u;  ++u;
    PerfMonServices::getHistogramService()->regHist( "/ESD/" + m_xSampleName + "/2dhisto_"+ xTmp.str(), xIter->second );
    xIter++;
  }
}

void EventAnalysis::Register_1DProfHistos()
{
  unsigned int u = 1;
  std::map<unsigned int, TProfile*>::iterator xIter =  m_x1DProfHistograms.begin();

  while ( xIter != m_x1DProfHistograms.end() )
  {
    std::stringstream xTmp;  xTmp << u;  ++u;
    PerfMonServices::getHistogramService()->regHist( "/ESD/" + m_xSampleName + "/1dprof_"+ xTmp.str(), xIter->second );
    xIter++;
  }
}

void EventAnalysis::Register_2DProfHistos()
{
  unsigned int u = 1;
  std::map<unsigned int, TProfile2D*>::iterator xIter =  m_x2DProfHistograms.begin();

  while ( xIter != m_x2DProfHistograms.end() )
  {
    std::stringstream xTmp;  xTmp << u;  ++u;
    PerfMonServices::getHistogramService()->regHist( "/ESD/" + m_xSampleName + "/2dprof_"+ xTmp.str(), xIter->second );
    xIter++;
  }
}

//=============================================================================
