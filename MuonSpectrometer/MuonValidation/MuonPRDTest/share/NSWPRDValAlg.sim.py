# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# jobOptions to activate the dump of the NSWPRDValAlg nTuple
# This file can be used with Sim_tf by specifying --postInclude MuonPRDTest/NSWPRDValAlg.sim.py
# It dumps Truth, MuEntry and Hits, for all muon technologies

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
NSWPRDValAlg.doMDTHit= True
NSWPRDValAlg.doRPCHit= True
NSWPRDValAlg.doTGCHit= True
NSWPRDValAlg.doCSCHit= MuonGeometryFlags.hasCSC()
NSWPRDValAlg.doMMHit = MuonGeometryFlags.hasMM()
NSWPRDValAlg.doSTGCHit = MuonGeometryFlags.hasSTGC()


#-----------------------------------------------------------------------------
# save ROOT histograms and Tuple
#-----------------------------------------------------------------------------
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = [ "NSWPRDValAlg DATAFILE='NSWPRDValAlg.sim.ntuple.root' OPT='RECREATE'" ]
