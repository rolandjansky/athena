
# configure the looper killer
# this configures the MT-friendly version of the looperKiller

import G4UserActions.G4UserActionsConfig

lkTool=CfgGetter.getPublicToolClone('QballLooperKillerTool','G4UA::LooperKillerTool', PrintSteps=2, MaxSteps=2000000, VerboseLevel=0)
G4UserActionsConfig.addLooperKillerTool(lkTool)
# add default HIP killer
G4UserActionsConfig.addHIPKillerTool()



