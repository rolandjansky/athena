include('Superchic_i/Pythia8_Base_Common.py')


if genSeq.SuperChicConfig.diff != 'dd':
    raise Exception("DD Pythia8 shower configuration can only be used with diff='dd'")

genSeq.Pythia8.Commands += [
    "PartonLevel:MPI = off",
    "SpaceShower:dipoleRecoil = on",
    "SpaceShower:pTmaxMatch = 2",
    "SpaceShower:QEDshowerByQ = on",   
    "BeamRemnants:primordialKT = off",
    "BeamRemnants:unresolvedHadron = 0",
    "SpaceShower:pTdampMatch=1"
]
