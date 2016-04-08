################################################################################
# Package: InDetJiveXML
################################################################################

# Declare the package name:
atlas_subdir( InDetJiveXML )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkEvent/VxVertex
                          graphics/JiveXML
                          PRIVATE
                          Control/DataModel
                          Control/StoreGate
                          DetectorDescription/IdDictDetDescr
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkSpacePoint
                          Tracking/TrkEvent/TrkTruthData )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( InDetJiveXML
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel VxVertex JiveXMLLib DataModel StoreGateLib SGtests IdDictDetDescr InDetIdentifier InDetReadoutGeometry InDetRawData InDetPrepRawData TrkEventPrimitives TrkPrepRawData TrkSpacePoint TrkTruthData )

# Install files from the package:
atlas_install_headers( InDetJiveXML )
atlas_install_joboptions( share/*.py )

