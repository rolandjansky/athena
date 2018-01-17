###############################################################
#
#  iPatRecExample jobOptions file:
#  runs everything needed for iPatRec (Truth/InnerDetector etc)
#
#==============================================================
    
#--------------------------------------------------------------
# Needed for Configurables
#--------------------------------------------------------------
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# use auditors to write out a summary of the time spent
#--------------------------------------------------------------
from AthenaCommon.ConfigurableDb import getConfigurable
ServiceMgr.AuditorSvc   += getConfigurable("ChronoAuditor")()
#ServiceMgr.AuditorSvc   += getConfigurable("NameAuditor")()
theApp.AuditAlgorithms  =  True # FIXME necessary even to get CPU time
theApp.AuditServices    =  True
theApp.CreateSvc += ["FPEControlSvc"]

#--------------------------------------------------------------
# set the number of events to be processed
#--------------------------------------------------------------
theApp.EvtMax   = 100
 
#--------------------------------------------------------------
# basic configuration: flags (a la RecExCommon)
#--------------------------------------------------------------
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs

# readESD no longer supported
rec.readRDO     = True
    
makeESD         = False
makeNtuple      = False
            
#--------------------------------------------------------------
# select test data: mc12 physics or single muons +/- pileup 
#--------------------------------------------------------------
noPileUp          = True
#singleMu2         = False    # deleted from eos
singleMu3         = False
#singleMu4         = False    # deleted from eos
singleMu5         = False
singleMu10        = False
singleMu20        = False
singleMu50        = False
singleMu100       = False
singleMu500       = False
testJpsi          = False
testT1            = False
testZmumu         = False
testBug           = False

#singleMu3         = True
#singleMu5         = True
#singleMu10        = True
#singleMu100       = True
#singleMu500       = True
#noPileUp          = False
testJpsi          = True
#testT1            = True
#testZmumu         = True
#testZprime        = True
#testBug           = True

#--------------------------------------------------------------
# careful with noPileUp option (only single mu datasets available)
#--------------------------------------------------------------
if not (singleMu3 or singleMu5 or singleMu10 or singleMu50 or singleMu100 or singleMu500):
    noPileUp        = False
    
#--------------------------------------------------------------
# select layout 
#--------------------------------------------------------------
if singleMu3 or singleMu5 or singleMu10 or singleMu50 or singleMu100 or singleMu500:
    DetDescrVersion = "ATLAS-GEO-20-00-01"
    ConditionsTag   = "OFLCOND-MC12-SDR-06"
elif testJpsi:
    DetDescrVersion = "ATLAS-GEO-20-00-01"
    ConditionsTag   = "OFLCOND-MC12-SDR-06"  
elif testT1:
    DetDescrVersion = "ATLAS-GEO-20-00-01"
    ConditionsTag   = "OFLCOND-MC12-SDR-06"    
elif testZmumu or testZprime:
    DetDescrVersion = "ATLAS-GEO-20-00-01"
    ConditionsTag   = "OFLCOND-MC12-SDR-06"
elif testBug:
    DetDescrVersion = "ATLAS-GEO-14-00-00"
    ConditionsTag = 'OFLCOND-SDR-BS7T-02'
elif recFlags.readESD:
    DetDescrVersion = "ATLAS-GEO-10-00-01"
else:
    DetDescrVersion = "ATLAS-GEO-16-00-00"

from AthenaCommon.JobProperties import jobproperties
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = DetDescrVersion
globalflags.ConditionsTag   = ConditionsTag
print globalflags.DetDescrVersion(),globalflags.ConditionsTag()

#--------------------------------------------------------------
# select detectors to be used for reconstruction
# uncomment appropriate line to turn pixel, SCT and/or TRT off)
#--------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
if rec.readRDO:
    DetFlags.readRDOPool.ID_setOn()
#DetFlags.makeRIO.ID_setOn()

#--------------------------------------------------------------
# set appropriate configuration into AthenaCommon.globalflags
#--------------------------------------------------------------
if rec.readESD:
    globalflags.DataSource      = 'data'
    globalflags.InputFormat     = 'pool'
else:
    globalflags.DataSource      = 'geant4'
    globalflags.InputFormat     = 'pool'
    
print globalflags

#--------------------------------------------------------------
# Geomodel and MagneticField setup
#--------------------------------------------------------------
#DetFlags.detdescr.ID_setOn()
DetFlags.ID_setOn()
DetFlags.digitize.ID_setOff()
DetFlags.pileup.ID_setOff()
DetFlags.readRDOBS.ID_setOff()
DetFlags.readRIOBS.ID_setOff()
DetFlags.readRIOPool.ID_setOff()
DetFlags.simulate.ID_setOff()

