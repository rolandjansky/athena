from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

if rec.doWriteAOD : 
    streamAOD=MSMgr.GetStream( 'StreamAOD' )  
    streamAOD.AddItem( "xAOD::TriggerTowerContainer#xAODTriggerTowers" )
    streamAOD.AddItem( "xAOD::TriggerTowerAuxContainer#xAODTriggerTowersAux." )
