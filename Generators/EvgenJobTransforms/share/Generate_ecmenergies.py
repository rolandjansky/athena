
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## Propagate energy argument to the generators
# TODO: Standardise energy setting in the GenModule interface

eCM = runArgs.ecmEnergy
eBeam = runArgs.ecmEnergy*0.5

if runArgs.trfSubstepName == 'afterburn':
    from PyJobTransforms.trfLogger import msg
    msg.info("Running in Afterburner mode, beam eCM is not set")
elif 'Pythia' in evgenConfig.generators:
    genSeq.Pythia.PythiaCommand.append("pyinit win %s" % eCM)
elif 'PythiaB' in evgenConfig.generators:
    genSeq.PythiaB.PythiaCommand.append("pyinit win %s" % eCM)
elif 'PythiaRhad' in evgenConfig.generators:
    genSeq.PythiaRhad.PythiaCommand.append("pyinit win %s" % eCM)
elif 'PythiaChL' in evgenConfig.generators:
    genSeq.PythiaChL.PythiaCommand.append("pyinit win %s" % eCM)
elif 'PythiaGS' in evgenConfig.generators:
    genSeq.PythiaGS.PythiaCommand.append("pyinit win %s" % eCM)
elif 'PythiaResMod' in evgenConfig.generators:
    genSeq.PythiaResMod.PythiaCommand.append("pyinit win %s" % eCM)
elif 'PythiaTopMdiff' in evgenConfig.generators:
    genSeq.PythiaTopMdiff.PythiaCommand.append("pyinit win %s" % eCM)
elif 'Pythia8' in evgenConfig.generators:
    genSeq.Pythia8.CollisionEnergy = eCM
elif 'Pythia8B' in evgenConfig.generators:
    genSeq.Pythia8B.CollisionEnergy = eCM
elif 'Herwig' in evgenConfig.generators:
    genSeq.Herwig.HerwigCommand.append("beam1energy %s" % eBeam)
    genSeq.Herwig.HerwigCommand.append("beam2energy %s" % eBeam)
elif 'Herwigpp' in evgenConfig.generators:
    genSeq.Herwigpp.Commands.append("set /Herwig/Generators/LHCGenerator:EventHandler:LuminosityFunction:Energy %s" % eCM)
#elif 'Herwig7' in evgenConfig.generators:
#    genSeq.Herwig7.Commands.append("set /Herwig/Generators/LHCGenerator:EventHandler:LuminosityFunction:Energy %s" % eCM)
elif 'Sherpa' in evgenConfig.generators:
    genSeq.Sherpa_i.Parameters.append("BEAM_ENERGY_1=%s" % eBeam)
    genSeq.Sherpa_i.Parameters.append("BEAM_ENERGY_2=%s" % eBeam)
elif 'Epos' in evgenConfig.generators:
    genSeq.Epos.BeamMomentum = -eBeam
    genSeq.Epos.TargetMomentum = eBeam
elif 'QGSJet' in evgenConfig.generators:
    genSeq.QGSJet.BeamMomentum = -eBeam
    genSeq.QGSJet.TargetMomentum = eBeam    

del eCM, eBeam
