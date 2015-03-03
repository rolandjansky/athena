/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// Athena/Gaudi include(s):
#include "AthenaKernel/errorcheck.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/RpcIdHelper.h"

// Local include(s):
#include "RpcCoinDataFillerTool.h"


using HepGeom::Point3D;


namespace D3PD {

   RpcCoinDataFillerTool::RpcCoinDataFillerTool( const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent )
      : BlockFillerTool< Muon::RpcCoinData >( type, name, parent ),
        m_muonManager( 0 ), m_rpcIdHelper( 0 )
   {
     book().ignore(); // Avoid coverity warnings.
   }

   StatusCode RpcCoinDataFillerTool::initialize(){

      // Initialize the base class:
      CHECK( BlockFillerTool< Muon::RpcCoinData >::initialize() );

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

   StatusCode RpcCoinDataFillerTool::book() {

      CHECK( addVariable( "time",              m_time,
                          "Time calculated from BCID and RPC clock ticks" ) );
      CHECK( addVariable( "threshold",         m_threshold,
                          "Highest threshold for the trigger pattern this hit is "
                          "contributing" ) );
      CHECK( addVariable( "isLowPtCoin",       m_isLowPtCoin,
                          "Is low Pt coin" ) );
      CHECK( addVariable( "isHighPtCoin",      m_isHighPtCoin,
                          "Is high Pt coin" ) );
      CHECK( addVariable( "isLowPtToHighPtCm", m_isLowPtToHighPtCm,
                          "Is low Pt input to High Pt CM" ) );
      CHECK( addVariable( "overlap",           m_overlap,
                          "The overlap flag" ) );
      CHECK( addVariable( "ijk",               m_ijk,
                          "ijk of the trigger hit (can be 6, 7 or 0)" ) );
      CHECK( addVariable( "parentCmId",        m_parentCmId,
                          "Parent ID (online-style) of CM" ) );
      CHECK( addVariable( "parentPadId",       m_parentPadId,
                          "Parent ID (online-style) of PAD" ) );
      CHECK( addVariable( "parentSectorId",    m_parentSectorId,
                          "Parent ID (online-style) of Pad" ) );

      CHECK( addVariable( "station",           m_station,
                          "Station name (maps to \"BIL\", \"BMS\", etc.)" ) );
      CHECK( addVariable( "eta",               m_eta,
                          "Integer in range [-8, 8]" ) );
      CHECK( addVariable( "phi",               m_phi,
                          "Integer in range [1, 8]" ) );
      CHECK( addVariable( "doubletR",          m_doubletR          ) );
      CHECK( addVariable( "doubletZ",          m_doubletZ          ) );
      CHECK( addVariable( "doubletPhi",        m_doubletPhi        ) );
      CHECK( addVariable( "measuresPhi",       m_measuresPhi       ) );
      CHECK( addVariable( "strip",             m_strip             ) );

      CHECK( addVariable( "stripX",            m_stripX            ) );
      CHECK( addVariable( "stripY",            m_stripY            ) );
      CHECK( addVariable( "stripZ",            m_stripZ            ) );
      CHECK( addVariable( "stripEta",          m_stripEta          ) );
      CHECK( addVariable( "stripPhi",          m_stripPhi          ) );

      return StatusCode::SUCCESS;
   }

   StatusCode RpcCoinDataFillerTool::fill( const Muon::RpcCoinData& rpcelem ) {

      *m_time              = rpcelem.time();
      *m_threshold         = rpcelem.threshold();
      *m_isLowPtCoin       = rpcelem.isLowPtCoin();
      *m_isHighPtCoin      = rpcelem.isHighPtCoin();
      *m_isLowPtToHighPtCm = rpcelem.isLowPtInputToHighPtCm();
      *m_overlap           = rpcelem.overlap();
      *m_ijk               = rpcelem.ijk();
      *m_parentCmId        = rpcelem.parentCmId();
      *m_parentPadId       = rpcelem.parentPadId();
      *m_parentSectorId    = rpcelem.parentSectorId();

      Identifier id = rpcelem.identify();
      if(m_rpcIdHelper){
         *m_station     = m_rpcIdHelper->stationName(id);
         *m_eta         = m_rpcIdHelper->stationEta(id);
         *m_phi         = m_rpcIdHelper->stationPhi(id);
         *m_doubletR    = m_rpcIdHelper->doubletR(id);
         *m_doubletZ    = m_rpcIdHelper->doubletZ(id);
         *m_doubletPhi  = m_rpcIdHelper->doubletPhi(id);
         *m_measuresPhi = m_rpcIdHelper->measuresPhi(id);
         *m_strip       = m_rpcIdHelper->strip(id);
      }else{
         *m_station     = -9999999;
         *m_eta         = -9999999;
         *m_phi         = -9999999;
         *m_doubletR    = -9999999;
         *m_doubletZ    = -9999999;
         *m_doubletPhi  = -9999999;
         *m_measuresPhi = -9999999;
         *m_strip       = -9999999;
      }

      Amg::Vector3D stripPos(-9999999.,-9999999.,-9999999.);
      if(m_muonManager){
         const MuonGM::RpcReadoutElement* descriptor =
            m_muonManager->getRpcReadoutElement(id);
         if( descriptor ) stripPos = descriptor->stripPos(id);
      }
      *m_stripX      = stripPos[0];
      *m_stripY      = stripPos[1];
      *m_stripZ      = stripPos[2];
      *m_stripEta    = stripPos.eta();
      *m_stripPhi    = stripPos.phi();

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
