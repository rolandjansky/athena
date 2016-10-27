containerName= "SCTonlyVertex"
AnAODList = []
AnAODList+=['xAOD::VertexContainer#'+containerName]
AnAODList+=['xAOD::VertexAuxContainer#'+containerName+'Aux.-vxTrackAtVertex']

StreamAOD.ItemList += AnAODList



if False:
  from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
  SCTonlyVertexStream = MSMgr.NewPoolRootStream( "SCTonlyVertexStream", "SCTonlyVertex.pool.root" )
  SCTonlyVertexStream.AddItem( "TrigDec::TrigDecision#TrigDecision" )
  SCTonlyVertexStream.AddItem( 'xAOD::VertexContainer#'+containerName )
  SCTonlyVertexStream.AddItem( 'xAOD::VertexAuxContainer#'+containerName+'Aux.-vxTrackAtVertex' )
  SCTonlyVertexStream.AddItem( 'xAOD::EventInfo#EventInfo' ) 
  SCTonlyVertexStream.AddItem( 'xAOD::EventAuxInfo#EventInfoAux.' )


