# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# This allows us to convert Run 2 ESD Trk::SegmentCollection "MuonSegments" to
# Run 3 "TrackMuonSegments"
def MuonSegmentNameFixCfg(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    cfg = ComponentAccumulator()
    if 'Trk::SegmentCollection#MuonSegments' in flags.Input.TypedCollections:
        from SGComps.AddressRemappingConfig import InputRenameCfg
        cfg.merge(InputRenameCfg("Trk::SegmentCollection", "MuonSegments", "TrackMuonSegments"))
    return cfg
