# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def getNewConfigFlags():
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    # Import some old-style flags
    from AthenaCommon.DetFlags import DetFlags
    from AthenaCommon.GlobalFlags import globalflags # noqa: F401
    from AthenaCommon.AthenaCommonFlags import jobproperties
    from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags

    # Files and conditions
    if jobproperties.Global.InputFormat() == 'bytestream':
        ConfigFlags.Input.Files = jobproperties.AthenaCommonFlags.BSRDOInput()
    elif jobproperties.Global.InputFormat() == 'pool':
        ConfigFlags.Input.Files = jobproperties.AthenaCommonFlags.FilesInput.get_Value()
    ConfigFlags.IOVDb.GlobalTag = jobproperties.Global.ConditionsTag()
    ConfigFlags.Beam.BunchSpacing = jobproperties.Beam.bunchSpacing

    # Geometry - General
    ConfigFlags.GeoModel.AtlasVersion = jobproperties.Global.DetDescrVersion()
    ConfigFlags.GeoModel.Align.Dynamic = InDetGeometryFlags.useDynamicAlignFolders()

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
    # TODO

    # Now set Geometry i.e. do equivalent of : 
    # ConfigFlags.Detector.GeometryBpipe = DetFlags.geometry.bpipe_on()
    ConfigFlags._loadDynaFlags('Detector')
    for flag in geom_flag_map:   
        ConfigFlags._set('Detector.Geometry'+flag, getattr(DetFlags.detdescr,geom_flag_map[flag]+'_on')()) 
    
    # Apparently we have detdescr flags and MuonGeometryFlags and they don't agree. FIXME.
    from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
    ConfigFlags.Detector.GeometrysTGC = MuonGeometryFlags.hasSTGC()
    ConfigFlags.Detector.GeometryMM = MuonGeometryFlags.hasMM()
    
    # Now setup Reco:
    reco_flag_map = { 'BCM':'BCM', 'IBL':'pixel', 'Pixel':'pixel', 'SCT':'SCT', 'TRT':'TRT'}
    for flag in reco_flag_map:   
        ConfigFlags._set('Detector.Reco'+flag, getattr(DetFlags.haveRIO,reco_flag_map[flag]+'_on')() )

    return ConfigFlags
