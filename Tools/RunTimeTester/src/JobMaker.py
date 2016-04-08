# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from formatCollection import formatCollection

# -------------------------------------------------------------------------
class JobMaker:
    "Construct a list of worker and watcher jobs"
    
    def __init__(self, xmlReader, paths, descFactory, minderFactory, logger):

        self.logger = logger
        self.logger.debug("JobMaker: __init__")

        
        self.jobMinders    = []
        self.paths         = paths
        self.xmlReader     = xmlReader
        self.descFactory   = descFactory
        self.minderFactory = minderFactory
        descs              = self.makeWorkerDescs()

        self.logger.debug("No of Worker descs: "+str(len(descs)))
        # [logger.debug(desc.dump()) for desc in descs]

        [self.makeWorkerMinder(desc)  for desc in descs]
        self.logger.debug("No of Worker jobs: "+str(len(self.jobMinders)))

        descs = self.makeWatcherDescs(self.jobMinders)
        self.logger.debug("No of Watcher descs: "+str(len(descs)))
        [self.makeWatcherMinder(desc) for desc in descs]

        self.logger.debug("No of Worker+Watcher jobs: "+str(len(self.jobMinders)))

        # debug for now: will  need to append watchers to jobMinders
        # [job.dump() for job in self.jobMinders]

    def makeWorkerMinder(self, desc):
        "use a worker descriptor to make a worker minder"
        
        msg = 'Creating a minder for descriptor %s' % desc.identifiedName
        self.logger.debug(msg)
        
        jm = self.minderFactory.create(desc)
        self.jobMinders.append(jm)

    def makeWatcherDescs(self, workerMinders):
        """
        make a list of watcher descriptors.
        !. Find confederations of worker __minders__
        2. For each federation, construct a watcher descriptor

        Keep track of the jobs associated with a watcher descriptor, and the
        name of of the confederation in dictionaries keyed on the descriptor.
        
        A federation is a set of jobs with the same value of the federation
        attribute. This is ususally set in the Jobs.xml config file
        """
        
        # list all descriptors with the confedration attribute set
 
        confederatedMinders = [jm for jm in workerMinders if jm.confederation]
        
        # place in dictionary key = confederation (text), val = [minders]
        
        dict = {}
        [dict.setdefault(jm.confederation, []).append(jm) for jm in confederatedMinders]

        # check that all jobs in a confederation are in the same job group
        
        watcherDescriptors = []

        
        for key in dict.keys():

            # a list of jobs...
            confederationMembers = dict[key]
            
            if not self.isIsoGroup(confederationMembers):
                # what should be done here?
                self.logger.error('Non iso group confederationMembers ! '+key)

            from getInheritanceHeirarchy import isAthenaJob
            athenaJobs = [isAthenaJob(minder) for minder in confederationMembers]
            
            allAreAthenaJobs = reduce(lambda x, y: x and y, athenaJobs)
            if allAreAthenaJobs:
                if not self.isIsoAthenaJob(confederationMembers):
                    # what should be done here?
                    self.logger.error('Non isoAthenaJob confederationMembers ! '+key)


            fJob   = confederationMembers[0]
            descriptor = self.descFactory.create('Watcher',
                                                 self.paths,
                                                 {'jobGroup':fJob.jobGroup+'Watcher'})
            
            if allAreAthenaJobs:            
                descriptor.datasets   = fJob.datasets

            # store the list of worker minders in the watcher descriptor
            # so the watcher can process workers' results
            descriptor.confederation = confederationMembers
            watcherDescriptors.append(descriptor)

        return watcherDescriptors
    

    def makeWatcherMinder(self, descriptor):
        "Minder construction"

        minder = self.minderFactory.create(descriptor)        
        self.jobMinders.append(minder)
    
        
    def isIsoGroup(self, confederation):
        "test if all jobs in a confederation are of the same jobGroup"
        
        fGroup = confederation[0].jobGroup
        
        # list the groups not equal to the jobGroup of the 0th job
        badGroups = [job.jobGroup for job in confederation if job.jobGroup != fGroup]

        if badGroups:
            msg = 'Job federation has mixed groups, exiting '+str(badGroups)
            self.logger.error(msg)
            exit(0)
            return False

        return True


    def isIsoAthenaJob(self, confederation):
        """ check jobs have certain athena job attributes, and that these attributes
        all have the same value"""
        
        # check all descs have certain attributes equal to that  of the first
        # areSimilar is a list of bools.
        areSimilar = [self.isAthenaSimilar(confederation[0], job)  for job in confederation]

        # if any differ, return false.
        athenaSimilar = reduce(lambda x, y: x and y, areSimilar)

        return athenaSimilar

    def isAthenaSimilar(self, job1, job2):
        "checks if a number of attributes are similar for athena descriptors"

        similar = True
        
        if job1.hasData() != job2.hasData():
            self.logger.error('job1 hasData() : %s, job2 hasData() : %s' % (job1.hasData(), job2.hasData()))
            similar = False


        if len([datasetType for datasetType in job1.datasets if datasetType not in job2.datasets]) != 0:
            self.logger.error('job1 contains data of a type not in job2!')
            similar = False
                
        #if job1.isDC1data  != job2.isDC1data:
        #    self.logger.error('job1 isDC1data : %s, job2 isDC1data : %s' % (job1.isDC1data, job2.isDC1data) )
        #    similar = False
        #if job1.DC1dataSet != job2.DC1dataSet:
        #    self.logger.error('job1 DC1dataSet : %s, job2 DC1dataSet : %s' % (job1.DC1dataSet, job2.DC1dataSet) )
        #    similar = False

        if job1.jobGroup != job2.jobGroup:
            self.logger.error('job1 jobGroup : %s, job2 jobGroup : %s' % (job1.jobGroup, job2.jobGroup) )
            similar = False

        #        if job1.isBSdata: similar =False

        return similar

    def makeWorkerDescs(self):
        "Obtain the worker job descriptors from the jobs xml file"

        try:
            descList = self.xmlReader.getDescriptors()
        except:
            self.logger.error('JobsXMLReader object could not create descs')
            return []

        if self.paths.isNightly:
            nBefore  = len(descList)
            descList = [desc for desc in descList if not desc.numberedOnly]
            nAfter   = len(descList)
            nCulled  = nBefore - nAfter
            msg = """numberedOnly selection: %d descriptors were removed; %d remain""" % (nCulled, nAfter)
            self.logger.info(msg)

        if self.paths.limitedNightlyVeto:
            nBefore  = len(descList)
            descList = [desc for desc in descList if not desc.limitedNightlies]
            nAfter   = len(descList)
            nCulled  = nBefore - nAfter
            msg = 'limitedNightlies: %d descriptors were removed; %d remain' % (nCulled, nAfter)
            self.logger.info(msg)

        return descList
    
    def __str__(self):
        str += self.__class__.__name__+'\n'
        str += formatCollection(self.__dict__)+'\n'
        return str

if __name__ == '__main__':
    pass
