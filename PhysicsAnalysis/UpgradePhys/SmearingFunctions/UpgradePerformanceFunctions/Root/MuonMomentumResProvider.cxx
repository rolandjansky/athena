/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "UpgradePerformanceFunctions/MuonMomentumResProvider.h"
#include "PathResolver/PathResolver.h"
#include <fstream>

MuonMomentumResProvider::MuonMomentumResProvider(const std::string & name): 
    MuonCalibrationAndSmearingTool(name){
    declareProperty("UpgradeResolutionRelease",m_upgradeResolutionRelease="Step1p6_170520");
}
double MuonMomentumResProvider::getExpMsResolution(double pt, double eta, double phi){
    int m_detRegion = GetRegion(eta,phi);
    /** do the average with the EXPECTED resolutions **/
    if ( m_detRegion<0 || m_detRegion>=m_nb_regions ) return 0;
    // std::cout << "eta = "<<eta<<" -  Region "<<m_detRegion<<std::endl;
    double expRes = 0.;
    if (pt == 0) return 1e12;
    double p0 = m_MC_p0_MS[m_detRegion] ;
    double p1 = m_MC_p1_MS[m_detRegion] ;
    double p2 = m_MC_p2_MS[m_detRegion] ;
    expRes =  sqrt( pow( p0/pt, 2 ) + pow( p1, 2 ) + pow( p2*pt ,2 ) );
    return expRes; 
}
double MuonMomentumResProvider::getExpItkResolution(double pt, double eta, double phi){
    int m_detRegion = GetRegion(eta,phi);
    /** do the average with the EXPECTED resolutions **/
    if ( m_detRegion<0 || m_detRegion>=m_nb_regions ) return 0;
    // std::cout << "eta = "<<eta<<" -  Region "<<m_detRegion<<std::endl;
    double expRes = 0.;
    if (pt == 0) return 1e12;
    double p1 = m_MC_p1_ID[m_detRegion] ;
    double p2 = m_MC_p2_ID[m_detRegion] ;
    expRes =  sqrt( pow( p1, 2 ) + pow( p2*pt ,2 ) );
    return expRes; 
}
StatusCode MuonMomentumResProvider::initialize(){
   
  //::: Greet the user:
  ATH_MSG_INFO( "Initialising..." );

  if( SetData( m_year ) == StatusCode::FAILURE ) return StatusCode::FAILURE;
  if( SetAlgorithm( m_algo ) == StatusCode::FAILURE ) return StatusCode::FAILURE;
  if( SetRelease( m_release ) == StatusCode::FAILURE ) return StatusCode::FAILURE;
  if( SetType( m_type ) == StatusCode::FAILURE ) return StatusCode::FAILURE;
  
  std::string regionsPath;
  int regionMode = 0; // simple Eta Bins
  regionsPath = PathResolverFindCalibFile( "UpgradePerformanceFunctions/MuonResRegions.dat" );
  ATH_MSG_DEBUG( "Checking Initialization - Regions file: " << regionsPath );
  if( Regions( regionsPath, regionMode ) == StatusCode::FAILURE ) return StatusCode::FAILURE;


    // now override the expected res based on UPF
    // first, clean up the vectors!
    m_MC_p1_ID.clear();
    m_MC_p2_ID.clear();
    m_MC_p2_ID_TAN.clear();
    m_MC_p0_MS.clear();
    m_MC_p1_MS.clear();
    m_MC_p2_MS.clear();
    // and open the ITk specific calib file
    std::string mc_val = PathResolverFindCalibFile( "UpgradePerformanceFunctions/"+m_upgradeResolutionRelease );
    ATH_MSG_DEBUG( "Checking Files - MC: " << mc_val );
    ATH_MSG_INFO("Trying to acquire Upgrade-specific muon resolutions values from "<<mc_val);
    std::ifstream InValues( mc_val.c_str() );
    int i = 0;
    double tmpval = 0;
    std::string tmpname;
    if( !InValues.good() ) {
    ATH_MSG_ERROR( "File " << mc_val << " not found!!" );
        return StatusCode::FAILURE;
    }
    else {
        while( InValues.good() && i<m_nb_regions ) {
          tmpval = 0;
          if( i == 0 ) {
            getline( InValues,tmpname );
          }
          InValues>>tmpval;
          m_MC_p1_ID.push_back( tmpval );
          InValues>>tmpval;
          m_MC_p2_ID.push_back( tmpval );
          InValues>>tmpval;
          m_MC_p2_ID_TAN.push_back( tmpval );
          InValues>>tmpval;
          m_MC_p0_MS.push_back( tmpval );
          InValues>>tmpval;
          m_MC_p1_MS.push_back( tmpval );
          InValues>>tmpval;
          m_MC_p2_MS.push_back( tmpval );
        }
    }
    InValues.close();
    InValues.clear();
    return StatusCode::SUCCESS;
}
