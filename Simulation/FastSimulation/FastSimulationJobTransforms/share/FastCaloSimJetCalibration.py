try:
    from JetRec.JetRecFlags import jetFlags
    jetFlags.CalibrationDBSuffix="AtlfastII"
except:
    print "Could not set AtlfastII jet calibration"
