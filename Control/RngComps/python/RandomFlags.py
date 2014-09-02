# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#=======================================================================
# File:   RandomFlags.py
#=======================================================================
"""  job properties used to configure Random Number Services

"""
__author__  = 'J Chapman'

#=======================================================================
# imports
#=======================================================================
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties

from AthenaCommon.Logging import logging
logRandomFlags = logging.getLogger('RandomFlags')

class RandomSeedList(JobProperty):
    """Random number stream seeds
    """
    statusOn = True
    allowedTypes = ['list']
    StoredValue = []

    def checkForExistingSeed(self, name):
        """Ensure that each stream is only initialized once
        """
        found = False
        seedlist = self.get_Value()
        for iseed in seedlist:
            found = iseed.startswith(name+" ")
            if found:
                break
        return found

    def addSeed( self, name, seed1, seed2 ):
        """Add seeds to internal seedlist. Seeds will be incremented by offset values
        """
        offset = jobproperties.RandomFlags.RandomSeedOffset.get_Value()
        newseed = name + " OFFSET " + str(offset) + " " + str(seed1) + " " + str(seed2)
        logRandomFlags.info("Adding random number stream seed '" + newseed + "'")

        ## Ensure that each stream is only initialized once
        found = self.checkForExistingSeed(name)
        if found:
            logRandomFlags.error ("Initialization values for random number stream " + name + " already exist!")
        else:
            seedlist = self.get_Value()
            seedlist += [newseed]
            self.set_Value (seedlist)

        return

    def printSeeds( self ):
        """print random seeds
        """
        from AthenaCommon.ConfigurableDb import getConfigurable
        rndmSvc = getConfigurable(jobproperties.RandomFlags.RandomSvc.get_Value())()
        logRandomFlags.info ("Random Number Seeds stored in RandomFlags.RandomSeedList: " + str(self.get_Value()))
        logRandomFlags.info ("Random Number Seeds attached to Service '" + rndmSvc.name() + "': " + str(rndmSvc.Seeds))
        return

    def checkRndmSvc(self):
        """Check if the random number service has already been defined
        """
        from AthenaCommon.AppMgr import ServiceMgr
        if hasattr( ServiceMgr , jobproperties.RandomFlags.RandomSvc.get_Value() ):
            from AthenaCommon.ConfigurableDb import getConfigurable
            rndmSvc = getConfigurable(jobproperties.RandomFlags.RandomSvc.get_Value())()
            if len(rndmSvc.Seeds) != 0:
                logRandomFlags.warn (rndmSvc.name() + ".Seeds is not empty!")
                logRandomFlags.warn ("Random Number Seeds already attached to Service '" + rndmSvc.name() + "': " + str(rndmSvc.Seeds))
                logRandomFlags.warn ( "Please use simFlags.RandomSeedList.addSeed() instead!")
                for seedstring in rndmSvc.Seeds:
                    if 'OFFSET' not in seedstring:
                        logRandomFlags.warn ( "Existing Seed:'" + seedstring + "' incorrectly defined - missing OFFSET! Removing...")
                    else:
                        ## If seed is correctly formatted add seed properly
                        ## after checking it hasn't already been defined in the stream list.
                        splitseedstring = seedstring.split()
                        if self.checkForExistingSeed(splitseedstring[0]):
                            logRandomFlags.error ("Initialization values for random number stream " + splitseedstring[0] + " already exist!")
                        else:
                            self.addSeed( splitseedstring[0], splitseedstring[3], splitseedstring[4] )
                rndmSvc.Seeds = []
        return

    def addtoService(self):
        """Attach seeds stored in RandomFlags.RandomSeedList to ATLAS random number sevice
        """
        from AthenaCommon.ConfigurableDb import getConfigurable
        rndmSvc = getConfigurable(jobproperties.RandomFlags.RandomSvc.get_Value())()
        logRandomFlags.info ("Adding random number seeds stored in RandomFlags.RandomSeedList to Random Number Service '" + rndmSvc.name() + "'")
        if jobproperties.RandomFlags.readSeedsFromFile.get_Value():
            rndmSvc.Seeds = []
            rndmSvc.ReadFromFile = True
            rndmSvc.FileToRead = jobproperties.RandomFlags.rndmSeedInputFile.get_Value()
            logRandomFlags.warning("Random seeds will be read from the file %s", jobproperties.RandomFlags.rndmSeedInputFile.get_Value())
        else:
            self.checkRndmSvc()
            rndmSvc.Seeds += self.get_Value()
        from GaudiKernel.Configurable import WARNING
        rndmSvc.OutputLevel = WARNING
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr( ServiceMgr , jobproperties.RandomFlags.RandomSvc.get_Value() ):
            ServiceMgr += rndmSvc
        return

class RandomSvc(JobProperty):
    """Name of the Athena random number service to use,
       e.g. 'AtDSFMTGenSvc' or 'AtRanluxGenSvc' or 'AtRndmGenSvc'.
    """
    statusOn = True
    allowedTypes = ['str']
    StoredValue = 'AtDSFMTGenSvc'


class RandomSeedOffset(JobProperty):
    """Integer value that will be added to initialization value the
       seed for each random number stream.  Default value (=0) will be
       always be used in straight athena jobs.
    """
    statusOn = True
    allowedTypes = ['int']
    StoredValue = 0

class readSeedsFromFile(JobProperty):
    """ Should seeds for the random number service be read in from a file?
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class RandomSeedInputFile(JobProperty):
    """ Name of file containing the seeds for the Random Number Service to use
    """
    statusOn = True
    allowedTypes = ['str']
    StoredValue = 'AtRanluxGenSvc.in'

## Definition and registration of the RandomFlags container
class RandomFlags(JobPropertyContainer):
    """ The global RandomFlags flag/job property container.
    """
## Register and populate the RandomFlags container
jobproperties.add_Container(RandomFlags)
## We want always the following flags in the container
list_jobproperties = [RandomSvc,RandomSeedList,RandomSeedOffset,readSeedsFromFile,\
                    RandomSeedInputFile]

for i in list_jobproperties:
    jobproperties.RandomFlags.add_JobProperty(i)

#=======================================================================
## Short-cut alias (standard naming)
randomFlags = jobproperties.RandomFlags
