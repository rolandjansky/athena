# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#=======================================================================
# File:   RecExConfig/python/RecFlags.py
#=======================================================================
""" Reconstruction specific flags and job properties.

"""
#
#
__author__  = 'D. Rousseau, M. Gallas'
__version__="$Revision: 1.49 $"
__doc__="Reconstruction specific flags . "

from AthenaCommon.Constants import *

#=======================================================================
#
#  In here I set some of the jobProperties from AthenaCommonFlags by
# importing the AthenaCommonFlags sub-container and changing
# some defaults
#from AthenaCommon.AthenaCommonFlags import jobproperties
#jobproperties.AthenaCommonFlags.BSRDOInput=\
#                        ["./dc2_mixing_all_phys_0003621_file100002.data"]
#jobproperties.AthenaCommonFlags.PoolRDOInput=\
#                                ["LFN:top_CSC-01-02-00_RDO_extract.pool"]

# this has nothing to do here !
## # In here I set some of the Global flags
## from AthenaCommon.GlobalFlags import jobproperties
## jobproperties.Global.DetGeo='atlas'
## jobproperties.Global.Luminosity ='zero'

#=======================================================================
# imports
#=======================================================================
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

from RecExConfig.RecoFunctions import AddValidItemToList,RemoveValidItemFromList

class readTAG(JobProperty):
    """ Switch to True if read TAG (in addition to one of RDO ESD AOD)
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
#
class TAGFromRDO(JobProperty):
    """ Switch to True if TAG made directly from RDO. Only active if  readTAG==True
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
#
class doTagRawSummary(JobProperty):
    """ Switch to True if RawSummary for Tag making needs to be written out
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
#
class doDPD(JobProperty):
    """ If True computes DPD objects and writes them
    """
    statusOn            = True
    allowedTypes        = ['bool']
    StoredValue         = True
    passThroughMode     = False
    ESDDESDEventTagging = False
#
class DPDMakerScripts(JobProperty):
    """ Setting DPDMakerScripts+=['MyPackage/MyDPDOptions.py'] will execute MyDPDOptions.py if rec.doDPD is True
    """
    statusOn=True
    allowedTypes=['list']
    StoredValue=[]
    def append(self,item):
        if self.is_locked():
            self._log.info('The JobProperty %s is blocked' % self.__name__)
        else:
            AddValidItemToList(item,self.StoredValue)
        return
    def remove(self,item):
        if self.is_locked():
            self._log.info('The JobProperty %s is blocked' % self.__name__)
        else:
            RemoveValidItemFromList(item,self.StoredValue)
        return
#
class AutoConfiguration(JobProperty):
    """ Optional configuration from input file info
    """
    statusOn=True
    allowedTypes=['list']
    StoredValue=[]
    def append(self,item):
        if self.is_locked():
            self._log.info('The JobProperty %s is blocked' % self.__name__)
        else:
            AddValidItemToList(item,self.StoredValue)
        return
    def remove(self,item):
        if self.is_locked():
            self._log.info('The JobProperty %s is blocked' % self.__name__)
        else:
            RemoveValidItemFromList(item,self.StoredValue)
        return
#
class AMITag(JobProperty):
    """ AMI tag set by production, propagated to svcMgr.TagInfoMgr.ExtraTagValuePairs
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue=""
#
class LoadGeometry(JobProperty):
    """ If True loads ATLAS detector geometry and magnetic field
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
class doAOD(JobProperty):
    """ If True computes AOD objects (without necessary writing them)
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
class doAODCaloCells(JobProperty):
    """ If True build AOD cell container
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
class doESD(JobProperty):
    """ If True run ESD making objects (without necessary writing them). (note that this flag was called AllAlgs in 12.0.X)
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

#
class doAODall(JobProperty):
    """ If True writes all ID track particles if available
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
#
class doCBNT(JobProperty):
    """ If True output combined ntuple
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class doPyDump(JobProperty):
    """ If True output ascii dump
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class doMonitoring(JobProperty):
    """ If True do monitoring
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#

