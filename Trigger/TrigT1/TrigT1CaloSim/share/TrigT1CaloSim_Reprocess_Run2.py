include('TrigT1CaloCalibConditions/L1CaloCalibConditions_jobOptions.py')

# Get the algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

svcMgr.ToolSvc += CfgMgr.LVL1__L1DatabaseOverrideForDataTool('L1DatabaseOverrideForDataTool')

topSequence += CfgMgr.LVL1__TransientDatabaseOverride(
    'TransientDatabaseOverride',
    OverrideTools = [ svcMgr.ToolSvc.L1DatabaseOverrideForDataTool ],
    InDeadChannelsFolder = '/TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels',
    InDisabledTowersFolder = '/TRIGGER/L1Calo/V1/Conditions/DisabledTowers',
    OutPprChanDefaultsKey = 'ModifiedPprChanDefaults',
    OutPprChanCalibKey = 'ModifiedPprChanCalib',
    OutDeadChannelsKey = 'ModifiedPpmDeadChannels',
    OutDisabledTowersKey = 'ModifiedDisabledTowers'
)

topSequence += CfgMgr.LVL1__Run2TriggerTowerMaker(
    'Run2TriggerTowerMaker',
    CellType=2, # Trigger Towers
    TriggerTowerLocation = 'ReprocessedTriggerTowers',
    ZeroSuppress = False,
    ChanDefaultsFolderKey = 'ModifiedPprChanDefaults',
    ChanCalibFolderKey = 'ModifiedPprChanCalib',
    DeadChannelsFolderKey = 'ModifiedPpmDeadChannels',
    DisabledTowersFolderKey = 'ModifiedDisabledTowers'
)
