# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Flags used in CI tests

def actsSiSpacePointSeedMakerFlags(flags):
    """flags for Reco_tf with CA used in CI tests: use SiSpacePointSeedMaker tool during reconstruction"""
    flags.Reco.EnableHGTDExtension = False
    flags.ITk.Tracking.convertInDetClusters = True
    flags.ITk.Tracking.produceNewSpacePointContainer = True
    flags.Acts.TrackFinding.useSiSpacePointSeedMaker = True
    flags.ITk.Tracking.writeSeedValNtuple = True

def actsWorkflowFlags(flags):
    """flags for Reco_tf with CA used in CI tests: add Acts workflow to reco sequence"""
    flags.Reco.EnableHGTDExtension = False
    flags.DQ.useTrigger = False
    flags.ITk.Tracking.doTruth = True
    flags.ITk.Tracking.convertInDetClusters = True
    flags.ITk.Tracking.produceNewSpacePointContainer = True
    flags.Output.HISTFileName = "ActsMonitoringOutput.root"

