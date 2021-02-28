/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMV2PARAMSVC_H
#define ISF_FASTCALOSIMV2PARAMSVC_H 1

#include "IFastCaloSimParamSvc.h"
#include "AthenaBaseComps/AthService.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "ISF_FastCaloSimParametrization/CaloGeometryFromCaloDDM.h"

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

    /** @brief The standard @c StoreGateSvc/DetectorStore
     * Returns (kind of) a pointer to the @c StoreGateSvc
     */
    ServiceHandle<StoreGateSvc>& detStore() const;

    typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
    /// Pointer to StoreGate (detector store by default)
    mutable StoreGateSvc_t m_detStore;

    std::string m_paramsFilename{"TFCSparam.root"};
    std::string m_paramsObject{"SelPDGID"};

    TFCSParametrizationBase*  m_param{};

    std::unique_ptr<CaloGeometryFromCaloDDM> m_caloGeo{};

    bool m_printParametrization{false};
    bool m_CompressMemory{true};
  };

}

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline
ServiceHandle<StoreGateSvc>& ISF::FastCaloSimV2ParamSvc::detStore() const
{ return m_detStore; }

#endif //> !ISF_FASTCALOSIMV2PARAMSVC_H
