try:
    from CaloClusterCorrection.CaloClusterCorrectionFlags import caloClusterCorrectionFlags
    caloClusterCorrectionFlags.CaloSwWhichCorrection='v1_atlfast'
except:
    print "Could not set AtlfastII egamma weights"
