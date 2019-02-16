################################################################################
# Package: ISF_ParSimTools
################################################################################

# Declare the package name:
atlas_subdir( ISF_ParSimTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          InnerDetector/InDetConditions/BeamSpotConditionsData
                          Simulation/ISF/ISF_Core/ISF_Interfaces
                          Simulation/ISF/ISF_ParametricSim/ISF_ParSimInterfaces
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          PRIVATE
                          Simulation/ISF/ISF_Core/ISF_Event
                          Tools/PathResolver
                          Tracking/TrkEvent/TrkParameters )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Matrix )

# Component(s) in the package:
atlas_add_component( ISF_ParSimTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel xAODTracking GaudiKernel ISF_Interfaces TrkExInterfaces ISF_Event PathResolver TrkParameters )

# Install files from the package:
atlas_install_headers( ISF_ParSimTools )
atlas_install_runtime( Data/*.txt Data/*.root )

