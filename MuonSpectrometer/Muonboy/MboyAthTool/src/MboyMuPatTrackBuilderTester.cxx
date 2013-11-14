/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "MboyAthToolInterfaces/IMboyAthToolHelper.h"

/////////////////////////////////////////////////////////
#include "MboyAthTool/MboyMuPatTrackBuilderTester.h"

MboyMuPatTrackBuilderTester::MboyMuPatTrackBuilderTester(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
, p_OwnEDMHelper                  ( "MboyAthToolHelper" )
{

  declareProperty("MboyAthToolHelper"                 , p_OwnEDMHelper          ) ;

//Declare the properties

  declareProperty( "spectroTrackLocation" , m_spectroTrackLocation = "MuonSpectrometerTracks"                ) ;
  declareProperty( "spectroPartiLocation" , m_spectroPartiLocation = "MuonSpectrometerParticles"             ) ;
  declareProperty( "extrapTrackLocation"  , m_extrapTrackLocation  = "ExtrapolatedMuonSpectrometerTracks"    ) ;
  declareProperty( "extrapPartiLocation"  , m_extrapPartiLocation  = "ExtrapolatedMuonSpectrometerParticles" ) ;

  declareProperty( "MboyDumpMboyBackTrackerOutputFilespectroTrackLocation" , m_MboyDumpMboyBackTrackerOutputFilespectroTrackLocation = "Out.MboyDumpMboyBackTrackerOutputFilespectroTrackLocation" ) ;      
  declareProperty( "MboyDumpMboyBackTrackerOutputFilespectroPartiLocation" , m_MboyDumpMboyBackTrackerOutputFilespectroPartiLocation = "Out.MboyDumpMboyBackTrackerOutputFilespectroPartiLocation" ) ;      
  declareProperty( "MboyDumpMboyBackTrackerOutputFileextrapTrackLocation"  , m_MboyDumpMboyBackTrackerOutputFileextrapTrackLocation  = "Out.MboyDumpMboyBackTrackerOutputFileextrapTrackLocation"  ) ;      
  declareProperty( "MboyDumpMboyBackTrackerOutputFileextrapPartiLocation"  , m_MboyDumpMboyBackTrackerOutputFileextrapPartiLocation  = "Out.MboyDumpMboyBackTrackerOutputFileextrapPartiLocation"  ) ;      

}
MboyMuPatTrackBuilderTester::~MboyMuPatTrackBuilderTester(){}


// Finalize
StatusCode MboyMuPatTrackBuilderTester::finalize(){return StatusCode::SUCCESS;}

// Initialize
StatusCode MboyMuPatTrackBuilderTester::initialize(){

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  ATH_MSG_INFO( "================================" );
  ATH_MSG_INFO( "=Proprieties are        " );
  ATH_MSG_INFO( "= spectroTrackLocation                                  " << m_spectroTrackLocation );
  ATH_MSG_INFO( "= spectroPartiLocation                                  " << m_spectroPartiLocation );
  ATH_MSG_INFO( "= extrapTrackLocation                                   " << m_extrapTrackLocation  );
  ATH_MSG_INFO( "= extrapPartiLocation                                   " << m_extrapPartiLocation  );
  ATH_MSG_INFO( "= MboyDumpMboyBackTrackerOutputFilespectroTrackLocation " << m_MboyDumpMboyBackTrackerOutputFilespectroTrackLocation );
  ATH_MSG_INFO( "= MboyDumpMboyBackTrackerOutputFilespectroPartiLocation " << m_MboyDumpMboyBackTrackerOutputFilespectroPartiLocation );
  ATH_MSG_INFO( "= MboyDumpMboyBackTrackerOutputFileextrapTrackLocation  " << m_MboyDumpMboyBackTrackerOutputFileextrapTrackLocation  );
  ATH_MSG_INFO( "= MboyDumpMboyBackTrackerOutputFileextrapPartiLocation  " << m_MboyDumpMboyBackTrackerOutputFileextrapPartiLocation  );
  ATH_MSG_INFO( "================================" );

//Retrieve p_OwnEDMHelper
  if ( p_OwnEDMHelper.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_OwnEDMHelper ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_OwnEDMHelper ) ;

//Set files for dumps for checks
  m_OutFile_MboyDumpMboyBackTrackerOutputFilespectroTrackLocation.open(m_MboyDumpMboyBackTrackerOutputFilespectroTrackLocation.c_str()) ; m_OutFile_MboyDumpMboyBackTrackerOutputFilespectroTrackLocation << setiosflags(std::ios::fixed);
  m_OutFile_MboyDumpMboyBackTrackerOutputFilespectroPartiLocation.open(m_MboyDumpMboyBackTrackerOutputFilespectroPartiLocation.c_str()) ; m_OutFile_MboyDumpMboyBackTrackerOutputFilespectroPartiLocation << setiosflags(std::ios::fixed);
  m_OutFile_MboyDumpMboyBackTrackerOutputFileextrapTrackLocation.open( m_MboyDumpMboyBackTrackerOutputFileextrapTrackLocation.c_str() ) ; m_OutFile_MboyDumpMboyBackTrackerOutputFileextrapTrackLocation  << setiosflags(std::ios::fixed);
  m_OutFile_MboyDumpMboyBackTrackerOutputFileextrapPartiLocation.open( m_MboyDumpMboyBackTrackerOutputFileextrapPartiLocation.c_str() ) ; m_OutFile_MboyDumpMboyBackTrackerOutputFileextrapPartiLocation  << setiosflags(std::ios::fixed);

  ATH_MSG_INFO( "Initialisation ended     " );

  return StatusCode::SUCCESS;

}

// Execute
StatusCode MboyMuPatTrackBuilderTester::execute(){

  StatusCode sc = StatusCode::SUCCESS;

  sc = p_OwnEDMHelper->MboyDumpTrackCollectionToolDoDump(m_spectroTrackLocation,&m_OutFile_MboyDumpMboyBackTrackerOutputFilespectroTrackLocation);
  if ( sc.isFailure() ) ATH_MSG_WARNING( "MboyDumpTrackCollectionToolDoDump failed on " << m_spectroTrackLocation);
  
  sc = p_OwnEDMHelper->MboyDumpTrackCollectionToolDoDump(m_extrapTrackLocation,&m_OutFile_MboyDumpMboyBackTrackerOutputFileextrapTrackLocation);
  if ( sc.isFailure() ) ATH_MSG_WARNING( "MboyDumpTrackCollectionToolDoDump failed on " << m_extrapTrackLocation);

  sc = p_OwnEDMHelper->MboyDumpTrackParticleContainerToolDoDump(m_spectroPartiLocation,&m_OutFile_MboyDumpMboyBackTrackerOutputFilespectroPartiLocation);
  if ( sc.isFailure() ) ATH_MSG_WARNING( "MboyDumpTrackCollectionToolDoDump failed on " << m_spectroPartiLocation);
  
  sc = p_OwnEDMHelper->MboyDumpTrackParticleContainerToolDoDump(m_extrapPartiLocation,&m_OutFile_MboyDumpMboyBackTrackerOutputFileextrapPartiLocation);
  if ( sc.isFailure() ) ATH_MSG_WARNING( "MboyDumpTrackCollectionToolDoDump failed on " << m_extrapPartiLocation);

  return StatusCode::SUCCESS;

}
