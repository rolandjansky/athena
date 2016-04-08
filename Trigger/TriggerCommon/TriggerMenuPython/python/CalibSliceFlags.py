# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Calib slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper, AllowedList

__author__  = ''
__version__="$Revision: 1.4 $"
__doc__="Calib slice specific flags  "

_flags = [] 
class doTrackingApproach(JobProperty):
    """ Use tracking approach in Calib Trigger """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
    
_flags.append(doTrackingApproach)

class signatures(JobProperty):
    """ signatures in Calib slice """
    statusOn=True
    allowedTypes = ['list', 'str']
    allowedValues= AllowedList([
        ##LAr items
        'g3_larcalib', 'g3_larcalibV1',
        'g5_larcalib','g10_larcalib',
        'g11_larcalib', 'g12_larcalib', #'g15_larcalib',
        'g20_larcalib','g40_larcalib',
        'g50_larcalib', 'g60_larcalib', 'g80_larcalib', 'g20i_larcalib',
        'g25_larcalib', 'g25i_larcalib',
        'j25_larcalib', 'j50_larcalib', 'j95_larcalib','j200_larcalib',
        'fj25_larcalib', 'fj50_larcalib',
        'g10_larcalib_rejAll',
        'em3_empty_larcalib','em5_empty_larcalib', 'em6_empty_larcalib',
        'j5_empty_larcalib', 'tau5_empty_larcalib','tau8_empty_larcalib',
        'j10_empty_larcalib', 'fj10_empty_larcalib', 'fj30_empty_larcalib',
        'fj0_empty_larcalib', 'fj1_empty_larcalib', 'fj5_empty_larcalib',
        'j10_larcalib','j80_larcalib',
        'fj3_larcalib','fj5_larcalib',
        'fj18_larcalib','fj15_larcalib',
        'fj30_larcalib', 'fj50_larcalib',
        'j20_larcalib', 'j50_larcalib', 'j95_larcalib',
        #'fj30_c4cchad_larcalib',
        #E.P.
        'fj30_larcalib',

        'fj75_c4cchad_larcalib',
        'fj140_c4cchad_larcalib',
        'j40_c4cchad_larcalib',
        'j165_c4cchad_larcalib',
        'L1FJ95_NoAlg_larcalib',
        'L1J115_NoAlg_larcalib',
        'fj30_jetNoCut_larcalib', 'fj50_jetNoCut_larcalib',
        'j20_jetNoCut_larcalib', 'j50_jetNoCut_larcalib', 'j95_jetNoCut_larcalib',
        'fj30_jetNoEF_larcalib', 'fj50_jetNoEF_larcalib',
        'j20_jetNoEF_larcalib', 'j50_jetNoEF_larcalib', 'j95_jetNoEF_larcalib',
        'j115_jetNoCut_larcalib','fj95_jetNoCut_larcalib',
        ## Tile items
        'hadCalib_trk9','hadCalib_trk9phi','hadCalib_trk18',
        'hadCalib_trk9_L1HA8',
        'hadCalib_trk18_L1HA8',
        'hadCalib_trk9_L1RD0',
        'hadCalib_trk18_L1RD0',
        ## ID items
        'trk9_Central_Tau_IDCalib',
        'trk9_Fwd_Tau_IDCalib',
        'trk16_Central_Tau_IDCalib',
        'trk16_Fwd_Tau_IDCalib',
        'trk29_Central_Tau_IDCalib',
        'trk29_Fwd_Tau_IDCalib',
        ##ALFA items 
        'ALFA_Calib','ALFA_Phys','ALFA_Phys_Any',
        ##Hi items
        'j15_larcalib_hi', 'j20_larcalib_hi',
        'fj20_larcalib_hi',
        ])
    StoredValue  = ['g3_larcalib','g10_larcalib','g20_larcalib','g20i_larcalib', 'j25_larcalib', 'j50_larcalib', 'j95_larcalib', 'fj25_larcalib', 'fj50_larcalib' ]
_flags.append(signatures)


# create container
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper

class CalibSlice(JobPropertyContainer, CommonSliceHelper):
    """ Calib Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(CalibSlice)

# add add common slice flags
TriggerFlags.CalibSlice.import_JobProperties('TriggerMenuPython.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.CalibSlice.add_JobProperty(flag)
del _flags

# make an alias
CalibSliceFlags = TriggerFlags.CalibSlice
