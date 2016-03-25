from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

if rec.doWriteESD : 
    streamESD=MSMgr.GetStream( 'StreamESD' )  
    protectedInclude ("HIRecExample/HIRecOutputESDList_jobOptions.py")
    for item in HIESDItemList : streamESD.AddItem(item)

if rec.doWriteAOD : 
    streamAOD=MSMgr.GetStream( 'StreamAOD' )  
    protectedInclude ("HIRecExample/HIRecOutputAODList_jobOptions.py")
    for item in HIAODItemList : streamAOD.AddItem(item)




