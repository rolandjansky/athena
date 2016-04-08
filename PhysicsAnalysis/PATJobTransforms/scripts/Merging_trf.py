#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Merge several input pool files into a single output file. Make TAG from merged file."""

from PyJobTransformsCore.trf import *
from PyJobTransformsCore.full_trfarg import *
from PyJobTransformsCore.trfutil import *

from PATJobTransforms.BaseOfCompositeTrf import BaseOfCompositeTrf
from PATJobTransforms.AODtoTAG_trf import AODtoTAGJobTransform
from PATJobTransforms.AODtoDPD_trf import AODtoDPDJobTransform
from PATJobTransforms.MergePool_trf import MergePoolJobTransform
from PATJobTransforms.MergeHIT_trf import MergeHITSJobTransform
from PATJobTransforms.MergeRDO_trf import MergeRDOJobTransform

ListOfDefaultPositionalKeys=['maxEvents','skipEvents','RunNumber','fastPoolMerge','inputAODFile','inputESDFile','outputAODFile','outputESDFile','outputTAGFile','DBRelease','geometryVersion','conditionsTag','autoConfiguration','preInclude','postInclude','preExec','postExec','--ignoreerrors','--athenaopts','--omitvalidation','extraParameter','inputHitsFile','outputHitsFile','inputLogsFile','outputNTUP_SUSYFile','outputNTUP_TOPFile','outputNTUP_TOPELFile','outputNTUP_TOPMUFile','outputNTUP_TOPJETFile','outputNTUP_TOPEJETFile','outputNTUP_PHOTONFile', 'outputNTUP_FASTMONFile','outputNTUP_HSG2File','inputRDOFile','outputRDOFile']


class MergingTransform( BaseOfCompositeTrf ):
    def __init__(self, inDic):
        BaseOfCompositeTrf.__init__(self,inputDic=inDic,ProdSysKeys=ListOfDefaultPositionalKeys,
                                    name="Merging_trf",
                                    authors = [ Author('David Cote','david.cote@cern.ch') ] ,
                                    help = __doc__ )

        self.dicMergeHITS=self.AddNewSubStep("mergeHITS",self.runMergeHITS)
        self.dicMergeRDO=self.AddNewSubStep("mergeRDO",self.runMergeRDO)
        self.dicMergePool=self.AddNewSubStep("merge",self.runMergePool)
        self.dicAODToDPD=self.AddNewSubStep("a2d",self.runAODtoDPD)
        self.dicAODToTAG=self.AddNewSubStep("a2t",self.runAODtoTAG)

        #Internal sub-step configuration (i.e. fill dictionaries)
        self.ConfigureInternalSubSteps()
        self.ConfigureInputOutputs()
        self.CommonSpecialFinalConfig()
        return

    def runJob(self):
        #initialization...
        allOK=True
        mergeOK=False
        tagAodOK=False
        report=JobReport()
        report.setProducer('MergingTrf')

        #First setup DBRelease if requested (do not propagate this arg to sub-transforms)
        if self.inDic.has_key('DBRelease'):
            self.getArgument('DBRelease').install()

        ######################
        # HIT Merging
        if(allOK and self.SubStepIsExecuted('mergeHITS')):
            dic=self.dicMergeHITS.copy()            
            print "MergeHITS dic:",dic
            mHITS = MergeHITSJobTransform(dic,lastInChain=False)
            mHITS.setParent(self)
            mHITS.setJobReportOptions('Summary')
            reportMergeHITS = mHITS.exeArgDict( dic )
            report.addReport( reportMergeHITS )
            mergeHITS_OK = ( reportMergeHITS.exitCode() == 0 )
            print "mergeHITS_OK is ",mergeHITS_OK
            allOK = (allOK and mergeHITS_OK)
        else:
            print "Skipping MergeHIT step..."

        ######################
        # RDO Merging
        if(allOK and self.SubStepIsExecuted('mergeRDO')):
            dic=self.dicMergeRDO.copy()
            print "MergeRDO dic:",dic
            mRDO = MergeRDOJobTransform(dic,lastInChain=False)
            mRDO.setParent(self)
            mRDO.setJobReportOptions('Summary')
            reportMergeRDO = mRDO.exeArgDict(dic)
            report.addReport( reportMergeRDO )
            mergeRDO_OK = ( reportMergeRDO.exitCode() == 0 )
            print "mergeRDO_OK is ", mergeRDO_OK
            allOK = (allOK and mergeRDO_OK)
        else:
            print "Skipping MergeRDO step..."

        ######################
        # Pool Merging
        # N.B. Option to use hybrid POOL merging has now moved to MergePool_trf itself 
        if(allOK and self.SubStepIsExecuted('merge')):
            dic=self.dicMergePool.copy()
            print "MergePool dic:",dic
            mPool = MergePoolJobTransform(dic,lastInChain=False)
            mPool.setParent(self)
            mPool.setJobReportOptions('Summary')
            reportMergePool = mPool.exeArgDict( dic )
            report.addReport( reportMergePool )
            mergeOK = ( reportMergePool.exitCode() == 0 )
            print "mergeOK is ",mergeOK
            allOK = (allOK and mergeOK)
        else:
            print "Skipping MergePool step..."

        ######################
        # (merged) AOD->DPD
        if(allOK and self.SubStepIsExecuted('a2d')):
            dic=self.dicAODToDPD.copy()            
            print "AODtoDPD dic:",dic
            dpdAOD = AODtoDPDJobTransform(dic,lastInChain=False)
            dpdAOD._lastInChain=False
            dpdAOD.setParent(self)
            dpdAOD.setJobReportOptions('Summary')
            reportDpdAod = dpdAOD.exeArgDict( dic )
            report.addReport( reportDpdAod )
            dpdAodOK = ( reportDpdAod.exitCode() == 0 )
            print "dpdAodOK is ",dpdAodOK
            allOK = (allOK and dpdAodOK)
        else:
            print "Skipping AOD->DPD step..."

        ######################
        # (merged) AOD->TAG
        if(allOK and self.SubStepIsExecuted('a2t')):
            dic=self.dicAODToTAG.copy()            
            print "AODtoTAG dic:",dic
            tagAOD = AODtoTAGJobTransform(dic,lastInChain=False)
            tagAOD.setParent(self)
            tagAOD.setJobReportOptions('Summary')
            reportTagAod = tagAOD.exeArgDict( dic )
            report.addReport( reportTagAod )
            tagAodOK = ( reportTagAod.exitCode() == 0 )
            print "tagAodOK is ",tagAodOK
            allOK = (allOK and tagAodOK)
        else:
            print "Skipping AOD->TAG step..."

        return report


    def ConfigureInternalSubSteps(self):
        self.ConfigureCommonInternalSubSteps()
        return

    def ConfigureInputOutputs(self):
        #Check for potential misconfigurations...        
        if self.inDic.has_key('inputHitsFile') and ( self.inDic.has_key('outputESDFile') or self.inDic.has_key('outputTAGFile') or self.inDic.has_key('outputAODFile') ):
            raise RuntimeError("input HITS and output ESD/AOD/TAG is not supported.")
        # Disable this - logfile merging is now optional for Hits merging step
