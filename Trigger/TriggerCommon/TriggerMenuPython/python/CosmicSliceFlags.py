# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Cosmics slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper, AllowedList

__author__  = 'A. Cerri, after X. Wu example'
__version__="$Revision: 1.40 $"
__doc__="Cosmic slice specific flags  "


_flags = []

class SliceList(JobProperty):
    """ List of slices provided by the Cosmics slice """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[
        #'MU1Dummy'                         ,
        #'MU2Dummy'                         ,
        #'MU3Dummy'                         ,
        #'MU11Dummy'                        ,
        #'MU3Empty'                         ,
        #'MU11Empty'                        ,
        #'Dummy'                         ,
        #'Empty'                         ,
        #'StandaloneDummy'               ,
        #'StandaloneEmpty'               ,
        #'CaloDummy'                     ,
        #'CaloEmpty'                     ,
        #'RPCDummy'                      ,
        #'RPCEmpty'                      ,
        #'TGCDummy'                      ,
        #'TGCEmpty'                      ,
        #'AllTeIDSCAN'                        ,
        #'AllTeSiTrack'                       ,
        #'AllTeIDSCANCombined'                ,
        #'AllTeSiTrackCombined'               ,
        'CosmicsAllTeIDSCAN'                        ,
        'CosmicsAllTeSiTrack'                       ,
        'CosmicsAllTeIDSCAN_TrkHypo'                        ,
        'CosmicsAllTeSiTrack_TrkHypo'                       ,
        'CosmicsAllTeIDSCAN_AllPhysics_TrkHypo'                        ,
        'CosmicsAllTeSiTrack_AllPhysics_TrkHypo'                       ,
        'CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo'                        ,
        'CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo'                       ,
        'CosmicsAllTeEFID',
        #'AllTeTRTxK'                         ,
        #'AllTeTRTxK_TRTTrkHypo'              ,
        # this i#s defined 2 lines above        'AllTeTRTxK'                 ,
        # as abo#ve repetition        'AllTeTRTxK_TRTTrkHypo'      ,
        #'AllTeTRTxK_TRTTrkHypo_Pixel',
        'CosmicsAllTeTRTxK'                 ,
        'CosmicsAllTeTRTxK_TRTTrkHypo'      ,
        'CosmicsAllTeTRTxK_TRTTrkHypo_Pixel',
        'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics'      ,
        'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_NoField'      ,
        'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_PEB'      ,
        'Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_Pixel',
        'Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_AllID',
        'Cosmics_MU11_TRTxK_TRTTrkHypo_Pixel',
        'Cosmics_MU11_TRTxK_TRTTrkHypo_AllID',
        'Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_Pixel_IDCalib',
        #'AllTeTileRODMu'                ,
        ##'TileRODMu'                     ,
        #'CaloTileRODMu'                 ,
        #'StandaloneTileRODMu'           ,
        #'AllTeTileLookForMu'            ,
        #'TileLookForMu'                 ,
        #'CaloTileLookForMu'             ,
        #'StandaloneTileLookForMu'       ,
        #'Egamma'                        ,
        #'AllTeEgamma'                   ,
        #'Egamma_noHad'                  ,
        #'Jet'                           ,
        #'Tau'                           ,
        #'Calo'                          ,
        #'AllTeJet'                      ,
        #'AllTeTau'                      ,
        #'AllTeCalo'                     ,
        #'Muon'                          ,
        #'MuonOnly'                      ,
        #'MuonOnly_TGC'                  ,
#        'Muon_Moore'                    ,
#        'Muon_Moore_TGC'                ,
        #'Muon_MuonEF'                    ,
        #'Muon_MuonEF_TGC'                ,
        #'Mufast'                        ,
        #'Mufast_WideRPC'                ,
        #'Mufast_TGC'                    ,
        #'Mufast_MuIso'                  ,
#        'Mufast_Moore_TGC'              ,
#        'Mufast_Moore'                  ,
        #'Mufast_MuonEF_TGC'              ,
        #'Mufast_MuonEF'                  ,
        ##        'Mufast_Moore_WideRPC'          ,
        #'Mufast_MuonEF_WideRPC'         ,
        #'AllTeMEt'                           ,
        #'AllTeMEt_EFOnly'                    ,
        #'AllTe_MEt_EFOnly_Mufast_MuonEF',
        #'AllTe_MEt_Mufast_MuonEF'       ,
        #'AllTe_MEt_EFOnly_MuonEF',
        #'AllTe_MEt_MuonEF'       ,
        #'MEt'                           ,
        #'MEt_EFOnly'                    ,
        #'MEt_Mufast'                    ,
        #'MEt_EFOnly_Mufast'             ,
        #'MEt_EFOnly_Mufast_MuonEF'      ,
        #'MEt_Mufast_MuonEF'             ,
        #'AllTeMBiasCalo_EndCapA',
        #'AllTeMBiasCalo_EndCapC',
        #'AllTeMBSpacePoints'                 ,
        ##'AllTeMBStandaloneSpacePoints'       ,
        #'StandaloneCalibration'         ,
        #'BCMPassthrough',
        #'LUCIDPassthrough',
        #'RNDM0Passthrough'               ,
        #'RNDM1Passthrough'               ,
        #'NIM0Passthrough'               ,
        #'NIM4Passthrough'               ,
        'TilePassthrough'               ,
        'TRTPassthrough'               ,
        'EM3_EMPTYPassthrough'       ,
        'J5_EMPTYPassthrough'       ,
        'RD0_EMPTYPassthrough'       ,
        #'Calibration'                   ,
        #'CALREQ0Calibration'                   ,
        #'CALREQ1Calibration'                   ,
        #'CALREQ2Calibration'                   ,
        #'EM5Passthrough'                   ,
        #'EM10Passthrough'                   ,
        #'EM15Passthrough'                   ,
        #'EM20Passthrough'                   ,
        #'EM30Passthrough'                   ,
        #'EM40Passthrough'                   ,
        #'EM50Passthrough'                   ,
        #'EM100Passthrough'                   ,
        #'JET5Passthrough'                   ,
        #'JET10Passthrough'                   ,
        #'JET15Passthrough'                   ,
        #'JET20Passthrough'                   ,
        #'JET30Passthrough'                   ,
        #'JET40Passthrough'                   ,
        #'JET50Passthrough'                   ,
        #'JET100Passthrough'                   ,
        #'TAU5Passthrough'                   ,
        #'TAU10Passthrough'                   ,
        #'TAU15Passthrough'                   ,
        #'TAU20Passthrough'                   ,
        #'TAU30Passthrough'                   ,
        #'TAU40Passthrough'                   ,
        #'TAU50Passthrough'                   ,
        #'TAU100Passthrough'                   ,
        #'JF5Passthrough'                   ,
        #'JF10Passthrough'                   ,
        #'JF15Passthrough'                   ,
        #'JF20Passthrough'                   ,
        #'JB5Passthrough'                   ,
        #'JB10Passthrough'                   ,
        #'JB15Passthrough'                   ,
        #'JB20Passthrough'                   ,
        #'MBTSPassthrough'                  ,
        #'L1CaloPassthrough'                , 
        #'L1CaloFilter'               ,
        #'L1CaloEMFilter'             ,
        #'L1CaloTauFilter'            ,
        #'L1CaloJetFilter'            ,
        #'CosmicDownwardMuonRPCROI',
        #'CosmicDownwardMuonTGCROI',
        #'PhysicsPassthrough'         ,
        #'CalibrationPassthrough'     ,
        #'L2CalibrationPassthrough'     ,
        #'EFCalibrationPassthrough'     ,
        #'HLTBeamLineFitterTest',
        #'HLTBeamLineFitterTestFake',
        'TileCalib_laser',
        'TileCalib_cis',
        'TileCalib_laser_rejAll',
        'TileCalib_cis_rejAll',
        #'TileCalib_ped',
        #'Cosmic_pixelnoise',
        'Cosmic_rateanalysis',
        #'LArCalib',
        #"SingleBeamTriggerType0",       
        #"SingleBeamL1CaloEMFilter",     
        #"SingleBeamTGCwBeam",   
        #"SingleBeamRPCwBeam",
        #V2 menu
        'PixelNoise',
        'PixelBeam',
        'PixelNoise_rejAll',
        'SCTNoise',
        'SCTNoise_rejAll',
        'LArCalib_V2',
        'AllTeMBSpacePoints_V2',
        'CaloTileLookForMu_V2',
        'CaloTileRODMu_V2',
        'Egamma_V2',
        'Jet_V2',
        'VdM_RD0_UNPAIRED_ISO',
        'VdM_MBTS_2_UNPAIRED_ISO',
        'VdM_MBTS_1_1_UNPAIRED_ISO',
        'VdM_BGRP7',
        'VdM_LUCID_BGRP7',
        'VdM_ZDC_A_C_BGRP7',
        'VdM_ZDC_A_BGRP7',
        'VdM_ZDC_C_BGRP7',
        'VdM_ZDC_A_C_VTE50',
        'VdM_MBTS_2_BGRP7', 
        'VdM_mbMbts_1_1_MBTS2_BRGP7',        
        #'Tau_V2',
        #
        'Mufast_V2',
        'Mufast_MuIso_V2',
        'Mufast_MuonEF_V2',
        'MBiasCalo_EndCapA_V2',
        'MBiasCalo_EndCapC_V2',
        'L1CaloFilter_V2',    
        'L1CaloEMFilter_V2',  
        'L1CaloTauFilter_V2', 
        'L1CaloJetFilter_V2',
        #'MEt_V2',
        #'MEt_EFOnly_V2',
        #'MEt_EFOnly_Mufast_V2',
        #'MEt_Mufast_V2',
        #'MEt_EFOnly_Mufast_MuonEF_V2',
        #'MEt_Mufast_MuonEF_V2',
        'TileMon',
        'PreS_loose', 'PreS_tight',
        'PreS_loose_hi', 'PreS_tight_hi',
        'PreS_EM_loose', 'PreS_L1EM3_empty', 'PreS_L1EM5_empty',  'PreS_L1EM6_empty',
        'PreS_TAU8_loose', 'PreS_L1TAU8_empty',
        'PreS_J10_loose',
        'PreS_L1J5_empty',
        'PreS_L1J10_empty', 'PreS_L1J10_firstempty',
        'PreS_L1J30_empty', 'PreS_L1J30_firstempty',
        'AllTeTileMon',
        'TileMon_cosmic',
        ]
    allowedValues=AllowedList(StoredValue)
    
