/**
 **     @file    TagNProbe.cxx
 **
 **     @author  marco aparo
 **     @date    Fri 02 Jul 2021 13:30:00 CET 
 **
 **     Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 **/


#include "TrigInDetAnalysisUtils/TagNProbe.h"


double TagNProbe::computeZ_obj( TIDA::Track* t1, TIDA::Track* t2 ) {

  double muonMass = 0.10565; // GeV
  double electronMass = 0.000510; // GeV
  double tauMass = 1.77686; // GeV

  /// get tag and probe particle type and setting mass

  TString tnpType( m_particleType );
  double mass = 0.; 
  if      ( tnpType.Contains("Muon") )      mass = muonMass;
  else if ( tnpType.Contains("Electron") )  mass = electronMass;
  else if ( tnpType.Contains("Tau") )       mass = tauMass;

  TLorentzVector v1, v2;
  double z0_1, z0_2;
  if ( tnpType.Contains("Electron") ) {
    
    if ( m_tom != 0 && !m_tom->status() )
      return -1.0;

    const TrackTrigObject* tobj1 = m_tom->object( t1->id() );
    const TrackTrigObject* tobj2 = m_tom->object( t2->id() );

    v1.SetPtEtaPhiM( (tobj1->pt())/1000., tobj1->eta(), tobj1->phi(), mass );
    v2.SetPtEtaPhiM( (tobj2->pt())/1000., tobj2->eta(), tobj2->phi(), mass );
    z0_1 = (double)(tobj1->z0());
    z0_2 = (double)(tobj2->z0());
  }
  else {
    v1.SetPtEtaPhiM( (t1->pT())/1000., t1->eta(), t1->phi(), mass );
    v2.SetPtEtaPhiM( (t2->pT())/1000., t2->eta(), t2->phi(), mass );
    z0_1 = (double)(t1->z0());
    z0_2 = (double)(t2->z0());
  }

  double invMass = (double)( v1 + v2 ).M();
  double z0diff = std::fabs( z0_1 - z0_2 );

  if ( invMass > m_ZmassMin && invMass < m_ZmassMax && z0diff < 5 )  return invMass;
  
  return -1.0;

}




// ------------------------------------------------------------------

double TagNProbe::computeZ( TIDA::Track* t1, TIDA::Track* t2 ) {

  double muonMass = 0.10565; // GeV
  double electronMass = 0.000510; // GeV
  double tauMass = 1.77686; // GeV

  /// get tag and probe particle type and setting mass

  TString tnpType( m_particleType );
  double mass = 0.; 
  if      ( tnpType.Contains("Muon") )      mass = muonMass;
  else if ( tnpType.Contains("Electron") )  mass = electronMass;
  else if ( tnpType.Contains("Tau") )       mass = tauMass;

  TLorentzVector v1;
  v1.SetPtEtaPhiM( (t1->pT())/1000., t1->eta(), t1->phi(), mass );

  TLorentzVector v2;
  v2.SetPtEtaPhiM( (t2->pT())/1000., t2->eta(), t2->phi(), mass );

  double invMass = (double)( v1 + v2 ).M();

  /// this cut is performed on the object-based inv. mass 
  return invMass;
  
}




// ------------------------------------------------------------------

std::pair<double,double> TagNProbe::selection( TIDA::Roi& troi, TIDA::Roi& proi )
{

  TIDARoiDescriptor roi_tag( troi.roi() );
  TIDARoiDescriptor roi_probe( proi.roi() );

  /// getting reference tracks from the tag roi
  dynamic_cast< Filter_Combined* >( m_refFilter )->setRoi( &roi_tag );
  std::vector< TIDA::Track* > refp_tag = m_refTracks->tracks( m_refFilter );

  /// getting reference tracks from the probe roi
  dynamic_cast< Filter_Combined* >( m_refFilter )->setRoi( &roi_probe );
  std::vector< TIDA::Track* > refp_probe = m_refTracks->tracks( m_refFilter );

  /// loop on tag ref tracks
  for ( size_t it=0; it<refp_tag.size() ; it++ ) {

    /// loop on probe ref tracks
    for ( size_t ip=0; ip<refp_probe.size() ; ip++ ) {

      /// check compatibility w.r.t. Z mass
      double pair_mass      = computeZ( refp_tag[it], refp_probe[ip] );
      double pair_mass_obj  = computeZ_obj( refp_tag[it], refp_probe[ip] );
      std::pair<double,double> InvMass = std::make_pair( pair_mass, pair_mass_obj );
      if ( pair_mass_obj > 0 )
          return InvMass;

    }

  }

  std::pair<double,double> pNull = std::make_pair( 0., 0. );

  return pNull;

}




// ------------------------------------------------------------------

