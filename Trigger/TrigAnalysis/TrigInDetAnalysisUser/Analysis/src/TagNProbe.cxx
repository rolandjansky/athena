/**
 **   @file    TagNProbe.cxx         
 **   
 **
 **   @author maparo
 **   @date   Wed 22 May 2019 21:21:50 BST
 **
 **   $Id: TagNProbe.cxx, v0.0   Wed 22 May 2019 21:21:50 BST maparo $
 **
 **   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/

#include "TagNProbe.h"



double TagNProbe::computeZ( TIDA::Track* t1, TIDA::Track* t2, double mass ) {

  double ZMass = 91.1876; // GeV

  /// !!!!! why ????
  TLorentzVector v1;
  v1.SetPtEtaPhiM( (t1->pT())/1000., t1->eta(), t1->phi(), mass );

  TLorentzVector v2;
  v2.SetPtEtaPhiM( (t2->pT())/1000., t2->eta(), t2->phi(), mass );

  double invMass = ( v1 + v2 ).M();

  //if ( std::fabs( invMass - ZMass )<10 );
  if ( std::fabs( invMass - ZMass )<50 ) // "relaxed" Z mass window
    return invMass;
  
  return -1.0;

}

// ------------------------------------------------------------------

double TagNProbe::selection( TIDA::Roi& troi, TIDA::Roi& proi )
{

  TIDARoiDescriptor roi_tag( troi.roi() );
  TIDARoiDescriptor roi_probe( proi.roi() );

  /// getting reference tracks from the tag roi
  dynamic_cast< Filter_Combined* >( m_refFilter )->setRoi( &roi_tag );
  std::vector< TIDA::Track* > refp_tag/*_vec*/ = m_refTracks->tracks( m_refFilter );
  //const std::vector< TIDA::Track* > & refp_tag = refp_tag_vec;

  /// getting reference tracks from the probe roi
  dynamic_cast< Filter_Combined* >( m_refFilter )->setRoi( &roi_probe );
  std::vector< TIDA::Track* > refp_probe/*_vec*/ = m_refTracks->tracks( m_refFilter );
  //const std::vector< TIDA::Track* > & refp_probe = refp_probe_vec;

  /// loop on tag ref tracks
  for ( size_t it=0; it<refp_tag.size() ; it++ ) {

    /// loop on probe ref tracks
    for ( size_t ip=0; ip<refp_probe.size() ; ip++ ) {

      /// check compatibility w.r.t. Z mass
      double TauMass = 1.77686; // GeV
      double pair_mass = computeZ( refp_tag[it], refp_probe[ip], TauMass );
      if ( pair_mass>0 )
          return pair_mass;

    }

  }

  return -1.0;

}

// ------------------------------------------------------------------

bool TagNProbe::FindProbes() 
{

  m_probes.clear();
  m_tags.clear();
  m_masses.clear();

  if ( m_chain==0 || m_chain_tnp==0 )
    return false;

  // loop for possible probes
  for ( size_t ip=0 ; ip<m_chain_tnp->size() ; ip++ ) {

    TIDA::Roi& proi = m_chain_tnp->rois()[ ip ];
    TIDARoiDescriptor roi_probe( proi.roi() );

    bool found_tnp = false;
  
    std::vector<TIDA::Roi*> tags;
    std::vector<double> masses;

    // loop for possible tags
    for ( size_t it=0 ; it<m_chain->size() ; it++ ) {  

      TIDA::Roi& troi = m_chain->rois()[ it ];
      TIDARoiDescriptor roi_tag( troi.roi() );
  
      /// tag and probe are the same: skip this tag
      if ( roi_probe == roi_tag ) continue;

      double mass = selection( troi, proi );

      if ( !found_tnp && mass>0 ) found_tnp = true;

      if ( mass>0 ) {
        tags.push_back( &troi );
        masses.push_back( mass );
        if ( m_unique ) break;
      }

    } // end loop on tags

    if ( found_tnp ) {
      m_probes.push_back( &proi );
      m_tags.push_back( tags );
      m_masses.push_back( masses );
    }

  } // end loop on probes

  return true;

}
