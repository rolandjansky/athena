/* emacs: this is -*- c++ -*- */
/**
 **     @file    TrackDistributions.h
 **
 **     @author  mark sutton
 **     @date    Sun 18 Jan 2009 19:08:11 GMT 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#include <iostream>

#include "TrigInDetAnalysis/TrackAnalysis.h"

// class Track;
// class Effplot;
// class Resplot;

#include "TH1D.h"

class TrackDistributions : public TrackAnalysis { 
   
public:
  
  using TrackAnalysis::execute;
  
public:
  
  TrackDistributions( const std::string& name ) :
    TrackAnalysis( name )
  { }  
  
  virtual void initialise();

  virtual void execute(const std::vector<TIDA::Track*>& tracks1,
		       const std::vector<TIDA::Track*>& tracks2,
		       TrackAssociator* matcher );

  virtual void finalise();
  
private:

  TH1D*   m_heta;
  TH1D*   m_hphi;
  TH1D*   m_hz0;
  TH1D*   m_hd0;
  TH1D*   m_hpT;

  TH1D*   m_hdeta;
  TH1D*   m_hdphi;
  TH1D*   m_hdz0;
  TH1D*   m_hdd0;
  TH1D*   m_hdpT;

  TH1D*   m_hchi2;

  TH1D*   m_hblayer;
  TH1D*   m_hpixel;
  TH1D*   m_hsct;
  TH1D*   m_hsilicon;
  TH1D*   m_hstraw;
  TH1D*   m_htr;
  
};


