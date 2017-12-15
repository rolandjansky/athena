# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" MinBias slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from TriggerMenu.menu.CommonSliceHelper import CommonSliceHelper

__author__  = 'R. Kwee, B. Demirkoz, T.Bold, T.Kohno, W. H. Bell'
__version__="$Revision: 1.18 $"
__doc__="Minimum Bias slice specific flags  "

#
# This file should not be modified without consulting 
# the Minimum Bias trigger slice coordinator.
#
# The Minimum Bias trigger slice is documented at:
# https://twiki.cern.ch/twiki/bin/view/Atlas/MinbiasTrigger
#

#===================================================================

_flags = [] 

class signatures(JobProperty):
    """ signatures in MinBias slice """
    statusOn=True
    allowedTypes=['list']

    StoredValue   = [
        ]


    
    
_flags.append(signatures)

# create container
class MinBiasSlice(JobPropertyContainer, CommonSliceHelper):
    """ MinBias Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(MinBiasSlice)

# add add common slice flags
TriggerFlags.MinBiasSlice.import_JobProperties('TriggerMenu.menu.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.MinBiasSlice.add_JobProperty(flag)
del _flags

# make an alias
MinBiasSliceFlags = TriggerFlags.MinBiasSlice

# add MinBias algs cuts as sub-container

#from TrigT2MinBias.TrigT2MinBiasProperties import TrigT2MinBiasProperties
#from TrigMinBias.TrigMinBiasProperties import TrigMinBiasProperties