class doFastPhysMonitoring(JobProperty):
    """ If True do Fast Physics Monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#

class doCheckDictionary(JobProperty):
    """ If True check dictionary
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class doCheckJOT(JobProperty):
    """ If True check multiple includes and property modifications. Not tested on 13.0.X.
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
class CBNTAthenaAware(JobProperty):
    """ If True use CBNT AthenaAware. Mandatory since 13.X.0.
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
class doDetailedAuditor(JobProperty):
    """ If True monitor alg/tools and services memory and CPU time
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class doFastMemoryAuditor(JobProperty):
    """ If True monitor memory allocations per algorithm and report if not freed, no stracktraces (memory leaks)
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class doMemoryAuditor(JobProperty):
    """ If True monitor memory allocations per algorithm and report if not freed, with stracktraces (memory leaks)
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class doSGAuditor(JobProperty):
    """ If True monitor data flow, see https://twiki.cern.ch/twiki/bin/view/Atlas/DataFlowMonitoring )
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class doTriggerFilter(JobProperty):
    """ Switch for TriggerFilter (apply trigger filter in RAW->ESD)"""
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class triggerFilterList(JobProperty):
    """ Switch for TriggerFilterAlg (apply trigger filter in RAW->ESD)"""
    statusOn=True
    allowedTypes=['str']
    StoredValue=""
#
class OutputSuffix (JobProperty):
    """ common suffix for auxilliary output files
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue=""
#
class OutputFileNameForRecoStep (JobProperty):
    """ common file name base for auxilliary output files
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue=""
#
class doPerfMon(JobProperty):
    """ If True monitor software performance, using doFastMon=True by default.
    See: https://twiki.cern.ch/twiki/bin/view/Atlas/PerfMonComps
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
class doDetailedPerfMon(JobProperty):
    """ If True monitor software performance, with doFastMon=False.
    See https://twiki.cern.ch/twiki/bin/view/Atlas/PerfMonComps
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class doSemiDetailedPerfMon(JobProperty):
    """ If True monitor software performance, with doFastMon=False.
    See https://twiki.cern.ch/twiki/bin/view/Atlas/PerfMonSD
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

#
class doRecoTiming(JobProperty):
    """ If True store information about CPU and Memory usage in output file
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

#
class doDumpMC(JobProperty):
    """ If True dumps mc truth in log file
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class doEdmMonitor(JobProperty):
    """ If True dumps mc truth in log file
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

#
class doDumpPoolInputContent(JobProperty):
    """ If True dumps pool input object list
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class doDumpProperties(JobProperty):
    """ If True dumps all algs properties
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class doDumpTDS(JobProperty):
    """ If True dumps storegate transient detector store
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class doDumpTES(JobProperty):
    """ If True dumps storegate transient event store
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class oldFlagCompatibility(JobProperty):
    """ If True old flags are available
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class oldFlagLandMine(JobProperty):
    """ If True reusing old flags will cause failure
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class oldFlagTopSteering(JobProperty):
    """ If True old flags are used to steer reconstruction
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doFloatingPointException(JobProperty):
    """ If True enable Floating Point exception
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class abortOnUncheckedStatusCode(JobProperty):
    """ If True abort on first unchecked status code
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class abortOnErrorMessage(JobProperty):
    """ If True abort on first unchecked status code
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

