include('Superchic_i/Pythia8_Base_Common.py')


if genSeq.SuperChicConfig.diff != 'el':
    raise Exception("EL Pythia8 showr configuration can only be used with diff='el'")

genSeq.Pythia8.Commands += [
    "PartonLevel:MPI = off",
    "SpaceShower:pTmaxMatch = 2",
    "BeamRemnants:primordialKT = off",
    "BeamRemnants:unresolvedHadron = 3",
    "SpaceShower:pTdampMatch=1",
    "PartonLevel:ISR = off",
    "LesHouches:matchInOut = off"
]
