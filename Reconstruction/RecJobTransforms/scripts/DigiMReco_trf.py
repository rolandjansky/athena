#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ ="""Specialist transform to run digitisation, followed by multiple 
reconstruction steps, each with configurable trigger menus. Individual ESDs
are merged then AOD/TAG generated."""

import shutil, ast, sys, os

from PyJobTransformsCore.trf import Author,JobReport
from SimuJobTransforms.Digi_trf import HITtoRDOJobTransform
from RecJobTransforms.RDOtoRDOTrigger_trf import RDOtoRDOTriggerJobTransform
from RecJobTransforms.RAWtoESD_trf import RAWtoESDJobTransform
from RecJobTransforms.ESDtoAOD_trf import ESDtoAODJobTransform
from PATJobTransforms.ESDtoDPD_trf import ESDtoDPDJobTransform
from PATJobTransforms.MergePool_trf import MergePoolJobTransform
from PATJobTransforms.ArgDicTools import DefaultConfigFromSysArgv
from PATJobTransforms.BaseOfCompositeTrf import BaseOfCompositeTrf
from PATJobTransforms.DQHistogramMerge_trf import DQHistogramMergeJobTransform
from PyJobTransformsCore.trferr import TransformDefinitionError


# Note0: ListOfDefaultPositionalKeys needs to be maintained by hand, to support 'grep' from ProdSys (arg!)
# Note1: Adding preInclude_h2r/r2e/e2a (and co.) to the ListOfDefaultPositionalKeys is a particular hack for prodsys 
#  to know that these are valid - it's not used at all by the trf, which does its usual mapping as key_substep 
ListOfDefaultPositionalKeys=['inputHitsFile','outputRDOFile','outputESDFile','outputAODFile','outputTAGFile','outputHISTFile','outputDESD_MBIASFile','outputDESDM_EGAMMAFile','outputDESDM_MUONFile','outputDESD_CALJETFile','outputDESDM_TRACKFile','outputDESD_SGLELFile','outputDESD_SGLMUFile','outputDESDM_METFile','outputDESD_PHOJETFile','outputDESDM_RPVLLFile','outputNTUP_TOPEJETFile', 'outputNTUP_HSG5GAMHFile', 'outputNTUP_JETMETWZFile', 'outputNTUP_TRIGBJETFile', 'outputNTUP_BTAGD3PDFile', 'outputNTUP_SUSYFile', 'outputNTUP_SMTRILEPFile', 'outputNTUP_HIFile', 'outputNTUP_JETMETFULLFile', 'outputNTUP_PHYSICSFile', 'outputNTUP_SUSY01LEPFile', 'outputNTUP_PHOTONFile', 'outputNTUP_SMZEEFile', 'outputNTUP_PROMPTPHOTFile', 'outputNTUP_SMQCDFile', 'outputNTUP_FASTMONFile', 'outputNTUP_SMDYMUMUFile', 'outputNTUP_SMBKGEFile', 'outputNTUP_HSG5ZHMETFile', 'outputNTUP_JETMETEMCLFile', 'outputNTUP_HSG5ZHLLFile', 'outputNTUP_L1TGCFile', 'outputNTUP_SUSYLLPFile', 'outputNTUP_SMWMUNUFile', 'outputNTUP_BKGDFile', 'outputNTUP_SMWMUNUJJFile', 'outputNTUP_TOPBOOSTFile', 'outputNTUP_SMDILEPFile', 'outputNTUP_SMWENUFile', 'outputNTUP_HSG5ZBBFile', 'outputNTUP_SMWZFile', 'outputNTUP_HSG2File', 'outputNTUP_ENHBIASFile', 'outputNTUP_MCPFile', 'outputNTUP_HSG5WHFile', 'outputNTUP_SMWZSOFTFile', 'outputNTUP_TRIGMUFile', 'outputNTUP_SUSYSKIMFile', 'outputNTUP_TOPELFile', 'outputNTUP_TOPFile', 'outputNTUP_JETMETFile', 'outputNTUP_SMDYEEFile', 'outputNTUP_HIGHMULTFile', 'outputNTUP_HSG5GAMZFile', 'outputNTUP_HECNOISEFile', 'outputNTUP_TRIGFile', 'outputNTUP_SMWENUJJFile', 'outputNTUP_SCTFile', 'outputNTUP_WZFile', 'outputNTUP_LARNOISEFile', 'outputNTUP_L1CALOPROBFile', 'outputNTUP_TRTFile', 'outputNTUP_SMBKGMUFile', 'outputNTUP_SMZMUMUFile', 'outputNTUP_BTAGFile', 'outputNTUP_TOPMUFile', 'outputNTUP_ZPRIMEEEFile', 'outputNTUP_EGAMMAFile', 'outputNTUP_TAUSMALLFile', 'outputNTUP_MINBIASFile', 'outputNTUP_L1CALOFile', 'outputNTUP_TAUMEDIUMFile', 'outputNTUP_MUONFile', 'outputNTUP_TOPJETFile', 'outputNTUP_SUSY23LEPFile','outputNTUP_TRKVALIDFile','doRDOTrigger','tmpRDO','tmpESD','maxEvents','skipEvents', 'triggerConfigByRun','preInclude_h2r','postInclude_h2r','preExec_h2r','postExec_h2r','preInclude_r2e','postInclude_r2e','preExec_r2e','postExec_r2e','preInclude_e2a','postInclude_e2a','preExec_e2a','postExec_e2a','preInclude_rdo2rdotrig','postInclude_rdo2rdotrig','preExec_rdo2rdotrig','postExec_rdo2rdotrig','RunNumber','autoConfiguration','DBRelease','conditionsTag','beamType','extraParameter','geometryVersion','--ignoreerrors','--athenaopts','--omitvalidation','--extraignorefilter','numberOfLowPtMinBias', 'numberOfHighPtMinBias', 'numberOfBeamHalo', 'numberOfBeamGas', 'numberOfCavernBkg','LowPtMinbiasHitsFile','bunchSpacing', 'pileupInitialBunch', 'pileupFinalBunch', 'doAllNoise', 'AddCaloDigi', 'DataRunNumber','LucidOn', 'ALFAOn', 'ZDCOn', 'jobNumber','digiSeedOffset1','digiSeedOffset2','HighPtMinbiasHitsFile','beamGasHitsFile','beamHaloHitsFile','cavernHitsFile','samplingFractionDbTag','IgnoreConfigError','testPileUpConfig','digiRndmSvc','outputTXT_FTKIPFile','outputDESDM_ALLCELLSFile']

# Default values of input/output types, for standard tests now set in PATJobTransforms/python/DefaultInputs.py

class DigiMultiTriggerRecoTrf( BaseOfCompositeTrf ):
    def __init__(self, inDic):

        if '-h' in inDic or '--help' in inDic:
            print '\n', sys.argv[0]
            print __doc__
            print "See: https://twiki.cern.ch/twiki/bin/viewauth/Atlas/DigiMReco"
            print "Prodsys understood keys are as follows:"
            print ListOfDefaultPositionalKeys
            sys.exit(0)
        
        BaseOfCompositeTrf.__init__(self,inputDic=inDic,ProdSysKeys=ListOfDefaultPositionalKeys,
                                    name="DigiMultiReco_trf",                                    
                                    authors = [ Author('Graeme A Stewart','graeme.andrew.stewart@cern.ch') ] ,
                                    help = __doc__ )
        
        
        # Remove arguments which are private to this transform
        # This is a bt of a hack, but will be fixed properly in the 
        # new trf version
        self.stripPrivateArguments()

        #Construct dictionaries
        self.dicHitsToRDO=self.AddNewSubStep("h2r", self.runHitstoRDO)
        self.dicRDOToRDOTrigger=self.AddNewSubStep("rdo2rdotrig",self.runRDOtoRDOtrigger)
        self.dicRAWToESD=self.AddNewSubStep("r2e",self.runRAWtoESD)
        self.dicESDMerge=self.AddNewSubStep("mrg",self.runESDMerge)        
        self.dicESDToAOD=self.AddNewSubStep("e2a",self.runESDtoAOD)
        self.dicESDToDPD=self.AddNewSubStep("e2d",self.runESDtoDPD)
        
        # Internal state for r2e sub-steps
        self.esdOutputs = []
        self.lastESDEvent = 0

        # Collect all monitoring from substeps in this list for final merge
        self.monHistOutputs = [] 
        
        # Parse the run or event dependent trigger configuration
        self.parseTriggerConfig()            
        
        #Internal sub-step configuration (i.e. fill dictionaries)
        self.ConfigureInternalSubSteps()
        self.ConfigureInputOutputs()
        self.CommonSpecialFinalConfig()


    def runJob(self):
        #initialization...
        allOK=True
        rdoOK=False
        rdotrigOK=False
        esdOK=False
        mrgOK=False
        aodOK=False
        dpdEsdOK=False
        DQMergeOK=False
        report=JobReport()
        report.setProducer('DigiMultiRecoTrf')
        
        # Useful check
        print sys.argv[0], "DigiMReco_trf substep configuration:"
        for k, v in self.ValidSubStepsDic.iteritems():
            if self.SubStepIsExecuted(k):
                print "%s substep will be executed" % k
            else:
                print "%s substep will be skipped" % k
            print k, ":", v
        
        # First setup DBRelease if requested (do not propagate this arg to sub-transforms)
        if self.inDic.has_key('DBRelease'):
            self.getArgument('DBRelease').install()
            

        ########################
        # HITS->RDO
        if (allOK and self.SubStepIsExecuted('h2r')):
            dic=self.dicHitsToRDO.copy()
            print "HitsToRDO dic:",dic
            RDO = HITtoRDOJobTransform(dic)
            RDO._lastInChain=False
            RDO.setParent(self)
            RDO.setJobReportOptions('Summary')
            reportRDO = RDO.exeArgDict( dic )
            report.addReport( reportRDO )
            rdoOK = ( reportRDO.exitCode() == 0 )
            print "rdoOK is", rdoOK
            allOK = (allOK and rdoOK)
        else:
            print "Skipping Hits->RDO step..."


        ########################
        # RDO->RDOTrigger
        if (allOK and self.SubStepIsExecuted('rdo2rdotrig')):
            dic=self.dicRDOToRDOTrigger.copy()
            print "RDOtoRDOtrigger dic:",dic
            RDOTrig = RDOtoRDOTriggerJobTransform(dic)
            RDOTrig._lastInChain=False
            RDOTrig.setParent(self)
            RDOTrig.setJobReportOptions('Summary')
            reportRDOTrig = RDOTrig.exeArgDict( dic )
            report.addReport( reportRDOTrig )
            rdotrigOK = ( reportRDOTrig.exitCode() == 0 )
            print "rdotrigOK is ",rdotrigOK
            allOK = (allOK and rdotrigOK)
        else:
            print "Skipping RDO->RDOTrigger step..."


        
        ########################
        # RAW->ESD
        if (allOK and self.SubStepIsExecuted('r2e')):
            # eventTrigList is a list of tuples giving (Nevents, trigger)
            eventTrigList = self.getEventTrigList()
            print "Got eventTriggerList:", eventTrigList
            if 'outputESDFile' in self.dicRAWToESD:
                myESD = self.dicRAWToESD['outputESDFile']
            else:
                myESD = self.dicRAWToESD['tmpESD']
            for eventCount, trigger in eventTrigList:
                dic=self.dicRAWToESD.copy()
                print "Orig RAWtoESD dic:",dic                
                ## Now modify the dictionary for this subjob
                # However, if there is only going to be one ESD then
                # we produce it 'directly' and merging can be skipped
                if len(eventTrigList) == 1:
                    self.esdOutputs.append(myESD)
                else:
                    if 'outputESDFile' in dic:
                        del dic['outputESDFile']
                    dic['tmpESD'] = myESD + '.%03d' % (len(self.esdOutputs)+1)
                    self.esdOutputs.append(dic['tmpESD'])
                # Add monitoring if required
                if 'outputDQMonitorFile' in dic:
                    dic['outputDQMonitorFile'] += '.%03d' % (len(self.esdOutputs))
                    self.monHistOutputs.append(dic['outputDQMonitorFile'])
                # Special value of 0 for events disables any run/trigger configuration
                if eventCount != 0:
                    dic['skipEvents'] = self.lastESDEvent
                    dic['maxEvents'] = eventCount
                    dic['triggerConfig'] = trigger
                # Add this here to stop it causing trouble in the digi step
                self.addDropAndReload(dic)
                print "New RAWtoESD dic:", dic

                ESD = RAWtoESDJobTransform(dic)
                ESD._lastInChain=False
                ESD.setParent(self)
                ESD.setJobReportOptions('Summary')
                reportESD = ESD.exeArgDict( dic )
                report.addReport( reportESD )
                esdOK = ( reportESD.exitCode() == 0 )
                print "esdOK for", self.esdOutputs[-1], "is", esdOK
                allOK = (allOK and esdOK)
                if not allOK:
                    # Abort subsequent r2e steps if there is a problem
                    break
                self.lastESDEvent += eventCount
        else:
            print "Skipping RAW->ESD step..."

        ########################
        # ESD->MergeESD
        # Only merge if there is more than 1 ESD file :-)
        if (allOK and self.SubStepIsExecuted('mrg') and len(self.esdOutputs) > 1):
            dic=self.dicESDMerge.copy()
            dic['inputESDFile'] = ",".join(self.esdOutputs)
            print "ESDMerge dic:",dic
            MRG = MergePoolJobTransform(dic)
            MRG._lastInChain=False
            MRG.setParent(self)
            MRG.setJobReportOptions('Summary')
            reportMRG = MRG.exeArgDict( dic )
            if 'outputESDFile' in dic:
                self.addTriggerMetaData(reportMRG, dic['outputESDFile'])
            report.addReport( reportMRG, mode='MERGE' )
            mrgOK = ( reportMRG.exitCode() == 0 )
            print "mrgOK is ", mrgOK
            allOK = (allOK and mrgOK)
        else:
            print "Skipping ESD Merge step..."
            


        ########################
        # ESD->AOD
        if(allOK and self.SubStepIsExecuted('e2a')):
            dic=self.dicESDToAOD.copy()
            # Add this here to stop it causing trouble in the digi step
            self.addDropAndReload(dic)
            if 'outputDQMonitorFile' in dic:
                self.monHistOutputs.append(dic['outputDQMonitorFile'])
            print "ESDtoAOD dic:",dic
            AOD = ESDtoAODJobTransform(dic)
            AOD._lastInChain=False
            AOD.setParent(self)
            AOD.setJobReportOptions('Summary')
            reportAOD = AOD.exeArgDict( dic )
            # Ensure metadata is ok for our outputs
            [ self.addTriggerMetaData(reportAOD, dic[key]) for key in dic.keys() if key.startswith('output')]
            report.addReport( reportAOD, mode='MERGE' )
            aodOK = ( reportAOD.exitCode() == 0 )
            print "aodOK is ",aodOK
            allOK = (allOK and aodOK)
        else:
            print "Skipping ESD->AOD step..."


        ########################
        # ESD->DPD
        if(allOK and self.SubStepIsExecuted('e2d')):
            dic=self.dicESDToDPD.copy()
            # Add this here to stop it causing trouble in the digi step
            self.addDropAndReload(dic)
            print "ESDtoDPD dic:",dic
            DPD = ESDtoDPDJobTransform(dic)
            DPD._lastInChain=False
            DPD.setParent(self)
            DPD.setJobReportOptions('Summary')
            reportDPD = DPD.exeArgDict( dic )
            # Ensure metadata is ok for our outputs
            [ self.addTriggerMetaData(reportDPD, dic[key]) for key in dic.keys() if key.startswith('output')]
            report.addReport( reportDPD, mode='MERGE' )
            dpdEsdOK = ( reportDPD.exitCode() == 0 )
            print "dpdEsdOK is ",dpdEsdOK
            allOK = (allOK and dpdEsdOK)
        else:
            print "Skipping ESD->DPD step..."
            
            
        ##########################
        # Monitoring file merging
        if(allOK and self.inDic.has_key('outputHISTFile')):
            dic={}
            dic['inputFile']=self.monHistOutputs
            dic['outputHISTFile']=self.inDic['outputHISTFile']
            print "DQHistogramMerg dic:",dic
            DQMerge = DQHistogramMergeJobTransform(dic)
            DQMerge._lastInChain=False
            DQMerge.setParent(self)
            DQMerge.forceSingleProcess()
            DQMerge.setJobReportOptions('Summary')
            reportDQMerge = DQMerge.exeArgDict( dic )
            report.addReport( reportDQMerge )
            DQMergeOK = ( reportDQMerge.exitCode() == 0 )
            print "DQMergeOK is ",DQMergeOK
            allOK = (allOK and DQMergeOK)
        else:
            print "Skipping monitoring file merging step..."


        return report
    
    
    def addTriggerMetaData(self, jobReport, outputFileName):
        '''Add special trigger configuration metadata to FileInfo objects in the job report'''
        for f in jobReport.outputFiles():
            if f.filename() == outputFileName and self.triggerConfType == 'run':
                f.addMetaData({'triggerConfig' : self.privateDic['triggerConfigByRun']})
                break
        
    
    def stripPrivateArguments(self):
        '''These are private arguments to this transform, only meaningful 
         for this module. Keep them out of the trf framework for now
         (but probably put them in medium term - but they are more complex than current var types, being
         string representations of dictionaries or lists)'''
        self.privateDic = {} 
        for privateKey in ('triggerConfigByRun', 'triggerConfigByEvent'):
            if privateKey in self.inDic:
                self.privateDic[privateKey] = self.inDic[privateKey]
                del self.inDic[privateKey]

    
    def parseTriggerConfig(self):
        '''Look at input dictionary and decide which trigger configuration we have'''
        if 'triggerConfigByRun' in self.privateDic and 'triggerConfigByEvent' in self.privateDic:
            raise RuntimeError('You cannot specify both triggerConfigByRun and triggerConfigByEvent - pick one only.')
        if 'triggerConfigByRun' in self.privateDic:
            self.triggerConfType = 'run'
            self.triggerConf = self.extractTrigConf(self.privateDic['triggerConfigByRun'], dict)
        elif 'triggerConfigByEvent' in self.privateDic:
            self.triggerConfType = 'event'
            self.triggerConf = self.extractTrigConf(self.privateDic['triggerConfigByEvent'], list)
        else:
            # No trigger switches then...
            self.triggerConfType = None
            self.triggerConf = []


    def extractTrigConf(self, triggerString, seqType):
        '''Parse triggerConfigByRun or triggerConfigByEvent to get the runNumber/trigger or event/trigger mapping'''
        try:
            triggerConfig = ast.literal_eval(triggerString)
            if not isinstance(triggerConfig, seqType):
                raise RuntimeError('''Trying to extract trigger configuration from "%s" did not give the correct python object (is %s, should be %s)''' % (triggerString, type(triggerString), seqType))
            for i in triggerConfig:
                if seqType == dict:
                    k = i
                    v = triggerConfig[i]
                else:
                    k = i[0]
                    v = i[1]
                if not (isinstance(k, int) and k > 0):
                    raise RuntimeError('Trigger configuration key %s is not a valid number' % k)
                if not (isinstance(v, str)):
                    raise RuntimeError('Trigger configuration value %s is not a trigger string' % v)
        except (ValueError, SyntaxError), x:
            raise RuntimeError('"%s" did not parse as a valid python object and caused an exception: %s' % (triggerString, x))
        return triggerConfig
    
                
    def getEventTrigList(self, fake=False):
        '''Get list of event numbers with corresponding trigger setup'''
        if fake:
            return [(1, 'MCRECO:MC_pp_v2_tight_mc_prescale'), (1, 'MCRECO:MC_pp_v3_tight_mc_prescale')]
        if self.triggerConfType == 'run':
            # Need to import the event/runNumber lists
            # This is generated by the h2r step - if that step is not run
            # then this module needs to be present for the correct
            # configuration to be found.
            try:
                sys.path.append(os.getcwd())
                from RunDMCTriggerRunsInfo import listOfRunsEvents
            except ImportError, x:
                raise RuntimeError('Could not find RunDMCTriggerRunsInfo to get event/runNumber list: %s' % x)
            # Loop over the RDO run/event list and fill in the correct trigger
            # Assume that the runnumber:event list is available somehow as self.RunEventStructure
            result = []
            for runEventEntry in listOfRunsEvents:
                try:
                    if ( len(result) > 0 ) and ( self.triggerConf[runEventEntry['run']] == result[-1][1] ):
                        # Same trigger configuration - so just extend the event span
                        result[-1][0] += runEventEntry['evts']
                    else:
                        result.append([runEventEntry['evts'], self.triggerConf[runEventEntry['run']]])
                except KeyError:
                    raise RuntimeError('No trigger configuration for run number %d was found' % runEventEntry['run'])
            return result
        if self.triggerConfType == 'event':
            # This is easy - just use the event/triggermap we have
            return self.triggerConf
        # No trigger conf found - set the special event value of 0, which processes the whole
        # file with no special setup
        return [(0,'')]


    def ConfigureInternalSubSteps(self):
        ## Treat special arguments here
        # For us inputESDFile has to replicate the multiple file outputs of the r2e stages
        # and these are used as inputs to the merge step.
        if 'inputESDFile' in self.inDic:
            self.esdOutputs = self.inDic['inputESDFile'].split(',')
            
        # Repeat the Reco_trf hack for monitoring histograms 
        if self.inDic.has_key("outputHISTFile"):
            self.dicRAWToESD['outputDQMonitorFile']="MonitorESD.root"
            self.dicESDToAOD['outputDQMonitorFile']="MonitorAOD.root"
            
        self.ConfigureCommonInternalSubSteps()
                

    def ConfigureInputOutputs(self):
        '''Daisy-chaining input->output'''

        # First sanity check: we can only start from one of HITS, RDO or ESD
        # Having more than one input type makes no sense
        inputs = set(('inputHitsFile', 'inputRDOFile', 'inputESDFile')) & set(self.inDic.keys())
        if len(inputs) > 1:
            raise TransformDefinitionError, ('This transform can only take one primary input file. You cannot specify %s at the same time.' % 
                                             ' and '.join(inputs))
        if len(inputs) == 0:
            raise TransformDefinitionError, 'One input file (HITS, RDO or ESD) must be specified.'

        ## Now check that outputs are defined
        # If necessary generate tmp filenames for RDO and ESD
        # HITS->RDO?
        if self.hasInput(self.dicHitsToRDO):
            outputs = set(self.dicHitsToRDO.keys()) & set(('tmpRDO', 'outputRDOFile'))
            if len(outputs) > 1:
                raise TransformDefinitionError, 'Only one output from Hits to RDO can be used - illegal to have both %s' % ' and '.join(outputs)
            if self.inDic.get('doRDOTrigger', 'false').lower() == 'true':
                # Different logic if we do the RDO -> RDOTrigger substep 
                if len(outputs) == 1:
                    finalRDOtype = outputs.pop()
                    self.dicHitsToRDO[finalRDOtype] = self.inDic[finalRDOtype]
                    self.dicRDOToRDOTrigger['inputRDOFile'] = self.dicHitsToRDO[finalRDOtype]
                    self.dicRDOToRDOTrigger['tmpRDO'] = self.inDic[finalRDOtype] + '.trigger'
                    self.dicRAWToESD['inputRDOFile'] = self.dicRDOToRDOTrigger['tmpRDO']
                else:
                    print "Autosetting tmpRDO file"
                    self.dicHitsToRDO['tmpRDO'] = self.dicRDOToRDOTrigger['inputRDOFile'] ='tmp.RDO.pool.root'
                    self.dicRDOToRDOTrigger['tmpRDO'] = self.dicRAWToESD['inputRDOFile'] = 'tmp.RDO.pool.root.trigger'
            else:
                if len(outputs) == 1:
                    self.dicRAWToESD['inputRDOFile'] = self.dicHitsToRDO[outputs.pop()]
                else:
                    print "Autosetting tmpRDO file"
                    self.dicHitsToRDO['tmpRDO'] = self.dicRAWToESD['inputRDOFile'] ='tmp.RDO.pool.root'

        # RDO->ESD?
        if self.hasInput(self.dicRAWToESD):
            if self.inDic.get('doRDOTrigger', 'false').lower() == 'true' and self.runHitstoRDO() == False:
                self.dicRDOToRDOTrigger['tmpRDO'] = self.inDic['inputRDOFile'] + '.trigger'
                self.dicRAWToESD['inputRDOFile'] = self.dicRDOToRDOTrigger['tmpRDO']
            outputs = set(self.dicRAWToESD.keys()) & set(('tmpESD', 'outputESDFile'))
            if len(outputs) > 1:
                raise TransformDefinitionError, 'Only one output from RDO to ESD can be used - illegal to have both %s' % ' and '.join(outputs)
            if len(outputs) == 1:
                self.dicESDMerge['outputESDFile'] = self.dicESDToAOD['inputESDFile'] = self.dicESDToDPD['inputESDFile'] = self.dicRAWToESD[outputs.pop()]
            else:
                print "Autosetting tmpESD file"
                self.dicRAWToESD['tmpESD'] = self.dicESDMerge['outputESDFile'] = self.dicESDToAOD['inputESDFile'] = self.dicESDToDPD['inputESDFile'] = 'tmp.ESD.pool.root'


    def GetFirstSubStep(self):
        # This transform only has two real starting points - HITS or RDO
        # but support different entry points for debugging 
        if self.runHitstoRDO():
            return ["h2r"]
        elif self.runRDOtoRDOtrigger():
            return ['rdo2rdotrig']
        elif self.runRAWtoESD():
            return ["r2e"]
        elif self.runESDMerge():
            return ["mrg"]
        return None

    def runHitstoRDO(self):
        return (self.hasInput(self.dicHitsToRDO) and self.hasOutput(self.dicHitsToRDO))
    
    def runRDOtoRDOtrigger(self):
        return (self.hasInput(self.dicRDOToRDOTrigger) and self.hasOutput(self.dicRDOToRDOTrigger))

    def runRAWtoESD(self):
        return (self.hasInput(self.dicRAWToESD) and self.hasOutput(self.dicRAWToESD))

    def runESDMerge(self):
        # Always say we do this step, but at actual run time check if we have more than 1 ESD
        return True

    def runESDtoAOD(self):
        return (self.hasInput(self.dicESDToAOD) and self.hasOutput(self.dicESDToAOD))
    
    def runESDtoDPD(self):
        return (self.hasInput(self.dicESDToDPD) and self.hasOutput(self.dicESDToDPD))
    
    def addDropAndReload(self, dic):
        '''Add --drop-and-reload to the athenaopts in the dictionary, as long as that's possible'''
        athena_config_related_opts = ['--config-only','--drop-and-reload','--drop-configuration','--keep-configuration']
        athena_current_opts=dic.get('--athenaopts','').split()
        conflict_opts = set(athena_config_related_opts).intersection(set(athena_current_opts))
        if not conflict_opts:
            print "Appending '--drop-and-reload' to --athenaopts"
            dic['--athenaopts']=' '.join(athena_current_opts+['--drop-and-reload'])
        else:
            print "User has specified '%s' to '--athenaopts', so not appending '--drop-and-reload'"%(' '.join(conflict_opts))

    
###############
if __name__ == '__main__':
    #Special preparation for command-line
    import sys
    from PATJobTransforms.ArgDicTools import BuildDicFromCommandLine
    inDic=BuildDicFromCommandLine(sys.argv)
    trf = DigiMultiTriggerRecoTrf(inDic)
    trf._lastInChain=True
    sys.exit(trf.exeArgDict(inDic).exitCode())

