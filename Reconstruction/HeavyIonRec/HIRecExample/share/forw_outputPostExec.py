from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

if rec.doWriteESD : 
    streamESD=MSMgr.GetStream( 'StreamESD' )  
    streamESD.AddItem( "xAOD::TriggerTowerContainer#ZdcTriggerTowers" )
    streamESD.AddItem( "xAOD::TriggerTowerAuxContainer#ZdcTriggerTowersAux." )
    streamESD.AddItem( "xAOD::ZdcModuleContainer#ZdcModules" )
    streamESD.AddItem( "xAOD::ZdcModuleAuxContainer#ZdcModulesAux." )
    streamESD.AddItem( "xAOD::ZdcModuleContainer#ZdcSums" )
    streamESD.AddItem( "xAOD::ZdcModuleAuxContainer#ZdcSumsAux." )
