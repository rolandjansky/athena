/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HepMC_TruthSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_SERVICES_HEPMC_TRUTHSVC_H
#define ISF_SERVICES_HEPMC_TRUTHSVC_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthService.h"

// ISF include
#include "ISF_Interfaces/ITruthSvc.h"

// DetectorDescription
#include "AtlasDetDescr/AtlasRegion.h"

// Barcode
#include "BarcodeInterfaces/Barcode.h"

// McEventCollection
#include "GeneratorObjects/McEventCollection.h"

// forward declarations
class StoreGateSvc;

namespace Barcode {
  class IBarcodeSvc;
}

namespace HepMC {
  class GenEvent;
}

namespace ISF {

  class ITruthStrategy;
  typedef ToolHandleArray<ITruthStrategy>     TruthStrategyArray;

  /** @class HepMC_TruthSvc

      HepMC based version of the ISF::ITruthSvc,
      currently it takes an ITruthIncident base class


      @author Andreas.Salzburger -at- cern.ch , Elmar.Ritsch -at- cern.ch
  */
  class HepMC_TruthSvc : public AthService, public ITruthSvc {
  public:

    //** Constructor with parameters */
    HepMC_TruthSvc( const std::string& name, ISvcLocator* pSvcLocator );

    /** Destructor */
    virtual ~HepMC_TruthSvc();

    /** Athena algorithm's interface method initialize() */
    StatusCode  initialize() override final;
    /** Athena algorithm's interface method finalize() */
    StatusCode  finalize() override final;

    /** Register a truth incident */
    void registerTruthIncident( ITruthIncident& truthincident) override final;

    /** Initialize the Truth Svc at the beginning of each event */
    StatusCode initializeTruthCollection() override final;

    /** Finalize the Truth Svc at the end of each event*/
    StatusCode releaseEvent() override final;

    /** Query the interfaces. **/
    StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

  private:
    /** Record the given truth incident to the MC Truth */
    void recordIncidentToMCTruth( ITruthIncident& truthincident);
    /** Record and end vertex to the MC Truth for the parent particle */
    HepMC::GenVertex *createGenVertexFromTruthIncident( ITruthIncident& truthincident);

    /** Set shared barcode for child particles */
    void setSharedChildParticleBarcode( ITruthIncident& truthincident);

    StoreGateSvc                             *m_storeGate;            //!< The storegate svc
    ServiceHandle<Barcode::IBarcodeSvc>       m_barcodeSvc;           //!< The Barcode service
    Barcode::IBarcodeSvc                     *m_barcodeSvcQuick;      //!< The Barcode service for quick access

    std::string                               m_collectionName;       //!< name of the output McEventCollection
    McEventCollection                        *m_mcEventCollection;    //!< pointer to the McEventCollection
    HepMC::GenEvent                          *m_mcEvent;

    /** the truth strategie applied (as AthenaToolHandle Array) */
    TruthStrategyArray                        m_geoStrategyHandles[AtlasDetDescr::fNumAtlasRegions];
    /** for faster access: using an internal pointer to the actual ITruthStrategy instances */
    ITruthStrategy**                          m_geoStrategies[AtlasDetDescr::fNumAtlasRegions];
    unsigned short                            m_numStrategies[AtlasDetDescr::fNumAtlasRegions];

    /** MCTruth steering */
    bool                                      m_skipIfNoChildren;       //!< do not record incident if numChildren==0
    bool                                      m_skipIfNoParentBarcode;  //!< do not record if parentBarcode==fUndefinedBarcode
    bool                                      m_ignoreUndefinedBarcodes;//!< do/don't abort if retrieve an undefined barcode

    std::string                               m_screenOutputPrefix;
    std::string                               m_screenEmptyPrefix;

    bool                                      m_storeExtraBCs;
    bool                                      m_passWholeVertex;

    std::vector<bool>                         m_forceEndVtxRegionsVec; //!< property containing AtlasRegions for which
                                                                              //   to write end-vtx
    bool                                      m_forceEndVtx[AtlasDetDescr::fNumAtlasRegions]; //!< attach end vertex to
                                                                                                     //   all parent particles if they die

    bool                                      m_quasiStableParticlesIncluded; //!< does this job simulate quasi-stable particles.

    Barcode::ParticleBarcode                  m_secondaryParticleBcOffset;
    Barcode::VertexBarcode                    m_myLowestVertexBC;

  };
}


#endif //> !ISF_SERVICES_HEPMC_TRUTHSVC_H
