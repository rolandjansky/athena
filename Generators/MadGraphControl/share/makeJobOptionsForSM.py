if not 'listOfRuns' in dir():
    listOfRuns = [164656, 164891]
if not 'campaign' in dir():
    campaign = 'MC12'
print 'Compiling job options for runs in',listOfRuns

import os
from MadGraphControl.SetupMadGraphEventsForSM import setupMadGraphEventsForSM
for run in xrange(listOfRuns[0],listOfRuns[1]):
    partialString = setupMadGraphEventsForSM(run_madgraph=False,runNumber=run)
    if partialString is None or partialString=='': continue
    f = open(os.getcwd()+'/{0}.{1}.py'.format(campaign,partialString),'w')
    f.write("stringy=''\n")
    f.write("include('MadGraphControl/preInclude.SMwithMG.py')\n")
    f.write("include('MadGraphControl/{0}.SMwithMG_P6.py')\n".format(campaign))
    f.close()

theApp.exit()