_flags.append(SliceList)

class CommandLineProperties(JobProperty):
    """ properties which will be affected by setting a global with the same name """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=['BFieldOn',
                 'isRealData','doM3Data','doM4Data',
                 'doEFIDCosmics','doIDHitsHypo','L2HitsHypoNTRTDC',
                 "EFHitsHypoNTRTDC","L2HitsHypoNTRTHTDC","EFHitsHypoNTRTHTDC",
                 'Signature',
                 'Threshold',

                 'Signature_V2'
                 'Threshold_V2'
                 
                  'testCosmic',
                 'CaloSignature',
                 'CaloThreshold',

                 'CaloSignature_V2',
                 'CaloThreshold_V2',

                 'RPCSignature',
                 'RPCThreshold',

                 'WidePatternRPCSignature',
                 'WidePatternRPCThreshold',

                 'TGCSignature',
                 'TGCThreshold',

                 'MuonSignature_V2'
                 'MuonThreshold_V2'

                 'TrackingSignature',
                 'TrackingThreshold',
                 
                 'TrackingSignature_V2',
                 'TrackingThreshold_V2',

                 'StandaloneSignature',
                 'StandaloneThreshold',

                 'StandaloneSignature_V2',
                 'StandaloneThreshold_V2',

                 'CalibrationSignature',
                 'CalibrationThreshold',

                 'CalibrationSignature_V2',
                 'CalibrationThreshold_V2',

                 'T2CaloFakeLVL1InLVL2',
                 'FakeLVL1',
                 'FakeLVL1Threshold',
                 'FakeLVL1ROIEta',
                 'FakeLVL1ROIPhi',
                 'forceLVL2Accept',
                 'filterEmptyROB',
                 'useCOMCONDDB',
                 'DontWriteOutAllTEStream',
                 'useUniqueMuFastInstance',
#                 'useUniqueMooreInstance',
                 'useUniqueMuonEFInstance',
                 'UseSingleBeam3BitL1Menu',
                 'UseSingleBeam7BitL1Menu',
                 'Use3Streams',
                 ]
    allowedValues = AllowedList(StoredValue)
