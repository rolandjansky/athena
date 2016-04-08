################################################################################
# Package: RecJiveXML
################################################################################

# Declare the package name:
atlas_subdir( RecJiveXML )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Tracking/TrkEvent/TrkParameters
                          graphics/JiveXML
                          PRIVATE
                          Calorimeter/CaloEvent
                          Control/Navigation
                          GaudiKernel
                          PhysicsAnalysis/JetTagging/JetTagEvent
                          Reconstruction/Jet/JetEvent
                          Reconstruction/MissingETEvent
                          Tracking/TrkEvent/TrkTrack )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( RecJiveXML
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps TrkParameters JiveXMLLib CaloEvent Navigation GaudiKernel JetTagEvent JetEvent MissingETEvent TrkTrack )

# Install files from the package:
atlas_install_headers( RecJiveXML )
atlas_install_joboptions( share/*.py )

