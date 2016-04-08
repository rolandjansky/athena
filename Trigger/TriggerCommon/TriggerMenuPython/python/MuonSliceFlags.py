# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Muon slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper, AllowedList

__author__  = 'T. Bold, B.Demirkoz'
__version__="$Revision: 1.46 $"
__doc__="Muon slice specific flags  "


_flags = [] 
class doSiTrack(JobProperty):
    """ do or not to do SiTrack algo """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

_flags.append(doSiTrack)

class doMuonCalibrationStream(JobProperty):
    """ do or not to do the setup for the muon calibration stream"""
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(doMuonCalibrationStream)

class doEFRoIDrivenAccess(JobProperty):
    """ do or not do the EF Roi driven access"""
    statusOn = True
    allowedType = ['bool']
    StoredValue = False

_flags.append(doEFRoIDrivenAccess)

class signatures(JobProperty):
    """ signatures in Muon slice """
    statusOn=True
    allowedTypes=['list']
    allowedValues = AllowedList([

        #test chain for running EF algos at L2
        'mu18_medium_L2EFMerged',
        'mu22_IDTrkNoCut_tight_IDT',

        #EFFS
        'mu15_mu10_EFFS', 'mu15_mu10_EFFS_medium', 'mu15_mu10_EFFS_tight', 
        'mu15_2mu4_EFFS_medium', 'mu18_2mu4_EFFS_tight',
        'mu18_mu8_EFFS_tight',
        'mu18_tight_Jpsi_EFFS',
        'mu20_mu10_EFFS_tight',
        'mu20_mu7_EFFS',
        'mu24_mu6_EFFS_medium',
        'mu15_mu7_EFFS', 
        'mu10_EFFS',
        'mu18_tight_mu8_EFFS', 'mu24_tight_mu6_EFFS', 'mu18_tight_2mu4_EFFS', 'mu24_tight_mu6_EFFS_2012', 

        #
        'mu18_L2_2mu4T_Jpsimumu',
        
        #outoftime
        'mu0_outOfTime1', 'mu0_outOfTime2',

        #l2cal
        'mu6_l2cal',
        'mu10_l2cal', 'mu10_l2cal_medium', 'mu10_l2cal_tight', 'mu15_l2cal',
        'mu0_l2cal_empty', 
        'mu4_l2cal_empty', 

        #MG
        'mu4_MG', 
        'mu10_MG', 
        'mu13_MG', 
        'mu15_MG', 
        'mu18_MG', 'mu18_MG_L1J10', 'mu18_MG_medium',
        'mu22_MG', 'mu22_MG_medium',
        'mu20_MG', 'mu20_MG_medium',
        'mu24_MG_medium', 'mu24_MG_tight',
        'mu30_MG_medium', 'mu30_MG_tight',

        'mu36_tight',
        'mu40_tight',
        'mu40_slow', 'mu40_slow_medium', 'mu40_slow_tight', 'mu40_slow_empty',
        'mu40_slow_outOfTime', 'mu40_slow_outOfTime_medium', 'mu40_slow_outOfTime_tight',
        'mu40_slow_tight', 'mu40_slow_outOfTime_tight',
        'mu60_slow_tight1', 'mu60_slow_outOfTime_tight1',
        'mu60_slow1_tight1',

        #Hidden Valley
        '2MUL1_l2j30_HV', 
        '2MUL1_l2j30_HV_cosmic', 
        '2MUL1_l2j30_HV_firstempty', 
        '2MUL1_l2j30_HV_unpaired_iso', 
        '2MUL1_l2j30_HV_unpaired_noniso', 
        '2MUL1_l2j30_HV_AllMS',

        #L2Star
        'mu18_medium_L2StarA',
        'mu18_medium_L2StarB',
        'mu18_medium_L2StarC',
        'mu4T_L2StarA',
        'mu4_L2StarA',
        'mu4T_L2StarB',
        'mu4_L2StarB',
        'mu4T_L2StarC',
        'mu4_L2StarC',
        'mu4_L2Star_NoCut',
        'mu18i4_medium_L2StarA',
        'mu18i4_medium_L2StarB',
        'mu18i4_medium_L2StarC',
        'mu10_L2StarA_wOvlpRm',
        'mu10_L2StarB_wOvlpRm',
        'mu10_L2StarC_wOvlpRm',
        'mu24_tight_L2StarB',
        'mu24_tight_L2StarC',
        'mu4T', 
        'mu4wOvlpRm', 
        'mu4T_wOvlpRm', 
        'mu4_MSonly_wOvlpRm', 
        'mu4',
        'mu4_NoCut',
        'mu4_loose',
        'mu4_cosmic', 
        'mu4T_cosmic', 
        'mu4_firstempty', 'mu4_unpaired', 'mu4_unpaired_iso', 
        'mu4_L1MU11_cosmic', 
        'mu4_MSonly_cosmic', 
        'mu4_L1MU11_MSonly_cosmic', 
        'mu6', 'mu6_wOvlpRm', 'mu6_NL',
        'mu6_L2StarB',
        'mu8', 'mu8_wOvlpRm',
#        'mu8_newl2muonSA',
        'mu10_wOvlpRm', 'mu10_empty_wOvlpRm', 'mu10_loose_empty_wOvlpRm', 'mu10_loose_wOvlpRm', 'mu10_loose1_wOvlpRm', 
        'mu10', 'mu10_loose', 'mu10_NL', 
        'mu13', 'mu13_wOvlpRm',
        'mu15',
        'mu15_wOvlpRm',
        'mu15_medium', 
        'mu18', 'mu18_L1J10', 'mu18_medium', 'mu18_tight', 
        'mu20', 'mu20_medium', 'mu20_tight',
        'mu20_empty', 
        'mu22', 'mu22_medium',
        'mu24', 'mu24_medium', 'mu24_tight', 'mu24_tight_MuonEF', 'mu24_tight_MG', 'mu24_tight_l2muonSA', 'mu24_tight_muFast',
        'mu30_medium', 'mu30_tight',
        'mu40',

        #IDTrkNoCut
	'mu8_IDTrkNoCut', 'mu10_IDTrkNoCut',
        'mu13_IDTrkNoCut', 
        'mu20_IDTrkNoCut_ManyVx', 
        'mu20_IDTrkNoCut', 
        'mu4_IDTrkNoCut', 
        'mu4T_IDTrkNoCut', 
        'mu6_IDTrkNoCut',
        'mu20_L2Star_IDTrkNoCut', 
        'mu18_IDTrkNoCut_tight', 'mu22_IDTrkNoCut_tight',
        
        #isolated trigger
        'mu10i_loose', 
        'mu15i_loose',
        'mu15i',
        'mu15i_medium',
        'mu18i1_medium', 'mu18i2_medium', 'mu18i3_medium','mu18i4_medium','mu18i5_medium',
        'mu18i4_tight',
        'mu20i1_medium', 'mu20i2_medium','mu20i3_medium', 'mu20i4_medium',                                 
        'mu22i1_medium', 'mu22i2_medium', 'mu22i3_medium', 'mu22i4_medium',
        'mu18i4_tight',
        'mu24i_tight', 'mu24i5_tight', 'mu24i_tight_MuonEF', 'mu24i_tight_MG', 'mu24i_tight_l2muonSA', 'mu24i_tight_muFast',
        'mu15ii_debug', 
        'mu4Tii_debug', 
        'mu6ii_debug',     
        'mu20i', 'mu20i_loose','mu20i_medium', 'mu20it_tight','mu18it_tight', 'mu20i_tight', 
        'mu4i',
        'mu4Ti',
        'mu6i', 
        'mu10i',

        #MSonly
        'mu40_MSonly', 
        'mu40_MSonly_empty', 
        'mu40_MSonly_tight', 
        'mu40_MSonly_tight_L1MU11', 
        'mu40_MSonly_tighter', 
        'mu40_MSonly_barrel_medium1',
        'mu60_MSonly', 
        'mu80_MSonly', 
        'mu100_MSonly', 
        'mu40_MSonly_barrel', 
        'mu40_MSonly_barrel_medium',
        'mu40_MSonly_barrel_tight', 'mu50_MSonly_barrel_tight',
        'mu20_MSonly', 
        'mu10_MSonly', 
        'mu10_MSonly_wOvlpRm', 
        'mu6_MSonly',
        'mu6_MSonly_wOvlpRm', 
        'mu4_MSonly', 
        
        #muCombTag
        'mu10_muCombTag_NoEF', 
        'mu13_muCombTag_NoEF', 
        'mu20_muCombTag_NoEF', 
        'mu24_muCombTag_NoEF_tight', 
        'mu10_muCombTag_NoEF_tight',
        'mu15_muCombTag_NoEF_tight',
        'mu20_muCombTag_NoEF_tight',
        'mu10_muCombTag_NoEF_tight1',
        'mu15_muCombTag_NoEF_tight1',
        'mu20_muCombTag_NoEF_tight1',

        #NSW emulation for upgrade study
        'mu15_NSWemul_NoL2',
        'mu15_NSWemul_NoEF',
        'mu20_NSWemul_NoL2',
        'mu20_NSWemul_NoEF',
        
        #SiTrk
        'mu4_SiTrk', 
        'mu4T_SiTrk', 
        'mu4_MSonly_EFFS_passL2', 

        #noroi
        'mu0_missingRoi',

        #NoAlg
        'L1MU0_NoAlg', 
        'L1MU10_NoAlg',
        'L1MU11_NoAlg', 
        'L1MU15_NoAlg', 
        'L1MU20_NoAlg', 
        'L12MU4_NoAlg',
        '2mu10_NoAlg', 
        'mu0_empty_NoAlg', 
        'mu4_empty_NoAlg', 
        'mu0_firstempty_NoAlg', 
        'mu4_firstempty_NoAlg', 
        'mu0_unpaired_iso_NoAlg', 
        'mu4_unpaired_iso_NoAlg', 
        'mu0_unpaired_noniso_NoAlg', 
        'L1MU10_firstempty_NoAlg', 
        'mu11_empty_NoAlg', 
        

        #MergedChains
        'mu20_medium_j10dR02veto',
        'mu20_medium_j30dR02veto',
        'mu20_medium_j10dR04veto',
        'mu20_medium_j30dR04veto',

        #MultiMuons
        '2mu4', '2mu4T', '2mu4_MSonly', '2mu4T_MSonly', '3mu4', '3mu4T', '4mu4', '2mu6', '2mu8',
        '2mu6_NL', 'mu4_mu6',
        '2mu10', '2mu13', '2mu15', '2mu13_l2muonSA', '2mu13_muFast',
        '3mu6' ,
        '3mu6_MSonly', '3mu6_MSonly_EMPTY', '3mu6_MSonly_UNPAIRED_ISO',
        '4mu4T',
        '2mu4_noOvlpRm', '2mu10_noOvlpRm', '3mu6_MSonly_noOvlpRm',
        '3mu6_MSonly_nonfilled', 
        '2mu10_loose', '2mu10_loose_noOvlpRm', '2mu10_loose_empty', '2mu10_empty', '2mu10_loose1',
        
        #L2Star
        '2mu10_L2StarA', '2mu10_L2StarB','2mu10_L2StarC',
        #HI
        '2mu6_MSonly', '2mu6T_MSonly',
        'mu4_barrel_L1VTE50', 'mu4_MSonly_barrel_L1TE50',
        'mu6_MSonly_L1TE50', 'mu6_L1VTE50', 

        #pPb
        'mu4_MSonly_EFFS_L1ZDC_OR', 'mu4_MSonly_EFFS_L1MBTS',
        '2mu2_MSonly_EFFS_L1ZDC_OR', '2mu2_MSonly_EFFS_L1MBTS', 
        #PbPb
        'mu4_MSonly_EFFS_L1ZDC', 'mu4_MSonly_EFFS_L1TE10',
        'mu4_MSonly_EFFS_L1TE20', 'mu4_MSonly_EFFS_L1TE50',
	'2mu4_MSonly_EFFS_L1ZDC', '2mu4_MSonly_EFFS_L1TE10',
        '2mu4_MSonly_EFFS_L1TE20', '2mu4_MSonly_EFFS_L1TE50',
        
        '2mu2_MSonly_EFFS_L1ZDC', 'mu4_mu2_MSonly_EFFS_L1ZDC',
        '2mu2_MSonly_EFFS_L1TE10', 'mu4_mu2_MSonly_EFFS_L1TE10',
        '2mu2_MSonly_EFFS_L1TE20', 'mu4_mu2_MSonly_EFFS_L1TE20',
        '2mu2_MSonly_EFFS_L1TE50', 'mu4_mu2_MSonly_EFFS_L1TE50',
        #
        '2mu2_EFFS', 'mu4_mu2_EFFS', 'mu2',
	'2mu2_MSonly_EFFS_L1MU0', '2mu2_EFFS_L1MU0', 'mu4_mu2_EFFS_L1MU0',
        '2mu4_MSonly_EFFS_L1MU0',
        'mu10_MSonly_EFFS_L1ZDC', 'mu10_MSonly_EFFS_L1TE10',
        'mu10_MSonly_EFFS_L1TE20', 'mu10_MSonly_EFFS_L1TE50',
        'mu13_MSonly_EFFS_L1ZDC', 'mu13_MSonly_EFFS_L1TE10',
        'mu13_MSonly_EFFS_L1TE20', 'mu13_MSonly_EFFS_L1TE50',
        #

        ###FTK
        'mu24_tight_FTK',
        'mu20it_tight_FTK',
        'mu6_FTK',
        ###
        'mu4_MSonly', 'mu4T_MSonly', 'mu4', 'mu4T', 'mu4_IDTrkNoCut', 'mu4T_IDTrkNoCut',
        'L1MU0_NoAlg', 'L1MU4_NoAlg', 'L1MU11_NoAlg', 'L1MU15_NoAlg', 'L1MU20_NoAlg',
        'mu4_cosmic', 'mu4_MSonly_cosmic', 'mu4T_cosmic', 'mu4T_MSonly_cosmic',
        'mu6_l2cal', 'mu4_l2cal_empty', 'mu4T_l2cal', 'mu15_l2cal',

        #MC_HI_v1
        'mu4_MSonly_EFFS_L1ZDC',
        '2mu4_MSonly_EFFS_L1ZDC',
        'mu10_MSonly_EFFS_L1ZDC',
        'mu13_MSonly_EFFS_L1ZDC',
        'mu4_MSonly_barrel_EFFS_L1ZDC',
        'mu4_MSonly_barrel',
        'mu4T_MSonly_barrel',
        'mu4_MSonly_EFFS_L1TE5_NZ',
        'mu4_MSonly_EFFS_L1MU0_NZ',
        'mu4_MSonly_L1TE50',
        'mu4_L1VTE50',
        'mu4T_MSonly_L1TE50',
        'mu6T_MSonly',
        'mu6T',
        'mu6T_IDTrkNoCut',
        'mu10T_MSonly',
        'mu13_MSonly', 'mu13T_MSonly',


        # TrigMuSuperEF chains
        #'mu18_OR_medium', 
        
        # TrigMuSuperEF iso chains
        ##'mu18i1_OR', 'mu18i2_OR', 'mu18i3_OR', 'mu18i4_OR', 'mu18i5_OR',
        ##'mu24i1_OR', 'mu24i2_OR', 'mu24i3_OR', 'mu24i4_OR',

        #'mu24i5_OR_tight',

        # TrigMuSuperEF special performance comparison chains
        'mu18_OR_medium_MGfirst',
        'mu18_OR_medium_TMEFfirst',
        'mu18_OR_medium_both',

        'mu24i5_OR_tight_MGfirst',
        'mu24i5_OR_tight_TMEFfirst',
        'mu24i5_OR_tight_both',

        # TrigMuSuperEF special validation chains
        'mu18_OR_medium_MGonly',
        'mu18_OR_medium_TMEFonly',
        #'mu24i1_OR_tight_TMEFonly',
        #'mu24i2_OR_tight_TMEFonly',
        #'mu24i3_OR_tight_TMEFonly',
        #'mu24i4_OR_tight_TMEFonly',
        'mu24i5_OR_tight_TMEFonly',
        'mu24i5_OR_tight_MGonly',
        'mu18ia1_medium', 'mu18ia2_medium',
        'mu18ia3_medium', 'mu18ia4_medium',
        'mu24ia1_tight', 'mu24ia2_tight',
        'mu24ia3_tight', 'mu24ia4_tight',
        'mu6ia1_MSonly_wOvlpRm', 'mu6ia2_MSonly_wOvlpRm',
        '2mu6ia1_MSonly', '2mu6ia2_MSonly',
        '2mu10ia1_MSonly', 'mu10_mu6ia1_MSonly',
        '2mu10ia2_MSonly', 'mu10_mu6ia2_MSonly',
        '2mu10ia3_MSonly', 'mu10_mu6ia3_MSonly',
        '2mu10ia4_MSonly', 'mu10_mu6ia4_MSonly',
        'mu6ia3_MSonly_wOvlpRm', '2mu6ia3_MSonly',
        'mu6ia4_MSonly_wOvlpRm', '2mu6ia4_MSonly',

        #l2muonSA chains
        'mu18_l2muonSA_medium',
        'mu10_l2muonSA_wOvlpRm',
        'mu4T_l2muonSA',
        'mu4T_muFast',
        '2mu10_l2muonSA',
        'mu13_wOvlpRm_l2muonSA'
        
        #IDTrkNoCut_R1, running still IDScan/SiTrack
        'mu4T_IDTrkNoCut_R1',
        'mu22_IDTrkNoCut_tight_R1'
        ])
    StoredValue   = [
        'mu4',
        'mu6', 'mu10', 'mu15', 'mu20', 'mu40',
        'mu15i_loose', 'mu20i_loose', 'mu15i', 'mu20i', 
        '2mu4','2mu6', '2mu10',
        '3mu6', '3mu4','4mu4', 'mu4_mu6', '3mu4T',
        ]

_flags.append(signatures)


# create container

class MuonSlice(JobPropertyContainer, CommonSliceHelper):
    """ Muon Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(MuonSlice)

# add add common slice flags
TriggerFlags.MuonSlice.import_JobProperties('TriggerMenuPython.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.MuonSlice.add_JobProperty(flag)
del _flags

# make an alias
MuonSliceFlags = TriggerFlags.MuonSlice
