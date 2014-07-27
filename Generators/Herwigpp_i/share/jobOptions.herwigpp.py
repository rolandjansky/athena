## Generic job options file for Herwig++

include("GeneratorUtils/StdEvgenSetup.py")

# ## Control log levels
# svcMgr.MessageSvc.OutputLevel = DEBUG

## Add H++ to the job list
from Herwigpp_i.Herwigpp_iConf import Herwigpp
topAlg += Herwigpp()

## Get basic Herwig++ Atlas tune params
from Herwigpp_i import config as hw
cmds = hw.energy_cmds(7000) + hw.base_cmds() + hw.lo_pdf_cmds("cteq6ll.LHpdf") + hw.ue_tune_cmds("UE-EE-4-CTEQ6L1")

## Add to commands
cmds += """
## Number of events for Herwig++ to print
#set /Herwig/Generators/LHCGenerator:PrintEvent 10

## Amount of info to print in debug output
set /Herwig/Generators/LHCGenerator:DebugLevel 2

## Insert MEs
insert /Herwig/MatrixElements/SimpleQCD:MatrixElements[0] /Herwig/MatrixElements/MEQCD2to2
#insert /Herwig/MatrixElements/SimpleQCD:MatrixElements[0] /Herwig/MatrixElements/MEqq2gZ2ff
#insert /Herwig/MatrixElements/SimpleQCD:MatrixElements[0] /Herwig/MatrixElements/MEqq2W2ff
#insert /Herwig/MatrixElements/SimpleQCD:MatrixElements[0] /Herwig/MatrixElements/MEGammaGamma
#insert /Herwig/MatrixElements/SimpleQCD:MatrixElements[0] /Herwig/MatrixElements/MEGammaJet
#insert /Herwig/MatrixElements/SimpleQCD:MatrixElements[0] /Herwig/MatrixElements/MEHiggs
#insert /Herwig/MatrixElements/SimpleQCD:MatrixElements[0] /Herwig/MatrixElements/MEHiggsJet
#insert /Herwig/MatrixElements/SimpleQCD:MatrixElements[0] /Herwig/MatrixElements/MEHeavyQuark

## Apply coloured parton pT cut on ME level
set /Herwig/Cuts/JetKtCut:MinKT 10*GeV
## This should be <= 2 * JetKtCut:MinKT unless you *want* a mhat cut. Default is 20 GeV.
set /Herwig/Cuts/QCDCuts:MHatMin 20*GeV
## This needs to be set to 0 if the signal process is generic QCD, otherwise -1
set /Herwig/UnderlyingEvent/MPIHandler:IdenticalToUE 0
"""

## Set commands
topAlg.Herwigpp.Commands = cmds.splitlines()

## Optionally dump equivalent standalone H++ steering card to file
# topAlg.Herwigpp.InFileDump = "myhpp.in"
