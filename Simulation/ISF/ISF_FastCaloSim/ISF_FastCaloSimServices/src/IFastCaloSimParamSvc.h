/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_IFASTCALOSIMPARAMSVC_H
#define ISF_IFASTCALOSIMPARAMSVC_H 1

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "ISF_FastCaloSimEvent/TFCSParametrizationBase.h"

namespace ISF {

  class IFastCaloSimParamSvc : virtual public IInterface {
  public:
    /// Creates the InterfaceID and interfaceID() method
    DeclareInterfaceID(IFastCaloSimParamSvc, 1, 0);

    /** Destructor */
    virtual ~IFastCaloSimParamSvc() = default;

    /** Simulation Call */
    virtual StatusCode simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const = 0;
  };

}

#endif //> !ISF_IFASTCALOSIMPARAMSVC_H
