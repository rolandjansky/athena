#Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def createITkTrackingGeometryFlags():
    from AthenaConfiguration.AthConfigFlags import AthConfigFlags
    itkcf = AthConfigFlags()

    itkcf.addFlag("beampipeMatZbins", 35) # Number of Z bins to be used for beampipe material layer
    itkcf.addFlag("pixelBarrelMatZbins", 100) # Number of z bins to be used for pixel barrel material layer
    itkcf.addFlag("pixelBarrelMatPhiBins", 50) # Number of phi bins to be used for pixel barrel material layer
    itkcf.addFlag("pixelEndcapMatRbins", 50) # Number of r bins to be used for pixel endcap material layer
    itkcf.addFlag("pixelEndcapMatPhiBins", 50) # Number of phi bins to be used for pixel endcap material layer
    itkcf.addFlag("stripBarrelMatZbins", 100) # Number of z bins to be used for strip barrel material layer
    itkcf.addFlag("stripBarrelMatPhiBins", 50) # Number of phi bins to be used for strip barrel material layer
    itkcf.addFlag("stripEndcapMatRbins", 50) # Number of r bins to be used for strip endcap material layer
    itkcf.addFlag("stripEndcapMatPhiBins", 50) # Number of phi bins to be used for strip endcap material layer
    itkcf.addFlag("passiveBarrelMatZbins", 100) # Number of z bins to be used for passive material layers
    itkcf.addFlag("passiveBarrelMatPhiBins", 50) # Number of phi bins to be used for passive material layers
    itkcf.addFlag("passiveEndcapMatRbins", 50) # Number of r bins to be used for passive material layers
    itkcf.addFlag("passiveEndcapMatPhiBins", 50) # Number of phi bins to be used for passive material layers
    itkcf.addFlag("minimalRadialGapForVolumeSplit", 10.) # Radial gap to enable disc splitting
    itkcf.addFlag("loadLocalDbForMaterialMaps", False) # Enable reading of local database
    itkcf.addFlag("localDatabaseName", "AtlasLayerMaterial.db") # EDatabase geometry name
    itkcf.addFlag("materialTag", "AtlasLayerMat_v") # Material tag
    itkcf.addFlag("version", "22") # material version

    return itkcf
