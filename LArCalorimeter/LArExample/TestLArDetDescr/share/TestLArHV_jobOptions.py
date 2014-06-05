###############################################################
#
# jobOptions file for LArDetDescr loading and CaloDDE tests
#==============================================================
# unfortunately, need to read dummy events !
# but this one also includes the mandatory one... 
# AthenaCommon/Atlas.UnixStandardJob.txt
#

doStandalone = False

if doStandalone :

    from AthenaCommon.GlobalFlags import GlobalFlags
    GlobalFlags.DataSource.set_geant4()

    theApp.setup( MONTECARLO )

    theApp.EvtMax = 1

    # Set output level threshold 
    #(0=ALL 1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
    MessageSvc = Service( "MessageSvc" )
    MessageSvc.OutputLevel               = 3

    include( "DetDescrCnvSvc/DetStore_joboptions.py" )

include( "LArRecUtils/LArHVGeometryTool_jobOptions.py" ) 

theApp.Dlls += [ "TestLArDetDescr" ]
theApp.TopAlg += ["TestLArHV" ]
