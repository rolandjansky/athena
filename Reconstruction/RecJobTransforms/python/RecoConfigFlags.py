# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.AthConfigFlags import AthConfigFlags


def createRecoConfigFlags():
    """Return an AthConfigFlags object with required flags"""
    flags = AthConfigFlags()
    # Different components
    flags.addFlag("Reco.EnableBTagging", lambda prevFlags: prevFlags.Reco.EnableJet)
    flags.addFlag("Reco.EnableCombinedMuon", lambda prevFlags: prevFlags.Detector.EnableMuon and prevFlags.Reco.EnableTracking)
    flags.addFlag("Reco.EnableEgamma",
                  lambda prevFlags: prevFlags.Detector.EnableCalo)
    flags.addFlag("Reco.EnableJet", lambda prevFlags: (
        prevFlags.Detector.EnableCalo
        and prevFlags.Reco.EnableTracking
        and prevFlags.Reco.EnableEgamma
        and prevFlags.Reco.EnableCombinedMuon
        and prevFlags.Reco.EnablePFlow))
    flags.addFlag("Reco.EnablePFlow", lambda prevFlags: prevFlags.Reco.EnableTracking and prevFlags.Detector.EnableCalo)
    flags.addFlag("Reco.EnableTau", lambda prevFlags: prevFlags.Reco.EnableJet)
    flags.addFlag("Reco.EnableTracking", lambda prevFlags: prevFlags.Detector.EnableID or prevFlags.Detector.EnableITk)
    flags.addFlag(
        "Reco.EnableCaloExtension", lambda prevFlags: (
            (
                prevFlags.Reco.EnablePFlow
                or prevFlags.Reco.EnableTau
                or prevFlags.Reco.EnableCombinedMuon
            )
            and prevFlags.Detector.EnableCalo
            and prevFlags.Reco.EnableTracking))
    # this flags enables trigger data decoding (not trigger simulation)
    flags.addFlag("Reco.EnableTrigger", lambda prevFlags: prevFlags.Input.Format == "BS")
    # enable automatically for HI data
    flags.addFlag("Reco.EnableHI", lambda prevFlags: "_hi" in prevFlags.Input.ProjectName)
    # common thinning and other post-processing
    flags.addFlag("Reco.EnablePostProcessing", True)

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