#--------------------------------------------------------------
# select detectors to be used for reconstruction
# uncomment appropriate line to turn pixel, SCT and/or TRT off)
#--------------------------------------------------------------
#DetFlags.pixel_setOff()
#DetFlags.SCT_setOff()
#DetFlags.makeRIO.TRT_setOff()

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
ServiceMgr.IOVSvc.sortKeys = True
include( "BFieldAth/BFieldAth_jobOptions.py" )

# Beam Spot service
include( "InDetBeamSpotService/BeamCondSvc.py" )

#--------------------------------------------------------------
# particle property service
#--------------------------------------------------------------
include( "PartPropSvc/PartPropSvc.py" )

#--------------------------------------------------------------
# InnerDetector jobOptions fragment
#--------------------------------------------------------------
include( "iPatRecExample/InnerDetector_jobOptions.py" )

#--------------------------------------------------------------
# iPatRec jobOptions fragment(s)
#--------------------------------------------------------------
include( "iPatRecExample/iPatRec_jobOptions.py" )

from iPatRecAlgs.iPatRecAlgsConf import iPatShortTracks
#iPatShortTracks = iPatShortTracks( DetectorOption = 'pixel')  # pixel, sct, or trt
#topSequence += iPatShortTracks

if makeNtuple:
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()

    # this makes the iPatRec ntuple
    from iPatRecAlgs.iPatRecAlgsConf import iPatNtuple
    iPatNtuple = iPatNtuple()
    topSequence += iPatNtuple
    SeviceMgr.THistSvc.Output = [ "FILE1 DATAFILE='tuple1.root' OPT='NEW'" ]
     
#--------------------------------------------------------------
# Legacy Conversion
#--------------------------------------------------------------
if makeESD:
    include( "iPatRecExample/MakeESD_jobOptions.py" )

#--------------------------------------------------------------
# print flag selection 
#--------------------------------------------------------------
globalflags.print_JobProperties()
DetFlags.Print()

#--------------------------------------------------------------
# file selection for data from POOL
#--------------------------------------------------------------
if globalflags.InputFormat == 'pool':
    import AthenaPoolCnvSvc.ReadAthenaPool
    if testBug:
        from MuonCnvExample.MuonCnvFlags import muonCnvFlags
        muonCnvFlags.RpcCablingMode="new"
        ServiceMgr.EventSelector.InputCollections = \
                                                  [ "/afs/cern.ch/user/p/pop/scratch0/bug72496/g4digi.pool.root" ]
