# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

def getNewConfigFlags():
    """Create new ConfigFlags from old-style jobproperties. Usage:

    from AthenaConfiguration.OldFlags2NewFlags import getNewConfigFlags
    ConfigFlags = getNewConfigFlags()
    """

    # Set up a logger object.
    from AthenaCommon.Logging import logging
    log = logging.getLogger('getNewConfigFlags')

    # Flags needed from this package.
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.Enums import BeamType

    # Import some old-style flags
    from AthenaCommon.DetFlags import DetFlags
    from AthenaCommon.AthenaCommonFlags import jobproperties
    import AthenaCommon.GlobalFlags      # noqa: F401
    import AthenaCommon.BeamFlags        # noqa: F401
    import AthenaCommon.ConcurrencyFlags # noqa: F401

    # Files and conditions
    if jobproperties.Global.InputFormat() == 'bytestream':
        ConfigFlags.Input.Files = ( jobproperties.AthenaCommonFlags.FilesInput() or
                                    jobproperties.AthenaCommonFlags.BSRDOInput() )
    elif jobproperties.Global.InputFormat() == 'pool':
        ConfigFlags.Input.Files = ( jobproperties.AthenaCommonFlags.FilesInput() or
                                    jobproperties.AthenaCommonFlags.PoolHitsInput() )

    ConfigFlags.IOVDb.GlobalTag = jobproperties.Global.ConditionsTag()
    ConfigFlags.Beam.Type = BeamType(jobproperties.Beam.beamType())
    ConfigFlags.Beam.BunchSpacing = jobproperties.Beam.bunchSpacing()
    try:
        from AthenaMonitoring.DQMonFlags import DQMonFlags
        ConfigFlags.Output.HISTFileName = DQMonFlags.histogramFile()
    except ImportError:
        log.info('AthenaMonitoring not available, "ConfigFlags.Output.HISTFileName" not set')
        pass
    # Geometry - General
    ConfigFlags.GeoModel.AtlasVersion = jobproperties.Global.DetDescrVersion()
    try:
        from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags
        ConfigFlags.GeoModel.Align.Dynamic = InDetGeometryFlags.useDynamicAlignFolders()
    except ImportError:
        log.info('AtlasGeoModel not available, "ConfigFlags.GeoModel.Align.Dynamic" not set')
        pass
    # Environment
    ConfigFlags.Common.isOnline = jobproperties.AthenaCommonFlags.isOnline()

    # Concurrency
    ConfigFlags.Concurrency.NumProcs = jobproperties.ConcurrencyFlags.NumProcs()
    ConfigFlags.Concurrency.NumThreads = jobproperties.ConcurrencyFlags.NumThreads()

    # Let's build a map whose key is new flagname, and whose value is old flagname.
    geom_flag_map = {}
    # Geometry - InnerDetector
    geom_flag_map.update({ 'Bpipe':'bpipe', 'BCM':'BCM', 'DBM':'DBM', 'Pixel':'pixel', 'SCT':'SCT', 'TRT':'TRT'})

    # Geometry - Upgrade Phase-2 - TODO
    # geom_flag_map.update({ 'BCMPrime':'', 'ITkPixel':'', 'ITkStrip':'', 'HGTD':''})

    # Geometry - Calo
    geom_flag_map.update({ 'LAr':'LAr', 'Tile':'Tile'})

    #Geometry - Muon
    geom_flag_map.update({ 'CSC':'CSC', 'MDT':'MDT', 'RPC':'RPC', 'TGC':'TGC'})
    geom_flag_map.update({ 'MM':'MM', 'sTGC':'sTGC'})

    # Geometry - Forward
    geom_flag_map.update({'Lucid':'Lucid', 'ZDC':'ZDC', 'ALFA':'ALFA', 'AFP':'AFP'})

    # Now set Geometry i.e. do equivalent of :
    # ConfigFlags.Detector.GeometryBpipe = DetFlags.geometry.bpipe_on()
    ConfigFlags._loadDynaFlags('Detector')
    for flag in geom_flag_map:
        ConfigFlags._set('Detector.Geometry'+flag, getattr(DetFlags.detdescr,geom_flag_map[flag]+'_on')())

    # Apparently we have detdescr flags and MuonGeometryFlags and they don't agree. FIXME.
    try:
        from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
        ConfigFlags.Detector.GeometrysTGC = MuonGeometryFlags.hasSTGC()
        ConfigFlags.Detector.GeometryMM = MuonGeometryFlags.hasMM()
        ConfigFlags.Detector.GeometryCSC = MuonGeometryFlags.hasCSC()
    except ImportError:
        log.info('AtlasGeoModel not available, "ConfigFlags.Detector.GeometrysTGC", "ConfigFlags.Detector.GeometryMM" and "ConfigFlags.Detector.GeometryCSC" not set')
        pass

    # Now setup Enable flags:
    reco_flag_map = { 'BCM':'BCM', 'Pixel':'pixel', 'SCT':'SCT', 'TRT':'TRT'}
    for flag in reco_flag_map:
        ConfigFlags._set('Detector.Enable'+flag, getattr(DetFlags.haveRIO,reco_flag_map[flag]+'_on')() )

    # miscellaneous settings
    try:
        from InDetRecExample.InDetJobProperties import InDetFlags
        InDetFlags.init()
        ConfigFlags.InDet.Tracking.doTIDE_Ambi = InDetFlags.doTIDE_Ambi()
        ConfigFlags.InDet.useDCS = InDetFlags.useDCS()
        ConfigFlags.InDet.PriVertex.doVertexFinding = InDetFlags.doVertexFinding()
    except ImportError:
        log.info('InDetRecExample not available, "ConfigFlags.InDet.Tracking.doTIDE_Ambi", "ConfigFlags.InDet.useDCS" and "ConfigFlags.InDet.PriVertex.doVertexFinding" not set')
        pass


    # LAr Flags
    try:
        from LArConditionsCommon.LArCondFlags import larCondFlags
        ConfigFlags.LAr.OFCShapeFolder = larCondFlags.OFCShapeFolder()
    except ImportError:
        log.info('LArConditionsCommon not available, "ConfigFlags.LAr.OFCShapeFolder" not set')
        pass
    try:
        from LArROD.LArRODFlags import larRODFlags
        ConfigFlags.LAr.ROD.DoOFCPileupOptimization=larRODFlags.doOFCPileupOptimization()
        ConfigFlags.LAr.ROD.NumberOfCollisions=larRODFlags.NumberOfCollisions()
        ConfigFlags.LAr.ROD.nSamples=larRODFlags.nSamples()
        ConfigFlags.LAr.ROD.FirstSample=larRODFlags.firstSample()
        ConfigFlags.LAr.ROD.UseHighestGainAutoCorr=larRODFlags.useHighestGainAutoCorr()
        ConfigFlags.LAr.ROD.DoOFCMixedOptimization=larRODFlags.doOFCMixedOptimization()
        ConfigFlags.LAr.ROD.UseDelta=larRODFlags.UseDelta()
        ConfigFlags.LAr.ROD.forceIter=larRODFlags.forceIter()
    except ImportError:
        log.info('LArROD not available, "ConfigFlags.LAr.ROD" flags not set')
        pass

    # Muon reco flags
    try:
        from RecExConfig.RecAlgsFlags import recAlgs
        ConfigFlags.MuonCombined.doMuGirl = recAlgs.doMuGirl()
    except ImportError:
        log.info('RecExConfig not available, "ConfigFlags.MuonCombined.doMuGirl" not set')
        pass
    try:
        from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
        ConfigFlags.MuonCombined.doMuGirlLowBeta = muonCombinedRecFlags.doMuGirlLowBeta()
    except ImportError:
        log.info('MuonCombinedRecExample not available, "ConfigFlags.MuonCombined.doMuGirlLowBeta" not set')
        pass
    try:
        from MuonRecExample.MuonRecFlags import muonRecFlags
        ConfigFlags.Muon.doMSVertex = muonRecFlags.doMSVertex()
    except ImportError:
        log.info('MuonRecExample not available, "ConfigFlags.Muon.doMSVertex" not set')
        pass

    # data overlay
    try:
        from AthenaCommon.GlobalFlags import globalflags
        from OverlayCommonAlgs.OverlayFlags import overlayFlags
        ConfigFlags.Overlay.DataOverlay = globalflags.isOverlay() and overlayFlags.isDataOverlay()
    except ImportError:
        log.info('OverlayCommonAlgs not available, "ConfigFlags.Overlay.DataOverlay" not set')
        pass

    try:
        from RecExConfig.RecFlags import rec
        if rec.doDPD():
            # flags for Physics Validation (ATLASRECTS-6636)
            ConfigFlags.BTagging.SaveSV1Probabilities = True
            ConfigFlags.BTagging.RunJetFitterNN = True
            ConfigFlags.BTagging.RunFlipTaggers = True
            pass
    except ImportError:
        log.info('RecExConfig not available, "ConfigFlags.BTagging.SaveSV1Probabilities", "ConfigFlags.BTagging.RunJetFitterNN" and "ConfigFlags.BTagging.RunFlipTaggers" not set')
        pass

    return ConfigFlags


if __name__=="__main__":
    ConfigFlags = getNewConfigFlags()
