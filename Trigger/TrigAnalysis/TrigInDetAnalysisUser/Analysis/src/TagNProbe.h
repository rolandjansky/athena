/* emacs: this is -*- c++ -*- */
/**
 **   @file    TagNProbe.h        
 **                   
 **   @author  maparo
 **   @date    Wed 22 May 2019 21:22:50 BST
 **
 **   $Id: TagNProbe.h, v0.0   Wed 22 May 2019 21:22:50 BST maparo $
 **
 **   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef  TIDA_TAGNPROBE_H
#define  TIDA_TAGNPROBE_H

#include <cstdlib>

#include "TLorentzVector.h"

#include "TrigInDetAnalysis/TIDAEvent.h" 
#include "TrigInDetAnalysis/TrackSelector.h" 
#include "TrigInDetAnalysisUtils/Filters.h" 
#include "TrigInDetAnalysisUtils/Filter_Offline2017.h" 
#include "TrigInDetAnalysisExample/NtupleTrackSelector.h" 

#include "RoiFilter.h"

#include <iostream> 
#include <vector> 
#include <map> 

#include "TrigInDetAnalysis/TrackAnalysis.h" 
#include "TrigInDetAnalysis/Track.h" 
#include "TrigInDetAnalysis/TIDDirectory.h" 
#include "TrigInDetAnalysis/Efficiency.h" 
#include "TrigInDetAnalysis/TIDARoiDescriptor.h" 
#include "TrigInDetAnalysisExample/ChainString.h"


class TagNProbe {

public:

  TagNProbe() { }

  virtual ~TagNProbe() { }

  void SetOfflineTracks( NtupleTrackSelector * refTracks, TrackFilter* refFilter ) {
    m_refTracks = refTracks;
    m_refFilter = refFilter;
  } 

  void SetChains( TIDA::Chain * chain, TIDA::Chain * chain_tnp ) {
    m_chain = chain;
    m_chain_tnp = chain_tnp;
  }

  bool FindProbes();

  std::vector<TIDA::Roi*> GetProbes() { return m_probes; }

  std::vector<TIDA::Roi*> GetTags( unsigned int probe_index=0 ) { return m_tags[ probe_index ]; }

  std::vector<double> GetInvMasses( unsigned int probe_index=0 ) { return m_masses[ probe_index ]; }

  double selection( TIDA::Roi & troi, TIDA::Roi & proi );

  double computeZ( TIDA::Track* t1, TIDA::Track* t2, double mass=0 );

  void SetUniqueFlag( bool flag ) { m_unique = flag; }

private:

  NtupleTrackSelector * m_refTracks;
  TrackFilter * m_refFilter;

  TIDA::Chain * m_chain;
  TIDA::Chain * m_chain_tnp;

  std::vector<TIDA::Roi*> m_probes;
  std::vector< std::vector<double> > m_masses;
  std::vector< std::vector<TIDA::Roi*> > m_tags;

  bool m_unique;

};

#endif  // TIDA_TAGNPROBE_H 
