# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = "Prepare isolation output list (density)"

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def IsoOutputCfg(flags, name="IsoOutputList"):

    mlog = logging.getLogger(name)
    mlog.info('Starting isolation Output configuration')

    acc = ComponentAccumulator()

    toESD = []
    toAOD = []

    DensityAODSupp = "-DensityArea.-DensitySigma"
    
    if flags.Detector.EnableCalo:
        toESD += [
            "xAOD::EventShape#TopoClusterIsoCentralEventShape",
            "xAOD::EventShapeAuxInfo#TopoClusterIsoCentralEventShapeAux."]
        toESD += [
            "xAOD::EventShape#TopoClusterIsoForwardEventShape",
            "xAOD::EventShapeAuxInfo#TopoClusterIsoForwardEventShapeAux."]
    if flags.Reco.EnablePFlow:
        toESD += [
            "xAOD::EventShape#NeutralParticleFlowIsoCentralEventShape",
            "xAOD::EventShapeAuxInfo#NeutralParticleFlowIsoCentralEventShapeAux."]
        toESD += [
            "xAOD::EventShape#NeutralParticleFlowIsoForwardEventShape",
            "xAOD::EventShapeAuxInfo#NeutralParticleFlowIsoForwardEventShapeAux."]
            
    toAOD = [ i.replace('Aux.',f'Aux.{DensityAODSupp}') for i in toESD ]

    if flags.Output.doWriteESD:
        from OutputStreamAthenaPool.OutputStreamConfig import addToESD
        acc.merge(addToESD(flags, toESD))
        mlog.info('isoDensityESDList: %s ', toESD)

    if flags.Output.doWriteAOD:
        from OutputStreamAthenaPool.OutputStreamConfig import addToAOD
        acc.merge(addToAOD(flags, toAOD))
        mlog.info('isoDensityAODList: %s ', toAOD)

    mlog.info("Isolation Output configured")
    return acc
