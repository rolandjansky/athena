# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
ATLAS Beam Spot web page. This is the root page that sets up the tree of beam spot
web pages. This is intended to be run by CherryPy through Apache's mod_python, but
you can test it locally using `python index.py`.

Sample Apache configuration:

<Location "/">
    PythonPath "sys.path+['/home/jb/atl/InDetBeamSpotExample/www']"
    SetHandler python-program
    PythonHandler cherrypy._cpmodpy::handler
    PythonOption cherrypy.setup index::setup_server
    PythonDebug On
</Location>
"""

__author__  = 'Juerg Beringer'
__version__ = 'index.py atlas/athena'

import os
import cherrypy

from BeamSpotWebPage import BeamSpotWebPage, DummyPage
from BeamSpotSummary import BeamSpotSummary
from JobBrowser import JobBrowser
from JobDetails import JobDetails
from FileBrowser import FileBrowser
from PlotServer import PlotServer
from MyPlots import MyPlots
from WebPlots import WebPlots
from DebugPage import DebugPage


# Application configuration
currentDir = os.path.dirname(os.path.abspath(__file__))
beamSpotConfig = {
    '/':       { 'tools.staticdir.root': currentDir},
    '/css':    { 'tools.staticdir.on': True,
                 'tools.staticdir.dir': 'css'},
    '/js':     { 'tools.staticdir.on': True,
                 'tools.staticdir.dir': 'js'},
    '/images': { 'tools.staticdir.on': True,
                 'tools.staticdir.dir': 'images'},
    '/tmp':    { 'tools.staticdir.on': True,
                 'tools.staticdir.dir': 'tmp'}
}


# Root page configuration
root = BeamSpotWebPage()
root.globalConfig.baseUrl =  '/webapp'

root.globalConfig['jobDir'] = os.environ.get('JOBDIR', '/afs/cern.ch/user/a/atlidbs/jobs')
root.globalConfig['taskDb'] = os.environ.get('TASKDB', 'auth_file:/afs/cern.ch/user/a/atlidbs/private/beamspotdbinfo_r.dat')
root.globalConfig['wwwDir'] = os.environ.get('WWWDIR', '/var/vhost/atlas-beamspot/secure/InDetBeamSpotExample/www')
root.globalConfig['ntDir']  = os.environ.get('NTDIR', '/afs/cern.ch/user/a/atlidbs/nt')
root.globalConfig['periodDir'] = os.environ.get('PERIODDIR','/afs/cern.ch/user/a/atlidbs/nt/DataPeriods')
root.globalConfig['webPlotDir']  = os.environ.get('WEBPLOTDIR', '/afs/cern.ch/user/a/atlidbs/nt/webplots')

# Local for testing
#root.globalConfig['jobDir'] = os.environ.get('JOBDIR', '/data/atlas/jobs/atlidbs')
#root.globalConfig['taskDb'] = os.environ.get('TASKDB', 'sqlite_file:'+root.globalConfig['jobDir']+'/taskdata.db')
#root.globalConfig['wwwDir'] = os.environ.get('WWWDIR', '/home/jb/ATLAS/BeamSpot/InDetBeamSpotExample/www')
#root.globalConfig['ntDir']  = os.environ.get('NTDIR', '/data/atlas/analysis/BeamSpot/nt')
#root.globalConfig['periodDir'] = os.environ.get('PERIODDIR','/home/jb/analysis/BeamSpot/nt/DataPeriods')
#root.globalConfig['webPlotDir']  = os.environ.get('WEBPLOTDIR', '/data/atlas/analysis/BeamSpot/nt/webplots')

# Page tree
root.addPage('summary', BeamSpotSummary())
root.addPage('t0Summary', BeamSpotSummary(), query='?type=DB_BEAMSPOT')
root.addPage('bcidSummary', BeamSpotSummary(), query='?type=BCID&limit=0')
root.addPage('reprocSummary', BeamSpotSummary(), query='?type=REPR&limit=0')
root.addPage('jobs',    JobBrowser())
root.addPage('details', JobDetails())
root.addPage('files',   FileBrowser('/jobfiles'))
root.addPage('myplots', MyPlots())
root.addPage('plots',   PlotServer())
root.addPage('webplots',WebPlots())
root.addPage('debug',   DebugPage())


# Setup for mod_python
def setup_server():
    cherrypy.config.update({'environment': 'production',
                            'log.screen': False,
                            'server.socket_host': '127.0.0.1',
                            'log.error_file': '/tmp/site.log',
                            'show_tracebacks': False})
    cherrypy.tree.mount(root, root.globalConfig.baseUrl, config=beamSpotConfig)


# Code to test or run locally
if __name__ == '__main__':

    # For plain CGI
    #import cgi
    #import cgitb; cgitb.enable()
    #p = Root(pageTitle = 'HelloWorld Test', contentType='Content-type: text/html\n\n')
    #print p.index()

    # For local CherryPy (run as `python index.py')
    # NOTE: see http://www.cherrypy.org/wiki/StaticContent on how to
    #       serve static content such as the css file
    import cherrypy
    cherrypy.quickstart(root, root.globalConfig.baseUrl, config=beamSpotConfig)
