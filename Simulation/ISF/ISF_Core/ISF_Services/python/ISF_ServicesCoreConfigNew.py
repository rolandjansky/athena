"""ComponentAccumulator core ISF_Service configuration
Provides local configuration to be imported without circular dependencies
Complements ISF_ServicesConfigNew

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.SystemOfUnits import mm
from SubDetectorEnvelopes.SubDetectorEnvelopesConfigNew import EnvelopeDefSvcCfg
from BarcodeServices.BarcodeServicesConfigNew import BarcodeSvcCfg
from ISF_Tools.ISF_ToolsConfigNew import ParticleKillerToolCfg


def ParticleBrokerSvcNoOrderingCfg(ConfigFlags, name="ISF_ParticleBrokerSvcNoOrdering", **kwargs):
    kwargs.setdefault("EntryLayerTool", "ISF_EntryLayerTool") # TODO
    kwargs.setdefault("GeoIDSvc", "ISF_GeoIDSvc") # TODO
    kwargs.setdefault("AlwaysUseGeoIDSvc", False)
    kwargs.setdefault("ValidateGeoIDs", ConfigFlags.ISF.ValidationMode)
    kwargs.setdefault("ValidationOutput", ConfigFlags.ISF.ValidationMode)
    kwargs.setdefault("ValidationStreamName", "ParticleBroker")
    result = BarcodeSvcCfg(ConfigFlags)
    kwargs.setdefault("BarcodeService", result.getPrimary())
    result.addService(CompFactory.ISF.ParticleBrokerDynamicOnReadIn(name, **kwargs))
    return result


def ParticleBrokerSvcCfg(ConfigFlags, name="ISF_ParticleBrokerSvc", **kwargs):
    #kwargs.setdefault("ParticleOrderingTool", "ISF_InToOutSubDetOrderingTool")
    kwargs.setdefault("ParticleOrderingTool", "ISF_ParticleOrderingTool") # TODO
    return ParticleBrokerSvcNoOrderingCfg(name, **kwargs)


def AFIIParticleBrokerSvcCfg(ConfigFlags, name="ISF_AFIIParticleBrokerSvc", **kwargs):
    kwargs.setdefault("EntryLayerTool", "ISF_AFIIEntryLayerTool") # TODO
    return ParticleBrokerSvcCfg(name, **kwargs)


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
    kwargs.setdefault("SimulatorTool", ParticleKillerToolCfg(ConfigFlags))
    svc = CompFactory.ISF.LegacySimSvc(name, **kwargs)
    result.addService(svc)
    return result
