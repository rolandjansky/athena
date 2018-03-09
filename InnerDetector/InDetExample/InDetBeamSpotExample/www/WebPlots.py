# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
ATLAS beam spot server for pre-made web plots (e.g. weekly beam spot history).
"""

__author__  = 'Juerg Beringer'
__version__ = 'WebPlots.py atlas/athena'

from BeamSpotWebPage import BeamSpotWebPage

import sys, os, time

errorPage = """\
<div class="errormsg">
We're sorry - this plot (%s) is currently not available.
</div>
"""

page = """
<div class="boldtext">
Last updated:  %s
</div>
<img id="webplot" src="/webplots/%s">
"""

class WebPlots(BeamSpotWebPage):

    def __init__(self):
        BeamSpotWebPage.__init__(self)
        self.pageConfig['pageTitle'] = 'ATLAS Beam Spot Overview'

    def content(self,**args):
        plotName = args.get('plot','summary-weekly.gif')
        webPlotDir = self.globalConfig['webPlotDir']
        filePath = os.path.join(webPlotDir,plotName)
        if not os.path.exists(filePath):
            return errorPage % plotName
        lastUpdate = time.ctime(os.path.getctime(filePath))
        return page % (lastUpdate,plotName)
