name = runArgs.jobConfig[0]
name_info = name.split("_JZ")[1].split(".py")
slice = int(name_info[0])

minkT = {0:0,1:0,2:15,3:50,4:150,5:350,6:600,7:950,8:1500,9:2200,10:2800,11:3500,12:4200}
 
# initialize Herwig7 generator configuration for built-in matrix elements
include("Herwig7_i/Herwig7_BuiltinME.py")
 
# configure Herwig7
Herwig7Config.add_commands("set /Herwig/Partons/RemnantDecayer:AllowTop Yes")
Herwig7Config.me_pdf_commands(order="NLO", name="NNPDF30_nlo_as_0118")

command = """
insert /Herwig/MatrixElements/SubProcess:MatrixElements[0] /Herwig/MatrixElements/MEQCD2to2
set /Herwig/UnderlyingEvent/MPIHandler:IdenticalToUE 0
set /Herwig/Cuts/JetKtCut:MinKT """+str(minkT[slice])+"""*GeV

cd /Herwig/Shower
do ShowerHandler:AddVariation isr:muRfac=2.0_fsr:muRfac=2.0 2.0 2.0 All
do ShowerHandler:AddVariation isr:muRfac=2.0_fsr:muRfac=1.0 2.0 1.0 All
do ShowerHandler:AddVariation isr:muRfac=2.0_fsr:muRfac=0.5 2.0 0.5 All
do ShowerHandler:AddVariation isr:muRfac=1.0_fsr:muRfac=2.0" 1.0 2.0 All
do ShowerHandler:AddVariation isr:muRfac=1.0_fsr:muRfac=0.5" 1.0 0.5 All
do ShowerHandler:AddVariation isr:muRfac=0.5_fsr:muRfac=2.0" 0.5 2.0 All
do ShowerHandler:AddVariation isr:muRfac=0.5_fsr:muRfac=1.0 0.5 1.0 All
do ShowerHandler:AddVariation isr:muRfac=0.5_fsr:muRfac=0.5 0.5 0.5 All
do ShowerHandler:AddVariation isr:muRfac=1.75_fsr:muRfac=1.0 1.75 1.0 All
do ShowerHandler:AddVariation isr:muRfac=1.5_fsr:muRfac=1.0 1.5 1.0 All
do ShowerHandler:AddVariation isr:muRfac=1.25_fsr:muRfac=1.0 1.25 1.0 All
do ShowerHandler:AddVariation isr:muRfac=0.625_fsr:muRfac=1.0" 0.625 1.0 All
do ShowerHandler:AddVariation isr:muRfac=0.75_fsr:muRfac=1.0 0.75 1.0 All
do ShowerHandler:AddVariation isr:muRfac=0.875_fsr:muRfac=1.0 0.875 1.0 All
do ShowerHandler:AddVariation isr:muRfac=1.0_fsr:muRfac=1.75 1.0 1.75 All
do ShowerHandler:AddVariation isr:muRfac=1.0_fsr:muRfac=1.5 1.0 1.5 All
do ShowerHandler:AddVariation isr:muRfac=1.0_fsr:muRfac=1.25 1.0 1.25 All
do ShowerHandler:AddVariation isr:muRfac=1.0_fsr:muRfac=0.625 1.0 0.625 All
do ShowerHandler:AddVariation isr:muRfac=1.0_fsr:muRfac=0.75 1.0 0.75 All
do ShowerHandler:AddVariation isr:muRfac=1.0_fsr:muRfac=0.875 1.0 0.85 All
"""
print command

Herwig7Config.add_commands(command)
 
# add EvtGen
include("Herwig7_i/Herwig71_EvtGen.py")

# run Herwig7
Herwig7Config.run()

