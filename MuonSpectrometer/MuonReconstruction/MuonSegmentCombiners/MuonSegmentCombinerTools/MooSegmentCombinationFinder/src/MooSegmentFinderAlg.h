/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MOOSEGMENTFINDERS_MOOSEGMENTFINDERALGS_H
#define MOOSEGMENTFINDERS_MOOSEGMENTFINDERALGS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonPrepRawData/CscPrepDataCollection.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"

#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "TrkSegment/SegmentCollection.h"

class MsgStream;
class StoreGateSvc;

namespace Muon {
  class IMuonClusterSegmentFinder;
  class IMuonPatternSegmentAssociationTool;
  class IMooSegmentCombinationFinder;
}

class MooSegmentFinderAlg : public AthAlgorithm
{
 public:
  MooSegmentFinderAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~MooSegmentFinderAlg();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:
  
  /** retrieve collections from storegate */
  void retrieveCollections( std::vector<const Muon::TgcPrepDataCollection*>& cols, std::string key );
  void retrieveCollections( std::vector<const Muon::RpcPrepDataCollection*>& cols, std::string key );
  void retrieveCollections( std::vector<const Muon::CscPrepDataCollection*>& cols, std::string key );
  void retrieveCollections( std::vector<const Muon::MdtPrepDataCollection*>& cols, std::string key );

  /** extract segments from a MuonSegmentCombinationCollection */
  Trk::SegmentCollection* extractSegmentCollection( const MuonSegmentCombinationCollection& segmentCombinations ) const;

  /** selection flags for all four technologies */
  bool                m_useTgc;
  bool                m_useTgcPriorBC;
  bool                m_useTgcNextBC;
  bool                m_useRpc;
  bool                m_useCsc;
  bool                m_useMdt;

  /** selection flags for cluster based segment finding */
  bool                m_doTGCClust;
  bool                m_doRPCClust;

  /** storegate location of the MuonPrepDataContainer for all four technologies */
  std::string         m_keyTgc;
  std::string         m_keyTgcPriorBC;
  std::string         m_keyTgcNextBC;
  std::string         m_keyRpc;
  std::string         m_keyCsc;
  std::string         m_keyMdt;
  
  std::string         m_patternCombiLocation;
  std::string         m_segmentLocation;
  std::string         m_segmentCombiLocation;

  ToolHandle<Muon::IMooSegmentCombinationFinder> m_segmentFinder;     //<! pointer to the segment finder
  ToolHandle<Muon::IMuonPatternSegmentAssociationTool> m_assocTool;
  ToolHandle<Muon::IMuonClusterSegmentFinder> m_clusterSegMaker;

};


#endif
