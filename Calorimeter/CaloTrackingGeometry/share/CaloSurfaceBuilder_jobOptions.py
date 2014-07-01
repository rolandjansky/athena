from AthenaCommon.GlobalFlags import GlobalFlags
###################################################################################
# CaloSurfaceBuilder
###################################################################################
# Python Setup Class for CaloSurfaceBuilder
#
# Author: claire.bourdarios@cern.ch
#
# Date: 26/09/2005
#
###################################################################################

if 'CaloTrackingGeometry' not in theApp.Dlls:
    theApp.Dlls = [ 'CaloTrackingGeometry' ]

# CaloDepthTool is here :
if 'CaloDetDescr' not in theApp.Dlls:
    theApp.Dlls   += [ 'CaloDetDescr' ]

# contains the implementation of the CaloDetDescr interfaces :	 
if 'LArDetDescr' not in theApp.Dlls:	 
     theApp.Dlls   += [ 'LArDetDescr'  ]

#uncomment if you want a jobOpt :
#CaloSurfaceBuilder = Algorithm("CaloSurfaceBuilder")
#CaloSurfaceBuilder.CaloDepthTool.DepthChoice= "entrance"



# and TestBeam contains the implementation of the ICaloCoordinate interface :
include ( 'TBCaloGeometry/TBCaloGeometry_H8_joboptions.py' )
