# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from TriggerMenu.api.TriggerAPI import TriggerAPI
from TriggerMenu.api.TriggerEnums import TriggerPeriod, TriggerType

# jet triggers (single Jets, prescaled and unprescaled)
def jetTrig():
	period2015tofuture = TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017 | TriggerPeriod.y2018 | TriggerPeriod.future
	period2017tofuture = TriggerPeriod.y2017 | TriggerPeriod.y2018 | TriggerPeriod.future
	API_jetTrig = TriggerAPI.getActive(period2015tofuture, TriggerType.j_single)
	# Large-radius multijet triggers
	API_jetTrig += TriggerAPI.getActive(period2017tofuture,TriggerType.j_multi,matchPattern=".*_a10t_.*")
	# Eta-intercalibration triggers (one central, one forward jet)
	API_jetTrig += TriggerAPI.getActive(period2015tofuture,TriggerType.j_multi,matchPattern="HLT_j.*_320eta490")
	return API_jetTrig

# electron triggers (unprescaled)
def single_el_Trig():
	API_singleElTriggers = TriggerAPI.getLowestUnprescaled(TriggerPeriod.y2015, TriggerType.el_single) + TriggerAPI.getLowestUnprescaled(TriggerPeriod.y2016, TriggerType.el_single) + TriggerAPI.getLowestUnprescaled(TriggerPeriod.y2017, TriggerType.el_single) + TriggerAPI.getLowestUnprescaled(TriggerPeriod.y2018, TriggerType.el_single) + TriggerAPI.getLowestUnprescaled(TriggerPeriod.future, TriggerType.el_single)
	return API_singleElTriggers

def multi_el_Trig():
	API_multiElTriggers = TriggerAPI.getLowestUnprescaled(TriggerPeriod.y2015, TriggerType.el_multi) + TriggerAPI.getLowestUnprescaled(TriggerPeriod.y2016, TriggerType.el_multi) + TriggerAPI.getLowestUnprescaled(TriggerPeriod.y2017, TriggerType.el_multi) + TriggerAPI.getLowestUnprescaled(TriggerPeriod.y2018, TriggerType.el_multi) + TriggerAPI.getLowestUnprescaled(TriggerPeriod.future, TriggerType.el_multi)
	return API_multiElTriggers

# single muon triggers (unprescaled)
def single_mu_Trig():
	API_singleMuTriggers = TriggerAPI.getLowestUnprescaled(TriggerPeriod.y2015, TriggerType.mu_single) + TriggerAPI.getLowestUnprescaled(TriggerPeriod.y2016, TriggerType.mu_single) + TriggerAPI.getLowestUnprescaled(TriggerPeriod.y2017, TriggerType.mu_single) + TriggerAPI.getLowestUnprescaled(TriggerPeriod.y2018, TriggerType.mu_single) + TriggerAPI.getLowestUnprescaled(TriggerPeriod.future, TriggerType.mu_single)
	return API_singleMuTriggers

def multi_mu_Trig():
	API_multiMuTriggers = TriggerAPI.getLowestUnprescaled(TriggerPeriod.y2015, TriggerType.mu_multi) + TriggerAPI.getLowestUnprescaled(TriggerPeriod.y2016, TriggerType.mu_multi) + TriggerAPI.getLowestUnprescaled(TriggerPeriod.y2017, TriggerType.mu_multi) + TriggerAPI.getLowestUnprescaled(TriggerPeriod.y2018, TriggerType.mu_multi) + TriggerAPI.getLowestUnprescaled(TriggerPeriod.future, TriggerType.mu_multi)
	return API_multiMuTriggers

# xe triggers (unprescaled)
def MET_Trig():
	API_MET_Triggers = TriggerAPI.getLowestUnprescaled(TriggerPeriod.y2015, TriggerType.xe) + TriggerAPI.getLowestUnprescaled(TriggerPeriod.y2016, TriggerType.xe) + TriggerAPI.getLowestUnprescaled(TriggerPeriod.y2017, TriggerType.xe) + TriggerAPI.getLowestUnprescaled(TriggerPeriod.y2018, TriggerType.xe) + TriggerAPI.getLowestUnprescaled(TriggerPeriod.future, TriggerType.xe)
	return API_MET_Triggers

# photon triggers (prescaled and unprescaled)
def single_photon_Trig():
	API_singlePhotonTriggers = TriggerAPI.getActive(TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017 | TriggerPeriod.y2018 | TriggerPeriod.future, TriggerType.g_single)
	return API_singlePhotonTriggers

