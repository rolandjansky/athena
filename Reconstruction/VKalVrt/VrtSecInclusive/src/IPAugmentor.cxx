/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VrtSecInclusive/IPAugmentor.h"

#include <string>
#include <iostream>
#include <functional>

using namespace std;

namespace Trk {
  extern 
  int  pgraphm_(
		long int *weit, long int *edges, long int *nodes,
		long int *set, long int *nptr,  long int *nth);
}

namespace VKalVrtAthena {
  
  //Constructor and destructor
  //__________________________________________________________________________
  IPAugmentor::IPAugmentor(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm                   (name,pSvcLocator),
    m_fitSvc                       ( "Trk::TrkVKalVrtFitter", this )
  {
    this->declareProperties();
  }
  
  

  //__________________________________________________________________________
  IPAugmentor::~IPAugmentor()
  { 
    ATH_MSG_DEBUG("destructor called");
  }



  //__________________________________________________________________________
  StatusCode IPAugmentor::beginRun()  { 

    ATH_MSG_DEBUG("beginRun()");

    return StatusCode::SUCCESS;
  }
  
  
  
  //__________________________________________________________________________
  StatusCode IPAugmentor::initialize()
  {

    //  VKalVrt vertex fitter
    ATH_CHECK( m_fitSvc.retrieve() );

    ATH_MSG_DEBUG("initialize: Exit IPAugmentor::initialize()");
    return StatusCode::SUCCESS; 
  }



  //__________________________________________________________________________
  StatusCode IPAugmentor::finalize()
  {
    
    ATH_MSG_DEBUG("finalize: IPAugmentor finalize()");
    return StatusCode::SUCCESS; 
  }

  //__________________________________________________________________________
  StatusCode IPAugmentor::execute()
  {
    
    ATH_MSG_DEBUG("IPAugmentor execute()");
    
    const xAOD::VertexContainer *secondaryVertexContainer { nullptr };
    StatusCode sc = evtStore()->retrieve( secondaryVertexContainer,    "VrtSecInclusive_" + m_jp.secondaryVerticesContainerName + m_jp.augVerString );
    if( sc.isFailure() ) {
      ATH_MSG_WARNING( __FUNCTION__ << ": VSI container " << ("VrtSecInclusive_" + m_jp.secondaryVerticesContainerName + m_jp.augVerString) << " not found!" );
      return StatusCode::SUCCESS;
    }
    
    try {

      if( m_jp.doAugmentDVimpactParametersToMuons     ) { ATH_CHECK( augmentDVimpactParametersToLeptons<xAOD::Muon>    ( "Muons"     ) ); }
      if( m_jp.doAugmentDVimpactParametersToElectrons ) { ATH_CHECK( augmentDVimpactParametersToLeptons<xAOD::Electron>( "Electrons" ) ); }
    
    } catch(...) {
      
      ATH_MSG_WARNING( " > " << __FUNCTION__ << ": some other error is detected in the vertexing scope."  );
      
      return StatusCode::SUCCESS;
      
    }
    
    ATH_MSG_VERBOSE( "execute: process done." );
    
    return StatusCode::SUCCESS;   
    
  }
    
  //____________________________________________________________________________________________________
  void IPAugmentor::declareProperties() {
    
    declareProperty("AugmentingVersionString",         m_jp.augVerString                    = ""                            );
    declareProperty("SecondaryVerticesContainerName",  m_jp.secondaryVerticesContainerName  = "SecondaryVertices"           );

    // Additional dressing option
    declareProperty("doAugmentDVimpactParametersToMuons",     m_jp.doAugmentDVimpactParametersToMuons     = true            );
    declareProperty("doAugmentDVimpactParametersToElectrons", m_jp.doAugmentDVimpactParametersToElectrons = true            );
    
    // Additional ToolHandles
    declareProperty("VertexFitterTool",                m_fitSvc, " Private TrkVKalVrtFitter"                                );
    
  }
  
} // end of namespace bracket
