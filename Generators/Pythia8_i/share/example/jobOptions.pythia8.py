include("GeneratorUtils/StdEvgenSetup.py")

svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.AtRndmGenSvc.Seeds = ["PYTHIA8 4789899 989240512",
                             "PYTHIA8_INIT 820021 2347532"]

from Pythia8_i.Pythia8_iConf import Pythia8_i
topAlg += Pythia8_i()
topAlg.Pythia8_i.CollisionEnergy = 7000
topAlg.Pythia8_i.Commands += ['HardQCD:all = on']

include("GeneratorUtils/postJO.DumpMC.py")
