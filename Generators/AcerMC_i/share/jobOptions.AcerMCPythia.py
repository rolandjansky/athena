###############################################################
#
# Job options file
#
#==============================================================
#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )
include( "PartPropSvc/PartPropSvc.py" )

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
theApp.Dlls  += [ "TruthExamples", "Pythia_i" ]
theApp.TopAlg = ["Pythia","PrintMC"]
theApp.ExtSvc += ["AtRndmGenSvc"]
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc.OutputLevel               = 3
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 5
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
AtRndmGenSvc = Service( "AtRndmGenSvc" )
AtRndmGenSvc.Seeds = ["PYTHIA 4789899 989240512", "PYTHIA_INIT 820021 2347532"]
# AtRndmGenSvc.ReadFromFile = true;
#Pythia.PythiaCommand = {"pyinit user acermc","acermc acset12 0","pyinit pylistf 1","pyinit dumpr 1 1"};
# with MSTP(128)=0
Pythia = Algorithm( "Pythia" )
Pythia.PythiaCommand = ["pyinit user acermc","acermc acset12 0","pyinit pylistf 1","pyinit dumpr 1 1","pypars mstp 128 0"]

#-- PrintMC
PrintMC = Algorithm( "PrintMC" )
#event SVC key
PrintMC.McEventKey = "GEN_EVENT"
# do you want output at all? TRUE/FALSE
PrintMC.VerboseOutput = TRUE
# Event print style Vertex(traditional)/Barcode(barcode ordered)
PrintMC.PrintStyle = "Barcode"
# First and last event to print, if no last events => job end are printed
PrintMC.FirstEvent = 1
PrintMC.LastEvent = 5

#---------------------------------------------------------------
# Ntuple service output
#---------------------------------------------------------------
#==============================================================
#
# End of job options file
#
###############################################################
