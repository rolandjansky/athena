# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
ATLAS beam spot web page for browsing directories with beam spot job data.
"""

__author__  = 'Juerg Beringer'
__version__ = '$Id: FileBrowser.py 467432 2011-11-10 16:36:27Z beringer $'

from BeamSpotWebPage import BeamSpotWebPage

fileBrowser = """
<iframe src="%s">
<p>Your browser does not support iFrames.</p>
</iFrame>
"""


class FileBrowser(BeamSpotWebPage):

    def __init__(self,baseUrl):
        BeamSpotWebPage.__init__(self)
        self.pageConfig['pageTitle'] = 'ATLAS Beam Spot Job File Browser'
        self.baseUrl = baseUrl

    def content(self,**args):
        url=self.baseUrl
        if 'u' in args:
            url += '/'+args['u']
        return fileBrowser % (url)
