/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonTruthDecorationAlg.h
//   Header file for class MuonTruthDecorationAlg
///////////////////////////////////////////////////////////////////

#ifndef TRUTHPARTICLEALGS_MUONTRUTHDECORATIONALG_H
#define TRUTHPARTICLEALGS_MUONTRUTHDECORATIONALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>
#include <map>
#include <vector>

#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/MuonStationIndex.h"
#include "MuonRecToolInterfaces/IMuonTrackTruthTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "Identifier/Identifier.h"

#include "TrackRecord/TrackRecordCollection.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertex.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "xAODMuon/MuonSegmentContainer.h"

class IMCTruthClassifier;
class MuonSimDataCollection;
class CscSimDataCollection;

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Muon {

class MuonTruthDecorationAlg : public AthAlgorithm  {

public:
  typedef std::map<Muon::MuonStationIndex::ChIndex, std::vector<Identifier> >   ChamberIdMap;

  // Constructor with parameters:
  MuonTruthDecorationAlg(const std::string &name,ISvcLocator *pSvcLocator);

  // Basic algorithm methods:
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:
  void addTrackRecords( xAOD::TruthParticle& truthParticle, std::vector< std::pair<const TrackRecordCollection*,std::string> >& trackRecords,
                        const xAOD::TruthVertex* vertex ) const;
  void addHitCounts( xAOD::TruthParticle& truthParticle, const std::vector<const PRD_MultiTruthCollection*>& collections,
		     ChamberIdMap* ids = 0 ) const;
  void addHitIDVectors( xAOD::TruthParticle& truthParticle, const MuonTruthDecorationAlg::ChamberIdMap& ids) const;
  void createSegments( const ElementLink< xAOD::TruthParticleContainer >& truthLink,
                       const MuonTruthDecorationAlg::ChamberIdMap& ids, 
		       xAOD::MuonSegmentContainer& segmentContainer,
		       const std::vector<const MuonSimDataCollection*>& sdoCollections,
		       const CscSimDataCollection* cscCollection ) const;

  std::string m_truthParticleContainerName;
  std::string m_muonTruthParticleContainerName;
  std::vector<std::string> m_trackRecordCollectionNames;
  std::vector<std::string> m_PRD_TruthNames;
  std::vector<std::string> m_SDO_TruthNames;
  std::string              m_CSC_SDO_TruthNames;
  ToolHandle<Muon::MuonIdHelperTool>    m_idHelper;
  ToolHandle<Muon::MuonEDMPrinterTool>  m_printer;
  ToolHandle<IMCTruthClassifier>        m_truthClassifier;
  ToolHandle<Trk::IExtrapolator>        m_extrapolator;
  const MuonGM::MuonDetectorManager * m_muonMgr;
  bool m_createTruthSegment;
  std::string m_muonTruthSegmentContainerName;
  int m_barcodeOffset;
};

} // namespace Muon


#endif //TRUTHPARTICLEALGS_MUONTRUTHDECORATIONALG_H
