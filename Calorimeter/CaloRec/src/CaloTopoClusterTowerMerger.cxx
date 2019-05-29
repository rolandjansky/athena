/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "xAODCaloEvent/CaloClusterAuxContainer.h"

#include "CaloRec/CaloTopoClusterTowerMerger.h"
#include "CaloRec/CaloTopoClusterFromTowerHelpers.h"

#include "CaloUtils/CaloClusterStoreHelper.h"

// #include <atomic>

// std::atomic<bool> CaloTopoClusterTowerMerger_checkMoments(true);

// #define CL_MNAME( NAME ) xAOD::CaloCluster::NAME
// #define CL_ENTRY( NAME ) std::tuple< CL_MNAME( MomentType ),std::string>( CL_MNAME( NAME ), #NAME )

#define CL_RHMSG( NAME ) MsgStream& operator<<(MsgStream& mstr,const SG::ReadHandleKey< NAME >& ckey )  { mstr << ckey.key(); return mstr; }
#define CL_WHMSG( NAME ) MsgStream& operator<<(MsgStream& mstr,const SG::WriteHandleKey< NAME >& ckey ) { mstr << ckey.key(); return mstr; }

namespace {
  CL_RHMSG( xAOD::CaloClusterContainer )
}

// std::vector<xAOD::CaloCluster::MomentType>                           CaloTopoClusterTowerMerger::m_momentList = std::vector< CL_MNAME( MomentType ) >();
//  std::vector<std::tuple<xAOD::CaloCluster::MomentType,std::string> > CaloTopoClusterTowerMerger::m_momentMap {
//   CL_ENTRY( FIRST_PHI ),
//   CL_ENTRY( FIRST_ETA ),
//   CL_ENTRY( SECOND_R ),
//   CL_ENTRY( SECOND_LAMBDA ),
//   CL_ENTRY( DELTA_PHI ),
//   CL_ENTRY( DELTA_THETA ),
//   CL_ENTRY( DELTA_ALPHA ),
//   CL_ENTRY( CENTER_X ),
//   CL_ENTRY( CENTER_Y ),
//   CL_ENTRY( CENTER_Z ),
//   CL_ENTRY( CENTER_MAG ),
//   CL_ENTRY( CENTER_LAMBDA ),
//   CL_ENTRY( LATERAL ),
//   CL_ENTRY( LONGITUDINAL ),
//   CL_ENTRY( ENG_FRAC_EM ),
//   CL_ENTRY( ENG_FRAC_MAX ),
//   CL_ENTRY( ENG_FRAC_CORE ),
//   CL_ENTRY( FIRST_ENG_DENS ),
//   CL_ENTRY( SECOND_ENG_DENS ),
//   CL_ENTRY( ISOLATION ),
//   CL_ENTRY( ENG_BAD_CELLS ),    
//   CL_ENTRY( N_BAD_CELLS ),
//   CL_ENTRY( N_BAD_CELLS_CORR ), 
//   CL_ENTRY( BAD_CELLS_CORR_E ), 
//   CL_ENTRY( BADLARQ_FRAC ),     
//   CL_ENTRY( ENG_POS ),
//   CL_ENTRY( SIGNIFICANCE ),
//   CL_ENTRY( CELL_SIGNIFICANCE ),
//   CL_ENTRY( CELL_SIG_SAMPLING ),
//   CL_ENTRY( AVG_LAR_Q ),
//   CL_ENTRY( AVG_TILE_Q ),       
//   CL_ENTRY( ENG_BAD_HV_CELLS ), 
//   CL_ENTRY( N_BAD_HV_CELLS ),
//   CL_ENTRY( PTD ),
//   CL_ENTRY( EM_PROBABILITY ),
//   CL_ENTRY( HAD_WEIGHT ),
//   CL_ENTRY( OOC_WEIGHT ),
//   CL_ENTRY( DM_WEIGHT ),        
//   CL_ENTRY( TILE_CONFIDENCE_LEVEL ),
//   CL_ENTRY( VERTEX_FRACTION ),
//   CL_ENTRY( NVERTEX_FRACTION ),
//   CL_ENTRY( ETACALOFRAME ), 
//   CL_ENTRY( PHICALOFRAME ),
//   CL_ENTRY( ETA1CALOFRAME ),
//   CL_ENTRY( PHI1CALOFRAME ),
//   CL_ENTRY( ETA2CALOFRAME ),
//   CL_ENTRY( PHI2CALOFRAME ),
//   CL_ENTRY( ENG_CALIB_TOT ),
//   CL_ENTRY( ENG_CALIB_OUT_L ),
//   CL_ENTRY( ENG_CALIB_OUT_M ),
//   CL_ENTRY( ENG_CALIB_OUT_T ),
//   CL_ENTRY( ENG_CALIB_DEAD_L ),
//   CL_ENTRY( ENG_CALIB_DEAD_M ),
//   CL_ENTRY( ENG_CALIB_DEAD_T ),
//   CL_ENTRY( ENG_CALIB_EMB0 ),       
//   CL_ENTRY( ENG_CALIB_EME0 ),       
//   CL_ENTRY( ENG_CALIB_TILEG3 ),    
//   CL_ENTRY( ENG_CALIB_DEAD_TOT ),   
//   CL_ENTRY( ENG_CALIB_DEAD_EMB0 ),  
//   CL_ENTRY( ENG_CALIB_DEAD_TILE0 ),
//   CL_ENTRY( ENG_CALIB_DEAD_TILEG3 ),
//   CL_ENTRY( ENG_CALIB_DEAD_EME0 ),  
//   CL_ENTRY( ENG_CALIB_DEAD_HEC0 ),  
//   CL_ENTRY( ENG_CALIB_DEAD_FCAL ),  
//   //  CL_ENTRY( ENG_CALIB_DEAD_LEAKAG ), // not in r21
//   //  CL_ENTRY( ENG_CALIB_DEAD_UNCLAS ), // not in r21
//   CL_ENTRY( ENG_CALIB_FRAC_EM ),    
//   CL_ENTRY( ENG_CALIB_FRAC_HAD ),   
//   CL_ENTRY( ENG_CALIB_FRAC_REST )  
// }; 