#    elif recFlags.readESD:
#        ServiceMgr.EventSelector.InputCollections = [ "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/det-muon/Bytestream2010/MuonBytestreamZmumu_rec16.0.X.ESD.pool.root" ]
    else:
        # careful when ignoring tag - this is dangerous !
        #GeoModelSvc.IgnoreTagDifference = True
        t = []
        if noPileUp:
            filseq = range(20)
            filseq = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]
        else:
            filseq = range(50)
            if singleMu3:
                filseq = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]
            elif singleMu5:
                filseq = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]
            elif singleMu10:
                filseq = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]
            elif singleMu100:
                filseq = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]
            elif singleMu500:
                filseq = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]
            elif testJpsi:
                filseq = [1,2,3,4,5]
            elif testT1:
                filseq = [1,2,3,4,5]
            elif testZmumu:
                filseq = [1,2,3,4,5]
            elif testZprime:
                filseq = [5,9,14,16,17] 

        for i in filseq:
            if noPileUp:
                if singleMu3:
                    s = "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/det-muon/ReferenceDatasets/Digitization/noPileUp/SingleMu_3GeV/group.phys-beauty.144580_005686.EXT0._"+"%05d"%(i)+".RDO.pool.root"
                elif singleMu5:
                    s = "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/det-muon/ReferenceDatasets/Digitization/noPileUp/SingleMu_5GeV/group.phys-beauty.144581_005688.EXT0._"+"%05d"%(i)+".RDO.pool.root"
                elif singleMu10:
                    s = "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/det-muon/ReferenceDatasets/Digitization/noPileUp/SingleMu_10GeV/group.phys-beauty.144582_005690.EXT0._"+"%05d"%(i)+".RDO.pool.root"
                elif singleMu50:
                    s = "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/det-muon/ReferenceDatasets/Digitization/noPileUp/SingleMu_50GeV/group.phys-beauty.144583_005692.EXT0._"+"%05d"%(i)+".RDO.pool.root"
                elif singleMu100:
                    s = "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/det-muon/ReferenceDatasets/Digitization/noPileUp/SingleMu_100GeV/group.phys-beauty.147613_005700.EXT0._"+"%05d"%(i)+".RDO.pool.root"
                elif singleMu500:
                    s = "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/det-muon/ReferenceDatasets/Digitization/noPileUp/SingleMu_500GeV/group.phys-beauty.144584_005694.EXT0._"+"%05d"%(i)+".RDO.pool.root"
            else:
                if singleMu10:
                    ## official mc12 with standard pileup 
                    s = "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/det-muon/ReferenceDatasets/Digitization/SingleMu_10GeV_e1717_s1609_s1611_r4076/RDO.01138971._"+"%06d"%(i)+".pool.root.1"
                elif singleMu20:
                    s = "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/det-muon/ReferenceDatasets/Digitization/noPileUp/SingleMu_10GeV/group.phys-beauty.144582_005690.EXT0._"+"%05d"%(i)+".RDO.pool.root"
                elif testJpsi:
                    s = "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/det-muon/valid/mc12_Jpsimu4mu4/RDO.01177876._"+"%06d"%(i)+".pool.root.1"
                elif testT1:
                    if (i == 1):
                        s = "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/det-muon/valid/mc12_T1/RDO.01177879._"+"%06d"%(i)+".pool.root.1"
                    else:
                        s = "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/det-muon/valid/mc12_T1/RDO.01177879._"+"%06d"%(i)+".pool.root.2"
                elif testZmumu:
                    s = "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/det-muon/valid/mc12_Zmumu/RDO.01177877._"+"%06d"%(i)+".pool.root.1"
                elif testZprime:
                    s = "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/det-muon/ReferenceDatasets/Digitization/mc12_8TeV.158028.Zprime_mumu_SSM2000.e1242_s1469_s1470_d700/RDO.01177878._"+"%06d"%(i)+".pool.root.1"

            t.append(s)
            
        print "input files: %s"%t
        ServiceMgr.EventSelector.InputCollections = t
        # from AthenaCommon.JobProperties import jobproperties
        jobproperties.AthenaCommonFlags.PoolRDOInput = t
                

#--------------------------------------------------------------
# jobO selection for data from POOL
#--------------------------------------------------------------
from AthenaCommon.DetFlags  import DetFlags
import AthenaPoolCnvSvc.ReadAthenaPool
# ServiceMgr.EventSelector.SkipEvents = 2366
# ServiceMgr.EventSelector.SkipEvents = 284
if DetFlags.readRDOPool.ID_on():
    include ( "TrkEventAthenaPool/TrkEventAthenaPool_joboptions.py" )
if rec.doTruth():
    include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )
        
#--------------------------------------------------------------
# Set output level threshold (ALL, VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL) 
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel  = INFO
ServiceMgr.MessageSvc.defaultLimit = 999999

#--------------------------------------------------------------
# (re-)configure the iPatRec algorithms
#--------------------------------------------------------------
from iPatRecAlgs.iPatRecAlgsConf import iPatRec
iPatRec.maxSecondaryImpact           = 100.0*mm
iPatRec.minPt                        = 0.4*GeV
iPatRec.printLevel                   = 2   # 2 gives parameters of found tracks
iPatRec.TracksLocation               = "Tracks"

from iPatRecAlgs.iPatRecAlgsConf import iPatStatistics
iPatStatistics.minPt		     = iPatRec.minPt

#iPatTrackTruthAssociator.OutputLevel      = DEBUG


#--------------------------------------------------------------
# ... and tools
#--------------------------------------------------------------
from iPatTrackFitter.iPatTrackFitterConf import TrackFitter
#ToolSvc += TrackFitter(name = 'TrackFitter', OutputLevel  = DEBUG)
from TrkiPatFitter.TrkiPatFitterConf import Trk__iPatFitter
ToolSvc.iPatRecFitter.OutputLevel  = DEBUG

from iPatTrackFinder.iPatTrackFinderConf import TrackFinder
ToolSvc += TrackFinder(name = 'TrackFinder',
                       OutputLevel           = INFO,
                       searchForPrimaries    = True,
                       searchForSecondaries  = True,
                       searchForVertexRegion = True)

#ToolSvc.TrajectoryTool.parametrizeSolenoid = False

#==============================================================
# End of iPatRecExample job options file
#
###############################################################
 
