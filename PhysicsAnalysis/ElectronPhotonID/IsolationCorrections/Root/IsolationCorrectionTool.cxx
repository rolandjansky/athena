/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// xAOD
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/EgammaDefs.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEventInfo/EventInfo.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "TFile.h"

// Header for the ROOT tool of this package 
#include "IsolationCorrections/IsolationCorrectionTool.h"

namespace CP { 

  IsolationCorrectionTool::IsolationCorrectionTool( const std::string &name )
    : asg::AsgTool(name)
  {	
    declareProperty("CorrFile", m_corr_file    = "IsolationCorrections/isolation_ptcorrections_rel17_2.root");
    declareProperty("ToolVer",  m_tool_ver_str = "REL17_2");
    declareProperty("IsMC",     m_is_mc        = true);
    m_isol_corr = new IsolationCorrection();
  }
  
  StatusCode IsolationCorrectionTool::initialize() {
    ATH_MSG_INFO( "in initialize of " << name() << "..." );
    
    CP::IsolationCorrection::Version tool_ver;
    
    if      (m_tool_ver_str == "REL18")   tool_ver = CP::IsolationCorrection::REL18;
    else if (m_tool_ver_str == "REL17_2") tool_ver = CP::IsolationCorrection::REL17_2;
    else if (m_tool_ver_str == "REL17")   tool_ver = CP::IsolationCorrection::REL17;
    else {
      ATH_MSG_WARNING("Tool version not recognized: "<<m_tool_ver_str<<"\nAllowed versions: REL18, REL17_2, REL17");
      return StatusCode::FAILURE;
    }
    
    m_isol_corr->SetDataMC(m_is_mc);
    m_isol_corr->SetCorrectionFile(m_corr_file);
    m_isol_corr->SetToolVer(tool_ver);
    
    return m_isol_corr->initialize();
  }
  
  StatusCode IsolationCorrectionTool::finalize() {
    ATH_MSG_INFO( "in finalize" );
    
    return m_isol_corr->finalize();
  }
  
  CP::CorrectionCode IsolationCorrectionTool::applyCorrection(xAOD::Egamma & input) { 
    
    std::vector<xAOD::Iso::IsolationType> topoisolation_types = {xAOD::Iso::topoetcone20,
								 xAOD::Iso::topoetcone30,
								 xAOD::Iso::topoetcone40};
    
    auto topoisol_it = topoisolation_types.begin();
    
    //ATH_MSG_INFO( "Correcting topoetconeXX isolation..." );
    
    for(; topoisol_it < topoisolation_types.end(); ){   
      float Etcone_value_ptcorr = m_isol_corr->GetPtCorrectedIsolation(input, *topoisol_it);
      input.setIsolationValue(Etcone_value_ptcorr, *topoisol_it);
      ++topoisol_it;
    }
    
    std::vector<xAOD::Iso::IsolationType> isolation_types = {xAOD::Iso::etcone20,
							     xAOD::Iso::etcone30,
							     xAOD::Iso::etcone40};
    
    auto isol_it = isolation_types.begin();
    
    //ATH_MSG_INFO( "Correcting etconeXX isolation..." );
    
    for(; isol_it < isolation_types.end();){    
      float Etcone_value_corr = m_isol_corr->GetPtCorrectedIsolation(input, *isol_it);
      input.setIsolationValue(Etcone_value_corr, *isol_it);
      ++isol_it;
    }
    
    return CP::CorrectionCode::Ok; 
  }
  
  float IsolationCorrectionTool::GetPtCorrectedIsolation(const xAOD::Egamma& input, xAOD::Iso::IsolationType isol){
    return m_isol_corr->GetPtCorrectedIsolation(input, isol);
  }
  float IsolationCorrectionTool::GetPtCorrection(const xAOD::Egamma& input, xAOD::Iso::IsolationType isol){
    return m_isol_corr->GetPtCorrection(input, isol);
  }
  
  CP::CorrectionCode IsolationCorrectionTool::correctedCopy( const xAOD::Egamma& input, xAOD::Egamma*& output) {
    // A sanity check:
    if( output ) ATH_MSG_WARNING( "Non-null pointer received. " "There's a possible memory leak!" );
    
    if ( input.type() == xAOD::Type::Electron ) {
      output = new xAOD::Electron();
      output->makePrivateStore( static_cast<const xAOD::Electron&>(input) );
      return applyCorrection( *output );
    }
    else if ( input.type() == xAOD::Type::Photon ) {
      output = new xAOD::Photon();
      output->makePrivateStore( static_cast<const xAOD::Photon&>(input) );
      return applyCorrection( *output );
    }
    else {
      ATH_MSG_WARNING("Didn't get an electron or photon");
      return CP::CorrectionCode::Error;
    }
    
  }
  
  //Systematics
  bool IsolationCorrectionTool::isAffectedBySystematic( const CP::SystematicVariation& systematic ) const { 
    CP::SystematicSet sys = affectingSystematics();
    return sys.find( systematic ) != sys.end();
  } 
  
  CP::SystematicSet IsolationCorrectionTool::affectingSystematics() const { 
    // TODO: systematic support for now is non-existent
    CP::SystematicSet result;
    
    //scale isolation
    //result.insert( m_scaleSystMap[egEnergyCorr::Scale::MomentumUp] = CP::SystematicVariation("ISO_SCALE_MOMENTUM", 1) );    
    //result.insert( m_scaleSystMap[egEnergyCorr::Scale::MomentumDown] = CP::SystematicVariation("ISO_SCALE_MOMENTUM", -1) );
    
    return result;
  }
  
  CP::SystematicSet IsolationCorrectionTool::recommendedSystematics() const { 
    return affectingSystematics(); 
  } 
  
  CP::SystematicCode IsolationCorrectionTool::applySystematicVariation( const CP::SystematicSet& systConfig ) { 
    
    if(systConfig.size()>0){
      
    }
    return CP::SystematicCode::Ok; 
  }
  
  IsolationCorrectionTool::~IsolationCorrectionTool() {
    delete m_isol_corr;
  }

}
