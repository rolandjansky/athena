/// emacs: this is -*- c++ -*-
/**
 **     @file    TagNProbe.h
 **
 **     @author  mark sutton
 **     @date    Sat Apr  9 12:55:17 CEST 2022
 **
 **     Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TIDAUTILS_TAGNPROBE_H
#define TIDAUTILS_TAGNPROBE_H

#include <vector> 

#include "TrigInDetAnalysis/TIDAChain.h"
#include "TrigInDetAnalysis/TIDARoiDescriptor.h"
#include "TrigInDetAnalysis/TrigObjectMatcher.h"
#include "TrigInDetAnalysis/Track.h" 

#include "TrigInDetAnalysis/TrackSelector.h" 
#include "TrigInDetAnalysisUtils/Filters.h" 

#include "TLorentzVector.h"


class TagNProbe {

public:

  TagNProbe( const std::string& refName, double massMin, double massMax, bool unique_flag=true );
  
  virtual ~TagNProbe() { }

  /// getters and setters

  /// could be moved to the constructor now ...
  void   tag( const std::string& chainName ) { m_tagChainName   = chainName; }
  void probe( const std::string& chainName ) { m_probeChainName = chainName; }

  const std::string&   tag() const { return m_tagChainName; }
  const std::string& probe() const { return m_probeChainName; }

public:

  template<typename T>
  std::vector<TIDA::Roi*> GetRois( std::vector<TIDA::Chain>& chains, 
				   const TrackSelector*  refTracks, 
				   TrackFilter*          refFilter,
				   T* hmass,  
				   T* hmass_obj,
				   TrigObjectMatcher* tom=0 ) const {  

    std::vector<TIDA::Roi*> probes;

    TIDA::Chain* chain_tag   = findChain( tag(), chains ); 
    TIDA::Chain* chain_probe = findChain( probe(), chains ); 

    if ( chain_tag==0 || chain_probe==0 ) return probes;

    // loop for possible probes
    for ( size_t ip=0 ; ip<chain_probe->size() ; ip++ ) {
      
      TIDA::Roi& proi = chain_probe->rois()[ip];
    
      TIDARoiDescriptor roi_probe( proi.roi() );
      
      bool found_tnp = false;
            
      // loop for possible tags
      for ( size_t it=0 ; it<chain_tag->size() ; it++ ) {  
	
	TIDA::Roi& troi = chain_tag->rois()[it];
	TIDARoiDescriptor roi_tag( troi.roi() );
	
	/// tag and probe are the same: skip this tag
	if ( roi_probe == roi_tag ) continue;
	
	if ( selection( troi, proi, refTracks, refFilter, hmass, hmass_obj, tom ) ) { 
	  found_tnp = true;
	  if ( m_unique ) break;
	}
	
      } // end loop on tags
      
      if ( found_tnp ) probes.push_back( &proi );
      
    } // end loop on probes
    
    return probes;
    
  }

protected:
  
  double pt( const TIDA::Track* t )     const { return t->pT(); }   
  double pt( const TrackTrigObject* t ) const { return t->pt(); }   
  
  template<typename T>
  double mass( const T* t1, const T* t2 ) const {
    TLorentzVector v1;
    v1.SetPtEtaPhiM( pt(t1)*0.001, t1->eta(), t1->phi(), m_mass );
    TLorentzVector v2;
    v2.SetPtEtaPhiM( pt(t2)*0.001, t2->eta(), t2->phi(), m_mass );
    return (v1+v2).M();
  }


  template<typename T>
  bool selection( const TIDA::Roi& troi, const TIDA::Roi& proi, 
		  const TrackSelector* refTracks, 
		  TrackFilter*         refFilter,
		  T* hmass, 
		  T* hmass_obj,
		  TrigObjectMatcher*  tom=0) const {
    
    /// get reference tracks from the tag roi                                                                                                                                                                           
    TIDARoiDescriptor roi_tag( troi.roi() );

    dynamic_cast<Filter_Combined*>(refFilter)->setRoi( &roi_tag );

    std::vector<TIDA::Track*> refp_tag = refTracks->tracks( refFilter );

    /// get reference tracks from the probe roi                                                                                                                                                                      
    TIDARoiDescriptor roi_probe( proi.roi() );

    dynamic_cast<Filter_Combined* >( refFilter )->setRoi( &roi_probe );

    std::vector<TIDA::Track*> refp_probe = refTracks->tracks( refFilter );

    /// loop over tag ref tracks                                                                                                                                                                                           
    bool found = false;

    for ( size_t it=0; it<refp_tag.size() ; it++ ) {

      /// loop over probe ref tracks
      for ( size_t ip=0; ip<refp_probe.size() ; ip++ ) {

	/// check compatibility of the track z and invariant mass ...
	double invmass     = mass( refp_tag[it], refp_probe[ip] );
	double invmass_obj = mass_obj( refp_tag[it], refp_probe[ip], tom );
	double deltaz0     = std::fabs(refp_tag[it]->z0() - refp_probe[ip]->z0() );
	
	if ( invmass_obj>m_massMin && invmass_obj<m_massMax && deltaz0<5 ) {
	  hmass->Fill( invmass );
	  hmass_obj->Fill( invmass_obj );
	  found = true;
	}

      }
    }

    return found;

  }


  double   mass_obj( const TIDA::Track* t1, const TIDA::Track* t2, TrigObjectMatcher* tom=0 ) const;

  TIDA::Chain* findChain( const std::string& chainname, std::vector<TIDA::Chain>& chains ) const;


private:

  std::string m_particleType;

  double m_mass;

  double m_massMin;
  double m_massMax;

  bool   m_unique;

  std::string  m_probeChainName ;
  std::string  m_tagChainName ;

};


#endif ///  TIDAUTILS_TAGNPROBE_H


