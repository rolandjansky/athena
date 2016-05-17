#*****************************************************************
#
# """topOptions file for Tile Reconstruciton and Monitoring in Athena""" 
# """This topOptions is intended to test the monitoring code"""
#=================================================================


# setting Atlantis flags for ED
if not 'doAtlantis' in dir():
    doAtlantis=True

if not 'OnlineAtlantis' in dir():
    OnlineAtlantis=True

include( "TileMonitoring/TileRec_topOptions.py" )
