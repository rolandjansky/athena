# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createHGTD_ConfigFlags():
  hgtdcf = AthConfigFlags()

  hgtdcf.addFlag("HGTD.trackingGeometry.passiveBarrelMatZbins", 100) # Number of z bins to be used for passive material layers
  hgtdcf.addFlag("HGTD.trackingGeometry.passiveBarrelMatPhiBins", 50) # Number of phi bins to be used for passive material layers
  hgtdcf.addFlag("HGTD.trackingGeometry.passiveEndcapMatRbins", 50) # Number of r bins to be used for passive material layers
  hgtdcf.addFlag("HGTD.trackingGeometry.passiveEndcapMatPhiBins", 50) # Number of phi bins to be used for passive material

  return hgtdcf
