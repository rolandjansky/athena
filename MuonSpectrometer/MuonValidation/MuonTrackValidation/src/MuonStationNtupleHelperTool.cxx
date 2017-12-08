/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonStationNtupleHelperTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "MuonStationNtupleHelperTool.h"
#include "TrkEventUtils/IdentifierExtractor.h"
#include "TTree.h"
#include "TrkEventPrimitives/TrackStateDefs.h"
//#include "MuonIdHelpers/RpcIdHelper.h" 

//================ Constructor =================================================

Muon::MuonStationNtupleHelperTool::MuonStationNtupleHelperTool(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_mdtSectorIx(0),
  m_mdtStationIx(0),
  m_rpcSectorIx(0),
  m_rpcStationIx(0),
  m_rpcMeasuresPhi(0),
  m_tgcStationIx(0),
  m_tgcMeasuresPhi(0)
{
  declareInterface<Trk::IValidationNtupleHelperTool>(this);

  //  template for property decalration
  //declareProperty("PropertyName", m_propertyName);
}

//================ Destructor =================================================

Muon::MuonStationNtupleHelperTool::~MuonStationNtupleHelperTool()
{}


//================ Initialisation =================================================

StatusCode Muon::MuonStationNtupleHelperTool::initialize()
{
  
  StatusCode sc = AlgTool::initialize();
  if (sc.isFailure()) return sc;

  /*
  if (detStore()->retrieve(m_rpcId).isFailure()) {
    ATH_MSG_WARNING ("Could not get RPC ID helper !");
    return StatusCode::FAILURE;
  }*/

  if (m_muonIdHelperTool.retrieve().isFailure()) {
    ATH_MSG_WARNING ("Could not get muon custom ID helper !");
    return StatusCode::FAILURE;
  }

  m_mdtSectorIx      = new std::vector<int>();
  m_mdtStationIx     = new std::vector<int>();
  m_rpcSectorIx      = new std::vector<int>();
  m_rpcStationIx     = new std::vector<int>();
  m_rpcMeasuresPhi   = new std::vector<int>();
  m_tgcStationIx     = new std::vector<int>();
  m_tgcMeasuresPhi   = new std::vector<int>();

  ATH_MSG_INFO ("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode Muon::MuonStationNtupleHelperTool::finalize()
{
  delete m_mdtSectorIx; m_mdtSectorIx=0;
  delete m_mdtStationIx; m_mdtStationIx=0;
  delete m_rpcSectorIx; m_rpcSectorIx=0;
  delete m_rpcStationIx; m_rpcStationIx=0;
  delete m_rpcMeasuresPhi; m_rpcMeasuresPhi=0;
  delete m_tgcStationIx; m_tgcStationIx=0;
  delete m_tgcMeasuresPhi; m_tgcMeasuresPhi=0;
  
  StatusCode sc = AlgTool::finalize();
  return sc;
}

///////////////////////////////////////
/// addNtupleItems
///////////////////////////////////////
StatusCode Muon::MuonStationNtupleHelperTool::addNtupleItems (
    TTree* tree,
    const int& detectorType) const {

    // add items to the ntuple
    if (detectorType==Trk::TrackState::MDT) {

      tree->Branch("MdtSectorIndex",      &m_mdtSectorIx);
      tree->Branch("MdtStationIndex",     &m_mdtStationIx);
    }
    if (detectorType==Trk::TrackState::RPC) {
      tree->Branch("RpcSectorIndex",      &m_rpcSectorIx);
      tree->Branch("RpcStationIndex",     &m_rpcStationIx);
      tree->Branch("RpcMeasuresPhi",      &m_rpcMeasuresPhi);
    }
    if (detectorType==Trk::TrackState::TGC) {
      // TGC sectors are not defined, will cause warning if tried to compute from helper
      tree->Branch("TgcStationIndex",     &m_tgcStationIx);
      tree->Branch("TgcMeasuresPhi",      &m_tgcMeasuresPhi);
    }

    ATH_MSG_VERBOSE ("added items to ntuple.");
    return StatusCode::SUCCESS;
}

////////////////////////////////////////
/// fill hit position data
////////////////////////////////////////
StatusCode Muon::MuonStationNtupleHelperTool::fillMeasurementData (
    const Trk::MeasurementBase* hit,
    const Trk::TrackParameters* ,
    const int& detectorType,
    const bool& /*isOutlier*/) const {

    // todo is this actually called for outliers?

    // identify the detector type:
    Identifier id = Trk::IdentifierExtractor::extract(hit);

    if (detectorType==Trk::TrackState::MDT) {
      m_mdtSectorIx->push_back(m_muonIdHelperTool->sector(id));
      m_mdtStationIx->push_back(m_muonIdHelperTool->stationIndex(id));
    }
    if (detectorType==Trk::TrackState::RPC) {
      m_rpcSectorIx->push_back(m_muonIdHelperTool->sector(id));
      m_rpcStationIx->push_back(m_muonIdHelperTool->stationIndex(id));
      m_rpcMeasuresPhi->push_back(m_muonIdHelperTool->measuresPhi(id));
    }
    if (detectorType==Trk::TrackState::TGC) {
      m_tgcStationIx->push_back(m_muonIdHelperTool->stationIndex(id));
      m_tgcMeasuresPhi->push_back(m_muonIdHelperTool->measuresPhi(id));
    }

    return StatusCode::SUCCESS;
}

StatusCode Muon::MuonStationNtupleHelperTool::resetVariables (
        const int& detectorType ) const {
    if (detectorType==Trk::TrackState::MDT) {
      m_mdtSectorIx->clear();
      m_mdtStationIx->clear();
    }
    if (detectorType==Trk::TrackState::RPC) {
      m_rpcSectorIx->clear();
      m_rpcStationIx->clear();
      m_rpcMeasuresPhi->clear();
    }
    if (detectorType==Trk::TrackState::TGC) {
      m_tgcStationIx->clear();
      m_tgcMeasuresPhi->clear();
    }
    return StatusCode::SUCCESS;
}

StatusCode Muon::MuonStationNtupleHelperTool::fillHoleData (
        const Trk::TrackStateOnSurface&,
        const int&) const {
    // we do nothing with holes
    return StatusCode::SUCCESS;
}

