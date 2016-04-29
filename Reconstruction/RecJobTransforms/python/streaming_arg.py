# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__author__ = "Ayana.Holloway@cern.ch"
__doc__ = """Arguments for streaming batch test."""

from copy import copy
from PyJobTransformsCore.basic_trfarg import *
from RecJobTransforms.MixStreamConfig import *

class PartitionArg(IntegerArg):
    """Batch job ('partition') number of this job.  The trf will calculate the run/lumi/sfo number, and the offset into the input file lists, from this."""
    def __init__(self,help='default',name='default'):
        IntegerArg.__init__(self,help,name)
        self.__mixPartitioner = MixingPartitioner()

    def metaData(self):
        val = self.value()
        if val is None:
            return {}
        else:            
            return { self.name() : self.value(),
                     "pseudoRun" : mixStreamConfig.WhichRunNumber(self.value()),
                     "pseudoSFO" : mixStreamConfig.WhichSFO(self.value()),
                     "pseudoLumiBlock" : mixStreamConfig.WhichLumiBlock(self.value()),
                     "pseudoLumiFraction" : mixStreamConfig.ThisLuminosityFraction(self.value()) }        
                     
    def GetPartitioner(self):
        return self.__mixPartitioner

    def isFullArgument(self):
        return True

    def jobOrTask(self):        
        return 'job'

    def preRunAction(self):
        self.__mixPartitioner.SetPartition(self.value())
        self.__mixPartitioner.ConfigureSelectors()
        self.__mixPartitioner.preStageInputFiles()

    def WritePartitionJobOptions(self):
        EventMixerList, outstring = self.__mixPartitioner.ConfigureServices()
        el = "',\n'"
        sp = ", "

        outstring += '\nprint ":::::::::: STREAMING JOB CONFIGURATION: LHC INSTANTANEOUS LUMINOSITY= %f x 10^%i cm^-2 s^-1"' % (self.__mixPartitioner.ScaleFactor(), 31)
        outstring += "\nEventMixer.TriggerList += [ '%s' ]\n" % el.join(EventMixerList)

        weighted = [str(i) for i in self.__mixPartitioner.ListOfSamplesToWeight()]
        weights = [str(f) for f in self.__mixPartitioner.ListOfSampleFractions()]
        if len(self.__mixPartitioner.ListOfSampleColonWeights()):
            outstring += "\nWeightConfiguration = [ '%s' ]\n" % el.join(self.__mixPartitioner.ListOfSampleColonWeights())
        else:
            outstring += "\nWeightConfiguration = [ ]\n"
        outstring += "\nListOfSamplesToWeight =  [%s]\n" %  sp.join(weighted)
        outstring += "\nListOfSampleFractions =  [%s]\n" %  (sp).join(weights) 
        return outstring


#####################################

class FakeTableArg(BoolArg):
    """Toggle between simple trigger table and full table"""
    def __init__(self,help='Toggle between simple trigger table and full table',name='default'):
        BoolArg.__init__(self,help,name)

    def isFullArgument(self):
        return True

    def jobOrTask(self):
        return 'task'
