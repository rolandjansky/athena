# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("TrkG4UserActions.TrkG4UserActionsConfig.getMaterialStepRecorder",  "MaterialStepRecorder"  )
addTool("TrkG4UserActions.TrkG4UserActionsConfig.getPositionMomentumWriter","PosMomWriter"          )
addTool("TrkG4UserActions.TrkG4UserActionsConfig.getEnergyLossRecorder"  ,  "EnergyLossRecorder"    )
addTool("TrkG4UserActions.TrkG4UserActionsConfig.getGeantFollowerHelper",   "GeantFollowerHelper"   )
addTool("TrkG4UserActions.TrkG4UserActionsConfig.getGeantFollower"       ,  "GeantFollower"         )
addTool("TrkG4UserActions.TrkG4UserActionsConfig.getGeantFollowerMSHelper", "GeantFollowerMSHelper" )
addTool("TrkG4UserActions.TrkG4UserActionsConfig.getGeantFollowerMS"     ,  "GeantFollowerMS"       )

