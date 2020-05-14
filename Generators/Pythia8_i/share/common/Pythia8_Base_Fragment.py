## Base config for Pythia8
from Pythia8_i.Pythia8_iConf import Pythia8_i
genSeq += Pythia8_i("Pythia8")
evgenConfig.generators += ["Pythia8"]

## Load basic parameters
genSeq.Pythia8.Commands += [
    "Main:timesAllowErrors = 500",
    "ParticleDecays:limitTau0 = on",
    "ParticleDecays:tau0Max = 10.0"]

## Load parameters by including parameter dictionary 'parameters' in 'offline_dict'
from EvgenProdTools.offline_dict import parameters
for k,v in parameters.items():
    if k == 'particles':
        for key,value in v.items():
## Only the top quark, the leptons and the bosons are included
            if (int(key) in range(6,26)):
                genSeq.Pythia8.Commands += [
                    str(key)+':m0 = '+str(value['mass']),
                    str(key)+':mWidth = '+str(value['width']),
                ]
## Only the parameters sin2thetaW and sin2thetaWbar are included
    if k == 'EW_parameters':
        for key,value in v.items():
            if key[1] == 'sin2thetaW' or key[1] == 'sin2thetaWbar':
                genSeq.Pythia8.Commands += [
                    'StandardModel:'+str(key[1])+' = '+str(value),
                ]

if "StoreLHE" in genSeq.Pythia8.__slots__.keys():
   print "Pythia8_Base_Fragment.py: DISABLING storage of LHE record in HepMC by default. Please reenable storeage if desired"
   genSeq.Pythia8.StoreLHE = False
