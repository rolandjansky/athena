/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

/////////////////////////////////////////////////////////
#include "MboyView/MboyViewPointerDumperTool.h"

#include "MboyAthEvt/MuonRecDigiStoreTDS.h"
#include "MboyAthEvt/MuonRecDigiStoreAth.h"

static const InterfaceID IID_IMboyViewPointerDumperTool("MboyViewPointerDumperTool", 1, 0);

const InterfaceID& MboyViewPointerDumperTool::interfaceID()
{
  return IID_IMboyViewPointerDumperTool;
}

MboyViewPointerDumperTool::MboyViewPointerDumperTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p)
{

  declareInterface<MboyViewPointerDumperTool>(this);

  declareProperty("DoDump" , m_DoDump = 1 ) ;

}

MboyViewPointerDumperTool::~MboyViewPointerDumperTool(){}

// Initialize
StatusCode MboyViewPointerDumperTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }

  ATH_MSG_INFO( "================================" ) ;
  ATH_MSG_INFO( "=Proprieties are     " ) ;
  ATH_MSG_INFO( "= DoDump             " << m_DoDump                   ) ;
  ATH_MSG_INFO( "================================" ) ;

  ATH_MSG_INFO( "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}

StatusCode MboyViewPointerDumperTool::finalize(){return StatusCode::SUCCESS;}

void MboyViewPointerDumperTool::DumpPointer(
            std::ofstream* pOut ,
            const Trk::Track* pTrack
){
  if ( m_DoDump  == 1 ) *pOut<< std::setw(20) << (uintptr_t)pTrack  << " " ;
}
void MboyViewPointerDumperTool::DumpPointer(
            std::ofstream* pOut ,
            TrackRecord* pTrackRecord
){
  if ( m_DoDump  == 1 ) *pOut<< std::setw(20) << (uintptr_t)pTrackRecord  << " " ;
}
