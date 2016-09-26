################################################################################
# Package: TrkVertexTools
################################################################################

# Declare the package name:
atlas_subdir( TrkVertexTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          PRIVATE
                          Control/CxxUtils
                          Event/EventPrimitives
                          Generators/GeneratorObjects
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )

# Component(s) in the package:
atlas_add_component( TrkVertexTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps AthenaKernel xAODTracking GaudiKernel TrkVertexFitterInterfaces CxxUtils EventPrimitives GeneratorObjects TrkEventPrimitives VxVertex )

# Install files from the package:
atlas_install_headers( TrkVertexTools )