_flags.append(CommandLineProperties)


class Use3Streams(JobProperty):
    """ UseSingleBeam7BitL1Menu """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
_flags.append(Use3Streams)

class UseSingleBeam3BitL1Menu(JobProperty):
    """ UseSingleBeam3BitL1Menu """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
_flags.append(UseSingleBeam3BitL1Menu)

class UseSingleBeam7BitL1Menu(JobProperty):
    """ UseSingleBeam7BitL1Menu """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
_flags.append(UseSingleBeam7BitL1Menu)

class testCosmic(JobProperty):
    """ testCosmic """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
_flags.append(testCosmic)

class BFieldOn(JobProperty):
    """ is B Field on? """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
_flags.append(BFieldOn)

class GlobalTag(JobProperty):
    """ GlobalTag """ 
    statusOn=True
    allowedTypes=['str']
    StoredValue="OFLCOND-CSC-00-01-00"
_flags.append(GlobalTag)

class DontWriteOutAllTEStream(JobProperty):
    """ DontWriteOutAllTEStream """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
_flags.append(DontWriteOutAllTEStream)

class useUniqueMuonEFInstance(JobProperty):
    """ useUniqueMuonEFInstance """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
_flags.append(useUniqueMuonEFInstance)

#class useUniqueMooreInstance(JobProperty):
#    """ useUniqueMooreInstance """ 
#    statusOn=True
#    allowedTypes=['bool']
#    StoredValue=True
#_flags.append(useUniqueMooreInstance)

