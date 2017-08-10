/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MooSegmentFinderAlg.h"
#include "MuonSegmentCombinerToolInterfaces/IMooSegmentCombinationFinder.h"

#include "MuonRecToolInterfaces/IMuonPatternSegmentAssociationTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonClusterSegmentFinder.h"

#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentCombination.h"

#include "MuonSegment/MuonSegmentCombinationCollection.h"
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
  m_segmentCombiLocation("MooreSegmentCombinations"),
  m_segmentFinder("Muon::MooSegmentCombinationFinder/MooSegmentCombinationFinder"),
  m_assocTool("Muon::MuonPatternSegmentAssociationTool/MuonPatternSegmentAssociationTool"),
  m_clusterSegMaker("Muon::MuonClusterSegmentFinder/MuonClusterSegmentFinder")
{
  declareProperty("UseRPC",m_useRpc = true);
  declareProperty("UseTGC",m_useTgc = true);
  declareProperty("UseTGCPriorBC",m_useTgcPriorBC = false);
  declareProperty("UseTGCNextBC",m_useTgcNextBC = false);
  declareProperty("UseCSC",m_useCsc = true);
  declareProperty("UseMDT",m_useMdt = true);

  declareProperty("doTGCClust",m_doTGCClust = false);
  declareProperty("doRPCClust",m_doRPCClust = false);


  declareProperty("CscPrepDataContainer", m_keyCsc);
  declareProperty("MdtPrepDataContainer", m_keyMdt);
  declareProperty("RpcPrepDataContainer", m_keyRpc);
  declareProperty("TgcPrepDataContainer", m_keyTgc);
  declareProperty("TgcPrepDataContainerPriorBC", m_keyTgcPriorBC);
  declareProperty("TgcPrepDataContainerNextBC", m_keyTgcNextBC);

  declareProperty("MuonPatternCombinationLocation",      m_patternCombiLocation);
  declareProperty("MuonSegmentOutputLocation",           m_segmentLocation );
  declareProperty("MuonSegmentCombinationOutputLocation",m_segmentCombiLocation);

  declareProperty("SegmentFinder", m_segmentFinder );
  declareProperty("MuonClusterSegmentFinderTool",m_clusterSegMaker);
}

MooSegmentFinderAlg::~MooSegmentFinderAlg()
{
}

StatusCode MooSegmentFinderAlg::initialize()
{
  if (AthAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  
  if( m_segmentFinder.retrieve().isFailure() ){
    ATH_MSG_FATAL("Could not get " << m_segmentFinder); 
    return StatusCode::FAILURE;
  }
  
  if( m_clusterSegMaker.retrieve().isFailure() ){
    ATH_MSG_FATAL("Could not get " << m_clusterSegMaker);
    return StatusCode::FAILURE;
  }

  if (m_assocTool.retrieve().isFailure()){
    ATH_MSG_FATAL("Could not get " << m_assocTool); 
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_keyMdt.initialize(m_useMdt) ); //Nullify key from scheduler if not needed
  ATH_CHECK( m_keyCsc.initialize(m_useCsc) );
  ATH_CHECK( m_keyRpc.initialize(m_useRpc) );
  ATH_CHECK( m_keyTgcPriorBC.initialize(m_useTgcPriorBC) );
  ATH_CHECK( m_keyTgcNextBC.initialize(m_useTgcNextBC) );
  ATH_CHECK( m_keyTgc.initialize(m_useTgc) );

  ATH_CHECK( m_patternCombiLocation.initialize() );
  ATH_CHECK( m_segmentLocation.initialize() );
  ATH_CHECK( m_segmentCombiLocation.initialize() );
  
  return StatusCode::SUCCESS; 
}




StatusCode MooSegmentFinderAlg::execute()
{

  m_assocTool->reset(); // clear all prior associations.

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

  Muon::IMooSegmentCombinationFinder::Output* output = m_segmentFinder->findSegments( mdtCols, cscCols, tgcCols, rpcCols );

  //do cluster based segment finding
  std::vector<const Muon::MuonSegment*>* segs(NULL);
  if (m_doTGCClust || m_doRPCClust) segs = m_clusterSegMaker->getClusterSegments(m_doTGCClust,m_doRPCClust);

  MuonSegmentCombinationCollection* segmentCombinations = output ? const_cast<MuonSegmentCombinationCollection*>(output->segmentCombinations) : 0;
  if( !segmentCombinations ) segmentCombinations = new MuonSegmentCombinationCollection();

  
  SG::WriteHandle<MuonSegmentCombinationCollection> segCombiHandle(m_segmentCombiLocation);
  if( segCombiHandle.record(std::unique_ptr<MuonSegmentCombinationCollection > (segmentCombinations)).isSuccess() ){
    ATH_MSG_VERBOSE("stored MuonSegmentCombinationCollection " << segmentCombinations->size() 
	     << " at " << m_segmentCombiLocation.key());
  }else{
    ATH_MSG_ERROR("Failed to store MuonSegmentCombinationCollection at " << m_segmentCombiLocation.key());
  }
  
  //FIXME FIX CONST_CASTS LATER
  Trk::SegmentCollection* segmentCollection = output ? const_cast<Trk::SegmentCollection*>(output->segmentCollection) : 0;
  if( !segmentCollection ) segmentCollection = new Trk::SegmentCollection();

  //add the cluster segments to the segmentCollection
  if (segs){ 
    for( std::vector<const Muon::MuonSegment*>::iterator sit = segs->begin(); sit!=segs->end();++sit ){
      Muon::MuonSegment* nonConstIt = const_cast<Muon::MuonSegment*>(*sit);
      Trk::Segment* trkSeg = nonConstIt;
      segmentCollection->push_back(trkSeg);
    }
    delete segs;
  }
 
  SG::WriteHandle<Trk::SegmentCollection> segHandle(m_segmentLocation); 

  if (segHandle.record(std::unique_ptr<Trk::SegmentCollection>(segmentCollection)).isSuccess() ){
    ATH_MSG_VERBOSE("stored MuonSegmentCollection at " <<  m_segmentLocation.key() 
			   << " size " << segmentCollection->size());
  }else{
    ATH_MSG_ERROR("Failed to store MuonSegmentCollection ");
  }

  MuonPatternCombinationCollection* patternCombinations = output ? const_cast<MuonPatternCombinationCollection*>(output->patternCombinations) : 0;
  if( !patternCombinations ) patternCombinations = new MuonPatternCombinationCollection();

  SG::WriteHandle<MuonPatternCombinationCollection> patHandle(m_patternCombiLocation); 
  
  if( patHandle.record(std::unique_ptr<MuonPatternCombinationCollection>(patternCombinations)).isSuccess() ){
    ATH_MSG_VERBOSE("stored MuonPatternCombinationCollection " << patternCombinations->size() 
						 << " at " << m_patternCombiLocation.key());
  }else{
    ATH_MSG_ERROR("Failed to store MuonPatternCombinationCollection at " << m_patternCombiLocation.key());
  }
  
  delete output;

  return StatusCode::SUCCESS;
} // execute

StatusCode MooSegmentFinderAlg::finalize()
{
  
  return AthAlgorithm::finalize();
}



