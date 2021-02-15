# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# jobOptions to activate the dump of the NSWPRDValAlg nTuple
# This file can be used with Reco_tf by specifying --postInclude MuonPRDTest/NSWPRDValAlg.reco.py
# It dumps Truth, MuEntry and Hits, Digits, SDOs, RDOs and PRDs for MM and sTGC
# It should only be used with Reco since the alg crashes in the absence of PRDs

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

from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
NSWPRDValAlg.doMMHit = MuonGeometryFlags.hasMM()
NSWPRDValAlg.doMMDigit = MuonGeometryFlags.hasMM()
NSWPRDValAlg.doMMRDO = MuonGeometryFlags.hasMM()
NSWPRDValAlg.doMMPRD = MuonGeometryFlags.hasMM()
NSWPRDValAlg.doMMFastDigit = False

NSWPRDValAlg.doSTGCHit = MuonGeometryFlags.hasSTGC()
NSWPRDValAlg.doSTGCDigit = MuonGeometryFlags.hasSTGC()
NSWPRDValAlg.doSTGCRDO = MuonGeometryFlags.hasSTGC()
NSWPRDValAlg.doSTGCPRD = MuonGeometryFlags.hasSTGC()
NSWPRDValAlg.doSTGCFastDigit = False

#-----------------------------------------------------------------------------
# save ROOT histograms and Tuple
#-----------------------------------------------------------------------------
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = [ "NSWPRDValAlg DATAFILE='NSWPRDValAlg.reco.ntuple.root' OPT='RECREATE'" ]
