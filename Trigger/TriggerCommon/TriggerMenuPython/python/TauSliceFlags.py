# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Tau slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper, AllowedList

__author__  = ''
__version__="$Revision: 1.34 $"
__doc__="Tau slice specific flags  "

_flags = [] 
class doTrackingApproach(JobProperty):
    """ Use tracking approach in Tau Trigger """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
    
_flags.append(doTrackingApproach)

class signatures(JobProperty):
    """ signatures in Tau slice """
    statusOn=True
    allowedTypes = ['list', 'str']
    allowedValues= AllowedList(['tau12_loose',
                                'tau16_loose',
                                'tau16_medium',
                                'tau20_loose',
                                'tau20_medium',
                                'tau20_medium_FTK',
                                'tau20i_medium',
                                'tau20v_medium',
                                'tau20T_medium',
                                'tau20T_medium1',
                                'tau20Ti_medium',
                                'tau20Ti_medium1',
                                'tau20_medium1',
                                'tau20i_medium1',
                                'tau20_medium1_pv',
                                'tau29_loose',
                                'tau29T_loose',
                                'tau29_loose1',
                                'tau29_medium',
                                'tau29T_medium',
                                'tau20_medium_llh',
                                'tau20_medium_llh',
                                'tau20_medium1_llh',
                                'tau20_medium1_llh',
                                'tau20Ti_medium1_llh',
                                'tau29Ti_medium1_llh',
                                'tau29_medium_llh',
                                'tau29_medium_llh',
                                'tau29_medium1_llh',
                                'tau29_medium1_llh',
                                'tau29T_medium1_llh',
                                'tau29_medium1',
                                'tau29i_medium1',
                                'tau29T_medium1',
                                'tau38_loose',
                                'tau50_loose',
                                'tau84_loose',
                                'tau84_medium',
                                'tau100_loose',
                                'tau100_medium',
                                'tau125_loose',
                                'tau20i_medium',
                                'tau29i_medium',
                                'tau29Ti_medium1',
                                'tau38_medium',
                                'tau38T_medium1',
                                'tau38T_medium1_llh',
                                'tau38T_medium',
                                'tau125_medium',
                                'tau125_medium1',
                                'tau125_medium1_llh',
                                'tauNoCut',                                
                                'tauNoCut_L1TAU40',
                                'tauNoCut_MV',
                                'tauNoCut_unpaired_iso',
                                'tauNoCut_unpaired_noniso',
                                'tauNoCut_firstempty',
                                'tauNoCut_L1TAU50',
                                'tauNoCut_IdScan',
                                'tauNoCut_L2Star',
                                'tauNoCut_FTK',
                                'tau50_medium',
                                'tau50_medium1',
                                'tau50_loose_IdScan',
                                'tauNoCut_cosmic',
                                'tau16_IDTrkNoCut',
                                'tau29_IDTrkNoCut',
                                'tau29_IDTrkNoCut_R1',
                                'tau29_IDTrkNoCut_IDT',
                                'tau12_IDTrkNoCut',
                                'tau50_IDTrkNoCut',
                                'tau125_IDTrkNoCut',
                                'tau125_IDTrkNoCut_IDT',
                                'tau16_L2Star_IDTrkNoCut',
                                'tau20T_medium1_L2NoCut',
                                'tau20T_loose1_L2NoCut',
                                'tau20T_loose2_L2NoCut',
                                'tau20Ti_medium1_L2NoCut',
                                'tau20Ti_loose1_L2NoCut',
                                'tau20Ti_loose2_L2NoCut',
                                'tau29Ti_loose1_L2NoCut',
                                'tau29Ti_loose2_L2NoCut',
                                'tau29Ti_medium1_L2NoCut',                                
                                'L1TAU30_NoAlg',
                                ##MultiTaus
                                '2tau29_loose1',
                                '2tau38_loose',
                                '2tau29_loose',
                                '2tau16_loose',
                                '2tau20_loose',
                                'tau29_medium_tau38_medium',
                                'tau29_medium_tau38_loose',
                                'tau29_loose_tau50_loose',
                                'tau29_loose_tau38_loose',
                                'tau20_loose_tau29_loose',
                                '2tau29_medium1',
                                '2tau29T_medium1',
                                '2tau29i_medium1',
                                '2tau38T_medium1',
                                '2tau38T_medium',
                                '2tau38T_medium1_llh',

                                'tau29_medium1_tau20_medium1',
                                'tau38_medium1_tau29_medium1',
                                'tau38T_medium1_tau29T_medium1',
                                'tau29T_medium1_tau20T_medium1',
                                'tau29Ti_medium1_tau20Ti_medium1',
                                'tau29Ti_medium1_llh_tau20Ti_medium1_llh',
                                'tau29Ti_medium1_tau20i_medium1',
                                'tau29Ti_medium1_1P_tau20Ti_medium2',
                                'tau29Ti_medium2_tau20Ti_medium2',
                                'tau29_medium_L2StarA', 'tau29_medium_L2StarB', 'tau29_medium_L2StarC', 
                                'tau125_medium1_L2StarA', 'tau125_medium1_L2StarB', 'tau125_medium1_L2StarC',
                                #2 step tracking test chain
                                'tau29_medium_2stTest',
                                'tau29_medium_L2Star_IDTrkNoCut',
                                #
                                'tau29T_tight1',
                                'tau20Ti_tight1',
                                'tau29Ti_tight1',
                                'tau29T_tight1_llh',
                                'tau20Ti_tight1_llh',
                                'tau29Ti_tight1_llh',
                                'tau29Ti_tight1_llh_tau20Ti_tight1',
                                'tau29Ti_tight1_llh_tau20Ti_tight1_llh',
                                'tau29Ti_tight1_tau20Ti_tight1',
                                #bdt chains
                                'tau20_medium_bdt', 'tau20_medium1_bdt',
                                'tau29_medium_bdt', 'tau29_medium1_bdt',
                                #chains for tau100_loose1_tau70_loose1
                                'tau70_loose1',
                                'tau100_loose1',
                                'tau100_loose1_tau70_loose1',
                                'tau115_medium1',
                                'tau115_medium1_llh',
                                #L2loose tau chains
                                'tau18_medium1_L2loose',
                                'tau18Ti_medium1_L2loose',
                                'tau20_medium1_L2loose',
                                'tau20Ti_medium1_L2loose',
                                'tau27T_medium1_L2loose',
                                'tau27Ti_medium1_L2loose',
                                'tau29T_medium1_L2loose',
                                'tau29Ti_medium1_L2loose',
                                'tau29Ti_medium1_tau20Ti_medium1_L2loose',
                                'tau29Ti_medium1_tau18Ti_medium1_L2loose',
                                'tau29Ti_medium1_L2loose_tau20Ti_medium1_L2loose',
                                'tau27Ti_medium1_L2loose_tau18Ti_medium1_L2loose',
                                ###
                                'tau27T_loose3',
                                'tau18Ti_loose3',
                                'tau18Ti_loose2',
                                'tau20_loose3',
                                'tau20_loose2',
                                'tau27Ti_loose3_tau18Ti_loose3',
                                'tau27Ti_loose2_tau18Ti_loose2',
                                'tau27Ti_loose3',
                                'tau27Ti_loose2',
                                'tau27T_loose2',
                                'tau27Ti_loose2_tau18Ti_loose2_deta25',
                                'tau27Ti_loose2_tau18Ti_loose2_deta20',
                                'tau27Ti_loose2_tau18Ti_loose2_deta18',
                                'tau27Ti_loose3_tau18Ti_loose3_deta25',
                                'tau27Ti_loose3_tau18Ti_loose3_deta20',
                                ##L2 pass through test chains
                                'tau29Ti_loose1_L2NoCut_tau20Ti_loose1_L2NoCut',
                                'tau29Ti_loose2_L2NoCut_tau20Ti_loose2_L2NoCut',
                                'tau29Ti_medium1_L2NoCut_tau20Ti_medium1_L2NoCut',
                                '',
                                ])

_flags.append(signatures)




# create container
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper

class TauSlice(JobPropertyContainer, CommonSliceHelper):
    """ Tau Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(TauSlice)

# add add common slice flags
TriggerFlags.TauSlice.import_JobProperties('TriggerMenuPython.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.TauSlice.add_JobProperty(flag)
del _flags

# make an alias
TauSliceFlags = TriggerFlags.TauSlice
