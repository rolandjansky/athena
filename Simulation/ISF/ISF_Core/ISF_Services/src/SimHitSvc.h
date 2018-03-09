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

#include "InDetSimEvent/SiHitCollection.h"
#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
#include "MuonSimEvent/CSCSimHitCollection.h"

#include "StoreGate/ReadHandle.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include <string>
#include <map>

class IdentifierHash;

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

  class SimHitSvc : public extends<AthService, ISimHitSvc> {
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


    //Inner Detector
    SG::ReadHandle<SiHitCollection>                              m_bcmHits;
    SG::ReadHandle<SiHitCollection>                              m_blmHits;
    SG::ReadHandle<SiHitCollection>                              m_pixHits;
    SG::ReadHandle<SiHitCollection>                              m_sctHits;
    SG::ReadHandle<TRTUncompressedHitCollection>                 m_trtHits;
    SG::ReadHandle<SiHitCollection>                              m_pixPileupHits;
    SG::ReadHandle<SiHitCollection>                              m_sctPileupHits;
    SG::ReadHandle<TRTUncompressedHitCollection>                 m_trtPileupHits;
    bool                                                         m_separateInDetPileupHits;

    //MuonSpectrometer
    SG::ReadHandle<MDTSimHitCollection>                          m_mdtHits;
    SG::ReadHandle<RPCSimHitCollection>                          m_rpcHits;
    SG::ReadHandle<TGCSimHitCollection>                          m_tgcHits;
    SG::ReadHandle<CSCSimHitCollection>                          m_cscHits;

    ToolHandle<ISensitiveDetectorMasterTool> m_senDetTool;
    ToolHandle<IFastSimulationMasterTool> m_fastSimTool;

  };
}


#endif //> !ISF_SERVICES_SIMHITSVC_H
