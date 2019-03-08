################################################################################
# Package: InDetBeamSpotExample
################################################################################

# Declare the package name:
atlas_subdir( InDetBeamSpotExample )

# Necessary external(s):
find_package( cx_Oracle )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( bin/dumpBeamSpot.py bin/checkJobs.py bin/runJobs.py bin/beamSpotT0_ESDToDPD_trf.py bin/beamSpotT0_Vertex_tf.py bin/taskman.py bin/beamspotman.py bin/beamspotnt.py bin/pickleTool.py bin/plotBeamSpotMon.py bin/plotBeamSpotVert.py bin/plotBeamSpotVxVal.py bin/beamSpotT1_Vertex_trf.py bin/dumpLBFileMap.py bin/makeDatasetOnDiskList bin/plotBeamSpotCompare.py bin/dqBeamSpot.py bin/periodInfo.py bin/resolveBeamSpotTag.py bin/bsCompare.py bin/getLatestRuns.py bin/updateCoolNtuple.py bin/extractScanInfo.py )

# Aliases:
atlas_add_alias( plotBeamSpotVert "plotBeamSpotVert.py" )
atlas_add_alias( pickleTool "pickleTool.py" )
atlas_add_alias( resolveBeamSpotTag "resolveBeamSpotTag.py" )
atlas_add_alias( beamspotnt "beamspotnt.py" )
atlas_add_alias( plotBeamSpotCompare "plotBeamSpotCompare.py" )
atlas_add_alias( dumpBeamSpot "dumpBeamSpot.py" )
atlas_add_alias( runJobs "runJobs.py" )
atlas_add_alias( plotBeamSpotMon "plotBeamSpotMon.py" )
atlas_add_alias( dqBeamSpot "dqBeamSpot.py" )
atlas_add_alias( extractScanInfo "extractScanInfo.py" )
atlas_add_alias( checkJobs "checkJobs.py" )
atlas_add_alias( taskman "taskman.py" )
atlas_add_alias( bsCompare "bsCompare.py" )
atlas_add_alias( plotBeamSpotVxVal "plotBeamSpotVxVal.py" )
atlas_add_alias( periodInfo "periodInfo.py" )
atlas_add_alias( beamspotman "beamspotman.py" )
atlas_add_alias( dumpLBFileMap "dumpLBFileMap.py" )

