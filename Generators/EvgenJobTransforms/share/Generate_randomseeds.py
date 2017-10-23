
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## Set random seeds depending on used generators

genseeds = {
    'Pythia'            : ["PYTHIA OFFSET {rnd} 47898993 {rnd}", "PYTHIA_INIT 820021 2347532"],
    'PythiaB'           : ["PYTHIA OFFSET {rnd} 47898993 {rnd}", "PYTHIA_INIT 820021 2347532"],
    'PythiaRhad'        : ["PYTHIA OFFSET {rnd} 47898993 {rnd}", "PYTHIA_INIT 820021 2347532"],
    'PythiaChL'         : ["PYTHIA OFFSET {rnd} 47898993 {rnd}", "PYTHIA_INIT 820021 2347532"],
    'PythiaGS'          : ["PYTHIA OFFSET {rnd} 47898993 {rnd}", "PYTHIA_INIT 820021 2347532"],
    'PythiaResMod'      : ["PYTHIA OFFSET {rnd} 47898993 {rnd}", "PYTHIA_INIT 820021 2347532"],
    'PythiaTopMdiff'    : ["PYTHIA OFFSET {rnd} 47898993 {rnd}", "PYTHIA_INIT 820021 2347532"],
    'Pythia8'           : ["PYTHIA8 OFFSET {rnd} 4789899 {rnd}", "PYTHIA8_INIT 820021 2347532"],
    'Pythia8B'          : ["PYTHIA8 OFFSET {rnd} 4789899 {rnd}", "PYTHIA8_INIT 820021 2347532"],
    'Herwig'            : ["HERWIG OFFSET {rnd} 30450275 {rnd}", "HERWIG_INIT 620021 5347532"],
    'Herwigpp'          : ["Herwigpp OFFSET {rnd} {rnd} 31122001"],
    'Herwig7'          : ["Herwig7 OFFSET {rnd} {rnd} 31122001"],
    'Jimmy'             : ["JIMMY OFFSET {rnd}  39002061 {rnd}", "JIMMY_INIT  720021 4347532"],
    'Cascade'           : ["CASCADE OFFSET {rnd} 4789899 {rnd}", "CASCADE_INIT 889223465 78782321"],
    'Tauola'            : ["TAUOLA OFFSET {rnd} 10480275 {rnd}", "TAUOLA_INIT 920021 3347532"],
    'Tauolapp'          : ["TAUOLAPP OFFSET {rnd} 10480275 {rnd}", "TAUOLAPP_INIT 920021 3347532"],
    'TauolaPP'          : ["TAUOLAPP OFFSET {rnd} 10480275 {rnd}", "TAUOLAPP_INIT 920021 3347532"],
    'Photos'            : ["PHOTOS OFFSET {rnd} 20450275 {rnd}", "PHOTOS_INIT 930021 3447532"],
    'Photospp'          : ["PHOTOSPP OFFSET {rnd} 20450275 {rnd}", "PHOTOSPP_INIT 930021 3447532"],
    'EvtGen'            : ["EVTGEN OFFSET {rnd} 30450275 {rnd}"],
    'Hijing'            : ["HIJING OFFSET {rnd} 327213897 {rnd}", "HIJING_INIT 31452781 78713307"],
    'Hydjet'            : ["HYDJET OFFSET {rnd} 252413389 {rnd}", "HYDJET_INIT 52413389 24133894"],
    'Pyquen'            : ["PYTHIA OFFSET {rnd} 47898993 {rnd}", "PYTHIA_INIT 820021 2347532"],
    'Isajet'            : ["ISAJET OFFSET {rnd} 1857585203 {rnd}", "ISAJET_INIT 820021 2347532"],
    'Phojet'            : ["PHOJET OFFSET {rnd} 174947321 {rnd}", "PHOJET_INIT 820021 2347532"],
    'Epos'              : ["EPOS OFFSET {rnd} {rnd} 31122001", "EPOS_INIT OFFSET {rnd} 820021 2347532"],
    'QGSJet'            : ["QGSJet OFFSET {rnd} {rnd} 51122005", "QGSJET_INIT OFFSET {rnd} 820021 2347532"],
    'ParticleGun'       : ["ParticleGun {rnd} {rnd}"],
    'ParticleGenerator' : ["SINGLE LUXURY 2 OFFSET 12345 {rnd} {rnd}"],
    'ParticleDecayer'   : ["ParticleDecayer LUXURY 2 OFFSET 54321 {rnd} {rnd}"],
    'CosmicGenerator'   : ["SINGLE OFFSET {rnd} 20401607 {rnd}"],
    'Sherpa'            : ["SHERPA OFFSET {rnd} 174947321 {rnd}", "SHERPA_INIT 820021 2347532"],
    'Exhume'            : ["ExhumeRand OFFSET {rnd} 4475757 {rnd}"],
    'Pomwig'            : ["POMWIG OFFSET {rnd} 37489241 {rnd}", "POMWIG_INIT 21219421 1984121"],
    'Starlight'         : ["STARLIGHT OFFSET {rnd} {rnd} 31122001"],
    'BeamHaloGenerator' : ["BeamHalo OFFSET {rnd} 3524752 {rnd}"]
}

## Decide whether to use the RanLux or standard random number service
# TODO: Is this necessary or can we just provide the seeds to both services and the alg chooses which to use?
ranluxlist = ["ParticleGenerator"]
assert evgenConfig.generators
if any(gen in ranluxlist for gen in evgenConfig.generators):
    evgenLog.info("Using RanLux random numbers!")
    atRndmGenSvc = svcMgr.AtRanluxGenSvc
else:
    atRndmGenSvc = svcMgr.AtRndmGenSvc

## Pass the random seed from the transform command line into each used generator's seed config string
seedstrs = []
for gen in evgenConfig.generators:
    if genseeds.has_key(gen):
        for seedtemplate in genseeds[gen]:
            seed = runArgs.randomSeed
            if runArgs.trfSubstepName == 'afterburn':
              seed = seed + 140280
            
            seedstr = seedtemplate.format(rnd=seed)
            evgenLog.info("Adding %s random seed config: %s" % (gen, seedstr))
            seedstrs.append(seedstr)
svcMgr.AtRanluxGenSvc.Seeds = seedstrs
svcMgr.AtRndmGenSvc.Seeds = seedstrs
