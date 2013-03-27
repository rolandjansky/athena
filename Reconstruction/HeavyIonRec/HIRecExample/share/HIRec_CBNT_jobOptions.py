from HIRecExample.HIRecExampleFlags import jobproperties

if jobproperties.HIRecExampleFlags.doHIGlobal:
    from HIGlobal.HIGlobalFlags import jobproperties
    if jobproperties.HIGlobalFlags.doHIGlobalVars :
        if jobproperties.HIGlobalFlags.doHIFlow :
            include( "HIGlobal/HIFlow_CBNT_jobOptions.py" )
        if jobproperties.HIGlobalFlags.doHIGlobalVars :
            include( "HIGlobal/HIGlobalVar_CBNT_jobOptions.py" )
