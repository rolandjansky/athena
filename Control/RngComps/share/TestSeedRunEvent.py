##@file TestSetSeedRunEvent.py
# @brief test SetSeedRunEventSvc
# @author Paolo Calafiura
#==============================================================
from AthenaCommon.AppMgr import ServiceMgr
from AthenaServices.AthenaServicesConf import TestRandomSeqAlg
from RngComps.RngCompsConf import AtDSFMTGenSvc, AtRanluxGenSvc, AtRndmGenSvc

atMars = AtDSFMTGenSvc()
atMars.OutputLevel=VERBOSE
atMars.EventReseeding=True
atMars.Seeds = [ "MARS 1 2", "MARSOFF OFFSET 73 1 2", "MARS_INIT 3 4", "TEST 5 6"];
ServiceMgr += atMars

atLux = AtRanluxGenSvc()
atLux.OutputLevel=VERBOSE
atLux.UseOldBrokenSeeding=False
atLux.EventReseeding=True
atLux.Seeds = [ "LUX 1 2", "LUXOFF OFFSET 73 1 2", "LUX_INIT LUXURY 0 3 4", "TEST 5 6"];
ServiceMgr += atLux

atRnd = AtRndmGenSvc()
atRnd.OutputLevel=VERBOSE
atRnd.UseOldBrokenSeeding=False
atRnd.EventReseeding=True
atRnd.ReseedStreamNames = [ "TEST", "RNDM", "RNDMOFF" ]
atRnd.Seeds = [ "RNDM 11 12", "RNDMOFF OFFSET 73 11 12", "RNDM_INIT 13 14", "TEST 15 16"];
ServiceMgr += atRnd

atLuxNN = AtRanluxGenSvc("LuxOld")
atLuxNN.OutputLevel=VERBOSE
atLuxNN.UseOldBrokenSeeding=True
atLuxNN.EventReseeding=False
atLuxNN.Seeds = [ "LUX 1 2", "LUX_INIT LUXURY 0 3 4", "TEST 5 6"];
ServiceMgr += atLuxNN

atRndOR = AtRndmGenSvc("RndNoReseed")
atRndOR.OutputLevel=VERBOSE
atRndOR.UseOldBrokenSeeding=False
atRndOR.EventReseeding=False
atRndOR.Seeds = [ "RNDM 11 12", "RNDM_INIT 13 14", "TEST 15 16"];
ServiceMgr += atRndOR

from AthenaCommon.AppMgr import theApp
theApp.EvtMax=10



from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

job += TestRandomSeqAlg("AtRanTestAlg", RndmSvc="AtRanluxGenSvc", OutputLevel=DEBUG)
job += TestRandomSeqAlg("AtLuxWithOffTestAlg", RndmSvc="AtRanluxGenSvc", StreamName="LUXOFF", OutputLevel=DEBUG)
job += TestRandomSeqAlg("AtLuxNoOffTestAlg", RndmSvc="AtRanluxGenSvc", StreamName="LUX", OutputLevel=DEBUG)
job += TestRandomSeqAlg("AtRndTestAlg", RndmSvc="AtRndmGenSvc", OutputLevel=DEBUG)
job += TestRandomSeqAlg("AtWithOffTestAlg", RndmSvc="AtRndmGenSvc", StreamName="RNDMOFF", OutputLevel=DEBUG)
job += TestRandomSeqAlg("AtNoOffTestAlg", RndmSvc="AtRndmGenSvc", StreamName="RNDM", OutputLevel=DEBUG)
job += TestRandomSeqAlg("OldTestAlg", RndmSvc="LuxOld", OutputLevel=DEBUG)
job += TestRandomSeqAlg("NoReseedTestAlg", RndmSvc="RndNoReseed", OutputLevel=DEBUG)
job += TestRandomSeqAlg("MarsTestAlg", RndmSvc="AtDSFMTGenSvc", OutputLevel=DEBUG)
