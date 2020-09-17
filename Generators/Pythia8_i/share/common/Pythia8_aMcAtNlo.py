## Enable MG5_aMC@NLO LHEF reading in Pythia8
include("Pythia8_i/Pythia8_LHEF.py")
evgenConfig.generators += ["aMcAtNlo"]

#aMC@NLO default Pythia8 settings from http://amcatnlo.web.cern.ch/amcatnlo/list_detailed2.htm#showersettings
genSeq.Pythia8.Commands += ["SpaceShower:pTmaxMatch = 1",
                            "SpaceShower:pTmaxFudge = 1",
                            "SpaceShower:MEcorrections = off",
                            "TimeShower:pTmaxMatch = 1",
                            "TimeShower:pTmaxFudge = 1",
                            "TimeShower:MEcorrections = off",
                            "TimeShower:globalRecoil = on",
                            "TimeShower:limitPTmaxGlobal = on",
                            "TimeShower:nMaxGlobalRecoil = 1",
                            "TimeShower:globalRecoilMode = 2",
                            "TimeShower:nMaxGlobalBranch = 1.",
                            "TimeShower:weightGluonToQuark=1.",
                            "Check:epTolErr = 1e-2" ]
