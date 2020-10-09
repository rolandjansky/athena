# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags
# TODO: clean up flags, should only contain general settings but no alg config
# TODO : Add some exta levels?

def createITkConfigFlags():
  itkcf=AthConfigFlags()
  itkcf.addFlag("ITk.useLocalGeometry", False) #take geometry XML files from local instance rather than Detector Database, for development
  itkcf.addFlag("ITk.stripGeometryFilename", 'ITkStrip.gmx')
  return itkcf
