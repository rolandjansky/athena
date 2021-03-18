######################################################
# CloseAntiBDecays.py
# Pythia8_i commands to switch off decays of B 
# Should be used when exclusive decays are needed from 
# the anti-B-hadron but not the B-hadron
######################################################

genSeq.Pythia8B.Commands += ['511:onMode = 2']
genSeq.Pythia8B.Commands += ['521:onMode = 2']
genSeq.Pythia8B.Commands += ['531:onMode = 2']
genSeq.Pythia8B.Commands += ['541:onMode = 2']
genSeq.Pythia8B.Commands += ['5122:onMode = 3']
genSeq.Pythia8B.Commands += ['5132:onMode = 3']
genSeq.Pythia8B.Commands += ['5232:onMode = 3']
genSeq.Pythia8B.Commands += ['5332:onMode = 3']
