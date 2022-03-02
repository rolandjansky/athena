/// emacs: this is -*- c++ -*-
/**
 **     @file    TagNProbe.h
 **
 **     @author  marco aparo
 **     @date    Fri 02 Jul 2021 13:30:00 CET 
 **
 **     Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TIDAUTILS_TAGNPROBE_H
#define TIDAUTILS_TAGNPROBE_H

#include <stdlib.h>

#include "TLorentzVector.h"

#include "TrigInDetAnalysis/TIDAEvent.h" 
#include "TrigInDetAnalysis/TrackSelector.h" 
#include "TrigInDetAnalysisUtils/Filters.h" 
#include "TrigInDetAnalysisUtils/Filter_Offline2017.h" 
#include "TrigInDetAnalysis/TrackSelector.h" 
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


class TagNProbe {

public:

  TagNProbe() { }

  virtual ~TagNProbe() { }


  /// configuration methods

  void SetEventConfiguration( 
          TrackSelector * refTracks,          // reference tracks
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

  void SetOfflineTracks( TrackSelector * refTracks, TrackFilter* refFilter ) {
    m_refTracks = refTracks;
    m_refFilter = refFilter;
  } 

  void SetChains( TIDA::Chain * chain, TIDA::Chain * chain_tnp ) {
    m_chain = chain;
    m_chain_tnp = chain_tnp;
  }

  template<typename T> 
  void Fill( T* h, T* h1, int i ) { 
    /// don't need to check both these, we can just call this as many times as we like, 
    /// could pass in the vector even so that  
    // we leave the old code in, but commented, since we are still developing, so once 
    // we know everything is working we can delete all the older code
    //    if ( m_masses[i].size() == m_masses_obj[i].size() && m_masses[i].size() > 0 ) {

    /// don't understand this - why is this method filling lots of masses 
    /// from an vector of masses from 0 up to the input index ?
    /// isn't this index just the index of the roi ? Why the loop ?
    for ( size_t im=0 ; im<m_masses[i].size() ; im++ ) { 
      h->Fill( m_masses[i].at(im) );
    }
    for ( size_t im=0 ; im<m_masses_obj[i].size() ; im++ ) { 
      h1->Fill( m_masses_obj[i].at(im) );
    }
  } 


  /// probe searching method

  bool FindProbes();

  void FindTIDAChains( std::vector<TIDA::Chain>& chains ) ;

  /// getter methods

  std::vector<TIDA::Roi*> GetProbes() { return m_probes; }

  std::vector<TIDA::Roi*> GetTags( unsigned int probe_index=0 ) { return m_tags[ probe_index ]; }

  std::vector<double> GetInvMasses( unsigned int probe_index=0 ) { return m_masses[ probe_index ]; }

  std::vector<double> GetInvMasses_obj( unsigned int probe_index=0 ) { return m_masses_obj[ probe_index ]; }

  std::vector<TIDA::Roi*> GetRois( TIDA::Chain * chain, std::vector<TIDA::Chain>& chains );

  std::vector<TIDA::Roi*> GetRois( std::vector<TIDA::Chain>& chains );

  void tag( const std::string& chainName ) { m_tagChainName = chainName ; }

  const std::string& tag() { return m_tagChainName ; }

  void probe( const std::string& chainName ) { m_probeChainName = chainName ; }

  const std::string& probe() { return m_probeChainName ; }

  TIDA::Chain* GetTIDAProbe() { return m_chain_tnp ; } 

  TIDA::Chain* GetTIDATag() { return m_chain ; }

  TH1D* GetMinvHisto() { return m_hMinv ; }

  TH1D* GetMinvObjHisto() { return m_hMinv_obj ; }


  /// utility methods
  
  void FillMap( std::vector<std::string>& tnpChains );
  
  std::vector<std::string> GetProbeChainNames() { return m_probe_chain_names; }

  bool isTnP() { return m_tnp_map.size()>0; }

  TIDA::Chain* GetTagChain( std::string probe_name, std::vector<TIDA::Chain>& chains );

  void BookMinvHisto( std::string chain_name );

  void BookMinvHisto();

  void FillMinvHisto( std::string chain_name, unsigned int probe_index );

  void FillMinvHisto( unsigned int probe_index );

  void WriteMinvHisto( TDirectory* foutdir );


  /// internal methods for computation (protected)

protected:

  std::pair<double,double> selection( TIDA::Roi & troi, TIDA::Roi & proi );

  double computeZ( TIDA::Track* t1, TIDA::Track* t2 );

  double computeZ_obj( TIDA::Track* t1, TIDA::Track* t2 );


  /// internally used variables

private:

  TrackSelector * m_refTracks;
  TrackFilter * m_refFilter;

  TIDA::Chain * m_chain;
  TIDA::Chain * m_chain_tnp;

  std::string m_probeChainName ;
  std::string m_tagChainName ;

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

  TH1D* m_hMinv ;
  TH1D* m_hMinv_obj ;

};

#endif  // TIDAUTILS_TAGNPROBE_H 
