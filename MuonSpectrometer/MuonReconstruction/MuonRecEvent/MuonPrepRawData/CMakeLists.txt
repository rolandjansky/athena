################################################################################
# Package: MuonPrepRawData
################################################################################

# Declare the package name:
atlas_subdir( MuonPrepRawData )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonPrepRawData
                   src/*.cxx
                   PUBLIC_HEADERS MuonPrepRawData
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers AthLinks AthenaKernel CxxUtils GeoPrimitives Identifier EventPrimitives MuonReadoutGeometry TrkSurfaces TrkPrepRawData EventContainers
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel )

atlas_add_dictionary( MuonPrepRawDataDict
                      MuonPrepRawData/MuonPrepDataDict.h
                      MuonPrepRawData/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks CxxUtils AthenaKernel GeoPrimitives Identifier EventPrimitives MuonReadoutGeometry TrkSurfaces TrkPrepRawData GaudiKernel MuonPrepRawData EventContainers)

