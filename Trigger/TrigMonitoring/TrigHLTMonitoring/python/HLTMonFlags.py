# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## HLTFlags... taken from DQMonFlags ..

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

## define jobproperty classes and put them in a list
list=[]

class doMonTier0(JobProperty):
	""" Global switch for monitoring """
	statusOn=True
	allowedTypes=['bool']
	StoredValue=True
list+=[doMonTier0]

class doGeneral(JobProperty):
	""" switch for general HLTMon Tool"""
	statusOn=True
	allowedTypes=['bool']
	StoredValue=True
list+=[doGeneral]

class doEgamma(JobProperty):
	""" Egamma switch for monitoring """
	statusOn=True
	allowedTypes=['bool']
	StoredValue=True
list+=[doEgamma]

class doCalo(JobProperty):
	""" Calo switch for monitoring """
	statusOn=True
	allowedTypes=['bool']
	StoredValue=True
list+=[doCalo]

class doMuon(JobProperty):
	""" Muon switch for monitoring """
	statusOn=True
	allowedTypes=['bool']
	StoredValue=True
list+=[doMuon]

class doIDtrk(JobProperty):
	""" ID track switch for monitoring """
	statusOn=True
	allowedTypes=['bool']
	StoredValue=True
list+=[doIDtrk]

class doMET(JobProperty):
	""" MET switch for monitoring """
	statusOn=True
	allowedTypes=['bool']
	StoredValue=True
list+=[doMET]

class doTau(JobProperty):
	""" Tau switch for monitoring """
	statusOn=True
	allowedTypes=['bool']
	StoredValue=True
list+=[doTau]

class doJet(JobProperty):
	""" Jet switch for monitoring """
	statusOn=True
	allowedTypes=['bool']
	StoredValue=True
list+=[doJet]

class doBphys(JobProperty):
	""" Bphys switch for monitoring """
	statusOn=True
	allowedTypes=['bool']
	StoredValue=True
list+=[doBphys]

class doBjet(JobProperty):
	""" Bjet switch for monitoring """
	statusOn=True
	allowedTypes=['bool']
	StoredValue=True
list+=[doBjet]

class doMinBias(JobProperty):
	""" Minbias switch for monitoring """
	statusOn=True
	allowedTypes=['bool']
	StoredValue=True
list+=[doMinBias]

class doIDJpsiMon(JobProperty):
	""" ID JPsi switch for Monitoring """
	statusOn=True
	allowdTypes=['bool']
	StoredValue=True
list+=[doIDJpsiMon]

class doDump(JobProperty):
	""" DumpTools switch for monitoring """
	statusOn=False
	allowedTypes=['bool']
	StoredValue=False
list+=[doDump]

class doOfflineTauTTP(JobProperty):
	""" TauDump switch for monitoring """
	statusOn=False
	allowedTypes=['bool']
	StoredValue=False
list+=[doOfflineTauTTP]

#create a JobProperty container
class HLTMonFlagsCont(JobPropertyContainer):
	"""Container for HLT Monitoring FLags
	"""
	pass

jobproperties.add_Container(HLTMonFlagsCont)

#loop over list and put classes in Container

for j in list:
	jobproperties.HLTMonFlagsCont.add_JobProperty(j)

#create alias
HLTMonFlags = jobproperties.HLTMonFlagsCont
