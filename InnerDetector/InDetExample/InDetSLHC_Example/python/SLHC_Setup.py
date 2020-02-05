""" SLHC_Setup
    Python module to hold storegate keys of InDet objects.
"""

# The content of this file is completely replicated by other files
# - kept temporarily to allow consistent tests commands while other changes are being made
# but should be removed from standard test configurations and the package in a next iteration

__author__ =   "A. Salzburger"
__version__=   "$Revision: 1.13 $"
__doc__    =   "SLHC_PathSetting"
__all__    = [ "SLHC_PathSetting" ]

import os
from os.path import exists, join
from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags

class SLHC_Setup :
    # constructor requires the SLHC_Flags
    def __init__(self):

        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
   
        # Alignments have to be disabled for Pixels, but only if they are included
        try:
            pixelTool = svcMgr.GeoModelSvc.DetectorTools['PixelDetectorTool']
            pixelTool.Alignable = False
        except IndexError:
            print "    SLHC_Setup.py: Warning: Cannot turn off pixel alignment (perhaps because pixels are not included?)"

        
    def search_file(self,filename, search_path):
        """Given a search path, find file
           -- will return the first occurrence
        """
        file_found = 0
        paths = search_path.split(os.pathsep)
        for path in paths:
            if exists(join(path, filename)):
                file_found = 1
                break
        if file_found:
            return path
        else:
            return None
