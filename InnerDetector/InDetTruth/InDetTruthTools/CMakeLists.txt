################################################################################
# Package: InDetTruthTools
################################################################################

# Declare the package name:
atlas_subdir( InDetTruthTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthenaBaseComps
                          InnerDetector/InDetTruth/InDetTruthInterfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkTruthTracks/TrkTruthTrackInterfaces
                          PRIVATE
                          DetectorDescription/AtlasDetDescr
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRawEvent/InDetSimData
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkTruthData )

# External dependencies:
find_package( HepMC )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( InDetTruthTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPMC_LIBRARIES} AthenaKernel AthenaBaseComps TrkEventPrimitives AtlasDetDescr GaudiKernel InDetIdentifier InDetReadoutGeometry InDetSimData InDetPrepRawData TrkPrepRawData TrkTruthData )

# Install files from the package:
atlas_install_headers( InDetTruthTools )
atlas_install_joboptions( share/*.py )

