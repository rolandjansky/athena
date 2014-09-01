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

#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "FadsSensitiveDetector/SensitiveDetectorCatalog.h"
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
class StoredLArHitContainers;
class StoredLArCalibHitContainers;
class SensitiveDetectorEntry;

namespace InDetDD {
    class SiDetectorManager;
    class TRT_DetectorManager;
}

namespace FADS {
  class FadsSensitiveDetector;

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
      StatusCode  initialize();
      StatusCode  finalize();

      /** Initialize the event */
      StatusCode  initializeEvent();

      /** Insert a SiHit - used for Pixels, SCT */
      void insert(const SiHit& siHit);

      /* Insert a TRT Hit */
      void insert(const TRTHit& trtHit);

      /* Insert a TRTUncompressed Hit */
      void insert(const TRTUncompressedHit& trtHit);

      /** Release the event */
      StatusCode releaseEvent();

      /** Query the interfaces. **/
          StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

    private:

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
        SiHitCollection*                                             m_pixHits;
        std::string                                                  m_pixHitCollectionName;
        SiHitCollection*                                             m_sctHits;
        std::string                                                  m_sctHitCollectionName;
        TRTUncompressedHitCollection*                                m_trtHits;
        std::string                                                  m_trtHitCollectionName;
        SiHitCollection*                                             m_blmHits;
        std::string                                                  m_blmHitCollectionName;
        SiHitCollection*                                             m_bcmHits;
        std::string                                                  m_bcmHitCollectionName;

        std::string                                                  m_embHitCollectionName;
        std::string                                                  m_emecHitCollectionName;
        std::string                                                  m_fcalHitCollectionName;
        std::string                                                  m_hecHitCollectionName;
        std::string                                                  m_miniFcalHitCollectionName;
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

        StoredLArHitContainers*      m_storedContainers;
        StoredLArCalibHitContainers* m_storedCalibContainers;

        //std::map<IdentifierHash, SiHitCollection* >                  m_pixHits;
        //std::map<IdentifierHash, SiHitCollection* >                  m_sctHits;
        //std::map<IdentifierHash, TRTUncompressedHitCollection* >     m_trtHits;
        //
        //// we need an ID helper and Detector Managers
        //const InDetDD::SiDetectorManager*                            m_pixMgr;
        //std::string                                                  m_pixMgrLocation;
        //const InDetDD::SiDetectorManager*                            m_sctMgr;
        //std::string                                                  m_sctMgrLocation;
        //const InDetDD::TRT_DetectorManager*                          m_trtMgr;
        //std::string                                                  m_trtMgrLocation;

        bool m_doMiniFcal;
        bool m_doTileCalibHits;

        TrackRecordCollection*                                         m_caloEntryLayerTracks;
        std::string                                                    m_caloEntryLayerTrackCollectionName;

        TrackRecordCollection*                                         m_muonEntryLayerTracks;
        std::string                                                    m_muonEntryLayerTrackCollectionName;

        TrackRecordCollection*                                         m_muonExitLayerTracks;
        std::string                                                    m_muonExitLayerTrackCollectionName;

        TrackRecordCollection*                                         m_cosmicPerigeeTracks;
        std::string                                                    m_cosmicPerigeeTrackCollectionName;

        std::vector<FADS::FadsSensitiveDetector*> m_sd;
  };
}


#endif //> !ISF_SERVICES_SIMHITSVC_H
