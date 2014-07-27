## Job options file for Herwig++, LEP1 Z -> mu+ mu- production

include("GeneratorUtils/StdEvgenSetup.py")

# ## Control log levels
# svcMgr.MessageSvc.OutputLevel = DEBUG

## Add H++ to the job list
from Herwigpp_i.Herwigpp_iConf import Herwigpp
topAlg += Herwigpp()

## Get basic Herwig++ Atlas tune params
from Herwigpp_i import config as hw
cmds = hw.energy_cmds(91.2) + hw.base_cmds()

## Add to commands
cmds += """
## Set up LEP1 e+ e- initial state
set /Herwig/Generators/LHCGenerator:EventHandler:CascadeHandler:MPIHandler NULL
set /Herwig/Generators/LHCGenerator:EventHandler:BeamA /Herwig/Particles/e+
set /Herwig/Generators/LHCGenerator:EventHandler:BeamB /Herwig/Particles/e-

## e+ e- -> Z/gamma* -> mu+ mu- process
insert /Herwig/MatrixElements/SimpleQCD:MatrixElements[0] /Herwig/MatrixElements/MEee2gZ2ll
set /Herwig/MatrixElements/SimpleQCD:MatrixElements[0]:Allowed Muon

## Tight windowing around Z mass (for MC-TESTER comparison to PHOTOS)
set /Herwig/Cuts/MassCut:MinM 90.2*GeV
set /Herwig/Cuts/MassCut:MaxM 92.2*GeV
"""

## Set commands
topAlg.Herwigpp.Commands = cmds.splitlines()


###########################


## Add MCTester
from MCTester_i.MCTester_iConf import MCTesterAlg
topAlg += MCTesterAlg()

topAlg.MCTesterAlg.McEventKey = "GEN_EVENT" # "TruthEvent" for RDOs and ESDs
topAlg.MCTesterAlg.result_path = "hpp-mctester-Zmumu.root" # output filename
topAlg.MCTesterAlg.decay_particle = 23 # particle pdgID to be examined
topAlg.MCTesterAlg.nbins = 100 # number of histogram bins
topAlg.MCTesterAlg.bin_min = 0.0 # bin minimum (MeV)
topAlg.MCTesterAlg.bin_max = 120000.0 # bin maximum (MeV)
topAlg.MCTesterAlg.collision_check_sum_margin = 5 # margin of allowance in 4 momentum check (MeV)

## Three lines of description to go in the booklet
MCTesterAlg.generation_description_line1 = r"H++ (Athena)"
MCTesterAlg.generation_description_line2 = r"LEP1 $Z \to \mu^+ \mu^-$, $m_Z \pm 1$~GeV."
MCTesterAlg.generation_description_line3 = r"Max. 2 photons. $x$-axis normalised to $m_Z$. Filter of $m_Z \pm 1$~GeV"

## Include particle -> particle + gamma decays?
MCTesterAlg.include_decays_to_self = 'false'
MCTesterAlg.reco_missing_vertex = 0;
