#!/bin/sh
## runcmds found in <../test/OnlineRecoTests_DefaultConfiguration.xml> : 


## The following (offline) jobs wait for the online jobs, and copy over the online results
## They need the offline timestampfile: latest_copied_release to see if the jobs need to be evaluated.

##
ort_getOnlineResults.py latest_copied_release http://atlas-project-fullchaintest.web.cern.ch/atlas-project-FullChainTest/tier0/rtt/webtimestamp http://atlas-project-fullchaintest.web.cern.ch/atlas-project-FullChainTest/tier0/rtt/webfilelist0

##
ort_getOnlineResults.py latest_copied_release webtimestamp http://atlas-project-fullchaintest.web.cern.ch/atlas-project-FullChainTest/tier0/rtt/webfilelist1

##
ort_getOnlineResults.py latest_copied_release webtimestamp http://atlas-project-fullchaintest.web.cern.ch/atlas-project-FullChainTest/tier0/rtt/webfilelist2

##
ort_finishedMail.py latest_copied_release mbaak@cern.ch,Anadi.Canepa@cern.ch

##
tct_finishedMail.py mbaak@cern.ch,acanepa@cern.ch,stelzer-chilton@triumf.ca,bernd.stelzer@cern.ch http://atlas-project-fullchaintest.web.cern.ch/atlas-project-FullChainTest/tier0/rtt/TCT.Results/page0.php

