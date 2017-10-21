################################################################################
# Package: egammaInterfaces
################################################################################

# Declare the package name:
atlas_subdir( egammaInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTracking
                          Reconstruction/egamma/egammaRecEvent
                          GaudiKernel
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkParameters )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_dictionary( egammaInterfacesDict
                      egammaInterfaces/egammaInterfacesDict.h
                      egammaInterfaces/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODCaloEvent xAODEgamma xAODTracking GaudiKernel TrkEventPrimitives TrkNeutralParameters TrkParameters )

# Install files from the package:
atlas_install_headers( egammaInterfaces )

