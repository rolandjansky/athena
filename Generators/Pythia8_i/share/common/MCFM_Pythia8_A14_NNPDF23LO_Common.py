######################################################################
#
# MCFM 8.0 / Pythia8
# Joboption only for showering the LHE files..
#

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.generators += [ 'MCFM', 'Pythia8' ]
evgenConfig.description = 'MCFM, CT10nnlo PDF , Pythia8'
evgenConfig.keywords = ['diboson', '2lepton', 'electroweak', 'Higgs', 'ZZ']
evgenConfig.contact = ['denys.denysiuk@cern.ch', 'xiangyang.ju@cern.ch']
#evgenConfig.inputfilecheck = 'ggH300_5SMW_ZZ_4l'

include('Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py')
include("Pythia8_i/Pythia8_LHEF.py")

# boson decays already done in the lhe file
genSeq.Pythia8.Commands += [ '25:onMode = off' ]
genSeq.Pythia8.Commands += [ '24:onMode = off' ]
genSeq.Pythia8.Commands += [ '23:onMode = off' ]

# no power shower, just wimpy showers
genSeq.Pythia8.Commands += [ 'SpaceShower:pTmaxMatch = 1' ]
