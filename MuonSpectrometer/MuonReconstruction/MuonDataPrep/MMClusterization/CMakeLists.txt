################################################################################
# Package: MMClusterization
################################################################################

# Declare the package name:
atlas_subdir( MMClusterization )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          PRIVATE
                          Control/AthenaBaseComps
                          Event/EventPrimitives
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          TestPolicy )
# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MMClusterizationLib
                   src/*.cxx
                   PUBLIC_HEADERS MMClusterization
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES GaudiKernel MuonPrepRawData MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps EventPrimitives MuonReadoutGeometry )

atlas_add_component( MMClusterization
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel MuonPrepRawData AthenaBaseComps EventPrimitives MuonIdHelpersLib MMClusterizationLib )

