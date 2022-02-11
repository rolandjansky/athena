## Config for Py8 tune A3 with NNPDF23LO
## This is the version without EvtGen, and as such is not the standard.  
## The default version is available in common/Pythia8/Pythia8_A3_NNPDF23LO_EvtGen_Common.py

include("Pythia8_i/Pythia8_Base_Fragment.py")

genSeq.Pythia8.Commands += [
    "Tune:ee = 7",
    "Tune:pp = 14",
    "PDF:pSet=LHAPDF6:NNPDF23_lo_as_0130_qed",
    "MultipartonInteractions:bProfile = 2",
    "MultipartonInteractions:pT0Ref = 2.45",
    "MultipartonInteractions:ecmPow = 0.21",
    "MultipartonInteractions:coreRadius = 0.55",
    "MultipartonInteractions:coreFraction = 0.9",
    "SigmaDiffractive:PomFlux = 4",
    "SigmaDiffractive:PomFluxEpsilon = 0.07",
    "SigmaDiffractive:PomFluxAlphaPrime = 0.25",
    "SigmaElastic:rho = 0.0",
    "SigmaDiffractive:mode = 0",
    "SigmaTotal:sigmaAXB =0",
    "SigmaDiffractive:OwnbMinDD = 5",
    "ColourReconnection:range = 1.8"]
#    "BeamRemnants:reconnectRange  = 1.8"]

rel = os.popen("echo $AtlasVersion").read()
print("Atlas release " + rel)

evgenConfig.tune = "A3 NNPDF23LO"
print("WARNING! These parameters are derived tuning Pythia 8.186 to data")



