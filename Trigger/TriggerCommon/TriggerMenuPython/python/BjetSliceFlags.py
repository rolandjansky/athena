# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Bjet slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper, AllowedList

__author__  = 'Andrea Coccaro'
__version__="$Revision: 1.1 $"
__doc__="Bjet slice specific flags  "


_flags = [] 
class doSiTrack(JobProperty):
    """ do or not to do SiTrack algo """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

_flags.append(doSiTrack)

class doIDSCAN(JobProperty):
    """ do or not to do IDSCAN algo """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

_flags.append(doIDSCAN)

class signatures(JobProperty):
    """ signatures in Bjet slice """
    statusOn=True
    allowedTypes=['list']
    allowedValues = AllowedList([
        ##v4 chains
        '2b35_loose_j145_j100_j35_a4tchad',
        
        #primary single b-jet triggers (#91842)
        'b55_looseEF_j55_a4tchad',
        'b80_looseEF_j80_a4tchad',
        'b110_looseEF_j110_a4tchad',        
        'b15_looseEF_j15_a4tchad',
        'b25_looseEF_j25_a4tchad',
        'b35_looseEF_j35_a4tchad_L1J15',#L2
        'b45_looseEF_j45_a4tchad_L1J15',
        'b145_loose_j145_a4tchad',
        'b180_loose_j180_a4tchad_L2j140', 
        'b220_loose_j220_a4tchad_L2j140', 
        'b280_loose_j280_a4tchad_L2j140', 
        'b360_loose_j360_a4tchad_L2j140', 

        #new tight multijet 
        'b55_tight_4j55_a4tchad',
        'b55_tight_4j55_a4tchad_L2FS',
        'b45_tight_4j45_a4tchad', 
        'b45_tight_4j45_a4tchad_L2FS', 
        'b55_tight_3j55_a4tchad_4L1J15',
        'b55_tight_3j55_a4tchad_L2FS_4L1J15',
        'b45_tight_j145_j45_a4tchad_ht400',
        
        
        #4-LVL1 jet seeded trigger (#91805, #92019)        
        'b55_medium_4j55_a4tchad',
        '2b55_medium_4j55_a4tchad',
        '2b55_loose_4j55_a4tchad',
        'b55_medium_3j55_a4tchad_L2FS_4L1J15',
        'b45_medium_3j45_a4tchad_L2FS_4L1J15',
        'b35_medium_3j35_a4tchad_L2FS_4L1J15',
        '2b55_medium_3j55_a4tchad_L2FS_4L1J15',
        '2b45_medium_3j45_a4tchad_L2FS_4L1J15',
        '2b35_medium_3j35_a4tchad_L2FS_4L1J15',
        'b45_medium_4j45_a4tchad',
        '2b35_loose_4j35_a4tchad',
        '2b35_loose_3j35_a4tchad_4L1J15',
        '2b35_loose_j145_2j35_a4tchad',
        '2b35_loose_j145_j35_a4tchad',

        '2b35_loose_3j35_a4tchad_L2FS_4L1J15',
        '2b35_loose_4j35_a4tchad_L2FS',
        'b55_loose_4j55_a4tchad_L2FS',

        # not combined b-jet triggers (#91828)
        #(i) 1b/2j and 2b/2j triggers
        '2b55_loose_j110_j55_a4tchad',
        '2b55_loose_j110_j55_a4tchad_1bL2',
        'b55_medium_j110_j55_a4tchad',
        #(ii)1b/4j and 2b/4j 
        'b55_medium_4j55_a4tchad',
        '2b55_medium_4j55_a4tchad',
        'b55_loose_4j55_a4tchad',
        'b55_medium_4j55_a4tchad_L2FS',
        '2b55_medium_4j55_a4tchad_L2FS',
        '2b55_loose_4j55_a4tchad_L2FS',
        #(iii) non-standard 1b/4j and 2b/4j triggers
        'b55_medium_3j55_a4tchad_4L1J15',
        'b45_medium_3j45_a4tchad_4L1J15',
        'b35_medium_3j35_a4tchad_4L1J15',
        '2b55_medium_3j55_a4tchad_4L1J15',
        '2b45_medium_3j45_a4tchad_4L1J15',
        '2b35_medium_3j35_a4tchad_4L1J15',
        #(iv)NoCut trigger
        'b15_NoCut_j15_a4tchad',
        'b55_NoCut_j55_a4tchad',
        'b55_NoCut_j55_a4tchad_R1',
        'b55_NoCut_j55_a4tchad_L2FS',
        'b35_NoCut_4j35_a4tchad',
        'b35_NoCut_4j35_a4tchad_L2FS',
        'b55_NoCut_j55_a4tchad_FTK',
        'b55_NoCut_j55_a4tchad_FTK_Refit',
        'b55_NoCut_j55_a4tchad_IDT',

        # b-tagged fat jets (#91900)
        'b110_loose_j110_a10tcem_L2FS_L1J75',
        'b145_loose_j145_a10tcem_L2FS',
        'b180_loose_j180_a10tcem_L2FS',
        'b180_loose_j180_a10tcem_L2j140',
        'b220_loose_j220_a10tcem',
        'b280_loose_j280_a10tcem',
        'b240_loose_j240_a10tcem_L2FS',
        'b240_loose_j240_a10tcem_L2j140',

        #HT triggers
        'b145_medium_j145_a4tchad_ht400',
        'b165_medium_j165_a4tchad_ht500',
        '2b55_loose_j110_j55_a4tchad_ht500',
        '2b55_medium_j110_j55_a4tchad_ht500',
        '2b55_medium_j165_j55_a4tchad_ht500',
        '2b80_medium_j165_j80_a4tchad_ht500',
        'b45_medium_j145_j45_a4tchad_ht400',
        'b45_medium_j145_j45_a4tchad_ht500',

        #mu+jet trigger
        'b45_NoCut_j45_a4tchad_L1J15',
        'b45_NoCut_j45_a4tchad',
        'b65_NoCut_j65_a4tchad',
        'b80_NoCut_j80_a4tchad',
        'b110_NoCut_j110_a4tchad',
        'b145_NoCut_j145_a4tchad',
        'b180_NoCut_j180_a4tchad',
        'b220_NoCut_j220_a4tchad',
        'b280_NoCut_j280_a4tchad',
        'b360_NoCut_j360_a4tchad',
        'b25_NoCut_j25_a4tchad',
        'b35_NoCut_j35_a4tchad',

        'b45_NoCut_j45_a4tchad_L2FS_L1J15',
        'b45_NoCut_j45_a4tchad_L2FS',
        'b55_NoCut_j55_a4tchad_L2FS',
        'b65_NoCut_j65_a4tchad_L2FS',
        'b80_NoCut_j80_a4tchad_L2FS',
        'b110_NoCut_j110_a4tchad_L2FS',
        'b145_NoCut_j145_a4tchad_L2FS',
        'b180_NoCut_j180_a4tchad_L2FS',
        'b220_NoCut_j220_a4tchad_L2FS',
        'b280_NoCut_j280_a4tchad_L2FS',
        'b360_NoCut_j360_a4tchad_L2FS',

        # NoCut triggers #95845
        'b80_NoCut_j80_a4tchad',
        'b110_NoCut_j110_a4tchad',
        'b145_NoCut_j145_a4tchad',
        'b180_NoCut_j180_a4tchad',
        'b220_NoCut_j220_a4tchad',
        'b280_NoCut_j280_a4tchad',
        'b360_NoCut_j360_a4tchad',
        'b45_NoCut_4j45_a4tchad',
        'b45_NoCut_4j45_a4tchad_L2FS',

        'b55_medium_2j55_a4tchad_L1J20',
        '2b55_loose_2j55_a4tchad_L1J20',
        'b65_medium_2j65_a4tchad',
        '2b55_medium_2j55_a4tchad_L1J20',
        
        #ZH triggers
        'b80_loose_j80_a4tchad',
        'b110_loose_j110_a4tchad', 
        ## Bjet + jet
        '2b55_loose_j145_j55_a4tchad',
        '2b45_loose_j145_j45_a4tchad',
        '2b35_loose_j110_2j35_a4tchad',
        'b35_loose_j110_2j35_a4tchad',
        #bjet+met (#92390)
        'b55_mediumEF_j110_j55_a4tchad',
        '2b35_loose_j110_2j35_a4tchad',
        'b55_mediumEF_j110_j45_a4tchad',
        'b145_mediumEF_j145_a4tchad', 

        # Additional Multijet Triggers (#92451)
        'b55_medium_4j55_a4tchad_4L1J10',        
        'b55_medium_4j55_a4tchad_L2FS_4L1J10',
        'b45_medium_4j45_a4tchad_4L1J10',
        'b45_medium_4j45_a4tchad_L2FS_4L1J10',
        '2b35_loose_3j35_a4tchad_4L1J10',
        '2b35_loose_3j35_a4tchad_L2FS_4L1J10',
        
        'b35_loose_4j35_a4tchad_L2FS_5L1J10',
        'b35_NoCut_4j35_a4tchad_L2FS_5L1J10',
        'b35_loose_4j35_a4tchad_5L1J10',
        'b35_NoCut_4j35_a4tchad_5L1J10',
        'b45_medium_4j45_a4tchad_L2FS',

        # FJ seeded 
        'b35_medium_j35_a4tchad_3L1J15_FJ15',
        'b35_medium_j35_a4tchad_L2FS_3L1J15_FJ15',
        'b35_NoCut_j35_a4tchad_3L1J15_FJ15',
        'b35_NoCut_j35_a4tchad_L2FS_3L1J15_FJ15',
        'b35_medium_j35_a4tchad_2L1FJ15',
        'b35_medium_j35_a4tchad_L2FS_2L1FJ15',
        'b35_NoCut_j35_a4tchad_2L1FJ15',
        'b35_NoCut_j35_a4tchad_L2FS_2L1FJ15',
        
    ##v3,v2 chains
        'b10', 'b10_loose', 'b10_medium', 'b10_tight',                
        'b15', 'b15_loose', 'b15_medium', 
        'b20_medium', 'b75_medium',       
        'b10_medium_4L1J10',         
        'b10_tight_4L1J10', 
        'b10_tight_L1JE100',
        'b10_tight_L1JE140',        
        'b10_IDTrkNoCut', 
        'b15_IDTrkNoCut', 
        'b15_medium_4L1J15',
        'b15_medium_L1J75',
        'b15_medium_L12J15J50',
        'b20_IDTrkNoCut', 
        'mu4_L1J5_matched', 
        'mu4_L1J10_matched',
        'mu4_L1J15_matched',
        'mu4_L1J20_matched',
        'mu4_L1J30_matched',
        'mu4_L1J50_matched', 
        'mu4_L1J75_matched',
        'mu4_j135_a4tc_EFFS_L1matched',
        'mu4_j180_a4tc_EFFS_L1matched',
        'b10_L2Star_IDTrkNoCut',
        'b10_j30_a4tc_EFFS_IDTrkNoCut', 
        'b10_EFj10_a4tc_EFFS_IDTrkNoCut', 
        'b10_L2Star_IDTrkNoCut_multiroi',
        'b10_tight_4j30_a4tc_EFFS', 
        'b10_medium_4j30_a4tc_EFFS',
        'b10_medium_j75_j55_2j30_a4tc_EFFS',
        'b10_tight_j75_j55_2j30_a4tc_EFFS', 
        'b15_medium_4j45_a4tc_EFFS',
        'b15_medium_j100_j40_a4tc_EFFS',
        'b15_medium_j100_j40_a4tc_EFFS_ht300',
        'b75_medium_j100_a4tc_EFFS',
        'b75_medium_j100_a4tc_EFFS_ht200',
        'b75_medium_j100_a4tc_EFFS_ht300',
        'mu4_j10_a4tc_EFFS_matched',
        'mu4_j20_a4tc_EFFS_matched',

        #HI
	'mu4_MSonly_j15_a2hi_EFFS_matched',
	'mu4_MSonly_j15_a4tchad_EFFS_matched',
        'mu4_j10_a4tchad_EFFS_matched', 'mu4_j10_a4hi_EFFS_matched',
	'mu4_j15_a4tchad_EFFS_matched', 'mu4_j15_a4hi_EFFS_matched',
        'mu4_j20_a4tchad_EFFS_matched', 'mu4_j20_a4hi_EFFS_matched',
        'mu4_j30_a4tchad_EFFS_matched', 'mu4_j30_a4hi_EFFS_matched',
        'mu4_j40_a4tchad_EFFS_matched', 'mu4_j40_a4hi_EFFS_matched',
        '2b10_3L1J10',
        '2b10_4L1J5',
        '3b10_4L1J5',
        '3b10_4L1J10',
        'b10_4L1J10',
        '2b15_3L1J15',
        '3b15_4L1J15',
        '2b15_3b10_4L1J10',
        '3b15_4L1J10',
        'b10_j55_j45_2j30_a4tc_EFFS',
        'b10_L1JE100', 'b10_L1JE140', 
        '2b10_L1JE100', '2b10_L1JE140', '2b10_L1_2J10J50',
        'b10_loose', 'b15_loose', 'b20_loose', 'b10_medium',
        'b10_tight',
        '2b15_loose_3L1J15', '2b20_loose_3L1J20', '2b10_loose_4L1J10', 
        '3b10_loose_4L1J10', '3b10_loose_4L1J10',
        '3b15_loose_4L1J15',
        '2b10_medium_3L1J10', '2b10_medium_L1JE140',
        'b10_tight_4L1J10', 'b10_tight_L1JE140',
        'b10_medium_4L1J10',
        
        #MultiBjet chains
        '3b10_loose_4L1J10',
        '2b10_medium_3L1J10',
        '2b15_medium_2L1J10',
        '2b15_medium_3L1J15',
        '2b20_medium_3L1J20',
        '2b20_medium_L1_2J15J50',
        '2b10_medium_4L1J10',
        '2b10_tight_4L1J10',
        '2b10_medium_L1JE100',
        '2b10_medium_L1JE140',
        '2b10_medium_L1_2J10J50',
        '2b15_medium_4L1J15',
        '3b15_loose_4L1J15',
        '3b10_medium_4L1J10',
        '2b10_medium_4j30_a4tc_EFFS',
        '2b10_tight_4j30_a4tc_EFFS', 
        '2b15_medium_4j45_a4tc_EFFS',
        '2b15_medium_j75_j40_a4tc_EFFS_ht350',
        '3b10_medium_4j30_a4tc_EFFS',
        '2b20_medium_3j45_a4tc_EFFS',
        '3b15_medium_4j40_a4tc_EFFS',
        '2b15_medium_3j40_a4tc_EFFS',    
        '2b10_medium_j75_j30_a4tc_EFFS', 
        '2b10_medium_j75_2j30_a4tc_EFFS',
        '2b10_medium_j75_j45_a4tc_EFFS', 
        '2b10_medium_4j40_a4tc_EFFS',    
        '2b20_medium_j75_j45_a4tc_EFFS', 
        '2b15_medium_j75_j40_a4tc_EFFS', 
        '2b10_medium_j100_j30_a4tc_EFFS',

        'mu22_j30_a4tc_EFFS',
        ])
    
    StoredValue = [
        '3b10_loose_4L1J10', '3b15_loose_4L1J15',
        '2b15_medium_3L1J15', '2b20_medium_3L1J20',
        '2b10_medium_4L1J10', 
        '2b10_medium_L1JE140',
        'b10_tight_4L1J10', 'b10_tight_L1JE140',
        '2b10_medium_L1JE100','2b10_medium_L1_2J10J50',
        'b10_IDTrkNoCut',
        'b10_EFj10_a4tc_EFFS_IDTrkNoCut',
        'b10_medium_4j30_a4tc_EFFS',
        ]

_flags.append(signatures)



# create container
class BjetSlice(JobPropertyContainer, CommonSliceHelper):
    """ Bjet Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(BjetSlice)

# add add common slice flags
TriggerFlags.BjetSlice.import_JobProperties('TriggerMenuPython.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.BjetSlice.add_JobProperty(flag)
del _flags

# make an alias
BjetSliceFlags = TriggerFlags.BjetSlice

