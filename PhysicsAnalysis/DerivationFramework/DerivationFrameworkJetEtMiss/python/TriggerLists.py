# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from TriggerMenu.api.TriggerAPI import TriggerAPI
from TriggerMenu.api.TriggerEnums import TriggerPeriod, TriggerType

# jet triggers (single Jets, prescaled and unprescaled)
def single_jetTrig():
	jetTrig = TriggerAPI.getAllHLT(TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017, TriggerType.j_single)
	API_jetTrig = list(jetTrig.keys())
	return API_jetTrig
# electron triggers (unprescaled)
def single_el_Trig():
	singleElTriggers = TriggerAPI.getUnprescaled(TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017, TriggerType.el_single)
	API_singleElTriggers = list(singleElTriggers.keys())
	return API_singleElTriggers

def multi_el_Trig():
	multiElTriggers = TriggerAPI.getUnprescaled(TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017, TriggerType.el_multi)
	API_multiElTriggers = list(multiElTriggers.keys())
	return API_multiElTriggers

# single muon triggers (unprescaled)
def single_mu_Trig():
	singleMuTriggers = TriggerAPI.getUnprescaled(TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017, TriggerType.mu_single)
	API_singleMuTriggers = list(singleMuTriggers.keys())
	return API_singleMuTriggers

def multi_mu_Trig():
	multiMuTriggers = TriggerAPI.getUnprescaled(TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017, TriggerType.mu_multi)
	API_multiMuTriggers = list(multiMuTriggers.keys())
	return API_multiMuTriggers

# xe triggers (prescaled and unprescaled)
def MET_Trig():
	MET_Triggers = TriggerAPI.getAllHLT(TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017, TriggerType.xe)
	API_MET_Triggers = list(MET_Triggers,keys())
	return API_MET_Triggers

# photon triggers (prescaled and unprescaled)
def single_photon_Trig():
	singlePhotonTriggers = TriggerAPI.getAllHLT(TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017, TriggerType.g_single)
	API_singlePhotonTriggers = list(singlePhotonTriggers.keys())
	return API_singlePhotonTriggers

def multi_photon_Trig():
	multiPhotonTriggers = TriggerAPI.getAllHLT(TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017, TriggerType.g_multi)
	API_multiPhotonTriggers = list(multiPhotonTriggers.keys())
	return API_multiPhotonTriggers