class useUniqueMuFastInstance(JobProperty):
    """ useUniqueMuFastInstance """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
_flags.append(useUniqueMuFastInstance)

class useCOMCONDDB(JobProperty):
    """ use Commissioning condition DB """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
_flags.append(useCOMCONDDB)

class useCOMCONDDBInLAr(JobProperty):
    """ use Commissioning condition DB in LAr setup """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
_flags.append(useCOMCONDDBInLAr)

class UsedLVL1Thresholds(JobProperty):
    """ used LVL1 Thresholds """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[]
    #allowedValues = []
_flags.append(UsedLVL1Thresholds)

class UsedLVL1Signatures(JobProperty):
    """ used LVL1 Thresholds """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[]
    #allowedValues = []
_flags.append(UsedLVL1Signatures)

class filterEmptyROB(JobProperty):
    """ filterEmptyROB """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
_flags.append(filterEmptyROB)

class forceLVL2Accept(JobProperty):
    """ forceLVL2Accept """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
_flags.append(forceLVL2Accept)

class FakeLVL1(JobProperty):
    """ FakeLVL1 """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
_flags.append(FakeLVL1)

class FakeLVL1Threshold(JobProperty):
    """ FakeLVL1Threshold """ 
    statusOn=True
    allowedTypes=['str']
    StoredValue='MU3'
_flags.append(FakeLVL1Threshold)

class FakeLVL1ROIEta(JobProperty):
    """ FakeLVL1ROIEta """ 
    statusOn=True
    allowedTypes=['float']
    StoredValue=-0.5
_flags.append(FakeLVL1ROIEta)

class FakeLVL1ROIPhi(JobProperty):
    """ FakeLVL1ROIPhi """ 
    statusOn=True
    allowedTypes=['float']
    StoredValue=-1.5
_flags.append(FakeLVL1ROIPhi)

class T2CaloFakeLVL1InLVL2(JobProperty):
    """ T2CaloFakeLVL1InLVL2 """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
_flags.append(T2CaloFakeLVL1InLVL2)

class isRealData(JobProperty):
    """ run the Empty slice """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
_flags.append(isRealData)

class doM3Data(JobProperty):
    """ run the Empty slice """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
_flags.append(doM3Data)

class doM4Data(JobProperty):
    """ run the Empty slice """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
_flags.append(doM4Data)

class doEFIDCosmics(JobProperty):
    """ run the Empty slice """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
_flags.append(doEFIDCosmics)

class doIDHitsHypo(JobProperty):
    """ run the Dummy slice """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
_flags.append(doIDHitsHypo)

class L2HitsHypoNTRTDC(JobProperty):
    """ number of TRT DC in IDHITSHYPO selection (L2) """ 
    statusOn=True
    allowedTypes=['int']
    StoredValue=0
_flags.append(L2HitsHypoNTRTDC)

class EFHitsHypoNTRTDC(JobProperty):
    """ number of TRT DC in IDHITSHYPO selection (EF) """ 
    statusOn=True
    allowedTypes=['int']
    StoredValue=0