#
class doHist(JobProperty):
    """ If True output some histograms . Not tested on 13.0.X.
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
class doJiveXML(JobProperty):
    """ If True output xml file for atlantis
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class doPersint(JobProperty):
    """ If True output ascii file for persint
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class doVP1(JobProperty):
    """ If True output VP1 output
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class doNameAuditor(JobProperty):
    """ If True one line print out per alg when entering
    initialize, beginRun, execute, finalise
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class doRestrictedESD(JobProperty):
    """ If True writes restricted ESD  (without inner detector prep raw data)
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class doShowSizeStatistics(JobProperty):
    """ If True print pool object size stat of input file
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class doTimeLimit(JobProperty):
    """ If True stops execution when allocated time elapsed
    (useful only on lsf at cern)
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class doTruth(JobProperty):
    """ If True use truth information
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#    def _do_action(self):
#        if jobproperties.Global.__dict__.get('InputFormat')()=='bytestream':
#            raise ValueError,jobproperties._log.info('RecExCommon_flags::doTruth :\
#                    can not be set to true because Global.InputFormat= bytestream  ')
#
class doWriteAOD(JobProperty):
    """ If True writes out AOD file
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doWritexAOD(JobProperty):
    """ If True writes out xAOD file
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

#
class doWriteESD(JobProperty):
    """ If True writes out ESD file
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
class doWriteBS(JobProperty):
    """ If True writes out RDO bytestream file
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class doWriteRDO(JobProperty):
    """ If True writes out RDO file
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
    def _do_action(self): # to be exercised and generalised
        jobproperties.Rec.__dict__.get('doESD').set_Value(False)
        jobproperties._log.info('RecExCommon_flags::doWriteRDO : all algorithms are off')
        jobproperties.Rec.__dict__.get('doWriteESD').set_Value(False)
        jobproperties.Rec.__dict__.get('doAOD').set_Value(False)
        jobproperties.Rec.__dict__.get('doWriteAOD').set_Value(False)
        jobproperties.Rec.__dict__.get('doWriteTAG').set_Value(False)
        jobproperties._log.info('RecExCommon_flags::doWriteRDO : all algorithms for ESD, AOD and TAG making are off')
#
class doWriteTAG(JobProperty):
    """ If True writes out TAG file
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
class doWriteTAGCOM(JobProperty):
    """ If True writes out commissioning TAG file
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class PoolTAGCOMOutput(JobProperty):
    """ commissioning TAG file name
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="TAGCOM.root"
#
class doFileMetaData(JobProperty):
    """ If True writes out TAG file
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
class noESDTrigger(JobProperty):
    """ specify that ESD has no trigger info (hack!)"""
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class OutputLevel(JobProperty):
    """ General msg output level VERBOSE DEBUG INFO
        look here mgallas
    """
    statusOn=True
    allowedTypes=['int']
    allowedValues=[ ALL,VERBOSE,DEBUG,INFO,WARNING,ERROR,FATAL ]
    StoredValue=INFO
#
class doDetStatus(JobProperty):
    """ If True, include DetStatusSvc_CondDB.py and optionaly DetStatusSvc_ToFileMetaData.py
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#
class readAOD(JobProperty):
    """ If True reads AOD file
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
    def _do_action(self):
        jobproperties.Rec.__dict__.get('doESD').set_Value(False)
        jobproperties._log.info('RecFlags::readAOD : all algorithms are off')
    # def _undo_action(self):
    #     jobproperties.Rec.__dict__.get('doESD').statusOn=True
#
class readESD(JobProperty):
    """ If True runs on ESD file
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class readRDO(JobProperty):
    """ If true reads RDO
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
#

class RootHistoOutput(JobProperty):
    """ Root Histogram output """
    statusOn=True
    allowedTypes=['str']
    StoredValue="histo.root"
#
class RootNtupleOutput(JobProperty):
    """ Root Ntuple output """
    statusOn=True
    allowedTypes=['str']
    StoredValue="ntuple.root"
#

class RootFastPhysMonOutput(JobProperty):
    """ Root Ntuple (and Histos) for fast PhysMon """
    statusOn=True
    allowedTypes=['str']
    StoredValue="FPMntuples.root"
#

class UserAlgs(JobProperty):
    """ Setting UserAlgs="MyPackage/MyJobOptions.py" will insert
    this joboption at the end of all algorithms definition.
    """
    statusOn=True
    allowedTypes=['list','str']
    StoredValue=[]
#
class UserExecs(JobProperty):
    """ Setting UserExecs=['from GaudiAlg.GaudiAlgConf import EventCounter','topSequence+=EventCounter("EndAlg",Frequency=1)'] will run these lines at the end of alg definition
    """
    statusOn=True
    allowedTypes=['list','str']
    StoredValue=[]
