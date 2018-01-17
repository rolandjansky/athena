/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: GetLCDeadMaterial.cxx,v 1.1 2009-03-03 17:30:23 pospelov Exp $
//
// Description: see GetLCDeadMaterial.h
// 
// Environment:
//      Software developed for the ATLAS Detector at CERN LHC
//
// Author List:
//      Gennady Pospelov
//
//-----------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "CaloLocalHadCalib/GetLCDeadMaterial.h"

//---------------
// C++ Headers --
//---------------
#include "StoreGate/StoreGateSvc.h" 
#include "PathResolver/PathResolver.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"

#include "CaloConditions/CaloLocalHadCoeff.h"
#include "CaloLocalHadCalib/CaloLocalHadCoeffHelper.h"
#include "CaloLocalHadCalib/CaloHadDMCoeffData.h"
#include "CaloLocalHadCalib/CaloHadDMCoeffFit.h"
#include "CaloLocalHadCalib/CaloHadDMCoeffMinim.h"
#include "CaloLocalHadCalib/CaloHadDMCoeffCheck.h"

#include "TTree.h"
#include <memory>


//###############################################################################
GetLCDeadMaterial::GetLCDeadMaterial(const std::string& name, 
                           ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_HadDMCoeffInputFile("CaloHadDMCoeff_init_v2.txt"),
    m_HadDMCoeffOutputFile("CaloHadDMCoeff_output.txt"),
    m_reportProfiles("report_CaloHadDMCoeff_profiles.ps"),
    m_reportMinimization("report_CaloHadDMCoeff_minim.ps"),
    m_reportCheck("report_CaloHadDMCoeff_check.ps"),
    m_doFit(true),
    m_doMinimization(false),
    m_doPool(true),
    m_doCheck(true),
    m_isTestbeam(false),
    m_isSingleParticle(true),
    m_NormalizationTypeForFit("Lin"),
    m_NormalizationTypeForMinim("Lin"),
    m_ClassificationType("pdg"),
    m_MaxEventsPerFile(0)
{

  // initial coeffitients
  declareProperty("HadDMCoeffInputFile",m_HadDMCoeffInputFile);
  // output file with coeffitients
  declareProperty("HadDMCoeffOutputFile",m_HadDMCoeffOutputFile);
  // name of file with fitting results
  declareProperty("ReportProfiles",m_reportProfiles);
  // name of file with fitting results
  declareProperty("ReportMinimization",m_reportMinimization);
  // name of file with fitting results
  declareProperty("ReportCheck",m_reportCheck);
  // do TProfile fit
  declareProperty("DoFit",m_doFit);
  // do minimization
  declareProperty("DoMinimization",m_doMinimization);
  // do writing to pool file
  declareProperty("DoPool",m_doPool);
  // do writing to pool file
  declareProperty("DoCheck",m_doCheck);
  // list of files to process
  declareProperty("InputRootFiles",m_inputRootFiles);
  // key to write into pool file
  declareProperty("CorrectionKey",m_key="HadDMCoeff2");
  // H6 combined testbeam flag
  declareProperty("isTestbeam",m_isTestbeam);
  // normalization types
  declareProperty("NormalizationTypeForFit",m_NormalizationTypeForFit);
  declareProperty("NormalizationTypeForMinim",m_NormalizationTypeForMinim);
  // number of maximum events to process in one root files
  declareProperty("MaxEventsPerFile", m_MaxEventsPerFile);

}



/* ****************************************************************************

***************************************************************************** */
GetLCDeadMaterial::~GetLCDeadMaterial()
{

}



/* ****************************************************************************

***************************************************************************** */
StatusCode GetLCDeadMaterial::initialize()
{
  //std::cout << std::endl;
  //std::cout << std::endl;
  //std::cout << "--- GetLCDeadMaterial::initialize() --- " << std::endl;

  /* ********************************************
  default coefficients
  ******************************************** */
  CaloLocalHadCoeffHelper dmHelper;
  std::string fileName = PathResolver::find_file (m_HadDMCoeffInputFile, "DATAPATH");
  std::unique_ptr<CaloLocalHadCoeff> initHadDMCoeff (dmHelper.InitDataFromFile(fileName.c_str()));
  if( !initHadDMCoeff ) {
    ATH_MSG_FATAL( " Error while initializing default dead material coefficients " );
    return StatusCode::FAILURE;
  }
  //dmHelper.PrintData(initHadDMCoeff, std::cout);
  ATH_MSG_INFO( "  Number of dead material areas defined:" << initHadDMCoeff->getSizeAreaSet()
      << ", total number of correction coefficients:" << initHadDMCoeff->getSizeCoeffSet() );

  /* ********************************************
  input chain
  ******************************************** */
  TChain *pChain = new TChain("DeadMaterialTree");
  if( !m_inputRootFiles.size() ) {
    ATH_MSG_FATAL( " Empty vector of input root files! "  );
    return StatusCode::FAILURE;
  }
  for(std::vector<std::string >::iterator it = m_inputRootFiles.begin(); it!=m_inputRootFiles.end(); it++){
    ATH_MSG_INFO( " Adding root file '" <<(*it) << "'"  );
    pChain->Add( (*it).c_str() );
  }

  // pointer to the chain and data in it
  std::auto_ptr<CaloHadDMCoeffData> dmData (new CaloHadDMCoeffData(pChain));
  if(m_ClassificationType == "particleid") {
    ATH_MSG_INFO( "Particle ID em fraction will be used to classify clusters:"  );
    m_isSingleParticle = false;
    dmData->SetClassificationType(CaloHadDMCoeffData::kCLASSIFY_USE_PARTICLEID);
  }
  dmData->SetMaxEventsPerFile(m_MaxEventsPerFile);

  std::unique_ptr<CaloLocalHadCoeff> newHadDMCoeffFit;
  std::unique_ptr<CaloLocalHadCoeff> newHadDMCoeffMinim;

  // TProfile approach (presamplers, material between EMB&TILE, EMEC&HEC)
  // and lookup approach (leakages, unclassified DM energy)
  if(m_doFit) {
    CaloHadDMCoeffFit *dmFit = new CaloHadDMCoeffFit();
    dmFit->SetNormalizationType(m_NormalizationTypeForFit);
    newHadDMCoeffFit = std::unique_ptr<CaloLocalHadCoeff>(dmFit->process(dmData.get(), initHadDMCoeff.get(), m_isSingleParticle, m_isTestbeam));
    if( !newHadDMCoeffFit ) {
      ATH_MSG_FATAL( "Failed in CaloHadDMCoeffFit::process()"  );
      delete dmFit;
      return StatusCode::FAILURE;
    }
    if(m_reportProfiles.size()) dmFit->make_report(m_reportProfiles);
    delete dmFit;
  }

  // minimization approach (material before FCAL)
  if(m_doMinimization) {
    CaloHadDMCoeffMinim *dmMinim = new CaloHadDMCoeffMinim();
    dmMinim->SetNormalizationType(m_NormalizationTypeForMinim);
    if(newHadDMCoeffFit) {
      // to use previous coefficients as initial values
      newHadDMCoeffMinim = std::unique_ptr<CaloLocalHadCoeff>(dmMinim->process(dmData.get(), newHadDMCoeffFit.get(), m_isSingleParticle, m_isTestbeam));
    }else{
      newHadDMCoeffMinim = std::unique_ptr<CaloLocalHadCoeff>(dmMinim->process(dmData.get(), initHadDMCoeff.get(), m_isSingleParticle, m_isTestbeam));
    }
    if( !newHadDMCoeffMinim ) {
      ATH_MSG_FATAL( "Failed in CaloHadDMCoeffMinim::process()"  );
      return StatusCode::FAILURE;
    }
    if(m_reportMinimization.size()) dmMinim->make_report(m_reportMinimization);
    delete dmMinim;
  }

  CaloLocalHadCoeff *result = 0;
  if(newHadDMCoeffMinim) {
    result = new CaloLocalHadCoeff(*newHadDMCoeffMinim);
  }else if(newHadDMCoeffFit) {
    result = new CaloLocalHadCoeff(*newHadDMCoeffFit);
  }else{
    result = new CaloLocalHadCoeff(*initHadDMCoeff);
  }
  // prints coefficients to ASCII file
  if (result) {
    std::ofstream fout;
    fout.open(m_HadDMCoeffOutputFile.c_str());
    dmHelper.PrintData(result, fout);
    fout.close();
  }

  // writing data to the pool
  if(m_doPool) {
    ATH_CHECK( detStore()->record(result,m_key) );
    ATH_CHECK( detStore()->setConst(result) );
  }

  // running toy reconstruction for validation of new dead material constants
  if(m_doCheck) {
    CaloHadDMCoeffCheck *dmCheck = new CaloHadDMCoeffCheck();
    dmCheck->process(dmData.get(), result, m_isSingleParticle, m_isTestbeam);
    dmCheck->make_report(m_reportCheck);
    delete dmCheck;
  }

  // end of the game
  delete pChain;

  if(result && !m_doPool) delete result;

  return StatusCode::SUCCESS;
}



/* ****************************************************************************

***************************************************************************** */
StatusCode GetLCDeadMaterial::finalize()
{
  ATH_MSG_INFO( " Nothing to be done in finalize() method "  );
  return StatusCode::SUCCESS;
}



/* ****************************************************************************

***************************************************************************** */
StatusCode GetLCDeadMaterial::execute()
{
  ATH_MSG_INFO( " Nothing to be done in execute() method "  );
  return StatusCode::SUCCESS;
}

