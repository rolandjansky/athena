# JobOption fragment for Herwig >=7.0.4

# Common job option for gamma gamma processes in Herwig7
# Contact: Oldrich Kepka

assert hasattr(genSeq, "Herwig7")

Herwig7Config.add_commands(Herwig7Config.pdf_gammagamma_cmds())


cmds = """
set /Herwig/Partons/QCDExtractor:FirstPDF /Herwig/Partons/BudnevPDF
set /Herwig/Partons/QCDExtractor:SecondPDF /Herwig/Partons/BudnevPDF
set /Herwig/Partons/BudnevPDF:Q2Max 4.

create ThePEG::O1AlphaS /Herwig/AlphaQCD_O1 O1AlphaS.so
set /Herwig/Generators/LHCGenerator:StandardModelParameters:QCD/RunningAlphaS /Herwig/AlphaQCD_O1

# Cuts
cd /Herwig/Cuts
set QCDCuts:ScaleMin 0.0*MeV
set QCDCuts:X1Min 0
set QCDCuts:X2Min 0
set QCDCuts:X1Max 1.
set QCDCuts:X2Max 1.
erase QCDCuts:MultiCuts 0
"""
Herwig7Config.add_commands(cmds)
del cmds

# To avoid warning from displaced vertices, bugfix needed in herwig++
testSeq.TestHepMC.MaxTransVtxDisp = 1000000
testSeq.TestHepMC.MaxVtxDisp      = 1000000000
