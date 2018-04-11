# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from TriggerMenu.api.TriggerAPI import TriggerAPI
from TriggerMenu.api.TriggerEnums import TriggerPeriod, TriggerType

# jet triggers (single Jets, prescaled and unprescaled)
def single_jetTrig():
	jetTrig = TriggerAPI.getAllHLT(TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017 | TriggerPeriod.future, TriggerType.j_single)
	API_jetTrig = list(jetTrig.keys())
	return API_jetTrig
# electron triggers (unprescaled)
def single_el_Trig():
	API_singleElTriggers = TriggerAPI.getUnprescaled(TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017 | TriggerPeriod.future, TriggerType.el_single)
	return API_singleElTriggers

def multi_el_Trig():
	API_multiElTriggers = TriggerAPI.getUnprescaled(TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017 | TriggerPeriod.future, TriggerType.el_multi)
	return API_multiElTriggers

# single muon triggers (unprescaled)
def single_mu_Trig():
	API_singleMuTriggers = TriggerAPI.getUnprescaled(TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017 | TriggerPeriod.future, TriggerType.mu_single)
	return API_singleMuTriggers

def multi_mu_Trig():
	API_multiMuTriggers = TriggerAPI.getUnprescaled(TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017 | TriggerPeriod.future, TriggerType.mu_multi)
	return API_multiMuTriggers

# xe triggers (prescaled and unprescaled)
def MET_Trig():
	MET_Triggers = TriggerAPI.getAllHLT(TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017 | TriggerPeriod.future, TriggerType.xe)
	API_MET_Triggers = list(MET_Triggers,keys())
	return API_MET_Triggers

# photon triggers (prescaled and unprescaled)
def single_photon_Trig():
	singlePhotonTriggers = TriggerAPI.getAllHLT(TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017 | TriggerPeriod.future, TriggerType.g_single)
	API_singlePhotonTriggers = list(singlePhotonTriggers.keys())
	return API_singlePhotonTriggers

def multi_photon_Trig():
	multiPhotonTriggers = TriggerAPI.getAllHLT(TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017 | TriggerPeriod.future, TriggerType.g_multi)
	API_multiPhotonTriggers = list(multiPhotonTriggers.keys())
	return API_multiPhotonTriggers
