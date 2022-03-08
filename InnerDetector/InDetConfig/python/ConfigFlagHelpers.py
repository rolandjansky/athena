# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration


def innerDetectorOnly(flags):
    """Only run inner detector tracking"""
    flags.Detector.EnableMuon = False
    flags.Detector.EnableCalo = True
    flags.Reco.EnableTracking = True
    flags.Reco.EnableCombinedMuon = False
    flags.Reco.EnableEgamma = False
    flags.Reco.EnablePFlow = False
    flags.Reco.EnableTau = False
    flags.Reco.EnableJet = False
    flags.Reco.EnableBTagging = False
    flags.Reco.EnableTrigger = False
    flags.Reco.EnablePostProcessing = False


def innerDetectorOnlyLegacy():
    from RecExConfig.RecFlags import rec
    from RecExConfig.RecAlgsFlags import recAlgs
    from AthenaMonitoring.DQMonFlags import DQMonFlags
    from JetRec.JetRecFlags import jetFlags
    from ParticleBuilderOptions.AODFlags import AODFlags

    rec.doCalo.set_Value_and_Lock(True)
    rec.doInDet.set_Value_and_Lock(True)
    rec.doEgamma.set_Value_and_Lock(False)
    rec.doTrigger.set_Value_and_Lock(False)
    rec.doTau.set_Value_and_Lock(False)
    rec.doMuon.set_Value_and_Lock(False)
    rec.doMuonCombined.set_Value_and_Lock(False)
    rec.doBTagging.set_Value_and_Lock(False)
    rec.doCaloRinger.set_Value_and_Lock(False)
    rec.doAlfa.set_Value_and_Lock(False)
    rec.doAFP.set_Value_and_Lock(False)
    rec.doLucid.set_Value_and_Lock(False)

    recAlgs.doMuonSpShower.set_Value_and_Lock(False)
    recAlgs.doEFlow.set_Value_and_Lock(False)
    recAlgs.doEFlowJet.set_Value_and_Lock(False)
    recAlgs.doMissingET.set_Value_and_Lock(False)
    recAlgs.doMissingETSig.set_Value_and_Lock(False)

    DQMonFlags.doMonitoring.set_Value_and_Lock(False)

    jetFlags.Enabled.set_Value_and_Lock(False)

    AODFlags.ThinGeantTruth.set_Value_and_Lock(False)
    AODFlags.egammaTrackSlimmer.set_Value_and_Lock(False)
    AODFlags.ThinTRTStandaloneTracks.set_Value_and_Lock(False)
    AODFlags.ThinInDetForwardTrackParticles.set_Value_and_Lock(False)
    AODFlags.ThinNegativeEnergyNeutralPFOs.set_Value_and_Lock(False)
