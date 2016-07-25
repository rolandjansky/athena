#------------------------------------------------------------------------------
import os

doTrigMatch = True
inputFiles = []

if 'inputDir' in dir():
    for f in os.listdir(inputDir):
        inputFiles += ['%s/%s' %(inputDir, f)]

elif os.path.isdir('input'):
    for f in os.listdir('input'):
        inputFiles += ['input/%s' %(f)]

else:
    print 'Missing input files - OK for pathena'

#------------------------------------------------------------------------------
def getMuonTriggers():

    if not doTrigMatch:
        return []

    trigs = ['HLT_mu10',
             'HLT_mu10_msonly',
             'HLT_mu14',
             'HLT_mu18',
             'HLT_mu20',
             'HLT_mu20_iloose_L1MU15',
             'HLT_mu20_L1MU15',
             'HLT_mu20_msonly',
             'HLT_mu22',
             'HLT_mu24',
             'HLT_mu24_iloose_L1MU15',
             'HLT_mu24_imedium',
             'HLT_mu24_L1MU15',
             'HLT_mu26',
             'HLT_mu26_imedium',
             'HLT_mu26_ivarmedium',
             'HLT_mu24_ivarmedium',
             'HLT_mu40',
             'HLT_mu4',
             'HLT_mu4_msonly',
             'HLT_mu50',
             'HLT_mu6',
             'HLT_mu6_msonly',
             ]

    return trigs

#------------------------------------------------------------------------------
def getMuonTriggersL1():

    if not doTrigMatch:
        return []

    trigs = ['L1_MU4',
             'L1_MU6',
             'L1_MU10',
             'L1_MU15',
             'L1_MU20',
             ]

    return trigs

#------------------------------------------------------------------------------
from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool

ServiceMgr.EventSelector.InputCollections = inputFiles
print ServiceMgr.EventSelector.InputCollections

if 'EvtMax' in dir():
    theApp.EvtMax = EvtMax
else:
    theApp.EvtMax = -1

#------------------------------------------------------------------------------
if not hasattr(ServiceMgr, 'AthenaEventLoopMg'):
    from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
    ServiceMgr += AthenaEventLoopMgr(EventPrintoutInterval = 100)

if 'dumpSG' in dir():
    StoreGateSvc = Service('StoreGateSvc')
    StoreGateSvc.Dump = dumpSG

#------------------------------------------------------------------------------
muon_sf  = CfgMgr.CP__MuonTriggerScaleFactors('muonTrigSFs', OutputLevel = INFO)

#muon_sf.filename = "/pc2012-data2/rustem/testarea/AtlasMuon/PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonEfficiencyCorrections/share/muon_trigger_eff_jun30.root"

ToolSvc += muon_sf

#------------------------------------------------------------------------------
def getTestMuon(input_name):

    alg = CfgMgr.Trig__TestTrigSF('%s_testTrigSF' %input_name)
    alg.inputContainerName = input_name
    alg.trigEff            = muon_sf
    alg.OutputLevel        = 0

    alg.outputStream   = 'out'

    return alg

#------------------------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
algSeq = AlgSequence()

algSeq += getTestMuon('Muons')

#------------------------------------------------------------------------------
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()

if 'outFile' not in dir():
    outFile = 'out.root'

ServiceMgr.THistSvc.Output += ["out DATAFILE='%s' OPT='RECREATE'" %outFile]
