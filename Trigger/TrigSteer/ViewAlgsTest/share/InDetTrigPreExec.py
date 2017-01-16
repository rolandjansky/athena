#Early failure warning
from GaudiHive.GaudiHiveConf import ForwardSchedulerSvc
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr += ForwardSchedulerSvc()
svcMgr.ForwardSchedulerSvc.CheckDependencies = True
## get a handle on the ServiceManager


## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from SGComps.SGCompsConf import SGInputLoader
topSequence += SGInputLoader( OutputLevel=INFO, ShowEventDump=False )
topSequence.SGInputLoader.Load = [ ('PixelRDO_Container','PixelRDOs'),
                                   ('TRT_RDO_Container','TRT_RDOs'),
                                   ('SCT_RDO_Container','SCT_RDOs'),
#                                   ('Trk::SegmentCollection','TRTSegments'),
                                 ]
#Turn off some ID stuff
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTRTPhaseCalculation = False
InDetFlags.doTruth = False
InDetFlags.doCaloSeededBrem = False
InDetFlags.doCaloSeededTRTSegments = False

from TrigConfigSvc.TrigConfigSvcConf import TrigConf__LVL1ConfigSvc
l1svc = TrigConf__LVL1ConfigSvc("LVL1ConfigSvc")
l1svc.XMLMenuFile = "LVL1config_Physics_pp_v5.xml"
svcMgr += l1svc

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them

def setOutput(obj, prop, name):
    setattr(obj, prop, name)
    setattr(obj, prop+"Aux", name+"Aux.")

def getOutput(obj, prop):
    try:
        # print "getOutput getattr"
        return getattr(obj, prop)
    except:
        # print "getOutput getDefaultProperty ", obj.getDefaultProperty(prop)
        return obj.getDefaultProperty(prop)
    # print "Can not read pro"
    raise "Error in reading property " + prop + " from " + obj
 
def genMenuAlg(name, inputHypos, inputChains):
    assert inputHypos != None, 'Alg to take hypo putput from is missing'
    assert inputChains != None, 'Alg to take chain decisions from is missing'
    print inputChains, inputHypos
    from ViewAlgs.ViewAlgsConf import MenuAlg
    menuAlg = MenuAlg(name)
    menuAlg.HypoDecisions = getOutput(inputHypos, "OutputDecisions")
    menuAlg.InputChainDecisions = getOutput(inputChains,"OutputChainDecisions")
    setOutput(menuAlg, "OutputDecisions", name+"RoIs")
    setOutput(menuAlg, "OutputChainDecisions", name+"Chains")
    return menuAlg



#Run Fake RoI
from L1Decoder.L1DecoderConf import FakeRoI
fakeRoI = FakeRoI("fakeRoI")
fakeRoI.InputFilename="caloRoIData.dat"
setOutput(fakeRoI, "OutputDecisions", "CaloRoIs")
fakeRoI.OutputLevel=DEBUG
topSequence += fakeRoI

theApp.EvtMax = 10
from TrigFastTrackFinder.TrigFastTrackFinderMT_Config import TrigFastTrackFinderMT_eGamma
theFTFMT = TrigFastTrackFinderMT_eGamma()

topSequence += theFTFMT
print theFTFMT
svcMgr.DetectorStore.Dump=True



theApp.EvtMax = 3
MessageSvc.debugLimit=1000000
