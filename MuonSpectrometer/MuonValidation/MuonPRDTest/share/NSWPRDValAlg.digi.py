#job options to activate the dump of the NSWPRDVal nTuple
#This file can be used with Digi_tf by specifying --postinclude MuonPRDTest/NSWPRDValAlg.digi.py
#It dumps Truth, MuEntry and Hits, Digits, SDOs and RDOs for MM and sTGC


#-----------------------------------------------------------------------------
# Algorithms
#-----------------------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
from MuonPRDTest.MuonPRDTestConf import *

job+=NSWPRDValAlg('NSWPRDValAlg', OutputLevel = WARNING)
NSWPRDValAlg.OutputLevel = INFO
NSWPRDValAlg.doTruth = True

NSWPRDValAlg.doMuEntry = True

NSWPRDValAlg.doMMHit = True
NSWPRDValAlg.doMMDigit = True
NSWPRDValAlg.doMMRDO = True
NSWPRDValAlg.doMMPRD = False
NSWPRDValAlg.doMMFastDigit = False

NSWPRDValAlg.doSTGCHit = True
NSWPRDValAlg.doSTGCDigit = True
NSWPRDValAlg.doSTGCRDO = True
NSWPRDValAlg.doSTGCPRD = False
NSWPRDValAlg.doSTGCFastDigit = False

#-----------------------------------------------------------------------------
# save ROOT histograms and Tuple
#-----------------------------------------------------------------------------
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = [ "NSWPRDValAlg DATAFILE='NSWPRDValAlg.digi.ntuple.root' OPT='RECREATE'" ]
