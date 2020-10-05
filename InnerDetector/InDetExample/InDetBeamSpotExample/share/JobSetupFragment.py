# $Id: JobSetupFragment.py 714548 2015-12-14 16:30:23Z amorley $
#
# Job option fragment for JobRunner templates to do common job setup.
#
# Written by Juerg Beringer in November 2009.
#
printfunc ("InDetBeamSpotExample INFO Using $Id: JobSetupFragment.py 714548 2015-12-14 16:30:23Z amorley $")

from AthenaCommon.GlobalFlags import globalflags

# Default values (please put a default for EACH jobConfig parameter
# so that the template can be used easily without JobRunner)
if not 'jobConfig' in dir():                         jobConfig = {}
if not 'inputfiles' in jobConfig:                    jobConfig['inputfiles'] = [ 'input.root' ]
if not 'bytestream' in jobConfig:                    jobConfig['bytestream'] = False
if not 'evtmax' in jobConfig:                        jobConfig['evtmax'] = -1
if not 'outputlevel' in jobConfig:                   jobConfig['outputlevel'] = WARNING
if not 'beamspotfile' in jobConfig:                  jobConfig['beamspotfile'] = ''
if not 'beamspotdb' in jobConfig:                    jobConfig['beamspotdb'] = ''
if not 'beamspottag' in jobConfig:                   jobConfig['beamspottag'] = ''
if not 'alignmentfile' in jobConfig:                 jobConfig['alignmentfile'] = ''
if not 'alignmentindettag' in jobConfig:             jobConfig['alignmentindettag'] = ''
if not 'alignmenttrttag' in jobConfig:               jobConfig['alignmenttrttag'] = ''
if not 'alignmenttrtdxtag' in jobConfig:             jobConfig['alignmenttrtdxtag']=''
if not 'UseFilledBCIDsOnly' in jobConfig:            jobConfig['UseFilledBCIDsOnly'] = True if globalflags.DataSource.get_Value() == 'data' else False
# Used only if present:
# - GlobalTag


# Common imports and definitions
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ServiceMgr
import AthenaCommon.CfgMgr as CfgMgr
from IOVDbSvc.CondDB import conddb
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


# Reduce verbosity
theApp.setOutputLevel = jobConfig['outputlevel']
ServiceMgr.MessageSvc.OutputLevel =  jobConfig['outputlevel']
#ServiceMgr.MessageSvc.Format      = "% F%40W%S%7W%R%T %0W%M"
eventLoopMgr = getattr( CfgMgr, theApp.EventLoop )()
eventLoopMgr.OutputLevel = jobConfig['outputlevel']
ServiceMgr += eventLoopMgr


# Input files
if not jobConfig['bytestream']:
    import AthenaPoolCnvSvc.ReadAthenaPool
    ServiceMgr.PoolSvc.AttemptCatalogPatch=True
    ServiceMgr.EventSelector.InputCollections = jobConfig['inputfiles']
else:
    ServiceMgr.EventSelector.Input = jobConfig['inputfiles']


# Number of events
theApp.EvtMax = jobConfig['evtmax']
#ServiceMgr.EventSelector.SkipEvents = 2
#ServiceMgr.StoreGateSvc.Dump = True


# Event counter
#from GaudiSequencer.GaudiSequencerConf import AthEventCounter
topSequence += CfgMgr.AthEventCounter(Frequency = 1000, OutputLevel = INFO)


# IOVDbSvc configuration (GlobalTag etc)
#from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
ServiceMgr += CfgMgr.IOVDbSvc()
if 'GlobalTag' in jobConfig:
    ServiceMgr.IOVDbSvc.GlobalTag = jobConfig['GlobalTag']

# If necessary to override run number
# ServiceMgr.IOVDbSvc.forceRunNumber = 52280


# Configure beam spot service
include("InDetBeamSpotService/BeamCondSvc.py")
if jobConfig['beamspotfile']:
    db = jobConfig['beamspotdb'] if jobConfig['beamspotdb'] else 'BEAMSPOT'
    tag = jobConfig['beamspottag'] if jobConfig['beamspottag'] else 'nominal'
    conddb.blockFolder("/Indet/Beampos")
    conddb.addFolderWithTag("", "<dbConnection>sqlite://X;schema=%s;dbname=%s</dbConnection> /Indet/Beampos" % (jobConfig['beamspotfile'],db),
                            tag, force=True)
else:
    if jobConfig['beamspottag']:
        conddb.addOverride("/Indet/Beampos", jobConfig['beamspottag'])


# Alignment
if jobConfig['alignmentfile']:
    printfunc ('Loading alignment constants from file',jobConfig['alignmentfile'])
    include ("DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )
    conddb.blockFolder("/Indet/Align")
    conddb.blockFolder("/TRT/Align")
    ServiceMgr += CfgMgr.CondProxyProvider()
    ServiceMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
    ServiceMgr.CondProxyProvider.InputCollections = [ jobConfig['alignmentfile'] ]
    ServiceMgr.CondProxyProvider.OutputLevel = min(INFO,jobConfig['outputlevel'])
    printfunc (ServiceMgr.CondProxyProvider)
    #from IOVSvc.IOVSvcConf import IOVSvc
    ServiceMgr += CfgMgr.IOVSvc()
    ServiceMgr.IOVSvc.preLoadData = True

if jobConfig['alignmentindettag']:
    conddb.addOverride("/Indet/Align", jobConfig['alignmentindettag'])
if jobConfig['alignmenttrttag']:
    conddb.addOverride("/TRT/Align",   jobConfig['alignmenttrttag'])
if jobConfig['alignmenttrtdxtag']:
    conddb.addFolder('TRT_OFL','/TRT/Calib/DX')
    conddb.addOverride("/TRT/Calib/DX", jobConfig['alignmenttrtdxtag'])

#if jobConfig['UseFilledBCIDsOnly']:
#    # Setup trigger config service (needed for Bunch Crossing Tool)
#    from TrigConfigSvc.TrigConfigSvcConfig import SetupTrigConfigSvc
#    trigcfg = SetupTrigConfigSvc()
#    trigcfg.SetStates("ds")
#    trigcfg.InitialiseSvc()
