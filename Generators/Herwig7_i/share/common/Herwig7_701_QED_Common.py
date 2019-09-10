## JobOption fragment for Herwig 7.0.1

# Common job option for gamma gamma processes in Herwig7
# MPI tune is not relevant as the pdf_gammagamma_cmds() function disables MPI
# Contact: Oldrich Kepka
include("MC15JobOptions/Herwig7_701_Base_Fragment.py")
from Herwig7_i import config as hw
cmds = hw.energy_cmds(runArgs.ecmEnergy) + hw.base_cmds() + hw.pdf_gammagamma_cmds()
cmds += """
set /Herwig/Partons/QCDExtractor:FirstPDF /Herwig/Partons/BudnevPDF
set /Herwig/Partons/QCDExtractor:SecondPDF /Herwig/Partons/BudnevPDF
"""
cmds += "create ThePEG::O1AlphaS /Herwig/AlphaQCD_O1 O1AlphaS.so\n"
cmds += "set /Herwig/Generators/LHCGenerator:StandardModelParameters:QCD/RunningAlphaS /Herwig/AlphaQCD_O1\n"
cmds += "set /Herwig/Partons/BudnevPDF:Q2Max 4."
genSeq.Herwig7.Commands += cmds.splitlines()
del cmds
