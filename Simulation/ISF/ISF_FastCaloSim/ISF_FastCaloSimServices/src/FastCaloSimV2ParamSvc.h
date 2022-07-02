/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMV2PARAMSVC_H
#define ISF_FASTCALOSIMV2PARAMSVC_H 1

#include "ISF_FastCaloSimInterfaces/IFastCaloSimParamSvc.h"
#include "AthenaBaseComps/AthService.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "ISF_FastCaloSimParametrization/CaloGeometryFromCaloDDM.h"
#ifdef USE_GPU
#include "ISF_FastCaloGpu/GeoLoadGpu.h"
#endif

//forward declarations
class TFCSParametrizationBase;

namespace ISF {

  class FastCaloSimV2ParamSvc : public extends<AthService, IFastCaloSimParamSvc> {
  public:
    /** Constructor with parameters */
    FastCaloSimV2ParamSvc(const std::string& name, ISvcLocator* pSvcLocator);

    /** Destructor */
    virtual ~FastCaloSimV2ParamSvc() = default;

    /** Athena algorithm's interface methods */
    virtual StatusCode  initialize() override final;

    /** Simulation Call */
    virtual StatusCode simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const override final;

  private:

#ifdef USE_GPU
    // for FCS-GPU
    //construct the geometry with GPU EDM
    bool convert_cellmap(t_cellmap* cellmap, t_cellmap_Gpu* newcellmap);
    void region_data_cpy( CaloGeometryLookup* glkup, GeoRegion* gr ) ;
    GeoLoadGpu* m_gl;
    GeoLoadGpu m_glg;
    void* m_rd4h;
    t_cellmap_Gpu    m_cellmap_gpu;
#endif
    /** @brief The standard @c StoreGateSvc/DetectorStore
     * Returns (kind of) a pointer to the @c StoreGateSvc
     */
    const ServiceHandle<StoreGateSvc>& detStore() const;

    /// Pointer to StoreGate (detector store by default)
    ServiceHandle<StoreGateSvc> m_detStore;

    std::string m_paramsFilename{"TFCSparam.root"};
    std::string m_paramsObject{"SelPDGID"};

    TFCSParametrizationBase*  m_param{};

    std::unique_ptr<CaloGeometryFromCaloDDM> m_caloGeo{};

    bool m_printParametrization{false};
    bool m_CompressMemory{true};
    bool m_runOnGPU{false};
  };

}

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline
const ServiceHandle<StoreGateSvc>& ISF::FastCaloSimV2ParamSvc::detStore() const
{ return m_detStore; }

#endif //> !ISF_FASTCALOSIMV2PARAMSVC_H
