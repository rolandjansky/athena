## Job options file for Herwig++, NLO W -> e nu_e production

include("GeneratorUtils/StdEvgenSetup.py")

# ## Control log levels
# svcMgr.MessageSvc.OutputLevel = DEBUG

## Add H++ to the job list
from Herwigpp_i.Herwigpp_iConf import Herwigpp
topAlg += Herwigpp()

## Get basic Herwig++ Atlas tune params
from Herwigpp_i import config as hw
cmds = hw.energy_cmds(7000) + hw.base_cmds() + hw.nlo_pdf_cmds("CT10.LHgrid", "cteq6ll.LHpdf") + hw.ue_tune_cmds("CTEQ6L1-UE-EE-7000-3")

## Add to commands
cmds += """
## Set up Powheg truncated shower
set /Herwig/Shower/Evolver:HardEmissionMode POWHEG
## Use 2-loop alpha_s (now done by default)
#create Herwig::O2AlphaS /Herwig/AlphaQCD_O2
#set /Herwig/Generators/LHCGenerator:StandardModelParameters:QCD/RunningAlphaS /Herwig/AlphaQCD_O2

## Set up qq/gg -> Higgs process at NLO
insert /Herwig/MatrixElements/SimpleQCD:MatrixElements[0] /Herwig/MatrixElements/PowhegMEHiggs
"""

## Set commands
topAlg.Herwigpp.Commands = cmds.splitlines()
