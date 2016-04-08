/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SimHitSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_SERVICES_SIMHITSVC_H
#define ISF_SERVICES_SIMHITSVC_H 1

#include "ISF_Interfaces/ISimHitSvc.h"
#include "AthenaBaseComps/AthService.h"

#include "G4AtlasInterfaces/ISensitiveDetectorMasterTool.h"
#include "G4AtlasInterfaces/IFastSimulationMasterTool.h"

#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "InDetSimEvent/SiHitCollection.h"
#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
#include "MuonSimEvent/CSCSimHitCollection.h"
#include "MuonSimEvent/GenericMuonSimHitCollection.h"
#include "TileSimEvent/TileHitVector.h"
#include "TrackRecord/TrackRecordCollection.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include <string>
#include <map>

class IdentifierHash;
class AthenaHitsCollectionHelper;
class StoredLArCalibHitContainers;

namespace InDetDD {
  class SiDetectorManager;
  class TRT_DetectorManager;
}

// THistogram service
class ITHistSvc;
// ROOT forward declarations
class TTree;

namespace ISF {

  /** @class SimHitSvc

      @author Andreas.Salzburger -at- cern.ch
  */

  class SimHitSvc : public ISimHitSvc, virtual public AthService {
  public:

    //** Constructor with parameters */
    SimHitSvc( const std::string& name, ISvcLocator* pSvcLocator );

    /** Destructor */
    ~SimHitSvc();

    /** Athena algorithm's interface methods */
    StatusCode  initialize() override final;
    StatusCode  finalize() override final;

    /** Initialize the event */
    StatusCode  initializeEvent() override final;

    /** Insert a SiHit - used for Pixels, SCT - OBSOLETE*/
    void insert(const SiHit& siHit) override final;

    /* Insert a TRT Hit - OBSOLETE*/
    void insert(const TRTHit& trtHit) override final;

    /* Insert a TRTUncompressed Hit - OBSOLETE*/
    void insert(const TRTUncompressedHit& trtHit) override final;

    /** Release the event */
    StatusCode releaseEvent() override final;

    /** Query the interfaces. **/
    StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface ) override final;

  private:

    /** Create the simhits tree - validation mode only */
    StatusCode createSimHitsTree();
    /** Fill the simhits tree - validation mode only */
    void fillSimHitsTree();
    //** Add information from HepMcParticleLink to TTree - common for all SimHit types */
    void addHepMcParticleLinkInfoToTree(HepMcParticleLink &HMPL);

    /** Validation output with histogram service */
    bool                                                         m_validationOutput; //!< turn validation mode on/off
    ServiceHandle<ITHistSvc>                                     m_thistSvc;         //!< the histogram service
    std::string                                                  m_validationStream; //!< validation THist stream name
    /** Validation output TTree (+variables) */
    TTree                                                       *m_t_simHits;        //!< ROOT tree containing simulated hit info
    int                                                          m_mdt_n;
    int                                                          m_rpc_n;
    int                                                          m_tgc_n;
    int                                                          m_csc_n;
    mutable int                                                  m_pileup;
    mutable int                                                  m_type;
    mutable int                                                  m_id;
    mutable int                                                  m_mother;
    mutable int                                                  m_barcode;
    mutable float                                                m_time;
    mutable float                                                m_drift;
    mutable float                                                m_edeposit;
    mutable float                                                m_momentum;
    mutable float                                                m_theta;
    mutable float                                                m_phi;
    mutable float                                                m_eta;


    AthenaHitsCollectionHelper*                                  m_hitCollectionHelper;
    //Inner Detector
    bool                                                         m_simulateID;
    SiHitCollection*                                             m_bcmHits;
    std::string                                                  m_bcmHitCollectionName;
    SiHitCollection*                                             m_blmHits;
    std::string                                                  m_blmHitCollectionName;
    SiHitCollection*                                             m_pixHits;
    std::string                                                  m_pixHitCollectionName;
    SiHitCollection*                                             m_sctHits;
    std::string                                                  m_sctHitCollectionName;
    TRTUncompressedHitCollection*                                m_trtHits;
    std::string                                                  m_trtHitCollectionName;
    SiHitCollection*                                             m_pixPileupHits;
    std::string                                                  m_pixPileupHitCollectionName;
    SiHitCollection*                                             m_sctPileupHits;
    std::string                                                  m_sctPileupHitCollectionName;
    TRTUncompressedHitCollection*                                m_trtPileupHits;
    std::string                                                  m_trtPileupHitCollectionName;
    bool                                                         m_separateInDetPileupHits;

    //Calorimeter
    bool                                                         m_simulateCalo;
    std::string                                                  m_embHitCollectionName;
    std::string                                                  m_emecHitCollectionName;
    std::string                                                  m_fcalHitCollectionName;
    std::string                                                  m_hecHitCollectionName;
    std::string                                                  m_miniFcalHitCollectionName;
    StoredLArCalibHitContainers*                                 m_storedCalibContainers;
    std::string                                                  m_activeLArHitCollectionName;
    std::string                                                  m_inactiveLArHitCollectionName;
    std::string                                                  m_deadLArHitCollectionName;
    TileHitVector*                                               m_tileHitVecHits;
    std::string                                                  m_tileHitVecHitCollectionName;
    TileHitVector*                                               m_mbtsHits;
    std::string                                                  m_mbtsHitCollectionName;
    CaloCalibrationHitContainer*                                 m_tileActiveCellCalibHits;
    std::string                                                  m_tileActiveCellCalibHitCollectionName;
    CaloCalibrationHitContainer*                                 m_tileInactiveCellCalibHits;
    std::string                                                  m_tileInactiveCellCalibHitCollectionName;
    CaloCalibrationHitContainer*                                 m_tileDeadMaterialCalibHits;
    std::string                                                  m_tileDeadMaterialCalibHitCollectionName;
    bool m_doMiniFcal;
    bool m_doTileCalibHits;

    //MuonSpectrometer
    bool                                                         m_simulateMS;
    MDTSimHitCollection*                                         m_mdtHits;
    std::string                                                  m_mdtHitCollectionName;
    RPCSimHitCollection*                                         m_rpcHits;
    std::string                                                  m_rpcHitCollectionName;
    TGCSimHitCollection*                                         m_tgcHits;
    std::string                                                  m_tgcHitCollectionName;
    CSCSimHitCollection*                                         m_cscHits;
    std::string                                                  m_cscHitCollectionName;
    GenericMuonSimHitCollection*                                 m_muonHits;
    std::string                                                  m_muonHitCollectionName;

    TrackRecordCollection*                                         m_caloEntryLayerTracks;
    std::string                                                    m_caloEntryLayerTrackCollectionName;

    TrackRecordCollection*                                         m_muonEntryLayerTracks;
    std::string                                                    m_muonEntryLayerTrackCollectionName;

    TrackRecordCollection*                                         m_muonExitLayerTracks;
    std::string                                                    m_muonExitLayerTrackCollectionName;

    TrackRecordCollection*                                         m_cosmicPerigeeTracks;
    std::string                                                    m_cosmicPerigeeTrackCollectionName;

    ToolHandle<ISensitiveDetectorMasterTool> m_senDetTool;
    ToolHandle<IFastSimulationMasterTool> m_fastSimTool;

  };
}


#endif //> !ISF_SERVICES_SIMHITSVC_H
