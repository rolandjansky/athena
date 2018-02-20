# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
ATLAS beam spot web page template, based on generic WebPage template.
"""

__author__  = 'Juerg Beringer'
__version__ = 'BeamSpotWebPage.py atlas/athena'


import os
from WebPage import WebPage


header = """\
<img id="logo" src="../images/logo.png">
<h1>%(pageTitle)s</h1>
<ul>
<li><a %(href_t0Summary)s>T0 Summary</a></li>
<li><a %(href_bcidSummary)s>PerBCID</a></li>
<li><a %(href_reprocSummary)s>Reprocessing</a></li>
<li><a %(href_jobs)s>Jobs</a></li>
<li><a %(href_files)s>Job files</a></li>
<li><a %(href_webplots)s>Overview</a></li>
<li><a %(href_myplots)s>Plots</a></li>
<li><a href="http://atlasdqm.cern.ch/webdisplay/tier0/">DQMF</a></li>
<li><a href="https://twiki.cern.ch/twiki/bin/view/Atlas/BeamSpot">TWiki</a></li>
</ul>
<br class="clearfloat" />
"""

footer = """\
<div class="floatleft">
Contact:
<script type="text/javascript">
var a = 'juerg'
var b = '.beringer'
var c = '@cern.ch'
document.write('<a href="mailto:'+a+b+c+'">Juerg Beringer</a>')
</script>
</div>
<div class="floatright">%(timeStamp)s</div>
<div style="clear: both;"></div>
"""


class BeamSpotWebPage(WebPage):

    def configure(self):
        self.pageConfig['cssName'] = self.globalConfig.baseUrl+'/css/beamspot.css'
        if not self.pageConfig.get('pageTitle',''):
            self.pageConfig['pageTitle'] = 'ATLAS Beam Spot Web Page'
        self.pageConfig['header'] = header
        self.pageConfig['footer'] = footer
        WebPage.configure(self)


class DummyPage(BeamSpotWebPage):
    def __init__(self):
        BeamSpotWebPage.__init__(self,content="<h3>We're sorry, this feature is not yet available.</h3>")
