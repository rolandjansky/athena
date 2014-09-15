/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

//----------------------------------------------------------------//
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"

/////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyRTTool.h"

//----------------------------------------------------------------//
#include "MboyEDMTool/MuonboyRTAdjustableT0SvcRt.h"
#include "MboyCoreEvt/MuonboyRTAbstract.h"
#include "MboyAthEvt/MuonRecDigiStoreAth.h"
#include "MboyEDMTool/MuonboyRTMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IAdjustableT0Tool.h"

MboyRTTool::MboyRTTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p),
 p_IAdjustableT0Tool ( "AdjT0::AdjustableT0Tool" ) ,
 p_IMdtDriftCircleOnTrackCreatorAdjustableT0 ( "Muon::MdtDriftCircleOnTrackCreator" ),
 p_IMdtDriftCircleOnTrackCreator ( "Muon::MdtDriftCircleOnTrackCreator" ) 
{

  declareInterface<IMboyRTTool>(this);

  p_MuonboyRTAdjustableT0SvcRt                              = 0;
  p_MuonboyRTMdtDriftCircleOnTrackCreatorAdjustableT0 = 0;

  p_MuonboyRTMdtDriftCircleOnTrackCreator = 0;
  

//Declare the properties
  declareProperty( "RToption" , m_RToption = 4   ) ;
  declareProperty( "AdMaxima" , m_AdMaxima = 240 ) ;

  declareProperty("IAdjustableT0Tool" , p_IAdjustableT0Tool ) ;
  declareProperty("IMdtDriftCircleOnTrackCreatorAdjustableT0" , p_IMdtDriftCircleOnTrackCreatorAdjustableT0 ) ;

  declareProperty("IMdtDriftCircleOnTrackCreator" , p_IMdtDriftCircleOnTrackCreator ) ;

}

MboyRTTool::~MboyRTTool(){

  delete p_MuonboyRTAdjustableT0SvcRt                              ;
  delete p_MuonboyRTMdtDriftCircleOnTrackCreatorAdjustableT0 ;

  delete p_MuonboyRTMdtDriftCircleOnTrackCreator             ;

}

