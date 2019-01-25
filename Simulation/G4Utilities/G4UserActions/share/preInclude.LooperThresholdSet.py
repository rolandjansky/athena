#########################################################
#
# G4UserActions/preInclude.LooperThresholdSet.py
# @author Miha Muskinja
#
# Sets the looper threshold values in transportation processes
#
#########################################################

from G4AtlasApps.SimFlags import simFlags
simFlags.OptionalUserActionList.addAction('G4UA::LooperThresholdSetTool')
simFlags.UserActionConfig.addConfig('G4UA::LooperThresholdSetTool', 'WarningEnergy', 0)
simFlags.UserActionConfig.addConfig('G4UA::LooperThresholdSetTool', 'ImportantEnergy', 0)
simFlags.UserActionConfig.addConfig('G4UA::LooperThresholdSetTool', 'NumberOfTrials', 10)
