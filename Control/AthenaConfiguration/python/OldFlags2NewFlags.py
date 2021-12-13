# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def getNewConfigFlags():
    """Create new ConfigFlags from old-style jobproperties. Usage:

    from AthenaConfiguration.OldFlags2NewFlags import getNewConfigFlags
    ConfigFlags = getNewConfigFlags()
    """

    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    # Import some old-style flags
    from AthenaCommon.DetFlags import DetFlags
    from AthenaCommon.AthenaCommonFlags import jobproperties
    import AthenaCommon.GlobalFlags      # noqa: F401
    import AthenaCommon.BeamFlags        # noqa: F401
    import AthenaCommon.ConcurrencyFlags # noqa: F401
    from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags
    from AthenaMonitoring.DQMonFlags import DQMonFlags
    from RecExConfig.RecFlags import rec

    # Files and conditions
    if jobproperties.Global.InputFormat() == 'bytestream':
        ConfigFlags.Input.Files = ( jobproperties.AthenaCommonFlags.FilesInput() or
                                    jobproperties.AthenaCommonFlags.BSRDOInput() )
    elif jobproperties.Global.InputFormat() == 'pool':
        ConfigFlags.Input.Files = ( jobproperties.AthenaCommonFlags.FilesInput() or
                                    jobproperties.AthenaCommonFlags.PoolHitsInput() )

    ConfigFlags.IOVDb.GlobalTag = jobproperties.Global.ConditionsTag()
    ConfigFlags.Beam.BunchSpacing = jobproperties.Beam.bunchSpacing()
    ConfigFlags.Output.HISTFileName = DQMonFlags.histogramFile()
    # Geometry - General
    ConfigFlags.GeoModel.AtlasVersion = jobproperties.Global.DetDescrVersion()
    ConfigFlags.GeoModel.Align.Dynamic = InDetGeometryFlags.useDynamicAlignFolders()
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
    geom_flag_map.update({ 'MM':'Micromegas', 'sTGC':'sTGC'})

    # Geometry - Forward
    geom_flag_map.update({'Lucid':'Lucid', 'ZDC':'ZDC', 'ALFA':'ALFA', 'AFP':'AFP'})

    # Now set Geometry i.e. do equivalent of : 
    # ConfigFlags.Detector.GeometryBpipe = DetFlags.geometry.bpipe_on()
    ConfigFlags._loadDynaFlags('Detector')
    for flag in geom_flag_map:   
        ConfigFlags._set('Detector.Geometry'+flag, getattr(DetFlags.detdescr,geom_flag_map[flag]+'_on')()) 
    
    # Apparently we have detdescr flags and MuonGeometryFlags and they don't agree. FIXME.
    from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
    ConfigFlags.Detector.GeometrysTGC = MuonGeometryFlags.hasSTGC()
    ConfigFlags.Detector.GeometryMM = MuonGeometryFlags.hasMM()
    ConfigFlags.Detector.GeometryCSC = MuonGeometryFlags.hasCSC()

    # Now setup Enable flags:
    reco_flag_map = { 'BCM':'BCM', 'Pixel':'pixel', 'SCT':'SCT', 'TRT':'TRT'}
    for flag in reco_flag_map:   
        ConfigFlags._set('Detector.Enable'+flag, getattr(DetFlags.haveRIO,reco_flag_map[flag]+'_on')() )

    # miscellaneous settings
    from InDetRecExample.InDetJobProperties import InDetFlags
    ConfigFlags.InDet.doTIDE_Ambi = InDetFlags.doTIDE_Ambi()
    ConfigFlags.InDet.useDCS = InDetFlags.useDCS()

    if rec.doDPD():
        # flags for Physics Validation (ATLASRECTS-6636)
        ConfigFlags.BTagging.SaveSV1Probabilities = True
        ConfigFlags.BTagging.RunJetFitterNN = True
        ConfigFlags.BTagging.RunFlipTaggers = True

    return ConfigFlags


if __name__=="__main__":
    ConfigFlags = getNewConfigFlags()
