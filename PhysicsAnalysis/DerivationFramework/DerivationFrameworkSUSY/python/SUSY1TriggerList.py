# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

################################################################ 
#
# the trigger list below is created as of Oct23, 2017 based on Twiki pages:
# https://twiki.cern.ch/twiki/bin/viewauth/Atlas/LowestUnprescaled
# https://twiki.cern.ch/twiki/bin/view/Atlas/L34TriggerMenu2017
#
################################################################ 


from DerivationFrameworkSUSY.SUSYCommonTriggerList import *

SUSY1LeptonTriggers = SingleLepton_2015+SingleLepton_2016+SingleLepton_2017+SingleLepton_2018+DiLepton_2015+DiLepton_2016+DiLepton_2017+DiLepton_2018
SUSY1PhotonTriggers = SinglePhoton_2015+SinglePhoton_2016+SinglePhoton_2017+SinglePhoton_2018
SUSY1XETriggers = MET_2015+MET_2016+MET_2017+MET_2018
SUSY1BJetTriggers = MultiBJet_2015+MultiBJet_2016+MultiBJet_2017+MultiBJet_2018

#all triggers
SUSY1AllTriggers = SUSY1LeptonTriggers + SUSY1PhotonTriggers + SUSY1XETriggers + JetTrigger_2016 + SUSY1BJetTriggers

#just those we need the navigation thinning for
SUSY1ThinTriggers = SUSY1LeptonTriggers + SUSY1PhotonTriggers + SUSY1BJetTriggers

