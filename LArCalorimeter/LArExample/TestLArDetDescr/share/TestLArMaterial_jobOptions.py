###############################################################
#
# jobOptions file for LArDetDescr loading and CaloDDE tests
#==============================================================
# unfortunately, need to read dummy events !
# but this one also includes the mandatory one... 
# AthenaCommon/Atlas.UnixStandardJob.txt
#
theApp.setup( MONTECARLO )
include( "AthenaCommon/Atlas.UnixStandardJob.py" )

theApp.EvtMax = 1
# Set output level threshold 
#(0=ALL 1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel               = 3

# make sure that other subdet are not built
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
GlobalFlags.DataSource.set_geant4()
GlobalFlags.Luminosity.set_zero()

#from AthenaCommon.DetFlags import DetFlags
#DetFlags.detdescr.all_setOff()
#DetFlags.detdescr.LAr_setOn()
# this is now done in the standalone jobOpt :

include( "LArDetDescr/LArDetDescr_standalone_joboptions.py" )
include( "CaloTrackingGeometry/CaloSurfaceBuilder_jobOptions.py" )

theApp.Dlls += [ "TestLArDetDescr"]
theApp.TopAlg += ["TestLArMaterial" ]
