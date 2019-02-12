################################################################################
# Package: TrigFTKTrackConverter
################################################################################

# Declare the package name:
atlas_subdir( TrigFTKTrackConverter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetCondTools
                          InnerDetector/InDetConditions/PixelConditionsData
                          InnerDetector/InDetConditions/PixelConditionsServices
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkFitter/TrkFitterUtils
                          Trigger/TrigFTK/TrigFTKSim
                          Trigger/TrigTools/TrigFTKToolInterfaces
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/GeoPrimitives
                          Generators/GeneratorObjects
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTruthData )

# External dependencies:
find_package( Eigen )
find_package( HepMC )

# Component(s) in the package:
atlas_add_component( TrigFTKTrackConverter
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps GaudiKernel TrkFitterInterfaces TrkFitterUtils TrigFTKSimLib StoreGateLib SGtests AtlasDetDescr GeoPrimitives GeneratorObjects InDetIdentifier InDetReadoutGeometry InDetRIO_OnTrack TrkEventPrimitives TrkParameters TrkRIO_OnTrack TrkTruthData PixelConditionsData)

# Install files from the package:
atlas_install_headers( TrigFTKTrackConverter )
atlas_install_python_modules( python/*.py )