// Initialize
StatusCode MboyRTTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;
  
  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL(  " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }

  ATH_MSG_INFO(  "================================" ) ;
  ATH_MSG_INFO(  "=Proprieties are     " ) ;
  ATH_MSG_INFO(  "= RToption           " << m_RToption          ) ;
  ATH_MSG_INFO(  "= AdMaxima           " << m_AdMaxima ) ;
  ATH_MSG_INFO(  "================================" ) ;

  if ( m_RToption != 4 ) {
    ATH_MSG_FATAL(  "m_RToption value " << m_RToption << " not allowed " ) ;
    return StatusCode::FAILURE;
  }

//Create RT
  if ( m_RToption == 4 ) {
    if ( p_IMdtDriftCircleOnTrackCreator.retrieve().isFailure() ) {
      ATH_MSG_FATAL(  "Failed to retrieve tool " << p_IMdtDriftCircleOnTrackCreator ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO(  "Retrieved tool " << p_IMdtDriftCircleOnTrackCreator ) ;
    p_MuonboyRTMdtDriftCircleOnTrackCreator = new MuonboyRTMdtDriftCircleOnTrackCreator;

    if ( p_IAdjustableT0Tool.retrieve().isFailure() ) {
      ATH_MSG_FATAL(  "Failed to retrieve tool " << p_IAdjustableT0Tool ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO(  "Retrieved tool " << p_IAdjustableT0Tool ) ;

    if ( p_IMdtDriftCircleOnTrackCreatorAdjustableT0.retrieve().isFailure() ) {
      ATH_MSG_FATAL(  "Failed to retrieve tool " << p_IMdtDriftCircleOnTrackCreatorAdjustableT0 ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO(  "Retrieved tool " << p_IMdtDriftCircleOnTrackCreatorAdjustableT0 ) ;
    p_MuonboyRTMdtDriftCircleOnTrackCreatorAdjustableT0 = new MuonboyRTMdtDriftCircleOnTrackCreator;
    p_MuonboyRTAdjustableT0SvcRt                        = new MuonboyRTAdjustableT0SvcRt           ;

  }


  ATH_MSG_INFO(  "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}

StatusCode MboyRTTool::finalize(){return StatusCode::SUCCESS;}

StatusCode MboyRTTool::GetMuonboyRTAbstractWithDigiUpdate(
               MuonboyRTAbstract*& pMuonboyRTAbstract,
               MuonRecDigiStoreAth* pMuonRecDigiStoreAth,
               int WireSag
){

  StatusCode sc = StatusCode::SUCCESS;


//Get RT
  sc=GetMuonboyRTAbstract(
                          pMuonboyRTAbstract,
                          pMuonRecDigiStoreAth
                         );
  if (sc.isFailure()){
    ATH_MSG_FATAL(  " GetMuonboyRTAbstract failed " ) ;
    return( StatusCode::FAILURE );
  }


//Update pMuonRecDigiStoreAth
  int TotalNberOfDigi = pMuonRecDigiStoreAth->NberOfDigi();
  for (int ItemNber=0; ItemNber<TotalNberOfDigi ; ItemNber++){
    int Ident = ItemNber;
    MuonRecDigitMDT* pMuonRecDigitMDT = pMuonRecDigiStoreAth->NonCte_LiIdGetMuonRecDigitMDT(ItemNber)   ;
    if (pMuonRecDigitMDT){
      int TDCcount = pMuonRecDigitMDT->GetTDCcount();
      double TimeFromTDC = 0. ;
      pMuonboyRTAbstract->TfTDC(Ident,TDCcount,TimeFromTDC);
      pMuonRecDigitMDT->SetSignalDriftTime(TimeFromTDC);
      std::string RsLine = pMuonboyRTAbstract->PrintRsLine(Ident,WireSag,m_AdMaxima);
      pMuonRecDigitMDT->SetRsLine(RsLine);
    }
  }

  return StatusCode::SUCCESS;

}

StatusCode MboyRTTool::GetMuonboyRTAbstractWithDigiUpdateT0Ctrl(
               MuonboyRTAbstract*& pMuonboyRTAbstract,
               MuonRecDigiStoreAth* pMuonRecDigiStoreAth,
               int WireSag
){

  StatusCode sc = StatusCode::SUCCESS;


//Get RT
  sc=GetMuonboyRTAbstractT0Ctrl(
                          pMuonboyRTAbstract,
                          pMuonRecDigiStoreAth
                         );
  if (sc.isFailure()){
    ATH_MSG_FATAL(  " GetMuonboyRTAbstractT0Ctrl failed " ) ;
    return( StatusCode::FAILURE );
  }


//Update pMuonRecDigiStoreAth
  int TotalNberOfDigi = pMuonRecDigiStoreAth->NberOfDigi();
  for (int ItemNber=0; ItemNber<TotalNberOfDigi ; ItemNber++){
    int Ident = ItemNber;
    MuonRecDigitMDT* pMuonRecDigitMDT = pMuonRecDigiStoreAth->NonCte_LiIdGetMuonRecDigitMDT(ItemNber)   ;
    if (pMuonRecDigitMDT){
      int TDCcount = pMuonRecDigitMDT->GetTDCcount();
      double TimeFromTDC = 0. ;
      pMuonboyRTAbstract->TfTDC(Ident,TDCcount,TimeFromTDC);
      pMuonRecDigitMDT->SetSignalDriftTime(TimeFromTDC);
      std::string RsLine = pMuonboyRTAbstract->PrintRsLine(Ident,WireSag,m_AdMaxima);
      pMuonRecDigitMDT->SetRsLine(RsLine);
    }
  }

  return StatusCode::SUCCESS;

}

StatusCode MboyRTTool::GetMuonboyRTAbstract(
               MuonboyRTAbstract*& pMuonboyRTAbstract,
               const MuonRecDigiStoreAth* pMuonRecDigiStoreAth
){


//Get RT
  if (m_RToption == 4) {
    p_MuonboyRTMdtDriftCircleOnTrackCreator->p_IMdtDriftCircleOnTrackCreator = &*p_IMdtDriftCircleOnTrackCreator;
    p_MuonboyRTMdtDriftCircleOnTrackCreator->p_MuonRecDigiStoreAth = pMuonRecDigiStoreAth;
    p_MuonboyRTMdtDriftCircleOnTrackCreator->SetLinks();
    pMuonboyRTAbstract = p_MuonboyRTMdtDriftCircleOnTrackCreator ;
  }


  return StatusCode::SUCCESS;

}

StatusCode MboyRTTool::GetMuonboyRTAbstractT0Ctrl(
               MuonboyRTAbstract*& pMuonboyRTAbstract,
               const MuonRecDigiStoreAth* pMuonRecDigiStoreAth
){

  StatusCode sc = StatusCode::SUCCESS;

//Get RT
  sc=GetMuonboyRTAbstract(
                          pMuonboyRTAbstract,
                          pMuonRecDigiStoreAth
                         );
  if (sc.isFailure()){
    ATH_MSG_FATAL(  " GetMuonboyRTAbstract failed " ) ;
    return( StatusCode::FAILURE );
  }
  if (m_RToption == 4) {
    p_MuonboyRTMdtDriftCircleOnTrackCreatorAdjustableT0->p_IMdtDriftCircleOnTrackCreator = &*p_IMdtDriftCircleOnTrackCreatorAdjustableT0;
    p_MuonboyRTMdtDriftCircleOnTrackCreatorAdjustableT0->p_MuonRecDigiStoreAth = pMuonRecDigiStoreAth;
    p_MuonboyRTMdtDriftCircleOnTrackCreatorAdjustableT0->SetLinks();
    p_MuonboyRTAdjustableT0SvcRt->p_MuonboyRTAbstract = p_MuonboyRTMdtDriftCircleOnTrackCreatorAdjustableT0 ;
    p_MuonboyRTAdjustableT0SvcRt->p_IAdjustableT0Tool = &*p_IAdjustableT0Tool;
    p_MuonboyRTAdjustableT0SvcRt->ResetSetTShift();
    pMuonboyRTAbstract = p_MuonboyRTAdjustableT0SvcRt ;
  }

  return StatusCode::SUCCESS;

}
