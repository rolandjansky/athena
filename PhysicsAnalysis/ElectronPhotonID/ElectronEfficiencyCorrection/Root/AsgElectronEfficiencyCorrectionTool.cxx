/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class AthElectronEfficiencyCorrectionTool
   @brief Calculate the egamma scale factors in Athena

   @author Rob Roy Fletcher <rob.fletcher@cern.ch>, Karsten Koeneke
   @date   May 2014
*/

// Include this class's header
#include "ElectronEfficiencyCorrection/AsgElectronEfficiencyCorrectionTool.h"

// STL includes
#include <string>
#include <cfloat>
//#include <climits>
#include <iostream>
#include <limits.h>

// Include the return object
#include "PATCore/TResult.h"
#include "PATCore/PATCoreEnums.h"

// xAOD includes
#include "xAODEgamma/Egamma.h"
#include "xAODEventInfo/EventInfo.h"

#include "PathResolver/PathResolver.h"


// ROOT includes
#include "TSystem.h"



// =============================================================================
// Standard constructor
// =============================================================================
AsgElectronEfficiencyCorrectionTool::AsgElectronEfficiencyCorrectionTool( std::string myname ) : 
  AsgTool(myname),
  m_rootTool(0)
{

  // Create an instance of the underlying ROOT tool
  m_rootTool = new Root::TElectronEfficiencyCorrectionTool( ("T"+(this->name())).c_str() );

  // Declare the needed properties
  declareProperty( "CorrectionFileNameList", m_corrFileNameList,
                   "List of file names that store the correction factors for simulation.");

  declareProperty( "ForceDataType", m_dataTypeOverwrite=-1,
                   "Force the DataType of the electron to specified value (to circumvent problem of incorrect DataType for forward electrons in some old releases)");

  declareProperty( "ResultPrefix",       m_resultPrefix="", "The prefix string for the result");
  declareProperty( "ResultName",         m_resultName="",                "The string for the result");

}

// =============================================================================
// Standard destructor
// =============================================================================
AsgElectronEfficiencyCorrectionTool::~AsgElectronEfficiencyCorrectionTool()
{
  if(finalize().isFailure()){
    ATH_MSG_ERROR ( "Failure in AsgElectronEfficiencyCorrectionTool finalize()");
  }

  if ( m_rootTool ) delete m_rootTool;
}

// =============================================================================
// Athena initialize method
// =============================================================================
StatusCode AsgElectronEfficiencyCorrectionTool::initialize()
{
  // Do some consistency checks
  bool allOK(true);
  if ( m_corrFileNameList.empty() )
    {
      ATH_MSG_ERROR ( "FullSimCorrectionFileNameList is empty! Please configure it properly..." );
      allOK = false;
    }

  // Stop here if the user configuration is wrong
  if ( !allOK ) return StatusCode::FAILURE;


    
    // Resolve the paths to the input files for the full Geant4 simualtion corrections
  for ( unsigned int i=0; i<m_corrFileNameList.size(); ++i )
    { 
      std::string& curFName = m_corrFileNameList[i];
      const char* fname;
      if ( curFName.find("/")==0 || curFName.find("$")==0 || curFName.find(".")==0 || curFName.find(":")!=std::string::npos )
        {
          fname = gSystem->ExpandPathName( curFName.c_str() );
        }
      else
        {
          curFName = PathResolverFindXMLFile( curFName );
          if ( curFName.empty() )
            {
              ATH_MSG_WARNING ( "Could NOT resolve file name " << m_corrFileNameList[i] );
            }
          fname = curFName.c_str();
        }
      m_corrFileNameList[i] = fname;
    }


  // Configure the underlying Root tool
  for ( unsigned int i=0; i<m_corrFileNameList.size(); ++i )
    {
      m_rootTool->addFileName( m_corrFileNameList[i] );
    }

  m_rootTool->setResultPrefix( m_resultPrefix );
  m_rootTool->setResultName( m_resultName );

  // Forward the message level
  bool debug(false);
  if ( this->msgLvl(MSG::VERBOSE) || this->msgLvl(MSG::DEBUG) ){ debug = true; }
  m_rootTool->setDebug(debug);

  // We need to initialize the underlying ROOT TSelectorTool
  if ( 0 == m_rootTool->initialize() )
    {
      ATH_MSG_ERROR("Could not initialize the TElectronEfficiencyCorrectionTool!");
      return StatusCode::FAILURE;
    }

  // Copy the now filled TResult to the dummy
  m_resultDummy = m_rootTool->getTResult();

  return StatusCode::SUCCESS ;
}


// =============================================================================
// Athena finalize method
// =============================================================================
StatusCode AsgElectronEfficiencyCorrectionTool::finalize()
{
  if ( !(m_rootTool->finalize()) )
    {
      ATH_MSG_ERROR("Something went wrong at finalize!");
      return StatusCode::FAILURE;
    }

  return StatusCode::SUCCESS ;
}


// =============================================================================
// The main accept method: the actual cuts are applied here 
// =============================================================================
const Root::TResult& AsgElectronEfficiencyCorrectionTool::calculate( const xAOD::Egamma* egam ) const
{
  if ( !egam )
    {
      ATH_MSG_ERROR ( "Did NOT get a valid egamma pointer!" );
      return m_resultDummy;
    }
  
  // Here, you have to make the translation from the ATLAS EDM object to the variables of simple type

  // Get the run number 

  const xAOD::EventInfo* eventInfo = evtStore()->retrieve< const xAOD::EventInfo> ("EventInfo");
  if(!eventInfo){
    ATH_MSG_ERROR ( "Could not retrieve EventInfo object!" );
    return m_resultDummy;
  }
  const unsigned int runnumber = eventInfo->runNumber();

  // Get the needed values from the egamma object
  double cluster_eta(-9999.9);
  double et(0.0);

  const xAOD::CaloCluster* cluster = egam->caloCluster();
  if ( cluster ) {
      cluster_eta = cluster->eta(); // Not etaS2????

      if (cluster_eta != 0.0) {
        et = cluster->e()/cosh(cluster_eta); 
      }
    }

  // Get the DataType of the current egamma object
//!  PATCore::ParticleDataType::DataType dataType = (PATCore::ParticleDataType::DataType) (egam->dataType());
//!  ATH_MSG_VERBOSE( "The egamma object with author=" << egam->author()
//!                   << " has PATCore::ParticleDataType::DataType=" << dataType
//!                   << " and EventKernel::ParticleDataType::DataType=" << egam->dataType() );
  /* For now the dataType must be set by the user. May be added to the IParticle class later.  */
  PATCore::ParticleDataType::DataType dataType = PATCore::ParticleDataType::DataType::Data;
  if ( m_dataTypeOverwrite >= 0 ) dataType = (PATCore::ParticleDataType::DataType)m_dataTypeOverwrite;

      
  // Call the ROOT tool to get an answer
  return m_rootTool->calculate( dataType,
                                runnumber,
                                cluster_eta,
                                et /* in MeV */
                                );
  
}



