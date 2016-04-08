################################################################################
# Package: xAODJiveXML
################################################################################

# Declare the package name:
atlas_subdir( xAODJiveXML )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMissingET
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTau
                          GaudiKernel
                          graphics/JiveXML
                          PRIVATE
                          Calorimeter/CaloGeoHelpers
                          Event/xAOD/xAODBTagging
                          Event/xAOD/xAODTracking )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( xAODJiveXML
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps xAODCaloEvent xAODEgamma xAODJet xAODMissingET xAODMuon xAODTau GaudiKernel JiveXMLLib CaloGeoHelpers xAODBTagging xAODTracking )

# Install files from the package:
atlas_install_headers( xAODJiveXML )
atlas_install_joboptions( share/*.py )

