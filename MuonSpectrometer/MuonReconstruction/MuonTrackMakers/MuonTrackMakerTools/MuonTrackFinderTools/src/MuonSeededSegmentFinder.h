/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSEEDEDSEGMENTFINDER_H
#define MUON_MUONSEEDEDSEGMENTFINDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonSeededSegmentFinder.h"
#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkGeometry/MagneticFieldMode.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "TrkExInterfaces/IPropagator.h"

#include "TrkParameters/TrackParameters.h"

#include <string>
#include <set>
#include <vector>

namespace Muon {

/**
   @brief seeded segment finder that gets its data directly from storegate for a give set of Identifiers or IdentifierHashes

*/
class MuonSeededSegmentFinder : virtual public IMuonSeededSegmentFinder, public AthAlgTool {
public:
  /** @brief constructor */
  MuonSeededSegmentFinder(const std::string&, const std::string&, const IInterface*);

  /** @brief destructor */
  ~MuonSeededSegmentFinder()=default;

  /** @brief AlgTool initilize */
  StatusCode initialize();

  /** @brief find segments in a set of chambers starting from seeding TrackParameters */
  std::unique_ptr<Trk::SegmentCollection> find( const Trk::TrackParameters& pars, const std::set<Identifier>& chIds ) const;

  /** @brief find segments in a set of chambers starting from seeding TrackParameters (version with Hashes) */
  std::unique_ptr<Trk::SegmentCollection> find( const Trk::TrackParameters& pars, const std::set<IdentifierHash>& chIdHs ) const;

  /** @brief find segments in a set of MdtPrepData starting from seeding TrackParameters */
  std::unique_ptr<Trk::SegmentCollection> find( const Trk::TrackParameters& pars, const std::vector<const MdtPrepData*>& mdtPrds ) const;


  /** @brief retrieve MDT PRD collections for the given hashes */
  void extractMdtPrdCols( const std::set<IdentifierHash>& chIdHs, std::vector<const MdtPrepDataCollection*>& target  ) const;

  /** @brief retrieve RPC PRD collections for the given hashes */
  void extractRpcPrdCols( const std::set<IdentifierHash>& chIdHs, std::vector<const RpcPrepDataCollection*>& target ) const;

  /** @brief retrieve TGC PRD collections for the given hashes */
  void extractTgcPrdCols( const std::set<IdentifierHash>& chIdHs, std::vector<const TgcPrepDataCollection*>& target ) const;

  /** @brief retrieve CSC PRD collections for the given hashes */
  void extractCscPrdCols( const std::set<IdentifierHash>& chIdHs, std::vector<const CscPrepDataCollection*>& target ) const;

  // New Small Wheel

  /** @brief retrieve STGC PRD collections for the given hashes */
  void extractsTgcPrdCols( const std::set<IdentifierHash>& chIdHs, std::vector<const sTgcPrepDataCollection*>& target ) const;

  /** @brief retrieve MM PRD collections for the given hashes */
  void extractMMPrdCols( const std::set<IdentifierHash>& chIdHs, std::vector<const MMPrepDataCollection*>& target ) const;


private:
  /** @brief retrieve the MdtPrepDataCollections for the give Identifiers */
  std::vector<const MdtPrepData*> extractPrds( const std::set<Identifier>& chIds ) const;

  /** @brief retrieve the MdtPrepDataCollections for the give Identifier Hashes */
  std::vector<const MdtPrepData*> extractPrds( const std::set<IdentifierHash>& chIdHs ) const;

  /** @brief select a set of Mdt hits and calibrate them */
  void selectAndCalibrate( const Trk::TrackParameters& pars,
                           const std::vector<const MdtPrepData*>& mdtPrdCols,
                           std::vector<const MdtDriftCircleOnTrack*>& mdtROTs,
                           bool& doHoleSearch ) const;

  /** @brief select and calibrate a single MdtPrepData */
  const MdtDriftCircleOnTrack* handleMdtPrd( const Trk::TrackParameters& pars,
      const MdtPrepData& mdtPrd,
      bool& doHoleSearch ) const;


  SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey {this, "DetectorManagerKey", 
      "MuonDetectorManager", 
      "Key of input MuonDetectorManager condition data"};    

  ToolHandle<Muon::IMuonSegmentMaker>              m_segMaker{this, "SegmentMaker", "Muon::DCMathSegmentMaker/DCMathSegmentMaker"};           //!< actual segment maker with hole search
  ToolHandle<Muon::IMuonSegmentMaker>              m_segMakerNoHoles{this, "SegmentMakerNoHoles", "Muon::DCMathSegmentMaker/DCMathSegmentMaker"};    //!< actual segment maker no hole search
  ToolHandle<Trk::IPropagator>                     m_propagator{this, "Propagator", "Trk::RungeKuttaPropagator/AtlasRungeKuttaPropagator"};         //!< propagator
  ToolHandle<Muon::IMdtDriftCircleOnTrackCreator>  m_mdtRotCreator{this, "MdtRotCreator", "Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator"};      //!< IMdtDriftCircleOnTrackCreator
  Trk::MagneticFieldProperties                     m_magFieldProperties; //!< magnetic field properties
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  ToolHandle<Muon::MuonEDMPrinterTool>             m_printer{this, "Printer", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"};            //!< EDM printer tool

  SG::ReadHandleKey<Muon::MdtPrepDataContainer> m_key_mdt{this,"MdtPrepDataContainer","MDT_DriftCircles","MDT PRDs"};
  SG::ReadHandleKey<Muon::CscPrepDataContainer> m_key_csc{this,"CscPrepDataContainer","CSC_Clusters","CSC PRDS"};
  SG::ReadHandleKey<Muon::TgcPrepDataContainer> m_key_tgc{this,"TgcPrepDataContainer","TGC_Measurements","TGC PRDs"};
  SG::ReadHandleKey<Muon::RpcPrepDataContainer> m_key_rpc{this,"RpcPrepDataContainer","RPC_Measurements","RPC PRDs"};
  SG::ReadHandleKey<Muon::sTgcPrepDataContainer> m_key_stgc{this,"sTgcPrepDataContainer","STGC_Measurements","sTGC PRDs"};
  SG::ReadHandleKey<Muon::MMPrepDataContainer> m_key_mm{this,"MMPrepDataContainer","MM_Measurements","MM PRDs"};

  Gaudi::Property<double> m_adcCut {this, "AdcCut", 50};
  Gaudi::Property<double> m_maxSigma {this, "MdtSigmaFromTrack", 3};
};

}

#endif
