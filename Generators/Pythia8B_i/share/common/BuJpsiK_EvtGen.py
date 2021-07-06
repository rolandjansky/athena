##############################################################
# Python snippet to generate EvtGen user decay file on the fly
# B+/- -> J/psi(mumu) K+/- 
##############################################################
f = open("BU_JPSI_K_USER.DEC","w")
f.write("Decay B0\n")
f.write("Enddecay\n")
f.write("Decay anti-B0\n")
f.write("Enddecay\n")
f.write("Decay B+\n")
f.write("Enddecay\n")
f.write("Decay B-\n")
f.write("Enddecay\n")
f.write("Decay B_s0\n")
f.write("Enddecay\n")
f.write("Decay anti-B_s0\n")
f.write("Enddecay\n")
f.write("Decay anti-Lambda_b0\n")
f.write("Enddecay\n")
f.write("Decay Lambda_b0\n")
f.write("Enddecay\n")
f.write("Decay anti-Xi_b+\n")
f.write("Enddecay\n")
f.write("Decay Xi_b-\n")
f.write("Enddecay\n")
f.write("Decay anti-Xi_b0\n")
f.write("Enddecay\n")
f.write("Decay Xi_b0\n")
f.write("Enddecay\n")
f.write("Decay anti-Sigma_b+\n")
f.write("Enddecay\n")
f.write("Decay Sigma_b+\n")
f.write("Enddecay\n")
f.write("Decay anti-Sigma_b-\n")
f.write("Enddecay\n")
f.write("Decay Sigma_b-\n")
f.write("Enddecay\n")
f.write("Decay anti-Sigma_b0\n")
f.write("Enddecay\n")
f.write("Decay B_c+\n")
f.write("Enddecay\n")
f.write("Decay B_c-\n")
f.write("Enddecay\n")
f.write("Decay B+\n")
f.write("1.0000  J/psi   K+             SVS;\n")
f.write("Enddecay\n")
f.write("Decay B-\n")
f.write("1.0000  J/psi   K-             SVS;\n")
f.write("Enddecay\n")
f.write("Decay J/psi\n")
f.write("1.0000    mu+  mu-             VLL;\n")
f.write("Enddecay\n")
f.write("End\n")
f.close()
##############################################################

evgenConfig.description = "Exclusive Bplus->Jpsi_mu4mu4_Kplus production"
evgenConfig.keywords = ["exclusiveBtoJpsi","Bu","Jpsi","dimuons"]
evgenConfig.minevents = 10

include("MC12JobOptions/Pythia8B_AU2_CTEQ6L1_Common.py")
include("MC12JobOptions/Pythia8B_EvtGenAfterburner.py") 
include("MC12JobOptions/BSignalFilter.py")

topAlg.Pythia8B.Commands += ['HardQCD:all = on'] 
topAlg.Pythia8B.Commands += ['PhaseSpace:pTHatMin = 7.']
topAlg.Pythia8B.Commands += ['ParticleDecays:mixB = off']
topAlg.Pythia8B.Commands += ['HadronLevel:all = off']

# Quark cuts
topAlg.Pythia8B.SelectBQuarks = True
topAlg.Pythia8B.SelectCQuarks = False
topAlg.Pythia8B.QuarkPtCut = 4.0
topAlg.Pythia8B.AntiQuarkPtCut = 4.0
topAlg.Pythia8B.QuarkEtaCut = 4.5
topAlg.Pythia8B.AntiQuarkEtaCut = 4.5
topAlg.Pythia8B.RequireBothQuarksPassCuts = False

topAlg.Pythia8B.VetoDoubleBEvents = True
topAlg.Pythia8B.NHadronizationLoops = 1

# List of B-species
include("Pythia8B_i/BPDGCodes.py")

# Final state selections
topAlg.Pythia8B.TriggerPDGCode = 0
topAlg.Pythia8B.SignalPDGCodes = [521]
topAlg.EvtInclusiveDecay.userDecayFile = "BU_JPSI_K_USER.DEC"
topAlg.BSignalFilter.LVL1MuonCutOn = True
topAlg.BSignalFilter.LVL2MuonCutOn = True
topAlg.BSignalFilter.LVL1MuonCutPT = 0.0 
topAlg.BSignalFilter.LVL1MuonCutEta = 2.7
topAlg.BSignalFilter.LVL2MuonCutPT = 0.0
topAlg.BSignalFilter.LVL2MuonCutEta = 2.7
