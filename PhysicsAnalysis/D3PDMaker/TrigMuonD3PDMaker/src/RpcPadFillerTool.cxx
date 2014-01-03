/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RpcPadFillerTool.cxx 502736 2012-05-27 22:05:57Z ssnyder $

// Athena/Gaudi include(s):
#include "AthenaKernel/errorcheck.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/RpcIdHelper.h"

// Local include(s):
#include "RpcPadFillerTool.h"

static const int maxFiredChannels = 20000;

namespace D3PD {

   RpcPadFillerTool::RpcPadFillerTool( const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent )
      : BlockFillerTool< RpcTree >( type, name, parent ),
        m_muonManager( 0 ), m_rpcIdHelper( 0 )
   {
     book().ignore(); // Avoid coverity warnings.
   }

   StatusCode RpcPadFillerTool::initialize(){

      // Initialize the base class:
      CHECK( BlockFillerTool< RpcTree >::initialize() );

      // Try to retrieve MuonDetectorManager from the detector store:
      if( detStore()->retrieve( m_muonManager ).isFailure() ) {
         ATH_MSG_WARNING( "Cannot retrieve MuonDetectorManager" );
      }
      // Try to retrieve RpcIdHelper from the detector store:
      if( detStore()->retrieve( m_rpcIdHelper, "RPCIDHELPER" ).isFailure() ) {
         ATH_MSG_WARNING( "Cannot retrieve RpcIdHelper" );
      }

      return StatusCode::SUCCESS;
   }

   StatusCode RpcPadFillerTool::book() {

      CHECK( addVariable( "sector",              m_sector,
                          "Sector"        ) );
      CHECK( addVariable( "padId",               m_padId,
                          "Pad Id"        ) );
      CHECK( addVariable( "status",              m_status,
                          "Status"        ) );
      CHECK( addVariable( "ercode",              m_ercode,
                          "Error code"    ) );
      CHECK( addVariable( "cmaId",               m_cmaId,
                          "Cma Id"        ) );
      CHECK( addVariable( "fel1Id",              m_fel1Id,
                          "fel1 Id"       ) );
      CHECK( addVariable( "febcId",              m_febcId,
                          "febc Id"       ) );
      CHECK( addVariable( "crc",                 m_crc,
                          "crc"           ) );
      CHECK( addVariable( "bcId",                m_bcId,
                           "bc Id"        ) );
      CHECK( addVariable( "ticks",               m_ticks,
                          "time in ticks" ) );
      CHECK( addVariable( "ijk",                 m_ijk,
                          "ijk"           ) );
      CHECK( addVariable( "cmachan",             m_cmachan,
                          "cmachan"       ) );
      CHECK( addVariable( "overlap",             m_overlap,
                          "overlap flag"  ) );
      CHECK( addVariable( "threshold",           m_threshold,
                          "threshold"     ) );

      return StatusCode::SUCCESS;
   }

   StatusCode RpcPadFillerTool::fill( const RpcTree& rpcelem ) {

      *m_bcId=rpcelem.Chid->bcid();
      *m_ticks=rpcelem.Chid->time();
      *m_ijk=rpcelem.Chid->ijk();
      *m_cmachan=rpcelem.Chid->channel();
      if (rpcelem.Chid->ijk()==7) {
         *m_overlap=rpcelem.Chid->ovl();
         *m_threshold=rpcelem.Chid->thr();
      }
      else {
         *m_overlap=-1;
         *m_threshold=-1;
      }

      *m_sector=rpcelem.Padid->sector();
      *m_padId=rpcelem.Padid->onlineId();
      *m_status=rpcelem.Padid->status();
      *m_ercode=rpcelem.Padid->errorCode();
      *m_cmaId=rpcelem.Cmaid->onlineId();
      *m_fel1Id=rpcelem.Cmaid->fel1Id();
      *m_febcId=rpcelem.Cmaid->febcId();
      *m_crc=rpcelem.Cmaid->crc();

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
