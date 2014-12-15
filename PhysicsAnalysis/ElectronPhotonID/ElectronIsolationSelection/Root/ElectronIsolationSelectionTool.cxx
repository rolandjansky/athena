/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronIsolationSelectionTool.cxx 635994 2014-12-15 14:37:46Z morrisj $

// Local include(s):
#include "ElectronIsolationSelection/ElectronIsolationSelectionTool.h"
#include "ElectronIsolationSelection/IsolationCondition.h"

// Framework include(s):
#include "PathResolver/PathResolver.h"

// std include(s):
#include <cmath>
#include <sstream>

// ROOT include(s):
#include <TROOT.h>
#include <TFile.h>
#include <TH1.h>
#include <TH3.h>

namespace CP {

  ElectronIsolationSelectionTool::ElectronIsolationSelectionTool( const std::string& name ) :
    asg::AsgTool( name ),
    m_configFileName("ElectronIsolationSelection/v1/ElectronIsolationCutMap_8TeV_Zee.root"),
    m_configFile(nullptr),
    h_etBins(nullptr),
    h_etaBins(nullptr),
    m_sgKeyEventInfo("EventInfo"),
    m_sgKeyVertex("PrimaryVertices"),
    m_isolationConditions(),
    m_isolationConditionsBackground(),
    m_accept( "ElectronIsolationSelectionTool" ),
    m_acceptBackground( "ElectronIsolationSelectionToolBackground" )
  {       
    declareProperty( "configFileName", m_configFileName = "ElectronIsolationSelection/v1/ElectronIsolationCutMap_8TeV_Zee.root","The config file to use" );
    declareProperty( "SGKeyEventInfo", m_sgKeyEventInfo = "EventInfo" );
    declareProperty( "SGKeyVertex"   , m_sgKeyVertex = "PrimaryVertices" ); 
  }
  

  StatusCode ElectronIsolationSelectionTool::initialize() 
  {
    // Greet the user:
    ATH_MSG_INFO( "Initialising..." );
    
    // Setup internal binning
    this->setupBinning();
    
    // Get input file needed for efficiency based isolation
    std::string filename = PathResolverFindCalibFile( m_configFileName ); 
    
    TString Tfilename( filename );    
    m_configFile = std::shared_ptr<TFile>( new TFile( Tfilename , "READ" ) );
    
    // Clear the results
    m_accept.clear();
    m_acceptBackground.clear();

    // Return gracefully:
    return StatusCode::SUCCESS;
  }
  
  StatusCode ElectronIsolationSelectionTool::finalize()
  {
    m_configFile->Close();
    ATH_MSG_INFO(" Closing file....."<<m_configFileName);
    // Return gracefully:
    return StatusCode::SUCCESS;    
  }
  
  void ElectronIsolationSelectionTool::setupBinning()
  {
    // Histogram binning 
    
    const float ArrayEt[14]={0.0,7.0,10.0,15.0,20.0,25.0,30.0,35.0,40.0,45.0,50.0,60.0,80.0,8000.0};
                              
    const float ArrayEta[21]={-2.47,-2.37,-2.01,-1.81,-1.52,-1.37,-1.15,-0.8,-0.6,-0.1,
                              0.0,0.1,0.6,0.8,1.15,1.37,1.52,1.81,2.01,2.37,2.47};
                             
    h_etBins = std::shared_ptr<TH1D>( new TH1D("ElectronIsolationSelectionTool_etBins","ElectronIsolationSelectionTool_etBins",13,ArrayEt) );
    h_etaBins = std::shared_ptr<TH1D>( new TH1D("ElectronIsolationSelectionTool_etaBins","ElectronIsolationSelectionTool_etaBins",20,ArrayEta) );
    h_etBins->SetDirectory(0);
    h_etaBins->SetDirectory(0);   
  }
  
  ////////////////////////////////////////////////////////////////////
  //   Configure the isolation type                                 //
  ////////////////////////////////////////////////////////////////////
  