CaloTopoClusterTowerMerger::CaloTopoClusterTowerMerger(const std::string& name,ISvcLocator* pSvcLocator)
  : AthAlgorithm(name,pSvcLocator)
  , m_clusterContainerKey("CaloCalTopoCluster")
  , m_towerContainerKey("CaloCalFwdTopoTower")
  , m_topoSignalContainerKey("CaloCalTopoSignal")
    // ** not in r21 **, m_cellLinkContainerKey("")
  , m_clusterRange(3.2)
{
  declareProperty("TopoClusterContainerKey",m_clusterContainerKey,   "Topo-cluster container key"                                    );
  declareProperty("TopoTowerContainerKey",  m_towerContainerKey,     "Topo-tower container key"                                      );
  declareProperty("TopoSignalContainerKey", m_topoSignalContainerKey,"Topo-signal container key"                                     ); 
  declareProperty("TopoClusterRange",       m_clusterRange,          "Rapidity range for using topo-clusters in combined signal mode");
}

CaloTopoClusterTowerMerger::~CaloTopoClusterTowerMerger()
{ }

StatusCode CaloTopoClusterTowerMerger::initialize()
{
  if ( m_clusterRange <= 0. ) {
    ATH_MSG_ERROR( CaloRec::Helpers::fmtMsg("Invalid topo-cluster range |y| < %6.3f - algorithm non-functional",m_clusterRange) );
    return StatusCode::FAILURE;
  }

  // ** not in r21 ** if ( m_cellLinkContainerKey.key().empty() ) { m_cellLinkKey = m_topoSignalContainerKey.key() + std::string("_links"); }

  ATH_CHECK( m_clusterContainerKey.initialize()    );
  ATH_CHECK( m_towerContainerKey.initialize()      );
  ATH_CHECK( m_topoSignalContainerKey.initialize() );
  // ** not in r21 **  ATH_CHECK( m_cellLinkContainerKey.initialize()            );

  ATH_MSG_INFO( CaloRec::Helpers::fmtMsg("Topo_cluster with |y| < %.2f will be merged with topo-towers with |y| > %.2f",m_clusterRange,m_clusterRange) );
  //  if ( m_copyMoments ) { ATH_MSG_INFO( "Cluster moments are explicitly copied." ); }

  return StatusCode::SUCCESS;
}