_flags.append(EFHitsHypoNTRTDC)

class L2HitsHypoNTRTHTDC(JobProperty):
    """ number of TRT HTDC in IDHITSHYPO selection (L2) """ 
    statusOn=True
    allowedTypes=['int']
    StoredValue=0
_flags.append(L2HitsHypoNTRTHTDC)

class EFHitsHypoNTRTHTDC(JobProperty):
    """ number of TRT HTDC in IDHITSHYPO selection (EF) """ 
    statusOn=True
    allowedTypes=['int']
    StoredValue=0
_flags.append(EFHitsHypoNTRTHTDC)

for x in SliceList.StoredValue:
    exec """class do%s(JobProperty):
\t ' run the %s slice '
\t statusOn=True
\t allowedTypes=['bool']
\t StoredValue=False
_flags.append(do%s)
""" % (x,x,x)

for x in ['StandaloneThreshold',
          'TrackingThreshold',
          'CalibrationThreshold',
          'StandaloneThreshold_V2',
          'TrackingThreshold_V2',
          'CalibrationThreshold_V2']:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='EM3'
_flags.append(%s)
""" % (x,x,x)

for x in ['StandaloneSignature',
          'TrackingSignature',
          'CalibrationSignature']:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='L1_EM3'
_flags.append(%s)
""" % (x,x,x)

for x in ['StandaloneSignature_V2',
          'TrackingSignature_V2',
          'CalibrationSignature_V2']:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='L1_EM3_EMPTY'
_flags.append(%s)
""" % (x,x,x)

for x in ['TGCSignature',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='L1_MU0_TGC'
_flags.append(%s)
""" % (x,x,x)

for x in ['TGCThreshold',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='MU0_TGC'
_flags.append(%s)
""" % (x,x,x)

for x in ['RPCSignature',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='L1_MU6_RPC'
_flags.append(%s)
""" % (x,x,x)

for x in ['RPCThreshold',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='MU6_RPC'
_flags.append(%s)
""" % (x,x,x)

for x in ['WidePatternRPCSignature',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='L1_MU0_LOW_RPC'
_flags.append(%s)
""" % (x,x,x)

for x in ['WidePatternRPCThreshold',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='MU0_LOW_RPC'
_flags.append(%s)
""" % (x,x,x)

for x in ['MuonSignature_V2',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='L1_MU0_EMPTY'
_flags.append(%s)
""" % (x,x,x)

for x in ['MuonThreshold_V2',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='MU0'
_flags.append(%s)
""" % (x,x,x)



for x in ['CaloSignature',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='L1_J10'
_flags.append(%s)
""" % (x,x,x)

for x in ['CaloSignature_V2',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='L1_J10_EMPTY'
_flags.append(%s)
""" % (x,x,x)


for x in ['CaloThreshold',
          'CaloThreshold_V2',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='J10'
_flags.append(%s)
""" % (x,x,x)


for x in ['Signature',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='L1_MU6_RPC'
_flags.append(%s)
""" % (x,x,x)

for x in ['Threshold',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='MU6_RPC'
_flags.append(%s)
""" % (x,x,x)

for x in ['Signature_V2',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='L1_MU6_EMPTY'
_flags.append(%s)
""" % (x,x,x)

for x in ['Threshold_V2',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='MU6'
_flags.append(%s)
""" % (x,x,x)




class signatures(JobProperty):
    """ signatures in Cosmic slice """
    statusOn=True
    allowedTypes=['list']
    StoredValue= SliceList.StoredValue
    allowedValues = AllowedList(StoredValue)
_flags.append(signatures)

class LVL2Payload(JobProperty):
    """ LVL2 payload """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[]
    #allowedValues = []
_flags.append(LVL2Payload)

class EFPayload(JobProperty):
    """ EF payload """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[]
    #allowedValues = []
_flags.append(EFPayload)

# create container
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper

class CosmicSlice(JobPropertyContainer, CommonSliceHelper):
    """ Cosmic Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(CosmicSlice)

# add add common slice flags
TriggerFlags.CosmicSlice.import_JobProperties('TriggerMenuPython.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.CosmicSlice.add_JobProperty(flag)
del _flags

# make an alias
CosmicSliceFlags = TriggerFlags.CosmicSlice

