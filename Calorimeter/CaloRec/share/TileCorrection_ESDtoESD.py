# this is valid only for Aug-09 ESDtoESD reprocessing of 2008 data
if rec.readESD and rec.doESD:
    mlog = logging.getLogger('CaloRec_logger')
    mlog.info("settng 2-G noise and enabling TileCell correction")
    from CaloRec.CaloTopoClusterFlags import jobproperties
    jobproperties.CaloTopoClusterFlags.doTwoGaussianNoise = True
    from CaloRec.CaloRecFlags import jobproperties
    jobproperties.CaloRecFlags.doTileCellCorrection = True
