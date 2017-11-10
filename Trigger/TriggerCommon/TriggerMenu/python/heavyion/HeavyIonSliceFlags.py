# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" HeavyIon slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from TriggerMenu.menu.CommonSliceHelper import CommonSliceHelper

__author__  = 'R. Kwee, B. Demirkoz, T.Bold, T.Kohno, W. H. Bell'
__version__="$Revision: 1.18 $"
__doc__="Minimum Bias slice specific flags  "

#
# This file should not be modified without consulting 
# the Heavy Ion Trigger Menu Forum coordinator.
#
# The HITMF trigger slice is documented at:
# https://twiki.cern.ch/twiki/bin/view/Atlas/HeavyIonTriggerMenuForum
#

#===================================================================

_flags = [] 

class signatures(JobProperty):
    """ signatures in HeavyIon slice """
    statusOn=True
    allowedTypes=['list']

    StoredValue   = [
        ]


    
    
_flags.append(signatures)

# create container
class HeavyIonSlice(JobPropertyContainer, CommonSliceHelper):
    """ HeavyIon Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(HeavyIonSlice)

# add add common slice flags
TriggerFlags.HeavyIonSlice.import_JobProperties('TriggerMenu.menu.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.HeavyIonSlice.add_JobProperty(flag)
del _flags

# make an alias
HeavyIonSliceFlags = TriggerFlags.HeavyIonSlice

# add HeavyIon algs cuts as sub-container

#from TrigT2MinBias.TrigT2MinBiasProperties import TrigT2MinBiasProperties
#from TrigMinBias.TrigMinBiasProperties import TrigMinBiasProperties
