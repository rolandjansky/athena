# JobOption fragment for Herwig >=7.0.4

# Common job option for gamma gamma -> ll processes in Herwig7
# Generation in Min < M < Max window. Min/Max and lepton flavour determined
# from the JO name, e.g. MC15.363749.Herwig7EvtGen_BudnevQED_ggTOee_20M45_LeptonFilter.py
# Contact: Oldrich Kepka

# Finnal states to be found in the JO name
h7finalStates = {"ggTOee": "Electron", "ggTOmumu": "Muon", "ggTOtautau": "Tau" }

# Assert right JO name format
info = runArgs.jobConfig[0].split("_")
assert (info[1] == "BudnevQED")  , "JobOption name does not contain BudnevQED"
assert (info[2] in h7finalStates), "Unknown final state key %s" % info[2]

# Extract final state and mass range from the JO name
fs     = h7finalStates[info[2]]
mrange = info[3].split('M')
assert (len(mrange) >0), "Cannot extract mass range from the JO"
del h7finalStates


# Initialize Herwig7 generator configuration for built-in matrix elements
include("Herwig7_i/Herwig7_BuiltinME.py")

# Generic QED include with maximum phase-space 
include("Herwig7_i/Herwig7_QED_EvtGen_Common.py")

# Mmin<M<Mmax
command = """\
set QCDCuts:MHatMin """+str(mrange[0])+"""*GeV\n"""
if len(mrange) > 1 and mrange[1]!='':
   command += "set QCDCuts:MHatMax "+str(mrange[1])+"*GeV\n"

# Hard process
command += """\
set LeptonKtCut:MinKT 2*GeV    # save minimal setting, override in upstream jo if pT-filter used
cd /Herwig/MatrixElements
insert SimpleQCD:MatrixElements 0 /Herwig/MatrixElements/MEgg2ff
set /Herwig/MatrixElements/MEgg2ff:Process """+str(fs)+"""
"""


Herwig7Config.add_commands(command)
del command
del mrange
del fs
