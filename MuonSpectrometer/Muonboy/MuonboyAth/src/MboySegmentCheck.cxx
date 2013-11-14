/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "MboyAthToolInterfaces/IMboyDumpNativeSegmentTool.h"

/////////////////////////////////////////////////////////
#include "MuonboyAth/MboySegmentCheck.h"

MboySegmentCheck::MboySegmentCheck(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
p_IMboyDumpNativeSegmentTool            ( "MboyDumpNativeSegmentTool"          ) 
{

  declareProperty("MboyDumpNativeSegmentTool"        , p_IMboyDumpNativeSegmentTool        ) ;

//Declare the properties

  declareProperty( "SegmentStoreLocation"             , m_SegmentStoreLocation             = "Muonboy_SegmentStore" ) ;
  declareProperty( "NameOfMboySegmentCheckOutputFile" , m_NameOfMboySegmentCheckOutputFile = "Out.MboySegmentCheck" ) ;

  declareProperty( "LetCSCGo"                         , m_LetCSCGo                         = 1                      ) ;
  
  declareProperty( "ApplyQualityFactorCut"            , m_ApplyQualityFactorCut            = 1                      ) ;
  declareProperty( "QualityFactorCut"                 , m_QualityFactorCut                 = 9.                     ) ;
  
  declareProperty( "ApplyNberofDigitsCut"             , m_ApplyNberofDigitsCut             = 1                      ) ;
  declareProperty( "NberofDigitsCut"                  , m_NberofDigitsCut                  = 3                      ) ;
  
  declareProperty( "ApplyNberofPrecisonDigitsCut"     , m_ApplyNberofPrecisonDigitsCut     = 1                      ) ;
  declareProperty( "NberofPrecisonDigitsCut"          , m_NberofPrecisonDigitsCut          = 3                      ) ;

}
MboySegmentCheck::~MboySegmentCheck(){}

// Initialize
StatusCode MboySegmentCheck::initialize(){

  ATH_MSG_INFO(  "Initialisation started     " ) ;
  ATH_MSG_INFO(  "================================" ) ;
  ATH_MSG_INFO(  "=Proprieties are     " ) ;
  ATH_MSG_INFO(  "= SegmentStoreLocation              " << m_SegmentStoreLocation ) ;
  ATH_MSG_INFO(  "= NameOfMboySegmentCheckOutputFile  " << m_NameOfMboySegmentCheckOutputFile ) ;
  ATH_MSG_INFO(  "= LetCSCGo                          " << m_LetCSCGo ) ;
  ATH_MSG_INFO(  "= ApplyQualityFactorCut             " << m_ApplyQualityFactorCut ) ;
  ATH_MSG_INFO(  "= QualityFactorCut                  " << m_QualityFactorCut      ) ;
  ATH_MSG_INFO(  "= ApplyNberofDigitsCut              " << m_ApplyNberofDigitsCut  ) ;
  ATH_MSG_INFO(  "= NberofDigitsCut                   " << m_NberofDigitsCut       ) ;
  ATH_MSG_INFO(  "= ApplyNberofPrecisonDigitsCut      " << m_ApplyNberofPrecisonDigitsCut ) ;
  ATH_MSG_INFO(  "= NberofPrecisonDigitsCut           " << m_NberofPrecisonDigitsCut      ) ;
  ATH_MSG_INFO(  "================================" ) ;

//Retrieve p_IMboyDumpNativeSegmentTool
  if ( p_IMboyDumpNativeSegmentTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL(  "Failed to retrieve tool " << p_IMboyDumpNativeSegmentTool ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO(  "Retrieved tool " << p_IMboyDumpNativeSegmentTool ) ;


//Set the file to dump the tracks
  m_OutMboySegmentCheck.open(m_NameOfMboySegmentCheckOutputFile.c_str());
  m_OutMboySegmentCheck << setiosflags(std::ios::fixed);


  ATH_MSG_INFO(  "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}

// Finalize
StatusCode MboySegmentCheck::finalize(){return StatusCode::SUCCESS;}


// Execute
StatusCode MboySegmentCheck::execute()
{
 return p_IMboyDumpNativeSegmentTool->dumpNativeSegments(
                          m_LetCSCGo                     ,
                          m_ApplyQualityFactorCut        , 
                          m_QualityFactorCut             ,
                          m_ApplyNberofDigitsCut         ,
                          m_NberofDigitsCut              ,
                          m_ApplyNberofPrecisonDigitsCut ,
                          m_NberofPrecisonDigitsCut      ,
                          m_SegmentStoreLocation   ,
                          &m_OutMboySegmentCheck 
                         );
}
