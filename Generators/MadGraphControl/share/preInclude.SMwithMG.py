# Generator transform pre-include
#  Runs MadGraph on the fly baseed on requested configuration

# Needs to keep, at the end, events.lhe
from MadGraphControl.MadGraphUtils import *
from MadGraphControl.SetupMadGraphEventsForSM import setupMadGraphEventsForSM

stringy = ''
def _setupMadGraphEventsForSM(run_madgraph=True,runNumber=None,runArgs=None,evgenConfig=None):
    print dir()
    if runNumber is None:
        if not 'runArgs' in dir():
            print 'Attempting to use this preInclude outside of the evgen transform is not recommended.  Will return.'
            print 'Please directly call this function in your python:'
            print 'from MadGraphControl.SetupMadGraphEventsForSM import setupMadGraphEventsForSM'
            print 'setupMadGraphEventsForSM(run_madgraph=run_madgraph,runNumber=None)'
            return
        if not hasattr(runArgs,"runNumber"):
            print 'ERROR: You didnt provide a run number.  Generation will fail.  Bailing out.'
            return
        setupMadGraphEventsForSM(run_madgraph=run_madgraph,runNumber=runArgs.runNumber,runArgs=runArgs,stringy=stringy,evgenConfig=evgenConfig)
    else:
        setupMadGraphEventsForSM(run_madgraph=run_madgraph,runNumber=None,runArgs=None,stringy=None,evgenConfig=evgenConfig)

if 'EventMultiplier' in dir(): runArgs.EventMultiplier = EventMultiplier
_setupMadGraphEventsForSM(runArgs=runArgs,evgenConfig=evgenConfig)
del _setupMadGraphEventsForSM
