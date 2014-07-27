## Generic job options file for Herwig++

include("GeneratorUtils/StdEvgenSetup.py")

# ## Control log levels
# svcMgr.MessageSvc.OutputLevel = DEBUG

## Add H++ to the job list
from Herwigpp_i.Herwigpp_iConf import Herwigpp
topAlg += Herwigpp()

## Get basic Herwig++ Atlas tune params
from Herwigpp_i import config as hw
cmds = hw.energy_cmds(7000) + hw.base_cmds() + hw.lo_pdf_cmds("cteq6ll.LHpdf") + hw.ue_tune_cmds("CTEQ6L1-UE-EE-7000-3")

## Set jet min pT cut: override on Athena command line with e.g. "-c JETPTCUT=30"
JET_PT_CUT = 10
if 'JETPTCUT' in dir():
     JET_PT_CUT = JETPTCUT

## Add QCD matrix element and chosen pT cut to commands
cmds += """
insert /Herwig/MatrixElements/SimpleQCD:MatrixElements[0] /Herwig/MatrixElements/MEQCD2to2
set /Herwig/Cuts/JetKtCut:MinKT %f*GeV
set /Herwig/Cuts/QCDCuts:MHatMin %f*GeV
set /Herwig/UnderlyingEvent/MPIHandler:IdenticalToUE 0
""" % (JET_PT_CUT, 2*JET_PT_CUT)

# ## Use weighted events with an enhanced pT spectrum
# cmds += """
# ## Use preweighting rather than jet slices to cover wide pT range
# create ThePEG::ReweightMinPT /Herwig/Generators/preweight ReweightMinPT.so
# set /Herwig/Generators/preweight:Power 6
# insert /Herwig/Generators/LHCGenerator:EventHandler:SubProcessHandlers[0]:Preweights[0] /Herwig/Generators/preweight
# set /Herwig/Generators/LHCGenerator:EventHandler:Weighted On
# """

## Set commands
topAlg.Herwigpp.Commands = cmds.splitlines()
