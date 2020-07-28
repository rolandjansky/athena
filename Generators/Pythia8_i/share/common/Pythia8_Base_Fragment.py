## Base config for Pythia8
from Pythia8_i.Pythia8_iConf import Pythia8_i
genSeq += Pythia8_i("Pythia8")
evgenConfig.generators += ["Pythia8"]

if evgenConfig.PDGparams:
## Load basic parameters
   genSeq.Pythia8.Commands += [
       "Main:timesAllowErrors = 500",
       "ParticleDecays:limitTau0 = on",
       "ParticleDecays:tau0Max = 10.0"]

## Load parameters by including parameter dictionary 'parameters' in 'offline_dict'
   from EvgenProdTools.offline_dict import parameters

## Particle masses and widths
   particle_params = parameters.get("particles")
   if particle_params:
       for key, value in particle_params.items():
           ## Only the top quark, the leptons and the bosons are applied
           if int(key) in range(6,26):
               genSeq.Pythia8.Commands += [
                   "{:d}:m0 = {}".format(int(key), value['mass']),
                   "{:d}:mWidth = {}".format(int(key), value['width'])
               ]
   else:
       print "Could not retrieve standard ATLAS particle parameters"

## SM electroweak parameters
   ew_params = parameters.get("EW_parameters")
   if ew_params:
       ## Only the parameters sin2thetaW and sin2thetaWbar are applied
       for key,value in ew_params.items():
           if key[1] in ('sin2thetaW', 'sin2thetaWbar'):
               genSeq.Pythia8.Commands += [
                   'StandardModel:{} = {}'.format(key[1], value)
               ]
   else:
       print "Could not retrieve standard ATLAS EW parameters"
else:
## Load basic parameters
   genSeq.Pythia8.Commands += [
       "Main:timesAllowErrors = 500",
        "6:m0 = 172.5",
        "23:m0 = 91.1876",
        "23:mWidth = 2.4952",
        "24:m0 = 80.399",
        "24:mWidth = 2.085",
        "StandardModel:sin2thetaW = 0.23113",
        "StandardModel:sin2thetaWbar = 0.23146",
        "ParticleDecays:limitTau0 = on",
        "ParticleDecays:tau0Max = 10.0"]
## Control storing LHE in the HepMC record
if "StoreLHE" in genSeq.Pythia8.__slots__.keys():
   print "Pythia8_Base_Fragment.py: DISABLING storage of LHE record in HepMC by default. Please re-enable storage if desired"
   genSeq.Pythia8.StoreLHE = False
