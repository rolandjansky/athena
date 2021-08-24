/**
 **     @file    TagNProbe.h
 **
 **     @author  marco aparo
 **     @date    Fri 02 Jul 2021 13:30:00 CET 
 **
 **     Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 **/

#ifndef TIDA_TAGNPROBE_H
#define TIDA_TAGNPROBE_H

#include <stdlib.h>

#include "TLorentzVector.h"

#include "TrigInDetAnalysis/TIDAEvent.h" 
#include "TrigInDetAnalysis/TrackSelector.h" 
#include "TrigInDetAnalysisUtils/Filters.h" 
#include "TrigInDetAnalysisUtils/Filter_Offline2017.h" 
#include "TrigInDetAnalysisExample/NtupleTrackSelector.h" 
#include "RoiFilter.h"
#include "utils.h"
#include "TrigInDetAnalysis/TrigObjectMatcher.h"
#include "TH1D.h"

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


  /// configuration methods

  void SetEventConfiguration( 
          NtupleTrackSelector * refTracks,    // reference tracks
          TrackFilter* refFilter,             // reference filter
          std::string refName,                // reference objects name
          TrigObjectMatcher* tom,             // trigger object matcher 
          double ZmassMin,                    // ZmassMin
          double ZmassMax,                    // ZmassMax
          bool unique_flag=true ) {           // unique flag (default=true)
    m_refTracks = refTracks;
    m_refFilter = refFilter;
    m_particleType = refName;
    m_tom = tom;
    m_ZmassMin = ZmassMin;
    m_ZmassMax = ZmassMax;
    m_unique = unique_flag;
  }

  void ResetEventConfiguration() { 
    m_refTracks = 0;
    m_refFilter = 0;
    m_particleType = "";
    m_tom = 0;
    m_ZmassMin = 0.;
    m_ZmassMax = 999.;
    m_unique = false;
  }
    
  void SetUniqueFlag( bool flag ) { m_unique = flag; }
 
  void SetParticleType( std::string type ) { m_particleType = type; }
 
  void SetZmassWindow( double ZmassMin, double ZmassMax ) { m_ZmassMin = ZmassMin; m_ZmassMax = ZmassMax; }

  void SetObjMatcher( TrigObjectMatcher* tom ) { m_tom = tom; }

  void SetOfflineTracks( NtupleTrackSelector * refTracks, TrackFilter* refFilter ) {
    m_refTracks = refTracks;
    m_refFilter = refFilter;
  } 

  void SetChains( TIDA::Chain * chain, TIDA::Chain * chain_tnp ) {
    m_chain = chain;
    m_chain_tnp = chain_tnp;
  }


  /// probe searching method

  bool FindProbes();


  /// getter methods

  std::vector<TIDA::Roi*> GetProbes() { return m_probes; }

  std::vector<TIDA::Roi*> GetTags( unsigned int probe_index=0 ) { return m_tags[ probe_index ]; }

  std::vector<double> GetInvMasses( unsigned int probe_index=0 ) { return m_masses[ probe_index ]; }

  std::vector<double> GetInvMasses_obj( unsigned int probe_index=0 ) { return m_masses_obj[ probe_index ]; }

  std::vector<TIDA::Roi*> GetRois( TIDA::Chain * chain, std::vector<TIDA::Chain>& chains );


  /// utility methods
  
  void FillMap( std::vector<std::string>& tnpChains );
  
  std::vector<std::string> GetProbeChainNames() { return m_probe_chain_names; }

  bool isTnP() { return m_tnp_map.size()>0; }

  TIDA::Chain* GetTagChain( std::string probe_name, std::vector<TIDA::Chain>& chains );

  void BookMinvHisto( std::string chain_name );

  void FillMinvHisto( std::string chain_name, unsigned int probe_index );

  void WriteMinvHisto( TDirectory* foutdir );


  /// internal methods for computation (protected)

protected:

  std::pair<double,double> selection( TIDA::Roi & troi, TIDA::Roi & proi );

  double computeZ( TIDA::Track* t1, TIDA::Track* t2 );

  double computeZ_obj( TIDA::Track* t1, TIDA::Track* t2 );


  /// internally used variables

private:

  NtupleTrackSelector * m_refTracks;
  TrackFilter * m_refFilter;

  TIDA::Chain * m_chain;
  TIDA::Chain * m_chain_tnp;

  std::vector<TIDA::Roi*> m_probes;
  std::vector< std::vector<double> > m_masses;
  std::vector< std::vector<double> > m_masses_obj;
  std::vector< std::vector<TIDA::Roi*> > m_tags;

  bool m_unique;

  std::string m_particleType;

  double m_ZmassMin, m_ZmassMax;

  TrigObjectMatcher* m_tom;


  /// supporting variables for utility methods

  std::map<std::string,std::string> m_tnp_map;
  std::vector<std::string> m_probe_chain_names;

  std::map<std::string,TH1D*> m_hMinv_map;
  std::map<std::string,TH1D*> m_hMinv_obj_map;

};

#endif  // TIDA_TAGNPROBE_H 
