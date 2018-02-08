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
#include "PathResolver/PathResolver.h"
#include "AthenaKernel/errorcheck.h"

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
  /* ********************************************
  default coefficients
  ******************************************** */
  CaloLocalHadCoeffHelper dmHelper;
  std::string fileName = PathResolver::find_file (m_HadDMCoeffInputFile, "DATAPATH");
  CaloLocalHadCoeff * initHadDMCoeff = dmHelper.InitDataFromFile(fileName.c_str());
  if( !initHadDMCoeff ) {
    msg(MSG::FATAL)
        << " Error while initializing default dead material coefficients "
        << endmsg;
    return StatusCode::FAILURE;
  }
  //dmHelper.PrintData(initHadDMCoeff, std::cout);
  msg(MSG::INFO)
      << "  Number of dead material areas defined:" << initHadDMCoeff->getSizeAreaSet()
      << ", total number of correction coefficients:" << initHadDMCoeff->getSizeCoeffSet()
      << endmsg;

  /* ********************************************
  input chain
  ******************************************** */
  TChain *pChain = new TChain("DeadMaterialTree");
  if( !m_inputRootFiles.size() ) {
    msg(MSG::FATAL) << " Empty vector of input root files! " << endmsg;
    return StatusCode::FAILURE;
  }
  for(std::vector<std::string >::iterator it = m_inputRootFiles.begin(); it!=m_inputRootFiles.end(); it++){
    msg(MSG::INFO) << " Adding root file '" <<(*it) << "'" << endmsg;
    pChain->Add( (*it).c_str() );
  }

  // pointer to the chain and data in it
  std::auto_ptr<CaloHadDMCoeffData> dmData (new CaloHadDMCoeffData(pChain));
  if(m_ClassificationType == "particleid") {
    msg(MSG::INFO) << "Particle ID em fraction will be used to classify clusters:" << endmsg;
    m_isSingleParticle = false;
    dmData->SetClassificationType(CaloHadDMCoeffData::kCLASSIFY_USE_PARTICLEID);
  }
  dmData->SetMaxEventsPerFile(m_MaxEventsPerFile);

  CaloLocalHadCoeff *newHadDMCoeffFit = 0;
  CaloLocalHadCoeff *newHadDMCoeffMinim = 0;

  // TProfile approach (presamplers, material between EMB&TILE, EMEC&HEC)
  // and lookup approach (leakages, unclassified DM energy)
  if(m_doFit) {
    CaloHadDMCoeffFit *dmFit = new CaloHadDMCoeffFit();
    dmFit->SetNormalizationType(m_NormalizationTypeForFit);
    newHadDMCoeffFit = dmFit->process(dmData.get(), initHadDMCoeff, m_isSingleParticle, m_isTestbeam);
    if( !newHadDMCoeffFit ) {
      msg(MSG::FATAL) << "Failed in CaloHadDMCoeffFit::process()" << endmsg;
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
      newHadDMCoeffMinim = dmMinim->process(dmData.get(), newHadDMCoeffFit, m_isSingleParticle, m_isTestbeam);
    }else{
      newHadDMCoeffMinim = dmMinim->process(dmData.get(), initHadDMCoeff, m_isSingleParticle, m_isTestbeam);
    }
    if( !newHadDMCoeffMinim ) {
      msg(MSG::FATAL) << "Failed in CaloHadDMCoeffMinim::process()" << endmsg;
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
    StoreGateSvc* detStore;
    StatusCode sc=service("DetectorStore",detStore);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Unable to get the DetectorStore" << endmsg;
      return sc;
    }
    sc=detStore->record(result,m_key);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Unable to record CaloHadDMCoeff" << endmsg;
      return sc;
    }
    sc=detStore->setConst(result);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Unable to lock CaloHadDMCoeff" << endmsg;
      return sc;
    }
  }

  // running toy reconstruction for validation of new dead material constants
  if(m_doCheck) {
    CaloHadDMCoeffCheck *dmCheck = new CaloHadDMCoeffCheck();
    dmCheck->process(dmData.get(), result, m_isSingleParticle, m_isTestbeam);
    dmCheck->make_report(m_reportCheck);
    delete dmCheck;
  }

  // end of the game
  if(newHadDMCoeffFit) delete newHadDMCoeffFit;
  if(newHadDMCoeffMinim) delete newHadDMCoeffMinim;
  delete initHadDMCoeff;
  delete pChain;

  if(result && !m_doPool) delete result;

  return StatusCode::SUCCESS;
}



/* ****************************************************************************

***************************************************************************** */
StatusCode GetLCDeadMaterial::finalize()
{
  msg(MSG::INFO) << " Nothing to be done in finalize() method " << endmsg;
  return StatusCode::SUCCESS;
}



/* ****************************************************************************

***************************************************************************** */
StatusCode GetLCDeadMaterial::execute()
{
  msg(MSG::INFO) << " Nothing to be done in execute() method " << endmsg;
  return StatusCode::SUCCESS;
}