  StatusCode ElectronIsolationSelectionTool::configureCutBasedIsolation( xAOD::Iso::IsolationType type , const float& isLessThan , const bool& divideByPt )
  {   
    std::shared_ptr<CP::IsolationCondition> iso = std::shared_ptr<CP::IsolationCondition>( new CP::IsolationCondition( type , isLessThan , divideByPt ) );
    if( ! iso->checkValidity() ){
      ATH_MSG_INFO(" Isolation Condition is invalid");
      return StatusCode::FAILURE;      
    }
  
    m_accept.addCut( iso->getTAcceptName() , iso->getTAcceptName() );
    iso->setCutPositionInElectronIsolationSelectionToolTAccept( m_accept.getCutPosition( iso->getTAcceptName() ) );   
    m_isolationConditions.push_back( iso );
       
    // Return gracefully:
    return StatusCode::SUCCESS;    
  }
  
  StatusCode ElectronIsolationSelectionTool::configureEfficiencyBasedIsolation( xAOD::Iso::IsolationType type , const std::string& isolationFunction )
  {
    // Future versions of the calibration file will have better histogram names
    std::stringstream histoName;
    histoName<< "/TightPP/TightPP_";
    if( type == xAOD::Iso::etcone20     ){histoName << "etconeCor20";   }
    if( type == xAOD::Iso::etcone30     ){histoName << "etconeCor30";   }
    if( type == xAOD::Iso::topoetcone20 ){histoName << "topoconeCor20"; }
    if( type == xAOD::Iso::topoetcone30 ){histoName << "topoconeCor30"; }
    if( type == xAOD::Iso::topoetcone40 ){histoName << "topoconeCor40"; }
    if( type == xAOD::Iso::ptcone20     ){histoName << "ptcone20";      }
    if( type == xAOD::Iso::ptcone30     ){histoName << "ptcone30";      } 
    if( type == xAOD::Iso::ptcone40     ){histoName << "ptcone40";      } 
    
    TString ThistoName( histoName.str() );
    
    std::shared_ptr<TH3D> histo = std::shared_ptr<TH3D>( (TH3D*)m_configFile->Get( ThistoName )->Clone() );
    histo->SetDirectory(0);
     
    std::shared_ptr<CP::IsolationCondition> iso = std::shared_ptr<CP::IsolationCondition>( new CP::IsolationCondition( type , isolationFunction , histo , h_etBins , h_etaBins ) );
    
    if( ! iso->checkValidity() ){
      ATH_MSG_INFO(" Isolation Condition is invalid");
      return StatusCode::FAILURE;      
    }
  
    m_accept.addCut( iso->getTAcceptName() , iso->getTAcceptName() );
    iso->setCutPositionInElectronIsolationSelectionToolTAccept( m_accept.getCutPosition( iso->getTAcceptName() ) );   
    m_isolationConditions.push_back( iso );   
    
    // Return gracefully:
    return StatusCode::SUCCESS;
  }
  
  
  // Configure the isolation type
  StatusCode ElectronIsolationSelectionTool::configureCutBasedIsolationBackground( xAOD::Iso::IsolationType type , const float& isLessThan , const bool& divideByPt )
  {
    
    std::shared_ptr<CP::IsolationCondition> iso = std::shared_ptr<CP::IsolationCondition>( new CP::IsolationCondition( type , isLessThan , divideByPt ) );
    if( ! iso->checkValidity() ){
      ATH_MSG_INFO(" Isolation Condition is invalid");
      return StatusCode::FAILURE;      
    }
  
    m_acceptBackground.addCut( iso->getTAcceptName() , iso->getTAcceptName() );
    iso->setCutPositionInElectronIsolationSelectionToolTAccept( m_acceptBackground.getCutPosition( iso->getTAcceptName() ) );   
    m_isolationConditionsBackground.push_back( iso );
 
    
    // Return gracefully:
    return StatusCode::SUCCESS;    
  }
  