bool TagNProbe::FindProbes() 
{

  m_probes.clear();
  m_tags.clear();
  m_masses.clear();
  m_masses_obj.clear();

  if ( m_chain==0 || m_chain_tnp==0 )
    return false;

  // loop for possible probes
  for ( size_t ip=0 ; ip<m_chain_tnp->size() ; ip++ ) {
    
    TIDA::Roi& proi = m_chain_tnp->rois()[ ip ];
    TIDARoiDescriptor roi_probe( proi.roi() );

    bool found_tnp = false;
  
    std::vector<TIDA::Roi*> tags;
    std::vector<double> masses;
    std::vector<double> masses_obj;

    // loop for possible tags
    for ( size_t it=0 ; it<m_chain->size() ; it++ ) {  

      TIDA::Roi& troi = m_chain->rois()[ it ];
      TIDARoiDescriptor roi_tag( troi.roi() );

      /// tag and probe are the same: skip this tag
      if ( roi_probe == roi_tag ) continue;

      std::pair<double,double> InvMasses = selection( troi, proi );
      double mass     = InvMasses.first;
      double mass_obj = InvMasses.second;

      if ( !found_tnp && mass>0 ) found_tnp = true;

      if ( mass>0 ) {
	tags.push_back( &troi );
        masses.push_back( mass );
        masses_obj.push_back( mass_obj );
        if ( m_unique ) break;
      }

    } // end loop on tags

    if ( found_tnp ) {
      m_probes.push_back( &proi );
      m_tags.push_back( tags );
      m_masses.push_back( masses );
      m_masses_obj.push_back( masses_obj );
    }

  } // end loop on probes

  return true;

}

// ------------------------------------------------------------------

void TagNProbe::FillMap( std::vector<std::string>& tnpChains ) {

  /// first clear the map
  m_tnp_map.clear();

  for ( size_t i=0 ; i<tnpChains.size()-1 ; i+=2 ) { 
     
    std::string chain_tag_name = (tnpChains[i]).substr( 0, (tnpChains[i]).find(":post") );
    std::string chain_probe_name = (tnpChains[i+1]).substr( 0, (tnpChains[i+1]).find(":post") );

    m_tnp_map.insert( std::map<std::string,std::string>::value_type( chain_probe_name, chain_tag_name ) );
      
    /// new testChain vector is filled only with probe chain names
    m_probe_chain_names.push_back( tnpChains[i+1] );
  }

}

// ------------------------------------------------------------------

TIDA::Chain* TagNProbe::GetTagChain( std::string probe_name, std::vector<TIDA::Chain>& chains ) {

  TIDA::Chain* chain_tag = 0;

  std::map<std::string,std::string>::const_iterator tnpitr = m_tnp_map.find( probe_name );
  
  if ( tnpitr != m_tnp_map.end() ) {

    for ( size_t icp=0 ; icp<chains.size() ; icp++ ) {

      std::string  tag_name = chains[icp].name();
      
      if ( tag_name == tnpitr->second ) {
        chain_tag = &( chains[icp] );
        break;
      }
    }
  }

  return chain_tag;

}

// ------------------------------------------------------------------

std::vector<TIDA::Roi*> TagNProbe::GetRois( TIDA::Chain * chain, std::vector<TIDA::Chain>& chains ) {

  std::vector<TIDA::Roi*> rois;
  TIDA::Chain* chain_tag = 0;

  /// for each configured probe chain find the correspondig tag chain in the event
  chain_tag = GetTagChain( chain->name(), chains );
  
  if ( chain_tag == 0 ) return rois; 

  /// resetting the chains
  m_chain = 0;
  m_chain_tnp = 0;

  /// setting the chains
  SetChains( chain, chain_tag );

  /// find the probe RoIs
  if ( !FindProbes() ) return rois;

  /// getting the vector of rois to process
  rois = GetProbes();

  return rois;
}

// ------------------------------------------------------------------

std::vector<TIDA::Roi*> TagNProbe::GetRois( std::vector<TIDA::Chain>& chains ) {

  std::vector<TIDA::Roi*> rois;
  FindTIDAChains(chains) ;

  /// find the probe RoIs
  if ( !FindProbes() ) return rois;

  /// getting the vector of rois to process
  rois = GetProbes();

  return rois;
}

// ------------------------------------------------------------------

void TagNProbe::BookMinvHisto( std::string chain_name ) {

  std::string hname_base = chain_name;
  std::replace( hname_base.begin(), hname_base.end(), '/', '_' );
  std::replace( hname_base.begin(), hname_base.end(), ':', '_' );

  std::string hname_1 = hname_base + "_Minv_TnP";
  std::string hname_2 = hname_base + "_Minv_obj_TnP";

  m_hMinv_map[ chain_name ] = new TH1D( hname_1.c_str(), hname_1.c_str(), 320, 0, 200 );
  m_hMinv_obj_map[ chain_name ] = new TH1D( hname_2.c_str(), hname_2.c_str(), 320, 0, 200 );

}

