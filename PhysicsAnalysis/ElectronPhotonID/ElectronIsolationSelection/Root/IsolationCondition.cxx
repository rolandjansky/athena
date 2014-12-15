/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IsolationCondition.cxx 635994 2014-12-15 14:37:46Z morrisj $

#include "ElectronIsolationSelection/IsolationCondition.h"

#include <TF1.h>
#include <TH1.h>
#include <TH3.h>
#include <TString.h>
#include <cmath>
#include <sstream>

#include <iostream>
using std::cout;
using std::endl;

namespace CP
{
  IsolationCondition::IsolationCondition( xAOD::Iso::IsolationType type , const float& isLessThan , const bool& divideByPt ) :
    m_isolationType(type),
    m_cutBasedIsolation(true),
    m_divideByPt(divideByPt),    
    m_isLessThanEqualTo(isLessThan),
    
    m_isolationFunctionName("null"),
    m_efficiencyHisto3D(nullptr),
    m_etBins(nullptr),
    m_etaBins(nullptr),       
    m_isolationFunction(nullptr),
    
    m_acceptName(""),
    m_cutPositionInLocalTAccept(0),
    m_cutPositionInElectronIsolationSelectionToolTAccept(0),
    m_accept( "IsolationCondition" )
  {
    this->setupTAccept();
  }
  
  IsolationCondition::IsolationCondition( xAOD::Iso::IsolationType type , const std::string& isolationFunction , const std::shared_ptr<TH3D> efficiencyHisto3D ,
                                          const std::shared_ptr<TH1D> etBins, const std::shared_ptr<TH1D> etaBins ) :
    m_isolationType(type),
    m_cutBasedIsolation(false),
    m_divideByPt(false),    
    m_isLessThanEqualTo(false),
    
    m_isolationFunctionName(isolationFunction),
    m_efficiencyHisto3D(efficiencyHisto3D),
    m_etBins(etBins),
    m_etaBins(etaBins),       
    m_isolationFunction(nullptr), 
    
    m_acceptName(""),
    m_cutPositionInLocalTAccept(0),
    m_cutPositionInElectronIsolationSelectionToolTAccept(0),
    m_accept( "IsolationCondition" )  
  {
    m_isolationFunction = std::shared_ptr<TF1>( new TF1( TString(m_isolationFunctionName) , TString(m_isolationFunctionName) , 0.0 , 1000.0 ) );
    this->setupTAccept();
  }
  
  void IsolationCondition::setupTAccept()
  {
    m_accept.clear();
    
    std::stringstream name;
    name << "ElectronIsolation_";
    if( m_isolationType == xAOD::Iso::etcone20     ){name << "etcone20_";    }
    if( m_isolationType == xAOD::Iso::etcone30     ){name << "etcone30_";    }  
    if( m_isolationType == xAOD::Iso::etcone40     ){name << "etcone40_";    }      
    if( m_isolationType == xAOD::Iso::topoetcone20 ){name << "topoetcone20_";}
    if( m_isolationType == xAOD::Iso::topoetcone30 ){name << "topoetcone30_";}
    if( m_isolationType == xAOD::Iso::topoetcone40 ){name << "topoetcone40_";}
    if( m_isolationType == xAOD::Iso::ptcone20     ){name << "ptcone20_";    }
    if( m_isolationType == xAOD::Iso::ptcone30     ){name << "ptcone30_";    }
    if( m_isolationType == xAOD::Iso::ptcone40     ){name << "ptcone40_";    }
    
    if( m_cutBasedIsolation ){
      name << "le_";
      name << m_isLessThanEqualTo;
      name << "MeV";
    }
    if( !m_cutBasedIsolation ){
      name << "fn_";
      name << m_isolationFunctionName;
    }
    
    m_acceptName = name.str();    
    m_accept.addCut( m_acceptName , m_acceptName );
    m_cutPositionInLocalTAccept = m_accept.getCutPosition( m_acceptName );
  }

