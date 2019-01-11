## Configure Pythia8 to read input events from an LHEF file
hasInput = hasattr(runArgs,"inputGeneratorFile")
if hasInput:
   include ('EvgenProdTools/mult_lhe_input.py')
assert hasattr(genSeq, "Pythia8")
genSeq.Pythia8.LHEFile = "events.lhe"
genSeq.Pythia8.CollisionEnergy = int(runArgs.ecmEnergy)
