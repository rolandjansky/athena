#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ ="""Any reconstruction step: RAW->RAW (run HLT) and/or RAW->ESD and/or ESD->AOD and/or ESD->DPD and/or AOD->DPD and/or ESD->TAG.
The job is self-configuring from the input file name and from its input arguments in general.
It requires a dictionary in its constructor and needs to be executed with exeArgDict().
Once the configuration is decided, the whole job stops if any intermediate job fails."""

from PyJobTransformsCore.trf import Author,JobReport
from RecJobTransforms.RAWtoESD_trf import RAWtoESDJobTransform
from RecJobTransforms.RDOtoBS_trf import RDOtoBSJobTransform
from PATJobTransforms.MergePool_trf import MergePoolJobTransform
from RecJobTransforms.ESDtoAOD_trf import ESDtoAODJobTransform
from PATJobTransforms.ESDtoDPD_trf import ESDtoDPDJobTransform
from PATJobTransforms.AODtoDPD_trf import AODtoDPDJobTransform
from PATJobTransforms.AODtoTAG_trf import AODtoTAGJobTransform
from PATJobTransforms.DQHistogramMerge_trf import DQHistogramMergeJobTransform
from PATJobTransforms.RAWtoRAWHLT_trf import RAWtoRAWHLTJobTransform
from PATJobTransforms.ArgDicTools import DefaultConfigFromSysArgv
from PATJobTransforms.BaseOfCompositeTrf import BaseOfCompositeTrf


#Note ListOfDefaultPositionalKeys needs to be maintained by hand, to support 'grep' from ProdSys
ListOfDefaultPositionalKeys=['inputBSFile','inputRDOFile','inputEVNTFile','inputESDFile','inputAODFile','maxEvents','skipEvents','RunNumber','autoConfiguration','preInclude','postInclude','preExec','postExec','topOptions','DBRelease','conditionsTag','geometryVersion','beamType','AMITag','asetup','outputBSFile','outputESDFile','outputCBNTFile','outputTXT_FTKIPFile','outputNTUP_FTKIPFile','outputNTUP_TRKVALIDFile','outputNTUP_MUONCALIBFile','outputTAG_COMMFile','outputTAGFile','outputAODFile','triggerConfig','outputHISTFile','outputNTUP_MUFASTFile','outputNTUP_TRIGFile','outputHIST_TRIGEXPERTFile','outputDESDM_EGAMMAFile','outputDESD_PHOJETFile','outputDESD_SGLELFile','outputDESDM_MUONFile','outputDESD_SGLMUFile','outputDESDM_CALJETFile','outputDESD_CALJETFile','outputDESDM_TRACKFile','outputDESD_METFile','outputDESDM_METFile','outputDESD_MBIASFile','outputDESD_PIXELCOMMFile','outputDESD_IDCOMMFile','outputDESD_CALOCOMMFile','outputDESD_TILECOMMFile','outputDESD_MUONCOMMFile','outputDESD_HIRAREFile','outputDESDM_HIRAREFile','outputDESDM_RPVLLFile','outputDESDM_RPVLLCCFile','outputD2ESD_JPSIUPSMMFile','outputD2ESD_ZMMFile','outputD2ESD_WENUFile','outputD2ESD_WMUNUFile','outputDAODM_SGLEMFile','outputDAODM_SGLMUFile','outputDAODM_SGLPHFile','outputDAODM_SGLTAUFile','outputDAOD_2EMFile','outputDAOD_2PHFile','outputDAOD_2MUFile','outputDAOD_EMMUFile','outputDAOD_EMTAUFile','outputDAOD_MUTAUFile','outputDAOD_EMMETFile','outputDAOD_MUMETFile','outputDAOD_JETMETFile','outputDAOD_EMJETFile','outputDAOD_MUJETFile','outputDAOD_TAUJETFile','outputDAOD_JPSIMUMUFile','outputDAOD_UPSIMUMUFile','outputD2AODM_TOPELFile','outputD2AODM_TOPMUFile','outputD2AODM_TOPQCDELFile','outputD2AODM_TOPQCDMUFile','outputD2AODM_TOPQCDJ1File','outputD2AODM_TOPQCDJ2File','outputD2AODM_TOPJETFile','outputD2AODM_TOPEJETFile','outputD2AODM_TOPFJETFile','outputDAOD_HSG1File','outputNTUP_TOPFile','outputNTUP_TOPVALFile','outputNTUP_SGTOPFile','outputNTUP_TOPELFile','outputNTUP_TOPMUFile','outputNTUP_TOPJETFile','outputNTUP_TOPEJETFile','outputDAOD_ONIAMUMUFile','outputDESD_COLLCANDFile','outputDAOD_EGLOOSEFile','outputDAOD_PHLOOSEFile','outputD2ESD_DIPHOFile','outputDESDM_DIPHOFile','outputD2AOD_DIPHOFile','outputDAOD_ELLOOSE18File','outputD2AOD_ELLOOSE18File','outputDAOD_MUFile','outputDRAW_ZEEFile','outputDRAW_WENUFile','outputDRAW_ZMUMUFile','outputDRAW_WMUNUFile','outputNTUP_BTAGFile','outputNTUP_BTAGD3PDFile','outputNTUP_BTAGSLIMFile','outputNTUP_BTAGEFFFile','outputNTUP_BTAGFULLFile','outputNTUP_EGAMMAFile','outputNTUP_MINBIASFile','outputNTUP_PROMPTPHOTFile','outputNTUP_JETMETFile','outputNTUP_JETMETFULLFile','outputNTUP_JETMETWZFile','outputNTUP_JETMETEMCLFile','outputDESDM_BEAMSPOTFile','outputNTUP_SMWZFile','outputNTUP_SMWZSOFTFile','outputNTUP_TRTFile','outputNTUP_HECNOISEFile','outputNTUP_SUSYFile','outputNTUP_SUSYSKIMFile','outputNTUP_SUSY01LEPFile','outputNTUP_SUSYLLPFile','outputNTUP_SUSYTAGFile','outputNTUP_SUSYRAZORFile','outputNTUP_SUSY23LEPFile','outputNTUP_SUSYLEPTAUFile','outputNTUP_SUSYBOOSTFile','outputNTUP_SUSY34LEPFile','outputNTUP_SMEWFile','outputDAOD_SUSYJETSFile','outputDAOD_SUSYMUONSFile','outputDAOD_SUSYEGAMMAFile','outputNTUP_HIFile','outputD2ESDM_TRKJETFile','--ignoreerrors','--athenaopts','--omitvalidation','--extraignorefilter','tmpESD','tmpAOD','extraParameter','outputDAOD_RNDMFile','outputNTUP_MCPFile','outputDAOD_2LHSG2File','outputDAOD_4LHSG2File','outputDAOD_2L2QHSG2File','outputDAOD_HSG2File','outputNTUP_SCTFile','outputNTUP_HIGHMULTFile','outputNTUP_1LHSG2File','outputNTUP_2LHSG2File','outputNTUP_4LHSG2File','outputNTUP_2L2QHSG2File','outputNTUP_HSG2File','outputNTUP_TPHSG2File','outputNTUP_ENHBIASFile','outputNTUP_L1CALOFile','outputNTUP_L1CALOPROBFile','outputNTUP_FASTMONFile','outputNTUP_PHOTONFile','outputNTUP_BKGDFile','outputNTUP_SMWENUFile','outputNTUP_SMWENUJJFile','outputNTUP_SMZEEFile','outputNTUP_SMDYEEFile','outputNTUP_SMBKGEFile','outputNTUP_SMWMUNUFile','outputNTUP_SMWMUNUJJFile','outputNTUP_SMZMUMUFile','outputNTUP_SMDYMUMUFile','outputNTUP_SMBKGMUFile','outputNTUP_SMDILEPFile','outputNTUP_TRIGMUFile','outputNTUP_TRIGBJETFile','outputHIST_PHYSVALMONFile','outputNTUP_TRUTHFile','outputNTUP_SUSYTRUTHFile','outputNTUP_LARNOISEFile','inputTAGFile','outputNTUP_SMQCDFile','outputNTUP_L1TGCFile','outputNTUP_HSG5WHFile','outputNTUP_HSG5ZHLLFile','outputNTUP_HSG5ZHMETFile','outputNTUP_HSG5GAMZFile','outputNTUP_HSG5ZBBFile','outputNTUP_HSG5WHUFile','outputNTUP_HSG5WHQFile','outputNTUP_HSG5ZHLLUFile','outputNTUP_HSG5ZHMETUFile','outputDESD_ZEEFile','outputDESD_WENUFile','outputDESD_ZMUMUFile','outputDESD_WMUNUFile','outputDAOD_ZEEFile','outputDAOD_WENUFile','outputDAOD_ZMUMUFile','outputDAOD_WMUNUFile','outputDAOD_ZEEGAMMAFile','outputDAOD_ZMUMUGAMMAFile','outputNTUP_SMTRILEPFile','outputNTUP_SMLIGHTFile','outputNTUP_TAUSMALLFile','outputNTUP_TAUMEDIUMFile','outputNTUP_TAUFULLFile','outputNTUP_TAUFile','outputNTUP_TOPBOOSTFile','outputNTUP_ZPRIMEEEFile','outputNTUP_ZPRIMEMMFile','outputNTUP_MUONFile','outputDESD_ONIAMUMUHIFile','outputNTUP_IDVTXFile','outputNTUP_IDVTXLUMIFile','outputNTUP_WPRIMEENFile','outputNTUP_WPRIMEMNFile','outputNTUP_COMMONFile','outputTXT_JIVEXMLTGZFile','outputNTUP_EMBLLDNFile','outputNTUP_EMBLLIMFile','outputNTUP_EMBLLUPFile','outputNTUP_EMBLHDNFile','outputNTUP_EMBLHIMFile','outputNTUP_EMBLHUPFile','outputNTUP_EMBHHDNFile','outputNTUP_EMBHHIMFile','outputNTUP_EMBHHUPFile','outputDESDM_ALLCELLSFile','trigFilterList','outputDAOD_JPSIHSG2File']

#Default values of input/output types, for standard tests now set in PATJobTransforms/python/DefaultInputs.py


#The transform class itself
class FlexibleRecoTransform( BaseOfCompositeTrf ):
    def __init__(self, inDic):
        BaseOfCompositeTrf.__init__(self,inputDic=inDic,ProdSysKeys=ListOfDefaultPositionalKeys,
                                    name="Reco_trf",                                    
                                    authors = [ Author('David Cote et al.','david.cote@cern.ch') ] ,
                                    help = __doc__ )

        #Construct dictionaries
        self.dicRAWToRAWHLT=self.AddNewSubStep("r2r",self.runRAWtoRAW)
        self.dicRAWToESD=self.AddNewSubStep("r2e",self.runRAWtoESD)
        self.dicRDOToBS=self.AddNewSubStep("r2b",self.runRDOtoBS)
        self.dicESDToESD=self.AddNewSubStep("e2e",self.runESDtoESD)
        self.dicESDToAOD=self.AddNewSubStep("e2a",self.runESDtoAOD)
        self.dicESDToDPD=self.AddNewSubStep("e2d",self.runESDtoDPD)
        self.dicAODToDPD=self.AddNewSubStep("a2d",self.runAODtoDPD)
        self.dicAODToTAG=self.AddNewSubStep("a2t",self.runAODtoTAG)
        
        #Add --drop-and-reload unless related options are specified:
        athena_config_related_opts = ['--config-only','--drop-and-reload','--drop-configuration','--keep-configuration']
        athena_current_opts=self.inDic.get('--athenaopts','').split()
        conflict_opts = set(athena_config_related_opts).intersection(set(athena_current_opts))
        if not conflict_opts:
            print "Appending '--drop-and-reload' to --athenaopts"
            self.inDic['--athenaopts']=' '.join(athena_current_opts+['--drop-and-reload'])
        else:
            print "User has specified '%s' to '--athenaopts', so not appending '--drop-and-reload'"%(' '.join(conflict_opts))
            
        #Internal sub-step configuration (i.e. fill dictionaries)
        self.ConfigureInternalSubSteps()
        self.ConfigureInputOutputs()
        self.CommonSpecialFinalConfig()
        return

    def runJob(self):
        #initialization...
        allOK=True
        esdOK=False
        aodOK=False
        dpdEsdOK=False
        dpdAodOK=False
        report=JobReport()
        report.setProducer('RecoTrf')

        #First setup DBRelease if requested (do not propagate this arg to sub-transforms)
        if self.inDic.has_key('DBRelease'):
            self.getArgument('DBRelease').install()

        print self.dicRAWToESD

        ########################
        # RDO->BS
        if (allOK and self.SubStepIsExecuted('r2b')):
            dic=self.dicRDOToBS.copy()
            print "RDOtoBS dic:",dic
            BS = RDOtoBSJobTransform(dic,lastInChain=False)
            BS._lastInChain=False
            BS.setParent(self)
            BS.setJobReportOptions('Summary')
            reportBS = BS.exeArgDict( dic )
            report.addReport( reportBS )
            bsOK = ( reportBS.exitCode() == 0 )
            print "bsOK is ",bsOK
            allOK = (allOK and bsOK)
        else:
            print "Skipping RDO->BS step..."



        ########################
        # RAW->RAW, run HLT
        if (allOK and self.SubStepIsExecuted('r2r')):
            print ">>> Raw to raw transform, re-running HLT"
            print self.dicRAWToRAWHLT
            dic=self.dicRAWToRAWHLT.copy()
            print "RAWToRAWHLT dic:",dic
            RAW = RAWtoRAWHLTJobTransform(dic,lastInChain=False)
            RAW._lastInChain=False
            RAW.setParent(self)
            RAW.setJobReportOptions('Summary')
            reportRAW = RAW.exeArgDict( dic )
            report.addReport( reportRAW )
            rawOK = ( reportRAW.exitCode() == 0 )
            print "rawOK is ",rawOK
            allOK = (allOK and rawOK)
        else:
            print "Skipping RAW->RAW runHLT step..."


        ########################
        # RAW->ESD
        if (allOK and self.SubStepIsExecuted('r2e')):
            dic=self.dicRAWToESD.copy()
            print "RAWtoESD dic:",dic
            ESD = RAWtoESDJobTransform(dic,lastInChain=False)
            ESD._lastInChain=False
            ESD.setParent(self)
            ESD.setJobReportOptions('Summary')
            reportESD = ESD.exeArgDict( dic )
            report.addReport( reportESD )
            esdOK = ( reportESD.exitCode() == 0 )
            print "esdOK is ",esdOK
            allOK = (allOK and esdOK)
        else:
            print "Skipping RAW->ESD step..."

        ########################
        # ESD->ESD, moved to ESD->MergedESD, if user does _not_ specify doESD in preExec
        if(allOK and self.SubStepIsExecuted('e2e')):
            doMerge=True
            dic=self.dicESDToESD.copy()
            ESD = MergePoolJobTransform(dic,lastInChain=False)
            ESD._lastInChain=False
            ESD.setParent(self)
            ESD.setJobReportOptions('Summary')
            reportESD = ESD.exeArgDict( dic )
            report.addReport( reportESD )
            esdOK = ( reportESD.exitCode() == 0 )
            print "esdOK is ",esdOK
            allOK = (allOK and esdOK)
        else:
            print "Skipping ESD->ESD step..."

        ########################
        # ESD->AOD
        if(allOK and self.SubStepIsExecuted('e2a')):
            dic=self.dicESDToAOD.copy()
            print "ESDtoAOD dic:",dic
            AOD = ESDtoAODJobTransform(dic,lastInChain=False)
            AOD._lastInChain=False
            AOD.setParent(self)
            AOD.setJobReportOptions('Summary')
            reportAOD = AOD.exeArgDict( dic )
            report.addReport( reportAOD )
            aodOK = ( reportAOD.exitCode() == 0 )
            print "aodOK is ",aodOK
            allOK = (allOK and aodOK)
        else:
            print "Skipping ESD->AOD step..."

        ######################
        # ESD->DPD
        if(allOK and self.SubStepIsExecuted('e2d')):
            dic=self.dicESDToDPD.copy()            
            print "ESDtoDPD dic:",dic
            dpdESD = ESDtoDPDJobTransform(dic,lastInChain=False)        
            dpdESD._lastInChain=False
            dpdESD.setParent(self)
            dpdESD.setJobReportOptions('Summary')            
            reportDpdEsd = dpdESD.exeArgDict( dic )
            report.addReport( reportDpdEsd )
            dpdEsdOK = ( reportDpdEsd.exitCode() == 0 )
            print "dpdEsdOK is ",dpdEsdOK
            allOK = (allOK and dpdEsdOK)
        else:
            print "Skipping ESD->DPD step..."
            
        ######################
        # AOD->DPD
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
        # AOD->TAG
        if(allOK and self.SubStepIsExecuted('a2t')):
            dic=self.dicAODToTAG.copy()            
            print "AODtoTAG dic:",dic
            tagAOD = AODtoTAGJobTransform(dic,lastInChain=False)
            tagAOD._lastInChain=False
            tagAOD.setParent(self)
            tagAOD.setJobReportOptions('Summary')
            reportTagAod = tagAOD.exeArgDict( dic )
            report.addReport( reportTagAod )
            tagAodOK = ( reportTagAod.exitCode() == 0 )
            print "tagAodOK is ",tagAodOK
            allOK = (allOK and tagAodOK)
        else:
            print "Skipping AOD->TAG step..."

        ##########################
        # Monitoring file merging
        if(allOK and self.inDic.has_key('outputHISTFile')):
            dic={}
            dic['inputFile']=[]
            if self.runRAWtoESD() and self.dicRAWToESD.has_key('outputDQMonitorFile'): dic['inputFile'].append(self.dicRAWToESD['outputDQMonitorFile'])
            if self.runESDtoAOD() and self.dicESDToAOD.has_key('outputDQMonitorFile'): dic['inputFile'].append(self.dicESDToAOD['outputDQMonitorFile'])
            dic['outputHISTFile']=self.inDic['outputHISTFile']
            print "DQHistogramMerg dic:",dic
            DQMerge = DQHistogramMergeJobTransform(dic,lastInChain=False)
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


    def ConfigureInternalSubSteps(self):
        self.ConfigureCommonInternalSubSteps()

        #Final hack to separate RDO->BS->ESD and RDO->ESD
        if self.runRDOtoBS() and self.dicRAWToESD.has_key('inputRDOFile'):
            self.dicRAWToESD.pop('inputRDOFile')

        #Final hack for Monitoring
        if self.inDic.has_key("outputHISTFile"):
            self.dicRAWToESD['outputDQMonitorFile']="Monitor.root"
            self.dicESDToAOD['outputDQMonitorFile']="MonitorESD.root"
        return

    def ConfigureInputOutputs(self):
        #Check for potential misconfigurations...        
        if self.inDic.has_key('inputAODFile') and self.inDic.has_key('outputAODFile'):
            raise RuntimeError("input AND output AOD is not supported.")
        if self.inDic.has_key("outputHISTFile") and self.inDic.has_key("outputDQMonitorFile"):
            raise RuntimeError("outputHISTFile *and* outputDQMonitorFile is not supported. Please use one or the other.")

        #--------------------------
        # Daisy-chaining input->output

        #RAW->RAW->ESD
        if self.runRAWtoRAW() and self.dicRAWToRAWHLT.has_key('outputBSFile') and self.hasOutput(self.dicRAWToESD):
            if not self.hasInput(self.dicRAWToESD):
                self.dicRAWToESD['inputBSFile']=self.dicRAWToRAWHLT['outputBSFile']
            else:
                msg="""Illegal configuration. Confusion about RAW->ESD input.
                To run RAW->RAW->ESD, please use inputBSFile_r2r='%s'""",self.dicRAWToRAWHLT['outputBSFile']
                raise RuntimeError(msg)

        #RDO->BS->ESD
        if self.runRDOtoBS() and self.hasOutput(self.dicRAWToESD):
            if not self.hasInput(self.dicRAWToESD):
                self.dicRAWToESD['inputBSFile']=self.dicRDOToBS['outputBSFile']
            else:
                msg="""Illegal configuration. Confusion about RAW->ESD input.
                To run RDO->BS->ESD, please use inputRDOFile outputBSFile outputESDFile"""
                raise RuntimeError(msg)

        #RAW->ESD->AOD+DPD
        outESD=None
        if self.dicRAWToESD.has_key('outputESDFile'): 
            outESD=self.dicRAWToESD['outputESDFile']
        elif self.dicRAWToESD.has_key('tmpESD'): 
            outESD=self.dicRAWToESD['tmpESD']
        elif self.hasInput(self.dicRAWToESD) and (self.hasOutput(self.dicESDToDPD) or self.hasOutput(self.dicESDToAOD)):
            # Look for a configuration where input BS/RDO is used, and the final outputs
            # require ESD as an intermediate step.
            print 'Found input for RAWtoESD, but no output - adding tmpESD to argument dictionary'
            outESD = self.dicRAWToESD['tmpESD'] = 'tmp.ESD.pool.root'
        if self.runRAWtoESD() and outESD!=None:
            if self.hasOutput(self.dicESDToAOD):
                if not self.hasInput(self.dicESDToAOD):
                    self.dicESDToAOD['inputESDFile']=outESD
                else:
                    raise RuntimeError("Illegal configuration. Confusion about ESD->AOD input.")
            if self.hasOutput(self.dicESDToDPD):
                if not self.hasInput(self.dicESDToDPD):
                    self.dicESDToDPD['inputESDFile']=outESD
                else:
                    raise RuntimeError("Illegal configuration. Confusion about ESD->DPD input.")
                    
        #ESD->ESD->AOD/DPD
        outESD=None
        if self.dicESDToESD.has_key('outputESDFile'): outESD=self.dicESDToESD['outputESDFile']
        elif self.dicESDToESD.has_key('tmpESD'): outESD=self.dicESDToESD['tmpESD']
        if self.runESDtoESD() and outESD!=None:        
            if self.hasOutput(self.dicESDToAOD):
                if self.hasInput(self.dicESDToAOD):
                    print "WARNING oldESD->newESD configuration: newESD will be used as input for ESD->AOD"
                self.dicESDToAOD['inputESDFile']=outESD

            if self.hasOutput(self.dicESDToDPD):
                if self.hasInput(self.dicESDToDPD):
                    print "WARNING oldESD->newESD configuration: newESD will be used as input for ESD->DPD"
                self.dicESDToDPD['inputESDFile']=outESD
                    
        #ESD->AOD->DPD/TAG
        outAOD=None
        if self.dicESDToAOD.has_key('outputAODFile'): outAOD=self.dicESDToAOD['outputAODFile']
        elif self.dicESDToAOD.has_key('tmpAOD'): outAOD=self.dicESDToAOD['tmpAOD']
        if self.runESDtoAOD() and outAOD!=None:
            if self.hasOutput(self.dicAODToDPD):
                if not self.hasInput(self.dicAODToDPD):
                    self.dicAODToDPD['inputAODFile']=outAOD
                else:
                    raise RuntimeError("Illegal configuration. Confusion about AOD->DPD input.")

            if self.hasOutput(self.dicAODToTAG):
                if not self.hasInput(self.dicAODToTAG):
                    self.dicAODToTAG['inputAODFile']=outAOD
                else:
                    raise RuntimeError("Illegal configuration. Confusion about AOD->TAG input.")

        return

    def GetFirstSubStep(self):
        if self.runRAWtoRAW():
            return ["r2r"] #["dicRAWToRAWHLT"]
        elif self.runRAWtoESD():
            return ["r2e"] #["dicRAWToESD"]
        elif self.runRDOtoBS():
            return ["r2b"] #["dicRDOToBS"]          
        elif self.runESDtoESD():
            return ["e2e"] #["dicESDToESD"]                                       
        elif self.runESDtoAOD() or self.runESDtoDPD():
            return ["e2a","e2d"] #["dicESDToAOD","dicESDToDPD"]
        elif self.runAODtoDPD() or self.runAODtoTAG():
            return ["a2d","a2t"] #["dicAODToDPD","dicAODToTAG"]
        return None

    def runRAWtoRAW(self):
        return (self.hasInput(self.dicRAWToRAWHLT) and self.dicRAWToRAWHLT.has_key('outputBSFile'))

    def runRAWtoESD(self):
        return (self.hasInput(self.dicRAWToESD) and self.hasOutput(self.dicRAWToESD))

    def runRDOtoBS(self):
        return (self.dicRDOToBS.has_key('inputRDOFile') and self.dicRDOToBS.has_key('outputBSFile'))
    
    def runESDtoESD(self):
        return (self.hasInput(self.dicESDToESD) and self.hasOutput(self.dicESDToESD))

    def runESDtoAOD(self):
        return (self.hasInput(self.dicESDToAOD) and self.hasOutput(self.dicESDToAOD))

    def runESDtoDPD(self):
        return (self.hasInput(self.dicESDToDPD) and self.hasOutput(self.dicESDToDPD))

    def runAODtoDPD(self):
        return (self.hasInput(self.dicAODToDPD) and self.hasOutput(self.dicAODToDPD))

    def runAODtoTAG(self):
        return (self.hasInput(self.dicAODToTAG) and self.dicAODToTAG.has_key('outputTAGFile'))


    
###############
if __name__ == '__main__':
    #Special preparation for command-line
    import sys
    from PATJobTransforms.ArgDicTools import BuildDicFromCommandLine
    inDic=BuildDicFromCommandLine(sys.argv)
    #Construct and execute the transform
    trf = FlexibleRecoTransform(inDic)
    trf._lastInChain=True
    #trf.Print()
    sys.exit(trf.exeArgDict(inDic).exitCode())

