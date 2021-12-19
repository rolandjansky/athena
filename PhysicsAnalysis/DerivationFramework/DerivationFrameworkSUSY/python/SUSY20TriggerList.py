# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from SUSYCommonTriggerList import SingleLepton_2018,SingleLepton_2017,SingleLepton_2016,SingleLepton_2015
from SUSYCommonTriggerList import SinglePhoton_2018,SinglePhoton_2017,SinglePhoton_2016,SinglePhoton_2015

triggersSingleLepton = SingleLepton_2018 + SingleLepton_2017 + SingleLepton_2016 + SingleLepton_2015
triggersPhoton = SinglePhoton_2018 + SinglePhoton_2017 + SinglePhoton_2016 + SinglePhoton_2015

from SUSYCommonTriggerList import MET_2018,MET_2017,MET_2016,MET_2015

# start with common list for MET triggers
triggersMET = MET_2015 + MET_2016 + MET_2017 + MET_2018

triggersMET += [
# these were unprescaled in 2015, in addition to the primary HLT_xe70_mht from
# the common list
'HLT_xe70',
'HLT_xe70_tc_lcw',
# useful for cross-checks
'HLT_noalg_L1J400',
'HLT_noalg_L1J420',
'HLT_noalg_L1J450',
]

triggersNavThin=triggersMET+triggersSingleLepton+triggersPhoton