#

class UserFlags(JobProperty):
    """ Change of flags to be done after RecExCommon_flags.py is done (for those flags where set_Value_and_Locked cannot be used) UserFlags=['DetFlags.dcs.all_setOff()']
    """
    statusOn=True
    allowedTypes=['list','str']
    StoredValue=[]
#

class doInDet(JobProperty):
    """ If false switch off everything related to InDet
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doLArg(JobProperty):
    """ If false switch off everything related to LArg
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    def _undo_action(self):
        if not jobproperties.Rec.doTile and jobproperties.Rec.doCalo:
            jobproperties.Rec.doCalo=False



class doTile(JobProperty):
    """ If false switch off everything related to Tile
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    def _undo_action(self):
        # if each calo is off should switch off doCalo
        if not jobproperties.Rec.doLArg and jobproperties.Rec.doCalo:
            jobproperties.Rec.doCalo=False

class doCalo(JobProperty):
    """ If false switch off everything related to Calo
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    def _undo_action(self):
        if jobproperties.Rec.doLArg:
            jobproperties.Rec.doLArg=False
        if jobproperties.Rec.doTile:
            jobproperties.Rec.doTile=False
    def _do_action(self):
        if not jobproperties.Rec.doLArg:
            jobproperties.Rec.doLArg=True
        if not jobproperties.Rec.doTile:
            jobproperties.Rec.doTile=True


class doMuon(JobProperty):
    """ If false switch off everything related to Muon
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    def _undo_action(self):
        if jobproperties.Rec.doMuonCombined:
            jobproperties.Rec.doMuonCombined=False
    def _do_action(self):
        if jobproperties.Rec.doMuonCombined:
            jobproperties.Rec.doMuonCombined=True


class doLucid(JobProperty):
    """ If false switch off everything related to Lucid
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    def _undo_action(self):
        if not jobproperties.Rec.doZdc and not jobproperties.Rec.doAlfa and not jobproperties.Rec.doAFP and not jobproperties.Rec.doFwdRegion and jobproperties.Rec.doForwardDet  :
            jobproperties.Rec.doForwardDet=False


class doZdc(JobProperty):
    """ If false switch off everything related to Zdc
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    def _undo_action(self):
        if not jobproperties.Rec.doLucid and not jobproperties.Rec.doAlfa and not jobproperties.Rec.doAFP and not jobproperties.Rec.doFwdRegion and jobproperties.Rec.doForwardDet  :
            jobproperties.Rec.doForwardDet=False



class doAlfa(JobProperty):
    """ If false switch off everything related to Alfa
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
    def _undo_action(self):
        if not jobproperties.Rec.doLucid and not jobproperties.Rec.doZdc and not jobproperties.Rec.doAFP and not jobproperties.Rec.doFwdRegion and jobproperties.Rec.doForwardDet  :
            jobproperties.Rec.doForwardDet=False


class doAFP(JobProperty):
    """ If false switch off everything related to AFP
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
    def _undo_action(self):
        if not jobproperties.Rec.doLucid and not jobproperties.Rec.doZdc and not jobproperties.Rec.doAlfa and not jobproperties.Rec.doFwdRegion and jobproperties.Rec.doForwardDet  :
            jobproperties.Rec.doForwardDet=False

class doFwdRegion(JobProperty):
    """ If false switch off everything related to AFP
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
    def _undo_action(self):
        if not jobproperties.Rec.doLucid and not jobproperties.Rec.doZdc and not jobproperties.Rec.doAlfa and not jobproperties.Rec.doAFP and jobproperties.Rec.doForwardDet  :
            jobproperties.Rec.doForwardDet=False

