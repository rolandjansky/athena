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
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
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

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

 private:
  template<class T, class Y>
  void retrieveCollections( std::vector<const T*>& cols, SG::ReadHandleKey<Y>& key );

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
  SG::ReadHandleKey<Muon::TgcPrepDataContainer>         m_keyTgc;
  SG::ReadHandleKey<Muon::TgcPrepDataContainer>         m_keyTgcPriorBC;
  SG::ReadHandleKey<Muon::TgcPrepDataContainer>         m_keyTgcNextBC;
  SG::ReadHandleKey<Muon::RpcPrepDataContainer>         m_keyRpc;
  SG::ReadHandleKey<Muon::CscPrepDataContainer>         m_keyCsc;
  SG::ReadHandleKey<Muon::MdtPrepDataContainer>         m_keyMdt;
  
  SG::WriteHandleKey<MuonPatternCombinationCollection>   m_patternCombiLocation;
  SG::WriteHandleKey<Trk::SegmentCollection>                   m_segmentLocation;
  SG::WriteHandleKey<MuonSegmentCombinationCollection>   m_segmentCombiLocation;

  ToolHandle<Muon::IMooSegmentCombinationFinder> m_segmentFinder;     //<! pointer to the segment finder
  ToolHandle<Muon::IMuonPatternSegmentAssociationTool> m_assocTool;
  ToolHandle<Muon::IMuonClusterSegmentFinder> m_clusterSegMaker;

};

template <class T, class Y>
void MooSegmentFinderAlg::retrieveCollections( std::vector<const T*>& cols, SG::ReadHandleKey<Y>& key ) {

  SG::ReadHandle<Y> cscPrds (key);
  if (cscPrds.isValid()==false) {
    ATH_MSG_ERROR("Cannot retrieve Container " << key.key() << " accessing via collections ");

  }else{
    const Y* ptr = cscPrds.cptr();
    cols.reserve(cols.size()+ptr->size());
    for(auto p : *ptr) if(!p->empty()) cols.push_back(p);
    ATH_MSG_VERBOSE("Retrieved " << cscPrds.key() << " Container " <<  cols.size());
  }
}


#endif
