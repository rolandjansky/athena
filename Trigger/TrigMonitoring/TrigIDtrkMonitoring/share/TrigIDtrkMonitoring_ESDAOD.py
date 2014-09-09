# Run TrigIDtrkMonitoring standalone on ESDs or AODs.
#
# Author: S. Ask (Manchester U.)
#
# Note: From 2010, need soft link, ESD.pool.root, in
#       run directory to an up to date file.

##### Standard ESD Part ###################

# Use MC (True) or data (False) configuration.
ConfForMC = False

# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags #JWH

poolRDO=["/tmp/jhowarth/data11_7TeV.00189421.physics_EnhancedBias.recon.ESD.r3356_r3369_tid736024_00/ESD.736024._000498.pool.root.1"]

athenaCommonFlags.FilesInput = poolRDO #JWH

# Set DB by hand.

#from AthenaCommon.GlobalFlags import GlobalFlags
#GlobalFlags.DetGeo.set_atlas()
#if (ConfForMC):
#    GlobalFlags.DataSource.set_geant4()
#else:
#    GlobalFlags.DataSource.set_data()

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.DataSource.set_Value_and_Lock('data')

from IOVDbSvc.CondDB import conddb

#from AthenaMonitoring.DQMonFlags import DQMonFlags
#DQMonFlags.monManFileKey             = "GLOBAL"
#DQMonFlags.monManManualRunLBSetup    = False
#DQMonFlags.monManDataType            = "cosmics"
#DQMonFlags.monManEnvironment         = "tier0"


## Event selector
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.BackNavigation = True
ServiceMgr.EventSelector.InputCollections = poolRDO # JWH

# Particle Properties
from PartPropSvc.PartPropSvcConf import PartPropSvc

# the POOL converters
include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py" )
include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py" )
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

# ESD input file
# Need env: STAGE_SVCCLASS=atlcal
# Need symbolic link "ESD.pool.root" to a relevant file.
# ServiceMgr.EventSelector.InputCollections = [ \
# "/tmp/jhowarth/data10_7TeV.00167776.physics_Egamma.merge.ESD.r1774_p327_tid203268_00/ESD.203268._000765.pool.root.1"]
   # "castor:/castor/cern.ch/grid/atlas/tzero/prod1/perm/data10_7TeV/express_express/0152166/data10_7TeV.00152166.express_express.recon.ESD.f236/data10_7TeV.00152166.express_express.recon.ESD.f236._lb0254._0001.1"]

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

##### Trigger Decision Part #####################

## set up trigger decision tool
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt
tdt.OutputLevel = WARNING
# might be needed for TriggerConfigGetter...
from RecExConfig.RecFlags  import rec
rec.readRDO=False
rec.readESD=True
#rec.readAOD=True
#rec.doWriteAOD=False
#rec.doWriteESD=False
# To read files with trigger config stored as in-file meta-data, i.e. 13.0.40
# and above: ds. To read AOD produced with 13.0.30 you need to change ds to aod:
#from TriggerJobOpts.TriggerFlags import TriggerFlags
#TriggerFlags.configurationSourceList = ['ds']
# set up trigger config service
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg =  TriggerConfigGetter( "ReadPool" )

##### TrigIDtrkMonitoring Part #####################

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "HLTMonManager")
HLTMonManager = topSequence.HLTMonManager


from TrigIDtrkMonitoring.TrigIDtrkMonitoringConfig import TrigIDtrkMonitoringTool
montool = TrigIDtrkMonitoringTool()
HLTMonManager.AthenaMonTools += montool

###### Run Zee part if calo is present #############

if rec.doCalo:

    ##########################################################################################################
    #  Configure the extrapolator
    ##########################################################################################################
    from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
    theAtlasExtrapolator=AtlasExtrapolator(name = 'egammaExtrapolator')
    theAtlasExtrapolator.DoCaloDynamic = False # this turns off dynamic calculation of eloss in calorimeters
    # all left to MaterialEffects/EnergyLossUpdators

    from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator as MaterialEffectsUpdator
    AtlasMaterialEffectsUpdator = MaterialEffectsUpdator(name = 'AtlasMaterialEffectsUpdator')
    ToolSvc += AtlasMaterialEffectsUpdator #default material effects updator
    NoElossMaterialEffectsUpdator = MaterialEffectsUpdator(name = 'NoElossMaterialEffectsUpdator')
    NoElossMaterialEffectsUpdator.EnergyLoss = False
    ToolSvc += NoElossMaterialEffectsUpdator

    # setup MaterialEffectsUpdator arrays
    MyUpdators = []
    #    MyUpdators += [AtlasMaterialEffectsUpdator] # for ID
    MyUpdators += [NoElossMaterialEffectsUpdator] # for ID
    MyUpdators += [NoElossMaterialEffectsUpdator] # for Calo
    MyUpdators += [NoElossMaterialEffectsUpdator] # for muon
    # MyUpdators += [NoElossMaterialEffectsUpdator] # for muon

    MySubUpdators = []
    MySubUpdators += [NoElossMaterialEffectsUpdator.name()] # for ID
    #    MySubUpdators += [AtlasMaterialEffectsUpdator.name()] # for ID
    MySubUpdators += [NoElossMaterialEffectsUpdator.name()] # for Calo
    MySubUpdators += [NoElossMaterialEffectsUpdator.name()] # for muon

    theAtlasExtrapolator.MaterialEffectsUpdators = MyUpdators
    theAtlasExtrapolator.SubMEUpdators = MySubUpdators
    ToolSvc+=theAtlasExtrapolator

    # add tool ExtrapolateTrackToCalo
    from TrackToCalo.TrackToCaloConf import ExtrapolateToCaloTool
    exToCalo = ExtrapolateToCaloTool(name         = "exToCalo", Extrapolator = theAtlasExtrapolator)
    ToolSvc += exToCalo

    ##### Add the Zee Monitoring to the manager ########

    #from TrigIDtrkMonitoring.TrigIDtrkMonitoringConfig import TrigIDZeeMonitoringTool
    #zeetool = TrigIDZeeMonitoringTool()
    #HLTMonManager.AthenaMonTools += zeetool

    
##### Histogram File Part #################

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["AANT DATAFILE='TrigIDtrkMonitoring_ESDAOD.root' OPT='RECREATE'"]
HLTMonManager.FileKey = "AANT"

#from AthenaMonitoring.DQMonFlags import DQMonFlags
#DQMonFlags.monManFileKey             = "GLOBAL"
#DQMonFlags.monManManualRunLBSetup    = False
#DQMonFlags.monManDataType            = "cosmics"
#DQMonFlags.monManEnvironment         = "tier0"

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = INFO
# Number of Events to process
theApp.EvtMax = 1000
#-1

#--------------------------------------------------------------
# Dump StoreGate (for tests)
#--------------------------------------------------------------
#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = True  #true will dump data store contents