class doForwardDet(JobProperty):
    """ If false switch off everything related to forward detectors
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    def _undo_action(self):
        if jobproperties.Rec.doLucid:
            jobproperties.Rec.doLucid=False
        if jobproperties.Rec.doZdc:
            jobproperties.Rec.doZdc=False
        if jobproperties.Rec.doAlfa:
            jobproperties.Rec.doAlfa=False
        if jobproperties.Rec.doAFP:
            jobproperties.Rec.doAFP=False
        if jobproperties.Rec.doFwdRegion:
            jobproperties.Rec.doFwdRegion=False
    def _do_action(self):
        if not jobproperties.Rec.doLucid:
            jobproperties.Rec.doLucid=True
        if not jobproperties.Rec.doZdc:
            jobproperties.Rec.doZdc=True
        if not jobproperties.Rec.doAlfa:
            jobproperties.Rec.doAlfa=True
        if not jobproperties.Rec.doAFP:
            jobproperties.Rec.doAFP=True
        if not jobproperties.Rec.doFwdRegion:
            jobproperties.Rec.doFwdRegion=True


class doJetMissingETTag(JobProperty):
    """ If false switch off everything related to Jet and MissingET
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True


class doEgamma(JobProperty):
    """ If false switch off everything related to Egamma
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doMuonCombined(JobProperty):
    """ If false switch off everything related to Combined Muon
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doTau(JobProperty):
    """ If false switch off everything related to Tau
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doBTagging(JobProperty):
    """If false switch off everything related to Btagging
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doFTK(JobProperty):
    """If false switch off everything related to FTK
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


class doTrigger(JobProperty):
    """ If false switch off everything related to Trigger
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doHeavyIon(JobProperty):
    """ If true configure reconstruction for Heavy Ion
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class doHIP(JobProperty):
    """ If true configure reconstruction for Heavy Ion - proton collisions
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


class doLowPt(JobProperty):
    """ If true use special low pt configuration
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class doMinimalRec(JobProperty):
    """ If true, configure minimal reconstruction
    (i.e. only a few reco algorithms switched on when making ESD)
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class doPhysValMonHists(JobProperty):
    """ Run code for doing PhysValMon plots, to be used to automatically
    produce validation plots for simulation jobs
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class ScopingLevel(JobProperty):
    """ level of complexity of the reco menu
    1 runs everything 4 run the bare minimum 5 very special config
    """
    statusOn=True
    allowedTypes=['int']
    allowedValues=[ 1,2,3,4,5]
    StoredValue=1

class Commissioning(JobProperty):
    """ true if running in commissioning configuration
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class oldRecExCommissionConfig(JobProperty):
    """ true if old commissioning configuration
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class RunNumber(JobProperty):
    """ RunNumber, only if not available from input file """
    statusOn=True
    allowedTypes=['int']
    StoredValue=0
#
class triggerStream(JobProperty):
    """ Input trigger stream """
    statusOn=True
    allowedTypes=['str']
    StoredValue=""
#
class projectName(JobProperty):
    """ Project name """
    statusOn=True
    allowedTypes=['str']
    StoredValue=""
#
class mergingStreamName(JobProperty):
    """Name of output pool stream during merging (mainly to distinguish between ESD/AOD-merging and DPD-merging)."""
    statusOn=True
    allowedTypes=['str']
    StoredValue=""
#
class Production(JobProperty):
    """ Used to set different default values in production or test run. True=Production, False=test. """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class AtlasReleaseVersion(JobProperty):
    """ release version number, none if not available
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="n/a"
#
class inputFileFromAthenaVersion(JobProperty):
    """ Atlas version with which input file was created
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="n/a"
#
class doApplyAODFix(JobProperty):
    """ decision, if AODFix has to be applied to this file
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
#
class LCGCMTVersion(JobProperty):
    """ version of LCGCMT, none if not available
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="n/a"
#
class doAODSelect(JobProperty):
    """ decision, if AODSelect should be running
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doRDOTrigger(JobProperty):
    """ decision, if RDO->RDO with trigger should be running
    """
    statusOn=False
    allowedTypes=['bool']
    StoredValue=False

