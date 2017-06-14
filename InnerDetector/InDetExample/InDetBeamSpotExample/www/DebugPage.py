# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
ATLAS beam spot web page for debugging
"""

__author__  = 'Juerg Beringer'
__version__ = 'DebugPage.py atlas/athena'

from BeamSpotWebPage import BeamSpotWebPage
from InDetBeamSpotExample.Utils import getUserName

import sys, os, commands


debuginfo = """
<div class="text">
<pre>
Python version:   %s
Web server user:  %s
whoami output:    %s
HOME dir:         %s
URL base path:    %s

List of pages:    %s
Page attributes:  %s

Job directory:    %s
TaskManager DB:   %s
www directory:    %s
nt directory:     %s
</pre>
</div>
"""


class DebugPage(BeamSpotWebPage):

    def content(self,**args):
        # Make sure we don't expose any password stored in oracle connection strings
        taskDb = 'oracle:******' if 'oracle:' in self.globalConfig['taskDb'] else self.globalConfig['taskDb']
        return debuginfo % (sys.version,
                            getUserName(),
                            commands.getoutput('whoami'),
                            os.environ.get('HOME','not defined'),
                            self.globalConfig.baseUrl,
                            self.globalConfig.pageList,
                            self.globalConfig.pageAttributes,

                            self.globalConfig['jobDir'],
                            taskDb,
                            self.globalConfig['wwwDir'],
                            self.globalConfig['ntDir'])
