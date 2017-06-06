# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
ATLAS beam spot plot server.
"""

__author__  = 'Juerg Beringer'
__version__ = 'DebugPage.py atlas/athena'

from BeamSpotWebPage import BeamSpotWebPage
from InDetBeamSpotExample.TaskManager import *

import sys, commands
import cherrypy

errorMsg = """\
<div class="errormsg">
%s
</div>
"""

class PlotServer(BeamSpotWebPage):

    def content(self,**args):
        """When called without a run number argument in the URL, generate a web page with an error message"""
        return errorMsg % 'ERROR: You must specify a run number in the URL (e.g. http://..../plots/160879)'

    def default(self,run):
        """Return a summary gif image for the run specified in the URL. If no such image exists (or the run
           doesn't exist), currently returns a zero-length image."""
        cherrypy.response.headers['Content-Type'] = "image/gif"
        try:
            with TaskManager(self.globalConfig['taskDb']) as taskman:
                t = taskman.taskIterDict('*',['where RUNNR =',DbParam(int(run)),"and TASKNAME like 'DB_BEAMSPOT%%' order by UPDATED desc"]).next()
            wantedFile = 'PlotBeamSpot.gif'
            for f in t['RESULTFILES'].split():
                if f[-len(wantedFile):] == wantedFile: break
            path = '/'.join([self.globalConfig['jobDir'],t['DSNAME'],t['TASKNAME'],f])
            return open(path,'r').read()
        except:
            return None
    default.exposed = True
