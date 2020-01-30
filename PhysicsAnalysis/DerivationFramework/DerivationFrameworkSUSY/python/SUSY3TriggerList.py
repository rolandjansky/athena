# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from DerivationFrameworkSUSY.SUSYCommonTriggerList import SingleLepton_2015,SingleLepton_2016,SingleLepton_2017,SingleLepton_2018

triggerRegEx = [
    'HLT.*_tau[0-9][0-9].*_medium.*',
    'HLT.*_j.*_xe.*', 'HLT_xe[0-9][0-9].*', 'HLT_noalg_L1J400',
]

triggerRegEx += SingleLepton_2015 + SingleLepton_2016 + SingleLepton_2017 + SingleLepton_2018

# add some more trigger navigation for efficiency measurements
triggerRegExThinning = triggerRegEx + ['HLT_tau[0-9][0-9].*perf.*'] + [ 'L1_TAU.*' ]
