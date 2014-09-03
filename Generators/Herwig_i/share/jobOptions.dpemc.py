###############################################################
#
# Job options file
#
#==============================================================
#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py")
include( "PartPropSvc/PartPropSvc.py" )
#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
theApp.ExtSvc += ["AtRndmGenSvc"]
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel               = 3
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 10
# Generator fragment - the following 2 lines are needed to invoke Dpemc:

theApp.Dlls  += [ "Dpemc_i","TruthExamples"]
theApp.TopAlg = ["Dpemc","DumpMC"]

AtRndmGenSvc = Service( "AtRndmGenSvc" )
AtRndmGenSvc.Seeds = ["DPEMC 390020611 821000366", "DPEMC_INIT 820021 2347532"]
Dpemc = Algorithm( "Dpemc" )
# parameters are passed via the following line:
Dpemc.DpemcCommand = ["typepr EXC","typint QCD","iproc 9905","gapspr 0.03","prospr 0.9","cdffac 3.8","nstru 11","ifit 2"]
# 
#
# End of job options file
#
###############################################################