StatusCode CaloTopoClusterTowerMerger::execute()
{

  // collect input
  rhandle_t clusterHandle(m_clusterContainerKey);
  if ( !clusterHandle.isValid() ) { 
    ATH_MSG_WARNING( "Topo-cluster container with key <" << m_clusterContainerKey << "> not found" );
    return StatusCode::SUCCESS;
  }
  rhandle_t towerHandle(m_towerContainerKey);
  if ( !towerHandle.isValid() ) {
    ATH_MSG_WARNING( "Topo-tower container with key <" << m_towerContainerKey << "> not found" ); 
    return StatusCode::SUCCESS;
  }

  // prepare output
  whandle_t signalHandle(m_topoSignalContainerKey);
  ATH_CHECK(this->addContainerWriteHandle(signalHandle));

  // check cluster moments 
  // if ( m_copyMoments ) {  if ( CaloTopoClusterTowerMerger_checkMoments ) { this->fillMoments(*(towerHandle.ptr()->front())); CaloTopoClusterTowerMerger_checkMoments = false; } }

  // fill output from topo-clusters
  for ( auto pClus : *clusterHandle ) { if ( clusterFilter(*pClus) ) { this->makeDeepCopy(*pClus,signalHandle.ptr()); } }
  // fill output from topo-towers
  for ( auto pTowr : *towerHandle )   { if ( towerFilter(*pTowr) )   { this->makeDeepCopy(*pTowr,signalHandle.ptr()); } }

  // take care of cell links
  ATH_CHECK(CaloClusterStoreHelper::finalizeClusters(&(*evtStore()),signalHandle.ptr(),m_topoSignalContainerKey.key(),msg()));

  return StatusCode::SUCCESS;
}

// bool CaloTopoClusterTowerMerger::fillMoments(const xAOD::CaloCluster& rClus) 
// {
//   m_momentList.reserve(m_momentMap.size());
//   double val(0);
//   if ( !msgLvl(MSG::DEBUG) ) { 
//     for ( auto fmom(m_momentMap.begin()); fmom != m_momentMap.end(); ++fmom ) { 
//       if ( rClus.retrieveMoment(std::get<0>(*fmom),val) ) { m_momentList.push_back(std::get<0>(*fmom)); }
//     }
//   } else { 
//     ATH_MSG_DEBUG("###############################");
//     ATH_MSG_DEBUG("## Moments (topo-towers)     ##");
//     ATH_MSG_DEBUG("##---------------------------##");
//     for ( auto fmom(m_momentMap.begin()); fmom != m_momentMap.end(); ++fmom ) { 
//       if ( rClus.retrieveMoment( std::get<0>(*fmom), val ) ) { 
// 	m_momentList.push_back(std::get<0>(*fmom)); 
// 	ATH_MSG_DEBUG( CaloRec::Helpers::fmtMsg("## %5.5x | \042%16.16s\042 ##", std::get<0>(*fmom), std::get<1>(*fmom).c_str()) );
//       }
//     }
//     ATH_MSG_DEBUG("###############################");
//   } // debug mode
//   return true;
// }

bool CaloTopoClusterTowerMerger::makeDeepCopy(const xAOD::CaloCluster& rClus,xAOD::CaloClusterContainer* pClusCont) const
{ pClusCont->push_back(new xAOD::CaloCluster(rClus)); return true; }

StatusCode CaloTopoClusterTowerMerger::addContainerWriteHandle(whandle_t& signalHandle)
{
  // get a new signal handle
  signalHandle = std::unique_ptr<xAOD::CaloClusterContainer>(new xAOD::CaloClusterContainer());
  if ( !signalHandle.isValid() ) { return StatusCode::FAILURE; }
  // get AUX container
  xAOD::CaloClusterAuxContainer* auxData = new xAOD::CaloClusterAuxContainer();
  std::string auxName(m_topoSignalContainerKey.key()+"Aux.");
  if ( evtStore()->overwrite(auxData,auxName).isFailure() ) {
    ATH_MSG_ERROR("Failed to record xAOD::CaloClusterAuxContainer with key <" << auxName << ">");
    delete auxData;
    return StatusCode::FAILURE;
  } 
  // connect store with object container
  signalHandle.ptr()->setStore(auxData);
  return StatusCode::SUCCESS;
}
