#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


## Propagate output level requirements to the generators interfaces
if 'Pythia8' in evgenConfig.generators:
    genSeq.Pythia8.OutputLevel = DEBUG
elif 'Pythia8B' in evgenConfig.generators:
    genSeq.Pythia8B.OutputLevel = DEBUG  
elif 'Herwig7' in evgenConfig.generators:
    genSeq.Herwig7.OutputLevel = DEBUG 
elif 'Sherpa' in evgenConfig.generators:
    genSeq.Sherpa_i.OutputLevel = DEBUG 
elif 'Epos' in evgenConfig.generators:
    genSeq.Epos.OutputLevel = DEBUG
elif 'QGSJet' in evgenConfig.generators:  
    genSeq.QGSJet.OutputLevel = DEBUG
elif 'SuperChic' in evgenConfig.generators:
    genSeq.Superchic.OutputLevel = DEBUG 
elif 'ParticleGun' in evgenConfig.generators:                            
    genSeq.ParticleGun.OutputLevel = DEBUG  
elif 'ParticleDecayer' in evgenConfig.generators:
    genSeq.ParticleDecayer.OutputLevel = DEBUG
elif 'Starlight' in evgenConfig.generators:                            
    genSeq.Starlight.OutputLevel = DEBUG 
elif 'Hydjet' in evgenConfig.generators:
    genSeq.Hydjet.OutputLevel = DEBUG
elif 'Hijing' in evgenConfig.generators:
    genSeq.Hijing.OutputLevel = DEBUG

if 'EvtGen' in evgenConfig.generators:
    genSeq.EvtInclusiveDecay.OutputLevel = DEBUG     
if 'Tauolapp' in evgenConfig.generators:
    genSeq.TauolaPP.OutputLevel = DEBUG   
if 'Photospp' in evgenConfig.generators:
    genSeq.Photospp.OutputLevel = DEBUG
if 'Powheg' in evgenConfig.generators and not hasattr(runArgs,'inputGeneratorFile'):
    genSeq.PowhegControl.OutputLevel = DEBUG
if 'MadGraph' in evgenConfig.generators and not hasattr(runArgs,'inputGeneratorFile'):
    genSeq.MadGraphControl.OutputLevel = DEBUG
