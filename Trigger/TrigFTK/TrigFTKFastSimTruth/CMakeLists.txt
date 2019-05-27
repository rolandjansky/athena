################################################################################
# Package: TrigFTKFastSimTruth
################################################################################

# Declare the package name:
atlas_subdir( TrigFTKFastSimTruth )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Trigger/TrigFTK/TrigFTKSim
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigTools/TrigFTKToolInterfaces
                          Trigger/TrigTools/TrigFTKTrackConverter
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkTruthTracks/TrkTruthTrackInterfaces
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          Trigger/TrigFTK/TrigFTK_RawData
                          PRIVATE
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          DetectorDescription/Identifier
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTruthData )

# External dependencies:
find_package( ROOT )
find_package( HepMC )

# Component(s) in the package:

atlas_add_library( TrigFTKFastSimTruthLib src/*.cxx
                   PUBLIC_HEADERS TrigFTKFastSimTruth
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps TrigFTKSim InDetPrepRawData TrigCaloEvent TrigFTKTrackConverter TrigFTKTrackConverter InDetIdentifier TrkToolInterfaces TrigFTK_RawData InDetReadoutGeometry
                   PRIVATE_LINK_LIBRARIES ${HEPMC_LIBRARIES} GaudiKernel Identifier TrkTrack TrkTruthData )

atlas_add_component( TrigFTKFastSimTruth
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps TrigFTKSim InDetPrepRawData TrigCaloEvent TrigFTKTrackConverter TrigFTKTrackConverter TrigFTK_RawData InDetIdentifier TrkToolInterfaces InDetReadoutGeometry  GaudiKernel Identifier TrkTrack TrkTruthData TrigFTKFastSimTruthLib )

# Install files from the package:
atlas_install_runtime( *.dat )
atlas_install_headers( TrigFTKFastSimTruth )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