#        if self.inDic.has_key('inputHitsFile') and not self.inDic.has_key('inputLogsFile'):
#            raise RuntimeError("inputLogsFile must be provided to the HIT merging step.")
        if self.inDic.has_key('inputAODFile') and self.inDic.has_key('outputESDFile'):
            raise RuntimeError("input AOD and output ESD is not supported.")
        if self.inDic.has_key('inputESDFile') and self.inDic.has_key('outputTAGFile'):
            raise RuntimeError("input ESD and output TAG is not supported.")

        #AOD->MergeAOD->TAG
        if self.runMergeAOD() and self.hasOutput(self.dicAODToTAG):
            #We overwrite dicAODToTAG['inputAODFile']. That's OK in this case.
            self.dicAODToTAG['inputAODFile']=self.dicMergePool['outputAODFile']
            print "AOD->MergeAOD->TAG requested. Input of TAG will be: '%s'."%self.dicAODToTAG['inputAODFile']
        return

    def GetFirstSubStep(self):
        if self.runMergeHITS():
            return ["mergeHITS"] #["dicMergeHIT"]
        if self.runMergePool():
            return ["merge"] #["dicMergePool"]
        elif self.runAODtoTAG():
            return ["a2t"] #["dicAODToTAG"]
        return None

    def runAODtoTAG(self):
        return (self.hasInput(self.dicAODToTAG) and self.dicAODToTAG.has_key('outputTAGFile'))

    def runAODtoDPD(self):
        return (self.hasInput(self.dicAODToDPD) and self.hasOutput(self.dicAODToDPD))

    def runMergePool(self):
        return (self.runMergeAOD() or self.runMergeESD())

    def runMergeHITS(self):
        return (self.hasInput(self.dicMergeHITS) and self.dicMergeHITS.has_key('outputHitsFile'))

    def runMergeAOD(self):
        return (self.hasInput(self.dicMergePool) and self.dicMergePool.has_key('outputAODFile'))
        
    def runMergeESD(self):
        return (self.hasInput(self.dicMergePool) and self.dicMergePool.has_key('outputESDFile'))
        
    def runMergeRDO(self):
        return (self.hasInput(self.dicMergeRDO) and self.dicMergeRDO.has_key('outputRDOFile'))


################# Python executable
if __name__ == '__main__':
    #Special preparation for command-line
    import sys
    from PATJobTransforms.ArgDicTools import BuildDicFromCommandLine
    inDic=BuildDicFromCommandLine(sys.argv)
    #Construct and execute the transform
    trf = MergingTransform(inDic)          
    sys.exit(trf.exeArgDict(inDic).exitCode())
