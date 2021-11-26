# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createRecoConfigFlags():
    """Return an AthConfigFlags object with required flags"""
    flags = AthConfigFlags()
    # Different components
    flags.addFlag("Reco.EnableBTagging", True)
    flags.addFlag("Reco.EnableCalo", True)
    flags.addFlag("Reco.EnableCombinedMuon", True)
    flags.addFlag("Reco.EnableEgamma", True)
    flags.addFlag("Reco.EnableJet", True)
    flags.addFlag("Reco.EnableMuon", True)
    flags.addFlag("Reco.EnablePFlow", True)
    flags.addFlag("Reco.EnableTau", True)
    flags.addFlag("Reco.EnableTracking", True)
    flags.addFlag("Reco.EnableTrigger", lambda prevFlags : prevFlags.Input.Format == "BS") # this flags enables trigger data decoding (not trigger simulation)
    flags.addFlag("Reco.EnableHI", lambda prevFlags: "_hi" in prevFlags.Input.ProjectName ) # enable automatically for HI data

    return flags


def recoRunArgsToFlags(runArgs, flags):
    if hasattr(runArgs, "RunNumber"):
        flags.Input.RunNumber = runArgs.RunNumber
        flags.Input.OverrideRunNumber = True

    if hasattr(runArgs, "projectName"): 
        flags.Input.projectName = runArgs.projectName

    # TODO: not handled yet
    # --autoConfiguration
    # --trigStream
    # --topOptions
    # --valid

    # --AMITag
    # --userExec
    # --triggerConfig
    # --trigFilterList
