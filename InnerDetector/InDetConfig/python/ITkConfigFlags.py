# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags
# TODO : Add some exta levels?

def createITkConfigFlags():
  itkcf = AthConfigFlags()

  # take geometry XML files from local instance rather than Detector Database, for development
  itkcf.addFlag("ITk.useLocalGeometry", False)
  itkcf.addFlag("ITk.pixelGeometryFilename", "ITKLayouts/PixelGeoModelXml/ITkPixel.gmx")
  itkcf.addFlag("ITk.stripGeometryFilename", "ITKLayouts/Strip/ITkStrip.gmx")

  return itkcf
