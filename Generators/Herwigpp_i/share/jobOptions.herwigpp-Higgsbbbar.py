## Job options file for Herwig++, NLO W -> e nu_e production

include("GeneratorUtils/StdEvgenSetup.py")

# ## Control log levels
# svcMgr.MessageSvc.OutputLevel = DEBUG

## Add H++ to the job list
from Herwigpp_i.Herwigpp_iConf import Herwigpp
topAlg += Herwigpp()

## Get basic Herwig++ Atlas tune params
from Herwigpp_i import config as hw
cmds = hw.energy_cmds(7000) + hw.base_cmds() + hw.lo_pdf_cmds("cteq6ll.LHpdf") + hw.ue_tune_cmds("CTEQ6L1-UE-EE-7000-3")

## Add to commands
cmds += """
## Insert the heavy quark + Higgs matrix element, pre-configured for bbbar
insert /Herwig/MatrixElements/SimpleQCD:MatrixElements[0] /Herwig/MatrixElements/MEPP2bbbarH
"""

## Set commands
topAlg.Herwigpp.Commands = cmds.splitlines()
