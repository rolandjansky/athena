# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getMinBiasScintillatorSD(name="MinBiasScintillatorSD", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["LArMgr::MBTS1", "LArMgr::MBTS2"])
    kwargs.setdefault("OutputCollectionNames", ["MBTSHits"])
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.beamType() == 'cosmics' or (hasattr(simFlags, "ReadTR") and simFlags.ReadTR.statusOn):
        kwargs.setdefault("DeltaTHit", [1])
        kwargs.setdefault("DoTOFCorrection", False)
    return CfgMgr.MinBiasScintillatorSDTool(name, **kwargs)
