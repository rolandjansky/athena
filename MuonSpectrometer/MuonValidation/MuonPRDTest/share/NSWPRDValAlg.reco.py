#job options to activate the dump of the NSWPRDVal nTuple
#This file can be used with Reco_tf by specifying --postinclude MuonPRDTest/NSWPRDValAlg.reco.py
#It dumps Truth, MuEntry and Hits, Digits, SDOs, RDOs and PRDs for MM and sTGC
#It should only be used with Reco since the alg crashes in the absence of PRDs


#-----------------------------------------------------------------------------
# Algorithms
#-----------------------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
from MuonPRDTest.MuonPRDTestConf import *

job+=NSWPRDValAlg('NSWPRDValAlg', OutputLevel = DEBUG)
NSWPRDValAlg.OutputLevel = INFO
NSWPRDValAlg.doTruth = True

NSWPRDValAlg.doMuEntry = True

NSWPRDValAlg.doMMHit = True
NSWPRDValAlg.doMMDigit = True
NSWPRDValAlg.doMMRDO = True
NSWPRDValAlg.doMMPRD = True
NSWPRDValAlg.doMMFastDigit = False

NSWPRDValAlg.doSTGCHit = True
NSWPRDValAlg.doSTGCDigit = True
NSWPRDValAlg.doSTGCRDO = True
NSWPRDValAlg.doSTGCPRD = True
NSWPRDValAlg.doSTGCFastDigit = False

#-----------------------------------------------------------------------------
# save ROOT histograms and Tuple
#-----------------------------------------------------------------------------
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = [ "NSWPRDValAlg DATAFILE='NSWPRDValAlg.reco.ntuple.root' OPT='RECREATE'" ]
