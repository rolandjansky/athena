# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file MuonRecFlags.py Flags to steer Muon Standalone Reconstruction

## @namespace python::MuonRecFlags @copydoc MuonRecFlags.py

from AthenaCommon.JobProperties import JobProperty,JobPropertyContainer,jobproperties
from AthenaCommon.Logging import logging
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.BeamFlags import jobproperties
beamFlags = jobproperties.Beam
from RecExConfig.RecFlags import rec
from MuonRecUtils import logMuon,logMuonResil,RecConfigInfo,fillJobPropertyContainer,SummaryJobProperty
# Load and give access to Muonboy and Moore flags (for convenience)
from MuonboyFlags import muonboyFlags
from MooreFlags import mooreFlags
from MuonStandaloneFlags import muonStandaloneFlags

from MuonCnvExample.MuonCalibFlags import mdtCalibFlags

import copy

logMuon.info("Importing %s", __name__)


## Run the Muonboy muon reconstruction algorithm
class doMuonboy(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


## Run the Moore muon reconstruction algorithm
class doMoore(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

## Run the integrated muon reconstruction algorithm
class doStandalone(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

## Run the new third chain configuration
class doNewThirdChain(JobProperty):
     StatusOn=True
     allowedType=['bool']
     StoredValue=False
## Run the new third chain configuration for the NSW
class doNSWNewThirdChain(JobProperty):
     StatusOn=True
     allowedType=['bool']
     StoredValue=False
    
## Run clusterization 
class doCreateClusters(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

## Run clusterization 
class prdToxAOD(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


## @brief OBSOLETE. Use instead: doMoore, doMuonboy
#
#    List of top-level tags (strings) of pre-configured Muon reco configurations
#    to be run. Only recos in this list are run. To switch off a certain reco,
#    remove it from this list, e.g. with the remove() function.
#    Format of the tags:
#    \"[package].[module].[class]\"
#    [class]             : the name of the configuration class
#
#    [module] (optional) : the name of the python module (inside package)
#                 default: equal to <class>
#    [package] (optional): the name of the package that contains the python module
#                 default: MuonRecExample
#    Examples of available tags:
#    \"Muonboy\"
#    \"Moore\" (currently equal to Moore.StandardMoore)
#    \"Moore.StandardMoore\"
#    \"Moore.MoMuMoore\"
#    \"Moore.Csc4dMoore\"
#    \"Moore.MoMu\"
#    Note that one can choose only one among the Moore variants.
class SelectReco(JobProperty):

    statusOn=False
    allowedTypes=['list']
    StoredValue=['Muonboy','Moore']

    def find(self,partOfTag,ignoreStatusOn=False):
        """Check that the <partOfTag> string is part of one of the selected configuration tags.
        It returns the full first matched tag, or an empty string if not found"""
        if not self.statusOn and not ignoreStatusOn: return ''
        for t in self.StoredValue:
            if partOfTag in t: return t
        return ''

    def contains(self,partOfTag,ignoreStatusOn=False):
        """Returns bool that indicated if the <partOfTag> string is part of one of the selected configuration tags."""
        return self.find(partOfTag,ignoreStatusOn) != ''

    def remove(self,partOfTag,ignoreStatusOn=True):
        """Remove all entries that match <partOfTag>"""
        if not self.statusOn and not ignoreStatusOn: return
        oldList = self.StoredValue
        newList = []
        for t in oldList:
            if partOfTag not in t: newList.append(t)
        if newList != oldList:
            self.set_Value(newList)

    def _do_action(self):
        """Load JobPropertyContainers and Modules associated to requested configs,
        and unload all others (given in SelectReco.Containers)"""
        print "muonRecFlags.SelectReco is obsolete. Please use muonRecFlags.doMoore and doMuonboy instead"
        global muonRecFlags
        fullTag = self.find("Muonboy")
        if fullTag:
            muonRecFlags.doMuonboy = True
        else:
            muonRecFlags.doMuonboy = False
        fullTag = self.find("Moore")
        if fullTag:
            muonRecFlags.doMoore = True
        else:
            muonRecFlags.doMoore = False
                

## Re-run muon digitization on-the-fly just before reconstruction. Needs DIGITS as input file.
class doDigitization(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

    def _do_action(self):
        muonRecFlags.sync_DetFlags()

    def _undo_action(self):
        muonRecFlags.sync_DetFlags()
        

class doFastDigitization(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

    def _do_action(self):
        muonRecFlags.sync_DetFlags()

    def _undo_action(self):
        muonRecFlags.sync_DetFlags()
        
class doPseudoTracking(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

    def _do_action(self):
        muonRecFlags.sync_DetFlags()

    def _undo_action(self):
        muonRecFlags.sync_DetFlags()
        

## Determine the MDT calibration
class doCalib(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


## Write Trk D3PDs
class doTrkD3PD(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

## Write calibration ntuple
class doCalibNtuple(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

## Name of the calibration ntuple file to write out
class calibNtupleOutput(JobProperty):
    statusOn = True
    allowedTypes=['str']
    StoredValue='CalibrationNtuple.root'

## Write segments to calibration ntuple
class calibNtupleSegments(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

## Write tracks to calibration ntuple
class calibNtupleTracks(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

## Write trigger info to calibration ntuple
class calibNtupleTrigger(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

## Write raw TGC info to calibration ntuple
class calibNtupleRawTGC(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

## Write moore tracks and segment to calibration ntuple
class calibMoore(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

## Write muonboy tracks and segment to calibration ntuple
class calibMuonboy(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

## Write muonboy tracks and segment to calibration ntuple
class calibMuonStandalone(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

 
## Allow job to continue in case of configuration errors in the Muon part
class applyResilience(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


## @brief Stop reconstruction after segment making.
#
# Typically used when making Calibration Ntuple.
class doSegmentsOnly(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

    def _change_action(self):
        value = self.get_Value()
        logMuon.info("Forwarding muonRecFlags.doSegmentsOnly = %r to muonboyFlags.doSegmentsOnly and mooreFlags.doSegmentsOnly", value )
        muonboyFlags.doSegmentsOnly = value
        mooreFlags.doSegmentsOnly = value
        muonStandaloneFlags.doSegmentsOnly = value

    def _do_action(self):
        self._change_action()

    def _undo_action(self):
        self._change_action()



## @brief Fit MDT segments using a variable t0.
#
# Used for cosmics and single beam to compensate for large errors on the trigger time.
# Enabling this will improve the resolution of the MDT segments.
class doSegmentT0Fit(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

## @brief Force some reconstruction settings to be in collision mode.
#
# Used to test collisions-data mode with cosmics-data.
# Affects MuonRecTools.py and Moore.py.
class forceCollisionsMode(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
                 
## @brief Force some reconstruction settings to be in data mode.
#
# Used to test collisions-data mode with collisions-simulation.
# Affects MuonRecTools.py and Moore.py.
class forceDataMode(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
    

## turn on error tuning to account for misalignments
class enableErrorTuning(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

## Use MDTs in reconstruction
class doMDTs(JobProperty):
    statusOn=True
    allowedTypes=['bool']

    StoredValue=True

    def _do_action(self):
        muonRecFlags.sync_DetFlags("MDT")

    def _undo_action(self):
        muonRecFlags.sync_DetFlags("MDT")


## Use CSCs in reconstruction
class doCSCs(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

    def _do_action(self):
        muonRecFlags.sync_DetFlags("CSC")

    def _undo_action(self):
        muonRecFlags.sync_DetFlags("CSC")
        

## Use RPCs in reconstruction
class doRPCs(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

    def _do_action(self):
        muonRecFlags.sync_DetFlags("RPC")

    def _undo_action(self):
        muonRecFlags.sync_DetFlags("RPC")

## Use TGCs in reconstruction
class doTGCs(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

    def _do_action(self):
        muonRecFlags.sync_DetFlags("TGC")

    def _undo_action(self):
        muonRecFlags.sync_DetFlags("TGC")


## @brief Use TGC measurements from Prior and Next Bunch Crossings.
#
# These measurements are available in the real data since somewhere in 2008.
class useTGCPriorNextBC(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True


## @brief Apply alignment corrections to MuonGeoModel.
#
# The corrections are read from a COOL database
class useAlignmentCorrections(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

## Apply wire sag corrections.
class useWireSagCorrections(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False 

## @brief Cut away some PrepRawData to simulate dead channels
#
# Useful during commissioning to simulate partly working detector
class doPrdSelect(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


## @brief Choose data taking period.
#
# Possible values: Sector13,MU1,MU2,MU3,M3,M4,M5,M6,P1,P2,P3,P4,Reproc08,Reproc09,LHC
# It indicates an old way (up to release 15.3.0) to access the calibration constants from
# special <tags> from the Db on real data only.
# Now we are accessing constants from the tag in the Db associated to a global conditions tag
# for all cases, Data (online or offline) and MC
# The flag  is set by default to "LHC" but it has no effect anymore
#
class dataPeriod(JobProperty):
    statusOn=True
    allowedTypes=['str']
    allowedValues=["MU1","MU2","MU3","Sector13","M3","M4","M5","M6","M7","M8","P1","P2","P3","P4","LHC","Reproc08","Reproc09"]
    StoredValue="LHC"


# @brief run truth-matching on tracks to evaluate tracking performance
#
# It will enable matching for Moore and/or Muonboy depending on flags doMoore and doMuonboy
# It uses MuonTrackPerformanceAlg
class doTrackPerformance(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

# @brief Set the summary level of the TrackPerformance algorithm.
#
# Only active if not doing truth matching.
# Meaning of values:
# 0,1: no summary
# 2: summary per event
class TrackPerfSummaryLevel(JobProperty):
    statusOn=True
    allowedTypes=['int']
    StoredValue=1

# @brief Set the debug level of the TrackPerformance algorithm.
#
# Meaning of values:    
# 1: summary at the end (in finalize)
# 4: summary for event with a problem (in execute)
# 5: summary for all tracks (in execute)
# 6: also print out the hits (in execute)
class TrackPerfDebugLevel(JobProperty):
    statusOn=True
    allowedTypes=['int']
    StoredValue=4


# @brief Write out Muon SDOs into a file (i.e. ESDs)
class writeSDOs(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

# @brief Write out Muon exit layer and filter it
class writeMuonExitLayer(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

# @brief Run the RDO->PRD conversion algorithms
#
# The technologies actually run depends on the flags doMDTS,doCSCs,doRPCs,doTGCs
class makePRDs(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

# @brief Run in commissioning mode (looser cuts)
#
# Replaces rec.Commissioning (which is to be removed). The default is set equal to rec.Commissioning
class Commissioning(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


## The flags to steer muon reconstruction
class MuonRec(JobPropertyContainer):
    ##set defaults of the flags depending on type of input (MC, data, cosmics etc.)
    def setDefaults(self):
        global globalflags,mooreFlags,muonboyFlags
        mooreFlags.setDefaults()
        muonboyFlags.setDefaults()

        from MuonRecUtils import setJobPropertyDefault as setDefault

        # as long as rec.Commissioning is alive, sync the default to it
        setDefault(self.Commissioning,rec.Commissioning())
        # in case of BS->RDO, RDO->RDO, RDO->BS, BS->BS: don't run RIO (i.e RDO->PRD)
        setDefault(self.makePRDs, rec.readRDO() and not rec.doWriteRDO() and not rec.doWriteBS())
        #setDefault(self.doMuonboy,True)
        #setDefault(self.doMoore,True)
        setDefault(self.doStandalone,True)
        setDefault(self.doDigitization,False)
        setDefault(self.doCalib,False)
        setDefault(self.applyResilience,athenaCommonFlags.AllowIgnoreConfigError())
        setDefault(self.doSegmentsOnly,False)
        setDefault(self.doMDTs,True)
        setDefault(self.doRPCs,True)
        setDefault(self.doTGCs,True)
        setDefault(self.doCSCs,True)
        setDefault(self.forceDataMode,self.Commissioning())
        setDefault(self.forceCollisionsMode,False)
        setDefault(self.useWireSagCorrections,False)
        setDefault(self.enableErrorTuning,True)
        setDefault(self.useAlignmentCorrections,DetFlags.detdescr.Muon_on() and rec.doMuon())
        setDefault(self.writeSDOs, rec.doWriteESD() and globalflags.DataSource != 'data')
        if globalflags.DataSource == 'data':
            setDefault(self.useTGCPriorNextBC,True)
#            setDefault(self.doCSCs,False)         
        elif self.forceDataMode():
            if beamFlags.beamType == 'collisions':
                # NB: this is setting a flag from another container!!!
                setDefault(mdtCalibFlags.correctMdtRtForBField,True)
            setDefault(self.useTGCPriorNextBC,False)
#            setDefault(self.doCSCs,False)
            setDefault(self.useAlignmentCorrections,DetFlags.detdescr.Muon_on() and rec.doMuon())
        else:
            setDefault(self.useTGCPriorNextBC,False)
#            setDefault(self.doCSCs,True)
#            setDefault(self.useAlignmentCorrections,False)

        if beamFlags.beamType == 'cosmics' or beamFlags.beamType == 'singlebeam':
            setDefault(self.doSegmentT0Fit,True)
        else:
            setDefault(self.doSegmentT0Fit,False)
            
        setDefault(self.doPrdSelect,False)
        setDefault(self.dataPeriod,"LHC")

        # Default for MuonCalibration ntuple
        setDefault(self.calibMoore,   muonRecFlags.doMoore()   or rec.readESD() )
        setDefault(self.calibMuonboy, muonRecFlags.doMuonboy() or rec.readESD() )
        setDefault(self.calibMuonStandalone, muonRecFlags.doStandalone() or rec.readESD() )
        try:
            from MuonCnvExample.MuonCalibFlags import muonCalibFlags
        except ImportError:
            from AthenaCommon.Resilience import treatException
            treatException("Could not load MuonCalibFlags. Switching off calibration ntuple")
            self.doCalib = False
            self.doCalibNtuple = False
        else:
            # for backwards compat: synchronise with muoncalibFlags
            if not muonCalibFlags.EventTag.statusOn:
                muonCalibFlags.EventTag = 'Moore'

            if muonCalibFlags.EventTag == 'Muonboy':
                logMuon.info("Setting muonRecFlags.calibMoore = False because muonCalibFlags.EventTag == \'Muonboy\'")
                setDefault(self.calibMoore, False)
                
            doTracks = (muonRecFlags.calibMoore()   and not   mooreFlags.doSegmentsOnly()) or \
                       (muonRecFlags.calibMuonboy() and not muonboyFlags.doSegmentsOnly()) or \
		       self.calibMuonStandalone()
            # chose a default
            if not muonCalibFlags.Mode.statusOn:
                if doTracks:
                    muonCalibFlags.Mode = 'trackNtuple'
                else:
                    muonCalibFlags.Mode = 'ntuple'

            if muonCalibFlags.Mode == 'trackNtuple' and doTracks:
                setDefault(self.doCalibNtuple,self.doCalib())
                setDefault(self.calibNtupleSegments,True)
                setDefault(self.calibNtupleTracks,True)
                setDefault(self.calibNtupleTrigger,True)
            elif muonCalibFlags.Mode == 'ntuple' or (muonCalibFlags.Mode == 'trackNtuple' and not doTracks):
                setDefault(self.doCalibNtuple,self.doCalib())
                setDefault(self.calibNtupleSegments,True)
                setDefault(self.calibNtupleTracks,False)
                setDefault(self.calibNtupleTrigger,True)
            else:
                setDefault(self.doCalibNtuple,False)
                setDefault(self.calibNtupleSegments,False)
                setDefault(self.calibNtupleTracks,False)
                setDefault(self.calibNtupleTrigger,False)



    ## Ensure that the flags are synchronised and consistent.
    def syncFlags(self):
        self.sync_DetFlags()

    ## @brief Synchronise Muon DetFlags with MuonRecFlags and RecFlags
    ##
    ## @arg @c technologies is string with comma separated list of technologies to include in the update. Default="MDT,RPC,CSC,TGC"
    def sync_DetFlags(self,technologies="MDT,RPC,CSC,TGC"):
        self.setDefaults()
        global rec
        from AthenaCommon.DetFlags import DetFlags
        # if Muons off, then do nothing
        if not DetFlags.Muon_on() or not rec.doMuon(): return

        flagsOn = []
        flagsOff = [ 'readRIOBS' ] # we don't store RIO (=PRD) on bytestream ???
        # Reading RDO
        if rec.readRDO():
            if muonRecFlags.makePRDs():
                flagsOn.append( 'makeRIO' )
            else:
                flagsOff.append( 'makeRIO' )
            if globalflags.InputFormat == 'pool'      :
                flagsOn.append( 'readRDOPool' )
                flagsOff.append( 'readRDOBS' )
            elif globalflags.InputFormat == 'bytestream':
                flagsOn.append( 'readRDOBS'   )
                flagsOff.append( 'readRDOPool' )
        else: # not reading RDO

            flagsOff.append( 'makeRIO' )

        # Reading ESD
        if rec.readESD():
            flagsOn.append( 'readRIOPool' )
        else:
            flagsOff.append( 'readRIOPool' )

        # Writing RDO Pool
        if rec.doWriteRDO():
            flagsOn.append( 'writeRDOPool' )
        else:
            flagsOff.append( 'writeRDOPool' )

        # Writing RDO bystream
        if rec.doWriteBS() :
            flagsOn.append( 'writeBS' )
        else:
            flagsOff.append( 'writeBS' )

        # Writing ESD
        if rec.doWriteESD():
            flagsOn.append( 'writeRIOPool' )
        else:
            flagsOff.append( 'writeRIOPool' )

        # Digitization on-the-fly
        if self.doDigitization():
            flagsOn.append( 'digitize' )
            try:
                flagsOn.remove( 'readRDOPool' )
            except ValueError: # not in list
                pass
            try:
                flagsOn.remove( 'readRDOBS' )
            except ValueError: # not in list
                pass
            flagsOff.append( 'readRDOPool' )
            flagsOff.append( 'readRDOBS'   )
            # very special case for normal Rome production (inspired by RecExCommon_DetFlags.py)
            if 'readRIOPool' not in flagsOn:
                flagsOn.append( 'readRIOPool' )
                try:
                    flagsOff.remove( 'readRIOPool' )
                except ValueError: # not in list
                    pass
            if not rec.doWriteRDO() and 'makeRIO' not in flagsOn:
                flagsOn.append( 'makeRIO' )
                try:
                    flagsOff.remove( 'makeRIO' )
                except ValueError: #not in list
                    pass
                    
        else:
            flagsOff.append( 'digitize')


        # do sync per technology for selected flags
        MDT_on = self.doMDTs()
        RPC_on = self.doRPCs()
        CSC_on = self.doCSCs()
        TGC_on = self.doTGCs()
        techList = technologies.split(',')
        for f in flagsOn:
            for tech in techList:
                setOn = eval(tech+'_on')
                isOn = eval("DetFlags.%s.%s_on()" % (f,tech))
                if setOn:
                    if not isOn:
                        cmd = "DetFlags.%s.%s_setOn()" % (f,tech)
                        logMuon.info(cmd)
                        exec cmd
                else: # set off
                    if isOn:
                        cmd = "DetFlags.%s.%s_setOff()" % (f,tech)
                        logMuon.info(cmd)
                        exec cmd

        # Turn off Muon flags (row in DetFlags printout)
        for f in flagsOff:
            isOn = eval("DetFlags.%s.Muon_on()" % f)
            if isOn:
                cmd = "DetFlags.%s.Muon_setOff()" % f
                logMuon.info(cmd)
                exec cmd

        # Turn off Technology flags (column in DetFlags printout)  Too drastic???
##         for tech in techList:
##             setOff = not eval(tech+'_on')
##             isOn = eval("DetFlags.%s_on()" % tech)
##             if setOff and isOn:
##                 cmd = "DetFlags.%s_setOff()" % tech
##                 logMuon.info(cmd)
##                 exec cmd
                
    



jobproperties.add_Container(MuonRec)

## shortcut to access flags
muonRecFlags = jobproperties.MuonRec

# fill properties in container
fillJobPropertyContainer(muonRecFlags,__name__)


## @brief Enable or disable the Muon Reconstruction (Moore and Muonboy).
#
# True  : run Moore and/or Muonboy depending on the flags doMoore and doMuonboy
# False : do not run Moore nor Muonboy
class Enabled(SummaryJobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

    # list of flags controlled by Enable flag
    _properties = [ muonRecFlags.makePRDs,
                    muonRecFlags.doMuonboy,
                    muonRecFlags.doMoore,
                    muonRecFlags.doStandalone,
                    muonRecFlags.doDigitization ]

muonRecFlags.add_JobProperty(Enabled)

muonRecFlags.setDefaults()

