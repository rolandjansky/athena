################################################################################
# Package: InDetJiveXML
################################################################################

# Declare the package name:
atlas_subdir( InDetJiveXML )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          InnerDetector/InDetRawEvent/InDetRawData
                          Tracking/TrkEvent/TrkSpacePoint
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkEvent/VxVertex
                          graphics/JiveXML
                          PRIVATE
                          Control/StoreGate
                          Control/AthContainers
                          DetectorDescription/IdDictDetDescr
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          InnerDetector/InDetConditions/InDetCondTools
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkPrepRawData )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( InDetJiveXML
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps StoreGateLib SGtests GaudiKernel InDetRawData TrkSpacePoint TrkTruthData VxVertex JiveXMLLib AthContainers IdDictDetDescr InDetIdentifier InDetReadoutGeometry InDetPrepRawData TrkEventPrimitives TrkPrepRawData )

# Install files from the package:
atlas_install_headers( InDetJiveXML )
atlas_install_joboptions( share/*.py )

