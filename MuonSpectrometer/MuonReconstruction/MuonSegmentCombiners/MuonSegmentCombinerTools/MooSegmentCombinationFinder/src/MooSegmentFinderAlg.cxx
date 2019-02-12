/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MooSegmentFinderAlg.h"
#include "MuonSegmentCombinerToolInterfaces/IMooSegmentCombinationFinder.h"

#include "MuonSegmentMakerToolInterfaces/IMuonClusterSegmentFinder.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentOverlapRemovalTool.h"

#include "MuonSegment/MuonSegment.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "StoreGate/StoreGateSvc.h"

MooSegmentFinderAlg::MooSegmentFinderAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator), 
  m_keyTgc("TGC_Measurements"),
  m_keyTgcPriorBC( "TGC_MeasurementsPriorBC"),
  m_keyTgcNextBC("TGC_MeasurementsNextBC"),
  m_keyRpc("RPC_Measurements"),
  m_keyCsc( "CSC_Clusters"),
  m_keyMdt("MDT_DriftCircles"),
  m_patternCombiLocation("MuonHoughPatternCombinations"),
  m_segmentLocation("MooreSegments"),
  m_segmentFinder("Muon::MooSegmentCombinationFinder/MooSegmentCombinationFinder"),
  m_clusterSegMaker("Muon::MuonClusterSegmentFinder/MuonClusterSegmentFinder"),
  m_overlapRemovalTool("Muon::MuonSegmentOverlapRemovalTool/MuonSegmentOverlapRemovalTool")
{
  declareProperty("UseRPC",m_useRpc = true);
  declareProperty("UseTGC",m_useTgc = true);
  declareProperty("UseTGCPriorBC",m_useTgcPriorBC = false);
  declareProperty("UseTGCNextBC",m_useTgcNextBC = false);
  declareProperty("UseCSC",m_useCsc = true);
  declareProperty("UseMDT",m_useMdt = true);

  declareProperty("doTGCClust",m_doTGCClust = false);
  declareProperty("doRPCClust",m_doRPCClust = false);
  declareProperty("doClusterTruth",m_doClusterTruth=false);

  declareProperty("CscPrepDataContainer", m_keyCsc);
  declareProperty("MdtPrepDataContainer", m_keyMdt);
  declareProperty("RpcPrepDataContainer", m_keyRpc);
  declareProperty("TgcPrepDataContainer", m_keyTgc);
  declareProperty("TgcPrepDataContainerPriorBC", m_keyTgcPriorBC);
  declareProperty("TgcPrepDataContainerNextBC", m_keyTgcNextBC);

  declareProperty("MuonPatternCombinationLocation",      m_patternCombiLocation);
  declareProperty("MuonSegmentOutputLocation",           m_segmentLocation );

  declareProperty("SegmentFinder", m_segmentFinder );
  declareProperty("MuonClusterSegmentFinderTool",m_clusterSegMaker);
  declareProperty("SegmentOverlapRemovalTool", m_overlapRemovalTool, "tool to removal overlaps in segment combinations" );
}

MooSegmentFinderAlg::~MooSegmentFinderAlg()
{
}

StatusCode MooSegmentFinderAlg::initialize()
{
  
  ATH_CHECK( m_segmentFinder.retrieve() ); 
  ATH_CHECK( m_clusterSegMaker.retrieve() );
  ATH_CHECK( m_overlapRemovalTool.retrieve() );

  ATH_CHECK( m_keyMdt.initialize(m_useMdt) ); //Nullify key from scheduler if not needed
  ATH_CHECK( m_keyCsc.initialize(m_useCsc) );
  ATH_CHECK( m_keyRpc.initialize(m_useRpc) );
  ATH_CHECK( m_keyTgcPriorBC.initialize(m_useTgcPriorBC) );
  ATH_CHECK( m_keyTgcNextBC.initialize(m_useTgcNextBC) );
  ATH_CHECK( m_keyTgc.initialize(m_useTgc) );

  ATH_CHECK( m_tgcTruth.initialize(m_doClusterTruth) );
  ATH_CHECK( m_rpcTruth.initialize(m_doClusterTruth) );

  ATH_CHECK( m_patternCombiLocation.initialize() );
  ATH_CHECK( m_segmentLocation.initialize() );
  
  return StatusCode::SUCCESS; 
}

StatusCode MooSegmentFinderAlg::execute()
{

  std::vector<const Muon::MdtPrepDataCollection*> mdtCols;
  std::vector<const Muon::CscPrepDataCollection*> cscCols;
  std::vector<const Muon::TgcPrepDataCollection*> tgcCols; 
  std::vector<const Muon::RpcPrepDataCollection*> rpcCols;
  if( m_useMdt ) retrieveCollections(mdtCols,m_keyMdt);
  if( m_useCsc ) retrieveCollections(cscCols,m_keyCsc);
  if( m_useTgc ) retrieveCollections(tgcCols,m_keyTgc);
  if( m_useTgcPriorBC ) retrieveCollections(tgcCols,m_keyTgcPriorBC);
  if( m_useTgcNextBC )  retrieveCollections(tgcCols,m_keyTgcNextBC);
  if( m_useRpc ) retrieveCollections(rpcCols,m_keyRpc);

  Muon::IMooSegmentCombinationFinder::Output output;

  SG::WriteHandle<Trk::SegmentCollection> segHandle(m_segmentLocation); 

  if (segHandle.record(std::make_unique<Trk::SegmentCollection>()).isSuccess() ){
    ATH_MSG_VERBOSE("stored MuonSegmentCollection at "<<m_segmentLocation.key());
  }else{
    ATH_MSG_ERROR("Failed to store MuonSegmentCollection ");
  }
  output.segmentCollection=segHandle.ptr();

  SG::WriteHandle<MuonPatternCombinationCollection> patHandle(m_patternCombiLocation); 
  
  if( patHandle.record(std::make_unique<MuonPatternCombinationCollection>()).isSuccess() ){
    ATH_MSG_VERBOSE("stored MuonPatternCombinationCollection at " << m_patternCombiLocation.key());
  }else{
    ATH_MSG_ERROR("Failed to store MuonPatternCombinationCollection at " << m_patternCombiLocation.key());
  }
  output.patternCombinations=patHandle.ptr();

  m_segmentFinder->findSegments( mdtCols, cscCols, tgcCols, rpcCols, output );

  //do cluster based segment finding
  if (m_doTGCClust || m_doRPCClust){
    const PRD_MultiTruthCollection* tgcTruthColl=0;
    const PRD_MultiTruthCollection* rpcTruthColl=0;
    if(m_doClusterTruth){
      SG::ReadHandle<PRD_MultiTruthCollection> tgcTruth(m_tgcTruth);
      SG::ReadHandle<PRD_MultiTruthCollection> rpcTruth(m_rpcTruth);
      tgcTruthColl=tgcTruth.cptr();
      rpcTruthColl=rpcTruth.cptr();
    }
    SG::ReadHandle<Muon::MdtPrepDataContainer> mdth(m_keyMdt);
    m_clusterSegMaker->getClusterSegments(mdth.cptr(), m_doTGCClust ? &tgcCols : 0, m_doRPCClust ? &rpcCols : 0, tgcTruthColl, rpcTruthColl, segHandle.ptr());
  }

  m_overlapRemovalTool->removeDuplicates(segHandle.ptr());

  return StatusCode::SUCCESS;
} // execute

StatusCode MooSegmentFinderAlg::finalize()
{
  return StatusCode::SUCCESS;
}
