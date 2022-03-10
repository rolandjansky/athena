# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createHGTD_ConfigFlags():
  hgtdcf = AthConfigFlags()

  hgtdcf.addFlag("HGTD.Geometry.useGeoModelXml", False)
  hgtdcf.addFlag("HGTD.Geometry.isLocal", False)
  hgtdcf.addFlag("HGTD.Geometry.Filename", "HGTD.gmx")
  hgtdcf.addFlag("HGTD.Geometry.ClobOutputName", "")

  hgtdcf.addFlag("HGTD.trackingGeometry.passiveBarrelMatZbins", 100) # Number of z bins to be used for passive material layers
  hgtdcf.addFlag("HGTD.trackingGeometry.passiveBarrelMatPhiBins", 50) # Number of phi bins to be used for passive material layers
  hgtdcf.addFlag("HGTD.trackingGeometry.passiveEndcapMatRbins", 50) # Number of r bins to be used for passive material layers
  hgtdcf.addFlag("HGTD.trackingGeometry.passiveEndcapMatPhiBins", 50) # Number of phi bins to be used for passive material

  return hgtdcf
