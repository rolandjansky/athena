"""ComponentAccumulator core ISF_Service configuration
Provides local configuration to be imported without circular dependencies
Complements ISF_ServicesConfigNew

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.SystemOfUnits import mm
from SubDetectorEnvelopes.SubDetectorEnvelopesConfigNew import EnvelopeDefSvcCfg
from ISF_Tools.ISF_ToolsConfigNew import ParticleKillerToolCfg


def ISFEnvelopeDefSvcCfg(ConfigFlags, name="ISF_ISFEnvelopeDefSvc", **kwargs):
    # ATLAS common envlope definitions
    result = ComponentAccumulator()
    kwargs.setdefault("ATLASEnvelopeDefSvc", result.getPrimaryAndMerge(EnvelopeDefSvcCfg(ConfigFlags)).name)
    result.addService(CompFactory.ISF.ISFEnvelopeDefSvc(name, **kwargs), primary = True)
    return result


def GeoIDSvcCfg(ConfigFlags, name="ISF_GeoIDSvc", **kwargs):
    result = ComponentAccumulator()
    # with ISF volume definitions
    kwargs.setdefault("EnvelopeDefSvc", result.getPrimaryAndMerge(ISFEnvelopeDefSvcCfg(ConfigFlags)).name)
    result.addService(CompFactory.ISF.GeoIDSvc(name, **kwargs), primary = True)
    return result


def AFIIEnvelopeDefSvcCfg(ConfigFlags, name="ISF_AFIIEnvelopeDefSvc", **kwargs):
    result = ComponentAccumulator()
    # ATLAS common envlope definitions
    kwargs.setdefault("ISFEnvelopeDefSvc", result.getPrimaryAndMerge(ISFEnvelopeDefSvcCfg(ConfigFlags)).name)
    kwargs.setdefault("InDetMaxExtentZ", 3549.5*mm)
    result.addService(CompFactory.ISF.AFIIEnvelopeDefSvc(name, **kwargs), primary = True)
    return result


def AFIIGeoIDSvcCfg(ConfigFlags, name="ISF_AFIIGeoIDSvc", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("EnvelopeDefSvc", result.getPrimaryAndMerge(AFIIEnvelopeDefSvcCfg(ConfigFlags)).name)
    result.addService(CompFactory.ISF.GeoIDSvc(name, **kwargs), primary = True)
    return result


# previously in LegacySimServicesConfig
def ParticleKillerSvcCfg(ConfigFlags, name="ISF_ParticleKillerSvc", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("Identifier", "ParticleKiller")
    tool = result.popToolsAndMerge(ParticleKillerToolCfg(ConfigFlags))
    kwargs.setdefault("SimulatorTool", tool)
    result.addService(CompFactory.ISF.LegacySimSvc(name, **kwargs), primary = True)
    return result
