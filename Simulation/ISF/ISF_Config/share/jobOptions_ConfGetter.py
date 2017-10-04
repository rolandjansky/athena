"""
Run jobOptions (ConfGetter) for ISF
KG Tan, 17/06/2012
"""

#--------------------------------------------------------------
# For RTT - get command line options (e.g. athena -c 'Input="muons"' jobOptions_ConfGetter.py)
#--------------------------------------------------------------
options = locals()
# Inputs: muons, electrons, pions, photons, ttbar, minbias
options.setdefault('Input',                    'ttbar'                         )
# Simulators: MC12G4, MC12G4_IDOnly, FastOnly, ATLFASTII, ATLFASTIIF, MultiSimTest
#             G4GammaCones, G4GammaCones_FastCalo, FatrasGammaCones_FastCalo
options.setdefault('Simulator',                'ATLFASTII'                     )
options.setdefault('EvtMax',                   15                              )
options.setdefault('OverrideInputFiles',       []                              )
options.setdefault('ValidationMode',           False                           )
options.setdefault('GeometryVersion',          'ATLAS-R2-2015-03-01-00'        )
options.setdefault('truthStrategy',            'MC15aPlus'                     )
options.setdefault('DataRunNumber',            222525                          )
options.setdefault('ConditionsTag',            'OFLCOND-RUN12-SDR-19'          )

from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,\
        getService,getServiceClone,getAlgorithm,getAlgorithmClone

#--------------------------------------------------------------
# Job setup
#--------------------------------------------------------------

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax            = EvtMax
athenaCommonFlags.PoolEvgenInput.set_Off() # set this off by default - will be switched on if we specify a file later.
athenaCommonFlags.FilesInput.set_Off()
#athenaCommonFlags.PoolHitsOutput    = 'Hits.pool.root'

from ISF_Config.ISF_jobProperties import ISF_Flags
#ISF_Flags.RunVP1                    = True    # default: False
#ISF_Flags.DoPerfMonStats            = False   # default: True
#ISF_Flags.DumpMcEvent               = True    # default: False
#ISF_Flags.DumpStoreGate             = True    # default: False
#ISF_Flags.RunValgrind               = True    # default: False
ISF_Flags.ValidationMode            = ValidationMode    # default: False
#ISF_Flags.VertexPositionFromFile    = True    # default: False
ISF_Flags.DoMemoryMonitoring        = True     # default: False
ISF_Flags.DoTimeMonitoring          = True     # default: False
ISF_Flags.Input                     = Input          # default: 'ttbar'
ISF_Flags.Simulator                 = Simulator      # default: 'MultiSimTest'

# If this flag is set (to a non-default), then it ignores the ISF_Flags.Input flag
ISF_Flags.OverrideInputFiles        = OverrideInputFiles    # default: []
#ISF_Flags.OverrideInputFiles = [ '/afs/cern.ch/atlas/groups/fatras/fatras_input/CSC.005144.PythiaZee.EVGEN.root' ]
#ISF_Flags.OverrideInputFiles = [ '/afs/cern.ch/atlas/groups/fatras/fatras_input/singlepart_e_pt100_evgen_merged.root' ]
#ISF_Flags.OverrideInputFiles = [ '/afs/cern.ch/atlas/groups/fatras/fatras_input/mc10_7TeV.105001.pythia_minbias.evgen.e574_merged.root' ]
#ISF_Flags.OverrideInputFiles = [ '/afs/cern.ch/atlas/groups/fatras/fatras_input/singlepart_mu_pt20_evgen_merged.root' ]
#ISF_Flags.OverrideInputFiles = [ '/afs/cern.ch/atlas/groups/fatras/fatras_input/singlepart_pi_pt5_evgen_merged.root' ]

#from ISF_FatrasServices.FatrasTuning import FatrasTuningFlags
#FatrasTuningFlags.MomCutOffSec = 50.

from G4AtlasApps.SimFlags import simFlags
if DataRunNumber>0:
  simFlags.RunNumber=DataRunNumber
simFlags.ReleaseGeoModel = False    # Comment this out when not making making DCube plots
simFlags.SimLayout = GeometryVersion

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = simFlags.SimLayout.get_Value()
globalflags.ConditionsTag   = ConditionsTag
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.PixelBuildingOutputLevel = INFO
TrkDetFlags.SCT_BuildingOutputLevel  = INFO
TrkDetFlags.TRT_BuildingOutputLevel  = INFO
TrkDetFlags.InDetBuildingOutputLevel = INFO

from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.defaultLimit = 1000000
ServiceMgr.MessageSvc.Format       = "% F%60W%S%7W%R%T %0W%M"
#ServiceMgr.MessageSvc.OutputLevel  = VERBOSE

#--------------------------------------------------------------
# Do the rest of the configuration
#--------------------------------------------------------------
try:
    from ISF_Config import FlagSetters
    FlagSetters.configureFlagsBase()
## Check for any simulator-specific configuration
    configureFlags = getattr(FlagSetters, ISF_Flags.Simulator.configFlagsMethodName(), None)
    if configureFlags is not None:
        configureFlags()
except:
    ## Select detectors
    if 'DetFlags' not in dir():
        ## If you configure one det flag, you're responsible for configuring them all!
        from AthenaCommon.DetFlags import DetFlags
        DetFlags.all_setOn()
    DetFlags.LVL1_setOff() # LVL1 is not part of G4 sim
    DetFlags.Truth_setOn()
    DetFlags.Forward_setOff() # Forward dets are off by default

if 'truthStrategy' in dir():
    simFlags.TruthService     = 'ISF_'     + truthStrategy + 'TruthService'
    simFlags.TruthStrategy    = truthStrategy
    try:
        from BarcodeServices.BarcodeServicesConfig import barcodeOffsetForTruthStrategy
        simFlags.SimBarcodeOffset  = barcodeOffsetForTruthStrategy(truthStrategy)
    except RuntimeError:
        if 'MC12' in truthStrategy or 'MC15a' in truthStrategy:
            simFlags.SimBarcodeOffset  = 200000 #MC12 setting
        else:
            simFlags.SimBarcodeOffset  = 1000000 #MC15 setting
        atlasG4log.warning('Using unknown truth strategy '+str(truthStrategy)+' guessing that barcode offset is '+str(simFlags.SimBarcodeOffset))
    except ImportError:
        # Temporary back-compatibility
        if 'MC12' in truthStrategy or 'MC15a' in truthStrategy:
            simFlags.SimBarcodeOffset  = 200000 #MC12 setting
        else:
            simFlags.SimBarcodeOffset  = 1000000 #MC15 setting
else:
    simFlags.TruthService     = 'ISF_MC12TruthService'
    simFlags.TruthStrategy    = 'MC12'
    simFlags.SimBarcodeOffset  = 200000 #MC12 setting

include('ISF_Config/ISF_ConfigJobInclude.py')


# the particle broker THist stream
if ISF_Flags.ValidationMode:
  if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc("THistSvc")
  ServiceMgr.THistSvc.Output += ["ParticleBroker DATAFILE='ISFBroker.root' OPT='RECREATE'"]
  ServiceMgr.THistSvc.Output += ["ISFSimHit DATAFILE='ISFSimHit.root' OPT='RECREATE'"]
  ServiceMgr.THistSvc.Output += ["ISFFatras DATAFILE='ISFFatras.root' OPT='RECREATE'"]
  ServiceMgr.THistSvc.Output += ["ISFG4SimKernel DATAFILE='ISFG4SimKernel.root' OPT='RECREATE'"]

#from AthenaCommon.ConfigurationShelve import saveToAscii
#saveToAscii("config.txt")

