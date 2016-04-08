################################################################################
# Package: EventShapeTools
################################################################################

# Declare the package name:
atlas_subdir( EventShapeTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          GaudiKernel
                          Reconstruction/EventShapes/EventShapeInterface
                          Reconstruction/Jet/JetInterface
                          PRIVATE
                          Control/AthenaBaseComps
                          Event/xAOD/xAODEventShape )

# External dependencies:
find_package( FastJet )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( EventShapeToolsLib
                   Root/*.cxx
                   src/*.cxx
                   PUBLIC_HEADERS EventShapeTools
                   INCLUDE_DIRS ${FASTJET_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${FASTJET_LIBRARIES} AsgTools GaudiKernel EventShapeInterface JetInterface
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps xAODEventShape )

atlas_add_component( EventShapeTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${FASTJET_LIBRARIES} AsgTools GaudiKernel EventShapeInterface JetInterface AthenaBaseComps xAODEventShape EventShapeToolsLib )

atlas_add_dictionary( EventShapeToolsDict
                      EventShapeTools/EventShapeToolsDict.h
                      EventShapeTools/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${FASTJET_LIBRARIES} AsgTools GaudiKernel EventShapeInterface JetInterface AthenaBaseComps xAODEventShape EventShapeToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

