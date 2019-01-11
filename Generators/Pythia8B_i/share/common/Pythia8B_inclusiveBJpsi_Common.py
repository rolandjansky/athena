##############################################################
# Pythia8B_inclusiveBJpsi_Common.py
#
# Common job options for inclusive b->J/psi production using
# Pythia8B.
##############################################################

# Hard process
genSeq.Pythia8B.Commands += ['HardQCD:all = on'] # Equivalent of MSEL1
genSeq.Pythia8B.Commands += ['ParticleDecays:mixB = off']
genSeq.Pythia8B.Commands += ['HadronLevel:all = off']

# Event selection
genSeq.Pythia8B.SelectBQuarks = True
genSeq.Pythia8B.SelectCQuarks = False
genSeq.Pythia8B.VetoDoubleBEvents = True
genSeq.Pythia8B.UserSelection = 'BJPSIINCLUSIVE'

# Close B decays and open antiB decays
include ("Pythia8B_i/Pythia8B_CloseBDecays.py")

# Open inclusive B->J/psi decays
include ("Pythia8B_i/Pythia8B_OpenBJpsiDecays.py")

# List of B-species
include("Pythia8B_i/Pythia8B_BPDGCodes.py")