  StatusCode ElectronIsolationSelectionTool::configureEfficiencyBasedIsolationBackground( xAOD::Iso::IsolationType type , const std::string& isolationFunction )
  {
    // Future versions of the calibration file will have better histogram names
    std::stringstream histoName;
    histoName<< "/TightPP/TightPP_";
    if( type == xAOD::Iso::etcone20     ){histoName << "etconeCor20";   }
    if( type == xAOD::Iso::etcone30     ){histoName << "etconeCor30";   }
    if( type == xAOD::Iso::topoetcone20 ){histoName << "topoconeCor20"; }
    if( type == xAOD::Iso::topoetcone30 ){histoName << "topoconeCor30"; }
    if( type == xAOD::Iso::topoetcone40 ){histoName << "topoconeCor40"; }
    if( type == xAOD::Iso::ptcone20     ){histoName << "ptcone20";      }
    if( type == xAOD::Iso::ptcone30     ){histoName << "ptcone30";      } 
    if( type == xAOD::Iso::ptcone40     ){histoName << "ptcone40";      } 
    
    TString ThistoName( histoName.str() );
    
    std::shared_ptr<TH3D> histo = std::shared_ptr<TH3D>( (TH3D*)m_configFile->Get( ThistoName )->Clone() );
    histo->SetDirectory(0);
     
    std::shared_ptr<CP::IsolationCondition> iso = std::shared_ptr<CP::IsolationCondition>( new CP::IsolationCondition( type , isolationFunction , histo , h_etBins , h_etaBins ) );
    
    if( ! iso->checkValidity() ){
      ATH_MSG_INFO(" Isolation Condition is invalid");
      return StatusCode::FAILURE;      
    }
  
    m_acceptBackground.addCut( iso->getTAcceptName() , iso->getTAcceptName() );
    iso->setCutPositionInElectronIsolationSelectionToolTAccept( m_acceptBackground.getCutPosition( iso->getTAcceptName() ) );   
    m_isolationConditionsBackground.push_back( iso );  
    
    // Return gracefully:
    return StatusCode::SUCCESS;
  }  
  
    
  ////////////////////////////////////////////////////////////////////
  //   Accept the electron - Does it pass isolation?                //
  ////////////////////////////////////////////////////////////////////    
   
   const Root::TAccept& ElectronIsolationSelectionTool::accept( const xAOD::Electron& x ) const
   {         
      // Reset the result:
      m_accept.clear();
      if( !this->validEtEtaRange( x ) ){
        return m_accept;
      }
      
      for( auto i : m_isolationConditions ){       
        if( i->accept( x ) ){
          m_accept.setCutResult( i->getCutPositionInElectronIsolationSelectionToolTAccept() , true );
        }
      }
      return m_accept;
   }
   
   const Root::TAccept& ElectronIsolationSelectionTool::acceptBackground( const xAOD::Electron& x ) const
   {     
      // Reset the result:
      m_acceptBackground.clear();
      if( !this->validEtEtaRange( x ) ){
        return m_accept;
      }
      
      for( auto i : m_isolationConditionsBackground ){      
        if( i->accept( x ) ){
          m_acceptBackground.setCutResult( i->getCutPositionInElectronIsolationSelectionToolTAccept() , true );
        }
      }
      return m_acceptBackground;
   }   
   
  ////////////////////////////////////////////////////////////////////
  //   Get the results                                              //
  ////////////////////////////////////////////////////////////////////  

  const Root::TAccept& ElectronIsolationSelectionTool::getTAccept() const 
  {
    return m_accept;
  }
  
  const Root::TAccept& ElectronIsolationSelectionTool::getTAcceptBackground() const 
  {
    return m_acceptBackground;
  } 
  
  ////////////////////////////////////////////////////////////////////
  //   Get the cut values                                           //
  ////////////////////////////////////////////////////////////////////  

