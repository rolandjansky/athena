################################################################################
# Package: InDetSegmentDriftCircleAssValidation
################################################################################

# Declare the package name:
atlas_subdir( InDetSegmentDriftCircleAssValidation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkTruthData
                          PRIVATE
                          Control/StoreGate
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkTrack )

# External dependencies:
find_package( HepMC )
find_package( HepPDT )

# Component(s) in the package:
atlas_add_component( InDetSegmentDriftCircleAssValidation
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps GaudiKernel InDetPrepRawData TrkSegment TrkTruthData InDetRIO_OnTrack TrkPseudoMeasurementOnTrack TrkTrack )

# Install files from the package:
atlas_install_headers( InDetSegmentDriftCircleAssValidation )

