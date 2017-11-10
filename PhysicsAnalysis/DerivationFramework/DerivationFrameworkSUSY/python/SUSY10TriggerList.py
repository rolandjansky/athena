# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

################################################################ 
#
# the trigger list below is created as of Oct23, 2017 based on Twiki pages:
# https://twiki.cern.ch/twiki/bin/viewauth/Atlas/LowestUnprescaled
# https://twiki.cern.ch/twiki/bin/view/Atlas/L34TriggerMenu2017
#
################################################################ 

#get all common menu items from the common list
from SUSYCommonTriggerList import *

SUSY10LeptonTriggers = SingleLepton_2015+SingleLepton_2016+SingleLepton_2017+DiLepton_2015+DiLepton_2016+DiLepton_2017
SUSY10PhotonTriggers = SinglePhoton_2015+SinglePhoton_2016+SinglePhoton_2017
SUSY10XETriggers = MET_2015+MET_2016+MET_2017

#all triggers
SUSY10AllTriggers = SUSY10LeptonTriggers + SUSY10PhotonTriggers + SUSY10XETriggers + JetTrigger_2016# + SUSY10CombinedXETriggers + SUSY10BjetTriggers + SUSY10RazorTriggers + SUSY10JetTriggers

#just those we need the navigation thinning for
SUSY10ThinTriggers = SUSY10LeptonTriggers + SUSY10PhotonTriggers

