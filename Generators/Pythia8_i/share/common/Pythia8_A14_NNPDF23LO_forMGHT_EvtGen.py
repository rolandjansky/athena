include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
genSeq.Pythia8.Commands += ["SoftQCD:inelastic = on"]

name = runArgs.jobConfig[0]
name_info = name.split("CKKW")[1].split("_for")[0].split("_JZ")
slice = int(name_info[1]) 
ktdurham = int(name_info[0])

ptcut = 1.5*ktdurham

minDict = {0:-1,1:20,2:ptcut}
maxDict = {0:20,1:ptcut,2:ptcut+100}

if slice == 2:
    genSeq.Pythia8.Commands += ["PhaseSpace:pTHatMin = 15."]

# Leading jet pT filter
include("GeneratorFilters/JetFilterAkt4.py")
filtSeq.QCDTruthJetFilter.MinPt = minDict[slice]*GeV
filtSeq.QCDTruthJetFilter.MaxPt = maxDict[slice]*GeV
