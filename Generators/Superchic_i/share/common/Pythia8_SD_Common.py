include('Superchic_i/Pythia8_Base_Common.py')


if not (genSeq.SuperChicConfig.diff == 'sda' or genSeq.SuperChicConfig.diff == 'sdb'):
    raise Exception("SD Pythia8 shower configuration can only be used with diff='sda' or 'sdb'")

unresolvedHadron = -1
if genSeq.SuperChicConfig.diff=='sda':
    unresolvedHadron = 2
elif genSeq.SuperChicConfig.diff=='sdb':
    unresolvedHadron = 1

genSeq.Pythia8.Commands += [
   "PartonLevel:MPI = off",
    "SpaceShower:dipoleRecoil = on",
    "SpaceShower:pTmaxMatch = 2",
    "BeamRemnants:primordialKT = off",
    "SpaceShower:QEDshowerByQ = off",
    "BeamRemnants:unresolvedHadron = {}".format(unresolvedHadron),
    "SpaceShower:pTdampMatch=1"
]
