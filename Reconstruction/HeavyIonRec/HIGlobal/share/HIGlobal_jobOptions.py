# main switch
# default set in HIGlobalFlags.py
# it can be changed in your top jobOption

from HIRecExample.HIRecExampleFlags import jobproperties

if jobproperties.HIRecExampleFlags.doHIGlobal:            
    from HIGlobal.HIGlobalFlags import jobproperties
    
    if jobproperties.HIGlobalFlags.doHIGlobalVars:
        include( "HIGlobal/HIGlobalVars_jobOptions.py" )
    if jobproperties.HIGlobalFlags.doHIGlobalNSiCluster:
        include( "HIGlobal/HIGlobalNSiCluster_jobOptions.py" )
    if jobproperties.HIGlobalFlags.doHIPixelTracklet:
        include( "HIGlobal/HIGlobal_PixelTracklets_jobOptions.py" )

    # === TRT ===
    #from  RecExConfig.ObjKeyStore import  objKeyStore
    #objKeyStore.addStreamESD("HITRTInfo", "myHI_TRTInfoObject" )

