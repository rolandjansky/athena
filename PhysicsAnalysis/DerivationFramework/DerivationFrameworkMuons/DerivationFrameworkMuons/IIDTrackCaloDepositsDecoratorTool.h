/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef IIDTRACKCALODEPOSITSDECORATORTOOL_H_
#define IIDTRACKCALODEPOSITSDECORATORTOOL_H_

#include "GaudiKernel/IAlgTool.h"
#include "xAODBase/IParticle.h"

class IIDTrackCaloDepositsDecoratorTool : virtual public IAlgTool {
public:
    static const InterfaceID& interfaceID() {
        static const InterfaceID IID("IIDTrackCaloDepositsDecoratorTool", 1, 0);
        return IID;
    }
    virtual ~IIDTrackCaloDepositsDecoratorTool() = default;

    virtual StatusCode decorate(const xAOD::IParticle* part) const = 0;
};

#endif /* IIDTRACKISOLATIONDECORATORTOOL_H_ */
