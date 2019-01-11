# Zprime resonance mass (in GeV)

splitConfig = runArgs.jobConfig[0].rstrip('.py').split('_')
ZprimeMass = splitConfig[4]

include('Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py')

genSeq.Pythia8.Commands += [
			    "NewGaugeBoson:ffbar2gmZZprime = on",  # create Z' bosons
                            "Zprime:gmZmode = 3",                  # Z',Z,g with interference: switch off gamma-Z-Z' interference
                            "32:m0 = "+str(ZprimeMass)]


# EVGEN configuration
evgenConfig.description = 'Pythia8 SSM Zprime: all decays'
evgenConfig.contact = ["Simone Amoroso <simone.amoroso@cern.ch>, Adam Jinaru <adam.jinaru@cern.ch>"]
evgenConfig.keywords    = [ 'BSM', 'Zprime', 'heavyBoson', 'SSM', 'resonance', 'electroweak' ]
evgenConfig.generators += [ 'Pythia8' ]
evgenConfig.process = "pp>Zprime>inclusive"


