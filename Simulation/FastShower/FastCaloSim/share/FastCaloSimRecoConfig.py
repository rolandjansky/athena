try:
    from JetRec.JetRecFlags import jetFlags
    jetFlags.CalibrationDBSuffix="AtlfastII"
except:
    print "jetFlags.CalibrationDBSuffix not defined yet"
try:
    from CaloClusterCorrection.CaloClusterCorrectionFlags import caloClusterCorrectionFlags
    caloClusterCorrectionFlags.CaloSwWhichCorrection='v1_atlfast'
except:
    print "Could not set AtlfastII egamma weights"

doTrigger = False
OutputLevel=DEBUG

doMonitoring=False
doPerfMon = False
