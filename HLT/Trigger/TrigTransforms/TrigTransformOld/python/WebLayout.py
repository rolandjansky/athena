# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# vim: set fileencoding=utf-8 :
# Created by Hegoi Garitaonandia <hegoi.garitaonandia@cern.ch>
import os


# all links for the web pages will be created the following way: 
# D_WEBBASE/run_name/whatever_link


D_WEBBASE        = '/offhlt'         
D_SUMMARIES        = 'summaries'
D_EXPERTWEB        = 'expert'
D_CRASHES        = 'crashes'
D_INTERNAL        = 'internal'
D_LOGS                = 'logs'



F_MAIN                        = "index.html"
F_CONFIGURATION                = "configuration.html"
F_CRASHES                = "crashes.html"
F_TIMES                        = "times.html"
F_PATHS                        = "paths.html"
F_OUTPUT_PATHS                = "outpaths.html"
F_STREAMING                = "streaming.html"

URL_HELP        = "https://twiki.cern.ch/twiki/bin/view/Atlas/OfflineHLT"

create_dirs        = [ D_SUMMARIES, D_EXPERTWEB, D_CRASHES, D_INTERNAL,D_LOGS,D_WEBBASE]

mailto                 = {'string':'this page is maintained by ','name':'NOSPAM@cern.ch','to':"mailto:Hegoi.Garitaonandia@cern.ch?SUBJECT=HLTOfflineBug"}


# this will be relative to the run name
all_inside_links = [\
("Run Summary"                                        ,F_MAIN),\
("Configuration"                                ,os.path.join(D_SUMMARIES, F_CONFIGURATION)),\
("Python and Root Files",D_INTERNAL),\
("Input Files"                                        ,os.path.join(D_SUMMARIES, F_PATHS)),\
("Output Files"                                        ,os.path.join(D_SUMMARIES, F_OUTPUT_PATHS)),\
("All Summaries"                                ,D_SUMMARIES),\
("Online Histogramming"                                ,D_EXPERTWEB),\
("Per Event Timing"                                ,os.path.join(D_SUMMARIES, F_TIMES)),\
("Crashes / Timeouts"                                ,os.path.join(D_SUMMARIES, F_CRASHES)),\
]

# this will be absolute links
all_outside_links = [\
("Help (Atlas Twiki)"                                                ,URL_HELP)\
]

search='''
<!-- SiteSearch Google -->
<form method="get" action="http://www.google.com/custom" target="_top">
<table border="0" bgcolor="#ffffff">
<tr><td nowrap="nowrap" valign="top" align="left" height="32">
<!--<a href="http://www.google.com/"><img width=210 src="img/escher.gif" border="0" alt="Google"></img></a>-->
<small>Search this site</small>
</td><td nowrap="nowrap">
<input type="hidden" name="domains" value="cern.ch"></input>
<input type="hidden" name="sitesearch" value="cern.ch"></input>
<input type="text" name="q" size="31" maxlength="255" value=""></input>
<input type="submit" name="sa" value="Search"></input>
</td></tr><tr><td>&nbsp;</td>
<td nowrap="nowrap">
<table><tr><td>

</td><td>
</td></tr></table>
<input type="hidden" name="client" value="pub-1444957896811922"></input>
<input type="hidden" name="forid" value="1"></input>
<input type="hidden" name="ie" value="ISO-8859-1"></input>
<input type="hidden" name="oe" value="ISO-8859-1"></input>
<input type="hidden" name="hl" value="en"></input>
</td></tr></table>
</form>
<!-- SiteSearch Google -->
'''
