# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#####################################
# Utility class
#######################################

from math import sqrt

class MixingSelector(object):
        """This class configures one EventSelector with the MC dataset ID and the average events per job.
           Later, the user can add lists of physical file names, or equivalent DSIDs (e.g. 5010 == 5030).
           This class only works properly for fixed-sized input files. """
        def __init__(self, numJobs, datasetName, eventsRequired, eventsPerFile=50):
                self.__np = numJobs
                self.__dsid = datasetName
                self.__evReq = eventsRequired
                self.__evPerFile = eventsPerFile
                self.__aliases = [ datasetName ]
                self.__physical = []
                print("*** MixingPartitioner: INFO Created new selector Selector%i requesting an average of %f events per job ***" % (datasetName, eventsRequired))
        def name(self):
                return "Selector"+str(self.__dsid)
        def __str__(self):
                """ These are for printing """
                return self.name()+"[ev/job="+str(self.__evReq)+",nFiles="+str(len(self.__physical))+"]"
        def __repr__(self):
                return self.__str__()
        def DSID(self):
                return self.__dsid
        def numFiles(self):
                return len(self.__physical)
        def numEvents(self):
                return self.__evPerFile * len(self.__physical)
        ### functions to add new files to the selector ###
        def addAlias(self,newAlias):
                """If some downstream module is weighting prescales based on MC run number, inform it that run number newAlias is equivalent to this __dsid."""
                if newAlias not in self.__aliases:
                        print("*** MixingPartitioner: INFO \tEventInfo run number %i interpreted like %i. ***" % (newAlias, self.__dsid))
                        self.__aliases += [ newAlias ]
        def addNewCatalog(self, pfnlist):
                if len(pfnlist) == 0:
                        print("*** MixingPartitioner: WARNING Adding empty list to %s?" % self.name())
                        return
                if self.numFiles():
                        print("*** MixingPartitioner: INFO Files (%s ...) will be appended to %s. ***" % (pfnlist[0], self.name()))
                else:
                        print("*** MixingPartitioner: INFO Using files (%s ...) to initialize %s. ***" % (pfnlist[0], self.name()))
                self.__physical += pfnlist
        ### functions to calculate staging and mixer configuration ###                                )
        def evAvailablePerJob(self):
                #This allows a three-sigma "buffer" between jobs.
                evraw = (1.0*self.numEvents())/self.__np
                return (2 + evraw + 9 - 3 * sqrt(4 * evraw + 9))
        def isSufficient(self):
                if True: #For safety, you could redefine 'sufficient' to include in-file buffers.
                        return self.__evReq*self.__np <= (self.numEvents())
                else:
                        return self.evAvailablePerJob() >= self.__evReq
        def weight(self):
                #This works regardless of how isSufficient() is defined.
                if self.isSufficient():
                        return 1.0
                else:
                        return self.evAvailablePerJob()/self.__evReq
        def firstFileIndex(self, fractionDone):
                #Always use maximal job spacing.
                return int ((self.numEvents()*fractionDone)/self.__evPerFile)
        def firstEventInFile(self, fractionDone) :
                return int (self.numEvents()*fractionDone) % self.__evPerFile
        def lastFileIndex(self, fractionDone, lumiFraction):
                if not self.isSufficient():
                        # maximum of 1 file overlap!
                        return int( (self.numEvents()*(fractionDone + lumiFraction))/self.__evPerFile) + 1
                lastProbableFile = self.firstFileIndex(fractionDone) + 1
                eventsRequired = self.__evReq * (lumiFraction * self.__np)
                lastProbableFile += int ((eventsRequired + 3 * sqrt(eventsRequired))/self.__evPerFile) #allow 3 sigma fluctuation.
                if lastProbableFile > self.numFiles():
                        lastProbableFile = self.numFiles()
                return lastProbableFile
        def totalEventsThisJob(self,lumiFraction):
                if self.isSufficient():
                        return self.__evReq * (lumiFraction * self.__np)
                else:
                        return self.evAvailablePerJob() * (lumiFraction * self.__np) #luminosities should not vary too much per job.
        def trigger(self,fractionDone,lumiFraction):
                firstEvt = self.firstEventInFile(fractionDone)
                return "EventSelectorAthenaPool/" + self.name() + ":" + str(firstEvt) + ":"  + str(int(firstEvt + self.totalEventsThisJob(lumiFraction)))
        def Equivalents(self):
                return self.__aliases
        def FilesToStage(self,fractionDone,lumiFraction):
                return self.__physical[self.firstFileIndex(fractionDone):self.lastFileIndex(fractionDone,lumiFraction)]
