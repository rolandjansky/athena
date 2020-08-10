/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCEVENTCOLLECTIONFILTER_H
#define MCEVENTCOLLECTIONFILTER_H

// Base class include
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "GeneratorObjects/McEventCollection.h"
#include "InDetSimEvent/SiHitCollection.h"
#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
#include "MuonSimEvent/CSCSimHitCollection.h"
#include "MuonSimEvent/sTGCSimHitCollection.h"
#include "MuonSimEvent/MMSimHitCollection.h"

#include "AtlasHepMC/GenParticle.h"

// std library includes
#include <string>
#include <vector>

class McEventCollectionFilter : public AthAlgorithm {
  //
  //
public:
  McEventCollectionFilter(const std::string& name, ISvcLocator* pSvcLocator);
  ~McEventCollectionFilter();
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:

  StatusCode ReduceMCEventCollection();
  StatusCode FindTRTElectronHits();
  StatusCode SiliconHitsTruthRelink();
  StatusCode SiHitsTruthRelink(SG::ReadHandle<SiHitCollection>& inputHits, SG::WriteHandle<SiHitCollection>& outputHits);
  StatusCode TRTHitsTruthRelink();
  StatusCode MDTHitsTruthRelink();
  StatusCode CSCHitsTruthRelink();
  StatusCode RPCHitsTruthRelink();
  StatusCode TGCHitsTruthRelink();
  StatusCode STGC_HitsTruthRelink();
  StatusCode MM_HitsTruthRelink();
  StatusCode BCMHitsTruthRelink();

  SG::ReadHandle<McEventCollection>  m_inputTruthCollection;
  SG::ReadHandle<SiHitCollection>  m_inputBCMHits;
  SG::ReadHandle<SiHitCollection>  m_inputPixelHits;
  SG::ReadHandle<SiHitCollection>  m_inputSCTHits;
  SG::ReadHandle<TRTUncompressedHitCollection>  m_inputTRTHits;
  SG::ReadHandle<CSCSimHitCollection>  m_inputCSCHits;
  SG::ReadHandle<MDTSimHitCollection>  m_inputMDTHits;
  SG::ReadHandle<RPCSimHitCollection>  m_inputRPCHits;
  SG::ReadHandle<TGCSimHitCollection>  m_inputTGCHits;
  SG::ReadHandle<sTGCSimHitCollection> m_inputSTGCHits;
  SG::ReadHandle<MMSimHitCollection>   m_inputMMHits;

  SG::WriteHandle<McEventCollection> m_outputTruthCollection;
  SG::WriteHandle<SiHitCollection> m_outputBCMHits;
  SG::WriteHandle<SiHitCollection> m_outputPixelHits;
  SG::WriteHandle<SiHitCollection> m_outputSCTHits;
  SG::WriteHandle<TRTUncompressedHitCollection> m_outputTRTHits;
  SG::WriteHandle<CSCSimHitCollection>  m_outputCSCHits;
  SG::WriteHandle<MDTSimHitCollection>  m_outputMDTHits;
  SG::WriteHandle<RPCSimHitCollection>  m_outputRPCHits;
  SG::WriteHandle<TGCSimHitCollection>  m_outputTGCHits;
  SG::WriteHandle<sTGCSimHitCollection> m_outputSTGCHits;
  SG::WriteHandle<MMSimHitCollection>   m_outputMMHits;

  bool m_IsKeepTRTElect;
  int m_PileupPartPDGID;
  bool m_UseTRTHits;
  bool m_UseCSCHits;
  bool m_UseSTGCHits;
  bool m_UseMMHits;
  bool m_UseBCMHits;
  //---------------------
  //std::string   m_HitName;
  int m_RefBarcode;
  std::vector<int> m_elecBarcode;



};
#endif
