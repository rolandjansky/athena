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
    result = EnvelopeDefSvcCfg(ConfigFlags)
    # ATLAS common envlope definitions
    kwargs.setdefault("ATLASEnvelopeDefSvc", result.getService("AtlasGeometry_EnvelopeDefSvc"))
    result.addService(CompFactory.ISF.ISFEnvelopeDefSvc(name, **kwargs))
    return result


def GeoIDSvcCfg(ConfigFlags, name="ISF_GeoIDSvc", **kwargs):
    result = ISFEnvelopeDefSvcCfg(ConfigFlags)
    # with ISF volume definitions
    kwargs.setdefault("EnvelopeDefSvc", result.getService("ISF_ISFEnvelopeDefSvc"))
    result.addService(CompFactory.ISF.GeoIDSvc(name, **kwargs))
    return result


def AFIIEnvelopeDefSvcCfg(ConfigFlags, name="ISF_AFIIEnvelopeDefSvc", **kwargs):
    result = EnvelopeDefSvcCfg(ConfigFlags)
    # ATLAS common envlope definitions
    kwargs.setdefault("ISFEnvelopeDefSvc", result.getService("AtlasGeometry_EnvelopeDefSvc"))
    kwargs.setdefault("InDetMaxExtentZ", 3549.5*mm)
    result.addService(CompFactory.ISF.AFIIEnvelopeDefSvc(name, **kwargs))
    return result


def AFIIGeoIDSvcCfg(ConfigFlags, name="ISF_AFIIGeoIDSvc", **kwargs):
    result = AFIIEnvelopeDefSvcCfg(ConfigFlags)
    kwargs.setdefault("EnvelopeDefSvc", result.getService("ISF_AFIIEnvelopeDefSvc"))
    result.addService(CompFactory.ISF.GeoIDSvc(name, **kwargs))
    return result


# previously in LegacySimServicesConfig
def ParticleKillerSvcCfg(ConfigFlags, name="ISF_ParticleKillerSvc", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("Identifier", "ParticleKiller")
    tool = result.popToolsAndMerge(ParticleKillerToolCfg(ConfigFlags))
    kwargs.setdefault("SimulatorTool", tool)
    svc = CompFactory.ISF.LegacySimSvc(name, **kwargs)
    result.addService(svc)
    return result
