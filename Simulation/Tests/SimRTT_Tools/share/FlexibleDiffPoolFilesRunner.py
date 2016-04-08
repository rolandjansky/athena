#!/usr/bin/env python
"""
FlexibleDiffPoolFilesRunner this module contains:
classes:

FlexibleDiffPoolFilesRunner - class to run the RTT diffPoolFiles.py or
                        acmd.py diff-root scripts.  This script should
                        be on $PATH. The RTT sets up the ATLAS
                        environment including $PATH in the job run
                        script. Based on DiffPoolFilesRunner.py
"""
import os
import re

from ExeRunnerBase import ExeRunnerBase
from TestResult import TestResult

class FlexibleDiffPoolFilesRunner(ExeRunnerBase):
    def __init__(self, argDict):
        """
        JobDescriptor - an object carrying RTT framework info.
        filename - name of the pool file to test against a reference pool file.
        refFileName - name of the reference pool file.
        doDetailedChecks - (optional) if True then acmd.py diff-root will be used.
        platformDependent - should the reference file used depend on the platform.
        """

        testFile = argDict['fileName']
        refFile = argDict['refFileName']
        self.doDetailedChecks  = argDict.get('doDetailedChecks', False)
        platformDependent = argDict.get('platformDependent', False)
        optionString =  argDict.get('optionString', '--enforce-leaves BCM_RDO_Container_p0_BCM_RDOs CMMCPHitsCollection_tlp1_CMMCPHits CMMEtSumsCollection_tlp1_CMMEtSums CMMJetHitsCollection_tlp1_CMMJetHits CMMRoI_p1_CMMRoIs CPMHitsCollection_tlp1_CPMHits CPMRoICollection_tlp1_CPMRoIs CPMTowerCollection_tlp1_CPMTowers CTP_RDO_p2_CTP_RDO CaloCalibrationHitContainer_p3_LArCalibrationHitActive CaloCalibrationHitContainer_p3_LArCalibrationHitDeadMaterial CaloCalibrationHitContainer_p3_LArCalibrationHitInactive CscRawDataContainer_p3_CSCRDO InDetRawDataContainer_p1_PixelRDOs InDetRawDataContainer_p1_TRT_RDOs InDetRawDataContainer_p2_PixelRDOs InDetRawDataContainer_p2_TRT_RDOs InDetSimDataCollection_p1_BCM_SDO_Map InDetSimDataCollection_p1_PixelSDO_Map InDetSimDataCollection_p1_SCT_SDO_Map InDetSimDataCollection_p1_TRT_SDO_Map InDetSimDataCollection_p2_BCM_SDO_Map InDetSimDataCollection_p2_PixelSDO_Map InDetSimDataCollection_p2_SCT_SDO_Map InDetSimDataCollection_p2_TRT_SDO_Map JEMEtSumsCollection_tlp1_JEMEtSums JEMHitsCollection_tlp1_JEMHits JEMRoICollection_tlp1_JEMRoIs JetCollection_tlp6_InTimeAntiKt4TruthJets JetCollection_tlp6_OutOfTimeAntiKt4TruthJets JetElementCollection_tlp1_JetElements LArDigitContainer_p1_LArDigitContainer_MC_Thinned LArRawChannelContainer_p4_LArRawChannels LArTTL1Container_LArTTL1EM LArTTL1Container_LArTTL1HAD McEventCollection_p5_TruthEvent MdtCsmContainer_p1_MDTCSM MuCTPI_RDO_p1_MUCTPI_RDO PileUpEventInfo_p4_McEventInfo RoIBResult_p1_RoIBResult RpcPadContainer_p2_RPCPAD SCT_RawDataContainer_p2_SCT_RDOs SCT_RawDataContainer_p3_SCT_RDOs TgcRdoContainer_p2_TGCRDO TileDigitsContainer_p1_TileDigitsFlt TileL2Container_p2_TileL2Cnt TileRawChannelContainer_p1_TileRawChannelCnt TileTTL1Container_p1_TileTTL1Cnt TileTTL1Container_p1_TileTTL1MBTS TrackRecordCollection_p2_MuonEntryLayer TriggerTowerCollection_tlp1_TriggerTowers --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings RecoTimingObj_p1_HITStoRDO_timings')

        cmd = ''
        if platformDependent:
            desc = argDict.get('JobDescriptor', None)
            cmtConfig = 'i686-slc5-gcc43-opt'
            if (desc != None):
                paths = desc.paths
                cmtConfig = paths.cmtConfig
            else:
                cmtConfig = argDict.get('cmtConfig', 'i686-slc5-gcc43-opt')
            refFile = refFile.replace( 'CMTCONFIG', cmtConfig )
        if self.doDetailedChecks:
            cmd = 'acmd.py diff-root %s %s %s' % (refFile, testFile, optionString)
            #self.diffPoolSuccessStr = 'Found [0] different leaves'
        else:
            cmd = 'diffPoolFiles.py -f %s -r %s' % (testFile, refFile)
            #self.diffPoolSuccessStr = '## Comparison : [OK]'

        testIdentifier = argDict['testIdentifierObj']

        outputFile = argDict.get('testOutputFile', False)
        if not outputFile:  # if no value, value = ''
            outputFile = '%s_%s.diffPool' % (os.path.basename(testFile),
                                             os.path.basename(refFile))

        ExeRunnerBase.__init__(self,
                               cmd=cmd,
                               testIdentifier=testIdentifier,
                               outputFile=outputFile)

    def run(self):

        ExeRunnerBase.run(self)

        if (not self.doDetailedChecks) or self.statSum == 0:
            return

        ## Temporary work-around until either TP conversion added for
        ## Muon SDOs or acmd diff-root can be made to ignore them.
        
        reLine = 'Found [0] different leaves'
        comment = 'ORIGINAL RESULT OVERRIDDEN'

        with open(self.outputFile) as ifile:
            for line in ifile:
                if reLine in line:
                    self.testResults[0] = TestResult(cmd=self.cmd,
                                                     status=0,
                                                     comment=comment)
                    ## Klobber statSum, as we want the test to return success.
                    self.statSum = 0 
                    return
                    
        self.statSum += 1
        self.testResults[0] = TestResult(cmd=self.cmd,
                                         status=1,
                                         comment=comment)
        return
