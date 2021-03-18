# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# jobOptions to activate the dump of the NSWPRDValAlg nTuple
# This file can be used with Digi_tf by specifying --postInclude MuonPRDTest/NSWPRDValAlg.digi.py
# It dumps Truth, MuEntry and Hits, Digits, SDOs and RDOs for MM and sTGC

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

from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
NSWPRDValAlg.doMMHit = MuonGeometryFlags.hasMM()
NSWPRDValAlg.doMMDigit = MuonGeometryFlags.hasMM()
NSWPRDValAlg.doMMRDO = MuonGeometryFlags.hasMM()
NSWPRDValAlg.doMMPRD = False
NSWPRDValAlg.doMMFastDigit = False

NSWPRDValAlg.doSTGCHit = MuonGeometryFlags.hasSTGC()
NSWPRDValAlg.doSTGCDigit = MuonGeometryFlags.hasSTGC()
NSWPRDValAlg.doSTGCRDO = MuonGeometryFlags.hasSTGC()
NSWPRDValAlg.doSTGCPRD = False
NSWPRDValAlg.doSTGCFastDigit = False

NSWPRDValAlg.doRPCHit = True
NSWPRDValAlg.doRPCDigit = True

NSWPRDValAlg.doMDTHit = True
NSWPRDValAlg.doMDTDigit = True

#-----------------------------------------------------------------------------
# save ROOT histograms and Tuple
#-----------------------------------------------------------------------------
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = [ "NSWPRDValAlg DATAFILE='NSWPRDValAlg.digi.ntuple.root' OPT='RECREATE'" ]