class doWriteCalibHits(JobProperty):
    """ forward CaloCalibHitContainers to ESD/AOD (if present in RDO)
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class doContainerRemapping(JobProperty):
    """ Enable container name remapping
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doPhysicsValidationAugmentation(JobProperty):
    """ Enable augmentation for physics validation
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class doESDReconstruction(JobProperty):
    """ Enable settings allowing re-reconstruction from ESD
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class doExpressProcessing(JobProperty):
    """ Enable settings for express processing
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class doPersistencyOptimization(JobProperty):
    """ Enable optimization POOL/ROOT configuration
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doCaloRinger(JobProperty):
    """ Enable ring-shaped calorimeter reconstruction
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doDigiTruth(JobProperty):
    """ Switch for TrackParticle cell association  """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

#
# Defines a sub-container for the algorithm switches
#class RecAlgs(JobPropertyContainer):
#    """ Algorithm switches. """

# Defines the container for the reconstruction flags
class Rec(JobPropertyContainer):
    """ The reconstruction flag/job property  container.
    """

# add the reconstruction flags container to the top container
jobproperties.add_Container(Rec)

# add the specific container for algorithms
#jobproperties.Rec.add_Container(RecAlgs)
# importing the alg flags
#jobproperties.Rec.RecAlgs.import_JobProperties('RecExConfig.RecAlgsFlags')

# I want always the following flags in the Rec container
_list_Rec=[TAGFromRDO, doTagRawSummary , \
OutputLevel, RootHistoOutput, RootNtupleOutput, UserAlgs, UserExecs, UserFlags, doDPD, DPDMakerScripts, LoadGeometry, doAOD, doAODCaloCells, doESD,\
doAODall, doCBNT, doPyDump, doMonitoring, doCheckDictionary, doCheckJOT,CBNTAthenaAware,\
doDetailedAuditor, doSGAuditor, doFastMemoryAuditor, doMemoryAuditor, OutputSuffix, OutputFileNameForRecoStep, doPerfMon, doDetailedPerfMon, doSemiDetailedPerfMon, doRecoTiming, doDumpMC, doDumpPoolInputContent, doDumpProperties,\
doDumpTDS, doDumpTES, doFloatingPointException, abortOnUncheckedStatusCode, abortOnErrorMessage, oldFlagCompatibility, oldFlagTopSteering,oldFlagLandMine,\
doEdmMonitor, doHist, doJiveXML, doPersint,doVP1, doNameAuditor, \
doRestrictedESD, doShowSizeStatistics, doTimeLimit, doTruth,\
doWriteAOD, doWritexAOD, doWriteESD, doWriteBS, doWriteRDO, doWriteTAG, doWriteTAGCOM, PoolTAGCOMOutput, readAOD, readTAG, readRDO, \
readESD, doDetStatus, AMITag, AutoConfiguration, RunNumber, triggerStream, projectName, \
doInDet,doLArg,doTile,doCalo,doMuon,doForwardDet,doLucid,doZdc,doAlfa,doAFP,doFwdRegion,doJetMissingETTag,doEgamma,doMuonCombined,doTau,doFTK,doTrigger,doBTagging, Production, doPhysValMonHists, \
doHeavyIon, doHIP, doLowPt, doMinimalRec, noESDTrigger,doFileMetaData,ScopingLevel, Commissioning, oldRecExCommissionConfig, mergingStreamName, LCGCMTVersion, \
AtlasReleaseVersion, inputFileFromAthenaVersion, doApplyAODFix, doAODSelect, doRDOTrigger, doFastPhysMonitoring, RootFastPhysMonOutput,doWriteCalibHits, doTriggerFilter, triggerFilterList, doContainerRemapping, doPhysicsValidationAugmentation, doESDReconstruction, doExpressProcessing, doPersistencyOptimization, doCaloRinger, doDigiTruth]
for j in _list_Rec:
    jobproperties.Rec.add_JobProperty(j)
del _list_Rec

# The short name (carefull do not select same name as shot name as well.
# otherwise problems with pickle)
rec=jobproperties.Rec


#=======================================================================
