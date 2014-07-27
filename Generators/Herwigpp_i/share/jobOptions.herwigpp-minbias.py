## Min bias job options file for Herwig++

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
## Set up min bias matrix element
insert /Herwig/MatrixElements/SimpleQCD:MatrixElements[0] /Herwig/MatrixElements/MEMinBias

## Remove all hard process cuts
set /Herwig/Cuts/JetKtCut:MinKT 0.0*GeV
set /Herwig/Cuts/QCDCuts:MHatMin 0.0*GeV

## x_min values for the "LO**_UE7000" tune
set /Herwig/Cuts/QCDCuts:X1Min 0.055
set /Herwig/Cuts/QCDCuts:X2Min 0.055

## Turn on the soft MPI modelling, and treat the "hard" process as equivalent to the secondary scatters
set /Herwig/UnderlyingEvent/MPIHandler:IdenticalToUE 0
set /Herwig/UnderlyingEvent/MPIHandler:softInt Yes
set /Herwig/UnderlyingEvent/MPIHandler:twoComp Yes
"""

## Set commands
topAlg.Herwigpp.Commands = cmds.splitlines()