  std::shared_ptr<std::map<TString,float>> ElectronIsolationSelectionTool::getCutValues( const xAOD::Electron& x ) const 
  {
    std::shared_ptr<std::map<TString,float>> map = 
    std::shared_ptr<std::map<TString,float>> ( new std::map<TString,float> );
    
    for( auto i : m_isolationConditions ){
      map->insert( std::make_pair( i->getTAcceptName() , i->getCutValue( x ) ) );
    }
    return map;
  }
  
  std::shared_ptr<std::map<TString,float>> ElectronIsolationSelectionTool::getCutValuesBackground( const xAOD::Electron& x ) const 
  {
    std::shared_ptr<std::map<TString,float>> map = 
    std::shared_ptr<std::map<TString,float>> ( new std::map<TString,float> );
    
    for( auto i : m_isolationConditionsBackground ){
      map->insert( std::make_pair( i->getTAcceptName() , i->getCutValue( x ) ) );
    }
    return map;    
  }
  
  ////////////////////////////////////////////////////////////////////
  //   Get a map of all xAOD::Iso::IsolationType in use             //
  ////////////////////////////////////////////////////////////////////
  
  // Get a map of all xAOD::Iso::IsolationType and uncorrected isolation values in use 
  std::shared_ptr<std::map<TString,float>> ElectronIsolationSelectionTool::getIsolationValue( const xAOD::Electron& x ) const
  {
    std::shared_ptr<std::map<TString,float>> map = 
    std::shared_ptr<std::map<TString,float>> ( new std::map<TString,float> ); 
    
    for( auto i : m_isolationConditions ){
      TString name;    
      if( i->isolationType() == xAOD::Iso::etcone20     ){name = "etcone20";     }
      if( i->isolationType() == xAOD::Iso::etcone30     ){name = "etcone30";     }
      if( i->isolationType() == xAOD::Iso::etcone40     ){name = "etcone40";     }     
      if( i->isolationType() == xAOD::Iso::topoetcone20 ){name = "topoetcone20"; }
      if( i->isolationType() == xAOD::Iso::topoetcone30 ){name = "topoetcone30"; }
      if( i->isolationType() == xAOD::Iso::topoetcone40 ){name = "topoetcone40"; }
      if( i->isolationType() == xAOD::Iso::ptcone20     ){name = "ptcone20";     }
      if( i->isolationType() == xAOD::Iso::ptcone30     ){name = "ptcone30";     } 
      if( i->isolationType() == xAOD::Iso::ptcone40     ){name = "ptcone40";     } 
      
      map->insert( std::make_pair( name , this->getIsolationValue( x , i->isolationType() ) ) );
    }
    
    return map;
  }
  
  float ElectronIsolationSelectionTool::getIsolationValue(const xAOD::Electron& x,xAOD::Iso::IsolationType type) const
  {
    float value(-99999.9);
    x.isolationValue( value , type );
    return value;
  }

  
  ////////////////////////////////////////////////////////////////////
  //   Internal member functions                                    //
  ////////////////////////////////////////////////////////////////////  

  bool ElectronIsolationSelectionTool:: validEtEtaRange( const xAOD::Electron& x ) const
  {
    if( this->getElectronClusterEnergy(x) / cosh( this->getElectronTrackEta(x) ) > 10000.0 ){
      float feta = fabs( this->getElectronClusterEta(x) );
      if( feta < 2.47 ){
        if( !(feta > 1.37 && feta < 1.52) ){
          return true;
        }
      }
    }
    return false;
  }
    
  float ElectronIsolationSelectionTool::getElectronClusterEnergy( const xAOD::Electron& x ) const
  {
    return x.caloCluster()->e();
  }
  
  float ElectronIsolationSelectionTool::getElectronClusterEta( const xAOD::Electron& x ) const
  {
    return x.caloCluster()->eta();
  }
    
  float ElectronIsolationSelectionTool::getElectronTrackEta( const xAOD::Electron& x ) const
  {
      return x.trackParticle()->eta();     
  }
          
} // namespace CP
