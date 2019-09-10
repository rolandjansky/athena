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
insert /Herwig/MatrixElements/SimpleQCD:MatrixElements[0] /Herwig/MatrixElements/MEQCD2to2
set /Herwig/UnderlyingEvent/MPIHandler:IdenticalToUE 0
set /Herwig/Cuts/JetKtCut:MinKT """+str(minkT[slice])+"""*GeV
"""
print command

Herwig7Config.add_commands(command)
 
# add EvtGen
include("Herwig7_i/Herwig7_EvtGen.py")

# run Herwig7
Herwig7Config.run()

