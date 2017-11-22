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

from MuonStandaloneFlags import muonStandaloneFlags

from MuonCnvExample.MuonCalibFlags import mdtCalibFlags

import copy

logMuon.info("Importing %s", __name__)

## @brief Switch on VP1 event display. Only used in Muon Standalone.
#
# This flag should really be in AthenaCommon or RecExConfig
class doVP1(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


## Run the integrated muon reconstruction algorithm
class doStandalone(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

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

## Run cluster segment finding
class doTGCClusterSegmentFinding(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

## Run cluster segment finding
class doRPCClusterSegmentFinding(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

## Run clusterization 
class prdToxAOD(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

## Add RPC RDO to xAOD
class rpcRawToxAOD(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

## Run MS vertex (arXiv:1311.7070)
class doMSVertex(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


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


## @brief Fit MDT segments using a variable t0.
#
# Used for cosmics and single beam to compensate for large errors on the trigger time.
# Enabling this will improve the resolution of the MDT segments.
class doSegmentT0Fit(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
   
## turn on error tuning to account for misalignments
class enableErrorTuning(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class useLooseErrorTuning(JobProperty):
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

## Use sTGCs in reconstruction
class dosTGCs(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False # Off by default until it can be autoconfigured

    def _do_action(self):
        muonRecFlags.sync_DetFlags("sTGC")

    def _undo_action(self):
        muonRecFlags.sync_DetFlags("sTGC")

## Use MMs in reconstruction
class doMicromegas(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False # Off by default until it can be autoconfigured

    def _do_action(self):
        muonRecFlags.sync_DetFlags("Micromegas")

    def _undo_action(self):
        muonRecFlags.sync_DetFlags("Micromegas")

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

# @brief run truth-matching on tracks to evaluate tracking performance
#
# It will MuonTrackPerformanceAlg
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

# @brief Write out Muon RDOs into a file (i.e. ESDs)
class writeRDOs(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

# @brief Write out Muon MDT RDOs into a file (i.e. ESDs)
class writeMdtRDOs(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

# @brief Write out Muon CSC RDOs into a file (i.e. ESDs)
class writeCscRDOs(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

# @brief Write out Muon RPC RDOs into a file (i.e. ESDs)
class writeRpcRDOs(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

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



## The flags to steer muon reconstruction
class MuonRec(JobPropertyContainer):
    ##set defaults of the flags depending on type of input (MC, data, cosmics etc.)
    def setDefaults(self):
        global globalflags

        from MuonRecUtils import setJobPropertyDefault as setDefault

        # as long as rec.Commissioning is alive, sync the default to it
        # in case of BS->RDO, RDO->RDO, RDO->BS, BS->BS: don't run RIO (i.e RDO->PRD)
        setDefault(self.makePRDs, rec.readRDO() and not rec.doWriteRDO() and not rec.doWriteBS())
        setDefault(self.doStandalone,True)
        setDefault(self.doDigitization,False)
        setDefault(self.doCalib,False)
        setDefault(self.applyResilience,athenaCommonFlags.AllowIgnoreConfigError())
        setDefault(self.doSegmentsOnly,False)
        setDefault(self.doMDTs,True)
        setDefault(self.doRPCs,True)
        setDefault(self.doTGCs,True)
        setDefault(self.doCSCs,True)
        setDefault(self.doMSVertex,True)
        setDefault(self.useWireSagCorrections,False)
        setDefault(self.enableErrorTuning,True)
        setDefault(self.useLooseErrorTuning,False)
        setDefault(self.useAlignmentCorrections,DetFlags.detdescr.Muon_on() and rec.doMuon())
        setDefault(self.writeSDOs, rec.doWriteESD() and globalflags.DataSource != 'data')
        setDefault(self.useTGCPriorNextBC,True)

        if beamFlags.beamType == 'cosmics' or beamFlags.beamType == 'singlebeam':
            setDefault(self.doSegmentT0Fit,True)
        else:
            setDefault(self.doSegmentT0Fit,False)
            
        setDefault(self.doPrdSelect,False)

        # Default for MuonCalibration ntuple
        setDefault(self.calibMuonStandalone, muonRecFlags.doStandalone() or rec.readESD() )
        try:
            from MuonCnvExample.MuonCalibFlags import muonCalibFlags
        except ImportError:
            from AthenaCommon.Resilience import treatException
            treatException("Could not load MuonCalibFlags. Switching off calibration ntuple")
            self.doCalib = False
            self.doCalibNtuple = False
        else:
               
            doTracks = self.calibMuonStandalone()
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
                if rec.doTrigger: setDefault(self.calibNtupleTrigger,True)
                else: setDefault(self.calibNtupleTrigger,False)
            elif muonCalibFlags.Mode == 'ntuple' or (muonCalibFlags.Mode == 'trackNtuple' and not doTracks):
                setDefault(self.doCalibNtuple,self.doCalib())
                setDefault(self.calibNtupleSegments,True)
                setDefault(self.calibNtupleTracks,False)
                if rec.doTrigger: setDefault(self.calibNtupleTrigger,True)
                else: setDefault(self.calibNtupleTrigger,False)
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


## @brief Enable or disable the Muon Reconstruction
class Enabled(SummaryJobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

    # list of flags controlled by Enable flag
    _properties = [ muonRecFlags.makePRDs,
                    muonRecFlags.doStandalone,
                    muonRecFlags.doDigitization,
                    muonRecFlags.doMSVertex ]

muonRecFlags.add_JobProperty(Enabled)

muonRecFlags.setDefaults()