  bool IsolationCondition::checkValidity() const
  {
    // Is the isolation type supported
    if( m_isolationType == xAOD::Iso::etcone20 ){return true;}
    if( m_isolationType == xAOD::Iso::etcone30 ){return true;}    
    if( m_isolationType == xAOD::Iso::etcone40 ){return true;}        
    if( m_isolationType == xAOD::Iso::topoetcone20 ){return true;}
    if( m_isolationType == xAOD::Iso::topoetcone30 ){return true;}
    if( m_isolationType == xAOD::Iso::topoetcone40 ){return true;}
    if( m_isolationType == xAOD::Iso::ptcone20 ){return true;}
    if( m_isolationType == xAOD::Iso::ptcone30 ){return true;}
    if( m_isolationType == xAOD::Iso::ptcone40 ){return true;}
    
    // If not supported, return false
    return false;
  }
  
  Root::TAccept& IsolationCondition::accept( const xAOD::Electron& x ) const
  {
    m_accept.clear();
    
    float isoVariableValue(-999.9);
    x.isolationValue( isoVariableValue , m_isolationType );
    
    if( m_cutBasedIsolation ){
      
      if( m_divideByPt ){
        
        float pT = x.caloCluster()->e() / cosh( x.trackParticle()->eta() );
                
        if( ( isoVariableValue / pT ) <= m_isLessThanEqualTo ){
          m_accept.setCutResult( m_cutPositionInLocalTAccept , true );
          return m_accept;
        }
      }
      
      if( !m_divideByPt ){
        if( isoVariableValue <= m_isLessThanEqualTo ){
          m_accept.setCutResult( m_cutPositionInLocalTAccept , true );
          return m_accept;
        }
      }
      
    }
    
    if( !m_cutBasedIsolation ){
      float pT = x.caloCluster()->e() / cosh( x.trackParticle()->eta() );
      float efficiencyFromFunction = m_isolationFunction->Eval( pT / 1000.0 );      
      int efficiencyBin = floor( 100.0 * efficiencyFromFunction );
      
      if( efficiencyBin < 70 ){
        return m_accept;
      }
      
      if( efficiencyBin > 99 ){
        m_accept.setCutResult( m_cutPositionInLocalTAccept , true );
        return m_accept;
      }
      
      float cutValue = m_efficiencyHisto3D->GetBinContent(this->findEtBin( pT ),
                                                          this->findEtaBin( x.caloCluster()->eta() ),
                                                          efficiencyBin);
    
      if( ( isoVariableValue / 1000.0 ) <= cutValue ){
        m_accept.setCutResult( m_cutPositionInLocalTAccept , true );
        return m_accept;
      }     
    }
    
    return m_accept;
  }
  
  // Get the cut value used
  float IsolationCondition::getCutValue( const xAOD::Electron& x ) const
  {
    if( m_cutBasedIsolation ){
      return m_isLessThanEqualTo;
    }
    if( !m_cutBasedIsolation ){
      float pT = x.caloCluster()->e() / cosh( x.trackParticle()->eta() );
      float efficiencyFromFunction = m_isolationFunction->Eval( pT / 1000.0 );      
      int efficiencyBin = floor( 100.0 * efficiencyFromFunction ); 
      if( efficiencyBin < 70 ){
        return 12345.6;
      }
      if( efficiencyBin > 99  ){
        return -12345.6;
      }
      
      return m_efficiencyHisto3D->GetBinContent(this->findEtBin( pT ),
                                                this->findEtaBin( x.caloCluster()->eta() ),
                                                efficiencyBin);
    }
    return 12345.6;
  }
  
  int IsolationCondition::findEtBin(const float& Et) const
  {
    int bin = m_etBins->FindBin(Et / 1000.0 );
    if( bin > 13 ){bin = 13;}
    return bin;
  }
  
  int IsolationCondition::findEtaBin(const float& Eta) const
  {
    return m_etaBins->FindBin(Eta);
  }
      
}