// ------------------------------------------------------------------

void TagNProbe::BookMinvHisto( ) {

  m_hMinv = new TH1D( "Minv_TnP", "Tag&Probe invariant mass", 320, 0, 200 );
  m_hMinv_obj = new TH1D( "Minv_obj_TnP", "Tag&Probe invariant mass (object-based)", 320, 0, 200 );

}

// ------------------------------------------------------------------

void TagNProbe::FillMinvHisto( std::string chain_name, unsigned int probe_index ) {

  /// find the histogram for chain_name
  std::map<std::string,TH1D*>::iterator hMinv_itr = m_hMinv_map.find( chain_name );
  std::map<std::string,TH1D*>::iterator hMinv_obj_itr = m_hMinv_obj_map.find( chain_name );

  /// check if histod exist, if yes fill them
  if ( hMinv_itr != m_hMinv_map.end() &&  
       hMinv_obj_itr != m_hMinv_obj_map.end() &&
       m_masses[probe_index].size() == m_masses_obj[probe_index].size() ) {

    //    for ( size_t im=0 ; im<m_masses[probe_index].size() ; im++ ) { 
    //      hMinv_itr->second->Fill( m_masses[probe_index].at(im) );
    //      hMinv_obj_itr->second->Fill( m_masses_obj[probe_index].at(im) );
    //    }
  }
}

// ------------------------------------------------------------------

void TagNProbe::FillMinvHisto( unsigned int probe_index ) {

  if ( m_masses[probe_index].size() == m_masses_obj[probe_index].size() &&
       m_masses[probe_index].size() > 0 ) {

    //  for ( size_t im=0 ; im<m_masses[probe_index].size() ; im++ ) { 
    //    m_hMinv->Fill( m_masses[probe_index].at(im) );
    //    m_hMinv_obj->Fill( m_masses_obj[probe_index].at(im) );
    //  }
  }
}

// ------------------------------------------------------------------

void TagNProbe::WriteMinvHisto( TDirectory* foutdir ) {

    foutdir->cd();

    std::map<std::string,TH1D*>::iterator hMinv_itr;
    for ( hMinv_itr=m_hMinv_map.begin() ; hMinv_itr!=m_hMinv_map.end() ; hMinv_itr++ ) {

      std::string dirname = hMinv_itr->first;
      std::replace( dirname.begin(), dirname.end(), '/', '_' );
      std::replace( dirname.begin(), dirname.end(), ':', '_' );

      std::string dirpath( foutdir->GetPath() );
      dirpath += dirname;

#if 0
      /// renaming (title and object name) histos
      /// setting output TDirectory
      /// Write is called in rmain.cxx
      hMinv_itr->second->SetTitle( "Tag&Probe invariant mass" );
      hMinv_itr->second->SetName( "Minv_TnP" );
      hMinv_itr->second->SetDirectory( foutdir->GetDirectory( dirpath.c_str() ) );
#endif

      foutdir->cd();
    }

    std::map<std::string,TH1D*>::iterator hMinv_obj_itr;
    for ( hMinv_obj_itr=m_hMinv_obj_map.begin() ; hMinv_obj_itr!=m_hMinv_obj_map.end() ; hMinv_obj_itr++ ) {

      std::string dirname = hMinv_obj_itr->first;
      std::replace( dirname.begin(), dirname.end(), '/', '_' );
      std::replace( dirname.begin(), dirname.end(), ':', '_' );

      std::string dirpath( foutdir->GetPath() );
      dirpath += dirname;

#if 0
      /// renaming (title and object name) histos and
      /// setting output TDirectory
      /// Write is called in rmain.cxx
      hMinv_obj_itr->second->SetTitle( "Tag&Probe invariant mass (object-based)" );
      hMinv_obj_itr->second->SetName( "Minv_obj_TnP" );
      hMinv_obj_itr->second->SetDirectory( foutdir->GetDirectory( dirpath.c_str() ) );
#endif

      foutdir->cd();
    }

}

// ------------------------------------------------------------------

void TagNProbe::FindTIDAChains( std::vector<TIDA::Chain>& chains ) {
  
  for ( int ic=0 ; ic<(int)(chains.size()) ; ++ic ){
    std::string chainConfig = chains[ic].name() ;
    if ( (chainConfig) == m_probeChainName ) m_chain_tnp = &( chains[ic] ) ;
    if ( (chainConfig) == m_tagChainName ) m_chain = &( chains[ic] ) ;
  }
}

