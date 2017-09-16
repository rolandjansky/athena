/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_ALGS_SIMHITTREECREATOR_H
#define ISF_ALGS_SIMHITTREECREATOR_H 1
// FrameWork includes
#include "StoreGate/ReadHandleKey.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"

// Hit Collection includes
#include "InDetSimEvent/SiHitCollection.h"
#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
#include "MuonSimEvent/CSCSimHitCollection.h"

// THistogram service
class ITHistSvc;
// ROOT forward declarations
class TTree;

namespace ISF {

  class SimHitTreeCreator : virtual public AthAlgorithm {

  public:
    /** Constructor with parameters */
    SimHitTreeCreator( const std::string& name, ISvcLocator* pSvcLocator );

    /** Destructor */
    virtual ~SimHitTreeCreator();

    /** Athena algorithm's interface method initialize() */
    StatusCode  initialize() override final;
    /** Athena algorithm's interface method execute() */
    StatusCode  execute() override final;

  private:

    /** Create the simhits tree - validation mode only */
    StatusCode createSimHitsTree();
    /** Fill the simhits tree - validation mode only */
    StatusCode fillSimHitsTree();
    //** Add information from HepMcParticleLink to TTree - common for all SimHit types */
    void addHepMcParticleLinkInfoToTree(HepMcParticleLink &HMPL);

    ServiceHandle<ITHistSvc>                                     m_thistSvc;         //!< the histogram service
    std::string                                                  m_validationStream; //!< validation THist stream name
    /** Validation output TTree (+variables) */
    TTree                                                       *m_t_simHits;        //!< ROOT tree containing simulated hit info
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
    SG::ReadHandleKey<SiHitCollection>                              m_bcmHits;
    SG::ReadHandleKey<SiHitCollection>                              m_blmHits;
    SG::ReadHandleKey<SiHitCollection>                              m_pixHits;
    SG::ReadHandleKey<SiHitCollection>                              m_sctHits;
    SG::ReadHandleKey<TRTUncompressedHitCollection>                 m_trtHits;
    SG::ReadHandleKey<SiHitCollection>                              m_pixPileupHits;
    SG::ReadHandleKey<SiHitCollection>                              m_sctPileupHits;
    SG::ReadHandleKey<TRTUncompressedHitCollection>                 m_trtPileupHits;

    //MuonSpectrometer
    SG::ReadHandleKey<MDTSimHitCollection>                          m_mdtHits;
    SG::ReadHandleKey<RPCSimHitCollection>                          m_rpcHits;
    SG::ReadHandleKey<TGCSimHitCollection>                          m_tgcHits;
    SG::ReadHandleKey<CSCSimHitCollection>                          m_cscHits;
  };
}

#endif //> !ISF_ALGS_SIMHITTREECREATOR_H
