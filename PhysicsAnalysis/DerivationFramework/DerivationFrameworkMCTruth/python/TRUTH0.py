# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#!/usr/bin/env python
# TRUTH0.py - direct and complete translation of HepMC in EVNT to xAOD truth 
# No additional information is added

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def TRUTH0Cfg(ConfigFlags):
    """Main config for TRUTH0"""
    acc = ComponentAccumulator()
    
    # Ensure EventInfoCnvAlg is scheduled
    if "EventInfo#EventInfo" not in ConfigFlags.Input.TypedCollections:    
        from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoCnvAlgCfg
        acc.merge(EventInfoCnvAlgCfg(ConfigFlags, inputKey="McEventInfo", outputKey="EventInfo", disableBeamSpot=True))
 
    # Decide what kind of input HepMC container we are dealing with
    # and schedule the xAOD converter appropriately
    from xAODTruthCnv.xAODTruthCnvConfig import GEN_EVNT2xAODCfg
    if "McEventCollection#GEN_EVENT" in ConfigFlags.Input.TypedCollections:
        acc.merge(GEN_EVNT2xAODCfg(ConfigFlags,name="GEN_EVNT2xAOD",AODContainerName="GEN_EVENT"))
    elif "McEventCollection#TruthEvent" in ConfigFlags.Input.TypedCollections:
        acc.merge(GEN_EVNT2xAODCfg(name="GEN_EVNT2xAOD",AODContainerName="TruthEvent"))
    else:
        raise RuntimeError("No recognised HepMC truth information found in the input")
 
    # Contents
    TRUTH0Items = ["xAOD::EventInfo#*",
                   "xAOD::EventAuxInfo#*",
                   "xAOD::TruthEventContainer#*",
                   "xAOD::TruthEventAuxContainer#*",
                   "xAOD::TruthVertexContainer#*",
                   "xAOD::TruthVertexAuxContainer#*",
                   "xAOD::TruthParticleContainer#*",
                   "xAOD::TruthParticleAuxContainer#*"]
    # Metadata
    TRUTH0MetaDataItems = [ "xAOD::TruthMetaDataContainer#TruthMetaData", "xAOD::TruthMetaDataAuxContainer#TruthMetaDataAux." ]

    # Create output stream 
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(OutputStreamCfg(ConfigFlags, "DAOD_TRUTH0", ItemList=TRUTH0Items, MetadataItemList=TRUTH0MetaDataItems))
 
    return acc
